/** @file
  PlatformBdsLib

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

/** 
  Head file for BDS Platform specific code

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BDS_PLATFORM_H_
#define _BDS_PLATFORM_H_
//[-start-120626-IB03530375-add]//
#include <Uefi.h>
//[-end-120626-IB03530375-add]//
#include <PiDxe.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Pci.h> 
#include <Uefi/UefiInternalFormRepresentation.h>
//[-start-120626-IB03530375-add]//
#include <MeChipset.h>
//[-end-120626-IB03530375-add]//
#include <Protocol/GenericMemoryTest.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/GenericMemoryTest.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/ConOutDevStarted.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion.h>
#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/OEMBadgingSupport.h>
//[-start-130403-IB09720138-remove]//
// #include <Protocol/ChipsetLibServices.h>
//[-end-130403-IB09720138-remove]//
#include <Protocol/UgaSplash.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/CpuIo.h>
#include <Protocol/SkipScanRemovableDev.h>
//[-start-121130-IB11410025-modify]//
#include <Protocol/PlatformGopPolicy.h>
#include <PlatformGopVbt.h>
//[-end-121130-IB11410025-modify]//
#include <Protocol/CRPolicy.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/SerialIo.h>
#include <Protocol/TerminalEscCode.h>
#include <Protocol/AcpiS3Save.h>
//[-start-120628-IB03530375-modify]//
#include <Protocol/AlertStandardFormat.h>
//[-end-120628-IB03530375-modify]//
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/LoadFile.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/BootLogo.h>
//[-start-120626-IB03530375-add]//
#include <Protocol/MePlatformPolicy.h>
#include <Protocol/SimpleTextIn.h>
//[-end-120626-IB03530375-add]//
#include <Protocol/MeBiosPayloadData.h>
#include <Protocol/MeAlertAt.h>
#include <Protocol/AmtPlatformPolicy.h>


#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/FdTableInfoHob.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/BdsHii.h>


#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PlatformBdsLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/IoLib.h>
//[-start-120626-IB03530375-add]//
#include <Library/Amt/Dxe/AmtLib.h>
#include <Library/DxeServicesLib.h>
//[-end-120626-IB03530375-add]//
#include <PostCode.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <ChipsetSetupConfig.h>


extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformRootBridges [];
extern BDS_CONSOLE_CONNECT_ENTRY gPlatformConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossibleIgfxConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePegConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePcieConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePciConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformConnectSequence [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformDriverOption [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformBootOption [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformConnectSata[];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformConnectLastBoot[];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossibleIgfxConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePegConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePcieConsole [];
extern EFI_DEVICE_PATH_PROTOCOL  *gPlatformAllPossiblePciConsole [];

typedef struct {
  UINTN  Seg;
  UINTN  Bus;
  UINTN  Dev;
  UINTN  Fun;
} PCI_DEV_DEF;

#ifndef PCICMD
#define PCICMD                          (0x4)
#endif

#define gPciRootBridge \
  {\
    ACPI_DEVICE_PATH,\
    ACPI_DP,\
    (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),\
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),\
    EISA_PNP_ID(0x0A03),\
    0\
  }

#define gEndEntire \
  {\
    END_DEVICE_PATH_TYPE,\
    END_ENTIRE_DEVICE_PATH_SUBTYPE,\
    END_DEVICE_PATH_LENGTH,\
    0\
  }

//
// Temporary console variables used in the ConnectConsole
//
#define CON_OUT_CANDIDATE_NAME    L"ConOutCandidateDev"
#define CON_IN_CANDIDATE_NAME     L"ConInCandidateDev"
#define ERR_OUT_CANDIDATE_NAME    L"ErrOutCandidateDev"

//
// Platform Root Bridge
//
typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ROOT_BRIDGE_DEVICE_PATH;

//
// Below is the platform console device path
//
typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      Keyboard;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_KEYBOARD_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      Mouse;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_MOUSE_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           OnboardVga;
  ACPI_ADR_DEVICE_PATH      DisplayDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_VGA_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           AgpBridge;
  PCI_DEVICE_PATH           AgpDevice;
  ACPI_ADR_DEVICE_PATH      DisplayDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_PLUG_IN_VGA_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  SATA_DEVICE_PATH          SataBridge;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_SATA_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      IsaSerial;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ISA_SERIAL_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           SerialOverLAN;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ISA_SERIAL_OVER_LAN_DEVICE_PATH;

//
// Below is the boot option device path
//
typedef struct {
  BBS_BBS_DEVICE_PATH             LegacyHD;
  EFI_DEVICE_PATH_PROTOCOL        End;
} LEGACY_HD_DEVICE_PATH;



typedef enum {
  Igfx  = 0,
  Peg,
  Pcie,
  Pci,
  PossibleVgaTypeMax
} POSSIBLE_VGA_TYPE;

typedef enum {
  DisplayModeIgfx  = 0,
  DisplayModePeg,
  DisplayModePci,
  DisplayModeAuto,
  DisplayModeSg
} PRIMARY_DISPLAY;

typedef struct {
  EFI_HANDLE                   Handle;
  POSSIBLE_VGA_TYPE            VgaType;
  UINT8                        Priority;
} VGA_DEVICE_INFO;

typedef struct {
  UINTN                        VgaHandleConut;
  EFI_HANDLE                   PrimaryVgaHandle;
  EFI_HANDLE                   *VgaHandleBuffer;
} VGA_HANDLES_INFO;

VOID
ChipsetPrepareReadyToBootEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

EFI_STATUS
PlatformBdsNoConsoleAction (
  VOID
  );

EFI_STATUS
PlatformBdsBootDisplayDevice (
  IN CHIPSET_CONFIGURATION        *SetupNVRam
  );

EFI_STATUS
PlatformBdsBootSelection (
  UINT16                                    Selection,
  UINT16                                    Timeout
  );

EFI_STATUS
AutoFailoverPolicyBehavior (
  IN BOOLEAN    AutoFailover,
  IN CHAR16     *Message
  );

VOID
PlatformBdsPredictBootOption (
  IN OUT LIST_ENTRY              *BdsBootOptionList
  );

VOID
PlatformBdsNoBootDevice (
  VOID
  );

EFI_STATUS
PlatformBdsConnectDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL    *HandleDevicePath,
  IN  BOOLEAN                     ConnectChildHandle,
  IN  BOOLEAN                     DispatchPossibleChild
  );

EFI_STATUS
UpdateLastBootDevicePath (
  IN   UINT16                     *BootPrevious,
  OUT  EFI_DEVICE_PATH_PROTOCOL   **LastBootDevicePath
  );

EFI_STATUS
PlatformBdsConnectConsoleMini (
  IN  BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  );

EFI_STATUS
ExitBootServiceSetVgaMode (
  IN CHIPSET_CONFIGURATION                   *SetupVariable,
  IN EFI_BOOT_MODE                          BootMode
  ); 

VOID
ExitBootServiceSetVgaModeCallback (
  EFI_EVENT  Event,
  VOID       *Context
  );

#ifdef ANTI_THEFT_SUPPORT
VOID
PlatformAlertAT (
  VOID
  );
#endif

//[-start-130204-IB10820231-remove]//
//#ifdef ME_5MB_SUPPORT
//[-end-130204-IB10820231-remove]//
VOID
InvokeMebxHotKey (
  VOID
  );

VOID
InvokeRemoteAsstHotKey (
  VOID
  );
//[-start-130204-IB10820231-remove]//
//#endif
//[-end-130204-IB10820231-remove]//

EFI_STATUS
EFIAPI
SetVgaCommandRegReadyToBootEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

//[-start-121029-IB04770247-add]//
UINT8
IsvtCall (
  IN UINT8 CheckPoint
  );
//[-end-121029-IB04770247-add]//

//[-start-130516-12360009-add]//
#ifdef TXT_SUPPORT
UINT8 *
FindCompatibility16Table (
  VOID
  );

STATIC
EFI_STATUS
AcpiPlatformChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  );

EFI_STATUS
PrepareAcpiTableforTXT (
  VOID
  );
#endif
//[-end-130516-12360009-add]//

BOOLEAN
FoundUSBConsoleOutput (
  OUT EFI_DEVICE_PATH_PROTOCOL  **UsbConsoleOuputDevPath
  );

VOID
SetUsbConsoleOutToConOutVar (
  IN EFI_DEVICE_PATH_PROTOCOL  *UsbConsoleOuputDevPath
  ); 

EFI_STATUS
BdsMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  * Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  * Single,
  IN  EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath OPTIONAL,
  IN  BOOLEAN                   Delete
  );

//[-start-140512-IB10300110-add]//
VOID
WatchDogReadyToBootEvent (
  VOID
  );
//[-end-140512-IB10300110-add]//

#endif
