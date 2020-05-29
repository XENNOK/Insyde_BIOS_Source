//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include  "OemEc.h"

EFI_STATUS
EcIdle (
  IN  BOOLEAN         EnableWrites
  )
/*++
Routine Description:
  Platform specific function to enable flash / hardware for OEM EC

Arguments:
  EnableWrites       - Boolean to enable/disable flash

Returns:
  EFI_SUCCESS

--*/
{
  //
  //Followings are EC idle reference code.
  //
  /*
  UINT8                       Buffer;
  UINT16                      IO_BASE=0x208;
  Buffer = 0;

  if (EnableWrites == TRUE) {
    //
    //wait ibe
    //
    SmiIoWrite8 (0x64, 0xdd);

    do {
      Buffer = IoRead8 (0x64);
    } while ((Buffer & 2) == 2);
    Buffer = IoRead8 (0x60);

    if (Buffer == 0xfa) {
      do {
        Buffer = IoRead8 (0x64);
      } while ((Buffer & 2) == 2);
    }

    IoWrite8 (0x2e, 0x07);
    IoWrite8 (0x2f, 0x0F);

    IoWrite8 (0x2e, 0x60);
    IoWrite8 (0x2f, 0x02);

    IoWrite8 (0x2e, 0x61);
    IoWrite8 (0x2f, 0x00);

    IoWrite8 (0x2e, 0x30);
    IoWrite8 (0x2f, 0x01);

    IoWrite8 (IO_BASE, 0xf0);
    IoWrite8 (IO_BASE, 0xe0);
    IoWrite8 (IO_BASE, 0xd0);
    IoWrite8 (IO_BASE, 0xc0);
    IoWrite8 (IO_BASE, 0xb0);
    IoWrite8 (IO_BASE, 0xa0);
    IoWrite8 (IO_BASE, 0x90);
    IoWrite8 (IO_BASE, 0x80);
    IoWrite8 (IO_BASE, 0x70);
    IoWrite8 (IO_BASE, 0x60);
    IoWrite8 (IO_BASE, 0x50);
    IoWrite8 (IO_BASE, 0x40);
    IoWrite8 (IO_BASE, 0x30);
    IoWrite8 (IO_BASE, 0x20);
    IoWrite8 (IO_BASE, 0x10);
    IoWrite8 (IO_BASE, 0x00);
                                                  // SST ROM part can ignore this Stall
    Stall(5000);

  } else {
    IoWrite8 (0x64, 0x55);
    IoWrite8 (IO_BASE, 0xf2);
    IoWrite8 (IO_BASE, 0xe2);
    IoWrite8 (IO_BASE, 0xd2);
    IoWrite8 (IO_BASE, 0xc2);
    IoWrite8 (IO_BASE, 0xb2);
    IoWrite8 (IO_BASE, 0xa2);
    IoWrite8 (IO_BASE, 0x92);
    IoWrite8 (IO_BASE, 0x82);
    IoWrite8 (IO_BASE, 0x72);
    IoWrite8 (IO_BASE, 0x62);
    IoWrite8 (IO_BASE, 0x52);
    IoWrite8 (IO_BASE, 0x42);
    IoWrite8 (IO_BASE, 0x32);
    IoWrite8 (IO_BASE, 0x22);
    IoWrite8 (IO_BASE, 0x12);
    IoWrite8 (IO_BASE, 0x02);
    IoWrite8 (0x2e, 0x07);
    IoWrite8 (0x2f, 0x0F);

    IoWrite8 (0x2e, 0x30);
    IoWrite8 (0x2f, 0x00);
  }
  */
  return EFI_SUCCESS;
}
