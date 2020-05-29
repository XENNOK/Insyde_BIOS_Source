/** @file
  To init counter 0 and 1 for 8254 chip.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/IoLib.h>

#define LEGACY_8259_MASK_REGISTER_MASTER  0x21

#define REG_TIMER_CONTROL_WORD            0x43
#define REG_COUNTER_0_STATUS_BYTE_FORMAT  0x40
#define REG_COUNTER_1_STATUS_BYTE_FORMAT  0x41

#define SELECT_COUNTER_0  0
#define SELECT_COUNTER_1  1
#define SELECT_COUNTER_2  2
#define READ_BACK_COMMAND 3

#define COUNTER_LATCH_COMMAND                       0
#define READ_WRITE_LEAST_SIGNIFICANT_BYTE_ONLY      1
#define READ_WRITE_MOST_SIGNIFICANT_BYTE_ONLY       2
#define READ_WRITE_LEAST_SIGNIFICANT_BYTE_FIRST     3

#define BINARY_COUNTER_16_BIT             0
#define BINARY_CODED_DECIMAIL_COUNTER     1


#define CONTROL_WORD_FORMAT(Sc, Rw, Mode, Bcd)   (UINT8)((Bcd) | (Mode << 1) | (Rw << 4) | ( Sc << 6))

/*
Control Word Format:

  D7    D6    D5    D4    D3   D2   D1   D0
 --------------------------------------------
| SC1 | SC0 | RW1 | RW0 | M2 | M1 | M0 | BCD |
 --------------------------------------------

SC-Select Counter:
SC1 SC0
 0   0   Select Counter0
 0   1   Select Counter1
 1   0   Select Counter2
 1   1   Read-Back Command


RW-Read/Write
RW1 RW0
 0   0   Counter Latch Command
 0   1   Read/Write least significant byte Only
 1   0   Read/Write most significant byte Only
 1   1   Read/Write least significant byte first, then most significant byte.


M-Mode
M2  M1  M0
 0   0   0    Mode 0: Out signal on end of count
 0   0   1    Mode 1: H/W re-triggerable one-shot
 x   1   0    Mode 2: Rate Generator
 x   1   1    Mode 3: Square wave output
 1   0   0    Mode 4: S/W triggered stobe
 1   0   1    Mode 5: H/W triggered stobe


BCD
 0 Binary Counter 16-bits
 1 Binary Coded Decimal (BCD) Counter.

 */

/**
  To init counter 0 of 8254 chip for refresh cycle toggle.

  @param[in]

  @retval

**/
VOID
InitCounter0 (
  )
{
  INT8 Mask = 0;

  Mask = IoRead8 (LEGACY_8259_MASK_REGISTER_MASTER);
  Mask = Mask | 1;
  IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, Mask);
  IoWrite8 (REG_TIMER_CONTROL_WORD, 0x36);
  IoWrite8 (REG_COUNTER_0_STATUS_BYTE_FORMAT, 0);
  IoWrite8 (REG_COUNTER_0_STATUS_BYTE_FORMAT, 0);
  IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, Mask);
}

/**
  To init counter 1 of 8254 chip for refresh cycle toggle.

  @param[in]

  @retval

**/
VOID
InitCounter1 (
  )
{
  UINT8 OperationMode;
  UINT8 SelectCounter;

  OperationMode = PcdGet8 (Pcd8254Counter1OperatingMode);
  SelectCounter = CONTROL_WORD_FORMAT(SELECT_COUNTER_1, READ_WRITE_LEAST_SIGNIFICANT_BYTE_ONLY, OperationMode, BINARY_COUNTER_16_BIT);

  IoWrite8 (REG_TIMER_CONTROL_WORD, SelectCounter);
  IoWrite8 (REG_COUNTER_1_STATUS_BYTE_FORMAT, 0x12);
}


/**
  To init 8254 chip for counter 0 and 1.

  @param[in]

  @retval

**/
VOID
Init8254 (
  )
{
  InitCounter0 ();
  InitCounter1 ();
}
