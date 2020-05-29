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

  H2OIpmiUtilityProtocol.h

Abstract:

  H2OIpmiUtilityProtocol header file

--*/

#ifndef _H2O_IPMI_UTILITY_PROTOCOL_H_
#define _H2O_IPMI_UTILITY_PROTOCOL_H_

#include "Tiano.h"

EFI_FORWARD_DECLARATION (H2O_IPMI_UTILITY_PROTOCOL);


//
// Variable Guid
//
#define H2O_IPMI_UTILITY_PROTOCOL_GUID \
  { 0x6d8be92c, 0xd97e, 0x431d, 0x8a, 0xdf, 0xd3, 0xf8, 0x0d, 0x53, 0x51, 0x45}



typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_UTILITY_PROTOCOL_EXECUTE_IPMI_UTILITY) (
  IN  H2O_IPMI_UTILITY_PROTOCOL   *This
);
/*++

Routine Description:

  

Arguments:

   This        - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance

Returns:

  
--*/



typedef struct _H2O_IPMI_UTILITY_PROTOCOL {
  H2O_IPMI_UTILITY_PROTOCOL_EXECUTE_IPMI_UTILITY ExecuteIpmiUtility;
} H2O_IPMI_UTILITY_PROTOCOL;


extern EFI_GUID gH2OIpmiUtilityProtocolGuid;

#endif
