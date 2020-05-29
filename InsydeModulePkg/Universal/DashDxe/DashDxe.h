/** @file
  Common definition file for DASH

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DASH_H_
#define _DASH_H_

#include "MctpBus.h"
#include "Mctp.h"
#include "Pldm.h"

typedef struct {
  MCTP_BUS_CONTEXT   MctpBusContext;
  MCTP_CONTEXT       MctpContext;
  PLDM_CONTEXT       PldmContext;
} DASH_CONTEXT;


EFI_STATUS
EFIAPI 
SetupMctpBusProtocol (
  MCTP_BUS_CONTEXT *MctpBusContext,
  MCTP_SMBUS_PROTOCOL *MctpSmbus
);

EFI_STATUS
EFIAPI 
SetupMctpProtocol (
  MCTP_CONTEXT           *MctpContext,
  EFI_MCTP_BUS_PROTOCOL  *MctpBus
);

EFI_STATUS
EFIAPI 
SetupPldmProtocol (
  PLDM_CONTEXT      *PldmContext,
  EFI_MCTP_PROTOCOL *Mctp
);

#endif
