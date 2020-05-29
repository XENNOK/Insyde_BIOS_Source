/** @file
  Thunk driver for SMM_OEM_SERVICES_PROTOCOL

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_OEM_SERVICES_THUNK_H_
#define _SMM_OEM_SERVICES_THUNK_H_

#include <Uefi.h>
#include <Protocol/SmmOemServices.h>

EFI_STATUS
OemSvcDisableAcpiCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcEnableAcpiCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetAspmOverrideInfoThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL          *This,
  IN  UINTN                              NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcOsResetCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPnpGpnvTableThunk (
  IN SMM_OEM_SERVICES_PROTOCOL          *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPnpStringTableThunk (
  IN SMM_OEM_SERVICES_PROTOCOL          *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcPowerButtonCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcRestoreAcpiCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcS1CallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcS3CallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcS4CallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcS5AcLossCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcS5CallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcVariablePreservedTableThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

#endif


