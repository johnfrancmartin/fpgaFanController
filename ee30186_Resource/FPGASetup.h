/*
 * FPGASetup.h
 *
 *  Created on: 7 Sep 2015
 *      Author: Alex Beasley
 */

#ifndef EE30186_RESOURCE_FPGASETUP_H_
#define EE30186_RESOURCE_FPGASETUP_H_

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


//function to initialise the direct memory access
ALT_STATUS_CODE socfpga_dma_setup(ALT_DMA_CHANNEL_t * allocated);


//clean up function to be called after the dma is finished with
void socfpga_dma_cleanup(ALT_DMA_CHANNEL_t channel);


//function to initialise the FPGA direct memory access
ALT_STATUS_CODE socfpga_fpga_setup_dma(const void * fpga_image, size_t fpga_image_size,
                                       ALT_DMA_CHANNEL_t dma_channel);

//clean up the fpga dma
void socfpga_fpga_cleanup(void);

//initialise the bridge between the fpga and hps
ALT_STATUS_CODE socfpga_bridge_setup(ALT_BRIDGE_t bridge);

//clean up and close the bridge between the fpga and hps
void socfpga_bridge_cleanup(ALT_BRIDGE_t bridge);

//call all functions to establish dma and bridge and allow configuration and communication between hps and fpga
ALT_STATUS_CODE init_FPGA();

//close all links between hps and fpga
ALT_STATUS_CODE close_FPGA();

#endif /* EE30186_RESOURCE_FPGASETUP_H_ */
