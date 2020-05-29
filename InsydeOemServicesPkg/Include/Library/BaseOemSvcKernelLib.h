/** @file
  Definition for Oem Services Default Lib.

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BASE_OEM_SVC_KERNEL_LIB_H_
#define _BASE_OEM_SVC_KERNEL_LIB_H_

#include <Uefi.h>

EFI_STATUS
OemSvcEcIdle (
  IN  BOOLEAN         EnableEcIdle
  );

EFI_STATUS
OemSvcEcWait (
  IN  BOOLEAN         EnableEcWait
  );

EFI_STATUS
OemSvcEcInit (
  );

EFI_STATUS
OemSvcEcAcpiMode (
  IN  BOOLEAN         EnableEcAcpiMode
  );

#endif
