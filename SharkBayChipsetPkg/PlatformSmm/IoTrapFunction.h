/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IO_TRAP_FUNCTION_H
#define _IO_TRAP_FUNCTION_H


#include <Platform.h>
#include <Protocol/SmmIoTrapDispatch.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//

void
IchnIoTrap0SmiFunctionCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT           *DispatchContext
  );

EFI_STATUS
EnableIoTrap ();

#endif

