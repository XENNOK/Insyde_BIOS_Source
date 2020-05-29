/** @file
 Multi Config Base library header file.

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


#ifndef _MULTI_CONFIG_BASE_LIB_H_
#define _MULTI_CONFIG_BASE_LIB_H_


#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <MultiConfigStructDef.h>


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
  );


/**
 Returns the length of Header (include MULTI_CONFIG_REGION_HEADER and all CONFIG_HEADER).

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @return The length of Header.
*/
UINTN
GetHeaderLenghth (
  VOID                                  *Binary
  );


/**
 Returns the CONFIG_HEADER point of ranking order.

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @return The CONFIG_HEADER point of ranking order.
*/
CONFIG_HEADER*
GetConfigHeaderByOrder (
  IN  UINT16                            Order
  );


/**
 Returns the Config Data point of ranking order.

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @return The Config Data point of ranking order.
*/
VOID*
GetConfigDataByOrder (
  IN  UINT16                            Order,
  IN  VOID                              *Binary
  );


/**
 Returns the Config Count.

 @return The Config Count.
*/
UINT16
GetConfigCount (
  VOID
  );


/**
 Get the specific Config Header and Order, order number is 0, 1, 2, 3,......

 @param[in]         Type                Config Data Type.
 @param[in]         ConfigId           Config Data Identity Number.
 @param[in]         Binary              Pointer to Multi Config Region Raw Data.
 @param[out]      ConfigHeader    Pointer to specific Config Header search by Type & ID.
 @param[out]      Order               Pointer to a UINT16 value indicate specific Config Order.

 @retval EFI_SUCCESS                    Get the specific Config Header and Order success.
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
  );


/**
 Check CRC32 for Multi Config Header.

 @param[in]         Binary              Pointer to Multi Config Region Raw Data.

 @retval EFI_SUCCESS                    Check CRC32 correct.
 @retval EFI_CRC_ERROR                  Check CRC32 error.
*/
EFI_STATUS
CheckMultiConfigHeaderCrc32 (
  IN  VOID                              *Binary
  );


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
  );


/**
 Find the specific Config Header and Order, order number is 0, 1, 2, 3,......
 This specific Config Data is Setup setting use for BIOS POST or Load Default.

 @param[in]           RequireKind           Find Setup Setting for (1: BIOS POST) or (2: Load Default).
 @param[in, out]    ConfigOrder           Pointer to a UINT16 value indicate specific Config Order.
 @param[in]           Binary                  Pointer to Multi Config Region Raw Data .
 @param[out]        ConfigHeader        Pointer to specific Config Header.

 @retval EFI_SUCCESS                    Get the specific Config Header and Order success.
 @retval EFI_NOT_FOUND                  Can't Find the specific Config Header.
 @retval EFI_INVALID_PARAMETER          Binary or ConfigHeader or ConfigOrder is NULL.
*/
EFI_STATUS
FindFullSetupSetting (
  IN CONST  UINT8                             RequireKind,
  IN OUT    UINT16                            *ConfigOrder,
  IN        VOID                              *Binary,
  OUT       CONFIG_HEADER                     **ConfigHeader
  );


/**
 Get the Active or Default Full Setup Setting Config Data, identify by Config Type & Attribute.

 @param[in]           RequireKind         Find Setup Setting for SETUP_FOR_BIOS_POST or SETUP_FOR_LOAD_DEFAULT.
 @param[in,out]	    ConfigOrder         Pointer to a UINT16 value indicate specific Config Order.
 @param[in]           DataBufferSize     Full Setup setting data size.
 @param[out]	        DataBuffer           Buffer to store the data.
 @param[out]	        ConfigId               Point to UINT8 for carrying Config Identity Number.
 @param[out]        Attribute               Point to UINT32 for carrying Config Attribute.

 @retval EFI_SUCCESS                    Get the specific Config Data success.
 @retval EFI_INVALID_PARAMETER   This or DataBuffer is NULL.
 @retval EFI_NOT_FOUND                Verify the signature error or can't find the specific Config Data.
 @retval EFI_UNSUPPORTED             The flash device is not supported.
 @retval EFI_CRC_ERROR                Check Multi Config Header or Data CRC32 error.
 @retval EFI_BAD_BUFFER_SIZE      Data Buffer size is not match with Config Data size.
*/
EFI_STATUS
GetFullSetupSetting (
  IN CONST  UINT8                             RequireKind,
  IN OUT    UINT16                            *ConfigOrder,
  IN        UINTN                             DataBufferSize,
  OUT       VOID                              **DataBuffer,
  OUT       UINT8                             *ConfigId OPTIONAL, 
  OUT       UINT32                            *Attribute OPTIONAL
  );


#endif

