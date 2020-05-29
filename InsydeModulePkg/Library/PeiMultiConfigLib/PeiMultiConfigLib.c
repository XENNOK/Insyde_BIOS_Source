/** @file
 Multi Config Base library implementation code.

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


#include <Library/MultiConfigBaseLib.h>

//
// Multi Config Region Base
//
#define FLASH_REGION_MULTI_CONFIG_BASE  PcdGet32(PcdFlashNvStorageMultiConfigBase)

//
// Multi Config Region Size
//
#define FLASH_REGION_MULTI_CONFIG_SIZE  PcdGet32(PcdFlashNvStorageMultiConfigSize)

/**
  This internal function reverses bits for 32bit data.

  @param  Value                 The data to be reversed.

  @return                       Data reversed.
**/
UINT32
ReverseBits (
  IN  UINT32  Value
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
  OUT UINT32  *CrcTable
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

    CrcTable[TableEntry] = ReverseBits (Value);
  }
}


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
CalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;
  UINT32  CrcTable[256] = {0};

  if (CrcTable[0] == 0) {
    InitializeCrc32Table(CrcTable);
  }

  if (Data == NULL || DataSize == 0 || CrcOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0xffffffff;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ CrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ 0xffffffff;
  return EFI_SUCCESS;
}


/**
 Returns the length of Header (including MULTI_CONFIG_REGION_HEADER and all CONFIG_HEADERs).

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @return The length of Header.
*/
UINTN
GetHeaderLenghth (
  IN  VOID                                  *Binary
  )
{
  MULTI_CONFIG_REGION_HEADER            *MultiConfigHdr;

  MultiConfigHdr = (MULTI_CONFIG_REGION_HEADER*)Binary;

  return (sizeof (MULTI_CONFIG_REGION_HEADER) + (sizeof (CONFIG_HEADER) * MultiConfigHdr->ConfigCount));
}


/**
 Returns the CONFIG_HEADER point of ranking order.

 @param[in]         Order              Pointer to a UINT16 value indicate specific Config Order.

 @return The CONFIG_HEADER point of ranking order.
*/
CONFIG_HEADER*
GetConfigHeaderByOrder (
  IN  UINT16                            Order
  )
{
  VOID                                  *MultiConfigRegionBase;

  MultiConfigRegionBase = (VOID*)(UINTN)FLASH_REGION_MULTI_CONFIG_BASE;

  return (CONFIG_HEADER*)((UINT8*)MultiConfigRegionBase + (sizeof (MULTI_CONFIG_REGION_HEADER) + sizeof (CONFIG_HEADER) * Order));
}


/**
 Returns the Config Data point of ranking order.

 @param[in]         Order              Pointer to a UINT16 value indicate specific Config Order.
 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @return The Config Data point of ranking order.
*/
VOID*
GetConfigDataByOrder (
  IN  UINT16                            Order,
  IN  VOID                              *Binary
  )
{
  UINTN                                 HeaderLenght;
  UINTN                                 DataOffset;
  CONFIG_HEADER                         *ConfigHeader;
  UINT16                                Index;

  HeaderLenght = GetHeaderLenghth (Binary);

  ConfigHeader = (CONFIG_HEADER *)((MULTI_CONFIG_REGION_HEADER *)Binary + 1);

  DataOffset = 0;

  for (Index = 0; Index < Order; Index++) {
    DataOffset += ConfigHeader->ConfigDataSize;
    ConfigHeader += 1;
  }

  return (VOID*)((UINT8*)Binary + (HeaderLenght + DataOffset));
}


/**
 Returns the Config Count.

 @return The Config Count.
*/
UINT16
GetConfigCount (
  VOID
  )
{
  VOID                                  *MultiConfigRegionBase;

  MultiConfigRegionBase = (VOID*)(UINTN)FLASH_REGION_MULTI_CONFIG_BASE;

  return (UINT16)((MULTI_CONFIG_REGION_HEADER*)(MultiConfigRegionBase))->ConfigCount;
}


/**
 Get the specific Config Header and Order, order number is 0, 1, 2, 3,......

 @param[in]         Type                Config Data Type.
 @param[in]         ConfigId            Config Data Identity Number.
 @param[in]         Binary              Pointer  to Multi Config Region Raw Data.
 @param[out]        ConfigHeader        Pointer  to specific Config Header search by Type & ID.
 @param[out]        Order               Pointer  to a UINT16 value indicate specific Config Order.

 @retval EFI_SUCCESS                    Configuration header found.
 @retval EFI_NOT_FOUND                  Can't Find the specific Config Header.
 @retval EFI_INVALID_PARAMETER          Binary is NULL.
*/
EFI_STATUS
GetConfigHeaderOrder (
  IN  UINT8                             Type,
  IN  UINT8                             ConfigId,
  IN  VOID                              *Binary,
  OUT CONFIG_HEADER                     **ConfigHeader, OPTIONAL
  OUT UINT16                            *ConfigOrder OPTIONAL
  )
{
  EFI_STATUS                            Status;
  CONFIG_HEADER                         *TempConfigHeader;
  UINT16                                ConfigCount;
  UINT16                                Index;

  if (Binary == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempConfigHeader = GetConfigHeaderByOrder (0);
  ConfigCount = GetConfigCount ();

  Status = EFI_NOT_FOUND;

  for (Index = 0; Index < ConfigCount; Index++) {
    if ((TempConfigHeader->Type == Type) && (TempConfigHeader->ConfigId == ConfigId)) {
      Status = EFI_SUCCESS;
      if (ConfigHeader != NULL) {
        *ConfigHeader = TempConfigHeader;
      }
      if (ConfigOrder != NULL) {
        *ConfigOrder = Index;
      }
      break;
    }
    TempConfigHeader += 1;
  }

  return Status;
}


/**
 Check CRC32 for Multi Config Header.

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @retval EFI_SUCCESS                    Check CRC32 correct.
 @retval EFI_CRC_ERROR                  Check CRC32 error.
*/
EFI_STATUS
CheckMultiConfigHeaderCrc32 (
  IN  VOID                              *Binary
  )
{
  EFI_STATUS                            Status;
  MULTI_CONFIG_REGION_HEADER            *MultiConfigHeader;
  UINT32                                TempCrc32;

  MultiConfigHeader = (MULTI_CONFIG_REGION_HEADER*)Binary;

  //
  // Save the original HeaderCrc32 value.
  //
  TempCrc32 = MultiConfigHeader->HeaderCrc32;
  MultiConfigHeader->HeaderCrc32 = 0;

  CalculateCrc32 (MultiConfigHeader, MultiConfigHeader->HeaderLength, &MultiConfigHeader->HeaderCrc32);
  if (MultiConfigHeader->HeaderCrc32 == TempCrc32) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_CRC_ERROR;
    //
    // Recovery the original HeaderCrc32 value.
    //
    MultiConfigHeader->HeaderCrc32 = TempCrc32;
  }

  return Status;
}


/**
 Check CRC32 for Multi Config Data.

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @retval EFI_SUCCESS                    Check CRC32 correct.
 @retval EFI_CRC_ERROR                  Check CRC32 error.
*/
EFI_STATUS
CheckMultiConfigDataCrc32 (
  IN  UINT32                            Crc32,
  IN  UINT16                            ConfigDataSize,
  IN  VOID                              *ConfigData
  )
{
  EFI_STATUS                            Status;
  UINT32                                TempCrc32;

  CalculateCrc32 (ConfigData, ConfigDataSize, &TempCrc32);
  if (TempCrc32 == Crc32) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_CRC_ERROR;
  }

  return Status;
}


/**
 Check Signature & Header CRC32 for Multi Config Region.

 @param[in]         MultiConfigRegion   Pointer to Multi Config Region Raw Data.

 @retval EFI_SUCCESS                    Check Signature & Header CRC32 success.
 @retval EFI_NOT_FOUND                  Verify the signature error.
 @retval EFI_CRC_ERROR                  Check Multi config Header CRC32 error.
*/
EFI_STATUS
CheckSignatureAndHeaderCrc (
  VOID                                  *MultiConfigRegion
  )
{
  EFI_STATUS                            Status;
  UINT64                                Signature;
  MULTI_CONFIG_REGION_HEADER            *RegionHeader;
  VOID                                  *TempRegionHeader;

  //
  // Verify Signature
  //
  Signature = MULTI_CONFIG_SIGNATURE;
  if (CompareMem (&Signature, MultiConfigRegion, sizeof (UINT64)) != 0) {
    return EFI_NOT_FOUND;
  }

  RegionHeader = (MULTI_CONFIG_REGION_HEADER*)MultiConfigRegion;

  TempRegionHeader = AllocateZeroPool (RegionHeader->HeaderLength);
  if (TempRegionHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  CopyMem (TempRegionHeader, MultiConfigRegion, RegionHeader->HeaderLength);

  //
  // Check CRC32 for Multi Config Header
  //
  Status = CheckMultiConfigHeaderCrc32 (TempRegionHeader);
  if (EFI_ERROR (Status)) {
    return EFI_CRC_ERROR;
  }

  return EFI_SUCCESS;
}


/**
 Find the specific Config Header and Order, order number is 0, 1, 2, 3,......
 This specific Config Data is Setup setting use for BIOS POST or Load Default.

 @param[in]         RequireKind         Find Setup Setting for (1: BIOS POST) or (2: Load Default).
 @param[in,out]   ConfigOrder     Pointer to a UINT16 value indicate specific Config Order.
 @param[in]         Binary              Pointer to Multi Config Region Raw Data.
 @param[out]      ConfigHeader        Point to specific Config Header.

 @retval EFI_SUCCESS                    Get the specific Config Header and Order success.
 @retval EFI_NOT_FOUND                  Can't Find the specific Config Header.
 @retval EFI_INVALID_PARAMETER          Binary or ConfigHeader is NULL.
*/
EFI_STATUS
FindFullSetupSetting (
  IN CONST  UINT8                             RequireKind,
  IN OUT    UINT16                            *ConfigOrder,
  IN        VOID                              *Binary,
  OUT       CONFIG_HEADER                     **ConfigHeader
  )
{
  EFI_STATUS                            Status;
  CONFIG_HEADER                         *TempConfigHeader;
  UINT16                                ConfigCount;
  UINT16                                Index;

  if (Binary == NULL || ConfigHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempConfigHeader = GetConfigHeaderByOrder (*ConfigOrder);
  ConfigCount = GetConfigCount ();

  Status = EFI_NOT_FOUND;

  for (Index = *ConfigOrder; Index < ConfigCount; Index++) {
    //
    // Config Type is Full Setup Setting & Attribute is VALID
    //
    if ((TempConfigHeader->Type == MULTI_CONFIG_TYPE_FULL_SETUP_SETTING_VALUE) && \
        ((TempConfigHeader->Attribute & MULTI_CONFIG_ATTRIBUTE_VALID) == MULTI_CONFIG_ATTRIBUTE_VALID)) {
      //
      // Full Setup Setting for BIOS POST or Load Default
      //
      if (RequireKind == SETUP_FOR_BIOS_POST) {
        //
        // Config attribute is ACTIVE | BIOS_POST
        //
        if ((TempConfigHeader->Attribute & 0x40000000) == 0x00000000) {
          Status = EFI_SUCCESS;
          *ConfigHeader = TempConfigHeader;
          *ConfigOrder = Index;
          break;
        }
      } else {
        //
        // Config attribute is ACTIVE | DEFAULT
        //
        if ((TempConfigHeader->Attribute & 0x20000000) == 0x20000000) {
          Status = EFI_SUCCESS;
          *ConfigHeader = TempConfigHeader;
          *ConfigOrder = Index;
          break;
        }
      }
    }
    TempConfigHeader += 1;
  }
  return Status;
}


/**
 Get the Active or Default Full Setup Setting Config Data, identify by Config Type & Attribute.

 @param[in]         RequireKind         Find Setup Setting for SETUP_FOR_BIOS_POST or SETUP_FOR_LOAD_DEFAULT.
 @param[in,out]   ConfigOrder         Pointer to a UINT16 value indicate specific Config Order.
 @param[in]         DataBufferSize      Full Setup setting data size.
 @param[out]        DataBuffer          Buffer to store the data.
 @param[out]        ConfigId            Pointer to UINT8 for carrying Config Identity Number.
 @param[out]        Attribute           Pointer to UINT32 for carrying Config Attribute.

 @retval EFI_SUCCESS                    Get the specific Config Data success.
 @retval EFI_INVALID_PARAMETER          DataBuffer is NULL.
 @retval EFI_NOT_FOUND                  Verify the signature error or can't find the specific Config Data.
 @retval EFI_UNSUPPORTED                The flash device is not supported.
 @retval EFI_CRC_ERROR                  Check Multi Config Header or Data CRC32 error.
 @retval EFI_BAD_BUFFER_SIZE            Data Buffer size is not match with Config Data size.
*/
EFI_STATUS
GetFullSetupSetting (
  IN CONST  UINT8                             RequireKind,
  IN OUT    UINT16                            *ConfigOrder,
  IN        UINTN                             DataBufferSize,
  OUT       VOID                              **DataBuffer,
  OUT       UINT8                             *ConfigId OPTIONAL, 
  OUT       UINT32                            *Attribute OPTIONAL
  )
{
  EFI_STATUS                            Status;
  VOID                                  *MultiConfigRegionBase;
  CONFIG_HEADER                         *TempHeader;
  VOID                                  *TempConfigData;
  
  if (DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MultiConfigRegionBase = (VOID*)(UINTN)FLASH_REGION_MULTI_CONFIG_BASE;

  //
  // Check Signature & Header CRC32 for Multi Config Region.
  //
  Status = CheckSignatureAndHeaderCrc (MultiConfigRegionBase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Search the specific Config Header
  //
  TempHeader = NULL;
  Status = FindFullSetupSetting (RequireKind, ConfigOrder, MultiConfigRegionBase, &TempHeader);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Check CRC32 for Multi Config Data
  // Direct get Config Data by Config Data Offset
  //
  TempConfigData = (UINT8*)MultiConfigRegionBase + TempHeader->ConfigDataOffset;
  Status = CheckMultiConfigDataCrc32 (TempHeader->Crc32, TempHeader->ConfigDataSize, TempConfigData);
  if (EFI_ERROR (Status)) {
    //
    // Get Config Data by Config Order
    //
    TempConfigData = GetConfigDataByOrder (*ConfigOrder, MultiConfigRegionBase);
    Status = CheckMultiConfigDataCrc32 (TempHeader->Crc32, TempHeader->ConfigDataSize, TempConfigData);
    if (EFI_ERROR (Status)) {
      return EFI_CRC_ERROR;
    }
  }

  *DataBuffer = TempConfigData;

  if (ConfigId != NULL) {
    *ConfigId = TempHeader->ConfigId;
  }
  if (Attribute != NULL) {
    *Attribute = TempHeader->Attribute;
  }

  return EFI_SUCCESS;
}


