//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
/*--
Module Name:

  H2OIpmiHob.h

Abstract:

  H2O IPMI HOB definition file

--*/

#ifndef _H2O_IPMI_HOB_H_
#define _H2O_IPMI_HOB_H_

#include "H2OIpmi.h"

#define H2O_IPMI_HOB_GUID \
  {0x545869d9, 0xf49f, 0x4ddf, 0x81, 0xfd, 0x92, 0x7a, 0xb2, 0x7e, 0xe5, 0x9d}

//
// This is the definition of the HOB structure.
//
typedef struct _H2O_IPMI_HOB {
  H2O_IPMI_BMC_STATUS             Status;
  H2O_IPMI_INTERFACE_TYPE         InterfaceType;
  H2O_IPMI_INTERFACE_ADDRESS_TYPE InterfaceAddressType;
  H2O_IPMI_INTERFACE_ADDRESS      InterfaceAddress[H2O_IPMI_MAX_BASE_ADDRESS_NUM];
  UINT16                          FirmwareVersion;
  UINT8                           IpmiVersion;
} H2O_IPMI_HOB;

extern EFI_GUID gH2OIpmiHobGuid;

#endif
