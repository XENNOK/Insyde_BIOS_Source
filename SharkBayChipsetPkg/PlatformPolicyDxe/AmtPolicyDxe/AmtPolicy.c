//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   AmtPolicy.c
//;

#include <AmtPolicy.h>

EFI_STATUS
EFIAPI
AMTPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
//[-start-120301-IB07360173-add]//
  DXE_MBP_DATA_PROTOCOL             *MBPDataProtocol;
//[-start-130417-IB05160439-remove]//
//  BOOLEAN                           FirstBootFlag;
//[-end-130417-IB05160439-remove]//
//[-end-120301-IB07360173-add]//
  DXE_AMT_POLICY_PROTOCOL           *AMTPlatformPolicy;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;
//[-start-120626-IB06460408-remove]//
//  UINT8                             SolEnabled;
//  UINT8                             IderEnabled;
//[-end-120626-IB06460408-remove]//
//[-start-120626-IB06460408-modify]//
  UINT8                             AtEnabled;
//[-end-120626-IB06460408-modify]//
  UINT8                             iAmtbxHotkeyPressed;
  UINT8                             iRemoteAsstHotkeyPressed;
//[-start-120307-IB07360174-add]//
//[-start-130417-IB05160439-remove]//
//  VOID                              *HobList;
////[-start-130208-IB10820234-modify]//
//  UINT8                             *Raw;
////[-end-130208-IB10820234-modify]//
//[-end-130417-IB05160439-remove]//
  UINT8                             CmosData;
//[-end-120307-IB07360174-add]//
//[-start-120806-IB05330368-add]//
  ME_BIOS_EXTENSION_SETUP           MeBiosExtensionSetupData;
  UINTN                             VariableSize;
//[-end-120806-IB05330368-add]//

  Handle            = NULL;
//[-start-120301-IB07360173-add]//
  MBPDataProtocol   = NULL;
//[-end-120301-IB07360173-add]//
  AMTPlatformPolicy = NULL;
  SetupUtility      = NULL;
  SetupVariable     = NULL;
//[-start-120626-IB06460408-remove]//
//  SolEnabled        = 0;
//  IderEnabled       = 0;
//[-end-120626-IB06460408-remove]//
//[-start-120626-IB06460408-modify]//
  AtEnabled           = 0;
//[-end-120626-IB06460408-modify]//
  iAmtbxHotkeyPressed = 0;
  iRemoteAsstHotkeyPressed = 0;
//[-start-130417-IB05160439-remove]//
////[-start-120301-IB07360173-add]//  
//  FirstBootFlag     = FALSE;
////[-end-120301-IB07360173-add]//
//[-end-130417-IB05160439-remove]//

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\nAMT Policy Entry\n" ) );

//[-start-120307-IB07360174-add]//
//[-start-130417-IB05160439-remove]//
//  CmosData = ReadExtCmos8(R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset);
//  HobList = GetHobList ();
////[-start-130208-IB10820234-modify]//
//  Raw = GetNextGuidHob ((CONST EFI_GUID*)&gSetupDefaultHobGuid, HobList);
//  if ( (Raw == NULL)|| CmosData == V_ME_NEED_BIOS_SYNC ) {
//    FirstBootFlag = TRUE;
//  }
////[-end-130208-IB10820234-modify]//
//  //
//  // Clean CMOS data in offset MEFlashReset
//  //
//  WriteExtCmos8(R_XCMOS_INDEX, R_XCMOS_DATA,MEFlashReset, V_ME_NO_RESET);
//[-end-130417-IB05160439-remove]//
  Status = gBS->LocateProtocol (
             &gMeBiosPayloadDataProtocolGuid,
             NULL,
             (VOID **)&MBPDataProtocol
             );
  ASSERT_EFI_ERROR ( Status );
//[-end-120307-IB07360174-add]//
  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );

  //
  // Allocate and set AMT Policy structure to known value
  //
  AMTPlatformPolicy = NULL;
//[-start-120307-IB07360174-modify]//
  AMTPlatformPolicy = AllocateZeroPool ( sizeof ( DXE_AMT_POLICY_PROTOCOL ) );
//[-end-120307-IB07360174-modify]//
  ASSERT ( AMTPlatformPolicy != NULL );

  if ( AMTPlatformPolicy == NULL ) {
    return EFI_OUT_OF_RESOURCES;
  }
  if ( EFI_ERROR ( Status ) ) {
    return Status;
  }

//[-start-120829-IB04320199-add]//
  if (FeaturePcdGet (PcdAntiTheftSupported)) {
    AMTPlatformPolicy->AmtConfig.AtEnabled = SetupVariable->AtConfig;
  }
//[-end-120829-IB04320199-add]//
//[-start-120301-IB07360173-add]//
  if ( MBPDataProtocol->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType != INTEL_ME_5MB_FW ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Install Dummy AMT Policy Protocol\n" ) );
    //
    // Install Dummy AMT Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxePlatformAmtPolicyGuid,
               EFI_NATIVE_INTERFACE,
               AMTPlatformPolicy
               );
    ASSERT_EFI_ERROR ( Status );
  
    //
    // Dump policy
    //
    DumpAMTPolicy ( AMTPlatformPolicy );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "AMT Policy Exit\n" ) );
    return Status;    
  }
//[-end-120301-IB07360173-add]//

//[-start-120626-IB06460408-remove]//
//  //
//  // AMTPlatformPolicy->AmtConfig.SolEnabled
//  //
//  // BUGBUG : It might need to read variable.
//  //
//  SolEnabled = 0;
//#ifdef ME_5MB_SUPPORT
//  SolEnabled = 1;
//#endif
//[-end-120626-IB06460408-remove]//

//[-start-120626-IB06460408-remove]//
//  //
//  // AMTPlatformPolicy->AmtConfig.IderEnabled
//  //
//  // BUGBUG : It might need to read variable.
//  //
//  IderEnabled = 0;
//#ifdef ME_5MB_SUPPORT
//  IderEnabled = 1;
//#endif
//[-end-120626-IB06460408-remove]//

  //
  // AMTPlatformPolicy->AmtConfig.iAmtbxHotkeyPressed
  //
  if ( FeaturePcdGet ( PcdMe5MbSupported ) ) {
    Status = gRT->GetVariable(
                    L"iAmtbxHotkeyPressed",
                    &gEfiGenericVariableGuid,
                    NULL,
                    NULL,
                    &iAmtbxHotkeyPressed
                    );
    if (EFI_ERROR(Status)) {
      iAmtbxHotkeyPressed = 0;
    }
    Status = gRT->GetVariable(
                    L"iRemoteAsstHotkeyPressed",
                    &gEfiGenericVariableGuid,
                    NULL,
                    NULL,
                    &iRemoteAsstHotkeyPressed
                    );
    if (EFI_ERROR(Status)) {
      iRemoteAsstHotkeyPressed = 0;
    }
  }

//[-start-120626-IB06460408-modify]//
  //
  // AMTPlatformPolicy->AmtConfig.AtEnabled
  //
  AtEnabled = 0;
//[-start-120829-IB04320199-modify]//
  if (FeaturePcdGet (PcdAntiTheftSupported)) {
    AtEnabled = SetupVariable->AtConfig;
  }
//[-end-120829-IB04320199-modify]//
//[-end-120626-IB06460408-modify]//

  //
  // Update REVISION number
  //
//[-start-120626-IB06460408-modify]//
//[-start-120321-IB07360180-modify]//
  AMTPlatformPolicy->Revision                            = DXE_PLATFORM_AMT_POLICY_PROTOCOL_REVISION_1;
//[-end-120321-IB07360180-modify]//
//[-end-120626-IB06460408-modify]//

  //
  // Initialzie AMT Configuration
  //
  AMTPlatformPolicy->AmtConfig.AsfEnabled                = SetupVariable->Asf;
  AMTPlatformPolicy->AmtConfig.iAmtEnabled               = SetupVariable->Amt;
//[-start-120301-IB07360173-add]//  
  AMTPlatformPolicy->AmtConfig.iAmtbxPasswordWrite       = SetupVariable->AmtbxPasswordWrite;
//[-end-120301-IB07360173-add]//
  AMTPlatformPolicy->AmtConfig.WatchDog                  = SetupVariable->WatchDog;
  AMTPlatformPolicy->AmtConfig.CiraRequest               = SetupVariable->CiraRequest;
  AMTPlatformPolicy->AmtConfig.ManageabilityMode         = PcdGet8 ( PcdAmtConfigManageabilityMode );    // BUGBUG : It might need to read variable.
//[-start-120626-IB06460408-remove]//
//  AMTPlatformPolicy->AmtConfig.SolEnabled                = SolEnabled;
//  AMTPlatformPolicy->AmtConfig.IderEnabled               = IderEnabled;
//[-end-120626-IB06460408-remove]//
  AMTPlatformPolicy->AmtConfig.UnConfigureMe             = SetupVariable->UnConfigureME;
  AMTPlatformPolicy->AmtConfig.MebxDebugMsg              = SetupVariable->MebxDebugMsg;
//[-start-120301-IB07360173-modify]//  
//[-start-130417-IB05160439-modify]//
  CmosData = ReadExtCmos8(R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset);
  if (CmosData == V_ME_NEED_BIOS_SYNC) {
    AMTPlatformPolicy->AmtConfig.ForcMebxSyncUp            = 1;
  } else {
    AMTPlatformPolicy->AmtConfig.ForcMebxSyncUp            = 0;
  }
  //
  // Clean CMOS data in offset MEFlashReset
  //
  WriteExtCmos8(R_XCMOS_INDEX, R_XCMOS_DATA,MEFlashReset, V_ME_NO_RESET);
//[-end-130417-IB05160439-modify]//
//[-end-120301-IB07360173-modify]//  
  AMTPlatformPolicy->AmtConfig.UsbrEnabled               = PcdGet8 ( PcdAmtConfigUsbrEnabled );
  AMTPlatformPolicy->AmtConfig.UsbLockingEnabled         = PcdGet8 ( PcdAmtConfigUsbLockingEnabled );
  AMTPlatformPolicy->AmtConfig.HideUnConfigureMeConfirm  = SetupVariable->HideUnConfigureMeConfirm;
//[-start-120626-IB06460408-modify]//
  AMTPlatformPolicy->AmtConfig.AtEnabled                 = AtEnabled;
//[-end-120626-IB06460408-modify]//
  AMTPlatformPolicy->AmtConfig.USBProvision              = SetupVariable->AmtUsbConfigure;
  AMTPlatformPolicy->AmtConfig.FWProgress                = SetupVariable->FWProgress;
  AMTPlatformPolicy->AmtConfig.iAmtbxHotkeyPressed       = iAmtbxHotkeyPressed;
  AMTPlatformPolicy->AmtConfig.CiraRequest               = iRemoteAsstHotkeyPressed;
  AMTPlatformPolicy->AmtConfig.iAmtbxSelectionScreen     = SetupVariable->iAmtbxSelectionScreen;
  AMTPlatformPolicy->AmtConfig.WatchDogTimerOs           = SetupVariable->WatchDogTimerOs;
  AMTPlatformPolicy->AmtConfig.WatchDogTimerBios         = SetupVariable->WatchDogTimerBios;
  AMTPlatformPolicy->AmtConfig.CiraTimeout               = SetupVariable->CiraTimeout;
  AMTPlatformPolicy->AmtConfig.PciDeviceFilterOutTable   = 0;
//[-start-120626-IB06460408-add]//
  //
  //  CPU Replacement Timeout:
  //  0: 10 seconds; 1: 30 seconds; 2~5: Reserved; 6: No delay; 7: Unlimited delay
  //
  AMTPlatformPolicy->AmtConfig.CpuReplacementTimeout     = PcdGet8 ( PcdAmtConfigCpuReplacementTimeout );
//[-end-120626-IB06460408-add]//
  AMTPlatformPolicy->AmtConfig.MebxNonUiTextMode         = PcdGet16 ( PcdAmtConfigMebxNonUiTextMode );
  AMTPlatformPolicy->AmtConfig.MebxUiTextMode            = PcdGet16 ( PcdAmtConfigMebxUiTextMode );
  AMTPlatformPolicy->AmtConfig.MebxGraphicsMode          = PcdGet16 ( PcdAmtConfigMebxGraphicsMode );

//[-start-120626-IB06460408-remove]//
//  //
//  // Initialzie BiosParamsPtr
//  //
//  AMTPlatformPolicy->BiosParamsPtr                       = NULL;
//[-end-120626-IB06460408-remove]//

//[-start-120806-IB05330368-add]//
  ///
  /// Get BIOS Sync-up data from MEBx to AMT platform policy
  ///
  VariableSize = sizeof (MeBiosExtensionSetupData);
  Status = gRT->GetVariable (
                  gEfiMeBiosExtensionSetupName,
                  &gEfiMeBiosExtensionSetupGuid,
                  NULL,
                  &VariableSize,
                  &MeBiosExtensionSetupData
                  );
  if (EFI_ERROR (Status) || (MeBiosExtensionSetupData.InterfaceVersion == 0)) {
    AMTPlatformPolicy->AmtConfig.ManageabilityMode = 0;
    //
    // This is first boot after update BIOS or the MebxSetupData variable is destroyed.
    // In this case, we can set the force bit to synch up data with Mebx. For the normal case,
    // ForcMebxSyncUp is still updated per customization as above comment. 
    // 
    AMTPlatformPolicy->AmtConfig.ForcMebxSyncUp    = 1;
  } else {
    AMTPlatformPolicy->AmtConfig.ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
  }
//[-end-120806-IB05330368-add]//
//[-start-121219-IB10820205-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateAmtPlatformPolicy (AMTPlatformPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateAmtPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return Status;
  }
//[-end-121219-IB10820205-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Install AMT Policy Protocol\n" ) );
  //
  // Install AMT Policy Protocol.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
             &Handle,
             &gDxePlatformAmtPolicyGuid,
             EFI_NATIVE_INTERFACE,
             AMTPlatformPolicy
             );
  ASSERT_EFI_ERROR ( Status );

  //
  // Dump policy
  //
  DumpAMTPolicy ( AMTPlatformPolicy );

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "AMT Policy Exit\n" ) );

  return Status;
}

VOID
DumpAMTPolicy (
  IN      DXE_AMT_POLICY_PROTOCOL        *AMTPlatformPolicy
  )
{
  UINTN        Count;

  Count = 0;

//[-start-120626-IB06460408-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "AMTPlatformPolicy ( Address : 0x%x )\n", AMTPlatformPolicy ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-Revision                     : %x\n", AMTPlatformPolicy->Revision ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-AmtConfig ( Address : 0x%x )\n", &AMTPlatformPolicy->AmtConfig ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AsfEnabled                : %x\n", AMTPlatformPolicy->AmtConfig.AsfEnabled ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-iAmtEnabled               : %x\n", AMTPlatformPolicy->AmtConfig.iAmtEnabled ) );
//[-start-120301-IB07360173-add]//  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-iAmtbxPasswordWrite       : %x\n", AMTPlatformPolicy->AmtConfig.iAmtbxPasswordWrite ) );
//[-end-120301-IB07360173-add]//  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-WatchDog                  : %x\n", AMTPlatformPolicy->AmtConfig.WatchDog ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-CiraRequest               : %x\n", AMTPlatformPolicy->AmtConfig.CiraRequest ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ManageabilityMode         : %x\n", AMTPlatformPolicy->AmtConfig.ManageabilityMode ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-UnConfigureMe             : %x\n", AMTPlatformPolicy->AmtConfig.UnConfigureMe ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MebxDebugMsg              : %x\n", AMTPlatformPolicy->AmtConfig.MebxDebugMsg ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ForcMebxSyncUp            : %x\n", AMTPlatformPolicy->AmtConfig.ForcMebxSyncUp ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-UsbrEnabled               : %x\n", AMTPlatformPolicy->AmtConfig.UsbrEnabled ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-UsbLockingEnabled         : %x\n", AMTPlatformPolicy->AmtConfig.UsbLockingEnabled ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-HideUnConfigureMeConfirm  : %x\n", AMTPlatformPolicy->AmtConfig.HideUnConfigureMeConfirm ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-USBProvision              : %x\n", AMTPlatformPolicy->AmtConfig.USBProvision ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-FWProgress                : %x\n", AMTPlatformPolicy->AmtConfig.FWProgress ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-iAmtbxHotkeyPressed       : %x\n", AMTPlatformPolicy->AmtConfig.iAmtbxHotkeyPressed ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-iAmtbxSelectionScreen     : %x\n", AMTPlatformPolicy->AmtConfig.iAmtbxSelectionScreen ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AtEnabled                 : %x\n", AMTPlatformPolicy->AmtConfig.AtEnabled ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Byte2Reserved             : %x\n", AMTPlatformPolicy->AmtConfig.Byte2Reserved ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-WatchDogTimerOs           : %x\n", AMTPlatformPolicy->AmtConfig.WatchDogTimerOs ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-WatchDogTimerBios         : %x\n", AMTPlatformPolicy->AmtConfig.WatchDogTimerBios ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-CiraTimeout               : %x\n", AMTPlatformPolicy->AmtConfig.CiraTimeout ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-CpuReplacementTimeout     : %x\n", AMTPlatformPolicy->AmtConfig.CpuReplacementTimeout ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MebxNonUiTextMode         : %x\n", AMTPlatformPolicy->AmtConfig.MebxNonUiTextMode ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MebxUiTextMode            : %x\n", AMTPlatformPolicy->AmtConfig.MebxUiTextMode ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MebxGraphicsMode          : %x\n", AMTPlatformPolicy->AmtConfig.MebxGraphicsMode ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-OemResolutionSettingsRsvd : %x\n", AMTPlatformPolicy->AmtConfig.OemResolutionSettingsRsvd ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PciDeviceFilterOutTable   : %x\n", AMTPlatformPolicy->AmtConfig.PciDeviceFilterOutTable ) );
//[-start-120301-IB07360173-modify]//  
  for ( Count = 0 ; Count < 9 ; Count = Count + 1 ) {
    if ( Count < 10 ) {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ByteReserved[%d]           : %x\n", Count, AMTPlatformPolicy->AmtConfig.ByteReserved[Count] ) );
    } else {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ByteReserved[%d]          : %x\n", Count, AMTPlatformPolicy->AmtConfig.ByteReserved[Count] ) );
    }
//[-end-120301-IB07360173-modify]//
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );
//[-end-120626-IB06460408-modify]//

  return;
}
