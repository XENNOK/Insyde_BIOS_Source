/** @file

  Include for AMT Bios Extentions Loader

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AMT_PTBX_H_
#define _AMT_PTBX_H_

#include <PchRegs.h>
#include <PchPlatformLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PerformanceLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/Amt/Dxe/AmtLib.h>

#include "Inventory.h"
#include "UsbProvision.h"

#include <Guid/Acpi.h>
#include <Guid/SmBios.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/Heci.h>
#include <Protocol/MebxProtocol.h>
#include <Protocol/AmtReadyToBoot.h>
#include <Protocol/ActiveManagement.h>
#include <Protocol/AlertStandardFormat.h>


#define MEBX_RET_ACTION_CONTINUE_TO_BOOT  0x01
#define MEBX_RET_ACTION_RESET             0x04
#define MEBX_RET_CODE_MASK                0x07

#define MEBX_RET_ACTION_GLOBAL_RESET      0x01

#define FW_IMAGE_TYPE_1_5MB               3
#define FW_IMAGE_TYPE_5MB                 4

#define MEBX_USER_ENTERED_RESPONSE        0x0002
#define MEBX_RA_SELECTION_MENU            0x0004
#define MEBX_HIDE_UNCONFIGURE_CONF_PROMPT 0x0040
#define MEBX_DEBUG_MSG                    0x4000
#define MEBX_UNCONFIGURE                  0x8000

#define MEBX_STF_PCI_DEV_TABLE            0x0001
#define MEBX_STF_MEDIA_DEV_TABLE          0x0002

#define MEBX_MEDIA_IN_ATA                 0x0000
#define MEBX_MEDIA_IN_ATAPI               0x0001
#define MEBX_MEDIA_IN_SATA                0x0002

#define MEBX_MEDIA_DT_HDD                 0x0000
#define MEBX_MEDIA_DT_CDROM               0x0001
#define MEBX_MEDIA_DT_DVD                 0x0002
#define MEBX_MEDIA_DT_FDD                 0x0003
#define MEBX_MEDIA_DT_MO                  0x0004

///
/// MEBX Software Class DXE Subclass Progress Code definitions.
///
#define EFI_SW_DXE_MEBX_OPROM_DONE              (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_SW_DXE_MEBX_OTHER_UNSPECD           (EFI_OEM_SPECIFIC | 0x00000009)
#define EFI_SW_DXE_MEBX_RESET_ACTION            (EFI_OEM_SPECIFIC | 0x0000000a)

#pragma pack(1)

#define DEVICES_LIST_VERSION            0x0001
#define AMT_DATA_VERSION                0x0101
#define MEBX_SETUP_VERSION              0x9000
#define MEBX_VERSION                    0x9000      ///< for MEBx 9.0.0 or later
#define MEBX_SETUP_PLATFORM_MNT_DEFAULT MNT_OFF
#define MEBX_SETUP_SOL_IDER_DEFAULT     0
#define PCI_DEVICE_MAX_NUM              256
#define MEDIA_DEVICE_MAX_NUM            32

#define SMB_SIG                         0x5F4D535F
#define RSDT_SIG                        0x54445352  ///< RSDT (in reverse)
#define ASF_SIG                         0x21465341  ///< ASF! (in reverse)
#define ONEMIN                          60000000
#define FIVEMIN                         300000000
#define TENMIN                          600000000

#define INVOKE_MEBX_BIT                 0x08

typedef enum _HWAI_FRU_TYPE_
{
  HWAI_FRU_TYPE_NONE= 0,
  HWAI_FRU_TYPE_PCI
} HWAI_FRU_TYPE;

typedef struct {
  UINT16  VersionInfo;
  UINT8   DevCount;
  UINT8   Reserved1[5];
} AMT_FRU_DEVICES_HEADER;

typedef struct {
  UINT16  Vid;
  UINT16  Did;
  UINT16  Rid;
  UINT16  SubSysVid;
  UINT16  SubSysDid;
  UINT32  ClassCode;
  UINT16  BusDevFcn;
} MEBX_FRU_PCI_DEVICES;

typedef struct {
  AMT_FRU_DEVICES_HEADER  PciDevicesHeader;
  MEBX_FRU_PCI_DEVICES    PciDevInfo[PCI_DEVICE_MAX_NUM];
} AMT_PCI_FRU;

typedef struct _PCI_DEV_INFO {
  UINT16  VersionInfo;
  UINT8   PciDevCount;
  UINT8   Reserved[5];
} PCI_DEV_INFO;

typedef struct _HWAI_PCI_FRU_DATA_ {
  UINT16  VendorID;
  UINT16  DeviceID;
  UINT16  RevID;
  UINT16  SubsystemVendorID;
  UINT16  SubsystemDeviceID;
  UINT32  ClassCode;
  UINT16  BusDevFunc;
} HWAI_PCI_DATA;

typedef struct _HWAI_PCI_FRU_ENTRY {
  UINT8         SmbiosType;
  UINT8         Length;
  UINT16        SmbiosHandle;
  UINT32        FruType;
  HWAI_PCI_DATA FruData;
} HWAI_PCI_FRU;

typedef struct _HWAI_FRU_TABLE_HEADER {
  UINT16        TableByteCount;
  UINT16        StructureCount;
  HWAI_PCI_FRU  Data[1];
} HWAI_FRU_TABLE;

typedef struct _HWAI_MEDIA_DATA_ {
  UINT16  Length;
  UINT8   Interface;
  UINT8   DevType;
  UINT8   Manufacturer[40];
  UINT8   SerialNumber[20];
  UINT8   Version[8];
  UINT8   ModelNumber[40];
  UINT32  MaxMediaSizeLow;
  UINT32  MaxMediaSizeHigh;
  UINT16  Capabilities[3];
} HWAI_MEDIA_DATA;

typedef struct _HWAI_MEDIA_ENTRY {
  UINT8           SmbiosType;
  UINT8           Length;
  UINT16          SmbiosHandle;
  HWAI_MEDIA_DATA MediaData;
} HWAI_MEDIA_ENTRY;

typedef struct _HWAI_MEDIA_DEVICE_TABLE_HEADER {
  UINT16            TableByteCount;
  UINT16            StructureCount;
  HWAI_MEDIA_ENTRY  Data[1];
} HWAI_MEDIA_TABLE;

typedef struct _MEDIA_DEV_INFO {
  UINT16  VersionInfo;
  UINT8   MediaDevCount;
  UINT8   Reserved[5];
} MEDIA_DEV_INFO;

typedef struct {
  UINT16  StructSize;
  UINT8   Interface;
  UINT8   DevType;
  UINT8   Rsvd[40];
  UINT8   SerialNo[20];
  UINT8   VersionNo[8];
  UINT8   ModelNo[40];
  UINT64  MaxMediaSize;
  UINT16  SupportedCmdSets[3];
} MEBX_FRU_MEDIA_DEVICES;

typedef struct {
  AMT_FRU_DEVICES_HEADER  MediaDevicesHeader;
  MEBX_FRU_MEDIA_DEVICES  MediaDevInfo[MEDIA_DEVICE_MAX_NUM];
} AMT_MEDIA_FRU;

typedef struct _COMMON_HEADER_ {
  UINT8   Type;
  UINT8   Length;
  UINT16  Handle;
} SMBIOS_HEADER;

typedef struct _SMBIOS_MAIN_ {
  UINT32  Signature;
  UINT8   EpChecksum;
  UINT8   EpLength;
  UINT8   MajorVersion;
  UINT8   MinorVersion;
  UINT16  MaxStruct_Size;
  UINT8   EpRevision;
  UINT8   RevInfo[5];
  UINT8   DmiSignature[5];                          ///< _DMI_
  UINT8   IntChecksum;
  UINT16  TableLength;
  UINT32  TableAddress;
  UINT16  StructureCnt;
  UINT8   SmbBcdRevision;
} SMBIOS_ENTRY_POINT;

typedef struct _SMBIOS_TABLE_TYPE_ONE {
  SMBIOS_HEADER Header;
  UINT8         ManufacturerStr;
  UINT8         ProductNameStr;
  UINT8         VersionStr;
  UINT8         SerialNumberStr;
  UINT8         Uuid[16];
  UINT8         WakeUp;
} SMBIOS_TABLE_TYPE_ONE;

typedef struct _SMBIOS_TABLE_TYPE_THREE {
  SMBIOS_HEADER Header;
  UINT8         ManufacturerStr;
  UINT8         Type;
  UINT8         VersionStr;
  UINT8         SerialNumberStr;
  UINT8         AssetTagStr;
  UINT8         BootState;
  UINT8         PowerSupplyState;
  UINT8         ThermalState;
  UINT8         SecurityStatus;
  UINT32        VendorInfo;
} SMBIOS_TABLE_TYPE_THREE;

typedef struct _SMBIOS_TABLE_TYPE_FOUR {
  SMBIOS_HEADER Header;
  UINT8         SocketStr;
  UINT8         Type;
  UINT8         Family;
  UINT8         Manufacturer;
  UINT8         ProcId[8];
  UINT8         VersionStr;
  UINT8         Voltage;
  UINT16        ExternalClock;
  UINT16        MaxSpeed;
  UINT16        CurrentSpeed;
  UINT8         Status;
  UINT8         Upgrade;
  UINT16        L1CacheHandle;
  UINT16        L2CacheHandle;
  UINT16        L3CacheHandle;
} SMBIOS_TABLE_TYPE_FOUR;

typedef struct _SMBIOS_TABLE_TYPE_TWENTY_THREE {
  SMBIOS_HEADER Header;
  UINT8         Capabilities;
  UINT16        ResetCount;
  UINT16        ResetLimit;
  UINT16        TimerInterval;
  UINT16        Timeout;
} SMBIOS_TABLE_TYPE_TWENTY_THREE;

typedef struct _ACPI_DESCRIPTOR_HEADER_ {
  UINT32  Signature;
  UINT32  Length;
  UINT8   Revision;
  UINT8   Checksum;
  UINT8   OemId[6];
  UINT8   OemTableId[8];
  UINT32  OemRevision;
  UINT8   CreatorId[4];
  UINT32  CreatorRev;
} ACPI_HEADER;

typedef struct _MEBX_DEBUG_FLAGS_ {
  UINT16  Port80 : 1;   ///< Port 80h
  UINT16  Rsvd : 15;    ///< Reserved
} MEBX_DEBUG_FLAGS;

typedef struct _MEBX_OEM_RESOLUTION_SETTINGS_ {
  UINT16  MebxNonUiTextMode : 4;
  UINT16  MebxUiTextMode : 4;
  UINT16  MebxGraphicsMode : 4;
  UINT16  Rsvd : 4;
} MEBX_OEM_RESOLUTION_SETTINGS;

typedef struct {
  UINT16                        BpfVersion;
  UINT8                         CpuReplacementTimeout;
  UINT8                         Reserved[7];
  UINT8                         ActiveRemoteAssistanceProcess;
  UINT8                         CiraTimeout;
  UINT16                        OemFlags;
  MEBX_DEBUG_FLAGS              MebxDebugFlags;
  UINT32                        MeBiosSyncDataPtr;
  UINT32                        UsbKeyDataStructurePtr;
  MEBX_OEM_RESOLUTION_SETTINGS  OemResolutionSettings;
  UINT8                         Reserved3[0x2E];
} MEBX_BPF;

typedef struct {
  UINT16  VersionInfo;
  UINT8   Reserved[2];
  UINT32  PtrSmbiosTable;
  UINT32  PtrAcpiRsdt;
  UINT16  SupportedTablesFlags;
  UINT32  PciDevAssertInfoPtr;
  UINT32  MediaDevAssetInfoPtr;
} AMT_DATA;

typedef union {
  UINT32  Data32;
  UINT16  Data16[2];
  UINT8   Data8[4];
} DATA32_UNION;

#pragma pack()

/**
  Signal a event for Amt ready to boot.

  @param[in] None

  @retval EFI_SUCCESS             Mebx launched or no controller
**/
EFI_STATUS
EFIAPI
AmtReadyToBoot (
  VOID
  );


/**
  This routine is run at boot time.
  1) Initialize AMT library.
  2) Check if MEBx is required to be launched by user.
  3) Build and send asset tables to ME FW.
  4) Check USB provision.
  5) Hide unused AMT devices in prior to boot.

  @param[in] None.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structure.
  @retval EFI_NOT_FOUND           1.5M FW SKU detected - there is no MEBx on 1.5 FW SKU
**/
EFI_STATUS
MebxNotifyEvent (
  VOID
  );

/**
  Check the status of Amt devices

  @param[in] MeBiosExtensionSetupData   MEBx setting data
**/
VOID
AmtDeviceConfigure (
  IN  ME_BIOS_EXTENSION_SETUP     *MeBiosExtensionSetupData
  );


/**
  Detect EFI MEBx support; Loading and execute.

  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
**/
EFI_STATUS
AdjustAndExecuteMebxImage (
  IN  VOID                        *MebxBiosParamsBuffer
  );

/**
  Detect BIOS invoke reasons.

  @param[in] InvokeMebx           Pointer to the Invoke MEBx flag
  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
**/
VOID
CheckForBiosInvokeReason (
  IN  UINT8       *InvokeMebx,
  IN  MEBX_BPF    *MebxBiosParams
  );

/**
  Dump MEBx BIOS Params

  @param[in]   MebxBiosParams - MEBx BIOS params
**/
VOID
DxeMebxBiosParamsDebugDump (
  IN MEBX_BPF *MebxBiosParams
  );

extern AMT_MEDIA_FRU  mAmtMediaFru;
extern AMT_PCI_FRU    mAmtPciFru;

///
/// USB Provisioning
///
extern VOID           *mUsbProvData;
extern VOID           *mUsbProvDataBackup;
extern UINTN          mUsbProvDataSize;
extern BOOLEAN        mUsbProvsionDone;
extern UINT8          mUsbKeyBus;
extern UINT8          mUsbKeyPort;
extern UINT8          mUsbKeyDevice;
extern UINT8          mUsbKeyFunction;

#endif /// _AMT_PTBX_H_
