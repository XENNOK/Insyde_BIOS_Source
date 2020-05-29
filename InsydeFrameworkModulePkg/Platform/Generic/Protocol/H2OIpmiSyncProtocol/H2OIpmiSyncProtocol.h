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

  H2OIpmiSyncProtocol.h

Abstract:

  H2O IPMI Sync Protocol definition file

--*/

#ifndef _H2O_IPMI_SYNC_PROTOCOL_H_
#define _H2O_IPMI_SYNC_PROTOCOL_H_


#include "Tiano.h"
#include "SetupConfig.h"

EFI_FORWARD_DECLARATION (H2O_IPMI_SYNC_PROTOCOL);
//
// Protocol Guid
//
#define H2O_IPMI_SYNC_PROTOCOL_GUID \
  { 0x705213C0, 0x0FAB, 0x4502, 0x8D, 0xD2, 0xE0, 0x45, 0x7C, 0x98, 0x79, 0xCC}

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SYNC_PROTOCOL_SYNC_FROM_BMC) (
  IN  H2O_IPMI_SYNC_PROTOCOL       *This,
  IN  SYSTEM_CONFIGURATION         *SetupConfig
);
/*++

Routine Description:

  Get current BMC setting to update SYSTEM_CONFIGURATION structure.

Arguments:

   This           - Pointer to H2O_IPMI_SYNC_PROTOCOL instance
   SetupConfig    - A pointer to SYSTEM_CONFIGURATION structure.
   
Returns:

  
--*/

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SYNC_PROTOCOL_SET_TO_BMC) (
  IN  H2O_IPMI_SYNC_PROTOCOL       *This,
  IN  SYSTEM_CONFIGURATION         *SetupConfig
);
/*++

Routine Description:

  Restore current SYSTEM_CONFIGURATION setting to BMC.

Arguments:

   This           - Pointer to H2O_IPMI_SYNC_PROTOCOL instance
   SetupConfig    - A pointer to SYSTEM_CONFIGURATION structure.
   
Returns:

   
  
--*/


typedef struct _H2O_IPMI_SYNC_PROTOCOL {
  H2O_IPMI_SYNC_PROTOCOL_SYNC_FROM_BMC                  SyncFromBMC;
  H2O_IPMI_SYNC_PROTOCOL_SET_TO_BMC                     SetToBMC;
} H2O_IPMI_SYNC_PROTOCOL;

extern EFI_GUID gH2OIpmiSyncProtocolGuid;

#endif
