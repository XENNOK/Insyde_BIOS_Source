//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "CmosLib.h"
#include "IoAccess.h"

VOID
CmosDisableInterrupt (
  VOID
  );

VOID
CmosEableInterrupt (
  VOID
  );

UINT16
CmosGetCpuFlags (
  VOID
  );

UINT8
EfiReadCmos8 (
  IN UINT8                                 Address
  )
{
  UINT16        Eflags;
  UINT8         Value;
    
  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();
  
  IoOutput8 (R_CMOS_INDEX, (UINT8) (Address | (UINT8) (IoInput8 (R_CMOS_INDEX) & 0x80)));
  Value = IoInput8 (R_CMOS_DATA);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return Value;
}

VOID
EfiWriteCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  )
{
  UINT16        Eflags;
  
  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();
  
  IoOutput8 (R_CMOS_INDEX, (UINT8) (Address | (UINT8) (IoInput8 (R_CMOS_INDEX) & 0x80)));  
  IoOutput8 (R_CMOS_DATA, Data);  

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return;
}

UINT16
EfiReadCmos16 (
  IN UINT8                                 Address
  )
{
  UINT16   Temp16 = 0;
  UINT8    Temp8 = 0;

  Temp8 = EfiReadCmos8(Address + 1);
  Temp16 = (UINT16)(Temp8<<8);

  Temp8 = EfiReadCmos8(Address);
  Temp16 =(UINT16) (Temp16 + Temp8);

  return Temp16;
}

VOID
EfiWriteCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                 Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x00FF);
  EfiWriteCmos8(Address, Temp8);

  Temp8 = (UINT8)((Data&0xFF00)>>8);
  EfiWriteCmos8(Address + 1, Temp8);

  return;
}

UINT32
EfiReadCmos32 (
  IN UINT8                                 Address
  )
{
  UINT32   Temp32 = 0;
  UINT8    Temp8 = 0;

  Temp8  = EfiReadCmos8(Address + 3);
  Temp32 = (UINT32) (Temp8<<24);

  Temp8  = EfiReadCmos8(Address + 2);
  Temp32 = Temp32 + (UINT32)(Temp8<<16);

  Temp8  = EfiReadCmos8(Address + 1);
  Temp32 = Temp32 + (UINT32)(Temp8<<8);

  Temp8  = EfiReadCmos8(Address);
  Temp32 =(UINT32) (Temp32 + Temp8);

  return Temp32;
}

VOID
EfiWriteCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                 Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x000000FF);
  EfiWriteCmos8(Address, Temp8);

  Temp8 = (UINT8)((Data&0x0000FF00)>>8);
  EfiWriteCmos8(Address + 1, Temp8);

  Temp8 = (UINT8)((Data&0x00FF0000)>>16);
  EfiWriteCmos8(Address + 2, Temp8);

  Temp8 = (UINT8)((Data&0xFF000000)>>24);
  EfiWriteCmos8(Address + 3, Temp8);

  return;
}

UINT16
EfiSumaryCmos (
  VOID
  )
/*++

Routine Description:
  Sum standard CMOS.

Arguments:
  None

Returns:
  Summed bytes 0x10 through 0x2D

--*/
{
 UINT8   Address;
 UINT16  RunningChecksum;
 UINT8   Temp;

 RunningChecksum = 0;
 for (Address = 0x10; Address < 0x2e; Address++) {
   Temp = EfiReadCmos8 (Address);
   RunningChecksum = (UINT16) ( RunningChecksum + Temp);
 }
 return RunningChecksum;
}

EFI_STATUS
EfiValidateCmosChecksum (
  VOID
  )
/*++

Routine Description:
  Validate standard CMOS.

Arguments:
  None

Returns:
  EFI_SUCCESS for a valid checksum
  EFI_VOLUME_CORRUPTED for a corrupted checksum

--*/
{
  UINT16  RunningChecksum;
  UINT16  Checksum;
  UINT16 TempChecksum;

  RunningChecksum = EfiSumaryCmos ();

  TempChecksum = EfiReadCmos16 (CmosCheckSum2E);
  Checksum = (TempChecksum >> 8) + ((TempChecksum & 0xff) << 8);

  if (Checksum == RunningChecksum) {
    return EFI_SUCCESS;
  } else {
    return EFI_VOLUME_CORRUPTED;
  }
}

UINT8
EfiReadExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT16        Eflags;
  UINT8         Value;
    
  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();
  
  IoOutput8 (XCmosIndex, Address);
  Value = IoInput8 (XCmosData);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return Value;
}

UINT16
EfiReadExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT16   Temp16 = 0;
  UINT8    Temp8 = 0;

  Temp8  = EfiReadExtCmos8 (XCmosIndex, XCmosData, Address + 1);
  Temp16 = (UINT16)(Temp8<<8);

  Temp8  = EfiReadExtCmos8 (XCmosIndex, XCmosData,Address);
  Temp16 =(UINT16) (Temp16 + Temp8);

  return Temp16;
}

UINT32
EfiReadExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT32   Temp32 = 0;
  UINT8    Temp8 = 0;

  Temp8 = EfiReadExtCmos8(XCmosIndex, XCmosData, Address + 3);
  Temp32 = (UINT32) (Temp8<<24);

  Temp8 = EfiReadExtCmos8(XCmosIndex, XCmosData, Address + 2);
  Temp32 = Temp32 + (UINT32)(Temp8<<16);

  Temp8 = EfiReadExtCmos8(XCmosIndex, XCmosData, Address + 1);
  Temp32 = Temp32 + (UINT32)(Temp8<<8);

  Temp8 = EfiReadExtCmos8(XCmosIndex, XCmosData, Address);
  Temp32 =(UINT32) (Temp32 + Temp8);

  return Temp32;
}

VOID
EfiWriteExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT8                                 Data
  )
{
  UINT16        Eflags;
  
  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();
  
  IoOutput8 (XCmosIndex, Address);
  IoOutput8 (XCmosData, Data);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return;
}

VOID
EfiWriteExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT16                                 Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x00FF);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address, Temp8);

  Temp8 = (UINT8)((Data&0xFF00)>>8);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address + 1, Temp8);

  return;
}

VOID
EfiWriteExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT32                                 Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x000000FF);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address, Temp8);

  Temp8 = (UINT8)((Data&0x0000FF00)>>8);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address + 1, Temp8);

  Temp8 = (UINT8)((Data&0x00FF0000)>>16);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address + 2, Temp8);

  Temp8 = (UINT8)((Data&0xFF000000)>>24);
  EfiWriteExtCmos8 (XCmosIndex, XCmosData, Address + 3, Temp8);

  return;
}
