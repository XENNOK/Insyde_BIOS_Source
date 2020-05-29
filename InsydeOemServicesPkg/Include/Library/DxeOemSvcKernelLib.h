/** @file
  Definition for Oem Services Default Lib.

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

#ifndef _DXE_OEM_SVC_KERNEL_LIB_H_
#define _DXE_OEM_SVC_KERNEL_LIB_H_

#include <Uefi.h>
#include <DmiStringInformation.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Oa3_0.h>
#include <IndustryStandard/SLP2_0.h>
#include <Protocol/DataHubRecordPolicy.h>
#include <Protocol/MsioIsaAcpi.h>
#include <IndustryStandard/LegacyBiosMpTable.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Sio/SioCommon.h>
#include <Library/BadgingSupportLib.h>
#include <Protocol/Smbios.h>

//
// data type definitions
//

#define IGNORE_DEVICE     0xFE
#define END_OF_TABLE      0xFF

#define NULL_ENTRY        0xff

#define DEVICE_ID_DONT_CARE 0xFFFF

#pragma pack (1)

typedef enum {
  UiNoOperation,
  UiDefault,
  UiDiscard,
  UiSelect,
  UiUp,
  UiDown,
  UiLeft,
  UiRight,
  UiReset,
  UiSave,
  UiSaveCustom,
  UiDefaultCustom,
  UiSaveAndExit,
  UiPrevious,
  UiPageUp,
  UiPageDown,
  UiJumpMenu,
  UiShowHelpScreen,
  UiMaxOperation
} UI_SCREEN_OPERATION;

typedef struct {
  UINT16              ScanCode;
  CHAR16              UnicodeChar;
  UI_SCREEN_OPERATION ScreenOperation;
} SCAN_CODE_TO_SCREEN_OPERATION;

typedef struct {
  UINT8   NodeUID;
  UINT8   DevNum;
  UINT8   FuncNum;
  UINT8   ReservedBusCount;
  UINT16  ReservedIoRange;
  UINT64  ReservedNonPrefetchableMmio;
  UINT64  AlignemntOfReservedNonPrefetchableMmio;
  UINT64  ReservedPrefetchableMmio;
  UINT64  AlignemntOfReservedPrefetchableMmio;
} HOT_PLUG_BRIDGE_INFO;


typedef struct {
  UINT16    VendorId;
  UINT16    DeviceId;
} PCI_SKIP_TABLE;

typedef struct  {
  UINT32       LogoResolutionX;
  UINT32       LogoResolutionY;
  UINT32       ScuResolutionX;
  UINT32       ScuResolutionY;
} OEM_LOGO_RESOLUTION_DEFINITION;

typedef struct {
  UINT8                             BridgeBus;
  UINT8                             BridgeDev;
  UINT8                             BridgeFunc;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT  SlotIrqTable;
} MP_TABLE_ENTRY_IO_INT_SLOT;

typedef enum {
  PCI_OPROM = 0x01,
  SYSTEM_ROM,
  SYSTEM_OEM_INT_ROM,
  SERVICE_ROM,
  BIS_LOADER,
  TPM_ROM,
  AMT_ROM,
  MAX_NUM = 0xFF
} OPROM_TYPE;

typedef struct {
  EFI_GUID  FileName;
  UINT16    VendorId;
  UINT16    DeviceId;
} PCI_OPTION_ROM_TABLE;

typedef struct {
  EFI_GUID   FileName;
  BOOLEAN    Valid;
  BOOLEAN    OptionRomSpace;
  UINTN      OpRomType;
} SERVICE_ROM_TABLE;

typedef struct {
  EFI_GUID   FileName;
  UINTN      Valid;
  UINTN      OpRomType;
} SYSTEM_ROM_TABLE;

typedef struct {
  UINT16   PrepareLegacyTableBeforeUEFIBoot :1;   // 0: Disable                                       ; 1: Enable
  UINT16   DisableKeyboardReset             :1;   // 0: Enable KeyboardReset                          ; 1: Disable KeyboardReset
  UINT16   DisableKeyboardBeep              :1;   // 0: Enable KeyboardBeep                           ; 1: Disable KeyboardBeep
  UINT16   LegacyAutoBootRetry              :1;   // 0: Disable AutoBootRetry                         ; 1: Enable AutoBootRetry
  UINT16   ODDImageDetectAfterINT19         :1;   // 0: Detect image before INT19                     ; 1: Detect image after INT19
  UINT16   SET_VGAMODE_BEFORE_INT19         :1;   // 0: Disable                                       ; 1: Set Vga mode before INT19
  UINT16   ENABLE_A20_BEFORE_INT19          :1;   // 0: Disable                                       ; 1: Enable A20 before INT19
  UINT16   A20_GATE_SUPPORT                 :1;   // 0: supported on keyboard controller              ; 1: supported with bit 1 of I/O port 92h
  UINT16   MonitorKey_With_OPROM            :1;   // 0: Disable MonitorKey during dispatch OPROM      ; 1: Enable MonitorKey during dispatch OPROM
  UINT16   PauseKeySupport                  :1;   // 0: Enable Pause Key                              ; 1: Disable Pause Key
  UINT16   SMBIOS_DATA_LOCATE               :1;   // 0: Locate at E segment                           ; 1: Locate at F segment
  UINT16   Pmm_High_Memory_Size             :3;   // if the value is 0: 4 MB (default)
                                                  // if the value is 1: 2 to the (4 + 1) = 32 MB
                                                  // if the value is 2: 2 to the (4 + 2) = 64 MB
                                                  // if the value is 3: 2 to the (4 + 3) = 128 MB
                                                  // if the value is 4: 2 to the (4 + 4) = 256 MB
                                                  // if the value is 5: 2 to the (4 + 5) = 512 MB
                                                  // if the value is 6: 2 to the (4 + 6) = 1024 MB
                                                  // if the value is 7: 2 to the (4 + 7) = 2048 MB
  UINT16   Rsvd                             :2;
} CSM16_SWITCH;

typedef union _CSM16_REFER_SWITCH {
  CSM16_SWITCH          Config;
  UINT16                CONFIG;
} CSM16_OEM_CONFIG;

#pragma pack ()

typedef struct  {
  UINT32                                Attributes;
  UINTN                                 EdidSize;
  UINT8                                 *EdidData;
  EFI_DEVICE_PATH_PROTOCOL              *DisplayDevPath;
} OEM_GET_EDID_OVERRIDE_DEFINITION;

EFI_STATUS
typedef
( EFIAPI *OEM_SSID_SVID_FUNCTION ) (
  IN     UINT8    Bus,
  IN     UINT8    Dev,
  IN     UINT8    Func,
  IN OUT UINT32  *SsidSvid
);

typedef struct {
  UINT16                      VendorId;
  UINT16                      DeviceId;
  OEM_SSID_SVID_FUNCTION      SpecialSsidSvidFunction;
} OEM_SSID_SVID_TABLE;

//
// OemSvc function prototypes
//

EFI_STATUS
OemSvcBootDisplayDeviceReplace (
  IN OUT BOOLEAN                               *SkipOriginalCode
  );

EFI_STATUS
OemSvcCalculateWriteCmosChecksum (
  VOID
  );

EFI_STATUS
OemSvcCheckPasswordFailCallBack (
  VOID
  );

EFI_STATUS
OemSvcCsm16ReferSwitch (
  IN OUT UINT16                                *CSM16OemSwitchPtr
  );

EFI_STATUS
OemSvcDisplayLogo (
  IN OUT BOOLEAN                               *QuietBoot
  );

EFI_STATUS
OemSvcGetHotplugBridgeInfo (
  OUT HOT_PLUG_BRIDGE_INFO                     **HotPlugBridgeInfoTable
  );

EFI_STATUS
OemSvcGetMaxCheckPasswordCount (
  IN OUT UINTN                                 *ReportPasswordCount
  );

EFI_STATUS
OemSvcGetOa30MsdmData (
  IN OUT EFI_ACPI_MSDM_DATA_STRUCTURE          *MsdmData
  );

EFI_STATUS
OemSvcGetSlp20PubkeyAndMarkerRom (
  IN OUT EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE     *PublicKey,
  IN OUT BOOLEAN                               *UpdatedPublickey,
  IN OUT EFI_ACPI_SLP_MARKER_STRUCTURE         *SlpMarker,
  IN OUT BOOLEAN                               *UpdatedMarker
  );

EFI_STATUS
OemSvcUpdateSsidSvidInfo (
  IN  UINT8                                 Bus,
  IN  UINT8                                 Dev,
  IN  UINT8                                 Func,
  IN  UINT16                                VendorId,
  IN  UINT16                                DeviceId,
  IN  UINT16                                ClassCode,
  OUT UINT32                                *SsidSvid
  );

EFI_STATUS
OemSvcInstallBootMangerKeyTable (
  OUT UINTN                                    *ScanCodeToOperationCount,
  OUT SCAN_CODE_TO_SCREEN_OPERATION            **mScanCodeToOperation
  );

EFI_STATUS
OemSvcInstallDmiSwitchTable (
  OUT UINTN                                    *MiscSubclassDefaultSize,
  OUT EFI_DATAHUB_RECORD_POLICY                **mDataHubRecordPolicy
  );

EFI_STATUS
OemSvcInstallLegacyBiosOemSlp (
  OUT UINTN                                    *SlpLength,
  OUT UINTN                                    *SlpAddress
  );

EFI_STATUS
OemSvcInstallOptionRomTable (
  IN  UINT8                                    RomType,
  OUT VOID                                     **mOptionRomTable
  );

EFI_STATUS
OemSvcInstallPciRomSwitchTextMode (
  IN OUT EFI_HANDLE                            DeviceHandle,
  IN OUT BOOLEAN                               *ForceSwitchTextMode
  );

EFI_STATUS
OemSvcInstallPciSkipTable (
  OUT UINTN                                    *PciSkipTableCount,
  OUT PCI_SKIP_TABLE                           **PciSkipTable
  );

EFI_STATUS
OemSvcDxeInstallPnpGpnvTable (
  OUT UINTN                                    *UpdateableGpnvCount,
  OUT OEM_GPNV_MAP                            **GetOemGPNVMap
  );

EFI_STATUS
OemSvcDxeInstallPnpStringTable (
  OUT UINTN                                    *UpdateableStringCount,
  OUT DMI_UPDATABLE_STRING                     **mUpdatableStrings
  );

EFI_STATUS
OemSvcInstallPostKeyTable (
  IN  UINTN                                    KeyDetected,
  IN  UINT16                                   ScanCode,
  OUT UINTN                                    *PostOperation
  );

EFI_STATUS
OemSvcSetPirqPriority (
  IN OUT  UINT8                                **SetPIrqPriority
  );

EFI_STATUS
OemSvcLoadDefaultSetupMenu (
  OUT BOOLEAN                                  *LoadDefaultPtr
  );

EFI_STATUS
OemSvcLogoResolution (
  IN OUT OEM_LOGO_RESOLUTION_DEFINITION        **OemLogoResolutionTable
  );

EFI_STATUS
OemSvcUpdateFormLen (
  IN     UINT8                                 *Data,
  IN     UINT8                                 KeepCurRoot,
  IN OUT CHAR8                                 *PromptBlockWidth,
  IN OUT CHAR8                                 *OptionBlockWidth,
  IN OUT CHAR8                                 *HelpBlockWidth
  );

EFI_STATUS
OemSvcPasswordEntryCallBack (
  VOID
  );

EFI_STATUS
OemSvcPasswordInputFilter (
  IN OUT EFI_KEY_DATA                          *EfiKeyData
  );

EFI_STATUS
OemSvcPrepareInstallMpTable (
  OUT UINTN                                    *CharNumOfOemIdString,
  OUT CHAR8                                    **pOemIdStringEntry,
  OUT UINTN                                    *CharNumOfProductIdString,
  OUT CHAR8                                    **pProductIdStringEntry,
  OUT UINTN                                    *NumOfIoApic,
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC         **pIoApicEntry,
  OUT UINTN                                    *NumOfIoApicIntLegacy,
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT         **pIoApicIntLegacyEntry,
  OUT UINTN                                    *NumOfIoApicInt,
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT         **pIoApicIntEntry,
  OUT UINTN                                    *NumOfIoApicIntSlot,
  OUT MP_TABLE_ENTRY_IO_INT_SLOT               **pIoApicIntSlotEntry,
  OUT UINTN                                    *NumOfLocalApicInt,
  OUT EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT      **pLocalApicIntEntry
  );

EFI_STATUS
OemSvcSkipLoadPciOptionRom (
  IN  UINTN                                    Segment,
  IN  UINTN                                    Bus,
  IN  UINTN                                    Device,
  IN  UINTN                                    Function,
  IN  UINT16                                   VendorId,
  IN  UINT16                                   DeviceId,
  OUT BOOLEAN                                  *SkipGetPciRom
  );

EFI_STATUS
OemSvcUnSkipPciDevice (
  IN UINT8                                     Bus,
  IN UINT8                                     Device,
  IN UINT8                                     Function,
  IN UINT16                                    VendorId,
  IN UINT16                                    DeviceId
  );

EFI_STATUS
OemSvcUpdateBbsTable (
  IN OUT EFI_TO_COMPATIBILITY16_BOOT_TABLE     *EfiToLegacy16BootTable,
  IN OUT BBS_TABLE                             *BbsTable
  );

EFI_STATUS
OemSvcVariableForReclaimFailTable (
  IN OUT PRESERVED_VARIABLE_TABLE              **TablePtr,
  IN OUT UINT32                                *TableSize
  );

EFI_STATUS
OemSvcCustomizeWpbtTable (
  IN OUT EFI_GUID                              *FileGuid,
  IN OUT CHAR16                                *InputArg
  );

EFI_STATUS
OemSvcUpdateAcpiFacsHardwareSignature (
  IN OUT  UINT32                               *HardwareSignature
  );

EFI_STATUS
OemSvcGetEdidOverride (
  IN OUT OEM_GET_EDID_OVERRIDE_DEFINITION      *GetEdidOverride
  );

EFI_STATUS
OemSvcRegSioDxe (
  IN UINT8                            SioInstance,
  IN OUT EFI_SIO_TABLE                **RegTable
  );

EFI_STATUS
OemSvcDeviceResourceSioDxe (
  IN UINT8                            SioInstance,
  IN OUT SIO_DEVICE_LIST_TABLE        *TablePtr,
  IN UINT16                           SioConfig
  );

EFI_STATUS
OemSvcAfterInitSioDxe (
  IN UINT8                            SioInstance,
  IN OUT SIO_DEVICE_LIST_TABLE        *TablePtr,
  IN UINT16                           SioConfig
  );

EFI_STATUS
OemSvcChangeDefaultLogoImage (
  IN OUT EFI_OEM_BADGING_LOGO_DATA       *Logo,
  IN OUT UINT8                           **ImageData,
  IN OUT UINTN                           *ImageSize
  );

EFI_STATUS
OemSvcChangeVbiosBootDisplay (
  VOID
  );

EFI_STATUS
OemSvcUpdateCRPolicy (
  IN OUT VOID    *CRPolicy
  );

EFI_STATUS
OemSvcDxeGetSmbiosReplaceString (
  IN  EFI_SMBIOS_TYPE   Type,
  IN  UINT8             FieldOffset,
  OUT UINTN            *StrLength,
  OUT CHAR8           **String
  );

EFI_STATUS
OemSvcDxeUpdateSmbiosRecord (
  IN OUT EFI_SMBIOS_TABLE_HEADER *RecordBuffer
  );

EFI_STATUS
OemSvcTpmUserConfirmDialog (
  IN  UINT8                          TpmPpCommand,
  IN  UINT8                          TpmVersion,
  OUT BOOLEAN                        *Confirmed
  );

EFI_STATUS
OemSvcAdjustNavigationMenu (
  IN OUT EFI_HII_HANDLE                        *HiiHandle,
  IN OUT UINTN                                 *HiiHandleCount,
  IN OUT EFI_GUID                              *FormSetGuid
  );

#endif
