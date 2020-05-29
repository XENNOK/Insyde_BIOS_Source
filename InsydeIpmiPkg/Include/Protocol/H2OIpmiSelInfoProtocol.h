/** @file
 H2O IPMI SEL Info Protocol header file.

 H2O IPMI SEL Info Protocol provides the function to translate system event log
 data to human readable strings if the system event log is defined in IPMI
 specification.

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

#ifndef _H2O_IPMI_SEL_INFO_PROTOCOL_H_
#define _H2O_IPMI_SEL_INFO_PROTOCOL_H_


#include <H2OIpmi.h>


//
// Protocol Guid
//
#define H2O_IPMI_SEL_INFO_PROTOCOL_GUID \
  {0x86e61ac4, 0x75f9, 0x47bc, 0x86, 0xf7, 0xa7, 0x59, 0xb8, 0xa9, 0xab, 0x6e}

#define H2O_SMM_IPMI_SEL_INFO_PROTOCOL_GUID \
  {0x36E31F07, 0xBCDA, 0x4061, 0x8E, 0xFD, 0x1A, 0xD6, 0x95, 0xE2, 0xA0, 0xE0}

typedef struct _H2O_IPMI_SEL_INFO_PROTOCOL H2O_IPMI_SEL_INFO_PROTOCOL;

#define SEL_INFO_STRING_LEN        64
#define SEL_INFO_DESC_STRING_LEN   256

typedef struct {
  CHAR16    SensorName[SEL_INFO_STRING_LEN];
  CHAR16    Type[SEL_INFO_STRING_LEN];
  CHAR16    Desc[SEL_INFO_DESC_STRING_LEN];
  CHAR16    Generator[SEL_INFO_STRING_LEN];
  CHAR16    IdType[SEL_INFO_STRING_LEN];
  CHAR16    Lun[SEL_INFO_STRING_LEN];
} SEL_INFO_STRUCT;


/**
 Convert SEL entry to string based SEL entry read from "Get SEL Entry" IPMI Command.

 @param[in]         This                H2O_IPMI_SEL_INFO_PROTOCOL pointer.
 @param[in]         SelEntry            SEL entry to be coverted.
 @param[out]        SelInfo             Structure contains SEL entry strings.
                                        This is not dynamic allocated memory,
                                        caller does not need to free it.

 @retval EFI_INVALID_PARAMETER          If one of arguments is NULL pointer.
 @retval EFI_UNSUPPORTED                If SelEntry is OEM SEL format or it contains OEM data.
 @retval EFI_SUCCESS                    Convert success.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_INFO_PROTOCOL_GET_SEL_INFO) (
  IN  H2O_IPMI_SEL_INFO_PROTOCOL        *This,
  IN  VOID                              *SelEntry,
  OUT SEL_INFO_STRUCT                   *SelInfo
);


//
// Protocol Structure
//
struct _H2O_IPMI_SEL_INFO_PROTOCOL {
  H2O_IPMI_SEL_INFO_PROTOCOL_GET_SEL_INFO   GetSelInfo;
};


extern EFI_GUID gH2OIpmiSelInfoProtocolGuid;
extern EFI_GUID gH2OSmmIpmiSelInfoProtocolGuid;


#endif

