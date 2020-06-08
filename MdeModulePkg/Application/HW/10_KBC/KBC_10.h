/** @file
  KBC_10 H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/CpuIo2.h>

#define COMMAND_PORT       0x64
#define DATA_PORT          0x60
#define RESTART_COMMAND    0xfe

#define F1_NOT_PRESS       0
#define F1_PRESS           1 

//
// control LED (Keyboard CMD)
//
#define LedSwitch(num)  DataBuffer = 0xed;                \
                        CpuIo2->Io.Write (                \
                                      CpuIo2,             \
                                      EfiCpuIoWidthUint8, \
                                      DATA_PORT,          \
                                      1,                  \
                                      &DataBuffer         \
                                      );                  \
                        Delay(250);                       \
                        DataBuffer = num;                 \
                        CpuIo2->Io.Write (                \
                                      CpuIo2,             \
                                      EfiCpuIoWidthUint8, \
                                      DATA_PORT,          \
                                      1,                  \
                                      &DataBuffer         \
                                      );                  \
                        Delay(250)                      
                    
#define Led_1_On  LedSwitch(0x1)
#define Led_2_On  LedSwitch(0x2)
#define Led_3_On  LedSwitch(0x4)
#define Led_Off   LedSwitch(0x0)

BOOLEAN
Delay (
  UINT32 Count
  );

VOID
ShowLed(
  VOID
  );

VOID
RestartSystem (
  VOID
  );

VOID
ShowMainPage (
  VOID
  );

VOID
ShowLedPage (
  VOID
  );