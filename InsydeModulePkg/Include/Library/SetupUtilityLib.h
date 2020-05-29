/** @file
  Header file for UEFI Setup Utility Library

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

#ifndef _UEFI_SETUP_UTILITY_LIB_H_
#define _UEFI_SETUP_UTILITY_LIB_H_

#include <Uefi.h>
#include <FrameworkDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Guid/HddPasswordVariable.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/KernelConfigLib.h>
#include <KernelSetupConfig.h>

#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/SetupUtilityBrowser.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/BootOptionPolicy.h>
#include <Protocol/HddPasswordService.h>
#include <Protocol/SysPasswordService.h>
#include <Protocol/FrameworkHii.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DiskIo.h>



#define SETUP_UTILITY_MAIN_FORMSET_GUID \
  { \
  0x32e781c7, 0xc751, 0x46f4, {0xaf, 0x94, 0xd0, 0x22, 0x1b, 0xc, 0x6a, 0x1c} \
  }

#define SETUP_UTILITY_SECURITY_FORMSET_GUID \
  { \
  0x8662276c, 0x4b73, 0x440a, {0xae, 0x7b, 0x94, 0xab, 0xd4, 0xa5, 0xff, 0x8b} \
  }

#define SETUP_UTILITY_BOOT_FORMSET_GUID  \
  { \
  0x992e4776, 0x6ed7, 0x4beb, {0xb4, 0x16, 0xff, 0x38, 0x6d, 0xf9, 0x90, 0xc4} \
  }

#define SETUP_UTILITY_EXIT_FORMSET_GUID  \
  { \
  0x736fa659, 0xf368, 0x40ad, {0x8b, 0xac, 0xbc, 0x2d, 0xe, 0x71, 0x26, 0x6e} \
  }

//
// Definitions using in Security page and SetupUtility driver
//
#define SECURITY_LOCK                0x01
#define NO_ACCESS_PASSWORD           0x10
#define CHANGE_PASSWORD              0x20
#define DISABLE_PASSWORD             0x30
#define ENABLE_PASSWORD              0x40
#define MASTER_PASSWORD_GRAYOUT      0x50
#define SUPERVISOR_FLAG              FALSE
#define USERPASSWORD_FLAG            TRUE

#define USER_PASSWORD_NO_ACCESS      1
#define USER_PASSWORD_VIEW_ONLY      2
#define USER_PASSWORD_LIMITED        3
#define USER_PASSWORD_FULL           4

#define USER_PSW                     FALSE
#define MASTER_PSW                   TRUE
#define NO_ACCESS_PSW                0xFF
#define INTO_SCU                     FALSE
#define INTO_BOOT                    TRUE

#define POWER_ON_PASSWORD            0x02
//
// Definitions using in Boot page and SetupUtility driver
//
#define DEFAULT_IDE                  0
#define DEFAULT_USB                  1
#define DEFAULT_BOOT_FLAG            DEFAULT_USB

#define LEGACY_NORMAL_MENU           0
#define LEGACY_ADV_MENU              1
#define EFI_BOOT_MENU                2
#define ADV_BOOT_MENU                3
#define LEGACY_DEVICE_TYPE_MENU      4

#define EFI_FIRST                    EFI_BOOT_DEV
#define LEGACY_FIRST                 LEGACY_BOOT_DEV

#define NORMAL_MENU                  0
#define ADV_MENU                     1

#define DEFAULT_BOOT_MENU_TYPE       NORMAL_MENU
#define DEFAULT_BOOT_NORMAL_PRIOR    EFI_FIRST
#define DEFAULT_LEGACY_NOR_MENU      NORMAL_MENU

#define OTHER_DRIVER                 BOOT_POLICY_OTHER_DEVICE

//
// RFC 3066
//
#define RFC_3066_ENTRY_SIZE             (42 + 1)

extern UINT8 SetupUtilityLibStrings[];
extern UINT8 SetupUtilityLibImages[];
extern UINT8 MainVfrBin[];
extern UINT8 SecurityVfrBin[];
extern UINT8 BootVfrBin[];
extern UINT8 ExitVfrBin[];
extern UINT8 MainVfrSystemConfigDefault0000[];
extern UINT8 SecurityVfrSystemConfigDefault0000[];
extern UINT8 BootVfrSystemConfigDefault0000[];

//
// The proto type for hot key callback function
//
typedef
EFI_STATUS
(EFIAPI *HOT_KEY_CALLBACK) (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  );

typedef enum {
  ExitHiiHandle,
  BootHiiHandle,
  PowerHiiHandle,
  SecurityHiiHandle,
  AdvanceHiiHandle,
  MainHiiHandle
} USER_HII_HANDLE;

typedef
EFI_STATUS
(EFIAPI *USER_INSTALL_CALLBACK_ROUTINE) (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

typedef
EFI_STATUS
(EFIAPI *USER_UNINSTALL_CALLBACK_ROUTINE) (
  IN EFI_HANDLE                             DriverHandle
  );

typedef struct {
  EFI_HANDLE        DriverHandle;
  EFI_HII_HANDLE    HiiHandle;
} HII_HANDLE_VARIABLE_MAP_TABLE;


typedef struct {
  //
  // for PowerOnSecurity
  //
  UINTN                                 Signature;
  EFI_HANDLE                            CallbackHandle;
  EFI_HII_HANDLE                        RegisteredHandle;
  UINT8                                 *SCBuffer;
  BOOLEAN                               DoRefresh;
  HII_HANDLE_VARIABLE_MAP_TABLE         MapTable[MAX_HII_HANDLES];

  //
  // Security Menu
  //
  UINT8                                 HddPassword;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *PswdConsoleService;
  SYS_PASSWORD_INFO_DATA                *SupervisorPassword;
  SYS_PASSWORD_INFO_DATA                *UserPassword;
  HDD_PASSWORD_INFO_DATA                *HarddiskPassword;
  UINTN                                 NumOfHarddiskPswd;
  BOOLEAN                               SupervisorPwdFlag;
  BOOLEAN                               UserPwdFlag;

  //
  // Boot Menu
  //
  UINT16                                *BootOrder;
  UINT16                                *BootPriority;
  UINT16                                AdvBootDeviceNum;
  UINT16                                LegacyBootDeviceNum;
  UINT16                                EfiBootDeviceNum;
  UINT16                                *LegacyBootDevType;
  UINT16                                LegacyBootDevTypeCount;
  STRING_REF                            LastToken;               ///< For compatiblity. It is not used when using H2O form browser
  STRING_REF                            BootTypeTokenRecord;     ///< For compatiblity. It is not used when using H2O form browser
  STRING_REF                            LegacyAdvanceTokenRecord;///< For compatiblity. It is not used when using H2O form browser
  STRING_REF                            EfiTokenRecord;          ///< For compatiblity. It is not used when using H2O form browser
  STRING_REF                            AdvanceTokenRecord;      ///< For compatiblity. It is not used when using H2O form browser
  STRING_REF                            LegacyNormalTokenRecord[MAX_BOOT_ORDER_NUMBER];///< For compatiblity. It is not used when using H2O form browser
  UINT8                                 PrevSataCnfigure;
} SETUP_UTILITY_CONFIGURATION;

typedef struct  {
  UINT8                                 Channel;
  UINT8                                 Device;
  EFI_HANDLE                            DiskInfoHandle;
  UINT8                                 IdeDevice;
  UINT16                                Formlabel;
  UINT16                                SecurityModeToken;
  UINT16                                PswToken1;
  UINT16                                PswToken2;
  CHAR16                                *DevNameString;
} IDE_CONFIG;

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    Interface;
  EFI_HII_DATABASE_PROTOCOL             *HiiDatabase;
  EFI_HII_STRING_PROTOCOL               *HiiString;
  EFI_HII_CONFIG_ROUTING_PROTOCOL       *HiiConfigRouting;
  EFI_FORM_BROWSER2_PROTOCOL            *Browser2;
  H2O_DIALOG_PROTOCOL                   *H2ODialog;
  EFI_HII_ACCESS_EXTRACT_CONFIG         ExtractConfig;
  EFI_HII_ACCESS_ROUTE_CONFIG           RouteConfig;
  HOT_KEY_CALLBACK                      HotKeyCallback;
  IDE_CONFIG                            *IdeConfig;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  UINT8                                 *SCBuffer;
} SETUP_UTILITY_BROWSER_DATA;

#define EFI_SETUP_UTILITY_BROWSER_SIGNATURE SIGNATURE_32('S','e','B','r')
#define EFI_SETUP_UTILITY_BROWSER_FROM_THIS(a) CR(a, SETUP_UTILITY_BROWSER_DATA, Interface, EFI_SETUP_UTILITY_BROWSER_SIGNATURE)

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            CallbackHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL        DriverCallback;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_GUID                              FormsetGuid;
} EFI_CALLBACK_INFO;

#define EFI_CALLBACK_INFO_SIGNATURE SIGNATURE_32('E','C','I','S')
#define EFI_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_CALLBACK_INFO_SIGNATURE)

//
// Publica function from main menu
//
EFI_STATUS
EFIAPI
InstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
EFIAPI
UninstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
UpdateStringToken (
  KERNEL_CONFIGURATION                      *IfrNVData
  );

EFI_STATUS
UpdateLangItem (
  IN const EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN UINT8                                 *Data
  );

EFI_STATUS
SetupVariableConfig (
  IN     EFI_GUID        *VariableGuid, OPTIONAL
  IN     CHAR16          *VariableName, OPTIONAL
  IN OUT UINTN           BufferSize,
  IN     UINT8           *Buffer,
  IN     BOOLEAN         RetrieveData
  );

//
// Publica function from security menu
//
EFI_STATUS
EFIAPI
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                           DriverHandle,
  IN EFI_HII_HANDLE                       HiiHandle
  );

EFI_STATUS
EFIAPI
UninstallSecurityCallbackRoutine (
  IN EFI_HANDLE                           DriverHandle
  );

EFI_STATUS
SetSecurityStatus (
  VOID
  );

EFI_STATUS
UpdateHddPasswordLabel (
  IN EFI_HII_HANDLE                       HiiHandle,
  IN HDD_PASSWORD_INFO_DATA               *HddPasswordDataInfo,
  IN UINTN                                NumOfHddPswd
  );

EFI_STATUS
UpdateHdPswLabel (
  IN UINT16                               CallBackFormId,
  IN BOOLEAN                              HaveSecurityData,
  IN UINTN                                HarddiskInfoIndex
  );

EFI_STATUS
HddPasswordCheck (
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  BOOLEAN                             BootOrSCU,
  IN  KERNEL_CONFIGURATION                *KernelConfig,
  OUT SEC_HDD_PASSWORD_STRUCT             **SavePasswordPoint
  );

EFI_STATUS
PasswordCheck (
  IN  SETUP_UTILITY_CONFIGURATION         *CallbackInfo,
  IN  KERNEL_CONFIGURATION                *KernelConfig
  );

EFI_STATUS
UpdatePasswordState (
  EFI_HII_HANDLE                          HiiHandle
);

EFI_STATUS
HddPasswordCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *FormCallback,
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  UINT8                               Type,
  IN  EFI_IFR_TYPE_VALUE                  *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest,
  OUT BOOLEAN                             *PState,
  IN  UINT16                              CurrentHddIndex,
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL   *PswdConsoleService,
  IN  HDD_PASSWORD_INFO_DATA              *HddPasswordDataInfo,
  IN  BOOLEAN                             MasterOrUser,
  IN  UINTN                               NumOfHddPswd
  );

/**
 Set disable password information to SecurityDataInfo and update string of Security mode.

 @param [in]   FormCallback     The form call back protocol.
 @param [in]   HiiHandle        Return string token of device status.
 @param [in]   SecurityDataInfo  Security Info table of Harddisk.
 @param [in]   NumOfHddPswd     Number of harddisk.

 @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
EFIAPI
DisableAllHarddiskPswd(
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL     *FormCallback,
  IN  EFI_HII_HANDLE                          HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA                  *HddPasswordDataInfo,
  IN  UINTN                                   NumOfHddPswd
  );

//
// Publica function from Boot menu
//
EFI_STATUS
EFIAPI
InstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
EFIAPI
UninstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

//
// Publica function from Exit menu
//
EFI_STATUS
EFIAPI
InstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
EFIAPI
UninstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

VOID
EFIAPI
SetupUtilityLibGetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  );

BOOLEAN
SetupUtilityLibIsLangCodeSupport (
  IN CHAR8                        *LangCode
  );

EFI_STATUS
GetLangIndex (
  IN  CHAR8         *LangStr,
  OUT UINT8         *LangIndex
  );

UINT16
GetVarStoreSize (
  IN EFI_HII_HANDLE            HiiHandle,
  IN EFI_GUID                  *FormsetGuid,
  IN EFI_GUID                  *VarStoreGuid,
  IN CHAR8                     *VarStoreName
  );


EFI_STATUS
BrowserRefreshFormSet (
  VOID
  );

EFI_STATUS
InitHddPasswordInfoForSCU (
  VOID
  );

extern PASSWORD_CONFIGURATION      mPasswordConfig;
#endif


