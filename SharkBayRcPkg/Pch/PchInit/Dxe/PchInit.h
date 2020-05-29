/** @file
  Header file for PCH Initialization Driver.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#ifndef _PCH_INITIALIZATION_DRIVER_H_
#define _PCH_INITIALIZATION_DRIVER_H_

///
/// Driver Consumed Protocol Prototypes
///
#include <EfiScriptLib.h>
#include <Guid/ChipsetInitHob.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/PchS3Support.h>
#include <Protocol/PchInfo.h>
//[-start-111222-IB05300263-add]//
#include <Protocol/GlobalNvsArea.h>
//[-end-111222-IB05300263-add]//

//[-start-121112-IB10920000-add]//
#include <Protocol/S3BootScriptDispatch.h>
//[-start-121112-IB10920000-end]//
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/PciIo.h>

#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/PchPciExpressHelpersLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/RcFviDxeLib.h>
#include <Library/PchAslUpdateLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PchUsbCommon.h>
#include <PchHSIO.h>
#include <IndustryStandard/Pci22.h>
#include <IobpDefinitions.h>
#include <PchInitVar.h>



#define AZALIA_MAX_LOOP_TIME         10
#define AZALIA_WAIT_PERIOD           100
#define AZALIA_MAX_SID_NUMBER_PCH_H  4
#define AZALIA_MAX_SID_NUMBER_PCH_LP 2
#define AZALIA_MAX_SID_MASK_PCH_H    ((1 << AZALIA_MAX_SID_NUMBER_PCH_H) - 1)
#define AZALIA_MAX_SID_MASK_PCH_LP   ((1 << AZALIA_MAX_SID_NUMBER_PCH_LP) - 1)
//
// CPUID and MSR definitions
//
#define CPUID_VERSION_INFO                    0x1
#define CPUID_FULL_FAMILY_MODEL               0x0FFF0FF0
#define CPUID_FULL_STEPPING                   0x0000000F
#define CPUID_FULL_FAMILY_MODEL_HASWELL       0x000306C0
#define CPUID_FULL_FAMILY_MODEL_HASWELL_ULT   0x00040650
#define CPUID_FULL_FAMILY_MODEL_CRYSTALWELL   0x00040660
#define MSR_TEMPERATURE_TARGET                0x000001A2

typedef enum {
  //
  // Haswell Family Stepping
  //
  EnumHswA0         = 1,
  EnumHswB0,
  EnumHswC0,
  EnumHswD0,
  //
  // Haswell ULT Family Stepping
  //
  EnumHswUltB0      = 0,
  EnumHswUltC0,
  //
  // Crystalwell Family Stepping
  //
  EnumCrwB0         = 0,
  EnumCrwC0,
  EnumCrwD0,
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;

#pragma pack(1)
typedef union _MSR_REGISTER {
  UINT64  Qword;

  struct _DWORDS {
    UINT32  Low;
    UINT32  High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;

typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;
#pragma pack()

typedef struct {
  EFI_PCH_INFO_PROTOCOL PchInfo;
} PCH_INSTANCE_PRIVATE_DATA;

#define IS_PCH_H_EHCI(DeviceNumber, FunctionNumber) \
    ( \
      (DeviceNumber == PCI_DEVICE_NUMBER_PCH_USB && FunctionNumber == PCI_FUNCTION_NUMBER_PCH_EHCI) || \
      (DeviceNumber == PCI_DEVICE_NUMBER_PCH_USB_EXT && FunctionNumber == PCI_FUNCTION_NUMBER_PCH_EHCI2) \
    )

#define IS_PCH_LP_EHCI(DeviceNumber, FunctionNumber) \
    ( \
      (DeviceNumber == PCI_DEVICE_NUMBER_PCH_USB && FunctionNumber == PCI_FUNCTION_NUMBER_PCH_EHCI) \
    )

//
// Data definitions
//
extern EFI_HANDLE mImageHandle;

///
/// SVID / SID init table entry
///
typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
} PCH_SVID_SID_INIT_ENTRY;

#define PCH_FVI_STRING            "Reference Code - PCH - Lynxpoint"
#define PCH_FVI_SMBIOS_TYPE       0xDD
#define PCH_FVI_SMBIOS_INSTANCE   0x04
#define PCH_CRID_STATUS           "PCH-CRID Status"
#define PCH_CRID_ORIGINAL_VALUE   "PCH-CRID Original Value"
#define PCH_CRID_NEW_VALUE        "PCH-CRID New Value"
#define PCH_CRID_ENABLED          "Enabled "
#define PCH_CRID_DISABLED         "Disabled"
#define PCH_LPTLPBX_HSIO_STRING "LPTLp Bx Hsio Version"
#define PCH_LPTHB0_HSIO_STRING  "LPTH B0 Hsio Version"
#define PCH_LPTHCX_HSIO_STRING  "LPTH Cx Hsio Version"
#define PCH_CRID_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#define RAID_FVI_STRING         "OPROM - RST - RAID"
#define RAID_RC_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

enum {
  RC_VER        = 0,
  CRID_STATUS,
  CRID_ORIGINAL,
  CRID_NEW,
  RAID_VER,
  HSIO_LPTLPAX_VER,
  HSIO_LPTLPBX_VER,
  HSIO_LPTHB0_VER,
  HSIO_LPTHCX_VER
} PCH_FVI_INDEX;

extern FVI_ELEMENT_AND_FUNCTION       mPchFviElementsData[];
extern FVI_DATA_HUB_CALLBACK_CONTEXT  mPchFviVersionData;
extern UINTN                          mPchFviElements;

//
// Function Prototype
//

/**
  Configures PCH IOBP and stores this configuration in S3 boot script

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register
**/
VOID
ProgramIobpWithScript (
  IN UINT32  RootComplexBar,
  IN UINT32  Address,
  IN UINT32  AndMask,
  IN UINT32  OrMask
  );

/**
  Configures 32-bit MMIO register and stores this configuration in S3 boot script

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register
**/
VOID
MmioAndThenOr32WithScript (
  IN UINTN   Address,
  IN UINT32  AndMask,
  IN UINT32  OrMask
  );

/**
  Detect and initialize the type of codec (AC'97 and HDA) present in the system.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] AzaliaEnable         Returned with TRUE if Azalia High Definition Audio codec
                                  is detected and initialized.

  @retval EFI_SUCCESS            Codec is detected and initialized.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources to initialize the codec.
**/
EFI_STATUS
ConfigureAzalia (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  BOOLEAN                             *AzaliaEnable
  );

/**
  Configure miscellaneous power management settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] GpioBase             GPIO base address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureMiscPm (
  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT32                              RootComplexBar,
  UINT16                              GpioBase
  );

/**
  Configure additional power management settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureAdditionalPm (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  );

/**
  Configure deep Sx programming

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ProgramDeepSx (
  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT32                              RootComplexBar
  );

/**
  Perform miscellany PCH initialization

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  The value of Function disable register

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureMiscItems (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg
  );

/**
  Initialize LAN device. Reference: PCH BIOS Spec Rev 0.5.0,
  10.2 Enabling / Disabling the Internal GbE Controller
  ** NOTE:
    - The platform reset mandated by GbE enabling / disabling is handled
  in PchInit PEIM. Platform PEI code is responsible for calling PCH Init PPI
    - (BUC register setting is also done in the PCH Init PPI)

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureLan (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  );

/**
  Configures PCH USB controller

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  Function Disable Register

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureUsb (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg
  );

/**
  Configures PCH Sata Controller

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  Function Disable Register
  @param[in] GpioBase             GPIO base address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureSata (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg,
  IN      UINT16                              GpioBase
  );

/**
  Perform Clock Gating programming
  Enables clock gating in various PCH interfaces and the registers must be restored during S3 resume.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] FuncDisableReg       The Function Disable Register

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureClockGating (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar,
  IN  UINT32                              FuncDisableReg
  );

/**
  Configure IoApic Controler

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureIoApic (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  );

/**
  Configure PCH Display

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureDisplay (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  );

/**
  Perform Root Port Initialization.

  @param[in] RootPort             The root port to be initialized (zero based)
  @param[in] RootPortFunction     The PCI function number of the root port
  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol
  @param[in] PmBase               The PM I/O Base address of the PCH
  @param[in] RootComplexBar       RCBA of the PCH

  @retval EFI_SUCCESS             Device found. The root port must be enabled.
  @retval EFI_NOT_FOUND           No device is found on the root port. It may be disabled.
  @exception EFI_UNSUPPORTED      Unsupported operation.
**/
EFI_STATUS
PchInitSingleRootPort (
  IN  UINT8                                     RootPort,
  IN  UINT8                                     RootPortFunction,
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL          *PchPlatformPolicy,
  IN  UINT16                                    PmBase,
  IN  UINT32                                    RootComplexBar
  );

/**
  Perform Initialization of the Downstream Root Ports.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol
  @param[in] RootComplexBar       RCBA of the PCH
  @param[in] PmBase               The PM I/O Base address of the PCH
  @param[in, out] FuncDisableReg  The function disable register. IN / OUT parameter.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   The PCIe Root Port Number of D28:F0 is not found
                                  or invalid
**/
EFI_STATUS
PchInitRootPorts (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL      *PchPlatformPolicy,
  IN      UINT32                                RootComplexBar,
  IN      UINT16                                PmBase,
  IN OUT  UINT32                                *FuncDisableReg
  );

/**
  This is the function to enable the clock gating for PCI Express ports.

  @param[in] BusNumber            The Bus Number of the PCH device
  @param[in] PchPlatformPolicy    PCH Platform Policy protocol
  @param[in] RpEnableMask         Bit Mask indicating the enabled root ports
  @param[in] RpHiddenMask         Bit Mask indicating the root ports used for other > x1 root ports
  @param[in] RootComplexBar       Root complex base address

  @retval EFI_SUCCESS             Successfully completed.
**/
EFI_STATUS
PcieEnableClockGating (
  IN  UINT8                                     BusNumber,
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL          *PchPlatformPolicy,
  IN  UINT32                                    RpEnableMask,
  IN  UINT32                                    RpHiddenMask,
  IN  UINT32                                    RootComplexBar,
  IN  UINT32                                    NandPort
  );

/**
  Set an Init Root Port Downstream devices S3 dispatch item, this function may assert if any error happend

  @param[in] RootPortBus          Pci Bus Number of the root port
  @param[in] RootPortDevice       Pci Device Number of the root port
  @param[in] RootPortFunc         Pci Function Number of the root port
  @param[in] TempBusNumberMin     Minimal temp bus number that can be assigned to the root port (as secondary
                                  bus number) and its down stream switches
  @param[in] TempBusNumberMax     Maximal temp bus number that can be assigned to the root port (as subordinate
                                  bus number) and its down stream switches

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SetInitRootPortDownstreamS3Item (
  IN  UINT8                           RootPortBus,
  IN  UINT8                           RootPortDevice,
  IN  UINT8                           RootPortFunc,
  IN  UINT8                           TempBusNumberMin,
  IN  UINT8                           TempBusNumberMax
  );

/**
  Set an PCH IOBP programming S3 dispatch item, this function may assert if any error happend

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Out of resources
  @retval EFI_INVALID_PARAMETER   Invalid parameter
  @retval EFI_NOT_FOUND           Protocol interface not found
**/
EFI_STATUS
SetProgramIobpS3Item (
  IN UINT32               RootComplexBar,
  IN UINT32               Address,
  IN UINT32               AndMask,
  IN UINT32               OrMask
  );

/**
  Locking Thermal Reporting Settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] GpioBase             GPIO base address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ThermalLockDown (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT16                                  GpioBase
  );

/**
  Configures PCH DMI according to policies specified in PCH Platform Policy protocol

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             Successfully completed.
**/
EFI_STATUS
EFIAPI
ConfigureDmiPm (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  );

/**
  Dump whole DXE_PCH_PLATFORM_POLICY_PROTOCOL and serial out.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval None
**/
VOID
PchDumpPlatformProtocol (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy
  );

/**
  Lock USB registers before boot

  @param[in] PchPlatformPolicy    The PCH Platform Policy

  @retval None
**/
VOID
UsbInitBeforeBoot (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy
  );

/**
  Configures ports of the PCH USB3 (xHCI) controller
  just before OS boot.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
VOID
ConfigureXhciAtBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
  );

#ifdef SERIAL_IO_FLAG
/**
  Puts Serial IO controllers in D3

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
**/
VOID
ConfigureSerialIoAtBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
  );

/**
  Hide PCI config space of Serial IO Controllerss and do any
  final initialization.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureSerialIoBeforeBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  );

/**
  Configures Serial IO Controllers

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
EFI_STATUS
ConfigureSerialIo (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  );
#endif // SERIAL_IO_FLAG

#ifdef ADSP_FLAG
/**
  Initialize Audio DSP subsystem

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  The value of Function disable register

  @retval EFI_SUCCESS             Codec is detected and initialized
  @retval EFI_UNSUPPORTED         Audio DSP disabled
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate resources to initialize the codec
**/
EFI_STATUS
ConfigureAudioDsp (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL  *PchPlatformPolicy,
  IN      UINT32                            RootComplexBar,
  IN OUT  UINT32                            *FuncDisableReg
  );

/**
  Finalize Audio DSP initialization after PCI enumeration.
  In particular configure ADSP in ACPI or PCI mode.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Audio DSP not found or not enabled
**/
EFI_STATUS
ConfigureAudioDspBeforeBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL       *PchPlatformPolicy,
  IN UINT32                                 RootComplexBar
  );

#endif // ADSP_FLAG
#endif
