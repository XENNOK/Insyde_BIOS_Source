/** @file
  Helper functions for calculating CRC32 checksum

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

UINT32  mCrcTable[256];

/**
  Calculate CRC32 for target data.

  @param  Data                  The target data.
  @param  DataSize              The target data size.
  @param  CrcOut                The CRC32 for target data.

  @retval EFI_SUCCESS           The CRC32 for target data is calculated successfully.
  @retval EFI_INVALID_PARAMETER Some parameter is not valid, so the CRC32 is not
                                calculated.

**/
EFI_STATUS
EFIAPI
CalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;

  if (Data == NULL || DataSize == 0 || CrcOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0xffffffff;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ mCrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ 0xffffffff;
  return EFI_SUCCESS;
}


/**
  This internal function reverses bits for 32bit data.

  @param  Value                 The data to be reversed.

  @return                       Data reversed.

**/
UINT32
ReverseBits (
  UINT32  Value
  )
{
  UINTN   Index;
  UINT32  NewValue;

  NewValue = 0;
  for (Index = 0; Index < 32; Index++) {
    if ((Value & (1 << Index)) != 0) {
      NewValue = NewValue | (1 << (31 - Index));
    }
  }

  return NewValue;
}

/**
  Initialize CRC32 table.

  @param  none

  @retval none
**/
VOID
InitializeCrc32Table (
  VOID
  )
{
  UINTN   TableEntry;
  UINTN   Index;
  UINT32  Value;

  for (TableEntry = 0; TableEntry < 256; TableEntry++) {
    Value = ReverseBits ((UINT32) TableEntry);
    for (Index = 0; Index < 8; Index++) {
      if ((Value & 0x80000000) != 0) {
        Value = (Value << 1) ^ 0x04c11db7;
      } else {
        Value = Value << 1;
      }
    }

    mCrcTable[TableEntry] = ReverseBits (Value);
  }
}

