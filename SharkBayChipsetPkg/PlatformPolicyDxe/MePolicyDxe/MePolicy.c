/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <MePolicy.h>

EFI_STATUS
RecordMeVersion (
  IN DXE_MBP_DATA_PROTOCOL  *MBPDataProtocol
);

PLATFORM_ME_HOOK_PROTOCOL       mPlatformMeHookData = {
  PLATFORM_ME_HOOK_PROTOCOL_REVISION,
  PreGlobalResetHook
};

//[-start-120420-IB05330338-modify]//
EFI_STATUS
EFIAPI
MEPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  DXE_ME_POLICY_PROTOCOL            *MEPlatformPolicy;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;
  BOOLEAN                           MEStatusFlag;
  DXE_MBP_DATA_PROTOCOL             *MBPDataProtocol;
  EFI_HECI_PROTOCOL                 *Heci;
//[-start-120301-IB07360173-add]//
  UINT32                            MeMode;
  UINT32                            MeStatus;
//[-end-120301-IB07360173-add]//

  Handle           = NULL;
  MEPlatformPolicy = NULL;
  SetupUtility     = NULL;
  SetupVariable    = NULL;
  Heci             = NULL;
  MEStatusFlag     = TRUE;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nME Policy Entry\n"));

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);
//[-start-120301-IB07360173-modify]//
  Status  = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **)&MBPDataProtocol);
//[-end-120301-IB07360173-modify]//
  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **)&Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "System cannot locate gEfiHeciProtocolGuid.\n"));
  }
  //
  // Allocate and set ME Policy structure to known value
  //
  MEPlatformPolicy = NULL;
  MEPlatformPolicy = AllocateZeroPool (sizeof (DXE_ME_POLICY_PROTOCOL));
  ASSERT (MEPlatformPolicy != NULL);
//[-start-130104-IB10870064-add]//
  if (MEPlatformPolicy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
  //
  // MEPlatformPolicy->MeVersion
  //
//[-start-120307-IB07360175-modify]//
  if ((MBPDataProtocol->MeBiosPayload.FwVersionName.MajorVersion != 9)) {
//[-end-120307-IB07360175-modify]//
    MEStatusFlag = FALSE;
  }  else {
    if (Heci != NULL) {
      Status = Heci->GetMeMode (&MeMode);
      if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
        MEStatusFlag = FALSE;
      }
    
      Status = Heci->GetMeStatus (&MeStatus);
      if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
        MEStatusFlag = FALSE;
      }
    } else {
      MEStatusFlag = FALSE;
    }
    RecordMeVersion (MBPDataProtocol);
  } 

  //
  // Update REVISION number
  //
//[-start-120626-IB06460408-modify]//
//[-start-120403-IB05300307-modify]//
  MEPlatformPolicy->Revision                         = DXE_PLATFORM_ME_POLICY_PROTOCOL_REVISION_1;
//[-end-120403-IB05300307-modify]//
//[-end-120626-IB06460408-modify]//

  //
  // Initialzie ME Configuration
  //
  MEPlatformPolicy->MeConfig.MeFwDownGrade           = SetupVariable->MEReFlash;
  MEPlatformPolicy->MeConfig.MeLocalFwUpdEnabled     = PcdGet8 (PcdMeConfigMeLocalFwUpdEnabled);
  if (MEStatusFlag) {
//[-start-120917-IB06150246-remove]//
//    MEPlatformPolicy->MeConfig.MbpSecurity           = 1;
//[-end-120917-IB06150246-remove]//
    MEPlatformPolicy->MeConfig.EndOfPostEnabled      = 1;
  } else {
    //
    // ME is not workable, Mbp is not working and EOP command shouldn't be sent.
    //
//[-start-120917-IB06150246-remove]//
//    MEPlatformPolicy->MeConfig.MbpSecurity           = 0;
//[-end-120917-IB06150246-remove]//
    MEPlatformPolicy->MeConfig.EndOfPostEnabled      = 0;
  }
//[-start-120411-IB06150209-modify]//
  if (FeaturePcdGet (PcdMrcRmt) || FeaturePcdGet (PcdH2ODdtSupported)) {
//[-start-120917-IB06150246-remove]//
//  MEPlatformPolicy->MeConfig.MbpSecurity             = 0;
//[-end-120917-IB06150246-remove]//
    MEPlatformPolicy->MeConfig.EndOfPostEnabled        = 0;
  }
//[-end-120411-IB06150209-modify]//
  MEPlatformPolicy->MeConfig.EndOfPostDone           = PcdGet8 (PcdMeConfigEndOfPostDone);    // Please don't change the default value of EndOfPostDone and the value will be updated to 1 when EOP has been sent to ME FW successfully.
//[-start-120403-IB05300307-modify]//
  MEPlatformPolicy->MeConfig.MdesForBiosState        = PcdGet8 (PcdMeConfigMdesForBiosState);
//[-end-120403-IB05300307-modify]//
//[-start-120626-IB06460408-add]//
  MEPlatformPolicy->MeConfig.MdesCapability          = PcdGet8 (PcdMeConfigMdesCapability);
  MEPlatformPolicy->MeConfig.SvtForPchCap            = PcdGet8 (PcdMeConfigSvtForPchCap);
//[-end-120626-IB06460408-add]//

//[-start-120301-IB07360173-add]//  
  MEPlatformPolicy->MeReportError                    = MEReportError;
//[-end-120301-IB07360173-add]//
  ///
  /// Misc. Config
  ///
  /// FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group
  /// Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  /// version information. The default value is type 221.
  ///
  MEPlatformPolicy->MeMiscConfig.FviSmbiosType       = PcdGet8 (PcdMeMiscConfigFviSmbiosType);

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install ME Policy Protocol\n"));
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxeMePolicy (MEPlatformPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxeMePolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install ME Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxePlatformMePolicyGuid,
               EFI_NATIVE_INTERFACE,
               MEPlatformPolicy
               );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121220-IB10820206-modify]//

  //
  // Dump policy
  //
  DumpMEPolicy (MEPlatformPolicy);

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
             &Handle,
             &gPlatformMeHookProtocolGuid,
             EFI_NATIVE_INTERFACE,
             &mPlatformMeHookData
             );
  ASSERT_EFI_ERROR(Status);   

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "ME Policy Exit\n"));

  return Status;
}
//[-end-120420-IB05330338-modify]//

VOID
MEReportError (
  IN      ME_ERROR_MSG_ID        MsgId
  )
{
  return;
}

VOID
DumpMEPolicy (
  IN      DXE_ME_POLICY_PROTOCOL        *MEPlatformPolicy
  )
{
  UINTN        Count;

  Count = 0;

//[-start-120626-IB06460408-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "MEPlatformPolicy ( Address : 0x%x )\n", MEPlatformPolicy));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                      : %x\n", MEPlatformPolicy->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-MeConfig ( Address : 0x%x )\n", &MEPlatformPolicy->MeConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MeFwDownGrade              : %x\n", MEPlatformPolicy->MeConfig.MeFwDownGrade));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MeLocalFwUpdEnabled        : %x\n", MEPlatformPolicy->MeConfig.MeLocalFwUpdEnabled));
//[-start-120917-IB06150246-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MbpSecurity                : %x\n", MEPlatformPolicy->MeConfig.MbpSecurity ) );
//[-end-120917-IB06150246-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EndOfPostEnabled           : %x\n", MEPlatformPolicy->MeConfig.EndOfPostEnabled));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EndOfPostDone              : %x\n", MEPlatformPolicy->MeConfig.EndOfPostDone));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MdesCapability             : %x\n", MEPlatformPolicy->MeConfig.MdesCapability));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SvtForPchCap               : %x\n", MEPlatformPolicy->MeConfig.SvtForPchCap));
//[-start-120403-IB05300308-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MdesForBiosState           : %x\n", MEPlatformPolicy->MeConfig.MdesForBiosState));
//[-end-120403-IB05300308-modify]//
 //[-start-120301-IB07360173-modify]//   
  for (Count = 0; Count < 15; Count = Count + 1) {
    if (Count < 10) {
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ByteReserved[%d]     : %x\n", Count, MEPlatformPolicy->MeConfig.ByteReserved[Count]));
    } else {
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ByteReserved[%d]    : %x\n", Count, MEPlatformPolicy->MeConfig.ByteReserved[Count]));
    }
//[-end-120301-IB07360173-modify]//
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
//[-end-120626-IB06460408-modify]//

  return;
}

EFI_STATUS
EFIAPI
PreGlobalResetHook (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;
  EFI_GUID                          GuidId = SYSTEM_CONFIGURATION_GUID;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  if (SetupVariable->MEReFlash == 1) {
    SetupVariable->MEReFlash = 0;
  }
  
  Status = gRT->SetVariable (
                  L"Setup",
                  &GuidId,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//[-start-130709-IB05160465-modify]//
                  PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                  (VOID *) SetupVariable
                  );

  return EFI_SUCCESS;
}

EFI_STATUS
RecordMeVersion (
  IN DXE_MBP_DATA_PROTOCOL  *MBPDataProtocol
)
{
  EFI_STATUS                  Status;
  UINTN                       VariableSize;
  RECORD_ME_VERSION           MeVersion;
  EFI_GUID                    RecordMeVersionGuid = RECORD_ME_VERSION_GUID;
  CHAR16                      *MeFwSkuString[ ] = {{L"1.5MB"},   {L"5MB"}};
  
  MeVersion.MajorVersion  = (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.MajorVersion;
  MeVersion.MinorVersion  = (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.MinorVersion;
  MeVersion.HotfixVersion = (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.HotfixVersion;
  MeVersion.BuildVersion  = (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.BuildVersion;

  if (MBPDataProtocol->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == INTEL_ME_1_5MB_FW) {
    MeVersion.SteppingString = MeFwSkuString[0];
  } else {
    MeVersion.SteppingString = MeFwSkuString[1];
  }    

  VariableSize = sizeof (RECORD_ME_VERSION);
  Status = gRT->SetVariable (
                  L"MeVersion",
                  &RecordMeVersionGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  VariableSize,
                  &MeVersion
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}