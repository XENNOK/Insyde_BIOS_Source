/** @file
 H2O IPMI FRU module implement code.

 This c file contains common functions for H2O IPMI FRU module internal use.

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


#include <Library/BaseLib.h>
#include "IpmiFruCommon.h"


//
// This driver will be single instance
//
H2O_IPMI_FRU_PROTOCOL   mFruProtocol;

//
// Keep current FRU's id and size so that we don't get it again and again
//
UINT8                   mCurrentFruId = 0;
UINT16                  mCurrentFruSize = 0xFFFF;
BOOLEAN                 mAlreadyGet = FALSE;


CHAR16  *mChassisTypeStr[] = {
  L"Other",
  L"Unknown",
  L"Desktop",
  L"Low Profile Desktop",
  L"Pizza Box",
  L"Mini Tower",
  L"Tower",
  L"Portable",
  L"LapTop",
  L"Notebook",
  L"Hand Held",
  L"Docking Station",
  L"All in One",
  L"Sub Notebook",
  L"Space-saving",
  L"Lunch Box",
  L"Main Server Chassis",
  L"Expansion Chassis",
  L"SubChassis",
  L"Bus Expansion Chassis",
  L"Peripheral Chassis",
  L"RAID Chassis",
  L"Rack Mount Chassis"
};

/**
 Convert a string packed in 6bit format to normal 8bit ascii string.

 Get 4 6bit packed bytes and resort their bits to form 3 8bit ascii bytes.

 @param[in]         Str6Bit             Pointer to 6 bit packed string
 @param[in]         Str6BitSize         Total size of 6 bit packed string
 @param[out]        Str8Bit             Converted 8 bit ascii string
 @param[out]        Str8BitSize         Total size of 8 bit string after converting
*/
VOID
FruLibConvert6BitTo8BitAscii (
  IN  CHAR8                             *Str6Bit,
  IN  UINT8                             Str6BitSize,
  OUT CHAR8                             *Str8Bit,
  OUT UINT8                             *Str8BitSize
  )
{
  INT8                                  Index;
  UINT32                                Temp;
  UINT8                                 DestIndex;
  UINT8                                 UnpackedSize;


  //
  // We need to handle three bytes to fit 4 8 bit ascii
  //
  DestIndex = 0;

  for (UnpackedSize = 0; UnpackedSize < Str6BitSize; UnpackedSize += 3) {
    Temp = 0;
    for (Index = 2; Index >= 0; --Index) {
      Temp |= ((UINT32)Str6Bit[Index + UnpackedSize] & 0xFF);
      Temp <<= 8;
    }

    Temp >>= 8;

    for (Index = 0; Index < 4; ++Index) {
      Str8Bit[DestIndex] = (CHAR8)(Temp & 0x3F) + 0x20;
      Temp >>= 6;
      ++DestIndex;
    }
  }

  *Str8BitSize = DestIndex;

}

/**
 Fru read function

 @param[in]         FruId               FRU ID
 @param[in]         Offset              Offset want to read
 @param[in]         ReadSize            Size want to read
 @param[out]        Buffer              Buffer to store read data

 @retval EFI_SUCCESS                    Read success
 @retval EFI_UNSUPPORTED                Read fail
*/
EFI_STATUS
FruLibReadFruCommand (
  IN  UINT8                             FruId,
  IN  UINT16                            Offset,
  IN  UINT16                            ReadSize,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS                            Status;
  UINT8                                 SendDataSize;
  UINT8                                 SendData[4];
  UINT8                                 RecvDataSize;
  UINT8                                 RecvData[0x20];
  UINT16                                StartOffset;
  UINT8                                 *BufferPtr;
  UINT8                                 Length;
  UINT16                                AlreadyReadLen;


  //
  // Get FRU Data 0x10 bytes one time
  //
  BufferPtr = Buffer;
  StartOffset = Offset;
  Length = 0;
  AlreadyReadLen = 0;

  for (AlreadyReadLen = 0; AlreadyReadLen < ReadSize; AlreadyReadLen = (UINT8)(AlreadyReadLen + Length)) {
    if ((ReadSize - AlreadyReadLen) > H2O_IPMI_CMD_MAX_LENGTH) {
      Length = H2O_IPMI_CMD_MAX_LENGTH;
    } else {
      Length = (UINT8)(ReadSize - AlreadyReadLen);
    }


    SendData[0] = FruId;
    SendData[1] = (UINT8)StartOffset;
    SendData[2] = (UINT8)(StartOffset >> 8);
    SendData[3] = Length;
    SendDataSize = sizeof (SendData);

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_STORAGE,
               H2O_IPMI_CMD_READ_FRU_DATA,
               SendData,
               SendDataSize,
               RecvData,
               &RecvDataSize
               );

    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    CopyMem (BufferPtr, &RecvData[1], (UINTN)RecvData[0]);

    StartOffset = (UINT16)(StartOffset + Length);

    BufferPtr += Length;

  }

  return EFI_SUCCESS;

}

/**
 Fru write function

 @param[in]         FruId               FRU ID
 @param[in]         Offset              Offset want to write
 @param[in]         WriteSize           Buffer to write to fru
 @param[in]         Buffer              Buffer to store read data

 @retval EFI_SUCCESS                    Write success
 @retval EFI_UNSUPPORTED                Write fail
*/
EFI_STATUS
FruLibWriteFruCommand (
  IN  UINT8                             FruId,
  IN  UINT16                            Offset,
  IN  UINT16                            WriteSize,
  IN  UINT8                             *Buffer
  )
{
  EFI_STATUS                            Status;
  UINT8                                 SendDataSize;
  UINT8                                 SendData[0x20];
  UINT8                                 RecvDataSize;
  UINT8                                 RecvData[0x10];
  UINT16                                StartOffset;
  UINT8                                 *BufferPtr;
  UINT8                                 Length;
  UINT16                                AlreadyWriteLen;


  //
  // Write FRU Data 0x10 bytes one time
  //
  BufferPtr = Buffer;
  StartOffset = Offset;
  Length = 0;
  AlreadyWriteLen = 0;

  for (AlreadyWriteLen = 0; AlreadyWriteLen < WriteSize; AlreadyWriteLen = (UINT8)(AlreadyWriteLen + Length)) {

    if ((WriteSize - AlreadyWriteLen) > H2O_IPMI_CMD_MAX_LENGTH) {
      Length = H2O_IPMI_CMD_MAX_LENGTH;
    } else {
      Length = (UINT8)(WriteSize - AlreadyWriteLen);
    }


    SendData[0] = FruId;
    SendData[1] = (UINT8)StartOffset;
    SendData[2] = (UINT8)(StartOffset >> 8);
    CopyMem (&SendData[3], BufferPtr, (UINTN)Length);
    SendDataSize = Length + 3;

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_STORAGE,
               H2O_IPMI_CMD_WRITE_FRU_DATA,
               SendData,
               SendDataSize,
               RecvData,
               &RecvDataSize
               );

    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    StartOffset = (UINT16)(StartOffset + Length);

    BufferPtr += Length;

  }

  return EFI_SUCCESS;

}

/**
 Get specified field's encode, length and data offset

 @param[in]         FruId               FRU ID
 @param[in]         Field               Specified field
 @param[out]        Encode              Encode of specified field
 @param[out]        Length              Length of specified field data
 @param[out]        Offset              Offset of specified field

 @retval EFI_SUCCESS                    Get success
 @retval EFI_UNSUPPORTED                1. FruId or Field does not exist
                                        2. IPMI command fail
*/
EFI_STATUS
FruLibGetOffset (
  IN  UINT8                             FruId,
  IN  UINT16                            Field,
  OUT UINT8                             *Encode,
  OUT UINT8                             *Length,
  OUT UINT16                            *Offset
  )
{
  H2O_IPMI_FRU_COMMON_HEADER            Header;
  UINT16                                Size;
  EFI_STATUS                            Status;
  UINT16                                TempField;
  UINT16                                Index;
  UINT16                                BaseOffset;
  UINT16                                AreaOffset;
  UINT16                                ReadOffset;
  UINT8                                 LengthByte;


  //
  // Check Field first
  //
  BaseOffset = 0;
  AreaOffset = 0;
  ReadOffset = 0;
  
  switch (Field & 0xF000) {
  case FRU_CHASSIS_INFO_AREA:
    BaseOffset = H2O_IPMI_FRU_CHASSIS_START_OFFSET;
    break;

  case FRU_BOARD_INFO_AREA:
    BaseOffset = H2O_IPMI_FRU_BOARD_START_OFFSET;
    break;

  case FRU_PRODUCT_INFO_AREA:
    BaseOffset = H2O_IPMI_FRU_PRODUCT_START_OFFSET;
    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }


  //
  // Get Fru Common Header first
  //
  Size = sizeof (H2O_IPMI_FRU_COMMON_HEADER);
  Status = FruLibReadFruCommand (FruId, 0, Size, (UINT8*)&Header);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }


  //
  // Check if this area is supported by BMC
  //
  switch (Field & 0xF000) {
  case FRU_CHASSIS_INFO_AREA:
    AreaOffset = Header.ChassisAreaOffset;
    break;

  case FRU_BOARD_INFO_AREA:
    AreaOffset = Header.BoardAreaOffset;
    break;

  case FRU_PRODUCT_INFO_AREA:
    AreaOffset = Header.ProductAreaOffset;
    break;
  }

  if (AreaOffset == 0 || AreaOffset == 0xFF) {
    return EFI_UNSUPPORTED;
  }


  //
  // Start to search Field
  //
  AreaOffset <<= 3;
  ReadOffset = AreaOffset + BaseOffset;
  TempField = Field & ~(FRU_CHASSIS_INFO_AREA | FRU_BOARD_INFO_AREA | FRU_PRODUCT_INFO_AREA);
  Index = 1;

  while (TRUE) {
    Status = FruLibReadFruCommand (FruId, ReadOffset, 1, &LengthByte);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (LengthByte == H2O_IPMI_FRU_AREA_END_LENGTH_BYTE) {
      break;
    }

    if (Index == TempField) {
      *Encode = LengthByte >> 6;
      *Length = LengthByte & 0x3F;
      *Offset = ReadOffset + 1;

      return EFI_SUCCESS;
    }

    ReadOffset += (LengthByte & 0x3F) + 1;
    ++Index;

  }

  return EFI_UNSUPPORTED;

}

/**
 Get specified field's checksum and checksum offset.

 @param[in]         FruId               FRU ID
 @param[in]         Field               Specified field
 @param[out]        NewChecksum         Checksum of specified field with current data
 @param[out]        OldChecksum         Checksum of specified field in FRU
 @param[out]        ChecksumOffset      Offset of checksum

 @retval EFI_SUCCESS                    Get success
 @retval EFI_UNSUPPORTED                Get fail
*/
EFI_STATUS
FruLibGetFieldChecksum (
  IN  UINT8                             FruId,
  IN  UINT16                            Field,
  OUT UINT8                             *NewChecksum,
  OUT UINT8                             *OldChecksum,
  OUT UINT16                            *ChecksumOffset
  )
{
  H2O_IPMI_FRU_COMMON_HEADER            Header;
  UINT16                                Size;
  EFI_STATUS                            Status;
  UINT16                                AreaOffset;
  UINT16                                Length;
  UINT8                                 Temp[2];
  UINT8                                 *Buffer;
  UINT8                                 Checksum;
  UINTN                                 Index;


  //
  // Get Fru Common Header first
  //
  AreaOffset = 0;
  Size = sizeof (H2O_IPMI_FRU_COMMON_HEADER);
  Status = FruLibReadFruCommand (FruId, 0, Size, (UINT8*)&Header);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check if this area is supported by BMC
  //
  switch (Field & 0xF000) {
  case FRU_CHASSIS_INFO_AREA:
    AreaOffset = Header.ChassisAreaOffset;
    break;

  case FRU_BOARD_INFO_AREA:
    AreaOffset = Header.BoardAreaOffset;
    break;

  case FRU_PRODUCT_INFO_AREA:
    AreaOffset = Header.ProductAreaOffset;
    break;
  }

  if (AreaOffset == 0 || AreaOffset == 0xFF) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get first two bytes of each area. We get area length from byte 1.
  //
  AreaOffset = AreaOffset << 3;
  Status = FruLibReadFruCommand (FruId, AreaOffset, 2, Temp);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Allocate memory to get all area data
  //
  Length = (UINT16)Temp[1] << 3;
  Buffer = (UINT8*)AllocateZeroPool ((UINTN)Length);
  if (Buffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = FruLibReadFruCommand (FruId, AreaOffset, Length, Buffer);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate current checksum
  //
  Checksum = 0;
  for (Index = 0; Index < (UINTN)(Length - 1); ++Index) {
    Checksum = Checksum + Buffer[Index];
  }

  *NewChecksum = 0 - Checksum;
  *OldChecksum = Buffer[Length -1];
  *ChecksumOffset = AreaOffset + Length - 1;

  FreePool (Buffer);

  return EFI_SUCCESS;

}

/**
 Read specified string/data based on field from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Field               Field id want to get.
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of string buffer.
 @param[out]        StringBuffer        Buffer to store string/data.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
EFI_STATUS
EFIAPI
FruReadFieldData (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN OUT UINT8                          *StringBufferSize,
  OUT    UINT8                          *StringBuffer
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Length;
  UINT8                                 Encode;
  UINT16                                Offset;
  UINT8                                 *LocalBuffer;
  UINT8                                 ConvLength;


  //
  // Check parameter
  //
  if (StringBufferSize == NULL || StringBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // Get specified field offset and length
  //
  Status = FruLibGetOffset (FruId, Field, &Encode, &Length, &Offset);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }


  //
  // Check buffer size
  //
  if (Encode == H2O_IPMI_FRU_TYPE_6BIT_ASCII) {
    ConvLength = (Length / 3) << 2;
  } else {
    ConvLength = Length;
  }

  if (*StringBufferSize < ConvLength) {
    *StringBufferSize = ConvLength;
    return EFI_BUFFER_TOO_SMALL;
  }


  //
  // Read data
  //
  LocalBuffer = (UINT8*)AllocateZeroPool ((UINTN)Length);
  if (LocalBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = FruLibReadFruCommand (FruId, Offset, (UINT16)Length, LocalBuffer);

  if (EFI_ERROR (Status)) {
    FreePool (LocalBuffer);
    return EFI_UNSUPPORTED;
  }


  //
  // Return data
  //
  if (Encode != H2O_IPMI_FRU_TYPE_6BIT_ASCII) {
    *StringBufferSize = Length;
    CopyMem (StringBuffer, LocalBuffer, (UINTN)Length);

  } else {
    FruLibConvert6BitTo8BitAscii ((CHAR8*)LocalBuffer, Length, (CHAR8*)StringBuffer, StringBufferSize);
  }

  FreePool (LocalBuffer);

  return EFI_SUCCESS;

}

/**
 Write specified string/data based on field to FRU.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Field               Field id want to write.
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of write string buffer.
 @param[in]         StringBuffer        Buffer to store string/data.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than write data size.
*/
EFI_STATUS
EFIAPI
FruWriteFieldData (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN OUT UINT8                          *StringBufferSize,
  IN     UINT8                          *StringBuffer
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Length;
  UINT8                                 Encode;
  UINT16                                Offset;
  UINT8                                 NewChecksum;
  UINT8                                 OldChecksum;
  UINT16                                ChecksumOffset;

  //
  // Check parameter
  //
  if (StringBufferSize == NULL || StringBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // Get specified field offset and length
  //
  Status = FruLibGetOffset (FruId, Field, &Encode, &Length, &Offset);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }


  //
  // We don't support 6 bit ascii
  //
  if (Encode == H2O_IPMI_FRU_TYPE_6BIT_ASCII) {
    return EFI_UNSUPPORTED;
  }


  if (*StringBufferSize > Length) {
    *StringBufferSize = Length;
    return EFI_BUFFER_TOO_SMALL;
  }


  //
  // Write data
  //
  Status = FruLibWriteFruCommand (FruId, Offset, (UINT16)*StringBufferSize, StringBuffer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Update checksum of this area
  //
  Status = FruLibGetFieldChecksum (FruId, Field, &NewChecksum, &OldChecksum, &ChecksumOffset);
  if (!EFI_ERROR (Status)) {
    FruLibWriteFruCommand (FruId, ChecksumOffset, 1, &NewChecksum);
  }

  return EFI_SUCCESS;

}

/**
 Get Chassis Type value/string field in Chassis info area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in,out]     ChassisTypeStrSize  When in, the size of chassis type string in bytes to store the result;
                                        When out, the real size of string buffer in bytes.
 @param[out]        ChassisType         Enumeration value for chassis type field.
 @param[out]        ChassisTypeString   Chassis type string.

 @retval EFI_UNSUPPORTED                Common header didn't exist.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
EFI_STATUS
EFIAPI
FruGetChassisType (
  IN     UINT8                          FruId,
  IN OUT UINT8                          *ChassisTypeStrSize,
  OUT    UINT8                          *ChassisType,
  OUT    UINT8                          *ChassisTypeString
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Size;
  H2O_IPMI_FRU_COMMON_HEADER            Header;
  UINT8                                 TempStrSize;
  UINT16                                ChassisTypeOffset;

  //
  // Get Fru Common Header first
  //
  Size = sizeof (H2O_IPMI_FRU_COMMON_HEADER);
  Status = FruLibReadFruCommand (FruId, 0, Size, (UINT8*)(&Header));
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (Header.ChassisAreaOffset != 0x00 && Header.ChassisAreaOffset != 0xFF) {

    ChassisTypeOffset = (Header.ChassisAreaOffset << 3) + H2O_IPMI_FRU_CHASSIS_TYPE_OFFSET;
    Status = FruLibReadFruCommand (
               FruId,
               ChassisTypeOffset,
               H2O_IPMI_FRU_CHASSIS_TYPE_LEN,
               ChassisType
               );

    if (!EFI_ERROR (Status)) {
      TempStrSize = (UINT8)StrSize (mChassisTypeStr[*ChassisType - 1]);

      //
      // Check if buffer too small
      //
      if (TempStrSize > *ChassisTypeStrSize) {
        *ChassisTypeStrSize = TempStrSize;
        return EFI_BUFFER_TOO_SMALL;
      }

      CopyMem (ChassisTypeString, mChassisTypeStr[*ChassisType - 1], TempStrSize);

      return EFI_SUCCESS;

    } else {
      return EFI_UNSUPPORTED;
    }
  } else {
    *ChassisType = 0;
    return EFI_UNSUPPORTED;
  }

}

/**
 Convert 3 byte Mfg time to EFI_TIME structure.

 @param[in]         MfgTime             Number of minutes from 0:00 hrs 1/1/96.
 @param[out]        EfiTime             EFI_TIME structure use to store the result converting from TimeStamp.

*/
VOID
MfgTimeToEfiTime (
  IN  UINT8                             *MfgTime,
  OUT EFI_TIME                          *EfiTime
  )
{
  UINT32                                MfgTotalTime;

  MfgTotalTime = (*(MfgTime + 2) << 16) + (*(MfgTime + 1) << 8) + *MfgTime;

  MfgTotalTime *= 60;
  MfgTotalTime += SECS_FROM_1970_1996;

  TimeStampToEfiTime (MfgTotalTime, EfiTime);

}

/**
 Get EFI_TIME convert from MFG Date / Time field in Board area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[out]        MfgDateTime         The EFI_TIME struct convert from MFG Date / Time field in Board Info Area.

 @retval EFI_SUCCESS                    Get MFG date and time successfully.
 @retval EFI_UNSUPPORTED                Get MFG date and time fail.
*/
EFI_STATUS
EFIAPI
FruGetMfgDateTime (
  IN  UINT8                             FruId,
  OUT EFI_TIME                          *MfgDateTime
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Size;
  H2O_IPMI_FRU_COMMON_HEADER            Header;
  UINT8                                 Buffer[3];
  UINT16                                MfgOffset;

  //
  // Get Fru Common Header first
  //
  Size = sizeof (H2O_IPMI_FRU_COMMON_HEADER);
  Status = FruLibReadFruCommand (FruId, 0, Size, (UINT8*)(&Header));
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (Header.BoardAreaOffset!= 0x00 && Header.BoardAreaOffset != 0xFF) {
    MfgOffset = (Header.BoardAreaOffset << 3) + H2O_IPMI_FRU_BOARD_MFG_TIME_OFFSET;
    Status = FruLibReadFruCommand (
               FruId,
               MfgOffset,
               H2O_IPMI_FRU_BOARD_MFG_TIME_LEN,
               Buffer
               );

    if (!EFI_ERROR (Status)) {

      MfgTimeToEfiTime (Buffer, MfgDateTime);
      return EFI_SUCCESS;
    } else {
      return EFI_UNSUPPORTED;
    }
  } else {

    return EFI_UNSUPPORTED;
  }

}

/**
 Get specified record's offset and header.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[out]        Offset              Offset of specified record.
 @param[out]        RecordHeader        Buffer to store specified record header.

 @retval EFI_SUCCESS                    Get MFG date and time successfully.
 @retval EFI_UNSUPPORTED                Get MFG date and time fail.
*/
EFI_STATUS
FruLibGetRecordOffset (
  IN  UINT8                             FruId,
  IN  UINT8                             RecordType,
  IN  UINTN                             RecordIndex,
  OUT UINT16                            *Offset,
  OUT H2O_IPMI_FRU_RECORD_HEADER        *RecordHeader
  )
{
  H2O_IPMI_FRU_COMMON_HEADER            Header;
  H2O_IPMI_FRU_RECORD_HEADER            TempRecordHeader;
  EFI_STATUS                            Status;
  UINTN                                 RecordInstance;
  UINT16                                Size;
  UINT16                                AreaOffset;
  BOOLEAN                               Found;


  //
  // Check basic parameters
  //
  switch (RecordType) {
  case FRU_RECORD_POWER_SUPPLY_INFO:
  case FRU_RECORD_DC_OUTPUT:
  case FRU_RECORD_DC_LOAD:
  case FRU_RECORD_MANAGE_ACCESS:
  case FRU_RECORD_BASE_COMPABILITY:
  case FRU_RECORD_EXTENDED_COMPABILITY:
  case FRU_RECORD_IGNORE_RECORDTYPE:
    break;

  default:
    if (RecordType < FRU_RECORD_OEM_MIN) {
      return EFI_UNSUPPORTED;
    }
    break;
  }

  //
  // Get Fru Common Header first
  //
  Size = sizeof (H2O_IPMI_FRU_COMMON_HEADER);
  Status = FruLibReadFruCommand (FruId, 0, Size, (UINT8*)&Header);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check if this area is supported by BMC
  //
  AreaOffset = Header.MultiRecordAreaOffset;
  if (AreaOffset == 0 || AreaOffset == 0xFF) {
    return EFI_UNSUPPORTED;
  }
  AreaOffset <<= 3;

  //
  // Start to search this record
  //
  RecordInstance = 0;
  Found = FALSE;
  while (TRUE) {
    Size = sizeof (H2O_IPMI_FRU_RECORD_HEADER);
    Status = FruLibReadFruCommand (FruId, AreaOffset, Size, (UINT8*)&TempRecordHeader);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    //
    // Check type then check index
    //
    if ((TempRecordHeader.RecordId == RecordType) || (RecordType == FRU_RECORD_IGNORE_RECORDTYPE)) {
      if (RecordInstance == RecordIndex) {
        Found = TRUE;
        break;
      }
      ++RecordInstance;
    }

    //
    // Is this last one record?
    //
    if (TempRecordHeader.EndOfList) {
      break;
    }

    AreaOffset = AreaOffset + TempRecordHeader.Length + sizeof (H2O_IPMI_FRU_RECORD_HEADER);

  }

  //
  // If we find it, return data
  //
  if (Found) {
    *Offset = AreaOffset;
    CopyMem (RecordHeader, &TempRecordHeader, sizeof (H2O_IPMI_FRU_RECORD_HEADER));

    return EFI_SUCCESS;

  }

  return EFI_UNSUPPORTED;

}

/**
 Read data from specified record.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that can store read data.
                                        OUT: Real read data.
 @param[out]        DataBuffer          Buffer to store read data.
 @param[out]        DataHeader          Record header of specified multi record.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
EFIAPI
FruReadMultiRecord (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer,
  OUT    VOID                           *DataHeader OPTIONAL
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_FRU_RECORD_HEADER            RecordHeader;
  UINT16                                Offset;


  if (DataBufferSize == NULL || DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Status = FruLibGetRecordOffset (FruId, RecordType, RecordIndex, &Offset, &RecordHeader);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (*DataBufferSize < RecordHeader.Length) {
    *DataBufferSize = RecordHeader.Length;
    return EFI_BUFFER_TOO_SMALL;
  }

  Offset = Offset + sizeof (H2O_IPMI_FRU_RECORD_HEADER);
  Status = FruLibReadFruCommand (FruId, Offset, (UINT16)RecordHeader.Length, DataBuffer);
  if (!EFI_ERROR (Status)) {
    *DataBufferSize = RecordHeader.Length;
    if (DataHeader != NULL) {
      CopyMem (DataHeader, &RecordHeader, sizeof (H2O_IPMI_FRU_RECORD_HEADER));
    }
  }

  return Status;

}

/**
 Write data to specified record.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write;
                                        OUT: Data size that can be written.
 @param[out]        DataBuffer          Buffer to store write data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Record buffer is smaller than write data.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
EFIAPI
FruWriteMultiRecord (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_FRU_RECORD_HEADER            RecordHeader;
  UINT16                                HeaderOffset;
  UINT16                                DataOffset;
  UINTN                                 Index;
  UINT8                                 *Buffer;
  UINT8                                 *Temp;
  UINT8                                 Checksum;


  //
  // Check parameters
  //
  if (DataBufferSize == NULL || DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get specified record and instance
  //
  Status = FruLibGetRecordOffset (FruId, RecordType, RecordIndex, &HeaderOffset, &RecordHeader);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (*DataBufferSize > RecordHeader.Length) {
    *DataBufferSize = RecordHeader.Length;
    return EFI_BUFFER_TOO_SMALL;
  }

  DataOffset = HeaderOffset + sizeof (H2O_IPMI_FRU_RECORD_HEADER);
  Status = FruLibWriteFruCommand (FruId, DataOffset, (UINT16)*DataBufferSize, DataBuffer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Buffer = (UINT8*)AllocateZeroPool ((UINTN)RecordHeader.Length);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FruLibReadFruCommand (FruId, DataOffset, (UINT16)RecordHeader.Length, Buffer);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return EFI_UNSUPPORTED;
  }

  Checksum = 0;
  for (Index = 0; Index < RecordHeader.Length; ++Index) {
    Checksum = Checksum + Buffer[Index];
  }

  RecordHeader.RecordChecksum = 0 - Checksum;

  Checksum = 0;
  Temp = (UINT8*)&RecordHeader;
  for (Index = 0; Index < (sizeof (H2O_IPMI_FRU_RECORD_HEADER) - 1); ++Index) {
    Checksum = Checksum + Temp[Index];
  }

  RecordHeader.HeaderChecksum = 0 - Checksum;

  FruLibWriteFruCommand (FruId, HeaderOffset, sizeof (H2O_IPMI_FRU_RECORD_HEADER), (UINT8*)&RecordHeader);
  FreePool (Buffer);

  return EFI_SUCCESS;

}

/**
 Get FRU size of specified FRU.

 @param[in]         FruId               FRU ID that want to get size.

 @retval The size of specified FRU. 0xFFFF means cannot get size.
*/
UINT16
FruLibGetFruSize (
  IN  UINT8                             FruId
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvDataSize;
  UINT8                                 RecvData[0x20];

  //
  // If we don't have the same FRU size or we never get this FRU size, get it.
  //
  if (!(mCurrentFruId == FruId && mAlreadyGet)) {
    mCurrentFruId = FruId;
    mAlreadyGet = TRUE;

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_STORAGE,
               H2O_IPMI_CMD_GET_FRU_INVENTORY_AREA_INFO,
               &FruId,
               sizeof (UINT8),
               RecvData,
               &RecvDataSize
               );

    if (!EFI_ERROR (Status)) {
      mCurrentFruSize = *(UINT16*)RecvData;
    } else {
      mCurrentFruSize = 0xFFFF;
    }
  }

  return mCurrentFruSize;

}

/**
 Read specified size of raw data from specified offset.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Offset              Offset of FRU that user wants to read from.
 @param[in, out]    DataBufferSize      IN:  Data buffer size. It also means data size that want to read.
                                        OUT: FRU data size that can be read from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
EFIAPI
FruReadRawData (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN OUT UINT16                         *DataBufferSize,
  OUT    VOID                           *DataBuffer
  )
{
  UINT16                                FruSize;


  if (DataBufferSize == NULL || DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  FruSize = FruLibGetFruSize (FruId);

  //
  // Offset must be smaller then total FRU size
  //
  if (Offset > FruSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Cannot read over total FRU size
  //
  if ((UINT32)(Offset + *DataBufferSize) > (UINT32)FruSize) {
    *DataBufferSize = FruSize - Offset;
    return EFI_BUFFER_TOO_SMALL;
  }

  return FruLibReadFruCommand (FruId, Offset, *DataBufferSize, DataBuffer);

}

/**
 Write specified size of raw data to specified offset.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Offset              Offset of FRU that user wants to write to.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write.
                                        OUT: FRU data size that can be written from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
EFIAPI
FruWriteRawData (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN OUT UINT16                         *DataBufferSize,
  IN     VOID                           *DataBuffer
  )
{
  UINT16                                FruSize;


  if (DataBufferSize == NULL || DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  FruSize = FruLibGetFruSize (FruId);

  //
  // Offset must be smaller then total FRU size
  //
  if (Offset > FruSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Cannot read over total FRU size
  //
  if ((UINT32)(Offset + *DataBufferSize) > (UINT32)FruSize) {
    *DataBufferSize = FruSize - Offset;
    return EFI_BUFFER_TOO_SMALL;
  }

  return FruLibWriteFruCommand (FruId, Offset, *DataBufferSize, DataBuffer);
}

/**
 Fill private data content

*/
VOID
InitialIpmiFruContent (
  VOID
  )
{

  //
  // Initial Context Structure
  //
  mFruProtocol.ReadFieldData    = FruReadFieldData;
  mFruProtocol.WriteFieldData   = FruWriteFieldData;
  mFruProtocol.GetChassisType   = FruGetChassisType;
  mFruProtocol.GetMfgDateTime   = FruGetMfgDateTime;
  mFruProtocol.ReadMultiRecord  = FruReadMultiRecord;
  mFruProtocol.WriteMultiRecord = FruWriteMultiRecord;
  mFruProtocol.ReadRawData      = FruReadRawData;
  mFruProtocol.WriteRawData     = FruWriteRawData;

}

