//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  GenericBdsLib.h

Abstract:

  BDS library definition, include the file and data structure

--*/

#ifndef _GENERIC_BDS_LIB_H_
#define _GENERIC_BDS_LIB_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "EfiPrintLib.h"
#include "BmMachine.h"
#include "EfiHobLib.h"
#include "Pci.h"
#include "Chipset.h"
#include "AdmiSecureBoot.h"
#include "PostCode.h"

#include EFI_PROTOCOL_DEFINITION (SerialIo)
#include EFI_PROTOCOL_DEFINITION (BlockIo)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (AcpiS3Save)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_PROTOCOL_DEFINITION (SimpleFileSystem)
#include EFI_PROTOCOL_DEFINITION (SimpleNetwork)
#include EFI_PROTOCOL_DEFINITION (LoadFile)
#include EFI_PROTOCOL_DEFINITION (PlatformDriverOverride)
#include EFI_PROTOCOL_DEFINITION (ConsoleControl)
#include EFI_PROTOCOL_DEFINITION (GraphicsOutput)
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#include "UefiIfrLibrary.h"
#include EFI_PROTOCOL_DEFINITION (HiiDatabase)
#include EFI_PROTOCOL_DEFINITION (HiiString)
#else
#include EFI_PROTOCOL_DEFINITION (Hii)
#include EFI_PROTOCOL_DEFINITION (FormBrowser)
#endif
#include EFI_PROTOCOL_DEFINITION (FirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (BootoptionPolicy)
#include EFI_PROTOCOL_CONSUMER (OemServices)
#include EFI_PROTOCOL_CONSUMER (PciRootBridgeIo)
#include EFI_PROTOCOL_CONSUMER (PciIo)
#include EFI_PROTOCOL_CONSUMER (EfiSetupUtility)
#include EFI_PROTOCOL_CONSUMER (EfiOEMBadgingSupport)
#include EFI_PROTOCOL_PRODUCER (StartOfBdsDiagnostics)
#include EFI_PROTOCOL_PRODUCER (EndOfBdsBootSelection)
#include EFI_PROTOCOL_PRODUCER (EndOfdisableQuietBoot)
#include EFI_PROTOCOL_DEFINITION (Ebc)
#include EFI_GUID_DEFINITION (PcAnsi)
#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (HotPlugDevice)
#include EFI_GUID_DEFINITION (GlobalVariable)
#include EFI_GUID_DEFINITION (GenericVariable)
#include EFI_GUID_DEFINITION (EfiShell)
#include EFI_GUID_DEFINITION (ConsoleInDevice)
#include EFI_GUID_DEFINITION (ConsoleOutDevice)
#include EFI_GUID_DEFINITION (StandardErrorDevice)
#include EFI_GUID_DEFINITION (MemoryTypeInformation)
#include EFI_GUID_DEFINITION (BootOrderHook)
#include "AcpiFPDT.h"
#include EFI_PROTOCOL_DEFINITION (AcpiSupport)
#include EFI_GUID_DEFINITION (Acpi)
#include EFI_ARCH_PROTOCOL_DEFINITION (Cpu)
#include EFI_PROTOCOL_DEFINITION (IdeControllerInit)
#include EFI_GUID_DEFINITION (UsbEnumeration)
#include EFI_PROTOCOL_DEFINITION (SysPasswordService)
#include EFI_PROTOCOL_DEFINITION (HddPasswordService)
#include EFI_PROTOCOL_DEFINITION (ChipsetLibServices)

//
// Include the performance head file and defind macro to add perf data
//
#ifdef FIRMWARE_PERFORMANCE
VOID
WriteBootToOsPerformanceData (
  VOID
  );

VOID
ClearDebugRegisters (
  VOID
  );

#define WRITE_BOOT_TO_OS_PERFORMANCE_DATA WriteBootToOsPerformanceData ()
#else
#define WRITE_BOOT_TO_OS_PERFORMANCE_DATA
#endif

extern EFI_HANDLE mBdsImageHandle;


//
// Constants which are variable names used to access variables
//
#define VarLegacyDevOrder L"LegacyDevOrder"

#define PHYSICAL_BOOT_ORDER_NAME L"PhysicalBootOrder"
//
// Data structures and defines
//
#define FRONT_PAGE_QUESTION_ID  0x0000
#define FRONT_PAGE_DATA_WIDTH   0x01

//
// ConnectType
//
#define CONSOLE_OUT 0x00000001
#define STD_ERROR   0x00000002
#define CONSOLE_IN  0x00000004
#define CONSOLE_ALL (CONSOLE_OUT | CONSOLE_IN | STD_ERROR)

//
// Load Option Attributes defined in EFI Specification
//
#define LOAD_OPTION_HIDDEN              0x00000000
#define LOAD_OPTION_ACTIVE              0x00000001
#define LOAD_OPTION_FORCE_RECONNECT     0x00000002
#define IS_LOAD_OPTION_TYPE(_c, _Mask)  (BOOLEAN) (((_c) & (_Mask)) != 0)

//
// Define Maxmim characters that will be accepted
//
#define MAX_CHAR            480
#define MAX_CHAR_SIZE       (MAX_CHAR * 2)

#define MIN_ALIGNMENT_SIZE  4
#define ALIGN_SIZE(a)       ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)

//
// Define maximum characters for boot option variable "BootXXXX"
//
#define BOOT_OPTION_MAX_CHAR 10
#define SHELL_OPTIONAL_DATA_SIZE 6
#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI            0x0000000000000001

//
// This data structure is the part of BDS_CONNECT_ENTRY that we can hard code.
//
#define BDS_LOAD_OPTION_SIGNATURE EFI_SIGNATURE_32 ('B', 'd', 'C', 'O')

typedef struct {

  UINTN                     Signature;
  EFI_LIST_ENTRY            Link;

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  CHAR16                    *OptionName;
  UINTN                     OptionNumber;
  UINT16                    BootCurrent;
  UINT32                    Attribute;
  CHAR16                    *Description;
  VOID                      *LoadOptions;
  UINT32                    LoadOptionsSize;

} BDS_COMMON_OPTION;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     ConnectType;
} BDS_CONSOLE_CONNECT_ENTRY;

typedef struct {
  CHAR16            *FilePathString;
  CHAR16            *Description;
}BDS_GENERAL_UEFI_BOOT_OS;


#define MAX_OPTION_ROM_STORAGE_DEVICE  16

typedef struct {
  UINTN Bus;
  UINTN Device;
  UINTN Function;
} OPROM_STORAGE_DEVICE_INFO;

//
// Lib Functions
//

//
// Bds boot relate lib functions
//
EFI_STATUS
BdsLibUpdateBootOrderList (
  IN  EFI_LIST_ENTRY                 *BdsOptionList,
  IN  CHAR16                         *VariableName
  );

EFI_STATUS
BdsLibBootNext (
  VOID
  );

EFI_STATUS
BdsLibBootViaBootOption (
  IN  BDS_COMMON_OPTION             * Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      * DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  );

BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  );

BOOLEAN
BdsLibMatchFilePathDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *FirstDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL   *SecondDevicePath
  );

EFI_STATUS
BdsLibEnumerateAllBootOption (
  IN     BOOLEAN           FullEnumerate,
  IN OUT EFI_LIST_ENTRY    *BdsBootOptionList
  );

VOID
BdsLibBuildOptionFromHandle (
  IN  EFI_HANDLE          Handle,
  IN  EFI_LIST_ENTRY             *BdsBootOptionList,
  IN  CHAR16                     *String
  );

VOID
BdsLibBuildOptionFromShell (
  IN  EFI_HANDLE                     Handle,
  IN  EFI_LIST_ENTRY                 *BdsBootOptionList,
  IN  CHAR16                         *Description
  );

//
// Bds misc lib functions
//
UINT16
BdsLibGetTimeout (
  VOID
  );

EFI_STATUS
BdsLibGetBootMode (
  OUT EFI_BOOT_MODE       *BootMode
  );

VOID
BdsLibLoadDrivers (
  IN  EFI_LIST_ENTRY              *BdsDriverLists
  );

EFI_STATUS
BdsLibBuildOptionFromVar (
  IN  EFI_LIST_ENTRY              *BdsCommonOptionList,
  IN  CHAR16                      *VariableName
  );

VOID                      *
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

EFI_STATUS
BdsLibOutputStrings (
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL   *ConOut,
  ...
  );

BDS_COMMON_OPTION         *
BdsLibVariableToOption (
  IN OUT EFI_LIST_ENTRY               *BdsCommonOptionList,
  IN CHAR16                           *VariableName
  );

EFI_STATUS
BdsLibRegisterNewOption (
  IN  EFI_LIST_ENTRY                 *BdsOptionList,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  );

EFI_STATUS
BdsLibGetDriverImageHandle (
  IN  EFI_GUID          *DriverFileGuid,
  OUT EFI_HANDLE        *DriverImageHandle
  );

EFI_STATUS
BdsLibUpdateOptionVar (
  IN  CHAR16                         *OptionName,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  );

//
// Bds connect or disconnect driver lib funcion
//
VOID
BdsLibConnectAllDriversToAllControllers (
  VOID
  );

VOID
BdsLibConnectAll (
  VOID
  );

EFI_STATUS
BdsLibConnectDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  );

EFI_STATUS
BdsLibConnectAllEfi (
  VOID
  );

EFI_STATUS
BdsLibDisconnectAllEfi (
  VOID
  );

VOID
BdsLibConnectUsbHID (
  VOID
  );

EFI_STATUS
EFIAPI
BdsLibConnectUsbHIDNotifyRegister (
  );

VOID
BdsLibConnectLegacyRoms (
  VOID
  );


//
// Bds console relate lib functions
//
VOID
BdsLibConnectAllConsoles (
  VOID
  );

EFI_STATUS
BdsLibConnectAllDefaultConsoles (
  VOID
  );

EFI_STATUS
BdsLibUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  );

EFI_STATUS
BdsLibConnectConsoleVariable (
  IN  CHAR16                 *ConVarName
  );

//
// Bds device path relate lib functions
//
EFI_DEVICE_PATH_PROTOCOL  *
BdsLibUnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  );

EFI_DEVICE_PATH_PROTOCOL *
BdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  );

BOOLEAN
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  );

CHAR16                    *
DevicePathToStr (
  EFI_DEVICE_PATH_PROTOCOL     *DevPath
  );

VOID                      *
EfiLibGetVariable (
  IN CHAR16               *Name,
  IN EFI_GUID             *VendorGuid
  );

//
// Internal definitions
//
typedef struct {
  CHAR16  *str;
  UINTN   len;
  UINTN   maxlen;
} POOL_PRINT;

typedef struct {
  UINT8 Type;
  UINT8 SubType;
  VOID (*Function) (POOL_PRINT *, VOID *);
} DEVICE_PATH_STRING_TABLE;

//
// Internal functions
//
EFI_STATUS
BdsBootByDiskSignatureAndPartition (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath,
  IN  UINT32                     LoadOptionsSize,
  IN  VOID                       *LoadOptions,
  OUT UINTN                      *ExitDataSize,
  OUT CHAR16                     **ExitData OPTIONAL
  );

#ifdef UEFI_NETWORK_SUPPORT
EFI_STATUS
BuildNetworkBootOption (
  IN     EFI_LIST_ENTRY  *BdsBootOptionList
  );
#endif

//
// Notes: EFI 64 shadow all option rom
//
#ifdef EFI64
#define EFI64_SHADOW_ALL_LEGACY_ROM() ShadowAllOptionRom ();
VOID
ShadowAllOptionRom();
#else
#define EFI64_SHADOW_ALL_LEGACY_ROM()
#endif

//
// BBS support macros and functions
//

#if defined(EFI32) || defined(EFIX64)
#define REFRESH_LEGACY_BOOT_OPTIONS \
        BdsDeleteAllInvalidLegacyBootOptions ();\
        BdsAddNonExistingLegacyBootOptions (); \
        BdsUpdateLegacyDevOrder ()
#else
#define REFRESH_LEGACY_BOOT_OPTIONS
#endif

EFI_STATUS
BdsDeleteAllInvalidLegacyBootOptions (
  VOID
  );

EFI_STATUS
BdsAddNonExistingLegacyBootOptions (
  VOID
  );

EFI_STATUS
BdsUpdateLegacyDevOrder (
  VOID
  );

EFI_STATUS
BdsRefreshBbsTableForBoot (
  IN BDS_COMMON_OPTION        *Entry
  );

EFI_STATUS
BdsDeleteBootOption (
  IN UINTN                       OptionNumber,
  IN OUT UINT16                  *BootOrder,
  IN OUT UINTN                   *BootOrderSize
  );

//
//The interface functions relate with Setup Browser Reset Reminder feature
//
VOID
EnableResetReminderFeature (
  VOID
  );

VOID
DisableResetReminderFeature (
  VOID
  );

VOID
EnableResetRequired (
  VOID
  );

VOID
DisableResetRequired (
  VOID
  );

BOOLEAN
IsResetReminderFeatureEnable (
  VOID
  );

BOOLEAN
IsResetRequired (
  VOID
  );

VOID
SetupResetReminder (
  VOID
  );

EFI_STATUS
BdsLibNewBootOptionPolicy(
  UINT16                                     **NewBootOrder,
  UINT16                                     *OldBootOrder,
  UINTN                                      OldBootOrderCount,
  UINT16                                     NewBootOptionNo,
  UINTN                                      Policy
);

EFI_STATUS
GetNewBootOptionNo (
  IN     UINT16                                     *BootOrder,
  IN     UINTN                                      BootOptionNum,
  IN OUT UINT16                                     *CurrentBootOptionNo
);


EFI_STATUS
SetBbsPriority (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios,
  IN  BDS_COMMON_OPTION                 *Option
  );

EFI_STATUS
BdsLibUpdateInvalidBootOrder (
  IN OUT UINT16        **BootOrderPtr,
  IN     UINTN         Index,
  IN OUT UINTN         *BootOrderSize
  );

EFI_STATUS
GetUnnecessaryEfiBootOption (
   IN     UINT16       *BootOrder,
   IN     UINT16       BootOrderCount,
   IN     UINTN        BootOrderIndex,
   IN OUT UINTN        *EfiOsCreatedIndex,
   IN OUT UINTN        *BiosCreatedIndex
  );

EFI_STATUS
BdsLibStartSetupUtility (
  BOOLEAN       PasswordCheck
  );

EFI_STATUS
BdsLibDisplayDeviceReplace (
  OUT    BOOLEAN          *SkipOriginalCode
  );

EFI_STATUS
BdsLibOnStartOfBdsDiagnostics (
  VOID
  );

EFI_STATUS
BdsLibOnEndOfBdsBootSelection (
  VOID
  );

EFI_STATUS
BdsLibShowOemStringInTextMode (
  IN BOOLEAN                       AfterSelect,
  IN UINT8                         SelectedStringNum
  );
//
// Recovery Flash Funciton definition
//
UINT8
FlashWrite (
  IN UINT8                *Buffer,
  IN UINT32               FlashSize,
  IN UINT32               FlashAddress,
  IN UINT16               SmiPort
  );

UINT8
FlashPartInfo (
  IN     UINT8            *PartInfo,
  IN     UINT8            *PartBlock,
  IN     UINT8            FlashTypeSelect,
  IN     UINT16           SmiPort
  );

UINT8
FlashComplete (
  IN     UINT16               Command,
  IN     UINT16               SmiPort
  );

typedef
BOOLEAN
(EFIAPI *HOT_KEY_FUNCTION) (
  IN UINT16  FunctionKey
  );

typedef struct {
  HOT_KEY_FUNCTION   PlatformGetKeyFunction;
  BOOLEAN            EnableQuietBootPolicy;
  BOOLEAN            CanShowString;
  UINTN              DisableQueitBootHotKeyCnt;
  EFI_INPUT_KEY      HotKeyList[1];
} HOT_KEY_CONTEXT;


EFI_STATUS
BdsLibInstallHotKeys (
  IN  HOT_KEY_CONTEXT    *NotityContext
  );

EFI_STATUS
BdsLibStopHotKeyEvent (
  VOID
  );

EFI_STATUS
BdsLibGetHotKey (
  OUT UINT16    *FunctionKey,
  OUT BOOLEAN   *HotKeyPressed
  );


EFI_STATUS
BdsLibGetQuietBootState (
  OUT BOOLEAN        *QuietBootState
  );

EFI_STATUS
BdsLibRemovedBootOption (
  IN  BOOLEAN                           RemovedLegacy
  );

EFI_STATUS
BdsLibSkipEbcDispatch (
  VOID
  );

BOOLEAN
BdsLibIsLegacyBootOption (
  IN UINT8                 *BootOptionVar,
  OUT BBS_TABLE            **BbsEntry,
  OUT UINT16               *BbsIndex
  );

UINT32
BdsLibGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

EFI_STATUS
BdsLibOpenFileFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  UINT64                     OpenMode,
  IN  UINT64                     Attributes,
  OUT EFI_FILE_HANDLE            *NewHandle
  );

UINTN
BdsLibGetBootType (
  VOID
  );

EFI_STATUS
BdsLibSetHotKeyDelayTime (
  VOID
  );

EFI_STATUS
BdsLibGetOpromStorageDevInfo (
  OUT OPROM_STORAGE_DEVICE_INFO **OpromStorageDev,
  OUT UINTN                     *OpromStorageDevCount
  );

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        *BlockIoDevicePath;
  CHAR16                          *HwDeviceName;
} HARDWARE_BOOT_DEVICE_INFO;

EFI_STATUS
BdsLibGetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      *HwBootDeviceInfo
  );

VOID
BdsLibAsciiToUnicodeSize (
  IN UINT8              *a,
  IN UINTN              Size,
  OUT UINT16            *u
  );

EFI_STATUS
BdsLibUpdateAtaString (
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  );

BOOLEAN
BdsLibCompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  );

EFI_STATUS
BdsLibDeleteInvalidBootOptions (
  VOID
  )
/*++

Routine Description:

  Delete all of invalid boot options which option number is saved
  in L"BootOrder" variable.

Arguments:

  None

Returns:

  EFI_SUCCESS  - Delete invalid boot options successful.
  Other        - Delete invalid boot options failed.

--*/
;

BOOLEAN
IsAdministerSecureBootSupport (
  VOID
  )
/*++

Routine Description:

  Check is whether support administer secure boot or not.

Arguments:

  None

Returns:

  TRUE    - Administer Secure boot is enabled.
  FALSE   - Administer Secure boot is disabled.

--*/
;

BOOLEAN
BdsLibIsDummyBootOption (
  UINT16       OptionNum
  )
/*++

Routine Description:

  According to option number to check this boot option is whether
  dummy boot option.

Arguments:

  OptionNum    - UINT16 to save boot option number.

Returns:

  TRUE         - This boot option is dummy boot option.
  FALSE        - This boot option isn't dummy boot option.

--*/
;

EFI_STATUS
BdsLibCreateLoadOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT VOID                           **LoadOption,
  OUT UINTN                          *LoadOptionSize
  )
/*++

Routine Description:

  This function allocates memory to generate load option. It is caller's responsibility
  to free load option if caller no longer requires the content of load option.

Arguments:

  DevicePath               - Pointer to a packed array of UEFI device paths.
  Description              - The user readable description for the load option.
  OptionalData             - Pointer to optional data for load option.
  OptionalDataSize         - The size of optional data.
  LoadOption               - Double pointer to load option.
  LoadOptionSize           - The load option size by byte.

Returns:

  EFI_INVALID_PARAMETER    - Any input parameter is invalid.
  EFI_OUT_OF_RESOURCES     - Allocate memory failed.
  EFI_SUCCESS              - Generate load option successful.

--*/
;

EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to
  "BootOrder"  variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Restore the contents of "BootOrder" variable successful.
  Other          - Any error occurred while restoring data to "BootOrder" variable.

--*/
;

EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
  the contents of "BootOrder" variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
  Other          - Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.

--*/
;

EFI_STATUS
BdsLibGetBootOrderType (
  IN  UINT16        *BootOrder,
  IN  UINTN         BootOrderNum,
  OUT BOOLEAN       *PhysicalBootOrder
  )
/*++

Routine Description:

  According to input boot order to get the boot order type.

Arguments:

  BootOrder              - Pointer to BootOrder array.
  BootOrderNum           - The boot order number in boot order array.
  PhysicalBootOrder      - BOOLEAN value to indicate is whether physical boot order.

Returns:

  EFI_INVALID_PARAMETER  - BootOrder is NULL, BootOrderNum is 0 or PhysicalBootOrder is NULL.
  EFI_SUCCESS            - Determine the boot order is physical boot order or virtual boot order successful.

--*/
;

CHAR16 *
BdsLibGetDescriptionFromBootOption (
  IN UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to get the description from this boot option.
  It is caller's responsibility to free the description if caller no longer requires
  the content of description.

Arguments:

  OptionNum       - The boot option number.


Returns:

  NULL            - Get description from boot option failed.
  Other           - Get description from boot option successful.

--*/
;

EFI_DEVICE_PATH_PROTOCOL *
BdsLibGetDevicePathFromBootOption (
  IN UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to get the device path from this boot option.
  It is caller's responsibility to free the device path if caller no longer requires
  the content of device path.

Arguments:

  OptionNum       - The boot option number.


Returns:

  NULL            - Get device path from boot option failed.
  Other           - Get device path from boot option successful.

--*/
;

EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to remove all of physical boot options and add virtual boot
  options to "BootOrder' variable

Arguments:

  None

Returns:

  EFI_SUCCESS     - Change the contents of "BootOrder" successful
  EFI_NOT_FOUND   - "BootOrder" variable doesn't exist.
  Other           - Set "BootOrder" variable failed.

--*/
;

EFI_STATUS
BdsLibGetMappingBootOptions (
  IN    UINT16         OptionNum,
  OUT   UINTN          *OptionCount,
  OUT   UINT16         **OptionOrder
  )
;

BOOLEAN
BdsLibIsWin8FastBootActive (
  );

EFI_STATUS
BdsLibConnectTargetDev (
  VOID
  );

EFI_STATUS
UpdateTargetHddVariable (
  VOID
  );

CHAR16 *
BdsLibGetStringById (
  IN  EFI_STRING_ID   Id
  );

typedef enum {
  BOOT_DEVICE_TYPE_FLOPPY,
  BOOT_DEVICE_TYPE_CD_DVD,
  BOOT_DEVICE_TYPE_USB,
  BOOT_DEVICE_TYPE_SCSI,
  BOOT_DEVICE_TYPE_MISC,
  BOOT_DEVICE_TYPE_HDD,
  BOOT_DEVICE_TYPE_NETWORK,
  BOOT_DEVICE_TYPE_SHELL,
  BOOT_DEVICE_TYPE_MAX
} BOOT_DEVICE_TYPE;

EFI_STATUS
BdsLibUpdateBootDevTypeStr (
  IN  BOOT_DEVICE_TYPE         BootDevType,
  IN  EFI_HII_HANDLE           StrPackHandle,
  IN  EFI_STRING_ID            StrToken
  );

//
// defintion for dummy option number
//
typedef enum {
  DummyBootOptionStartNum = 0x2000,
  DummyUsbBootOptionNum,
  DummyCDBootOptionNum,
  DummyNetwokrBootOptionNum,
  DummyBootOptionEndNum,
} DUMMY_BOOT_OPTION_NUM;

typedef struct _DUMMY_BOOT_OPTION_INFO {
  DUMMY_BOOT_OPTION_NUM      BootOptionNum;
  CHAR16                     *Description;
} DUMMY_BOOT_OPTION_INFO;

//
// Define the boot type which to classify the boot option type
// Different boot option type could have different boot behavior
// Use their device path node (Type + SubType) as type value
// The boot type here can be added according to requirement
//
//
// ACPI boot type. For ACPI device, cannot use sub-type to distinguish device, so hardcode their value
//
#define  BDS_EFI_ACPI_FLOPPY_BOOT         0x0201
//
// Message boot type
// If a device path of boot option only point to a message node, the boot option is message boot type
//
#define  BDS_EFI_MESSAGE_ATAPI_BOOT       0x0301 // Type 03; Sub-Type 01
#define  BDS_EFI_MESSAGE_SCSI_BOOT        0x0302 // Type 03; Sub-Type 02
#define  BDS_EFI_MESSAGE_USB_DEVICE_BOOT  0x0305 // Type 03; Sub-Type 05
#define  BDS_EFI_MESSAGE_MAC_BOOT         0x030b // Type 03; Sub-Type 11
#define  BDS_EFI_MESSAGE_SATA_BOOT        0x0312 // Type 03; Sub-Type 18
#define  BDS_EFI_MESSAGE_ISCSI_BOOT       0x0313 // Type 03; Sub-Type 19
#define  BDS_EFI_MESSAGE_MISC_BOOT        0x03FF
//
// Media boot type
// If a device path of boot option contain a media node, the boot option is media boot type
//
#define  BDS_EFI_MEDIA_HD_BOOT            0x0401 // Type 04; Sub-Type 01
#define  BDS_EFI_MEDIA_CDROM_BOOT         0x0402 // Type 04; Sub-Type 02
#define  BDS_EFI_MEDIA_FV_FILEPATH_BOOT   0x0406 // Type 04; Sub-Type 06
//
// BBS boot type
// If a device path of boot option contain a BBS node, the boot option is BBS boot type
//
#define  BDS_LEGACY_BBS_BOOT              0x0501 //  Type 05; Sub-Type 01

#define  BDS_EFI_UNSUPPORT                0xFFFF

//
// for UEFI_NETWORK_SUPPORT
//
#define UEFI_NETWORK_BOOT_OPTION_IPV4      0
#define UEFI_NETWORK_BOOT_OPTION_IPV6      1
#define UEFI_NETWORK_BOOT_OPTION_BOTH      2
#define UEFI_NETWORK_BOOT_OPTION_NONE      3

#define UEFI_NETWORK_BOOT_OPTION_MAX       8

#define UEFI_NETWORK_BOOT_OPTION_MAX_CHAR  60
#define UEFI_NETWORK_MAC_ADDRESS_LENGTH    32

//
// String Length : "VLAN(XXXX)"
//   Max length 10 + 1 null string end
//
#define UEFI_NETWORK_VLAN_STRING_LENGTH    11

EFI_STATUS
SetSysPasswordCheck (
  VOID
  )
/*++

Routine Description:

  After checking system password, the checking flag is set to disable.
  Using this function to set the checking flag enable

Arguments:

  None

Returns:

  EFI_SUCCESS           - set the checking flag successful.

--*/
;

DUMMY_BOOT_OPTION_NUM
BdsLibGetDummyBootOptionNum (
  IN  UINT16                 OptionNum
  )
/*++

Routine Description:

  Based on the boot option number, return the dummy boot option number.
  If return DummyBootOptionEndNum, it means this boot option does not belong to dummy boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  Dummy boot option number or DummyBootOptionEndNum if input boot option does not belong to dummy boot option.

--*/
;

BOOLEAN
BdsLibIsBiosCreatedOption (
  IN UINT8                  *Variable,
  IN UINTN                  VariableSize
  )
/*++

Routine Description:

 Check if option variable is created by BIOS or not.

Arguments:

  Variable             - Pointer to option variable
  VariableSize         - Option variable size

Returns:

  TRUE                 - Option varible is created by BIOS
  FALSE                - Option varible is not created by BIOS

--*/
;

BOOLEAN
BdsLibCheckDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     QueryType,
  IN UINT8                     QuerySubType
  );

EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Enable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Enable BootOrder variable hook mechanism successfully.
  Other       - Enable BootOrder variable hook mechanism failed.

--*/
;

EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Disable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Disable BootOrder variable hook mechanism successfully.
  Other       - Disable BootOrder variable hook mechanism failed.

--*/
;


BOOLEAN
BdsLibIsBootOrderHookEnabled (
  VOID
  )
/*++

Routine Description:

  Function uses to check BootOrder variable hook mechanism is whether enabled.

Arguments:

  None

Returns:

  TRUE   - BootOrder variable hook mechanism is enabled.
  FALSE  - BootOrder variable hook mechanism is disabled.

--*/
;

EFI_STATUS
EFIAPI
BdsLibGetImageHeader (
  IN  EFI_HANDLE                            Device,
  IN  CHAR16                                *FileName,
  OUT EFI_IMAGE_DOS_HEADER                  *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  );

BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  );
#endif
