/*
 * EE30186.c
 *
 *	This is the file that manages the includes for starting and configuring the FPGA material for the EE30186 course
 *
 *  Created on: 22 Sep 2015
 *      Author: Alex Beasley
 */


#include "FPGASetup.h"
#include "FPGADemo.h"
#include <inttypes.h>
#include "alt_address_space.h"
#include "alt_bridge_manager.h"
#include "alt_clock_manager.h"
#include "alt_dma.h"
#include "alt_fpga_manager.h"
#include "socal/socal.h"
#include "alt_hps_detect.h"
#include "alt_printf.h"

/* enable semihosting with gcc by defining an __auto_semihosting symbol */
int __auto_semihosting;

void EE30186_Start()
{

    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    status = init_FPGA();

    if (status != ALT_E_SUCCESS)
    {
    	ALT_PRINTF("Initialisation of FPGA has failed");
    }

    return;

}

void EE30186_End()
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    status = close_FPGA();

    if (status != ALT_E_SUCCESS)
    {
    	ALT_PRINTF("Closing FPGA has failed");
    }

    return;
}
