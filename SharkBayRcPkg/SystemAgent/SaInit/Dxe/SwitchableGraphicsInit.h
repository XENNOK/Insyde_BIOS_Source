/** @file
  Header file for the SwitchableGraphics Dxe driver.
  This driver loads SwitchableGraphics ACPI tables.

@copyright
  Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
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

#ifndef _SWITCHABLE_GRAPHICS_DXE_H_
#define _SWITCHABLE_GRAPHICS_DXE_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
//f !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
//#include "EdkIIGlueDxe.h"
#include <Uefi.h>
//-#include <EfiScriptLib.h>
////#endif

#include <PchAccess.h>
#include <IndustryStandard/Acpi30.h>
#include <SaAccess.h>
#include <CpuRegs.h>

#include <Protocol/PciIo.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Guid/SaDataHob.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Library/CpuPlatformLib.h>

//[-start-140218-IB10920078-add]//
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/LegacyBios.h>

#include <Pi/PiS3BootScript.h>
#include <PchRegs.h>
#include <Guid/HobList.h>

#include <Guid/NvidiaAcpiTablePchGuid.h>
#include <Guid/AmdAcpiTablePchGuid.h>

//[-end-140218-IB10920078-add]//
///
/// SG ACPI table data storage file
///
#include <Guid/SgAcpiTableStorage.h>

///
/// Switchable Graphics defines.
///
#define CONVENTIONAL_MEMORY_TOP 0xA0000 ///< 640 KB
#define BIN_FILE_SIZE_MAX       0x10000

#define OPTION_ROM_SIGNATURE    0xAA55

#define MemoryRead16(Address)   * (UINT16 *) (Address)
#define MemoryRead8(Address)    * (UINT8 *) (Address)

//[-start-140218-IB10920078-add]//
#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#endif
#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif
#ifndef EFI_SIGNATURE_64
#define EFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (EFI_SIGNATURE_32 (A, B, C, D) | ((UINT64) (EFI_SIGNATURE_32 (E, F, G, H)) << 32))
#endif
//[-end-140218-IB10920078-add]//

///
/// PEG Capability Equates
///
#define PEG_CAP_ID  0x10
#define PEG_CAP_VER 0x2

#pragma pack(1)
typedef struct {
  UINT16  Signature;  ///< 0xAA55
  UINT8   Reserved[22];
  UINT16  PcirOffset;
} VBIOS_OPTION_ROM_HEADER;
#pragma pack()

#pragma pack(1)
typedef struct {
  UINT32  Signature;  ///< "PCIR"
  UINT16  VendorId;
  UINT16  DeviceId;
  UINT16  Reserved0;
  UINT16  Length;
  UINT8   Revision;
  UINT8   ClassCode[3];
  UINT16  ImageLength;
  UINT16  CodeRevision;
  UINT8   CodeType;
  UINT8   Indicator;
  UINT16  Reserved1;
} VBIOS_PCIR_STRUCTURE;
#pragma pack()

/**
  Initialize the SwitchableGraphics support.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS         - SwitchableGraphics initialization complete
  @retval EFI_OUT_OF_RESOURCES - Unable to allocated memory
**/
EFI_STATUS
SwitchableGraphicsInit (
  IN EFI_HANDLE                      ImageHandle,
  IN EFI_SYSTEM_TABLE                *SystemTable,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

/**
  Load and execute the dGPU VBIOS.

  @param[in] VbiosData - Pointer to VbiosData policy for Load/Execute and VBIOS Source.
      LoadVbios    - 0 = Do Not Load   ; 1 = Load VBIOS
      ExecuteVbios - 0 = Do Not Execute; 1 = Execute VBIOS
      VbiosSource  - 0 = PCIE Device   ; 1 = FirmwareVolume => TBD

  @retval EFI_SUCCESS     - Load and execute successful.
  @exception EFI_UNSUPPORTED - Secondary VBIOS not loaded.
**/
EFI_STATUS
LoadAndExecuteDgpuVbios (
  IN SA_SG_VBIOS_CONFIGURATION  *VbiosConfig
  )
;

/**
  Initialize the runtime SwitchableGraphics support data for ACPI tables in GlobalNvs.
  @param[in] SgInfoDataHob   - Pointer to Hob for SG system details.
  @param[in] DxePlatformSgPolicy - Pointer to the loaded image protocol for this driver.

  @retval EFI_SUCCESS - The data updated successfully.
**/
EFI_STATUS
UpdateGlobalNvsData (
  IN SG_INFO_HOB                     SgInfo,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

/**
  Do an AllocatePages () of type AllocateMaxAddress for EfiBootServicesCode
  memory.

  @param[in] AllocateType     - Allocated Legacy Memory Type
  @param[in] StartPageAddress - Start address of range
  @param[in] Pages            - Number of pages to allocate
  @param[in, out] Result      - Result of allocation

  @retval EFI_SUCCESS - Legacy16 code loaded
  @retval Other       - No protocol installed, unload driver.
**/
EFI_STATUS
AllocateLegacyMemory (
  IN  EFI_ALLOCATE_TYPE         AllocateType,
  IN  EFI_PHYSICAL_ADDRESS      StartPageAddress,
  IN  UINTN                     Pages,
  IN OUT  EFI_PHYSICAL_ADDRESS  *Result
  )
;

/**
  Search and return the offset of desired Pci Express Capability ID
    CAPID list:
      0x0001 = Advanced Error Rreporting Capability
      0x0002 = Virtual Channel Capability
      0x0003 = Device Serial Number Capability
      0x0004 = Power Budgeting Capability

    @param[in] Bus       -   Pci Bus Number
    @param[in] Device    -   Pci Device Number
    @param[in] Function  -   Pci Function Number
    @param[in] CapId     -   Extended CAPID to search for

    @retval 0       - CAPID not found
    @retval Other   - CAPID found, Offset of desired CAPID
**/
UINT32
PcieFindExtendedCapId (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function,
  IN UINT16 CapId
  )
;

/**
  Find the Offset to a given Capabilities ID
    CAPID list:
      0x01 = PCI Power Management Interface
      0x04 = Slot Identification
      0x05 = MSI Capability
      0x10 = PCI Express Capability

    @param[in] Bus       -   Pci Bus Number
    @param[in] Device    -   Pci Device Number
    @param[in] Function  -   Pci Function Number
    @param[in] CapId     -   CAPID to search for

    @retval 0       - CAPID not found
    @retval Other   - CAPID found, Offset of desired CAPID
**/
UINT32
PcieFindCapId (
  IN UINT8 Bus,
  IN UINT8 Device,
  IN UINT8 Function,
  IN UINT8 CapId
  )
;

/**
  Read SG GPIO value

  @param[in] Value - PCH GPIO number and Active value
      Bit0 to Bit7    - PCH GPIO Number
      Bit8            - GPIO Active value (0 = Active Low; 1 = Active High)

  @retval GPIO read value.
**/
BOOLEAN
GpioRead (
  IN UINT8 Value
  )
;

/**
  Write SG GPIO value

  @param[in] Value - PCH GPIO number and Active value
      Bit0 to Bit7    - PCH GPIO Number
      Bit8            - GPIO Active value (0 = Active Low; 1 = Active High)
  @param[in] Level - Write data (0 = Disable; 1 = Enable)
**/
VOID
GpioWrite (
  IN UINT8   Value,
  IN BOOLEAN Level
  )
;

/**
  Load Intel SG SSDT Tables

  @param[in] None

  @retval EFI_SUCCESS - SG SSDT Table load successful.
**/
EFI_STATUS
LoadAcpiTables (
  VOID
  )
;


VOID
EFIAPI
SgExitPmAuthCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

//[-start-140218-IB10920078-add]//
#define NVIDIA_VID      0x10DE

#define AMD_SVID_OFFSET 0x4C
#define AMD_SDID_OFFSET 0x4E
#define AMD_VID         0x1002

///
/// OpRegion Header #defines.
///
#define NVIG_HEADER_SIGNATURE "NVSG-IGD-DSM-VAR"
#define NVIG_OPREGION_SIZE    1
#define NVIG_OPREGION_VER     0x00000101
#define NVHM_HEADER_SIGNATURE "NVSwitchableGraphicss"
#define NVHM_OPREGION_SIZE    65
#define NVHM_OPREGION_VER     0x00000101
#define APXM_HEADER_SIGNATURE "AMD--PowerXpress"
#define APXM_OPREGION_SIZE    65
#define APXM_OPREGION_VER     0x00000101

///
/// OpRegion structures:
///
/// Note: These structures are packed to 1 byte offsets because the exact
/// data location is requred by the supporting design specification due to
/// the fact that the data is used by ASL and Graphics driver code compiled
/// separatly.
///
///
/// NVIG OpRegion
///
#pragma pack(1)
typedef struct {
  ///
  /// OpRegion Header             ///< Byte offset(decimal)
  ///
  CHAR8   NISG[0x10]; ///< 0            NVIG OpRegion Signature
  UINT32  NISZ;       ///< 16           NVIG OpRegion Size in KB
  UINT32  NIVR;       ///< 20           NVIG OpRegion Version
  /// OpRegion Data
  ///
  UINT32  GPSS;   ///< 24           Policy Selection Switch Status (Current GPU)
  UINT16  GACD;   ///< 32           Active Displays
  UINT16  GATD;   ///< 34           Attached Displays
  CHAR8   LDES;   ///< 36           Lid Event State
  CHAR8   DKST;   ///< 37           Dock State
  CHAR8   DACE;   ///< 38           Display ACPI Event
  CHAR8   DHPE;   ///< 39           Display Hot-Plug Event
  CHAR8   DHPS;   ///< 40           Display Hot-Plug Status
  CHAR8   SGNC;   ///< 41           Notify Code (Cause of Notify(..,0xD0))
  CHAR8   GPPO;   ///< 42           Policy Override
  CHAR8   USPM;   ///< 43           Update Scaling Preference Mask
  CHAR8   GPSP;   ///< 44           Panel Scaling Preference
  CHAR8   TLSN;   ///< 45           Toggle List Sequence Number
  CHAR8   DOSF;   ///< 46           Flag for _DOS
  UINT16  ELCT;   ///< 47           Endpoint Link Contol Register Value
} NVIG_OPREGION;  ///< Total 49  Bytes
#pragma pack()
///
/// NVHM OpRegion
///
#pragma pack(1)
typedef struct {
  ///
  /// OpRegion Header             ///< Byte offset(decimal)
  ///
  CHAR8   NVSG[0x10]; ///< 0            NVHM OpRegion Signature
  UINT32  NVSZ;       ///< 16           NVHM OpRegion Size in KB
  UINT32  NVVR;       ///< 20           NVHM OpRegion Version
  /// OpRegion Data
  ///
  UINT32  NVHO;           ///< 24           NVHM Opregion Address
  UINT32  RVBS;           ///< 28           Nvidia VBIOS Image Size
  CHAR8   RBUF[0x10000];  ///< 32           64KB VBIOS
} NVHM_OPREGION;          ///< Total 65568 Bytes
#pragma pack()
///
/// Entire Nvidia OpRegion
///
#pragma pack(1)
typedef struct {
  NVIG_OPREGION *NvIgOpRegion;  ///< 47  Bytes
  NVHM_OPREGION *NvHmOpRegion;  ///< 65568 Bytes
} NVIDIA_OPREGION;              ///< Total 65615 Bytes
#pragma pack()
///
/// APXM OpRegion
///
#pragma pack(1)
typedef struct {
  ///
  /// OpRegion Header        ///< Byte offset(decimal)
  ///
  CHAR8   APSG[0x10]; ///< 0            APXM OpRegion Signature
  UINT32  APSZ;       ///< 16           APXM OpRegion Size in KB
  UINT32  APVR;       ///< 20           APXM OpRegion Version
  /// OpRegion Data
  ///
  UINT32  APXA;           ///< 24           PX OpRegion Address
  UINT32  RVBS;           ///< 28           PX Runtime VBIOS Image Size
  UINT16  NTLE;           ///< 32           Total number of toggle list entries
  UINT16  TLEX[15];       ///< 34           The display combinations in the list...
  UINT16  TGXA;           ///< 64           Target GFX adapter as notified by ATPX function 5
  UINT16  AGXA;           ///< 66           Active GFX adapter as notified by ATPX function 6
  CHAR8   GSTP;           ///< 68           GPU switch transition in progress
  CHAR8   DSWR;           ///< 69           Display Switch Request
  CHAR8   EMDR;           ///< 70           Expansion Mode Change Request
  CHAR8   PXGS;           ///< 71           PowerXpress graphics switch toggle request
  UINT16  CACD;           ///< 72           Currently Active Displays
  UINT16  CCND;           ///< 74           Currently Connected Displays
  UINT16  NACD;           ///< 76           Next Active Index
  CHAR8   EXPM;           ///< 78           Expansion Mode
  UINT16  TLSN;           ///< 79           Toggle list sequence index
  UINT16  ELCT;           ///< 81           Endpoint Link Contol Register Value
  CHAR8   RBUF[0x10000];  ///< 83          VBIOS 64KB
//[-start-140220-IB10920078-add]//
  UINT8   SlaveDgpuSupport;             // Slave dGPU Support or not
  UINT8   PxDynamicSupportState;        // Dynamic Support State 1: dynamic, 0: Non-dynamic
  UINT8   PxFixedSupportState;          // Fixed Support State 1: Fixed,   0: Non-fixed
  UINT8   PxFullDgpuPowerOffDynamic;    // Full dGPU PowerOff Dynamic mode    
//[-end-140220-IB10920078-add]//    
} APXM_OPREGION;          ///< Total 65626   Bytes
#pragma pack()
///
/// Entire AMD OpRegion
///
#pragma pack(1)
typedef struct {
  APXM_OPREGION *ApXmOpRegion;  ///< Total 65617   Bytes
} AMD_OPREGION;
#pragma pack()

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
} OPERATION_REGION_SG;
#pragma pack (pop)

/**
  Load Third part graphics vendor support SSDT Tables

  @param[in] None

  @retval EFI_SUCCESS        - SSDT Table load successful.
  @exception EFI_UNSUPPORTED - Supported SSDT not found.
**/
EFI_STATUS
LoadTpvAcpiTables (
  VOID
  );

EFI_STATUS
InitializeOpRegion (
  IN EFI_ACPI_DESCRIPTION_HEADER *NvStoreTable
  );

EFI_STATUS
UpdateSgOpRegion (
  VOID
  );

/**
  Nvidia Graphics OpRegion installation function.

  @param[in] None

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                  installation of the ACPI tables.
**/
EFI_STATUS
InstallNvidiaOpRegion (
  VOID
  );

/**
  AMD graphics OpRegion installation function.

  @param[in] None

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                  installation of the ACPI tables.
**/
EFI_STATUS
InstallAmdOpRegion (
  VOID
  );
//[-end-140218-IB10920078-add]//

#endif
