/** @file

  Icc Initialization Driver.

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
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Icc.c

Abstract:

  Icc Initialization Driver.

Revision History

--*/

#include "IccDxe.h"

/**
  This is the standard EFI driver point for the Platform Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param [in] ImageHandle         Standard entry point parameter.
  @param [in] SystemTable         Standard entry point parameter.

  @retval EFI_SUCCESS             Policy decisions set

**/
EFI_STATUS
EFIAPI
IccInitEntryPoint (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  CHIPSET_CONFIGURATION                 *SetupVariable;
  EFI_SETUP_UTILITY_PROTOCOL           *SetupUtility;
  EFI_BOOT_MODE                        BootMode;
  EFI_GUID                             GuidId = SYSTEM_CONFIGURATION_GUID;
  UINT8                                ProfileSoftStrap;
  UINT8                                Profile;
  EFI_EVENT                            ReadyToBootEvent;
  VOID                                 *Registration;
  UINT8                                IccFlag;
  UINT8                                ICCBackupProfile;
  DXE_PLATFORM_ICC_POLICY_PROTOCOL     *ICCPlatformPolicy;
  EFI_HECI_PROTOCOL                    *HECI;
  UINT32                               MEOperationMode;
  UINT32                               MEStatus;
//[-start-120301-IB07360173-modify]//
  DXE_MBP_DATA_PROTOCOL               *MBPDataProtocol;
//[-end-120301-IB07360173-modify]//
//[-start-120315-IB06460374-add]//
  UINT8                               SupportedProfilesNumber;  
  VOID                                *HobList;
  EFI_PEI_HOB_POINTERS                Hob;
  CHIPSET_CONFIGURATION                *SetupVariableHobData;
//[-end-120315-IB06460374-add]//

  ICCBackupProfile  = 0;
  ICCPlatformPolicy = NULL;
  HECI              = NULL;
  MEOperationMode   = 0;
  MEStatus          = 0;
//[-start-120301-IB07360173-remove]//
//  //
//  // If MebpSupportFlag is changed to "TRUE", it means this platfrom support MBP feature.
//  // BIOS could get some of ME information from MBP data.
//  // The otherwise, BIOS should use HECI message to get ME information.
//  //
//  MebpSupportFlag   = FALSE;
//  MbpData           = NULL;
//[-end-120301-IB07360173-remove]//

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **)&HECI
                  );
  ASSERT_EFI_ERROR (Status);

  Status = HECI->GetMeMode (&MEOperationMode);
  ASSERT_EFI_ERROR (Status);

  Status = HECI->GetMeStatus (&MEStatus);
  ASSERT_EFI_ERROR (Status);

  if ((MEOperationMode != ME_MODE_NORMAL) || (ME_STATUS_ME_STATE_ONLY (MEStatus) != ME_READY)) {
    DEBUG ((DEBUG_INFO | DEBUG_ERROR, "Skip ICC programming.\n"));

    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);
  
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);
  
  Status = GetBootMode(&BootMode);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gDxePlatformICCPolicyGuid, NULL, (VOID **)&ICCPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

//[-start-120301-IB07360173-modify]//
  Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **)&MBPDataProtocol);
//[-end-120301-IB07360173-modify]//
//[-start-120315-IB06460374-add]//
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID *) &HobList);
  Hob.Raw = GetNextGuidHob (&gSetupDefaultHobGuid, HobList);
  if (Hob.Raw != NULL) {
    SetupVariableHobData = GET_GUID_HOB_DATA (Hob.Guid);  
    if (SetupVariableHobData->SetupVariableInvalid) {
      SetupVariable->IccProfile = ICCPlatformPolicy->IccDefaultProfile;
    }
  }
//[-end-120315-IB06460374-add]//

  //
  // If ICCPlatformPolicy->FunctionEnabling->FCIM is set to ICC_ENABLE, the platform will execute in Full Clock Integration Mode.
  // Otherwise, the platform will execute in Buffer Through Mode.
  //
  if (ICCPlatformPolicy->FunctionEnabling->FCIM == ICC_ENABLE) {
    ICCPlatformPolicy->SetBackupProfile (&ICCBackupProfile);
    SetupVariable->IccBackupProfile = ICCBackupProfile;

    //
    // Reference PCH BIOS spec revision 0.8.0, we only get and set profile in FCIM mode.
    //
    //
    // check IccCapabilities
    //
//[-start-120301-IB07360173-modify]//
    Profile = MBPDataProtocol->MeBiosPayload.IccProfile.IccProfileIndex;
    ProfileSoftStrap = MBPDataProtocol->MeBiosPayload.IccProfile.IccProfileSoftStrap;
    SetupVariable->IccSupportedProfilesNumber = MBPDataProtocol->MeBiosPayload.IccProfile.NumIccProfiles;
//[-end-120301-IB07360173-modify]//    
//[-start-120315-IB06460374-add]//
    //
    // Workaround Start:
    // Workaround to get Icc profile data via HECI, due to the relative data in MBP is incorrect with ME Kit 9.0.0.1018.
    // These code should be removed after this issue is fixed by ME.
    //
    HeciGetIccProfile (&Profile, &ProfileSoftStrap, &SupportedProfilesNumber);
    SetupVariable->IccSupportedProfilesNumber = SupportedProfilesNumber;
//[-end-120315-IB06460374-add]//    
    if (ProfileSoftStrap >> 7) {
      //
      // If BIT7 is enable, support soft strap
      //
      SetupVariable->IccCapabilities = ICC_SOFT_STRAP_SUPPORT;
    } else {
      SetupVariable->IccCapabilities = ICC_BIOS_SUPPORT;
    }
    
    if (BootMode != BOOT_ON_S3_RESUME) {
      IccFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, IccWatchdogFlag);
      if (IccFlag == NOT_CLEAR_WATCHDOG_TIMER) {
        //
        // system will hang after set new profile, so we rollback profile setting from IccBackupProfile
        //
        SetupVariable->IccProfile = SetupVariable->IccBackupProfile;
      }
      
      if (SetupVariable->IccCapabilities == ICC_BIOS_SUPPORT) {
        //
        // if ICC Boot Profile is specified by BIOS
        //
        WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, IccWatchdogFlag, NOT_SET_WATCHDOG_TIMER);
        
        EfiCreateProtocolNotifyEvent (
          &gEfiWatchdogTimerArchProtocolGuid,
          TPL_CALLBACK,
          WatchDogNotifyFunction,
          NULL,
          &Registration
          );        
      }
    }
  }
  
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             SetIccLock,
             (VOID *)SetupVariable,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR(Status);
  
  Status = gRT->SetVariable (
                  L"Setup",
                  &GuidId,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//[-start-130709-IB05160465-modify]//
                  PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                  (VOID *) SetupVariable
                  );

  ICCSetClockEnables ();

  return EFI_SUCCESS;
}

/**
  This function will set the ICC profile

  @param [in] IccProfile          Standard entry point parameter.
  @param [in] SetupVariable       NV variable.

  @retval EFI_SUCCESS             Policy decisions set

**/
EFI_STATUS
SetIccProfile (
  IN UINT8                   IccProfile,
  IN CHIPSET_CONFIGURATION    *SetupVariable
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  UINT8                             Buffer;
  UINT8                             CmosNVFlag;

  CmosNVFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, SetupNVFlag);

  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
  ASSERT_EFI_ERROR (Status);

  if (SetupVariable->IccProfile != IccProfile) {
    //
    // back up current profile before set profile
    //
    if (CmosNVFlag != NV_NOT_FOUND) {
      SetupVariable->IccBackupProfile = IccProfile;
    }
    //
    // if current profile setting != user setting, set ICC profile to new setting.
    //
    Status = HeciSetIccProfile(SetupVariable->IccProfile);
    //
    // In PCH spec v.0.6.0 page 189, if selected profile is not the same as current profile,
    // BIOS can sends the new profile selection to ME and perform a power cycle reset afterward.
    //
    DEBUG ((DEBUG_INFO | DEBUG_ERROR, "Perform a global reset!\n"));
//[-start-120807-IB04770241-modify]//
    PchLpcPciCfg32Or (R_PCH_LPC_PM_INIT, B_PCH_LPC_GLOBAL_RESET_CF9);
//[-end-120807-IB04770241-modify]//
    Buffer = V_PCH_RST_CNT_FULLRESET;
    PciRootBridgeIo->Io.Write (
                          PciRootBridgeIo,
                          EfiPciWidthUint8,
                          R_PCH_RST_CNT,
                          1,
                          &Buffer
                          );
    CpuDeadLoop ();
  }
  
  return Status;
}

/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param [out] BootMode            Boot mode from PEI handoff HOB.

  @retval EFI_SUCCESS             Successfully get boot mode.
  @retval EFI_NOT_FOUND           Can not find the current system boot mode.

**/
EFI_STATUS
GetBootMode (
  OUT EFI_BOOT_MODE                    *BootMode
  )
{
  VOID        *HobList;
  EFI_STATUS  Status;

  //
  // Get Hob list
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Hob list not found\n"));
    *BootMode = 0;
    return EFI_NOT_FOUND;
  }
  
  *BootMode = GetBootModeHob ();

  return EFI_SUCCESS;
}

/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param [out] SetupVariable      NV variable.

  @retval EFI_SUCCESS             Policy decisions set.

**/
EFI_STATUS
SetIccWdt (
  IN CHIPSET_CONFIGURATION              *SetupVariable
  )
{
  UINT32           WdtAddress;
  UINT32           WdtCtlData;
  
  WdtAddress = (MmioRead32 (
                  MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_ACPI_BASE)
                  ) & B_PCH_LPC_ACPI_BASE_BAR) + R_PCH_OC_WDT_CTL;
  
  WdtCtlData = IoRead32 (WdtAddress);
  if (SetupVariable->IccWatchDog) {
    WdtCtlData |= B_PCH_OC_WDT_CTL_ICCSURV;
  } else {
    WdtCtlData &= ~B_PCH_OC_WDT_CTL_ICCSURV;
  }
  IoWrite32 (WdtAddress, WdtCtlData);
  
  return EFI_SUCCESS;
}

//[-start-120403-IB05300307-modify]//
/**
  Lock ICC registers.

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,
                                  which is implementation-dependent.

  @retval EFI_SUCCESS             disable complete..

**/
VOID
EFIAPI
SetIccLock (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  CHIPSET_CONFIGURATION              *SetupVariable;
  EFI_STATUS                        Status;
  ICC_LOCK_REGS_INFO                IccLockRegsInfo;
  
  SetupVariable = (CHIPSET_CONFIGURATION *)Context;
  if (SetupVariable->IccLockRegisters == ICC_LOCK_ALL_REG) {
    IccLockRegsInfo.RegBundles.BundlesCnt = ICC_LOCK_MASK_COUNT;
    IccLockRegsInfo.RegBundles.AU         = ICC_LOCK_ALL_REG;
    IccLockRegsInfo.RegMask[0]            = ICC_LOCK_ALL_MASK;
    IccLockRegsInfo.RegMask[1]            = ICC_LOCK_ALL_MASK;
    IccLockRegsInfo.RegMask[2]            = ICC_LOCK_ALL_MASK;
  } else {
    IccLockRegsInfo.RegBundles.BundlesCnt = ICC_LOCK_MASK_COUNT;
    IccLockRegsInfo.RegBundles.AU         = 1;
    IccLockRegsInfo.RegMask[0]            = ICC_LOCK_ALL_MASK;
    IccLockRegsInfo.RegMask[1]            = ICC_LOCK_STATIC_MASK;
    IccLockRegsInfo.RegMask[2]            = ICC_LOCK_ALL_MASK;
  }
  Status = HeciLockIccRegisters (ICC_LOCK_ACCESS_MODE_SET, ICC_RESPONSE_MODE_WAIT, &IccLockRegsInfo);
}
//[-end-120403-IB05300307-modify]//

/**
  Signal handlers for WatchDog event

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,
                                  which is implementation-dependent.

  @retval none

**/
VOID
EFIAPI
WatchDogNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *WatchDog;
  EFI_STATUS                        Status;
  UINT8                             Profile;
  UINT8                             ProfileSoftStrap;
  CHIPSET_CONFIGURATION              *SetupVariable;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
//[-start-120301-IB07360173-modify]//
  DXE_MBP_DATA_PROTOCOL             *MBPDataProtocol;
//[-end-120301-IB07360173-modify]//
//[-start-120315-IB06460374-add]//
  UINT8                             SupportedProfilesNumber;  
//[-end-120315-IB06460374-add]//

//[-start-120301-IB07360173-remove]//
//  MebpSupportFlag   = FALSE;
//  MbpData           = NULL;
//[-end-120301-IB07360173-remove]//

  Status = gBS->LocateProtocol (
                  &gEfiWatchdogTimerArchProtocolGuid,
                  NULL,
                  (VOID **)&WatchDog
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

//[-start-120301-IB07360173-modify]//
  Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **)&MBPDataProtocol);
  Profile = MBPDataProtocol->MeBiosPayload.IccProfile.IccProfileIndex;
  ProfileSoftStrap = MBPDataProtocol->MeBiosPayload.IccProfile.IccProfileSoftStrap;
//[-end-120301-IB07360173-modify]//
//[-start-120315-IB06460374-add]//
  //
  // Workaround Start:
  // Workaround to get Icc profile data via HECI, due to the relative data in MBP is incorrect with ME Kit 9.0.0.1018.
  // These code should be removed after this issue is fixed by ME.
  //
  HeciGetIccProfile (&Profile, &ProfileSoftStrap, &SupportedProfilesNumber);
//[-end-120315-IB06460374-add]//
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);

  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);
  

  //
  // Register watchdog callback function for check POST is wheter success
  //
  Status = WatchDog->RegisterHandler (WatchDog, WatchDogCallback);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Set watchdog timer
  //
  WatchDog->SetTimerPeriod (WatchDog, WATCHDOG_TIMER_TIME);
  
  //
  // set ICC profile 
  //
  Status = SetIccProfile (Profile, SetupVariable);
  ASSERT_EFI_ERROR(Status);
  //
  // Clear watchdog timer
  //
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, IccWatchdogFlag, CLEAR_WATCHDOG_TIMER_SUCCESS);
  WatchDog->SetTimerPeriod (WatchDog, 0);
  
  Status = SetIccWdt (SetupVariable);
  ASSERT_EFI_ERROR(Status);
}

/**
  A function of this type is called when the watchdog timer fires if a
  handler has been registered.

  @param [in] Time                The time in 100 ns units that has passed since the watchdog
                                  timer was armed.  For the notify function to be called, this
                                  must be greater than TimerPeriod.

  @retval none

**/
VOID
EFIAPI
WatchDogCallback (
  IN UINT64  Time
  )
{
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, IccWatchdogFlag, NOT_CLEAR_WATCHDOG_TIMER);
  gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
}

/**
  retrieves the number of currently used ICC clock profile

  @param [out] Profile            number of current ICC clock profile
  @param [out] ProfileSoftStrap
  

  @retval EFI_UNSUPPORTED

**/
EFI_STATUS
HeciGetIccProfileFunc (
  OUT UINT8                            *Profile,
  OUT UINT8                            *ProfileSoftStrap
  )
{
  EFI_STATUS              Status;
  ICC_GET_PROFILE_BUFFER  Buffer;
  UINT32                  CommandSize;
  UINT32                  ResponseSize;
  EFI_HECI_PROTOCOL       *Heci;
  UINT32                  MeMode;
  UINT32                  MeStatus;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **)&Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  DEBUG ((DEBUG_INFO, "(ICC) GetIccProfile\n"));
  CommandSize                         = sizeof (ICC_GET_PROFILE_MESSAGE);
  ResponseSize                        = sizeof (ICC_GET_PROFILE_RESPONSE);

  Buffer.message.Header.ApiVersion    = COUGAR_POINT_PLATFORM;
  Buffer.message.Header.IccCommand    = GET_ICC_PROFILE;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Status = Heci->SendwACK (
                   (UINT32 *) &Buffer,
                   CommandSize,
                   &ResponseSize,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_ICC_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "(ICC) GetIccProfile: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) GetIccProfile: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    Status = EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_INFO, "(ICC) GetIccProfile: Current profile = 0x%x\n", Buffer.response.IccProfileIndex));
  }

  if (Profile != NULL) {
    *Profile = Buffer.response.IccProfileIndex;
    *ProfileSoftStrap = Buffer.response.IccProfileSoftStrap;
  }

  return Status;
}

/**

  @param none

  @retval none

**/
VOID
ICCSetClockEnables (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_EVENT         Event;
  VOID              *Registration;

  Registration = NULL;

  DEBUG ((DEBUG_INFO | DEBUG_ERROR, "ICCSetClockEnables Entry\n"));

  //
  // We hope it to be executed after PCI enumeration
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ICCSetClockEnablesCallback,
                  NULL,
                  &Event
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiPciHostBridgeEndResourceAllocationNotifyGuid,
                    Event,
                    &Registration
                    );
    ASSERT_EFI_ERROR (Status);
  }
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO | DEBUG_ERROR, "ICCSetClockEnables Exit\n"));

  return;
}

/**  
  Enables/disables clocks. Used to turn off clocks in unused pci/pcie slots.

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,

  @retval    EFI_DEVICE_ERROR      Wrong response
  @retval    EFI_NOT_READY         ME is not ready
  @retval    EFI_INVALID_PARAMETER ResponseMode is invalid value
  @retval    EFI_SUCCESS           The function completed successfully.
  @exception EFI_UNSUPPORTED       Current ME mode doesn't support this function 

**/
VOID
EFIAPI
ICCSetClockEnablesCallback (
  IN      EFI_EVENT        Event,
  IN      VOID             *Context
  )
{
  EFI_STATUS                              Status;
  DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy;
  ICC_CLOCK_ENABLES_CONTROL_MASK          ClockEnables;
  ICC_CLOCK_ENABLES_CONTROL_MASK          ClockEnablesMask;

  ICCPlatformPolicy = NULL;

  DEBUG ((DEBUG_INFO | DEBUG_ERROR, "ICCSetClockEnablesCallback Entry\n"));

  Status = gBS->CloseEvent (Event);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gDxePlatformICCPolicyGuid, NULL, (VOID **)&ICCPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  ClockEnables.Fields.Flex0          = (ICCPlatformPolicy->ClockEnabling->Fields.Flex0      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Flex1          = (ICCPlatformPolicy->ClockEnabling->Fields.Flex1      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Flex2          = (ICCPlatformPolicy->ClockEnabling->Fields.Flex2      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Flex3          = (ICCPlatformPolicy->ClockEnabling->Fields.Flex3      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Reserved1      = (ICCPlatformPolicy->ClockEnabling->Fields.Reserved1  == ICC_DISABLE) ? 0 : 0x7;
  ClockEnables.Fields.PCI_Clock0     = (ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock0 == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PCI_Clock1     = (ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock1 == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PCI_Clock2     = (ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock2 == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PCI_Clock3     = (ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock3 == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PCI_Clock4     = (ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock4 == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Reserved2      = (ICCPlatformPolicy->ClockEnabling->Fields.Reserved2  == ICC_DISABLE) ? 0 : 0xF;
  ClockEnables.Fields.SRC0           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC0       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC1           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC1       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC2           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC2       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC3           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC3       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC4           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC4       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC5           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC5       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC6           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC6       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.SRC7           = (ICCPlatformPolicy->ClockEnabling->Fields.SRC7       == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.CSI_SRC8       = (ICCPlatformPolicy->ClockEnabling->Fields.CSI_SRC8   == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.CSI_DP         = (ICCPlatformPolicy->ClockEnabling->Fields.CSI_DP     == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PEG_A          = (ICCPlatformPolicy->ClockEnabling->Fields.PEG_A      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.PEG_B          = (ICCPlatformPolicy->ClockEnabling->Fields.PEG_B      == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.DMI            = (ICCPlatformPolicy->ClockEnabling->Fields.DMI        == ICC_DISABLE) ? 0 : 1;
  ClockEnables.Fields.Reserved3      = (ICCPlatformPolicy->ClockEnabling->Fields.Reserved3  == ICC_DISABLE) ? 0 : 0x7;

  ClockEnablesMask.Fields.Flex0      = (ICCPlatformPolicy->ClockEnablesMask->Fields.Flex0      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Flex1      = (ICCPlatformPolicy->ClockEnablesMask->Fields.Flex1      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Flex2      = (ICCPlatformPolicy->ClockEnablesMask->Fields.Flex2      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Flex3      = (ICCPlatformPolicy->ClockEnablesMask->Fields.Flex3      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Reserved1  = (ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved1  == ICC_DISABLE) ? 0 : 0x7;
  ClockEnablesMask.Fields.PCI_Clock0 = (ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock0 == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PCI_Clock1 = (ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock1 == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PCI_Clock2 = (ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock2 == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PCI_Clock3 = (ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock3 == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PCI_Clock4 = (ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock4 == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Reserved2  = (ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved2  == ICC_DISABLE) ? 0 : 0xF;
  ClockEnablesMask.Fields.SRC0       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC0       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC1       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC1       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC2       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC2       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC3       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC3       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC4       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC4       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC5       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC5       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC6       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC6       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.SRC7       = (ICCPlatformPolicy->ClockEnablesMask->Fields.SRC7       == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.CSI_SRC8   = (ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_SRC8   == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.CSI_DP     = (ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_DP     == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PEG_A      = (ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_A      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.PEG_B      = (ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_B      == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.DMI        = (ICCPlatformPolicy->ClockEnablesMask->Fields.DMI        == ICC_DISABLE) ? 0 : 1;
  ClockEnablesMask.Fields.Reserved3  = (ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved3  == ICC_DISABLE) ? 0 : 0x7;

  Status = HeciSetIccClockEnables (ClockEnables.Dword, ClockEnablesMask.Dword, ICC_RESPONSE_MODE_SKIP);
  DEBUG ((DEBUG_INFO | DEBUG_ERROR, "HeciSetIccClockEnables Status : %r\n", Status));

  DEBUG ((DEBUG_INFO | DEBUG_ERROR, "ICCSetClockEnablesCallback Exit\n"));

}
