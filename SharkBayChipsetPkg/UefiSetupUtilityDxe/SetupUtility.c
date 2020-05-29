/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SetupUtility.h>
#include <Protocol/SetupMouse.h>
#include <Protocol/RapidStartPlatformPolicy.h>
#include <Library/RapidStartDxeLib.h>
//[-start-130611-IB11120016-add]//
#include <Protocol/PlatformEventProtocolGuid.h>
#include <Library/DxeIpmiSetupUtilityLib.h>
//[-end-130611-IB11120016-add]//
//[-start-131129-IB09740048-add]//
#include <Library/CrConfigDefaultLib.h>
//[-end-131129-IB09740048-add]//
//[-start-130605-IB10930032-add]//
#include <Protocol/VariableEdit.h>
//[-end-130605-IB10930032-add]//
//[-start-140520-IB10300110-add]//
#include <Library/WatchDogLib.h>
//[-end-140520-IB10300110-add]//

UINT8 mFullResetFlag = 0;
//[-start-120301-IB03090376-add]//
BOOLEAN mGetSetupNvDataFailed = FALSE;
//[-end-120301-IB03090376-add]//

//
// Declare informaiton for EZH2O tool use
//
#define MAX_STRING_PACKAGE_NUMBER 2
#define DRIVER_VFR_NUMBER         2
#define LIBRARY_FVR_NUMBER        4
#define TOTAL_VFR_NUMBER          (DRIVER_VFR_NUMBER + LIBRARY_FVR_NUMBER)
#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  UINT32                         Reserved;
  UINT64                         UniqueId;
} HII_VENDOR_DEVICE_PATH_NODE;
#pragma pack()

typedef struct {
  HII_VENDOR_DEVICE_PATH_NODE    Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_TEMP_DEVICE_PATH;

//
// Hii vendor device path template
//
HII_TEMP_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
        (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
      },
      EFI_IFR_TIANO_GUID,
    },
    0,
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

/**
 The HII driver handle passed in for HiiDatabase.NewPackageList() requires
 that there should be DevicePath Protocol installed on it.
 This routine create a virtual Driver Handle by installing a vendor device
 path on it, so as to use it to invoke HiiDatabase.NewPackageList().

 @param [in]     DriverHandle         Handle to be returned

 @retval EFI_SUCCESS            Handle destroy success.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  )
{
  EFI_STATUS                   Status;
  HII_VENDOR_DEVICE_PATH_NODE  *VendorDevicePath;

  VendorDevicePath = AllocateCopyPool (sizeof (HII_TEMP_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
  if (VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use memory address as unique ID to distinguish from different device paths
  //
  VendorDevicePath->UniqueId = (UINT64) ((UINTN) VendorDevicePath);

  *DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  VendorDevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
/**
 Destroy the Driver Handle created by CreateHiiDriverHandle().

 @param [in]     DriverHandle   Handle returned by CreateHiiDriverHandle()

 @retval EFI_SUCCESS            Handle destroy success.
 @return other                  Handle destroy fail.

**/
EFI_STATUS
DestroyHiiDriverHandle (
  IN EFI_HANDLE                 DriverHandle
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  DevicePath
                  );
  gBS->FreePool (DevicePath);
  return Status;
}

#pragma pack(push, 1)
typedef struct {
  UINT8  Id[7];
  UINT8  HIIVersion;
  UINT32 EZH2OVersion;
  UINT32 StringPackageNumber;
  UINT64 VfrNumber[MAX_STRING_PACKAGE_NUMBER];
  UINT64 StringPackageoffset[MAX_STRING_PACKAGE_NUMBER];
  UINT64 VfrBinOffset[TOTAL_VFR_NUMBER];
  UINT64 VfrDefaultOffset[TOTAL_VFR_NUMBER];
//[-start-120221-IB07010064-add]//
  UINT64 StringPackageTotalSizeOffset[MAX_STRING_PACKAGE_NUMBER];
//[-end-120221-IB07010064-add]//
} EFI_IFR_PACKAGE_HEAD;
#pragma pack(pop)
EFI_IFR_PACKAGE_HEAD     IFR_PACKAGE_HEADER =
                           { {'$', 'I', 'F', 'R', 'P', 'K', 'G'},
                             0x21,
//[-start-120221-IB07010064-modify]//
                             0x02,
//[-end-120221-IB07010064-modify]//
                             MAX_STRING_PACKAGE_NUMBER,
                             {DRIVER_VFR_NUMBER, LIBRARY_FVR_NUMBER},
                             {(UINT64) (UINTN) SetupUtilityStrings,               (UINT64) (UINTN) SetupUtilityLibStrings},
                             {(UINT64) (UINTN) AdvanceVfrBin,                     (UINT64) (UINTN) PowerVfrBin,
                              (UINT64) (UINTN) MainVfrBin,                        (UINT64) (UINTN) SecurityVfrBin,
                              (UINT64) (UINTN) BootVfrBin,                        (UINT64) (UINTN) ExitVfrBin},
                             {(UINT64) (UINTN) AdvanceVfrSystemConfigDefault0000, (UINT64) (UINTN) PowerVfrSystemConfigDefault0000,
                              (UINT64) (UINTN) MainVfrSystemConfigDefault0000,    (UINT64) (UINTN) SecurityVfrSystemConfigDefault0000,
                              (UINT64) (UINTN) BootVfrSystemConfigDefault0000,    (UINT64) (UINTN) 0},
//[-start-120221-IB07010064-add]//
                             {(UINT64) (UINTN) &SetupUtilityStringsTotalSize,     (UINT64) (UINTN) &SetupUtilityLibStringsTotalSize}
//[-end-120221-IB07010064-add]//
                           };


USER_UNINSTALL_CALLBACK_ROUTINE mUninstallCallbackRoutine[] ={
  UninstallExitCallbackRoutine,     UninstallBootCallbackRoutine,
  UninstallPowerCallbackRoutine,    UninstallSecurityCallbackRoutine,
  UninstallAdvanceCallbackRoutine,  UninstallMainCallbackRoutine
  };


EFI_GUID  mFormSetGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID  mFormSetClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;

CHAR16    mVariableName[] = L"SystemConfig";
EFI_HII_HANDLE                   mDriverHiiHandle;

EFI_STATUS
InitSetupUtilityBrowser(
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
);

SETUP_UTILITY_BROWSER_DATA                   *gSUBrowser;
UINT16                                       gCallbackKey;

EFI_STATUS
EFIAPI
SetupUtilityInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_DATA                    *SetupData;
//[-start-130605-IB10930032-add]//
  VARIABLE_EDIT_PROTOCOL               *VariableEditor;
  EFI_HANDLE                            Handle;
//[-end-130605-IB10930032-add]//
//[-start-120323-IB07010077-add]//
  UINT32                                DummyVersion;
//[-end-120323-IB07010077-add]//
  EFI_EVENT                             Event;
  VOID                                  *Registration;

//[-start-130605-IB10930032-add]//
  VariableEditor   =  NULL;
  Handle           =  NULL;
//[-end-130605-IB10930032-add]//

//[-start-120323-IB07010077-add]//
  //
  // Do not remove, it is for compiler optimization
  //
  DummyVersion = IFR_PACKAGE_HEADER.EZH2OVersion;
//[-end-120323-IB07010077-add]//
  //
  // Initialize the library.
  //

  CheckLanguage ();

  SetupData = AllocatePool (sizeof(SETUP_UTILITY_DATA));
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetupData->Signature                    = EFI_SETUP_UTILITY_SIGNATURE;
  SetupData->SetupUtility.StartEntry      = NULL;
  SetupData->SetupUtility.PowerOnSecurity = PowerOnSecurity;
  mFullResetFlag = 0;

  //
  // The FirstIn flag is for that if there is the first time entering SCU, we should install
  // Vfr of Menu to Hii database.
  // After that, we shouldn't install the Vfr to Hii database again.
  //
  SetupData->SetupUtility.FirstIn         = TRUE;
  Status = GetSystemConfigurationVar (SetupData);

//[-start-130611-IB11120016-add]//
  IpmiConfigInit (SetupData->SetupUtility.SetupNvData);
//[-end-130611-IB11120016-add]//
  //[-start-131129-IB09740048-add]//
  //
  // Initial CRconfig variable
  //
  CrConfigVarInit();
  //[-end-131129-IB09740048-add]//
  //
  // Install Setup Utility
  //
  SetupData->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                 &SetupData->Handle,
                 &gEfiSetupUtilityProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &SetupData->SetupUtility
                 );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (SetupData);
    return Status;
  }

//[-start-130605-IB10930032-add]//
  VariableEditor = AllocatePool (sizeof (VARIABLE_EDIT_PROTOCOL));
  if (VariableEditor == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VariableEditor->InstallSetupHii = InstallSetupHii;
  Status = gBS->InstallProtocolInterface (
                   &Handle,
                   &gVariableEditProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   VariableEditor
                   );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (VariableEditor);
    return Status;
  }
//[-end-130605-IB10930032-add]//
  //
  // When execute Setup Utility application, install HII data
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK - 1,
                  SetupUtilityNotifyFn,
                  NULL,
                  &Event
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiSetupUtilityApplicationProtocolGuid,
                    Event,
                    &Registration
                    );
  }

  return Status;



}

//[-start-111124-IB10930032-add]//
EFI_STATUS
EFIAPI
InstallSetupHii (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This,
  IN  BOOLEAN                               InstallHii
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;

  if (InstallHii) {
    Status = gBS->LocateProtocol (
                    &gEfiSetupUtilityBrowserProtocolGuid,
                    NULL,
                    (VOID **)&Interface
                    );
    //
    // If there was no error, assume there is an installation and fail to load
    //
    if (EFI_ERROR(Status) ) {
      Status = InitSetupUtilityBrowser(This);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = InstallSetupUtilityBrowserProtocol (This);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      ZeroMem (gSUBrowser->SUCInfo->MapTable, sizeof (HII_HANDLE_VARIABLE_MAP_TABLE) * MAX_HII_HANDLES);
  #ifdef HDD_PASSWORD_SUPPORT
  //    InitHddPasswordInfoForSCU ();
  #endif


//[-start-111124-IB08620016-modify]//
      Status = InstallHiiData ();
//[-end-111124-IB08620016-modify]//

      This->FirstIn = FALSE;

      if (EFI_ERROR(Status)) {
        return Status;
      }

      //
      // Load the variable data records
      //
      gSUBrowser->Interface.SCBuffer = (UINT8 *) gSUBrowser->SCBuffer;
    }
  } else {
    Status = RemoveHiiData (
               SetupUtilityStrings,
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[ExitHiiHandle]),
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[BootHiiHandle]),
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[PowerHiiHandle]),
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle]),
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle]),
               (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[MainHiiHandle]),
               NULL
               );

    Status = UninstallSetupUtilityBrowserProtocol ();
  }

  return Status;
}
//[-end-111124-IB10930032-add]//


EFI_STATUS
CreateScuData (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  UINTN                                     BufferSize;
  UINT8                                     *Lang;
  EFI_SETUP_UTILITY_PROTOCOL                *This;

  POST_CODE (BDS_ENTER_SETUP); //PostCode = 0x29, Enter Setup Menu
  //
  // There will be only one DeviceManagerSetup in the system.
  // If there is another out there, someone is trying to install us
  // again.  We fail in that scenario.
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  //
  // If there was no error, assume there is an installation and fail to load
  //
  if (EFI_ERROR(Status) ) {
    Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InitSetupUtilityBrowser(This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InstallSetupUtilityBrowserProtocol (This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    ZeroMem (gSUBrowser->SUCInfo->MapTable, sizeof (HII_HANDLE_VARIABLE_MAP_TABLE) * MAX_HII_HANDLES);
    InitHddPasswordInfoForSCU ();
//[-start-110830-IB07370065-add]//
    PlugInVgaUpdateInfo ();
//[-end-110830-IB07370065-add]//
//[-start-120222-IB10030005-add]//
    if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
      DisableQuietBoot ();
    }
//[-end-120222-IB10030005-add]//
    Status = InstallHiiData ();
    This->FirstIn = FALSE;
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Load the variable data records
    //
    gSUBrowser->Interface.SCBuffer = (UINT8 *) gSUBrowser->SCBuffer;
  }
  Lang = GetVariableAndSize (
           L"PlatformLang",
           &gEfiGlobalVariableGuid,
           &BufferSize
           );
  if (Lang != NULL) {
    Status = gRT->SetVariable (
                    L"BackupPlatformLang",
//[-start-120326-IB02960435-modify]//
                    &gEfiGenericVariableGuid,
//[-end-120326-IB02960435-modify]//
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Lang
                    );
    gBS->FreePool (Lang);
  }

//[-start-120607-IB02960452-modify]//
//[-start-130611-IB11120016-modify]//
  EfiNamedEventSignal (&gStartOfSetupUtilityProtocolGuid);

  return Status;
}

/**
  Read data from Sensitive setup variable and synchronize these data to input setup config instance.
  If the sensitive variable doesn't exist, synchronize data from setup config data to sensitive setup variable.

  @param [in] SetupNvData        Points to CHIPSET_CONFIGURATION instance.

  @retval EFI_SUCCESS            Sync setup data with sensitive variables successfully.
  @retval EFI_INVALID_PARAMETER  SetupNvData is NULL.
**/
EFI_STATUS
SyncSetupDataWithSensitiveVariable (
  IN OUT CHIPSET_CONFIGURATION        *SetupNvData
  )
{
  EFI_STATUS            Status;
  UINTN                 TokenNum;
  CHAR16                *ConfigName;
  UINT32                OffsetAndSize;
  UINTN                 Offset;
  UINTN                 DataSize;
  UINT8                 *Data;

  //
  // Sync setup config sensitive variables
  //
  TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, 0);
  while (TokenNum != 0) {
    ConfigName = (CHAR16 *) LibPcdGetExPtr(&gSetupConfigNameGuid, TokenNum);
    OffsetAndSize = LibPcdGetEx32 (&gSetupConfigOffsetSizeGuid, TokenNum);
    Offset        = (UINTN) ((OffsetAndSize >> 16) & 0xFFFF);
    DataSize      = (UINTN) (OffsetAndSize & 0xFFFF);
    if (ConfigName != NULL && DataSize != 0) {
      Data = CommonGetVariableData (
                 ConfigName,
                 &mFormSetGuid
                 );
      if (Data == NULL) {
        Status = gRT->SetVariable (
                        ConfigName,
                        &mFormSetGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        DataSize,
                        (VOID *) ((UINT8 *) SetupNvData + Offset)
                        );
        ASSERT_EFI_ERROR (Status);
      } else {
        //
        // 1. Needn't synchronize BootType variable, since any change for boot type will pop-up message
        // in BDS phase for user to indicate this setting is modified by other tools.
        // 2. To prevent from BootType is modified to incorrect value, synchronize from BootType variable if the
        // content is incorrect.
        //
        if (StrCmp (ConfigName, L"BootType") == 0) {
          if (*((UINT8 *) SetupNvData + Offset) > EFI_BOOT_TYPE) {
            CopyMem ((UINT8 *) SetupNvData + Offset, Data, DataSize);
          }
        } else {
          CopyMem ((UINT8 *) SetupNvData + Offset, Data, DataSize);
        }
        FreePool (Data);
      }
    }
    TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, TokenNum);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
DestroyScuData (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  UINT8                                     *Lang;
  EFI_SETUP_UTILITY_PROTOCOL                *SetupUtility;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_SETUP_UTILITY_PROTOCOL                *This;

  EfiNamedEventSignal (&gEndOfSetupUtilityProtocolGuid);
//[-end-130611-IB11120016-modify]//
  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  SysPasswordService->LockPassword (SysPasswordService);
//[-end-120607-IB02960452-modify]//

  Status = RemoveHiiData (
             SetupUtilityStrings,
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[ExitHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[BootHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[PowerHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[MainHiiHandle]),
             NULL
             );

  Status = UninstallSetupUtilityBrowserProtocol ();
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &This);
  if (!EFI_ERROR(Status)) {
    This->FirstIn = TRUE;
  }
  gSUBrowser = NULL;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  gST->ConOut->ClearScreen (gST->ConOut);

  Lang = GetVariableAndSize (
           L"BackupPlatformLang",
//[-start-120326-IB02960435-modify]//
           &gEfiGenericVariableGuid,
//[-end-120326-IB02960435-modify]//
           &BufferSize
           );
  if (Lang != NULL) {
    Status = gRT->SetVariable (
                    L"PlatformLang",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Lang
                    );
    gBS->FreePool (Lang);

    gRT->SetVariable (
           L"BackupPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           0,
           NULL
           );
  }

//[-start-130710-IB05160465-modify]//
  //
  // restore NVvMemory map of SetupData from rom
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  if (!EFI_ERROR(Status)) {
    BufferSize = PcdGet32 (PcdSetupConfigSize);
    Status = gRT->GetVariable (
                    L"Setup",
                    &mFormSetGuid,
                    NULL,
                    &BufferSize,
                    SetupUtility->SetupNvData
                    );
    SyncSetupDataWithSensitiveVariable ((CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData);

  }
//[-end-130710-IB05160465-modify]//

  return Status;
}

BOOLEAN
AsfResetSetupData (
  IN  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
  EFI_ASF_BOOT_OPTIONS                *mAsfBootOptions;

  //
  // Get Protocol for ASF
  //
  Status = gBS->LocateProtocol (
                      &gEfiAlertStandardFormatProtocolGuid,
                      NULL,
                      (VOID **)&Asf
                      );

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Error gettings ASF protocol -> %r\n", Status));
    return FALSE;
  }

  Status = Asf->GetBootOptions(Asf, &mAsfBootOptions);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (mAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
    return FALSE;
  }

  if ((mAsfBootOptions->BootOptions & CONFIG_DATA_RESET) == CONFIG_DATA_RESET) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
PowerOnSecurity (
  IN  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility
  )
{
  EFI_STATUS                            Status;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_GUID                              StringPackGuid = STRING_PACK_GUID;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo = NULL;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  EFI_HANDLE                            DriverHandle;
  VOID                                  *Table;
  UINT8                                 *HobSetupData;
  VOID                                  *HobList = NULL;
  EFI_BOOT_MODE                         BootMode;
  SEC_HDD_PASSWORD_STRUCT               *SavePasswordPoint;

  HiiHandle = 0;

  //
  // There will be only one DeviceManagerSetup in the system.
  // If there is another out there, someone is trying to install us
  // again.  We fail in that scenario.
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );

  //
  // If there was no error, assume there is an installation and fail to load
  //
  if (!EFI_ERROR(Status) ) {
    Status = UninstallSetupUtilityBrowserProtocol ();
  }

  Status = InitSetupUtilityBrowser(SetupUtility);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallSetupUtilityBrowserProtocol (SetupUtility);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  SUCInfo = gSUBrowser->SUCInfo;
  Status = CreateHiiDriverHandle (&DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = HiiAddPackages (&StringPackGuid, DriverHandle, SecurityVfrBin ,SetupUtilityLibStrings, NULL);
  ASSERT(HiiHandle != NULL);

  gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle = HiiHandle;
  gSUBrowser->Interface.SCBuffer = (UINT8 *) gSUBrowser->SCBuffer;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&SUCInfo->SysPasswordService
                  );

//[-start-120615-IB08520049-add]//
  SUCInfo->SupervisorPassword = NULL;
  SUCInfo->UserPassword       = NULL;
//[-end-120615-IB08520049-add]//

  if (!EFI_ERROR(Status)) {

    //
    // Check password
    //
//[-start-120615-IB08520049-remove]//
//    SUCInfo->SupervisorPassword = NULL;
//    SUCInfo->UserPassword       = NULL;
//[-end-120615-IB08520049-remove]//
    BootMode = GetBootModeHob();
    if (BootMode == BOOT_IN_RECOVERY_MODE) {
//[-start-130110-IB10820219-add]//
      HobList = GetHobList ();
//[-end-130110-IB10820219-add]//
      Table= GetNextGuidHob (&gEfiPowerOnPwSCUHobGuid, HobList);
      if (Table !=NULL) {
        HobSetupData = ((UINT8 *) Table) + sizeof (EFI_HOB_GUID_TYPE);
        ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->PowerOnPassword = ((CHIPSET_CONFIGURATION *) HobSetupData)->PowerOnPassword;
      }
    }
    if ((((CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer)->PowerOnPassword) == POWER_ON_PASSWORD) {
      Status = PasswordCheck (
                 SUCInfo,
                 (KERNEL_CONFIGURATION *) gSUBrowser->SCBuffer
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }
  SUCInfo->HarddiskPassword  = NULL;
  SUCInfo->NumOfHarddiskPswd = 0;
  Status = InitHddPasswordInfoForSCU ();
  if (!EFI_ERROR (Status)) {
  Status = HddPasswordCheck (
             HiiHandle,
             INTO_BOOT,
              (KERNEL_CONFIGURATION *)gSUBrowser->SCBuffer,
             &SavePasswordPoint
             );
  }

  if (SUCInfo->HarddiskPassword  != NULL) {
    if (SUCInfo->HarddiskPassword->NumOfEntry != 0) {
      gBS->FreePool (SUCInfo->HarddiskPassword);
      SUCInfo->HarddiskPassword   = NULL;
      SUCInfo->NumOfHarddiskPswd  = 0;
     }
  }

  if (SUCInfo->SupervisorPassword  != NULL) {
    if (SUCInfo->SupervisorPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->SupervisorPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->SupervisorPassword);
      SUCInfo->SupervisorPassword = NULL;
     }
  }

  if (SUCInfo->UserPassword  != NULL) {
    if (SUCInfo->UserPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->UserPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->UserPassword);
      SUCInfo->UserPassword = NULL;
     }
  }

  gSUBrowser->HiiDatabase->RemovePackageList (gSUBrowser->HiiDatabase, HiiHandle);
  DestroyHiiDriverHandle (DriverHandle);
  Status = UninstallSetupUtilityBrowserProtocol ();

  return Status;
}

/**
 Installs the SetupUtilityBrowser protocol including allocating
 storage for variable record data.

 @param [in]   This

 @retval EFI_SUCEESS            Protocol was successfully installed
 @retval EFI_OUT_OF_RESOURCES   Not enough resource to allocate data structures
 @return Other                  Some other error occured

**/
EFI_STATUS
InstallSetupUtilityBrowserProtocol (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
  )
{
  EFI_STATUS                                 Status;

  gSUBrowser->Interface.AtRoot        = TRUE;
  gSUBrowser->Interface.Finished      = FALSE;
  gSUBrowser->Interface.Guid          = &mFormSetGuid;
  gSUBrowser->Interface.UseMenus      = FALSE;
  gSUBrowser->Interface.Direction     = NoChange;
  gSUBrowser->Interface.CurRoot       = FALSE;
  gSUBrowser->Interface.MenuItemCount = FALSE;
//[-start-130709-IB05160465-modify]//
  gSUBrowser->Interface.Size          = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  gSUBrowser->Interface.Firstin       = TRUE;
  gSUBrowser->Interface.Changed       = FALSE;
  gSUBrowser->Interface.JumpToFirstOption = TRUE;

  gSUBrowser->Interface.SCBuffer      = NULL;
  gSUBrowser->Interface.MyIfrNVData = NULL;
  gSUBrowser->Interface.PreviousMenuEntry = 0;
  //
  // Install the Protocol
  //
  gSUBrowser->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &gSUBrowser->Handle,
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gSUBrowser->Interface
                  );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
 Uninstalls the DeviceManagerSetup protocol and frees memory
 used for storing variable record data.

 @param None

 @retval EFI_SUCEESS            Protocol was successfully installed
 @retval EFI_ALREADY_STARTED    Protocol was already installed
 @retval EFI_OUT_OF_RESOURCES   Not enough resource to allocate data structures
 @return Other                  Some other error occured

**/
EFI_STATUS
UninstallSetupUtilityBrowserProtocol (
  VOID
  )
{
  EFI_STATUS                                Status;

  Status = gBS->UninstallProtocolInterface (
                  gSUBrowser->Handle,
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  &gSUBrowser->Interface
                  );

  if ( EFI_ERROR(Status) ) {
    return Status;
  }
  if (gSUBrowser->SUCInfo->HarddiskPassword  != NULL) {
    if (gSUBrowser->SUCInfo->HarddiskPassword->NumOfEntry != 0) {
      gBS->FreePool (gSUBrowser->SUCInfo->HarddiskPassword);
    }
  }
  if (gSUBrowser->SUCInfo->SupervisorPassword  != NULL) {
    if (gSUBrowser->SUCInfo->SupervisorPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->SupervisorPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->SupervisorPassword);
    }
  }

  if (gSUBrowser->SUCInfo->UserPassword  != NULL) {
    if (gSUBrowser->SUCInfo->UserPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->UserPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->UserPassword);
    }
  }

  gBS->FreePool (gSUBrowser->SUCInfo);
  gBS->FreePool (gSUBrowser);

  return EFI_SUCCESS;
}

/**
 Call the browser and display the SetupUtility

 @param None

**/
EFI_STATUS
CallSetupUtilityBrowser (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  EFI_HANDLE                                DispPage;
  BOOLEAN                                   SetupUtilityBrowserEmpty;
  BOOLEAN                                   Continue;
  EFI_BROWSER_ACTION_REQUEST                ResetRequired;
  STRING_REF                                TempToken;
  UINTN                                     Index;
  EFI_HII_PACKAGE_LIST_HEADER               *Buffer;
  EFI_HII_DATABASE_PROTOCOL                 *HiiDatabase;
  EFI_FORM_BROWSER2_PROTOCOL                 *Browser2;
  EFI_SETUP_MOUSE_PROTOCOL                   *SetupMouse;
  EFI_IFR_FORM_SET                          *FormSetPtr;
  UINT16                                     Class;
  UINT16                                     SubClass;
  EFI_GUID                                   FormSetGuid = EFI_HII_PLATFORM_SETUP_FORMSET_GUID;
  UINT8                                      *TempPtr;


  Status = EFI_SUCCESS;
  SetupUtilityBrowserEmpty = TRUE;
  Buffer = NULL;
  gCallbackKey = 0;
  Continue = TRUE;
  HiiDatabase = gSUBrowser->HiiDatabase;
  Browser2    = gSUBrowser->Browser2;
  SetupMouse  = NULL;

  for (Index = 0, BufferSize = 0; Index < MAX_HII_HANDLES && gSUBrowser->SUCInfo->MapTable[Index].HiiHandle != NULL; Index++) {
    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            gSUBrowser->SUCInfo->MapTable[Index].HiiHandle,
                            &BufferSize,
                            Buffer
                            );

    if (Status == EFI_NOT_FOUND) {
      break;
    }

    //
    // BufferSize should have the real size of the forms now
    //
    Buffer = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);

    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            gSUBrowser->SUCInfo->MapTable[Index].HiiHandle,
                            &BufferSize,
                            Buffer
                            );


    //
    // Skips the header, now points to data
    //

    TempPtr = (UINT8 *) (Buffer + 1);
    TempPtr = (UINT8 *) ((EFI_HII_FORM_PACKAGE_HDR *) TempPtr + 1);
    FormSetPtr = (EFI_IFR_FORM_SET *) TempPtr;
    //
    // If this formset belongs in the device manager, add it to the menu
    //
    //
    // check Platform form set guid
    //
    if (CompareGuid ((EFI_GUID *) (FormSetPtr + sizeof (EFI_IFR_FORM_SET)), &FormSetGuid)) {
      TempPtr = (UINT8 *) ((EFI_IFR_FORM_SET *) TempPtr + 1);
      Class = ((EFI_IFR_GUID_CLASS *) TempPtr)->Class;
      TempPtr += sizeof (EFI_IFR_GUID_CLASS);
      SubClass = ((EFI_IFR_GUID_SUBCLASS *) TempPtr)->SubClass;
      if (Class == EFI_NON_DEVICE_CLASS) {
        continue;
      } else {
        if (SubClass == EFI_USER_ACCESS_THREE) {
          if ((((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SetUserPass == 1) && (((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->UserAccessLevel == 3)) {
            BufferSize = 0;
            gBS->FreePool (Buffer);
            continue;
          };
        }

        if (SubClass == EFI_USER_ACCESS_TWO) {
          if ((((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SetUserPass == 1) && (((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->UserAccessLevel == 2)) {
            BufferSize = 0;
            gBS->FreePool(Buffer);
            continue;
          }
        }
      }
    }


    SetupUtilityBrowserEmpty = FALSE;


    if (gSUBrowser->Interface.MenuItemCount < MAX_ITEMS) {
      TempToken = FormSetPtr->FormSetTitle;
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].MenuTitle = TempToken;
      //
      // Set the display page.  Last page found is the first to be displayed.
      //
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].Page = gSUBrowser->SUCInfo->MapTable[Index].HiiHandle;
      //
      // NULL out the string pointer
      //
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].String = NULL;
      //
      // Entry is filled out so update count
      //
      gSUBrowser->Interface.MenuItemCount++;
    }
    BufferSize = 0;
    gBS->FreePool(Buffer);
  }

  //
  // Drop the TPL level from TPL_APPLICATION+2 to TPL_APPLICATION
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  //
  // If we didn't add anything - don't bother going to device manager
  //
  if (!SetupUtilityBrowserEmpty) {
    Status = gBS->LocateProtocol (
                    &gSetupMouseProtocolGuid,
                    NULL,
                    (VOID **)&(gSUBrowser->Interface.SetupMouse)
                    );
    if (!EFI_ERROR (Status)) {
      gSUBrowser->Interface.SetupMouseFlag = TRUE;
      SetupMouse = (EFI_SETUP_MOUSE_PROTOCOL *) gSUBrowser->Interface.SetupMouse;
      Status = SetupMouse->Start (SetupMouse);
      if (EFI_ERROR (Status)) {
        SetupMouse = NULL;
        gSUBrowser->Interface.SetupMouseFlag = FALSE;
      }
    }
    //
    // Init before root page loop
    //
    gSUBrowser->Interface.AtRoot    = TRUE;
    gSUBrowser->Interface.CurRoot   = gSUBrowser->Interface.MenuItemCount - 1;
    gSUBrowser->Interface.UseMenus  = TRUE;
    gSUBrowser->Interface.Direction = NoChange;
    DispPage                       = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount - 1].Page;

    //
    // Loop until exit condition is found.  Use direction indicators and
    // the menu list to determine what root page needs to be displayed.
    //
    while (Continue) {
      Status = Browser2->SendForm (
                           Browser2,
                           (EFI_HII_HANDLE *) &DispPage,
                           1,
                           (FeaturePcdGet (PcdH2OFormBrowserSupported)) ? &mFormSetClassGuid : &mFormSetGuid,
                           1,
                           NULL,
                           &ResetRequired
                           );

      if (ResetRequired == EFI_BROWSER_ACTION_REQUEST_RESET) {
        gBS->RaiseTPL (TPL_NOTIFY);
        gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      } else if (ResetRequired == EFI_BROWSER_ACTION_REQUEST_EXIT) {
        gSUBrowser->Interface.UseMenus = FALSE;
        break;
      }

      //
      // Force return to Device Manager or Exit if finished
      //
      gSUBrowser->Interface.AtRoot  = TRUE;
      gSUBrowser->Interface.Firstin = FALSE;
      if (gSUBrowser->Interface.Finished) {
        // Need to set an exit at this point
        gSUBrowser->Interface.UseMenus = FALSE;
        Continue = FALSE;
        break;
      }

      //
      // Check for next page or exit states
      //
      switch (gSUBrowser->Interface.Direction) {

     case Right:
        //
        // Update Current Root Index
        //
        if (gSUBrowser->Interface.CurRoot == 0) {
          gSUBrowser->Interface.CurRoot = gSUBrowser->Interface.MenuItemCount - 1;
        } else {
          gSUBrowser->Interface.CurRoot--;
        }
        //
        // Set page to display
        //
        DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        //
        // Update Direction Flag
        //
        gSUBrowser->Interface.Direction = NoChange;
        break;

      case Left:
        //
        // Update Current Root Index
        //
        if (gSUBrowser->Interface.CurRoot == gSUBrowser->Interface.MenuItemCount - 1) {
          gSUBrowser->Interface.CurRoot = 0;
        } else {
          gSUBrowser->Interface.CurRoot++;
        }
        //
        // Set page to display
        //
        DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        //
        // Update Direction Flag
        //
        gSUBrowser->Interface.Direction = NoChange;
        break;

      case Jump:
        //
        // Update Current Root Index
        //
//[-start-140708-IB05080434-modify]//
        if (gSUBrowser->Interface.CurRoot <= gSUBrowser->Interface.MenuItemCount - 1) {
//[-end-140708-IB05080434-modify]//
          //
          // Set page to display
          //
          DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        }
        gSUBrowser->Interface.Direction = NoChange;
        break;

      default:
        break;
      }

    }
  }
  if ((SetupMouse != NULL) && (gSUBrowser->Interface.SetupMouseFlag == TRUE)) {
    Status = SetupMouse->Close (SetupMouse);
  }
  //
  // We are exiting so clear the screen
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BLACK | EFI_BACKGROUND_BLACK);
  gST->ConOut->ClearScreen (gST->ConOut);

  gBS->RaiseTPL (TPL_APPLICATION+2);   // TPL_APPLICATION+2 = EFI_TPL_DRIVER
  return Status;
}

/**
  Function to delete all of setup related variables

  @retval EFI_SUCCESS            Delete all of setup related variable successfully.
**/
EFI_STATUS
ClearSetupSettings (
  VOID
  )
{
  UINTN                 TokenNum;
  CHAR16                *ConfigName;
  UINT32                OffsetAndSize;
  //
  // clear Setup Variable
  //
  gRT->SetVariable (
         L"Setup",
         &mFormSetGuid,
         0,
         0,
         NULL
         );
  //
  // clear all sensitive setup variable
  //
  TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, 0);
  while (TokenNum != 0) {
    ConfigName    = (CHAR16 *) LibPcdGetExPtr(&gSetupConfigNameGuid, TokenNum);
    OffsetAndSize = LibPcdGetEx32 (&gSetupConfigOffsetSizeGuid, TokenNum);
    if ((ConfigName != NULL) && ((OffsetAndSize & 0xFFFF) != 0)) {
      gRT->SetVariable (
             ConfigName,
             &mFormSetGuid,
             0,
             0,
             NULL
             );
    }
    TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, TokenNum);
  }

  return EFI_SUCCESS;
}




EFI_STATUS
GetSystemConfigurationVar (
  IN SETUP_UTILITY_DATA                     *SetupData
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  CHIPSET_CONFIGURATION                      *SetupNvData;
#ifdef RECLAIM_SUPPORT_FLAG
  UINTN                                     Index=0;
#endif
  UINT16                                    Timeout;

  //
  // Check the setup variable was create or not, if not then create default setup variable.
  //
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//

  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID **)&SetupNvData
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ZeroMem (SetupNvData, BufferSize);

  //
  // Save default setup by variable
  //
  if (FeaturePcdGet(PcdXtuSupported)){
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  Status = gRT->GetVariable (
                L"SetupDefault",
                &mFormSetGuid,
                NULL,
                &BufferSize,
                (VOID *) SetupNvData
                );

  if (EFI_ERROR (Status)) {
    DefaultSetup (SetupNvData);
    SetupRuntimeDetermination ( SetupNvData );
  }
  Status = gRT->SetVariable (
                  L"SetupDefault",
                  &mFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *)SetupNvData
                  );
  }

  //
  // Check "Setup" variable is exist or not...
  //
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  if (AsfResetSetupData() == FALSE) {
    Status = gRT->GetVariable (
                    L"Setup",
                    &mFormSetGuid,
                    NULL,
                    &BufferSize,
                    (VOID *) SetupNvData
                    );
  } else {
    ClearSetupSettings ();
    Status = EFI_NOT_FOUND;
  }

  if (EFI_ERROR (Status)) {
    //
    // "Setup" Variable doesn't exist,so get a buffer with default variable
    //
//[-start-120301-IB03090376-add]//
    mGetSetupNvDataFailed = TRUE;
//[-end-120301-IB03090376-add]//
    DefaultSetup (SetupNvData);
    gRT->SetVariable (
           L"PlatformLang",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
           (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
           );

    Timeout = (UINT16) PcdGet16 (PcdPlatformBootTimeOut);
    gRT->SetVariable (
           L"Timeout",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           sizeof (UINT16),
           (VOID *) &Timeout
           );

//[-start-120301-IB03090376-add]//
    mGetSetupNvDataFailed = FALSE;
//[-end-120301-IB03090376-add]//
  }

  SetupRuntimeDetermination ( SetupNvData );
  SyncSetupDataWithSensitiveVariable (SetupNvData);
  SetupNvData->DefaultBootType = DEFAULT_BOOT_FLAG;

  //
  // Save Setup variable.
  //
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  Status = SaveSetupConfig (
              L"Setup",
             &mFormSetGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             BufferSize,
             (VOID *) SetupNvData
             );
  //
  // Set Quick boot setting to variable, common code can use this setting to
  // do quick boot or not.
  //
  gRT->SetVariable (
         L"QuickBoot",
//[-start-120326-IB02960435-modify]//
         &gEfiGenericVariableGuid,
//[-end-120326-IB02960435-modify]//
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (SetupNvData->QuickBoot),
         (VOID *) &SetupNvData->QuickBoot
         );
#ifdef RECLAIM_SUPPORT_FLAG
  if (SetupNvData->ReclaimLongrunTest) {
    for (Index = 0; Index < FAST_RECLAIM_COUNT; Index += 1) {
      SetupNvData->NumLock = ~SetupNvData->NumLock;
      gRT->SetVariable (
                  L"Setup",
                  &mFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *) SetupNvData
                  );
    }
  }
#endif
  SetupData->SetupUtility.SetupNvData = (UINT8 *) SetupNvData;

  //
  // Check "Custom" variable is exist or not...
  //
  SetupNvData = AllocateZeroPool (BufferSize);
//[-start-130207-IB10870073-add]//
  ASSERT (SetupNvData != NULL);
  if (SetupNvData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  Status = gRT->GetVariable (
                  L"Custom",
                   &mFormSetGuid,
                  NULL,
                  &BufferSize,
                  (VOID *)SetupNvData
                  );

  if (EFI_ERROR (Status)) {
    //
    // "Custom" Variable doesn't exist,so get a buffer with default variable
    //
//[-start-120301-IB03090376-add]//
    mGetSetupNvDataFailed = TRUE;
//[-end-120301-IB03090376-add]//
    DefaultSetup (SetupNvData);
    gRT->SetVariable (
           L"CustomPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
           (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
           );
//[-start-120301-IB03090376-add]//
    mGetSetupNvDataFailed = FALSE;
//[-end-120301-IB03090376-add]//
  }

  SetupRuntimeDetermination ( SetupNvData );

  //
  // Save Custom variable.
  //
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  Status = gRT->SetVariable (
                  L"Custom",
                  &mFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *)SetupNvData
                  );

//[-start-120229-IB03090376-remove]//
//  mSetupVariableInvalid = FALSE;
//[-end-120229-IB03090376-remove]//

  gBS->FreePool (SetupNvData);

  return EFI_SUCCESS;
}

/**
 Installs a string and ifr pack set

 @param None

 @retval VOID

**/
EFI_STATUS
InstallHiiData (
  VOID
  )
{
  EFI_HII_HANDLE                 HiiHandle;

  EFI_HANDLE                     DriverHandle;
  UINTN                          Index;
  EFI_STATUS                     Status;
  UINTN                          HandleCnt;
  NEW_PACKAGE_INFO               NewPackageInfo [] =
                                 {{InstallExitCallbackRoutine,     ExitVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallBootCallbackRoutine,     BootVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallPowerCallbackRoutine,    PowerVfrBin,    SetupUtilityStrings,    SetupUtilityImages},
                                  {InstallSecurityCallbackRoutine, SecurityVfrBin, SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallAdvanceCallbackRoutine,  AdvanceVfrBin,  SetupUtilityStrings,    SetupUtilityImages},
                                  {InstallMainCallbackRoutine,     MainVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages}
                                 };


  HandleCnt = sizeof (NewPackageInfo) / sizeof (NEW_PACKAGE_INFO);
  if (HandleCnt > MAX_HII_HANDLES) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < HandleCnt; Index++) {
    Status = CreateHiiDriverHandle (&DriverHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    HiiHandle = HiiAddPackages (
                  &mFormSetGuid,
                  DriverHandle,
                  NewPackageInfo[Index].IfrPack,
                  NewPackageInfo[Index].StringPack,
                  NewPackageInfo[Index].ImagePack,
                  NULL
                  );
    ASSERT(HiiHandle != NULL);

    gSUBrowser->SUCInfo->MapTable[Index].HiiHandle = HiiHandle;
    gSUBrowser->SUCInfo->MapTable[Index].DriverHandle = DriverHandle;
    NewPackageInfo[Index].CallbackRoutine (DriverHandle, HiiHandle);

  }
  mDriverHiiHandle = gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;

  DisplayPlatformInfo (gSUBrowser);
  if (FeaturePcdGet(PcdRapidStartSupported)) {
     CheckRapidStartStore(gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle);
  }
  return Status;

}
EFI_STATUS
InitSetupUtilityBrowser(
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
)
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  UINT8                                     *Setup = NULL;

  gSUBrowser = AllocateZeroPool (sizeof(SETUP_UTILITY_BROWSER_DATA));
  if (gSUBrowser == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  gSUBrowser->SUCInfo = AllocateZeroPool (sizeof(SETUP_UTILITY_CONFIGURATION));
  if (gSUBrowser->SUCInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Setup = GetVariableAndSize (
            L"Setup",
            &mFormSetGuid,
            &BufferSize
            );
  SyncSetupDataWithSensitiveVariable ((CHIPSET_CONFIGURATION *) Setup);
  CopyMem (This->SetupNvData, Setup, BufferSize);
  gSUBrowser->SCBuffer          = This->SetupNvData;
  gSUBrowser->SUCInfo->SCBuffer = gSUBrowser->SCBuffer;
  gSUBrowser->Signature         = EFI_SETUP_UTILITY_BROWSER_SIGNATURE;
  gSUBrowser->ExtractConfig     = GenericExtractConfig;
  gSUBrowser->RouteConfig       = GenericRouteConfig;
  gSUBrowser->HotKeyCallback    = HotKeyCallBack;
  if (Setup != NULL) {
    gBS->FreePool (Setup);
  }
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiStringProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiString
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiConfigRouting
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // There will be only one FormConfig in the system
  // If there is another out there, someone is trying to install us
  // again.  Fail that scenario.
  //
  Status = gBS->LocateProtocol (
                  &gEfiFormBrowser2ProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->Browser2
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->LocateProtocol (
                 &gH2ODialogProtocolGuid,
                 NULL,
                 (VOID **) &gSUBrowser->H2ODialog
                 );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
};

/**
 Installs a string and ifr pack set

 @param [in]   StringPack       string pack to store in database and associate with IfrPack
                                IrfPack - ifr pack to store in database (it will use StringPack data)
 @param        ...

 @retval VOID

**/
EFI_STATUS
RemoveHiiData (
  IN VOID     *StringPack,
  ...
)
{
  VA_LIST                                   args;
  HII_HANDLE_VARIABLE_MAP_TABLE             *MapTable;
  EFI_HII_DATABASE_PROTOCOL                 *HiiDatabase;
  UINTN                                     Index;

  HiiDatabase = gSUBrowser->HiiDatabase;
  VA_START (args, StringPack );

  Index = 0;
  while (TRUE) {
    MapTable = VA_ARG( args, HII_HANDLE_VARIABLE_MAP_TABLE *);
    if (MapTable == NULL) {
      break;
    }
    HiiDatabase->RemovePackageList (HiiDatabase, MapTable->HiiHandle);
    mUninstallCallbackRoutine[Index++] (MapTable->DriverHandle);
    DestroyHiiDriverHandle (MapTable->DriverHandle);
  }

  if (gSUBrowser->IdeConfig != NULL) {
    gBS->FreePool (gSUBrowser->IdeConfig);
    gSUBrowser->IdeConfig = NULL;
  }

  return EFI_SUCCESS;

}
/**
 get RapidStart Persistent data variable to check RapidStart store

 @param [in]   AdvanceHiiHandle

 @retval EFI_SUCCESS

**/
EFI_STATUS
CheckRapidStartStore (
  IN  EFI_HII_HANDLE               AdvanceHiiHandle
  )
{
  UINTN                         Tohm;
  UINTN                         Tolm;
  UINTN                         TotalMem;
  UINTN                         MchBase;
  UINT32                        StoreSector;
  RAPID_START_PLATFORM_POLICY_PROTOCOL *RapidStartPolicy   = NULL;
  UINT64                        StoreLbaAddr  = 0;
  UINT8                         StoreSataPort = 0;
  UINT32                        TotalMemToSec = 0;
  EFI_STATUS                    Status;
  STRING_REF                    TokenToUpdate;
  CHAR16                        *StringDataBuffer;
  BOOLEAN                       RapidStartStoreValid  = FALSE;
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  CHIPSET_CONFIGURATION          *SetupVariable;


  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &SetupUtility);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

//[-start-120404-IB05300309-add]//
  MchBase = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);
  Tolm    = MmioRead32 (MchBase + R_SA_BDSM) & B_SA_BDSM_BDSM_MASK;
  Tohm    = MmioRead64 (MchBase + R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
//[-end-120404-IB05300309-add]//
  if ( Tohm > MEM_EQU_4GB ){
     TotalMem = Tolm + (Tohm-MEM_EQU_4GB);
  }
  else{
     TotalMem = Tolm;
  }
  //
  // Locate RapidStart platform policy.
  //
  Status = gBS->LocateProtocol (
                  &gRapidStartPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID **)&RapidStartPolicy
                  );
  if(EFI_ERROR(Status)){
    return Status;
  }
  StoreSector = 0;
  Status = SearchRapidStartStore(RapidStartPolicy,&StoreSector,&StoreLbaAddr,&StoreSataPort);
  TotalMemToSec = (UINT32)(TotalMem >> 9);
  //
  // 1 sector = 512 byte
  // 1 MB = 1024K = 1024 * 2 * sector
  //
  if ((Status == EFI_SUCCESS) && (((SetupVariable->ActivePageThresholdEnable == 1) &&
      (StoreSector >= (UINT32) (SetupVariable->ActivePageThresholdSize * 1024 * 2))) ||
      ((SetupVariable->ActivePageThresholdEnable == 0) && (StoreSector >= TotalMemToSec)))) {
    RapidStartStoreValid = TRUE;
  }

  if (RapidStartStoreValid) {
    StringDataBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_BLANK_STRING), NULL);
    TokenToUpdate = STRING_TOKEN (STR_RAPID_START_STORE_ERROR_STRING);
    HiiSetString (AdvanceHiiHandle, TokenToUpdate, StringDataBuffer, NULL);
    gBS->FreePool (StringDataBuffer);
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
SetupUtilityNotifyFn (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   *SetupUtilityApp;


  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityApplicationProtocolGuid,
                  NULL,
                  (VOID **) &SetupUtilityApp
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

//[-start-140520-IB10300110-add]//
  WatchDogStop ();
//[-end-140520-IB10300110-add]//

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    if (SetupUtilityApp->VfrDriverState == InitializeSetupUtility) {
      CreateScuData ();
      CallSetupUtilityBrowser ();
      DestroyScuData ();
    }
    return;
  }

  switch (SetupUtilityApp->VfrDriverState) {

  case InitializeSetupUtility:
    CreateScuData ();
    break;

  case ShutdownSetupUtility:
    DestroyScuData ();
    break;

  default:
    break;
  }
}


