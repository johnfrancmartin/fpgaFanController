/*
 * FPGADemo.c
 *
 *  Created on: 7 Sep 2015
 *      Author: Alex Beasley
 */

#include "FPGADemo.h"
#include <stdio.h>
#include <inttypes.h>
#include "alt_address_space.h"
#include "alt_bridge_manager.h"
#include "alt_clock_manager.h"
#include "alt_dma.h"
#include "alt_fpga_manager.h"
#include "socal/socal.h"
#include "system.h"

#include "alt_hps_detect.h"
#include "alt_printf.h"

ALT_STATUS_CODE socfpga_bridge_io(void)
{
    ALT_PRINTF("INFO: Demostrating IO across bridge ...\n");

    // Attempt to read the system ID peripheral
 //   uint32_t sysid = alt_read_word(ALT_LWFPGA_BASE + ALT_LWFPGA_SYSID_OFFSET);
  //  ALT_PRINTF("INFO: LWFPGA Slave => System ID Peripheral value = 0x%" PRIx32 ".\n", sysid);

    // Attempt to toggle the 4 LEDs
    const uint32_t bits = 4;
    ALT_PRINTF("INFO: Toggling LEDs ...\n");
    for (uint32_t i = 0; i < (1 << bits); ++i)
    {
        // Use Gray code ... for fun!
        // http://en.wikipedia.org/wiki/Gray_code
        uint32_t gray = (i >> 1) ^ i;

        alt_write_word(ALT_LWFPGA_BASE + ALT_LWFPGA_LED_OFFSET, gray);

        ALT_PRINTF("INFO: Gray code(i=0x%x) => 0x%x [", (unsigned int)i, (unsigned int)gray);

        for (uint32_t j = 0; j < bits; ++j)
        {
            ALT_PRINTF("%c", (gray & (1 << (bits - j - 1))) ? '1' : '0');
        }

        ALT_PRINTF("].\n");

        int n=0;
        while (n<500000){n++;}
    }

    // Reset the LEDs to all on
    alt_write_word(ALT_LWFPGA_BASE + ALT_LWFPGA_LED_OFFSET, 0);

    ALT_PRINTF("INFO: LEDs should have blinked.\n\n");

    return ALT_E_SUCCESS;
}



