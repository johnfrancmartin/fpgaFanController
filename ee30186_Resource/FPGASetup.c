/*
 * FPGASetup.c
 *
 *  Created on: 7 Sep 2015
 *      Author: Alex Beasley
 */

#include "FPGASetup.h"
#include <stdio.h>
#include <inttypes.h>
#include "alt_address_space.h"
#include "alt_bridge_manager.h"
#include "alt_clock_manager.h"
#include "alt_dma.h"
#include "alt_fpga_manager.h"
#include "socal/socal.h"

#include "alt_hps_detect.h"
#include "alt_printf.h"

//global variable
ALT_DMA_CHANNEL_t channel;



ALT_STATUS_CODE socfpga_dma_setup(ALT_DMA_CHANNEL_t * allocated)
{
    ALT_PRINTF("INFO: Setup DMA System ...\n");

    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    if (status == ALT_E_SUCCESS)
    {
        // Configure everything as defaults.

        ALT_DMA_CFG_t dma_config;
        dma_config.manager_sec = ALT_DMA_SECURITY_DEFAULT;
        for (int i = 0; i < 8; ++i)
        {
            dma_config.irq_sec[i] = ALT_DMA_SECURITY_DEFAULT;
        }
        for (int i = 0; i < 32; ++i)
        {
            dma_config.periph_sec[i] = ALT_DMA_SECURITY_DEFAULT;
        }
        for (int i = 0; i < 4; ++i)
        {
            dma_config.periph_mux[i] = ALT_DMA_PERIPH_MUX_DEFAULT;
        }

        status = alt_dma_init(&dma_config);
        if (status != ALT_E_SUCCESS)
        {
            ALT_PRINTF("ERROR: alt_dma_init() failed.\n");
        }
    }

    // Allocate the DMA channel

    if (status == ALT_E_SUCCESS)
    {
        status = alt_dma_channel_alloc_any(allocated);
        if (status != ALT_E_SUCCESS)
        {
            ALT_PRINTF("ERROR: alt_dma_channel_alloc_any() failed.\n");
        }
        else
        {
            ALT_PRINTF("INFO: Channel %d allocated.\n", *allocated);
        }
    }

    // Verify channel state

    if (status == ALT_E_SUCCESS)
    {
        ALT_DMA_CHANNEL_STATE_t state;
        status = alt_dma_channel_state_get(*allocated, &state);
        if (status == ALT_E_SUCCESS)
        {
            if (state != ALT_DMA_CHANNEL_STATE_STOPPED)
            {
                ALT_PRINTF("ERROR: Bad initial channel state.\n");
                status = ALT_E_ERROR;
            }
        }
    }

    if (status == ALT_E_SUCCESS)
    {
        ALT_PRINTF("INFO: Setup of DMA successful.\n\n");
    }
    else
    {
        ALT_PRINTF("ERROR: Setup of DMA failed. [status = %" PRIi32 "].\n\n", status);
    }

    return status;
}

void socfpga_dma_cleanup(ALT_DMA_CHANNEL_t channel)
{
    ALT_PRINTF("INFO: Cleaning up DMA System ...\n");

    if (alt_dma_channel_free(channel) != ALT_E_SUCCESS)
    {
        ALT_PRINTF("WARN: alt_dma_channel_free() returned non-SUCCESS.\n");
    }

    if (alt_dma_uninit() != ALT_E_SUCCESS)
    {
        ALT_PRINTF("WARN: alt_dma_uninit() returned non-SUCCESS.\n");
    }
}

ALT_STATUS_CODE socfpga_fpga_setup_dma(const void * fpga_image, size_t fpga_image_size,
                                       ALT_DMA_CHANNEL_t dma_channel)
{
    ALT_PRINTF("INFO: Setup FPGA System ...\n");

    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    if (status == ALT_E_SUCCESS)
    {
        status = alt_fpga_init();
    }

    // Verify power is on
    if (status == ALT_E_SUCCESS)
    {
        if (alt_fpga_state_get() == ALT_FPGA_STATE_POWER_OFF)
        {
            ALT_PRINTF("ERROR: FPGA Monitor reports FPGA is powered off.\n");
            status = ALT_E_ERROR;
        }
    }

    // Take control of the FPGA CB
    if (status == ALT_E_SUCCESS)
    {
        status = alt_fpga_control_enable();
    }

    // Verify the MSELs are appropriate for the type of image we're using.
    if (status == ALT_E_SUCCESS)
    {
        // From experimentation, a compressed image is ~2.3 MiB an uncompressed is ~6.8 MiB.
        // Thus if it's smaller than 3 MiB, consider it compressed.
        bool compressed = (fpga_image_size < 3 * 1024 * 1024);

        ALT_FPGA_CFG_MODE_t mode = alt_fpga_cfg_mode_get();
        switch (mode)
        {
            // Compressed image modes
        case ALT_FPGA_CFG_MODE_PP32_FAST_AESOPT_DC:
        case ALT_FPGA_CFG_MODE_PP32_SLOW_AESOPT_DC:
        case ALT_FPGA_CFG_MODE_PP16_FAST_AESOPT_DC:
        case ALT_FPGA_CFG_MODE_PP16_SLOW_AESOPT_DC:
            if (compressed)
            {
                ALT_PRINTF("INFO: MSEL [%d] configured correctly for compressed FPGA image.\n", mode);
            }
            else
            {
                ALT_PRINTF("WARNING: MSEL [%d] likely configured incorrectly for uncompressed FPGA image.\n", mode);
                ALT_PRINTF("WARNING: Because of this, FPGA programming will probably fail.\n");
            }
            break;

            // Uncompressed image modes
        case ALT_FPGA_CFG_MODE_PP16_FAST_NOAES_NODC:
        case ALT_FPGA_CFG_MODE_PP16_FAST_AES_NODC:
        case ALT_FPGA_CFG_MODE_PP16_SLOW_NOAES_NODC:
        case ALT_FPGA_CFG_MODE_PP16_SLOW_AES_NODC:
        case ALT_FPGA_CFG_MODE_PP32_FAST_NOAES_NODC:
        case ALT_FPGA_CFG_MODE_PP32_FAST_AES_NODC:
        case ALT_FPGA_CFG_MODE_PP32_SLOW_NOAES_NODC:
        case ALT_FPGA_CFG_MODE_PP32_SLOW_AES_NODC:
            if (compressed)
            {
                ALT_PRINTF("WARNING: MSEL [%d] likely configured incorrectly for compressed FPGA image.\n", mode);
                ALT_PRINTF("WARNING: Because of this, FPGA programming will probably fail.\n");
            }
            else
            {
                ALT_PRINTF("INFO: MSEL [%d] configured correctly for uncompressed FPGA image.\n", mode);
            }
            break;

            // Invalid modes
        default:
            ALT_PRINTF("ERROR: Incompatible MSEL [%d] set. Cannot continue with FPGA programming.\n MSEL should be 010100 (MSEL[0] -> MSEL[4] \n", mode);
            status = ALT_E_ERROR;
            break;
        }
    }

    // Program the FPGA
    if (status == ALT_E_SUCCESS)
    {
        // Try the full configuration a few times.
        const uint32_t full_config_retry = 5;
        for (uint32_t i = 0; i < full_config_retry; ++i)
        {
            status = alt_fpga_configure_dma(fpga_image, fpga_image_size, dma_channel);
            if (status == ALT_E_SUCCESS)
            {
                break;
            }
        }
    }

    if (status == ALT_E_SUCCESS)
    {
        ALT_PRINTF("INFO: Setup of FPGA successful.\n\n");
    }
    else
    {
        ALT_PRINTF("ERROR: Setup of FPGA failed. [status = %" PRIi32 "].\n\n", status);
    }

    return status;
}

void socfpga_fpga_cleanup(void)
{
    ALT_PRINTF("INFO: Cleanup of FPGA ...\n");

    if (alt_fpga_control_disable() != ALT_E_SUCCESS)
    {
        ALT_PRINTF("WARN: alt_fpga_control_disable() returned non-SUCCESS.\n");
    }

    if (alt_fpga_uninit() != ALT_E_SUCCESS)
    {
        ALT_PRINTF("WARN: alt_fpga_uninit() returned non-SUCCESS.\n");
    }
}

ALT_STATUS_CODE socfpga_bridge_setup(ALT_BRIDGE_t bridge)
{
    ALT_PRINTF("INFO: Setup Bridge [%d] ...\n", bridge);

    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    if (status == ALT_E_SUCCESS)
    {
        status = alt_bridge_init(bridge, NULL, NULL);
    }

    if (status == ALT_E_SUCCESS)
    {
        status = alt_addr_space_remap(ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
                                      ALT_ADDR_SPACE_NONMPU_ZERO_AT_OCRAM,
                                      ALT_ADDR_SPACE_H2F_ACCESSIBLE,
                                      ALT_ADDR_SPACE_LWH2F_ACCESSIBLE);
    }

    if (status == ALT_E_SUCCESS)
    {
        ALT_PRINTF("INFO: Setup of Bridge [%d] successful.\n\n", bridge);
    }
    else
    {
        ALT_PRINTF("ERROR: Setup of Bridge [%d] failed. [status = %" PRIi32 "].\n\n", bridge, status);
    }

    return status;
}

void socfpga_bridge_cleanup(ALT_BRIDGE_t bridge)
{
    ALT_PRINTF("INFO: Cleanup of Bridge [%d] ...\n", bridge);

    if (alt_bridge_uninit(bridge, NULL, NULL) != ALT_E_SUCCESS)
    {
        ALT_PRINTF("WARN: alt_bridge_uninit() returned non-SUCCESS.\n");
    }
}

ALT_STATUS_CODE init_FPGA()
{
	   // This example only works on the Cyclone 5.
	    if (!alt_hps_detect_is_cyclone5())
	    {
	        printf("ERROR: FPGA example is specific to Cyclone 5 SoCFPGA.\n");
	        printf(" -> Please visit http://www.altera.com/ for more SoCFPGA examples.\n");
	        return 1;
	    }

	    ALT_STATUS_CODE status = ALT_E_SUCCESS;
	   // ALT_DMA_CHANNEL_t channel;

	    if (status == ALT_E_SUCCESS)
	    {
	        status = socfpga_dma_setup(&channel);
	    }

	    if (status == ALT_E_SUCCESS)
	    {
	        // This is the symbol name for the SOF file contents linked in.
	        extern char _binary_soc_system_dc_rbf_start;
	        extern char _binary_soc_system_dc_rbf_end;

	        // Use the above symbols to extract the FPGA image information.
	        const char *   fpga_image      = &_binary_soc_system_dc_rbf_start;
	        const uint32_t fpga_image_size = &_binary_soc_system_dc_rbf_end - &_binary_soc_system_dc_rbf_start;

	        // Trace the FPGA image information.
	        ALT_PRINTF("INFO: FPGA Image binary at %p.\n", fpga_image);
	        ALT_PRINTF("INFO: FPGA Image size is %" PRIu32 " bytes.\n", fpga_image_size);

	        status = socfpga_fpga_setup_dma(fpga_image, fpga_image_size, channel);
	    }

	    if (status == ALT_E_SUCCESS)
	    {
	        status = socfpga_bridge_setup(ALT_BRIDGE_LWH2F);
	    }

	return status;

}


ALT_STATUS_CODE close_FPGA()
{

    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    //ALT_DMA_CHANNEL_t channel;

    socfpga_bridge_cleanup(ALT_BRIDGE_LWH2F);
    socfpga_fpga_cleanup();
    socfpga_dma_cleanup(channel);
    ALT_PRINTF("\n");

    if (status == ALT_E_SUCCESS)
    {
        ALT_PRINTF("RESULT: Example completed successfully.\n");
        return 0;
    }
    else
    {
        ALT_PRINTF("RESULT: Some failures detected.\n");
        return 1;
    }

	return status;

}
