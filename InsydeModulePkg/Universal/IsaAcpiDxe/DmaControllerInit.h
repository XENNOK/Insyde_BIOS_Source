/** @file

Declaration for DmaControllerInit.c

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DMA_CONTROLLER_INIT_H_
#define _DMA_CONTROLLER_INIT_H_

#include <Protocol/CpuIo.h>
#include <Library/UefiBootServicesTableLib.h>

#define DMA1_03_COMMAND_REG         0x08    //DMA channel 0-3 command register
#define DMA1_03_WRITE_REQUEST       0x09    //DMA channel 0-3 write request register
#define DMA1_03_MASK_REG            0x0A    //DMA channel 0-3 mask register
#define DMA1_03_MODE_REG            0x0B    //DMA channel 0-3 mode register
#define DMA1_03_CLEAR_POINTER       0x0C    //DMA channel 0-3 clear byte pointer flip-flop register any write clears LSB/MSB flip-flop of address and counter registers

#define DMA2_47_COMMAND_REG         0x0D0   //DMA channel 4-7 command register
#define DMA2_47_WRITE_REQUEST       0x0D2   //DMA channel 4-7 write request register
#define DMA2_47_MASK_REG            0x0D4   //DMA channel 4-7 write single mask register
#define DMA2_47_MODE_REG            0x0D6   //DMA channel 4-7 mode register
#define DMA2_47_CLEAR_POINTER       0x0D8   //DMA channel 4-7 clear byte pointer flip-flop

//Bitfields for DMA channel 4-7 mode define
//Bit 7-6	Transfer mode
#define DEMAND_MODE                 0x00   //Demand mode
#define SINGLE_MODE                 0x40   //Single mode
#define BLOCK_MODE                  0x80   //Block mode	
#define CASCADE_MODE                0xC0   //Cascade mode

//Bit 5	        Direction
#define ADDR_INCREMENT_SELECT       0x00   //Address increment select
#define ADDR_DECREMENT_SELECT       0x20   //Address decrement select

//Bit 4	        Auto initialisation enabled
#define AUTO_INITIALISATION_DISABLE 0x00   //Auto initialisation disable
#define AUTO_INITIALISATION_ENABLE  0x10   //Auto initialisation enabled

//Bit 3-2	Operation
#define VERIFY_OPERATION            0x00   //Verify operation
#define WRITE_TO_MEMORY             0x04   //Write to memory
#define READ_FROM_MEMORY            0x08   //Read from memory

//Bit 1-0	Channel number
#define CHANNEL_4_SELECT            0x00   //Channel 4 select
#define CHANNEL_5_SELECT            0x01   //Channel 5 select
#define CHANNEL_6_SELECT            0x02   //Channel 6 select	
#define CHANNEL_7_SELECT            0x03   //Channel 7 select

#endif
