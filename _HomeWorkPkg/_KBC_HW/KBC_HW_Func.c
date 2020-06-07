/** @file

  Keyboard Controller Function
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"KBC_HW.h"

EFI_STATUS
WaitKBC_IBF (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{
  EFI_STATUS Status;
  UINT8      IBF_Data;
  
  do {
    Status = CpuIo->Io.Read (
                         CpuIo,
                         EfiCpuIoWidthUint8,
                         KBC_STD_CMD,
                         1,
                         &IBF_Data
                         );
    
    if (EFI_ERROR (Status)) {
      Print (L"Wait IBF Error");
      return Status;
    }
  } while ( (IBF_Data & 0x02) != 0);
  return Status;
}

EFI_STATUS
KBC_Event (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN UINT64                                 KBC_CMD_PORT,
  IN UINT64                                 KBC_DATA_PORT,
  IN UINT8                                  *KBC_ADD,
  IN UINT8                                  *KBC_DATA
  )
{
  EFI_STATUS Status;

  //
  // Wait for IBF Empty
  //
  WaitKBC_IBF (CpuIo);

  //
  // Write to command port
  //
  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiCpuIoWidthUint8,
                       KBC_CMD_PORT,
                       1,
                       KBC_ADD
                       );
    if (EFI_ERROR (Status)) {
      Print (L"Write KBC_CMD_PORT Error");
      return Status;
    }
    
  //
  // Wait for IBF Empty
  //  
  WaitKBC_IBF (CpuIo);

  //
  //Write to data port
  //
  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiCpuIoWidthUint8,
                       KBC_DATA_PORT,
                       1,
                       KBC_DATA
                       );
  
  if (EFI_ERROR (Status)) {
      Print (L"Write KBC_DATA_PORT Error");
      return Status;
  }
  
  return Status;
}
