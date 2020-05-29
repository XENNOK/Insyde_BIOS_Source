/** @file

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

#ifndef _DEBUG_MASK_PEI_H_
#define _DEBUG_MASK_PEI_H_

#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/DebugMask.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/PeimEntryPoint.h>

EFI_STATUS
EFIAPI
DebugMaskPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

#endif
