/** @file
 H2O IPMI SEL Data Protocol header file.

 H2O IPMI SEL Data Protocol provides an convenience way to get system event log
 from BMC.

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

#ifndef _H2O_IPMI_SEL_DATA_PROTOCOL_H_
#define _H2O_IPMI_SEL_DATA_PROTOCOL_H_


#include <H2OIpmi.h>


//
// Protocol Guid
//
#define H2O_IPMI_SEL_DATA_PROTOCOL_GUID \
  {0x27cf90da, 0x36d4, 0x43c0, 0xaf, 0xbb, 0x56, 0x1b, 0xf0, 0xad, 0xf6, 0x86}

#define H2O_SMM_IPMI_SEL_DATA_PROTOCOL_GUID \
  {0xCFCDB243, 0x4B9F, 0x42c2, 0x9D, 0x02, 0x19, 0x67, 0xCF, 0xC8, 0x2F, 0x82}

typedef struct _H2O_IPMI_SEL_DATA_PROTOCOL H2O_IPMI_SEL_DATA_PROTOCOL;


/**
 Get SEL entry raw data from "Get SEL Entry" IPMI Command.

 @param[in]         This                H2O_IPMI_SEL_INFO_PROTOCOL pointer.
 @param[in]         SelId               SEL Id to be got.
 @param[out]        SelData             Structure contains SEL Raw Data.
                                        Caller need to provide buffer to store.

 @retval EFI_INVALID_PARAMETER          If one of arguments is NULL pointer.
 @retval EFI_UNSUPPORTED                If SelEntry is OEM SEL format or it contains OEM data.
 @retval EFI_SUCCESS                    Convert success.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_DATA) (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL        *This,
  IN  UINT16                            SelId,
  OUT H2O_IPMI_SEL_ENTRY                *SelData
);


/**
 Get SEL entry raw data from "Get SEL Entry" IPMI Command.

 @param[in]         This                H2O_IPMI_SEL_INFO_PROTOCOL pointer.
 @param[out]        SelCount            Number of total SEl entries.
 @param[out]        SelData             Structure Array contains SEL Raw Data.
                                        This is dynamic allocated memory, caller need to free it.

 @retval EFI_INVALID_PARAMETER          If one of arguments is NULL pointer.
 @retval EFI_UNSUPPORTED                If SelEntry is OEM SEL format or it contains OEM data.
 @retval EFI_SUCCESS                    Convert success.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_ALL_DATA) (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL        *This,
  OUT UINT16                            *SelCount,
  OUT H2O_IPMI_SEL_ENTRY                **SelData
);


//
// Protocol Structure
//
struct _H2O_IPMI_SEL_DATA_PROTOCOL {
  H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_DATA     GetSelData;
  H2O_IPMI_SEL_DATA_PROTOCOL_GET_SEL_ALL_DATA GetAllSelData;
};


extern EFI_GUID gH2OIpmiSelDataProtocolGuid;
extern EFI_GUID gH2OSmmIpmiSelDataProtocolGuid;


#endif

