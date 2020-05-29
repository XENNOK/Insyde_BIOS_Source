/** @file
  Thunk driver for OEM_SERVICES_PROTOCOL

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

#ifndef _DXE_OEM_SERVICES_THUNK_H_
#define _DXE_OEM_SERVICES_THUNK_H_

#include <Uefi.h>
#include <Protocol/OemServices.h>

EFI_STATUS
OemSvcBootDisplayDeviceReplaceThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallBootMangerKeyTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcCalculateWriteCmosChecksumThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcCheckVgaOrderThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcCsm16ReferSwitchThunk (
  IN OEM_SERVICES_PROTOCOL             *OemServices,
  IN UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcDisplayLogoThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcDisplaySpiNotSupportThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetHotplugBridgeInfoThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetOa30MsdmDataThunk (
  IN  OEM_SERVICES_PROTOCOL                *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetSioResourceTableThunk (
  IN OEM_SERVICES_PROTOCOL  *Services,
  IN UINTN                  NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetSlp20PubkeyAndMarkerRomThunk (
  IN  OEM_SERVICES_PROTOCOL                *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetSsidSvidInfoThunk (
  IN     OEM_SERVICES_PROTOCOL        *This,
  IN     UINTN                        NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallDmiSwitchTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPciRomSwitchTextModeThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallLegacyBiosOemSlpThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcPrepareInstallMpTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcLoadDefaultSetupMenuThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN UINTN                                 NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcLogoResolutionThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallOptionRomTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcUnLoadPciOptionRomThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallMultiSuperIoTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcGetMaxCheckPasswordCountThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcCheckPasswordFailCallBackThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcPasswordEntryCallBackThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPciSkipTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcUnSkipPciDeviceThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallIrqRoutingTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPnpGpnvTableThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPnpStringTableThunk (
  IN  OEM_SERVICES_PROTOCOL             *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcInstallPostKeyTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcScuCallbackFunctionThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcUpdateBbsTableThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcUpdateFormLenThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  );

EFI_STATUS
OemSvcUpdateScuSystemInfoThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  );

#endif
