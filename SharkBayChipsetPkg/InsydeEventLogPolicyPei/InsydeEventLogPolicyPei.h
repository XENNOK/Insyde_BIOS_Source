/** @file

  Header file of Insyde Event Log Policy PEI implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INSYDE_EVENT_LOG_POLICY_PEI_H_
#define _INSYDE_EVENT_LOG_POLICY_PEI_H_

//
// Statements that include other files.
//
#include <SetupConfig.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Pi/PiPeiCis.h>

EFI_STATUS
ReadOnlyVariable2Callback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_STATUS
UpdateInsydeEventLogPolicy (
  IN CONST EFI_PEI_SERVICES         **PeiServices
  );

#endif

