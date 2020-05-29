/** @file
 Generic BDS library defines general interfaces for a BDS driver, including:
 1) BDS boot policy interface.
 2) BDS boot device connect interface.
 3) BDS Misc interfaces for mainting boot variable, ouput string.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef _GENERIC_BDS_LIB_H_
#define _GENERIC_BDS_LIB_H_

#include <Protocol/UserManager.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/LegacyBios.h>

#include <Library/DevicePathLib.h>

//
// Include the performance head file and defind macro to add perf data
//
#define WRITE_BOOT_TO_OS_PERFORMANCE_DATA PERF_CODE (WriteBootToOsPerformanceData ();)
///
/// Constants which are variable names used to access variables.
///
#define VAR_LEGACY_DEV_ORDER L"LegacyDevOrder"

#define PHYSICAL_BOOT_ORDER_NAME L"PhysicalBootOrder"

///
/// Data structures and defines.
///
#define FRONT_PAGE_QUESTION_ID  0x0000
#define FRONT_PAGE_DATA_WIDTH   0x01

///
/// ConnectType
///
#define CONSOLE_OUT 0x00000001
#define STD_ERROR   0x00000002
#define CONSOLE_IN  0x00000004
#define CONSOLE_ALL (CONSOLE_OUT | CONSOLE_IN | STD_ERROR)

///
/// Load Option Attributes
///
#define LOAD_OPTION_ACTIVE              0x00000001
#define LOAD_OPTION_FORCE_RECONNECT     0x00000002

#define LOAD_OPTION_HIDDEN              0x00000008
#define LOAD_OPTION_CATEGORY            0x00001F00

#define LOAD_OPTION_CATEGORY_BOOT       0x00000000
#define LOAD_OPTION_CATEGORY_APP        0x00000100

#define EFI_BOOT_OPTION_SUPPORT_KEY     0x00000001
#define EFI_BOOT_OPTION_SUPPORT_APP     0x00000002

#define IS_LOAD_OPTION_TYPE(_c, _Mask)  (BOOLEAN) (((_c) & (_Mask)) != 0)

///
/// Define the maximum characters that will be accepted.
///
#define MAX_CHAR            480
#define MAX_CHAR_SIZE       (MAX_CHAR * 2)

#define MIN_ALIGNMENT_SIZE  4
#define ALIGN_SIZE(a)       ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)

///
/// Define maximum characters for boot option variable "BootXXXX".
///
#define BOOT_OPTION_MAX_CHAR 10

#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI            0x0000000000000001

//
// Common language code definition
//
#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"

#define VGA_DRIVER_POLICY_NAME L"Vga Driver Policy"
//
// It is the volatile variable which store the Primary Vga contoller devicepath and will be Updated by Platform code.
// Provide Vga policy Driver to stop the non-Primary Vga controller.
// The Data type will be the EFI_DEVICE_PATH_PROTOCOL *ActiveVgaDev.
//
#define ACTIVE_VGA_VAR_NAME    L"ActiveVgaDev"

//
// RecoveryFlash Mode
//
#define DEFAULT_FLASH_DEVICE_TYPE 0
#define SPI_FLASH_DEVICE_TYPE     1
#define OTHER_FLASH_DEVICE_TYPE   2

#define TPL_DRIVER      6

#pragma pack(1)

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

//
// This data structure is the part of BDS_CONNECT_ENTRY
//
#define BDS_LOAD_OPTION_SIGNATURE SIGNATURE_32 ('B', 'd', 'C', 'O')

typedef struct {
  UINTN                     Signature;
  LIST_ENTRY                Link;

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  CHAR16                    *OptionName;
  UINTN                     OptionNumber;
  UINT16                    BootCurrent;
  UINT32                    Attribute;
  CHAR16                    *Description;
  VOID                      *LoadOptions;
  UINT32                    LoadOptionsSize;
  CHAR16                    *StatusString;
} BDS_COMMON_OPTION;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     ConnectType;
} BDS_CONSOLE_CONNECT_ENTRY;


#define MAX_OPTION_ROM_STORAGE_DEVICE  16
typedef struct {
  UINTN Bus;
  UINTN Device;
  UINTN Function;
} OPROM_STORAGE_DEVICE_INFO;

//
// Bds boot related lib functions
//
/**
 Boot from the UEFI spec defined "BootNext" variable.
**/
EFI_STATUS
EFIAPI
BdsLibBootNext (
  VOID
  );

/**
 Process the boot option according to the UEFI specification.
 The legacy boot option device path includes BBS_DEVICE_PATH.

 @param[in]  Option             The boot option to be processed.
 @param[in]  DevicePath         The device path describing where to load the
                                boot image or the legcy BBS device path to boot the legacy OS.
 @param[out] ExitDataSize       The size of exit data.
 @param[out] ExitData           Data returned when Boot image failed.

 @retval EFI_SUCCESS            Boot from the input boot option succeeded.
 @retval EFI_NOT_FOUND          The Device Path is not found in the system.
**/
EFI_STATUS
EFIAPI
BdsLibBootViaBootOption (
  IN  BDS_COMMON_OPTION             * Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      * DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  );

/**
 This function will enumerate all possible boot devices in the system, and
 automatically create boot options for Network, Shell, Removable BlockIo,
 and Non-BlockIo Simplefile devices.

 BDS separates EFI boot options into six types:
 1. Network - The boot option points to the SimpleNetworkProtocol device.
              Bds will try to automatically create this type of boot option during enumeration.
 2. Shell   - The boot option points to internal flash shell.
              Bds will try to automatically create this type of boot option during enumeration.
 3. Removable BlockIo      - The boot option points to a removable media
                             device, such as a USB flash drive or DVD drive.
                             These devices should contain a *removable* blockIo
                             protocol in their device handle.
                             Bds will try to automatically create this type boot option
                             when enumerate.
 4. Fixed BlockIo          - The boot option points to a Fixed blockIo device,
                             such as a hard disk.
                             These devices should contain a *fixed* blockIo
                             protocol in their device handle.
                             BDS will skip fixed blockIo devices, and not
                             automatically create boot option for them. But BDS
                             will help to delete those fixed blockIo boot options,
                             whose description rules conflict with other auto-created
                             boot options.
 5. Non-BlockIo Simplefile - The boot option points to a device whose handle
                             has SimpleFileSystem Protocol, but has no blockio
                             protocol. These devices do not offer blockIo
                             protocol, but BDS still can get the
                             \EFI\BOOT\boot{machinename}.EFI by SimpleFileSystem
                             Protocol.
 6. File    - The boot option points to a file. These boot options are usually
              created by the user, either manually or with an OS loader. BDS will not delete or modify
              these boot options.

 This function will enumerate all possible boot devices in the system, and
 automatically create boot options for Network, Shell, Removable BlockIo,
 and Non-BlockIo Simplefile devices.
 It will excute once every boot.

 @param[in]      FullEnumerate       Flag to force to do enumeration.
 @param[in, out] BdsBootOptionList   The header of the linked list that indexed all
                                     current boot options.

 @retval EFI_SUCCESS            Finished all the boot device enumerations and
                                created the boot option based on the boot device.
 @retval EFI_OUT_OF_RESOURCES   Failed to enumerate the boot device and create
                                the boot option list.
**/
EFI_STATUS
BdsLibEnumerateAllBootOption (
  IN     BOOLEAN             FullEnumerate,
  IN OUT LIST_ENTRY          *BdsBootOptionList
  );

/**
 Build the boot option with the handle parsed in.

 @param[in] Handle              The handle representing the device path for which
                                to create a boot option.
 @param[in] BdsBootOptionList   The header of the link list that indexed all
                                current boot options.
 @param[in] String              The description of the boot option.
**/
VOID
EFIAPI
BdsLibBuildOptionFromHandle (
  IN  EFI_HANDLE                 Handle,
  IN  LIST_ENTRY                 *BdsBootOptionList,
  IN  CHAR16                     *String
  );

/**
  Build the on flash shell boot option with the handle parsed in.

  @param[in] Handle              The handle which present the device path to create
                                 the on flash shell boot option.
  @param[in] BdsBootOptionList   The header of the link list that indexed all
                                 current boot options.
  @param[in] Description         The description of the shell boot option.
**/
VOID
EFIAPI
BdsLibBuildOptionFromShell (
  IN  EFI_HANDLE                     Handle,
  IN  LIST_ENTRY                     *BdsBootOptionList,
  IN  CHAR16                         *Description
  );

//
// Bds misc lib functions
//
/**
 Get boot mode by looking up the configuration table and parsing the HOB list.

 @param[out] BootMode          The boot mode from PEI handoff HOB.

 @retval EFI_SUCCESS           Successfully got boot mode.
**/
EFI_STATUS
EFIAPI
BdsLibGetBootMode (
  OUT EFI_BOOT_MODE       *BootMode
  );

/**
 The function will go through the driver option link list, and then load and start
 every driver to which the driver option device path points.

 @param[in] BdsDriverLists        The header of the current driver option link list.
**/
VOID
EFIAPI
BdsLibLoadDrivers (
  IN LIST_ENTRY                   *BdsDriverLists
  );

/**
 This function processes BootOrder or DriverOrder variables, by calling

 BdsLibVariableToOption () for each UINT16 in the variables.

 @param[in] BdsCommonOptionList   The header of the option list base on the variable VariableName.
 @param[in] VariableName          An EFI Variable name indicate the BootOrder or DriverOrder.

 @retval EFI_SUCCESS           Successfully created the boot option or driver option list.
 @retval EFI_OUT_OF_RESOURCES  Failed to get the boot option or the driver option list.
**/
EFI_STATUS
EFIAPI
BdsLibBuildOptionFromVar (
  IN  LIST_ENTRY                      *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  );

/**
 This function reads the EFI variable (VendorGuid/Name) and returns a dynamically allocated
 buffer and the size of the buffer. If it fails, return NULL.

 @param[in]  Name              The string part of the  EFI variable name.
 @param[in]  VendorGuid        The GUID part of the EFI variable name.
 @param[out] VariableSize      Returns the size of the EFI variable that was read.

 @return                       Dynamically allocated memory that contains a copy
                               of the EFI variable. The caller is responsible for freeing the buffer.
 @retval NULL                  The variable was not read.
**/
VOID *
EFIAPI
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

/**
 This function prints a series of strings.

 @param[in] ConOut             A pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.
 @param[in] ...                A variable argument list containing a series of
                               strings, the last string must be NULL.

 @retval EFI_SUCCESS           Successfully printed out the string using ConOut.
 @retval EFI_STATUS            Return the status of the ConOut->OutputString ().
**/
EFI_STATUS
EFIAPI
BdsLibOutputStrings (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut,
  ...
  );

/**
 Build the boot#### or driver#### option from the VariableName. The
 build boot#### or driver#### will also be linked to BdsCommonOptionList.

 @param[in] BdsCommonOptionList   The header of the boot#### or driver#### option link list.
 @param[in] VariableName          EFI Variable name, indicates if it is boot#### or driver####.

 @retval BDS_COMMON_OPTION     The option that was created.
 @retval NULL                  Failed to get the new option.
**/
BDS_COMMON_OPTION *
EFIAPI
BdsLibVariableToOption (
  IN OUT LIST_ENTRY                   *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  );

/**
 This function registers the new boot#### or driver#### option based on
 the VariableName. The new registered boot#### or driver#### will be linked
 to BdsOptionList and also update to the VariableName. After the boot#### or
 driver#### updated, the BootOrder or DriverOrder will also be updated.

 @param[in] BdsOptionList         The header of the boot#### or driver#### link list.
 @param[in] DevicePath            The device path that the boot#### or driver#### option present.
 @param[in] String                The description of the boot#### or driver####.
 @param[in] VariableName          Indicate if the boot#### or driver#### option.
 @param[in] OptionalData          Option data.
 @param[in] OptionalDataSize      Optional data size.

 @retval EFI_SUCCESS              The boot#### or driver#### have been successfully registered.
 @retval EFI_STATUS               Return the status of gRT->SetVariable ().
**/
EFI_STATUS
EFIAPI
BdsLibRegisterNewOption (
  IN  LIST_ENTRY                     *BdsOptionList,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  );

//
// Bds connect and disconnect driver lib funcions
//
/**
 This function connects all system drivers with the corresponding controllers.
**/
VOID
EFIAPI
BdsLibConnectAllDriversToAllControllers (
  VOID
  );

/**
 This function connects all system drivers to controllers.
**/
VOID
EFIAPI
BdsLibConnectAll (
  VOID
  );

/**
 This function creates all handles associated with the given device
 path node. If the handle associated with one device path node cannot
 be created, then it tries to execute the dispatch to load the missing drivers.

 @param[in] DevicePathToConnect   The device path to be connected. Can be
                                  a multi-instance device path.

 @retval EFI_SUCCESS           All handles associates with every device path node were created.
 @retval EFI_OUT_OF_RESOURCES  Not enough resources to create new handles.
 @retval EFI_NOT_FOUND         At least one handle could not be created.
**/
EFI_STATUS
EFIAPI
BdsLibConnectDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  );

/**
 This function will connect all current system handles recursively.
 gBS->ConnectController() service is invoked for each handle exist in system handler buffer.
 If the handle is bus type handler, all childrens also will be connected recursively  by gBS->ConnectController().

 @retval EFI_SUCCESS           All handles and child handles have been connected.
 @retval EFI_STATUS            Return the status of gBS->LocateHandleBuffer().
**/
EFI_STATUS
EFIAPI
BdsLibConnectAllEfi (
  VOID
  );

/**
 This function will disconnect all current system handles.
 gBS->DisconnectController() is invoked for each handle exists in system handle buffer.
 If handle is a bus type handle, all childrens also are disconnected recursively by  gBS->DisconnectController().

 @retval EFI_SUCCESS           All handles have been disconnected.
 @retval EFI_STATUS            Error status returned by of gBS->LocateHandleBuffer().
**/
EFI_STATUS
EFIAPI
BdsLibDisconnectAllEfi (
  VOID
  );

//
// Bds console related lib functions
//
/**
 This function will search every simpletxt device in the current system,
 and make every simpletxt device a potential console device.
**/
VOID
EFIAPI
BdsLibConnectAllConsoles (
  VOID
  );

/**
 This function will connect console device based on the console
 device variable ConIn, ConOut and ErrOut.

 @retval EFI_SUCCESS              At least one of the ConIn and ConOut devices have
                                  been connected.
 @retval EFI_STATUS               Return the status of BdsLibConnectConsoleVariable ().
**/
EFI_STATUS
EFIAPI
BdsLibConnectAllDefaultConsoles (
  VOID
  );

/**
 This function updates the console variable based on ConVarName. It can
 add or remove one specific console device path from the variable

 @param[in] ConVarName               The console-related variable name: ConIn, ConOut, ErrOut.
 @param[in] CustomizedConDevicePath  The console device path to be added to
                                     the console variable ConVarName. Cannot be multi-instance.
 @param[in] ExclusiveDevicePath      The console device path to be removed
                                     from the console variable ConVarName. Cannot be multi-instance.

 @retval EFI_SUCCESS              Successfully added or removed the device path from the console variable.
 @retval EFI_UNSUPPORTED          The added device path is the same as a removed one.
**/
EFI_STATUS
EFIAPI
BdsLibUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  );

/**
 Connect the console device base on the variable ConVarName. If
 ConVarName is a multi-instance device path, and at least one
 instance connects successfully, then this function
 will return success.

 @param[in] ConVarName            The console related variable name: ConIn, ConOut, ErrOut.

 @retval EFI_SUCCESS              Connected at least one instance of the console
                                  device path based on the variable ConVarName.
 @retval EFI_NOT_FOUND            No console devices were connected successfully
**/
EFI_STATUS
EFIAPI
BdsLibConnectConsoleVariable (
  IN  CHAR16                 *ConVarName
  );

//
// Bds device path related lib functions
//
/**
 Delete the instance in Multi that overlaps with Single.

 @param[in] Multi                 A pointer to a multi-instance device path data structure.
 @param[in] Single                A pointer to a single-instance device path data structure.

 @return This function removes the device path instances in Multi that overlap
         Single, and returns the resulting device path. If there is no
         remaining device path as a result, this function will return NULL.
**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  );

/**
 This function compares a device path data structure to that of all the nodes of a
 second device path instance.

 @param[in] Multi              A pointer to a multi-instance device path data structure.
 @param[in] Single             A pointer to a single-instance device path data structure.

 @retval TRUE                  If the Single device path is contained within a Multi device path.
 @retval FALSE                 The Single device path is not contained within a Multi device path.
**/
BOOLEAN
EFIAPI
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  );

/**
 This function converts an input device structure to a Unicode string.

 @param[in] DevPath                  A pointer to the device path structure.

 @return A newly allocated Unicode string that represents the device path.
**/
CHAR16 *
EFIAPI
DevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  );

//
// Internal definitions
//
typedef struct {
  CHAR16  *Str;
  UINTN   Len;
  UINTN   Maxlen;
} POOL_PRINT;

typedef
VOID
(*DEV_PATH_FUNCTION) (
  IN OUT POOL_PRINT       *Str,
  IN VOID                 *DevPath
  );

typedef struct {
  UINT8             Type;
  UINT8             SubType;
  DEV_PATH_FUNCTION Function;
} DEVICE_PATH_STRING_TABLE;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  Header;
  EFI_GUID                  Guid;
  UINT8                     VendorDefinedData[1];
} VENDOR_DEVICE_PATH_WITH_DATA;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  Header;
  UINT16                    NetworkProtocol;
  UINT16                    LoginOption;
  UINT64                    Lun;
  UINT16                    TargetPortalGroupTag;
  CHAR16                    TargetName[1];
} ISCSI_DEVICE_PATH_WITH_NAME;

//
// BBS support macros and functions
//

//
// Notes: EFI 64 shadow all option rom
//
#ifdef MDE_CPU_X64
#define EFI64_SHADOW_ALL_LEGACY_ROM()
//#define EFI64_SHADOW_ALL_LEGACY_ROM() ShadowAllOptionRom ();
//VOID
//ShadowAllOptionRom();
#else
#define EFI64_SHADOW_ALL_LEGACY_ROM()
#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)
#define REFRESH_LEGACY_BOOT_OPTIONS \
        BdsDeleteAllInvalidLegacyBootOptions ();\
        BdsAddNonExistingLegacyBootOptions (); \
        BdsUpdateLegacyDevOrder ()
#else
#define REFRESH_LEGACY_BOOT_OPTIONS
#endif

/**
 Delete all the invalid legacy boot options.

 @retval EFI_SUCCESS             All invalid legacy boot options are deleted.
 @retval EFI_OUT_OF_RESOURCES    Failed to allocate necessary memory.
 @retval EFI_NOT_FOUND           Failed to retrieve variable of boot order.
**/
EFI_STATUS
EFIAPI
BdsDeleteAllInvalidLegacyBootOptions (
  VOID
  );

/**
 Add the legacy boot options from BBS table if they do not exist.

 @retval EFI_SUCCESS          The boot options were added successfully,
                              or they are already in boot options.
 @retval EFI_NOT_FOUND        No legacy boot options is found.
 @retval EFI_OUT_OF_RESOURCE  No enough memory.
 @return Other value          LegacyBoot options are not added.
**/
EFI_STATUS
EFIAPI
BdsAddNonExistingLegacyBootOptions (
  VOID
  );

/**
 Add the legacy boot devices from BBS table into
 the legacy device boot order.

 @retval EFI_SUCCESS           The boot devices were added successfully.
 @retval EFI_NOT_FOUND         The legacy boot devices are not found.
 @retval EFI_OUT_OF_RESOURCES  Memory or storage is not enough.
 @retval EFI_DEVICE_ERROR      Failed to add the legacy device boot order into EFI variable
                               because of a hardware error.
**/
EFI_STATUS
EFIAPI
BdsUpdateLegacyDevOrder (
  VOID
  );

/**
 Refresh the boot priority for BBS entries based on boot option entry and boot order.

 @param[in] Entry              The boot option is to be checked for a refreshed BBS table.

 @retval EFI_SUCCESS           The boot priority for BBS entries refreshed successfully.
 @retval EFI_NOT_FOUND         BBS entries can't be found.
 @retval EFI_OUT_OF_RESOURCES  Failed to get the legacy device boot order.
**/
EFI_STATUS
EFIAPI
BdsRefreshBbsTableForBoot (
  IN BDS_COMMON_OPTION        *Entry
  );

//
//The interface functions related to the Setup Browser Reset Reminder feature
//
/**
 Enable the setup browser reset reminder feature.
 This routine is used in a platform tip. If the platform policy needs the feature, use the routine to enable it.
**/
VOID
EFIAPI
EnableResetReminderFeature (
  VOID
  );

/**
 Disable the setup browser reset reminder feature.
 This routine is used in a platform tip. If the platform policy does not want the feature, use the routine to disable it.
**/
VOID
EFIAPI
DisableResetReminderFeature (
  VOID
  );

/**
 Record the info that a reset is required.
 A module boolean variable is used to record whether a reset is required.
**/
VOID
EFIAPI
EnableResetRequired (
  VOID
  );

/**
 Record the info that no reset is required.
 A module boolean variable is used to record whether a reset is required.
**/
VOID
EFIAPI
DisableResetRequired (
  VOID
  );

/**
 Check whether platform policy enables the reset reminder feature. The default is enabled.
**/
BOOLEAN
EFIAPI
IsResetReminderFeatureEnable (
  VOID
  );

/**
 Check if the user changed any option setting that needs a system reset to be effective.
**/
BOOLEAN
EFIAPI
IsResetRequired (
  VOID
  );

/**
 Check whether a reset is needed, and finish the reset reminder feature.
 If a reset is needed, pop up a menu to notice user, and finish the feature
 according to the user selection.
**/
VOID
EFIAPI
SetupResetReminder (
  VOID
  );

/**
 According to option number to check this boot option is whether
 dummy boot option.

 @param[in] OptionNum   UINT16 to save boot option number.

 @retval TRUE           This boot option is dummy boot option.
 @retval FALSE          This boot option isn't dummy boot option.
**/
BOOLEAN
BdsLibIsDummyBootOption (
  IN UINT16       OptionNum
  );

/**
 This function allocates memory to generate load option. It is caller's responsibility
 to free load option if caller no longer requires the content of load option.

 @param[in]  DevicePath         Pointer to a packed array of UEFI device paths.
 @param[in]  Description        The user readable description for the load option.
 @param[in]  OptionalData       Pointer to optional data for load option.
 @param[in]  OptionalDataSize   The size of optional data.
 @param[out] LoadOption         Double pointer to load option.
 @param[out] LoadOptionSize     The load option size by byte.

 @retval EFI_SUCCESS            Generate load option successful.
 @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
 @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
**/
EFI_STATUS
BdsLibCreateLoadOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT VOID                           **LoadOption,
  OUT UINTN                          *LoadOptionSize
  );

/**
 This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to
 "BootOrder"  variable.

 @retval EFI_SUCCESS      Restore the contents of "BootOrder" variable successful.
 @retval Other            Any error occurred while restoring data to "BootOrder" variable.
**/
EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  );

/**
 This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
 the contents of "BootOrder" variable.

 @retval EFI_SUCCESS      Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
 @retval Other            Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.
**/
EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  );

/**
 According to input boot order to get the boot order type.

 @param[in]  BootOrder          Pointer to BootOrder array.
 @param[in]  BootOrderNum       The boot order number in boot order array.
 @param[out] PhysicalBootOrder  BOOLEAN value to indicate is whether physical boot order.

 @retval EFI_SUCCESS            Determine the boot order is physical boot order or virtual boot order successful.
 @retval EFI_INVALID_PARAMETER  BootOrder is NULL, BootOrderNum is 0 or PhysicalBootOrder is NULL.
**/
EFI_STATUS
BdsLibGetBootOrderType (
  IN  UINT16        *BootOrder,
  IN  UINTN         BootOrderNum,
  OUT BOOLEAN       *PhysicalBootOrder
  );

/**
 According to boot option number to get the description from this boot option.
 It is caller's responsibility to free the description if caller no longer requires
 the content of description.

 @param[in] OptionNum           The boot option number.

 @return NULL                   Get description from boot option failed.
 @return Other                  Get description from boot option successful.
**/
CHAR16 *
BdsLibGetDescriptionFromBootOption (
  IN UINT16     OptionNum
  );

/**
 According to boot option number to get the device path from this boot option.
 It is caller's responsibility to free the device path if caller no longer requires
 the content of device path.

 @param[in] OptionNum           The boot option number.

 @return NULL                   Get device path from boot option failed.
 @return Other                  Get device path from boot option successful.
**/
EFI_DEVICE_PATH_PROTOCOL *
BdsLibGetDevicePathFromBootOption (
  IN UINT16     OptionNum
  );

/**
 This function uses to remove all of physical boot options and add virtual boot
 options to "BootOrder' variable

 @retval EFI_SUCCESS             Change the contents of "BootOrder" successful
 @retval EFI_NOT_FOUND           "BootOrder" variable doesn't exist.
 @retval Other                   Set "BootOrder" variable failed.
**/
EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  );

EFI_STATUS
BdsLibGetMappingBootOptions (
  IN    UINT16         OptionNum,
  OUT   UINTN          *OptionCount,
  OUT   UINT16         **OptionOrder
  );

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

///
/// Define the boot type with which to classify the boot option type.
/// Different boot option types could have different boot behaviors.
/// Use their device path node (Type + SubType) as the type value.
/// The boot type here can be added according to requirements.
///

///
/// ACPI boot type. For ACPI devices, using sub-types to distinguish devices is not allowed, so hardcode their values.
///
#define  BDS_EFI_ACPI_FLOPPY_BOOT         0x0201
///
/// Message boot type
/// If a device path of boot option only points to a message node, the boot option is a message boot type.
///
#define  BDS_EFI_MESSAGE_ATAPI_BOOT       0x0301 // Type 03; Sub-Type 01
#define  BDS_EFI_MESSAGE_SCSI_BOOT        0x0302 // Type 03; Sub-Type 02
#define  BDS_EFI_MESSAGE_USB_DEVICE_BOOT  0x0305 // Type 03; Sub-Type 05
#define  BDS_EFI_MESSAGE_SATA_BOOT        0x0312 // Type 03; Sub-Type 18
#define  BDS_EFI_MESSAGE_ISCSI_BOOT       0x0313 // Type 03; Sub-Type 19
#define  BDS_EFI_MESSAGE_MAC_BOOT         0x030b // Type 03; Sub-Type 11
#define  BDS_EFI_MESSAGE_MISC_BOOT        0x03FF

///
/// Media boot type
/// If a device path of boot option contains a media node, the boot option is media boot type.
///
#define  BDS_EFI_MEDIA_HD_BOOT            0x0401 // Type 04; Sub-Type 01
#define  BDS_EFI_MEDIA_CDROM_BOOT         0x0402 // Type 04; Sub-Type 02
#define  BDS_EFI_MEDIA_FV_FILEPATH_BOOT   0x0406 // Type 04; Sub-Type 06
///
/// BBS boot type
/// If a device path of boot option contains a BBS node, the boot option is BBS boot type.
///
#define  BDS_LEGACY_BBS_BOOT              0x0501 //  Type 05; Sub-Type 01

#define  BDS_EFI_UNSUPPORT                0xFFFF


//
// for PcdH2ONetworkSupported
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

/**
 Check whether an instance in BlockIoDevicePath has the same partition node as the HardDriveDevicePath device path.

 @param[in] BlockIoDevicePath      Multi device path instances to check.
 @param[in] HardDriveDevicePath    A device path starting with a hard drive media device path.

 @retval TRUE                   There is a matched device path instance.
 @retval FALSE                  There is no matched device path instance.
**/
BOOLEAN
EFIAPI
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  );

/**
 Expand a device path that starts with a hard drive media device path node to be a
 full device path that includes the full hardware path to the device. This function enables the device to boot.
 To avoid requiring a connect on every boot, the front match is saved in a variable (the part point
 to the partition node. E.g. ACPI() /PCI()/ATA()/Partition() ).
 All successful history device paths
 that point to the front part of the partition node will be saved.

 @param[in] HardDriveDevicePath    EFI Device Path to boot, if it starts with a hard
                                   drive media device path.
 @return A Pointer to the full device path, or NULL if a valid Hard Drive devic path
         cannot be found.
**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsExpandPartitionPartialDevicePathToFull (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  );

/**
 Return the bootable media handle.
 First, check whether the device is connected.
 Second, check whether the device path points to a device that supports SimpleFileSystemProtocol.
 Third, detect the the default boot file in the Media, and return the removable Media handle.

 @param[in] DevicePath             The Device Path to a  bootable device.

 @return The bootable media handle. If the media on the DevicePath is not bootable, NULL will return.
**/
EFI_HANDLE
EFIAPI
BdsLibGetBootableHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  );

/**
 Checks whether the Device path in a boot option points to a valid bootable device, and if the device
 is ready to boot now.

 @param[in] DevPath     The Device path in a boot option.
 @param[in] CheckMedia  If true, check whether the device is ready to boot now.

 @retval TRUE        The Device path is valid.
 @retval FALSE       The Device path is invalid.
**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia
  );

/**
 Checks whether the Device path in a boot option points to a valid bootable device, and if the device
 is ready to boot now.
 If Description is not NULL and the device path points to a fixed BlockIo
 device, this function checks whether the description conflicts with other auto-created
 boot options.

 @param[in] DevPath     The Device path in a boot option.
 @param[in] CheckMedia  If true, checks if the device is ready to boot now.
 @param[in] Description The description of a boot option.

 @retval TRUE        The Device path is valid.
 @retval FALSE       The Device path is invalid.
**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePathExt (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia,
  IN CHAR16                       *Description
  );

/**
 For a bootable Device path, return its boot type.

 @param[in] DevicePath                   The bootable device Path to check.

 @retval BDS_EFI_MEDIA_HD_BOOT           The given device path contains MEDIA_DEVICE_PATH type device path node,
                                         whose subtype is MEDIA_HARDDRIVE_DP.
 @retval BDS_EFI_MEDIA_CDROM_BOOT        If given device path contains MEDIA_DEVICE_PATH type device path node,
                                         whose subtype is MEDIA_CDROM_DP.
 @retval BDS_EFI_ACPI_FLOPPY_BOOT        A given device path contains ACPI_DEVICE_PATH type device path node,
                                         whose HID is floppy device.
 @retval BDS_EFI_MESSAGE_ATAPI_BOOT      A given device path contains MESSAGING_DEVICE_PATH type device path node,
                                         and its last device path node's subtype is MSG_ATAPI_DP.
 @retval BDS_EFI_MESSAGE_SCSI_BOOT       A given device path contains MESSAGING_DEVICE_PATH type device path node,
                                         and its last device path node's subtype is MSG_SCSI_DP.
 @retval BDS_EFI_MESSAGE_USB_DEVICE_BOOT A given device path contains MESSAGING_DEVICE_PATH type device path node,
                                         and its last device path node's subtype is MSG_USB_DP.
 @retval BDS_EFI_MESSAGE_MISC_BOOT       The device path does not contain any media device path node, and
                                         its last device path node points to a message device path node.
 @retval BDS_LEGACY_BBS_BOOT             A given device path contains BBS_DEVICE_PATH type device path node.
 @retval BDS_EFI_UNSUPPORT               An EFI Removable BlockIO device path does not point to a media and message device.
**/
UINT32
EFIAPI
BdsGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

/**
 This routine registers a function to adjust the different types of memory page numbers
 just before booting, and saves the updated info into the variable for the next boot to use.
**/
VOID
EFIAPI
BdsLibSaveMemoryTypeInformation (
  VOID
  );

/**
 Identify a user and, if authenticated, returns the current user profile handle.

 @param[out]  User           Points to the user profile handle.

 @retval EFI_SUCCESS         The user is successfully identified, or user identification
                             is not supported.
 @retval EFI_ACCESS_DENIED   The user was not successfully identified.
**/
EFI_STATUS
EFIAPI
BdsLibUserIdentify (
  OUT EFI_USER_PROFILE_HANDLE         *User
  );

/**
 This function checks if a Fv file device path is valid, according to a file GUID. If it is invalid,
 it tries to return the valid device path.
 FV address maybe changes for memory layout adjust from time to time, use this funciton
 could promise the Fv file device path is right.

 @param[in] DevicePath          On input, the Fv file device path to check. On
                                output, the updated valid Fv file device path
 @param[in] FileGuid            the Fv file GUID.

 @retval EFI_INVALID_PARAMETER  The input DevicePath or FileGuid is invalid.
 @retval EFI_UNSUPPORTED        The input DevicePath does not contain an Fv file
                                GUID at all.
 @retval EFI_ALREADY_STARTED    The input DevicePath has pointed to the Fv file and is
                                valid.
 @retval EFI_SUCCESS            Successfully updated the invalid DevicePath
                                and returned the updated device path in DevicePath.
**/
EFI_STATUS
EFIAPI
BdsLibUpdateFvFileDevicePath (
  IN  OUT EFI_DEVICE_PATH_PROTOCOL      ** DevicePath,
  IN  EFI_GUID                          *FileGuid
  );

/**
 Connect the specific USB device that matches the RemainingDevicePath,
 and whose bus is determined by Host Controller (Uhci or Ehci).

 @param[in] HostControllerPI      Uhci (0x00) or Ehci (0x20) or Both uhci and ehci (0xFF).
 @param[in] RemainingDevicePath   A short-form device path that starts with the first
                                  element being a USB WWID or a USB Class device path.

 @retval EFI_SUCCESS           The specific Usb device is connected successfully.
 @retval EFI_INVALID_PARAMETER Invalid HostControllerPi (not 0x00, 0x20 or 0xFF)
                               or RemainingDevicePath is not the USB class device path.
 @retval EFI_NOT_FOUND         The device specified by device path is not found.
**/
EFI_STATUS
EFIAPI
BdsLibConnectUsbDevByShortFormDP(
  IN UINT8                      HostControllerPI,
  IN EFI_DEVICE_PATH_PROTOCOL   *RemainingDevicePath
  );


//
// The implementation of this function is provided by Platform code.
//
/**
 Convert Vendor device path to a device name.

 @param[in] Str      The buffer storing device name.
 @param[in] DevPath  The pointer to vendor device path.
**/
VOID
DevPathVendor (
  IN OUT POOL_PRINT       *Str,
  IN VOID                 *DevPath
  );

/**
 Concatenates a formatted unicode string to an allocated pool.
 The caller must free the resulting buffer.

 @param[in] Str      Tracks the allocated pool, size in use, and amount of pool allocated.
 @param[in] Fmt      The format string.
 @param[in] ...      The data will be printed.

 @return Allocated buffer with the formatted string printed in it.
         The caller must free the allocated buffer.
         The buffer allocation is not packed.
**/
CHAR16 *
EFIAPI
CatPrint (
  IN OUT POOL_PRINT   *Str,
  IN CHAR16           *Fmt,
  ...
  );

UINTN
BdsLibGetBootType (
  VOID
  );

EFI_STATUS
BdsLibSkipEbcDispatch (
  VOID
  );

VOID
EFIAPI
BdsLibGetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  );

EFI_STATUS
BdsLibShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  );

//
// Recovery Flash Funciton definition
//
UINT8
EFIAPI
FlashWrite (
  IN UINT8               *Buffer,
  IN UINT32               FlashSize,
 IN UINT32               FlashAddress,
 IN UINT16               SmiPort
  );

UINT8
EFIAPI
FlashPartInfo (
  IN     UINT8            *PartInfo,
  IN     UINT8            *PartBlock,
  IN     UINT8            FlashTypeSelect,
  IN     UINT16           SmiPort
  );

UINT8
EFIAPI
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
  IN HOT_KEY_CONTEXT      *NotifyContext
  );

EFI_STATUS
BdsLibSetHotKeyDelayTime (
  VOID
  );

EFI_STATUS
BdsLibGetHotKey (
  OUT UINT16    *FunctionKey,
  OUT BOOLEAN   *HotKeyPressed
  );

EFI_STATUS
LegacyBiosDependency (
  IN EFI_EVENT                   Event
  );

VOID
BdsLibConnectUsbHID (
  VOID
  );

/**
 Register USB enumeration protocol event.
 User can install protocol to trigger event to do USB connection.

 @retval EFI_SUCCESS  Notify register success.
 @retval Other        Notify register fail.
**/
EFI_STATUS
EFIAPI
BdsLibConnectUsbHIDNotifyRegister (
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
BdsLibGetQuietBootState (
  OUT BOOLEAN        *QuietBootState
  );

EFI_STATUS
BdsLibShowOemStringInTextMode (
  IN BOOLEAN                       AfterSelect,
  IN UINT8                         SelectedStringNum
  );

EFI_STATUS
ShowSystemInfo (
  IN OUT UINT8                        *PrintLine
  );

VOID
BdsLibConnectLegacyRoms (
  VOID
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

EFI_STATUS
BdsLibStopHotKeyEvent (
  VOID
  );

EFI_STATUS
BdsLibOnEndOfBdsBootSelection (
  VOID
  );

EFI_STATUS
BdsLibStartSetupUtility (
  BOOLEAN       PasswordCheck
  );

BOOLEAN
BdsLibMatchFilePathDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *FirstDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL   *SecondDevicePath
  );

EFI_STATUS
GetNewBootOptionNo (
  IN     UINT16                                     *BootOrder,
  IN     UINTN                                      BootOptionNum,
  IN OUT UINT16                                     *CurrentBootOptionNo
  );

EFI_STATUS
BdsLibNewBootOptionPolicy(
  UINT16                                     **NewBootOrder,
  UINT16                                     *OldBootOrder,
  UINTN                                      OldBootOrderCount,
  UINT16                                     NewBootOptionNo,
  UINTN                                      Policy
  );

EFI_DEVICE_PATH_PROTOCOL *
BdsLibUnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  );

BOOLEAN
IsAdministerSecureBootSupport (
  VOID
  );

EFI_STATUS
BdsLibDeleteInvalidBootOptions (
  VOID
  );

BOOLEAN
BdsLibIsLegacyBootOption (
  IN UINT8                 *BootOptionVar,
  OUT BBS_TABLE            **BbsEntry,
  OUT UINT16               *BbsIndex
  );

EFI_STATUS
BdsLibRemovedBootOption (
  IN  BOOLEAN                           RemovedLegacy
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
  OUT HARDWARE_BOOT_DEVICE_INFO      **HwBootDeviceInfo
  );

UINT32
BdsLibGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

EFI_STATUS
BdsLibUpdateInvalidBootOrder (
  IN OUT UINT16        **BootOrderPtr,
  IN     UINTN         Index,
  IN OUT UINTN         *BootOrderSize
  );

EFI_STATUS
EFIAPI
BdsLibDeleteBootOption (
  IN UINTN                       OptionNumber,
  IN OUT UINT16                  *BootOrder,
  IN OUT UINTN                   *BootOrderSize
  );

BOOLEAN
BdsLibCompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  );

EFI_STATUS
RecoveryPopUp (
  IN  UINTN     FlashMode
  );

VOID
RecoveryFlash (
  IN  UINTN     FlashMode
  );


EFI_STATUS
EFIAPI
BdsLibSetConsoleMode (
  BOOLEAN  IsSetupMode
  );

EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  );

EFI_STATUS
BdsBootByDiskSignatureAndPartition (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath,
  IN  UINT32                     LoadOptionsSize,
  IN  VOID                       *LoadOptions,
  OUT UINTN                      *ExitDataSize,
  OUT CHAR16                     **ExitData OPTIONAL
  );

EFI_STATUS
BdsLibGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  );

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

/**
 Based on the boot option number, return the dummy boot option number.
 If return DummyBootOptionEndNum, it means this boot option does not belong to dummy boot option.

 @param[in] OptionNum           The boot option number.

 @return Dummy boot option number or DummyBootOptionEndNum if input boot option does not belong to dummy boot option.
**/
DUMMY_BOOT_OPTION_NUM
BdsLibGetDummyBootOptionNum (
  IN  UINT16                 OptionNum
  );

/**
  Check if option variable is created by BIOS or not.

  @param[in]  Variable          Pointer to option variable
  @param[in]  VariableSize      Option variable size

  @retval     TRUE              Option varible is created by BIOS
  @retval     FALSE             Option varible is not created by BIOS
**/
BOOLEAN
BdsLibIsBiosCreatedOption (
  IN UINT8                  *Variable,
  IN UINTN                  VariableSize
  );

/**
  Enable BootOrder variable hook mechanism.

  @retval EFI_SUCCESS    Enable BootOrder variable hook mechanism successfully.
  @return Other          Enable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  );

/**
  Disable BootOrder variable hook mechanism.

  @retval EFI_SUCCESS    Disable BootOrder variable hook mechanism successfully.
  @return Other          Disable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  );

/**
  Function uses to check BootOrder variable hook mechanism is whether enabled.

  @retval TRUE    BootOrder variable hook mechanism is enabled.
  @retval FALSE   BootOrder variable hook mechanism is disabled
**/
BOOLEAN
BdsLibIsBootOrderHookEnabled (
  VOID
  );

#endif

