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

  H2OIpmiSelDataProtocol.h

Abstract:

  H2O IPMI SelData Protocol definition file

--*/

#ifndef _H2O_IPMI_SEL_Data_PROTOCOL_H_
#define _H2O_IPMI_SEL_Data_PROTOCOL_H_
#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiInterfaceProtocol)

EFI_FORWARD_DECLARATION (H2O_IPMI_SEL_DATA_PROTOCOL);

//
// Protocol Guid
//
#define H2O_IPMI_SEL_DATA_PROTOCOL_GUID \
  {0x27cf90da, 0x36d4, 0x43c0, 0xaf, 0xbb, 0x56, 0x1b, 0xf0, 0xad, 0xf6, 0x86}

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_DATA) (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL    *This,
  IN  UINT16                        SelId,
  OUT H2O_IPMI_SEL_ENTRY            *SelData
);
/*++

Routine Description:

  Get SEL entry raw data from "Get SEL Entry" IPMI Command.

Arguments:

  This      - H2O_IPMI_SEL_INFO_PROTOCOL pointer.

  SelId     - SEL Id to be got.

  SelInfo   - Structure contains SEL Raw Data. Caller need to provide buffer to store.
              
Returns:

  EFI_INVALID_PARAMETER    - If one of arguments is NULL pointer.

  EFI_UNSUPPORTED          - If SelEntry is OEM SEL format or it contains OEM data.

  EFI_SUCCESS              - Convert success.
  
--*/

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_ALL_DATA) (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL    *This,
  OUT UINT16                        *SelCount,
  OUT H2O_IPMI_SEL_ENTRY            **SelData
);
/*++

Routine Description:

  Get ALL SEL entry raw data from "Get SEL Entry" IPMI Command.

Arguments:

  This      - H2O_IPMI_SEL_INFO_PROTOCOL pointer.

  SelCount  - Count all SEL Entries.

  SelInfo   - Structure Array contains SEL Raw Data. This is dynamic allocated memory, 
              caller need to free it.
              
Returns:

  EFI_INVALID_PARAMETER    - If one of arguments is NULL pointer.

  EFI_UNSUPPORTED          - If SelEntry is OEM SEL format or it contains OEM data.

  EFI_SUCCESS              - Convert success.
  
--*/


//
// Protocol Structure
//
typedef struct _H2O_IPMI_SEL_DATA_PROTOCOL {
  H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_DATA     GetSelData;
  H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_ALL_DATA GetAllSelData;
} H2O_IPMI_SEL_DATA_PROTOCOL;

extern EFI_GUID gH2OIpmiSelDataProtocolGuid;

#endif


