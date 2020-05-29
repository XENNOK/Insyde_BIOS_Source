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

#ifndef _UEFI_SETUP_UTILITY_H_
#define _UEFI_SETUP_UTILITY_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/VariableLib.h>

//[-start-121109-IB05280008-remove]//
//#include <OemServices/Kernel.h>
//[-end-121109-IB05280008-remove]//
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <ChipsetCmos.h>
#include <Library/CmosLib.h>
#include <Library/HobLib.h>
#include <Guid/SystemPasswordVariable.h>
#include <Guid/XTUInfoHob.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/SetupUtilityApplication.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/DiskInfo.h>
//[-start-130110-IB10820219-add]//
#include <Protocol/CheckRaid.h>
#include <Protocol/PlatformInfo.h>
#include <Protocol/IsaAcpi.h>
//[-end-130110-IB10820219-add]//
#include <Guid/DataHubRecords.h>
#include <Protocol/AlertStandardFormat.h>
#include <Guid/GlobalVariable.h>
#include <ChipsetSetupConfig.h>
#include <SetupFuncs.h>
#include <Advance/Advance.h>
#include <Power/Power.h>
#include <Guid/DebugMask.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <PostCode.h>
#include <Library/OemGraphicsLib.h>
#include <HeciMsgLib.h>
//[-start-130110-IB10820219-add]//
#include <PlatformGopVbt.h>
//[-end-130110-IB10820219-add]//

#include <RapidStartData.h>
//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include <SetupUtilityStrDefs.h>
#define bit(a)   (1 << (a))
#define HDD_SECURITY_SUPPORT bit(0)
#define HDD_SECURITY_ENABLE  bit(1)
#define HDD_SECURITY_LOCK    bit(2)
#define HDD_SECURITY_FROZEN  bit(3)

//
// Global externs
//
extern UINT8 SetupUtilityStrings[];
//[-start-120221-IB07010064-add]//
extern UINT32 SetupUtilityStringsTotalSize;
extern UINT32 SetupUtilityLibStringsTotalSize;
//[-end-120221-IB07010064-add]//
extern UINT8 AdvanceVfrBin[];
extern UINT8 PowerVfrBin[];
extern UINT8 AdvanceVfrSystemConfigDefault0000[];
//[-start-131129-IB09740048-add]//
extern UINT8 AdvanceVfrCrConfigDefault0000[];
//[-end-131129-IB09740048-add]//
extern UINT8 PowerVfrSystemConfigDefault0000[];
extern EFI_GUID  mFormSetGuid;
extern CHAR16    mVariableName[];
extern EFI_HII_HANDLE mDriverHiiHandle;

#define EFI_SETUP_UTILITY_SIGNATURE SIGNATURE_32('S','e','t','u')

#define STRING_PACK_GUID  { 0x8160a85f, 0x934d, 0x468b, 0xa2, 0x35, 0x72, 0x89, 0x59, 0x14, 0xf6, 0xfc }
#define HAVE_CREATE_SYSTEM_HEALTH_EVENT   0xCF8F

extern UINT16   gSaveItemMapping[][2];

#define CUSTOM_BOOT_ORDER_SIZE              256
#define TIMEOUT_OF_EVENT                5000000

#ifndef _MRC_API_H
//
// define the graphics stolen memory
//
typedef enum {
  gms_0MB     = 0x0,  // 0MB
  gms_32MB    = 0x1,  // 32MB
  gms_64MB    = 0x2,  // 64MB
  gms_96MB    = 0x3,  // 96MB
  gms_128MB   = 0x4,  // 128MB
  gms_160MB   = 0x5,  // 160MB
  gms_192MB   = 0x6,  // 192MB
  gms_224MB   = 0x7,  // 224MB
  gms_256MB   = 0x8,  // 256MB
  gms_288MB   = 0x9,  // 288MB
  gms_320MB   = 0xA,  // 320MB
  gms_352MB   = 0xB,  // 352MB
  gms_384MB   = 0xC,  // 384MB
  gms_416MB   = 0xD,  // 416MB
  gms_448MB   = 0xE,  // 448MB
  gms_480MB   = 0xF,  // 480MB
  gms_512MB   = 0x10, // 512MB
} MRC_TGraphicsStolenSize;
#endif

extern UINT16                          gSCUSystemHealth;
extern EFI_EVENT                       gSCUTimerEvent;

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;
  EFI_SETUP_UTILITY_PROTOCOL            SetupUtility;
} SETUP_UTILITY_DATA;

typedef struct _NEW_PACKAGE_INFO {
  USER_INSTALL_CALLBACK_ROUTINE         CallbackRoutine;
  UINT8                                 *IfrPack;
  UINT8                                 *StringPack;
  UINT8                                 *ImagePack;
} NEW_PACKAGE_INFO;

#define EFI_SETUP_UTILITY_FROM_THIS(a) CR (a, SETUP_UTILITY_DATA, SetupUtility, EFI_SETUP_UTILITY_SIGNATURE)

//[-start-111124-IB10930032-add]//
EFI_STATUS
EFIAPI
InstallSetupHii (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This,
  IN  BOOLEAN                               InstallHii
  );
//[-end-111124-IB10930032-add]//


EFI_STATUS
EFIAPI
PowerOnSecurity (
  IN  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility
  );

EFI_STATUS
CheckIde (
  IN EFI_HII_HANDLE                        HiiHandle,
  IN CHIPSET_CONFIGURATION                  *SetupVariable,
  IN BOOLEAN                               UpdateIde
  );

BOOLEAN
AsfResetSetupData (
  IN  VOID
  );

EFI_STATUS
InstallHiiData (
  VOID
);


EFI_STATUS
RemoveHiiData (
  IN VOID     *StringPack,
  ...
);

EFI_STATUS
GetSystemConfigurationVar (
  IN SETUP_UTILITY_DATA                     *SetupData
  );

EFI_STATUS
InstallSetupUtilityBrowserProtocol (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
  );

EFI_STATUS
UninstallSetupUtilityBrowserProtocol (
  VOID
  );

EFI_STATUS
EFIAPI
HotKeyCallBack (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  );

extern SETUP_UTILITY_BROWSER_DATA       *gSUBrowser;
#define RST_CNT                         0xCF9
#define     V_RST_CNT_FULLRESET         0x0E
extern BOOLEAN                          mFullResetFlag;


EFI_STATUS
DisplayPlatformInfo (
  IN  SETUP_UTILITY_BROWSER_DATA            *SUBrowser
  );

EFI_STATUS
EFIAPI
GenericExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
GenericRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );
EFI_STATUS
CheckRapidStartStore (
  IN  EFI_HII_HANDLE               AdvanceHiiHandle
  );

VOID
EFIAPI
SetupUtilityNotifyFn (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  );

#endif
