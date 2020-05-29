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

  H2OIpmiPlatformHookPpi.h

Abstract:

  H2O IPMI Platform Hook Ppi Definition

--*/

#ifndef _H2O_IPMI_PLATFORM_HOOK_PPI_H_
#define _H2O_IPMI_PLATFORM_HOOK_PPI_H_

#include EFI_PPI_DEFINITION (H2OIpmiInterfacePpi)


EFI_FORWARD_DECLARATION (H2O_IPMI_PLATFORM_HOOK_PPI);



#define H2O_IPMI_PLATFORM_HOOK_PPI_GUID \
  {0x5d47d660, 0x544a, 0x4e0e, 0x8c, 0x0c, 0x23, 0x2a, 0xd9, 0xfa, 0x45, 0x71}


//
// Functions
//
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_PLATFORM_HOOK_PPI_PLATFORM_WAIT_BMC) (
  IN  EFI_PEI_SERVICES           **PeiServices,
  IN  H2O_IPMI_INTERFACE_PPI      *IpmiPpi
);
/*++

Routine Description:

  Wait BMC ready via platform way

Arguments:

   PeiServices - A pointer to PEI Services.
   
   IpmiPpi     - A pointer to H2O_IPMI_INTERFACE_PPI structure.

Returns:

  EFI_SUCCESS      - Platform wait BMC successfully.

  EFI_UNSUPPORTED  - Platform does not implement this function.
                     This means platform does not need to do any special things
                     to wait for BMC ready. System will use original way to wait
                     for BMC ready.
  
--*/


//
// PPI data structure
//
typedef struct _H2O_IPMI_PLATFORM_HOOK_PPI {
  H2O_IPMI_PLATFORM_HOOK_PPI_PLATFORM_WAIT_BMC PlatformWaitBmc;
} H2O_IPMI_PLATFORM_HOOK_PPI;

extern EFI_GUID gH2OIpmiPlatformHookPpiGuid;

#endif 

