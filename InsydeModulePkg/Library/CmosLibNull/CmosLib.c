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
  )
{
}

VOID
CmosEableInterrupt (
  VOID
  )
{
}

UINT16
CmosGetCpuFlags (
  VOID
  )
{
  return 0;
}

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
  return 0;
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
  return 0;
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
  return 0;
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
  return 0;
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
  return EFI_SUCCESS;
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
  return 0;
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
  return 0;
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
  return 0;
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
}

