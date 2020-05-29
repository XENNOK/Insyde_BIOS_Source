/** @file
 DXE IPMI Device Manager Config VFR library header file.

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


#ifndef _DXE_IPMI_DM_CONFIG_VFR_LIB_H_
#define _DXE_IPMI_DM_CONFIG_VFR_LIB_H_


#include <PiDxe.h>

#include <H2OIpmi.h>

#include <Guid/H2OIpmiHob.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>


//
// Define a unique string ID to indicate the end of string function table.
//
#define STRING_FUNCTION_TABLE_END       0xFFFF

#define IPMI_CONFIG_TEMP_STR_LEN        0x100


/**
 UPDATE_STRING_FUNCTION function point prototype.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_SUCCESS                    Update string successful.
 @return EFI_ERROR                      Update string fail.
*/
typedef
EFI_STATUS
(EFIAPI *UPDATE_STRING_FUNCTION) (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 GET_STRING_FUNCTION function point prototype.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
typedef
EFI_STATUS
(EFIAPI *GET_STRING_FUNCTION) (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


#pragma pack (1)

typedef struct {
  UINT16                     StringToken;
  UPDATE_STRING_FUNCTION     UpdateString;
  GET_STRING_FUNCTION        GetString;
} IPMI_DM_STRING_FUNCTION_TABLE;

#pragma pack ()


extern IPMI_DM_STRING_FUNCTION_TABLE    gIpmiDmStringFunction[];


#endif

