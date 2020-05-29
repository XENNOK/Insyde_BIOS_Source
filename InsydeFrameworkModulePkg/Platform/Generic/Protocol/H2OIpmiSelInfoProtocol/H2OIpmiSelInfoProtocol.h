//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*--
Module Name:

  H2OIpmiSelInfoProtocol.h

Abstract:

  H2O IPMI SelInfo Protocol definition file

--*/

#ifndef _H2O_IPMI_SEL_INFO_PROTOCOL_H_
#define _H2O_IPMI_SEL_INFO_PROTOCOL_H_


#include "Tiano.h"



EFI_FORWARD_DECLARATION (H2O_IPMI_SEL_INFO_PROTOCOL);



//
// Protocol Guid
//
#define H2O_IPMI_SEL_INFO_PROTOCOL_GUID \
  {0x86e61ac4, 0x75f9, 0x47bc, 0x86, 0xf7, 0xa7, 0x59, 0xb8, 0xa9, 0xab, 0x6e}


#define SEL_INFO_STRING_LEN        64
#define SEL_INFO_DESC_STRING_LEN   256


typedef struct {
  CHAR16    SensorName[SEL_INFO_STRING_LEN];
  CHAR16    Type[SEL_INFO_STRING_LEN];
  CHAR16    Desc[SEL_INFO_DESC_STRING_LEN];
  CHAR16    Generator[SEL_INFO_STRING_LEN];
  CHAR16    IDType[SEL_INFO_STRING_LEN];
  CHAR16    LUN[SEL_INFO_STRING_LEN];
} SEL_INFO_STRUCT;



typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_INFO_PROTOCOL_GET_SEL_INFO) (
  IN  H2O_IPMI_SEL_INFO_PROTOCOL   *This,
  IN  VOID                         *SelEntry,
  OUT SEL_INFO_STRUCT              *SelInfo
);
/*++

Routine Description:

  Convert SEL entry to string based SEL entry read from "Get SEL Entry" IPMI Command.

Arguments:

  This      - H2O_IPMI_SEL_INFO_PROTOCOL pointer.

  SelEntry  - SEL entry to be coverted.

  SelInfo   - Structure contains SEL entry strings. This is not dynamic allocated
              memory, caller does not need to free it.
              
Returns:

  EFI_INVALID_PARAMETER    - If one of arguments is NULL pointer.

  EFI_UNSUPPORTED          - If SelEntry is OEM SEL format or it contains OEM data.

  EFI_SUCCESS              - Convert success.
  
--*/


//
// Protocol Structure
//
typedef struct _H2O_IPMI_SEL_INFO_PROTOCOL {
  H2O_IPMI_SEL_INFO_PROTOCOL_GET_SEL_INFO      GetSelInfo;
} H2O_IPMI_SEL_INFO_PROTOCOL;

extern EFI_GUID gH2OIpmiSelInfoProtocolGuid;

#endif


