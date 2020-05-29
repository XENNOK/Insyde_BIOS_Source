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

#include "DxeOemServicesThunk.h"
#include <OemServices/Kernel.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  OEM_SERVICES_PROTOCOL                 OemServices;
} OEM_SERVICES_INSTANCE;

//
// function prototypes
//
EFI_STATUS
NullDxeOemSvcFunc (
  IN  OEM_SERVICES_PROTOCOL             *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

VOID
AssignNullDxeOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  );

EFI_STATUS
RegisterDxeOemSvcFunc (
  IN  OEM_SERVICES_PROTOCOL             *OemServices
  );

//
// helper functions implementation
//

/**
  An empty OEM_SERVICES_PROTOCOL function.

  @param  *This                 OEM_SERVICES_PROTOCOL
  @param  NumOfArgs             Number of variable arguments

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
**/
EFI_STATUS
NullDxeOemSvcFunc (
  IN  OEM_SERVICES_PROTOCOL             *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Assign empty OEM_SERVICES_PROTOCOL functions by default.

  @param  *OemServicesFunc      Array of OEM_SERVICES_PROTOCOL functions.
  @param  NumOfEnum             Number of functions in array.

  @retval                       None
**/
VOID
AssignNullDxeOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  )
{
  UINT32  Index;

  for (Index = 0; Index < NumOfEnum; Index++) {
    OemServicesFunc[Index] = NullDxeOemSvcFunc;
  }
}

/**
  Register OEM_SERVICES_PROTOCOL functions.

  @param  *OemServices          OEM_SERVICES_PROTOCOL

  @retval EFI_SUCCESS           Always return success.
**/
EFI_STATUS
RegisterDxeOemSvcFunc (
  IN  OEM_SERVICES_PROTOCOL             *OemServices
  )
{
  OemServices->Funcs[COMMON_DMI_SWITCH_TABLE]                   = OemSvcInstallDmiSwitchTableThunk;
  OemServices->Funcs[COMMON_BOOTMANGER_KEY_TABLE]               = OemSvcInstallBootMangerKeyTableThunk;
  OemServices->Funcs[COMMON_POST_KEY_TABLE]                     = OemSvcInstallPostKeyTableThunk;
  OemServices->Funcs[COMMON_PCI_SKIP_TABLE_INDEX]               = OemSvcInstallPciSkipTableThunk;
  OemServices->Funcs[COMMON_LEGACY_BIOS_OEM_SLP]                = OemSvcInstallLegacyBiosOemSlpThunk;
  OemServices->Funcs[COMMON_GET_MAX_CHECK_COUNT]                = OemSvcGetMaxCheckPasswordCountThunk;
  OemServices->Funcs[COMMON_ERROR_PASSWORD_CALL_BACK]           = OemSvcCheckPasswordFailCallBackThunk;
  OemServices->Funcs[COMMON_PASSWORD_ENTRY_CALL_BACK]           = OemSvcPasswordEntryCallBackThunk;
  OemServices->Funcs[COMMON_UPDATE_SCU_SYSTEM_INFO]             = OemSvcUpdateScuSystemInfoThunk;
  OemServices->Funcs[COMMON_SCU_CALLBACK_FUNCTION]              = OemSvcScuCallbackFunctionThunk;
  OemServices->Funcs[COMMON_CHECK_VGA_ORDER]                    = OemSvcCheckVgaOrderThunk;
  OemServices->Funcs[COMMON_DISPLAY_LOGO]                       = OemSvcDisplayLogoThunk;
  OemServices->Funcs[COMMON_CALCULATE_WRITE_CMOS_CHECKSUM]      = OemSvcCalculateWriteCmosChecksumThunk;
  OemServices->Funcs[COMMON_CSM16_REFER_SWITCH]                 = OemSvcCsm16ReferSwitchThunk;
  OemServices->Funcs[COMMON_UPDATE_SLP20_PUBKEY_AND_SLP_MARKER] = OemSvcGetSlp20PubkeyAndMarkerRomThunk;
  OemServices->Funcs[COMMON_UPDATE_MSDM_DATA]                   = OemSvcGetOa30MsdmDataThunk;
  OemServices->Funcs[COMMON_FORM_LENGTH_UDPATE]                 = OemSvcUpdateFormLenThunk;
  OemServices->Funcs[COMMON_GET_MULTI_SIO_RESOURCE_TABLE]       = OemSvcInstallMultiSuperIoTableThunk;
  OemServices->Funcs[COMMON_LOAD_DEFAULT_SETUP_MENU]            = OemSvcLoadDefaultSetupMenuThunk;
  OemServices->Funcs[COMMON_LOGO_RESOLUTION]                    = OemSvcLogoResolutionThunk;
  OemServices->Funcs[COMMON_IRQ_ROUTING_TABLE]                  = OemSvcInstallIrqRoutingTableThunk;
  OemServices->Funcs[COMMON_OPTION_ROM_TABLE]                   = OemSvcInstallOptionRomTableThunk;
  OemServices->Funcs[COMMON_UNSKIP_PCI_DEVICE]                  = OemSvcUnSkipPciDeviceThunk;
  OemServices->Funcs[COMMON_GET_HOTPLUG_BRIDGE_INFO]            = OemSvcGetHotplugBridgeInfoThunk;
  OemServices->Funcs[COMMON_UNLOAD_PCI_OPTION_ROM]              = OemSvcUnLoadPciOptionRomThunk;
  OemServices->Funcs[COMMON_BOOT_DISPLAY_DEVICE_REPLACE]        = OemSvcBootDisplayDeviceReplaceThunk;
  OemServices->Funcs[COMMON_PREPARE_INSTALL_MP_TABLE]           = OemSvcPrepareInstallMpTableThunk;
  OemServices->Funcs[COMMON_GET_SIO_RESOURCE_TABLE]             = OemSvcGetSioResourceTableThunk;
  OemServices->Funcs[COMMON_INSTALL_PCI_ROM_SWITCH_TEXT_MODE]   = OemSvcInstallPciRomSwitchTextModeThunk;
  OemServices->Funcs[COMMON_DISPLAY_SPI_NOT_SUPPORT]            = OemSvcDisplaySpiNotSupportThunk;
  OemServices->Funcs[COMMON_UPDATE_BBS_TABLE]                   = OemSvcUpdateBbsTableThunk;
  OemServices->Funcs[COMMON_GET_SSID_SVID_INFO_CALLBACK]        = OemSvcGetSsidSvidInfoThunk;
  OemServices->Funcs[COMMON_PNP_DXE_UPDATE_STRING_TABLE]        = OemSvcInstallPnpStringTableThunk;
  OemServices->Funcs[COMMON_PNP_DXE_GPNV_HANDLE_TABLE]          = OemSvcInstallPnpGpnvTableThunk;

  return EFI_SUCCESS;
}


/**
  Entry point of the Thunk driver.

  @param  ImageHandle           ImageHandle
  @param  *SystemTable          EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Register OEM_SERVICES_PROTOCOL functions successfully.
  @retval Other                 Failed.
**/
EFI_STATUS
DxeOemSvcThunkEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  OEM_SERVICES_INSTANCE                 *PrivateData;

  Status = EFI_OUT_OF_RESOURCES;

  PrivateData = (OEM_SERVICES_INSTANCE *)AllocatePool (sizeof (OEM_SERVICES_INSTANCE));
  if (PrivateData != NULL) {
    PrivateData->OemServices.Funcs = (OEM_SERVICES_FUNCTION *)AllocateZeroPool (sizeof (OEM_SERVICES_FUNCTION) * RUNTIME_MAX_NUM_COMMON_SUPPORT);
    if (PrivateData->OemServices.Funcs == NULL) {
     return EFI_OUT_OF_RESOURCES;
    }
    AssignNullDxeOemSvcFunc (PrivateData->OemServices.Funcs, RUNTIME_MAX_NUM_COMMON_SUPPORT);

  Status = RegisterDxeOemSvcFunc (&PrivateData->OemServices);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PrivateData->Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &PrivateData->Handle,
                  &gOemServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PrivateData->OemServices
                  );
  }

  return Status;
}
