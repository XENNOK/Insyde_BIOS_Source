/** @file

  WheaPlatform driver lib functions.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <WheaErrorLib.h>

#include <IndustryStandard/Pci.h>
#include <Library/PciExpressLib.h>
#include <Guid/Cper.h>

//
// PCI CONFIGURATION MAP REGISTER OFFSETS
//
#ifndef PCI_VID
#define PCI_VID             0x0000  /// Vendor ID Register
#define PCI_DID             0x0002  /// Device ID Register
#define PCI_CMD             0x0004  /// PCI Command Register
#define PCI_STS             0x0006  /// PCI Status Register
#define PCI_RID             0x0008  /// Revision ID Register
#define PCI_IFT             0x0009  /// Interface Type
#define PCI_SCC             0x000A  /// Sub Class Code Register
#define PCI_BCC             0x000B  /// Base Class Code Register
#define PCI_CLS             0x000C  /// Cache Line Size
#define PCI_PMLT            0x000D  /// Primary Master Latency Timer
#define PCI_HDR             0x000E  /// Header Type Register
#define PCI_BIST            0x000F  /// Built in Self Test Register
#define PCI_BAR0            0x0010  /// Base Address Register 0
#define PCI_BAR1            0x0014  /// Base Address Register 1
#define PCI_BAR2            0x0018  /// Base Address Register 2
#define PCI_PBUS            0x0018  /// Primary Bus Number Register
#define PCI_SBUS            0x0019  /// Secondary Bus Number Register
#define PCI_SUBUS           0x001A  /// Subordinate Bus Number Register
#define PCI_SMLT            0x001B  /// Secondary Master Latency Timer
#define PCI_BAR3            0x001C  /// Base Address Register 3
#define PCI_IOBASE          0x001C  /// I/O base Register
#define PCI_IOLIMIT         0x001D  /// I/O Limit Register
#define PCI_SECSTATUS       0x001E  /// Secondary Status Register
#define PCI_BAR4            0x0020  /// Base Address Register 4
#define PCI_MEMBASE         0x0020  /// Memory Base Register
#define PCI_MEMLIMIT        0x0022  /// Memory Limit Register
#define PCI_BAR5            0x0024  /// Base Address Register 5
#define PCI_PRE_MEMBASE     0x0024  /// Prefetchable memory Base register
#define PCI_PRE_MEMLIMIT    0x0026  /// Prefetchable memory Limit register
#define PCI_PRE_MEMBASE_U   0x0028  /// Prefetchable memory base upper 32 bits
#define PCI_PRE_MEMLIMIT_U  0x002C  /// Prefetchable memory limit upper 32 bits
#define PCI_SVID            0x002C  /// Subsystem Vendor ID
#define PCI_SID             0x002E  /// Subsystem ID
#define PCI_IOBASE_U        0x0030  /// I/O base Upper Register
#define PCI_IOLIMIT_U       0x0032  /// I/O Limit Upper Register
#define PCI_CAPP            0x0034  /// Capabilities Pointer
#define PCI_EROM            0x0038  /// Expansion ROM Base Address
#define PCI_INTLINE         0x003C  /// Interrupt Line Register
#define PCI_INTPIN          0x003D  /// Interrupt Pin Register
#define PCI_MAXGNT          0x003E  /// Max Grant Register
#define PCI_BRIDGE_CNTL     0x003E  /// Bridge Control Register
#define PCI_MAXLAT          0x003F  /// Max Latency Register
#endif

UINT8
MmPci8 (
  IN UINT8              SegMent,
  IN UINT8              Bus,
  IN UINT8              Device,
  IN UINT8              Function,
  IN UINT16             Reg
  )
{
  UINTN                 PcieAddress;

  PcieAddress = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, Reg);
  return PciExpressRead8 (PcieAddress);
}

UINT16
MmPci16 (
  IN UINT8              SegMent,
  IN UINT8              Bus,
  IN UINT8              Device,
  IN UINT8              Function,
  IN UINT16             Reg
  )
{
  UINTN                 PcieAddress;

  PcieAddress = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, Reg);
  return PciExpressRead16 (PcieAddress);
}

UINT32
MmPci32 (
  IN UINT8              SegMent,
  IN UINT8              Bus,
  IN UINT8              Device,
  IN UINT8              Function,
  IN UINT16             Reg
  )
{
  UINTN                 PcieAddress;

  PcieAddress = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, Reg);
  return PciExpressRead32 (PcieAddress);
}

VOID
InsydeWheaMemcpy (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  )

{
  CHAR8 *Destination8;
  CHAR8 *Source8;

  Destination8 = Destination;
  Source8 = Source;
  while (Length--) {
    *(Destination8++) = *(Source8++);
  }
}

//[-start-140425-IB10310054-modify]//
/**
  BIOS SMI handler for handling WHEA error records.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] MemInfo             Pointer to the specific memory device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogMemory(
  IN UINT16                            ErrorType,
  IN MEMORY_DEV_INFO                   *MemInfo,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
  )
{
  GENERIC_ERROR_STATUS                  *ErrStatusBlk;
  GENERIC_ERROR_DATA_ENTRY              *ErrDataEntry;
  PLATFORM_MEMORY_ERROR_DATA_UEFI_231   *MemErrData;
  GENERIC_HW_ERROR_SOURCE               *HWErrSource;

  //
  //  Calculate Error Status Block Address
  //
  if (SysErrSrc == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  HWErrSource = (GENERIC_HW_ERROR_SOURCE *) SysErrSrc->SourceData;
  ErrStatusBlk= (GENERIC_ERROR_STATUS *) (*(UINTN *)(UINTN)HWErrSource->ErrorStatusAddress.Address);

  if (ErrStatusBlk == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  //  Section Descriptor
  //
  ErrDataEntry = (GENERIC_ERROR_DATA_ENTRY *)((UINTN)ErrStatusBlk + sizeof(GENERIC_ERROR_STATUS) + ErrStatusBlk->ErrDataSize);
  //
  //  Section Body
  //
  MemErrData = (PLATFORM_MEMORY_ERROR_DATA_UEFI_231 *)((UINTN)ErrDataEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));
  //
  //  Generic Error Status
  //
  if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED) {
    ErrStatusBlk->Severity = GENERIC_ERROR_CORRECTED;
    
    if (ErrStatusBlk->BlockStatus.CeValid != 1) {
      ErrStatusBlk->BlockStatus.CeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleCeValid = 1;
    }
 
  } else {
    ErrStatusBlk->Severity = GENERIC_ERROR_FATAL;
    
    if (ErrStatusBlk->BlockStatus.UeValid != 1) {
      ErrStatusBlk->BlockStatus.UeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleUeValid = 1;
    }
  }

  ErrStatusBlk->RawDataOffset = 0;
  ErrStatusBlk->RawDataSize   = 0; 
  ErrStatusBlk->BlockStatus.NumErrorDataEntry++;
  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    ErrStatusBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PLATFORM_MEMORY_ERROR_DATA_UEFI_231);
  } else {
    ErrStatusBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PLATFORM_MEMORY_ERROR_DATA);
  }
  //
  //  Generic Error Data Entry
  //
  InsydeWheaMemcpy (&ErrDataEntry->SectionType, &gEfiPlatformMemoryErrorSectionGuid, sizeof(EFI_GUID));
  ErrDataEntry->Severity = ErrStatusBlk->Severity;
  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    ErrDataEntry->Revision = GENERIC_ERROR_SECTION_REVISION_UEFI231;
    ErrDataEntry->DataSize = sizeof (PLATFORM_MEMORY_ERROR_DATA_UEFI_231);
  } else {
    ErrDataEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
    ErrDataEntry->DataSize = sizeof (PLATFORM_MEMORY_ERROR_DATA);
  }
  ErrDataEntry->SectionFlags.Primary = 1;

  //
  //  Memory Error Section
  //
  if ((MemInfo->ValidBits & 0x0003FFFF) != 0) {
    MemErrData->ValidFields = PLATFORM_MEM_ERROR_STATUS_VALID | (MemInfo->ValidBits & 0x0003FFFF);
  } else {
    MemErrData->ValidFields = PLATFORM_MEM_ERROR_STATUS_VALID;
  }
  MemErrData->ErrorStatus.Type = ErrorMemStorage;
  MemErrData->Node         = MemInfo->Node;
  MemErrData->ModuleRank   = MemInfo->Dimm;
  MemErrData->Bank         = MemInfo->Bank;
  MemErrData->Device       = MemInfo->Device;
  MemErrData->Row          = MemInfo->Row;
  MemErrData->Column       = MemInfo->Column;
  MemErrData->ErrorType    = (UINT8)MemInfo->ErrorType;  

  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    MemErrData->RankNumber         = MemInfo->Rank;
    MemErrData->SmBiosCardHandle   = MemInfo->SmBiosCardHandle;
    MemErrData->SmBiosModuleHandle = MemInfo->SmBiosModuleHandle;
  }
  return EFI_SUCCESS;
}
//[-end-140425-IB10310054-modify]//



/**
  Fill the PCIE Error message to specific errors status block.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] ErrPcieDev          Pointer to the specific PCIE device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogPcieRootDevBridge(
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
  )
{
  EFI_STATUS                            Status;
  GENERIC_ERROR_STATUS                  *ErrStatusBlk;
  GENERIC_ERROR_DATA_ENTRY              *ErrDataEntry;
  PCIE_ERROR_DATA                       *PcieErrData;
  GENERIC_HW_ERROR_SOURCE               *HWErrSource;
  UINT16                                StatusReg;
  UINT16                                SlotNum = 0x00;
  UINT8                                 PciHeader;

  //
  //  Calculate Error Status Block Address
  //
  if (SysErrSrc == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  HWErrSource = (GENERIC_HW_ERROR_SOURCE *) SysErrSrc->SourceData;
  ErrStatusBlk= (GENERIC_ERROR_STATUS *) (*(UINTN *)(UINTN)HWErrSource->ErrorStatusAddress.Address);
  if (ErrStatusBlk == NULL) {
    return EFI_NOT_FOUND;
  }

  ErrDataEntry = (GENERIC_ERROR_DATA_ENTRY *)((UINTN)ErrStatusBlk + sizeof(GENERIC_ERROR_STATUS) + ErrStatusBlk->ErrDataSize);
  PcieErrData = (PCIE_ERROR_DATA *)((UINTN)ErrDataEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));
  //
  //  Generic Error Status
  //
  if (ErrorType == GEN_ERR_SEV_PCIE_CORRECTED) {
    ErrStatusBlk->Severity = GENERIC_ERROR_CORRECTED;
    
    if (ErrStatusBlk->BlockStatus.CeValid != 1) {
      ErrStatusBlk->BlockStatus.CeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleCeValid = 1;
    }
 
  } else {
    ErrStatusBlk->Severity = GENERIC_ERROR_FATAL;
    
    if (ErrStatusBlk->BlockStatus.UeValid != 1) {
      ErrStatusBlk->BlockStatus.UeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleUeValid = 1;
    }
  }

  ErrStatusBlk->RawDataOffset = 0;
  ErrStatusBlk->RawDataSize = 0; 
  ErrStatusBlk->BlockStatus.NumErrorDataEntry++;
  ErrStatusBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PCIE_ERROR_DATA);
  //
  //  Generic Error Data Entry
  //
  InsydeWheaMemcpy (&ErrDataEntry->SectionType, &gEfiPcieErrorSectionGuid, sizeof(EFI_GUID));
  ErrDataEntry->Severity = ErrStatusBlk->Severity;
  ErrDataEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  ErrDataEntry->DataSize = sizeof (PCIE_ERROR_DATA);
  ErrDataEntry->SectionFlags.Primary = 1;
  //
  //  PCIE Error Section
  //
  PcieErrData->ValidFields = PCIE_ERROR_VERSION_VALID | PCIE_ERROR_COMMAND_STATUS_VALID | PCIE_ERROR_DEVICE_ID_VALID;
  PcieErrData->Version = PCIE_SPECIFICATION_SUPPORTED;

  StatusReg = MmPci16 (ErrPcieDev->Segment, ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_STS);

  PcieErrData->CommandStatus = ((PcieErrData->CommandStatus | StatusReg) << 16) | \
                                MmPci16 (ErrPcieDev->Segment, ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_CMD);
  
  PcieErrData->DevBridge.Device = ErrPcieDev->Device;
  PcieErrData->DevBridge.Function = ErrPcieDev->Function;
  PcieErrData->DevBridge.Segment= ErrPcieDev->Segment;
  PcieErrData->DevBridge.VendorId = MmPci16 (ErrPcieDev->Segment,
                                             ErrPcieDev->Bus,
                                             ErrPcieDev->Device,
                                             ErrPcieDev->Function,
                                             PCI_VID
                                             );
  
  PcieErrData->DevBridge.DeviceId = MmPci16 (ErrPcieDev->Segment,
                                             ErrPcieDev->Bus,
                                             ErrPcieDev->Device,
                                             ErrPcieDev->Function,
                                             PCI_DID
                                             );

  PcieErrData->DevBridge.ClassCode = ((PcieErrData->DevBridge.ClassCode | \
              MmPci8 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_BCC)) >> 8) | \
              MmPci8 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_SCC);
  //
  //  
  //
  PciHeader = MmPci8 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_HDR);
  if (PciHeader & BIT0) { ///Type 1
    PcieErrData->DevBridge.PrimaryOrDeviceBus = MmPci8 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_PBUS);
    PcieErrData->DevBridge.SecondaryBus = MmPci8 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_SBUS);
    PcieErrData->BridgeControlStatus = ((PcieErrData->BridgeControlStatus | \
                                       MmPci16 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_BRIDGE_CNTL)) << 16) | \
                                       MmPci16 (ErrPcieDev->Segment,ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function, PCI_SECSTATUS);
    PcieErrData->ValidFields |= PCIE_ERROR_BRIDGE_CRL_STS_VALID; 
  } else { /// Type 0
    PcieErrData->DevBridge.PrimaryOrDeviceBus = ErrPcieDev->Bus;
  }
  //
  //  Check Status Reg for detect capability support  
  //
  if (StatusReg & BIT4) {
    Status = InsydeWheaGetPcieCapabilities (&PcieErrData->Capability, ErrPcieDev->Segment, ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function);
    PcieErrData->ValidFields |= PCIE_ERROR_CAPABILITY_INFO_VALID; 

    if (PcieErrData->Capability.PcieCap[0] != 0x00) {
      //
      //  Get Pci Express Port Type
      //
      PcieErrData->PortType = PcieErrData->Capability.PcieCap[2] >> 4;  ///Offset 02h
      PcieErrData->ValidFields |= PCIE_ERROR_PORT_TYPE_VALID; 
      //
      //  Get Pci Express Slot Number
      //
      SlotNum = (((SlotNum | PcieErrData->Capability.PcieCap[23]) << 8) | PcieErrData->Capability.PcieCap[22]) >> 3; ///Offset 14h
      PcieErrData->DevBridge.Slot.Number = SlotNum;
    }
    //
    //  According the port type to Detect Pci Express support AER
    //
    if ((PcieErrData->PortType) == ROOT_PORT_OF_PCI_EXPRESS_ROOT_COMPLEX || (PcieErrData->Capability.PcieCap[2] >> 4) == ROOT_COMPLEX_EVENT_COLLECTOR) {
      Status = InsydeWheaGetPcieAer(&PcieErrData->AerInfo, ErrPcieDev->Segment, ErrPcieDev->Bus, ErrPcieDev->Device, ErrPcieDev->Function);
      PcieErrData->ValidFields |= PCIE_ERROR_AER_INFO_VALID; 
    }
  }

  return EFI_SUCCESS;
}

/**
  Fill the PCI Error message to specific errors status block.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] ErrPcieDev          Pointer to the specific PCIE device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogPciDev(
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
  )
{
  GENERIC_ERROR_STATUS                  *ErrStatusBlk;
  GENERIC_ERROR_DATA_ENTRY              *ErrDataEntry;
  PCI_PCIX_DEVICE_ERROR_DATA            *PciDevErrData;
  GENERIC_HW_ERROR_SOURCE               *HWErrSource;

  //
  //  Calculate Error Status Block Address
  //
  if (SysErrSrc == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  HWErrSource = (GENERIC_HW_ERROR_SOURCE *) SysErrSrc->SourceData;
  ErrStatusBlk= (GENERIC_ERROR_STATUS *) (*(UINTN *)(UINTN)HWErrSource->ErrorStatusAddress.Address);
  if (ErrStatusBlk == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  //  Section Descriptor
  //
  ErrDataEntry = (GENERIC_ERROR_DATA_ENTRY *)((UINTN)ErrStatusBlk + sizeof(GENERIC_ERROR_STATUS) + ErrStatusBlk->ErrDataSize);
  ///
  ///  Section Body
  ///
  PciDevErrData = (PCI_PCIX_DEVICE_ERROR_DATA *)((UINTN)ErrDataEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));
  //
  //  Generic Error Status
  //
  if (ErrorType == GEN_ERR_SEV_PCIE_CORRECTED) {
    ErrStatusBlk->Severity = GENERIC_ERROR_CORRECTED;
    
    if (ErrStatusBlk->BlockStatus.CeValid != 1) {
      ErrStatusBlk->BlockStatus.CeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleCeValid = 1;
    }
 
  } else {
    ErrStatusBlk->Severity = GENERIC_ERROR_FATAL;
    
    if (ErrStatusBlk->BlockStatus.UeValid != 1) {
      ErrStatusBlk->BlockStatus.UeValid = 1;
    } else {
      ErrStatusBlk->BlockStatus.MultipleUeValid = 1;
    }
  }

  ErrStatusBlk->RawDataOffset = 0;
  ErrStatusBlk->RawDataSize = 0; 
  ErrStatusBlk->BlockStatus.NumErrorDataEntry++;
  ErrStatusBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PCI_PCIX_DEVICE_ERROR_DATA);
  //
  //  Generic Error Data Entry
  //
  InsydeWheaMemcpy (&ErrDataEntry->SectionType, &gEfiPcieErrorSectionGuid, sizeof(EFI_GUID));
  ErrDataEntry->Severity = ErrStatusBlk->Severity;
  ErrDataEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  ErrDataEntry->DataSize = sizeof (PCI_PCIX_DEVICE_ERROR_DATA);
  ErrDataEntry->SectionFlags.Primary = 1;
  //
  //  PCI Device Error Section
  //
  PciDevErrData->ValidFields = PCI_PCIX_DEV_ERROR_STATUS_VALID | PCI_PCIX_DEV_ERROR_ID_INFO_VALID;
  PciDevErrData->ErrorStatus.Type = ErrorTimeout;
  PciDevErrData->DeviceId.Segment = ErrPcieDev->Segment;
  PciDevErrData->DeviceId.Bus = ErrPcieDev->Bus;
  PciDevErrData->DeviceId.Device = ErrPcieDev->Device;
  PciDevErrData->DeviceId.Function= ErrPcieDev->Function;
  
  PciDevErrData->DeviceId.VendorId = MmPci16 (ErrPcieDev->Segment,
                                              ErrPcieDev->Bus,
                                              ErrPcieDev->Device,
                                              ErrPcieDev->Function,
                                              PCI_VID);

  PciDevErrData->DeviceId.DeviceId = MmPci16 (ErrPcieDev->Segment,
                                              ErrPcieDev->Bus,
                                              ErrPcieDev->Device,
                                              ErrPcieDev->Function,
                                              PCI_DID);

  PciDevErrData->DeviceId.ClassCode= MmPci16 (ErrPcieDev->Segment,
                                              ErrPcieDev->Bus,
                                              ErrPcieDev->Device,
                                              ErrPcieDev->Function,
                                              PCI_SCC);

  return EFI_SUCCESS;
}

/**
  Get the PCI Express Capability structure information from the specific Pci Express Device.

  @param [in, out] AerBuf         Pointer to the Capability Buffer.
  @param [in]      Segment        Specific PCIE Device's segment Number.
  @param [in]      Bus            Specific PCIE Device's bus Number.
  @param [in]      Device         Specific PCIE Device's device Number.
  @param [in]      Function       Specific PCIE Device's Function Number.

  @retval EFI_SUCCESS             Get Capability Structure Success.

**/
STATIC
EFI_STATUS
InsydeWheaGetPcieCapabilities (
  IN OUT PCIE_CAPABILITY               *CapBuf,
  IN     UINT8                         Segment,
  IN     UINT8                         Bus,
  IN     UINT8                         Device,
  IN     UINT8                         Function
  )
{
  UINT8                           CapaId;
  UINT8                           NextCapaId;
  UINT8                           CapaIdPtr = 0;
  UINT8                           Index;

  //
  //  Get First Capability ID from PCI Configuration space offset 0x34h
  //
  NextCapaId = MmPci8 (Segment, Bus, Device, Function, PCI_CAPP);
  //
  //  Compare the CapabilityID to decide the PCI Express Capability Support
  //  if support, store the PCI Express Capability structure
  //    
  do {
    CapaId = MmPci8 (Segment, Bus, Device, Function, NextCapaId);
    if (CapaId == PCIE_CAPABILITY_ID) {
      CapaIdPtr = NextCapaId;
      break;
    }
    NextCapaId = MmPci8 (Segment, Bus, Device, Function, NextCapaId + 1);
  } while (NextCapaId != 0);
  
  if (CapaId == PCIE_CAPABILITY_ID) {
    for (Index = 0; Index < END_OF_PCIE_CAPABILITY_STRUCTURE; Index ++) {
      CapBuf->PcieCap[Index] = MmPci8 (Segment, Bus, Device, Function, CapaIdPtr + Index);
    }
  }
  
  return EFI_SUCCESS;
}

/**
  Get the advanced error reporting structure information from the specific Pci Express Device.

  @param [in, out] AerBuf         Pointer to the Capability Buffer.
  @param [in]      Segment        Specific PCIE Device's segment Number.
  @param [in]      Bus            Specific PCIE Device's bus Number.
  @param [in]      Device         Specific PCIE Device's device Number.
  @param [in]      Function       Specific PCIE Device's Function Number.

  @retval EFI_SUCCESS             Get Capability Structure Success.

**/
STATIC
EFI_STATUS
InsydeWheaGetPcieAer (
  IN OUT PCIE_AER                      *AerBuf,
  IN     UINT8                         Segment,
  IN     UINT8                         Bus,
  IN     UINT8                         Device,
  IN     UINT8                         Function
  )
{
  UINT32                                                EnCapabilityData;
  UINT8                                                 Index;
  UINT16                                                Register;
  PCIE_ENHANCED_CAPABILITY_HEADER                       *CapabilityHdr;

  //
  //  Get the Extended Capability Header from Offset 0x100h
  //
  Register = PCIE_EXTENDED_CAPABILITY_OFFSET;
  EnCapabilityData = MmPci32 (Segment, Bus, Device, Function, Register);
  CapabilityHdr = (PCIE_ENHANCED_CAPABILITY_HEADER *)&EnCapabilityData;
  //
  //  To decide the absence of the extended capability
  //
  if (CapabilityHdr->CapabilityID == 0xFFFF) {
    return EFI_UNSUPPORTED;
  }
  //
  //  Compare the CapabilityID to decide the AER Support
  //  if support, store the AER structure
  //    
  do {
    if (CapabilityHdr->CapabilityID == ADVANCED_ERROR_REPORTING_CAPABILITY_ID) {
      for (Index = 0x00; Index < END_OF_PCIE_AER_CAPABILITY_STRUCTURE; Index ++) {
        AerBuf->PcieAer[Index] = MmPci8(Segment, Bus, Device, Function, (Register + Index));
      }
      break;
    }
    Register = (UINT16) CapabilityHdr->NextCapabilityOffset;
    EnCapabilityData = MmPci32 (Segment, Bus, Device, Function, Register);
    CapabilityHdr = (PCIE_ENHANCED_CAPABILITY_HEADER *)&EnCapabilityData;
  } while (CapabilityHdr->NextCapabilityOffset != 0x00);

  return EFI_SUCCESS;
}

