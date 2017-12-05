/*
 * FPGADemo.h
 *
 *  Created on: 7 Sep 2015
 *      Author: Alex Beasley
 */

#ifndef EE30186_RESOURCE_FPGADEMO_H_
#define EE30186_RESOURCE_FPGADEMO_H_

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

//performs a gray code pattern on the LEDs of the DE1-SoC and returns gray code values to the console

ALT_STATUS_CODE socfpga_bridge_io(void);


#endif /* EE30186_RESOURCE_FPGADEMO_H_ */
