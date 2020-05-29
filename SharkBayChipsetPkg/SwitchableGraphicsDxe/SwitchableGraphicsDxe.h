/** @file
  This header file is for Switchable Graphics Feature DXE driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SWITCHABLE_GRAPHICS_DXE_H_
#define _SWITCHABLE_GRAPHICS_DXE_H_

#include <Guid/HobList.h>
#include <Guid/SgInfoHob.h>
#include <Guid/SwitchableGraphicsVariable.h>
#include <IndustryStandard/Acpi.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CommonPciLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciExpressLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PchRegs.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion.h>
#include <Protocol/PciIo.h>
#include <Protocol/SwitchableGraphicsEvent.h>
#include <Protocol/SwitchableGraphicsInfo.h>
#include <SaAccess.h>
#include <SwitchableGraphicsDefine.h>

#define MemoryRead16(Address)                 *(UINT16 *)(Address)
#define MemoryRead8(Address)                  *(UINT8 *)(Address)

#pragma pack (push, 1)
typedef struct _SG_DXE_INFORMATION_DATA {
  BINARY_INFORMATION                          Vbios;
  BINARY_INFORMATION                          MasterMxmBinFile;
  BINARY_INFORMATION                          SlaveMxmBinFile;
  UINT16                                      GpioBaseAddress;
  UINT8                                       MasterDgpuBus;
  UINT8                                       SlaveDgpuBus;
  UINT16                                      MasterDgpuVendorId;
  UINT16                                      SlaveDgpuVendorId;
  UINT8                                       SaDeven;
  UINT8                                       OptimusFlag;
  UINT8                                       AmdSecondaryGrcphicsCommandRegister;
  UINT8                                       NvidiaSecondaryGrcphicsCommandRegister;
  BOOLEAN                                     SlaveMxmGpuSupport;
} SG_DXE_INFORMATION_DATA;
#pragma pack (pop)

#pragma pack (push, 1)
//
// This structure must match with the structure in AmdSsdt.asl and NvidiaSsdt.asl SG OpRegion "SGOP"
//
typedef struct {
  //
  // PEG Endpoint Info
  //
  UINT32      XPcieCfgBaseAddress;          // Any Device's PCIe Config Space Base Address
  UINT32      EndpointBaseAddress;          // PEG Endpoint PCIe Base Address
  UINT32      EndpointPcieCapBaseAddress;   // PEG Endpoint PCIe Capability Structure Base Address
  UINT32      DgpuPcieCfgBaseAddress;       // dGPU Device's PCIe Config Space Base Address
  //
  // HG Board Info
  //
  UINT8       SgGPIOSupport;                // SG GPIO Support
  UINT8       SgMode;                       // SG Mode (0=Disabled, 1=SG Muxed, 2=SG Muxless, 3=DGPU Only)
  UINT8       SgDgpuPwrOK;                  // dGPU PWROK GPIO assigned
  UINT8       SgDgpuHoldRst;                // dGPU HLD RST GPIO assigned
  UINT8       SgDgpuDisplaySel;             // dGPU Display Select GPIO assigned
  UINT8       SgDgpuEdidSel;                // dGPU EDID Select GPIO assigned
  UINT8       SgDgpuPwmSel;                 // dGPU PWM Select GPIO assigned
  UINT8       SgDgpuPwrEnable;              // dGPU PWR Enable GPIO assigned
  UINT8       SgDgpuPrsnt;                  // dGPU Present Detect GPIO assigned
  UINT16      GpioBaseAddress;              // GPIO Base Address
  //
  // GPIO Delay Time
  //
  UINT32      ActiveDgpuPwrEnableDelayTime; // Active dGPU_PWR_EN Delay time
  UINT32      ActiveDgpuHoldRstDelayTime;   // Active dGPU_HOLD_RST_PIN Delay time
  UINT32      InctiveDgpuHoldRstDelayTime;  // Inactive dGPU_HOLD_RST_PIN Delay time
  UINT8       IsUltBoard;
} OPERATION_REGION_SG;
#pragma pack (pop)

#pragma pack (push, 1)
//
// This structure must match with the structure in AmdSsdt.asl AMD OpRegion "AOPR"
//
typedef struct {
  UINT8       SlaveDgpuSupport;             // Slave dGPU Support or not
  CHAR8       ExpansionMode;                // Expansion Mode
  UINT8       PxDynamicSupportState;        // Dynamic Support State 1: dynamic, 0: Non-dynamic
  UINT8       PxFixedSupportState;          // Fixed Support State 1: Fixed,   0: Non-fixed
  UINT8       PxFullDgpuPowerOffDynamic;    // Full dGPU PowerOff Dynamic mode
} OPERATION_REGION_AMD;
#pragma pack (pop)

#pragma pack (push, 1)
//
// This structure must match with the structure in NvidiaSsdt.asl NVIDIA OpRegion "NOPR"
//
typedef struct {
  UINT8       DgpuHotPlugSupport;           // Optimus dGPU HotPlug Support or not
  UINT8       DgpuPowerControlSupport;      // Optimus dGPU Power Control Support or not
  UINT8       GpsFeatureSupport;            // GPS Feature Support or not
  UINT8       VenturaFeatureSupport;        // Ventura Feature Support or not
  UINT8       OptimusGc6Support;            // Optimus GC6 Feature Support or not
  UINT8       SlaveDgpuSupport;             // Slave dGPU Support or not
  UINT8       SpbConfig;                    // SpbConfig
  UINT8       ExpansionMode;                // Expansion Mode
  UINT32      MxmBinarySize;                // MXM bin file Size (bits)
  CHAR8       MxmBinaryBuffer[0x1000];      // MXM Bin file 4K
  UINT32      SlaveMxmBinarySize;           // Slave MXM bin file Size (bits)
  CHAR8       SlaveMxmBinaryBuffer[0x1000]; // Slave MXM Bin file 4K
} OPERATION_REGION_NVIDIA;
#pragma pack ()

#pragma pack (push, 1)
//
// This structure must match with the structure in NvidiaSsdt.asl NVIDIA OpRegion "NOPR"
//
typedef struct {
  UINT32      RVBS;                         // Runtime VBIOS Image Size
  CHAR8       VBOIS[0x20000];               // VBIOS 128KB
} OPERATION_REGION_VBIOS;
#pragma pack ()

#endif
