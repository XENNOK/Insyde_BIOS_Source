//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   UefiSetupUtilityLib.h
//;
//; Abstract:
//;
//;   Header file for UEFI Setup Utility Library
//;

#ifndef _UEFI_SETUP_UTILITY_LIB_H_
#define _UEFI_SETUP_UTILITY_LIB_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "UefiIfrLibrary.h"
#include "SetupConfig.h"

#include EFI_PROTOCOL_CONSUMER (HiiDatabase)
#include EFI_PROTOCOL_CONSUMER (HiiString)
#include EFI_PROTOCOL_CONSUMER (HiiConfigRouting)
#include EFI_PROTOCOL_CONSUMER (FormBrowser2)
#include EFI_PROTOCOL_CONSUMER (OemFormBrowser2)
#include EFI_PROTOCOL_PRODUCER (HiiConfigAccess)
#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)
#include EFI_PROTOCOL_DEFINITION (SetupUtilityBrowser)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (BootOptionPolicy)
#include EFI_PROTOCOL_DEFINITION (HddPasswordService)
#include EFI_PROTOCOL_DEFINITION (SysPasswordService)
#include EFI_GUID_DEFINITION (HddPassWordVariable)
#include EFI_GUID_DEFINITION (GenericVariable)

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

#ifdef PLATFOMR_BOOT_TYPE_ORDER_SEQUENCE
#define BOOT_TYPE_ORDER_SEQUENCE     PLATFOMR_BOOT_TYPE_ORDER_SEQUENCE
#else
#define BOOT_TYPE_ORDER_SEQUENCE     \
             BBS_FLOPPY,\
             BBS_HARDDISK,\
             BBS_CDROM,\
             BBS_USB,\
             OTHER_DRIVER
#endif

//
// The proto type for hot key callback function
//
typedef
EFI_STATUS
(EFIAPI *HOT_KEY_CALLBACK) (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_HII_HANDLE                        HiiHandle,
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
  AdvancedHiiHandle,
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
  SYSTEM_CONFIGURATION                  *SCBuffer;
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
  UINT16                                AdvBootDeviceNum;
  UINT16                                LegacyBootDeviceNum;
  UINT16                                EfiBootDeviceNum;
  UINT16                                *LegacyBootDevType;
  UINT16                                LegacyBootDevTypeCount;
  STRING_REF                            LastToken;
  STRING_REF                            BootTypeTokenRecord;
  STRING_REF                            LegacyAdvanceTokenRecord;
  STRING_REF                            EfiTokenRecord;
  STRING_REF                            AdvanceTokenRecord;
  STRING_REF                            LegacyNormalTokenRecord[MAX_BOOT_ORDER_NUMBER];
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
  EFI_OEM_FORM_BROWSER2_PROTOCOL        *OemBrowser2;
  EFI_HII_ACCESS_EXTRACT_CONFIG         ExtractConfig;
  EFI_HII_ACCESS_ROUTE_CONFIG           RouteConfig;
  HOT_KEY_CALLBACK                      HotKeyCallback;
  IDE_CONFIG                            *IdeConfig;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SYSTEM_CONFIGURATION                  *SCBuffer;

} SETUP_UTILITY_BROWSER_DATA;

#define EFI_SETUP_UTILITY_BROWSER_SIGNATURE EFI_SIGNATURE_32('S','e','B','r')
#define EFI_SETUP_UTILITY_BROWSER_FROM_THIS(a) CR (a, SETUP_UTILITY_BROWSER_DATA, Interface, EFI_SETUP_UTILITY_BROWSER_SIGNATURE)

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            CallbackHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL        DriverCallback;
  EFI_HII_HANDLE                        HiiHandle;
} EFI_CALLBACK_INFO;

//
// Publica function from main menu
//
EFI_STATUS
InstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
UpdateStringToken (
  SYSTEM_CONFIGURATION                  *IfrNVData
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
  IN OUT UINTN           *BufferSize,
  IN     UINT8           *Buffer,
  IN     BOOLEAN         RetrieveData
  );

//
// Publica function from security menu
//
EFI_STATUS
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallSecurityCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
SetSecurityStatus (
  VOID
  );

EFI_STATUS
UpdateHddPasswordLabel (
  IN EFI_HII_HANDLE HiiHandle,
  IN HDD_PASSWORD_INFO_DATA * HddPasswordDataInfo,
  IN UINTN NumOfHddPswd
  );

EFI_STATUS
UpdateHdPswLabel (
  IN UINT16              CallBackFormId,
  IN BOOLEAN             HaveSecurityData,
  IN UINTN               HarddiskInfoIndex
  );

EFI_STATUS
HddPasswordCheck (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           BootOrSCU,
  IN  SYSTEM_CONFIGURATION              *SetupVariable,
  OUT SEC_HDD_PASSWORD_STRUCT           **SavePasswordPoint
  );

EFI_STATUS
PasswordCheck (
  IN  SETUP_UTILITY_CONFIGURATION       *CallbackInfo,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  );

EFI_STATUS
UpdatePasswordState (
  EFI_HII_HANDLE                        HiiHandle
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

//
// Publica function from Boot menu
//
EFI_STATUS
InstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

//
// Publica function from Exit menu
//

EFI_STATUS
InstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

BOOLEAN
SetupUtilityLibIsLangCodeSupport (
  IN UINT8                        *LangCode
  );

EFI_STATUS
GetLangIndex (
  IN  CHAR8         *LangStr,
  OUT UINT8         *LangIndex
  );

EFI_STATUS
InitHddPasswordInfoForSCU (
  VOID
  );

extern PASSWORD_CONFIGURATION mPasswordConfig;

#endif


