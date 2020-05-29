/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SIO.h"

//
// Super Io Register
//
//..............................................................................
UINT8 SIO_Reg_List_UART [] = {
  0x02, 0x07, 0x0A, 0x0C, 0x12,
  0x13, 0x24, 0x25, 0x28, 0x29,
  0x2B, 0x2C,
};
UINT8 SIO_Reg_Save_UART [sizeof (SIO_Reg_List_UART)];


SIO_LDN_DEVICE  SioLdnList [] = {
//==============================================================================================
//  LDN |            Register Number                |   SIO RegTable   | Reg Table Save Area
//==============================================================================================
  { 0x0, sizeof (SIO_Reg_List_UART) / sizeof (UINT8), SIO_Reg_List_UART, SIO_Reg_Save_UART },
};

EFI_STATUS
SioReadWrite (
  IN  BOOLEAN                       SaveRestoreFlag,
  IN  UINTN                         RegNum,
  IN  UINT8                         *RegTable,
  IN OUT UINT8                      *SaveTable
  )
{
  UINTN                             Index;
  UINT8                             EnterConfig;
  UINT8                             ExitConfig;

  EnterConfig = 0x55;
  IoWrite8 (SMM_SIO_INDEX, EnterConfig);

  for (Index = 0; Index < RegNum; Index++) {
    IoWrite8 (SMM_SIO_INDEX, RegTable[Index]);

    if (SaveRestoreFlag) {
      IoWrite8 (SMM_SIO_DATA, SaveTable[Index]);

    } else {
      SaveTable[Index] = IoRead8 (SMM_SIO_DATA);

    }
  }

  ExitConfig = 0xAA;
  IoWrite8 (SMM_SIO_INDEX, ExitConfig);

  return  EFI_SUCCESS;
}

EFI_STATUS
SaveRestoreSio (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  return EFI_SUCCESS;
}


