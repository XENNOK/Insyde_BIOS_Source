/** @file
 Function definition for the CMOS library.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/IoLib.h>
#include <Library/CmosLib.h>

#define R_CMOS_INDEX                 0x70
#define R_CMOS_DATA                  0x71
#define INTERRUPT_FLAG               BIT9

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

/**
 Read an 8-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return An 8-bit value in the address offset of CMOS.
**/
UINT8
ReadCmos8 (
  IN UINT8                                 Address
  )
{
  UINT16        Eflags;
  UINT8         Value;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (R_CMOS_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_CMOS_INDEX) & 0x80)));
  Value = IoRead8 (R_CMOS_DATA);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }

  return Value;
}

/**
 Write an 8-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  )
{
  UINT16        Eflags;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (R_CMOS_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_CMOS_INDEX) & 0x80)));
  IoWrite8 (R_CMOS_DATA, Data);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return;
}

/**
 Read a 16-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 16-bit value in the address offset of CMOS.
**/
UINT16
ReadCmos16 (
  IN UINT8                                 Address
  )
{
  UINT16   Temp16 = 0;
  UINT8    Temp8 = 0;

  Temp8 = ReadCmos8(Address + 1);
  Temp16 = (UINT16)(Temp8<<8);

  Temp8 = ReadCmos8(Address);
  Temp16 =(UINT16) (Temp16 + Temp8);

  return Temp16;
}

/**
 Write a 16-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x00FF);
  WriteCmos8(Address, Temp8);

  Temp8 = (UINT8)((Data&0xFF00)>>8);
  WriteCmos8(Address + 1, Temp8);

  return;
}

/**
 Read a 32-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 32-bit value in the address offset of CMOS.
**/
UINT32
ReadCmos32 (
  IN UINT8                                 Address
  )
{
  UINT32   Temp32 = 0;
  UINT8    Temp8 = 0;

  Temp8 = ReadCmos8(Address + 3);
  Temp32 = (UINT32) (Temp8<<24);

  Temp8 = ReadCmos8(Address + 2);
  Temp32 = Temp32 + (UINT32)(Temp8<<16);

  Temp8 = ReadCmos8(Address + 1);
  Temp32 = Temp32 + (UINT32)(Temp8<<8);

  Temp8 = ReadCmos8(Address);
  Temp32 =(UINT32) (Temp32 + Temp8);

  return Temp32;
}

/**
 Write a 32-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x000000FF);
  WriteCmos8(Address, Temp8);

  Temp8 = (UINT8)((Data&0x0000FF00)>>8);
  WriteCmos8(Address + 1, Temp8);

  Temp8 = (UINT8)((Data&0x00FF0000)>>16);
  WriteCmos8(Address + 2, Temp8);

  Temp8 = (UINT8)((Data&0xFF000000)>>24);
  WriteCmos8(Address + 3, Temp8);

  return;
}

/**
 Sum standard CMOS.

 @return Summed bytes 0x10 through 0x2D
**/
UINT16
SumaryCmos (
  VOID
  )
{
  UINT8   Address;
  UINT16  RunningChecksum;
  UINT8   Temp;

  RunningChecksum = 0;
  for (Address = 0x10; Address < 0x2e; Address++) {
     Temp = ReadCmos8(Address);
     RunningChecksum = (UINT16) ( RunningChecksum + Temp);
  }
  return RunningChecksum;
}

/**
 Validate standard CMOS.

 @retval EFI_SUCCESS            valid checksum
 @retval EFI_VOLUME_CORRUPTED   a corrupted checksum
**/
EFI_STATUS
ValidateCmosChecksum (
  VOID
  )
{
  UINT16  RunningChecksum;
  UINT16  Checksum;
  UINT16 TempChecksum;

  RunningChecksum = SumaryCmos();

  TempChecksum = ReadCmos16(CmosCheckSum2E);
  Checksum = (TempChecksum >> 8) + ((TempChecksum & 0xff) << 8);

  if (Checksum == RunningChecksum) {
    return EFI_SUCCESS;
  } else {
    return EFI_VOLUME_CORRUPTED;
  }
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read an 8-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return An 8-bit value read.
**/
UINT8
ReadExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT16        Eflags;
  UINT8         Value;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (XCmosIndex, Address);
  Value = IoRead8 (XCmosData);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return Value;
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read a 16-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return A 16-bit value read.
**/
UINT16
ReadExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT16   Temp16 = 0;
  UINT8    Temp8 = 0;

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData, Address + 1);
  Temp16 = (UINT16)(Temp8<<8);

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData,Address);
  Temp16 =(UINT16) (Temp16 + Temp8);

  return Temp16;
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read an 32-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return A 32-bit value read.
**/
UINT32
ReadExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  )
{
  UINT32   Temp32 = 0;
  UINT8    Temp8 = 0;

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData, Address + 3);
  Temp32 = (UINT32) (Temp8<<24);

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData, Address + 2);
  Temp32 = Temp32 + (UINT32)(Temp8<<16);

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData, Address + 1);
  Temp32 = Temp32 + (UINT32)(Temp8<<8);

  Temp8 = ReadExtCmos8(XCmosIndex, XCmosData, Address);
  Temp32 =(UINT32) (Temp32 + Temp8);

  return Temp32;
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write an 8-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT8                                 Data
  )
{
  UINT16        Eflags;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (XCmosIndex, Address);
  IoWrite8 (XCmosData, Data);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return;
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write a 16-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT16                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x00FF);
  WriteExtCmos8(XCmosIndex, XCmosData, Address, Temp8);

  Temp8 = (UINT8)((Data&0xFF00)>>8);
  WriteExtCmos8(XCmosIndex, XCmosData, Address + 1, Temp8);

  return;
}

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write a 32-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT32                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x000000FF);
  WriteExtCmos8(XCmosIndex, XCmosData, Address, Temp8);

  Temp8 = (UINT8)((Data&0x0000FF00)>>8);
  WriteExtCmos8(XCmosIndex, XCmosData, Address + 1, Temp8);

  Temp8 = (UINT8)((Data&0x00FF0000)>>16);
  WriteExtCmos8(XCmosIndex, XCmosData, Address + 2, Temp8);

  Temp8 = (UINT8)((Data&0xFF000000)>>24);
  WriteExtCmos8(XCmosIndex, XCmosData, Address + 3, Temp8);

  return;
}

