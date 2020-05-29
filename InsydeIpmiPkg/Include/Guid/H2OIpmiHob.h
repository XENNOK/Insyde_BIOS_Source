/** @file
 GUID and structure definition of H2O IPMI hob.

 This file contains H2O IPMI hob GUID and strcture definitions. H2O IPMI hob
 passes basic BMC/IPMI related information from PEI phase to DXE phase.

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

#ifndef _H2O_IPMI_HOB_H_
#define _H2O_IPMI_HOB_H_


#include <H2OIpmi.h>


#define H2O_IPMI_HOB_GUID \
  {0x545869d9, 0xf49f, 0x4ddf, 0x81, 0xfd, 0x92, 0x7a, 0xb2, 0x7e, 0xe5, 0x9d}

typedef struct _H2O_IPMI_HOB H2O_IPMI_HOB;


#pragma pack(1)

//
// This is the definition of the HOB structure.
//
struct _H2O_IPMI_HOB {
  UINT8                           BmcStatus;
  UINT8                           InterfaceType;
  UINT8                           InterfaceAddressType;
  H2O_IPMI_INTERFACE_ADDRESS      InterfaceAddress[H2O_IPMI_MAX_BASE_ADDRESS_NUM];
  UINT16                          BmcFirmwareVersion;
  UINT8                           IpmiVersion;
};

#pragma pack()


extern EFI_GUID gH2OIpmiHobGuid;


#endif

