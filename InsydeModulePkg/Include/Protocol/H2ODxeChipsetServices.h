/** @file
 H2O Chipset Services protocol interface definition.
 
 This file provides the H2O Chipset Services protocol definition. It is 
 consumed by drivers and libraries in the Kernel and Platform layers. It is
 produced by drivers and libraries in the Chipset layer.
 
 WARNING: This protocol should not be called directly. Instead, the library
 functions in the DxeChipsetSvcLib should be used, because they add the 
 necessary error checking.

;***************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_DXE_CHIPSET_SERVICES_H_
#define _H2O_DXE_CHIPSET_SERVICES_H_
#include <ChipsetSvc.h>
#include <IndustryStandard/Acpi10.h>
#include <CommonSmbiosMemory.h>
#include <IrqRoutingInformation.h>

#include <FastRecoveryData.h>
#include <Protocol/BiosRegionLock.h>

#define H2O_CHIPSET_SERVICES_PROTOCOL_GUID \
  {0x14afc99e, 0xe23e, 0x11e1, {0xa2, 0x34, 0xd0, 0xdf, 0x9a, 0x35, 0xc1, 0x06}}

//
// Function prototypes
//

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_SATA_COMRESET) (
  IN  UINTN                    PortNumber
  );

/**
 Platform specific function to enable/disable flash device write access. 

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function always returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_ENABLE_FD_WRITES) (
  IN  BOOLEAN                  EnableWrites
  );

/**
 IO address decode

 @param[in]         Type                Decode type
 @param[in]         Address             Decode address
 @param[in]         Length              Decode length
                                
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER One of the parameters has an invalid value
                                          1. Type has invalid value
                                          2. Address big then 0xFFFF
                                          3. Length need to be 4-byte aligned
 @retval            EFI_OUT_OF_RESOURCES  There are not enough resources available to set IO decode
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_IO_DECODE_CTRL) (
  IN IO_DECODE_TYPE            Type,
  IN IO_DECODE_ADDRESS         Address,
  IN UINT16                    Length
  );

/**
 Legacy Region Access Ctrl.

 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                If LEGACY_REGION_ACCESS_LOCK, then
                                        LegacyRegionAccessCtrl() lock the specific
                                        legacy region.
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_LEGACY_REG_ACCESS_CTRL) (
  IN  UINT32                   Start,
  IN  UINT32                   Length,
  IN  UINT32                   Mode
  );


/**
 Program Chipset SSID

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         VendorId            Vendor ID
 @param[in]         DeviceId            Device ID
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     1. The specific ID is not find.
                                        2. The specific device can not be set SSID.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PROGRAM_CHIPSET_SSID) (
  IN  UINT8                        Bus,
  IN  UINT8                        Dev,
  IN  UINT8                        Func,
  IN  UINT16                       VendorId,
  IN  UINT16                       DeviceId,
  IN  UINT32                       SsidSvid
  );
  
/**
 Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
 The chipset platform library can determine if it requires to switch to text
 mode while installing option rom for specific device.

 @param[in]         DeviceHandle        Handle of device OPROM is associated with.
 @param[in, out]    ForceSwitchTextMode Initial value contains the upper level's switch text
                                        mode policy. Set to TRUE if it requires to switch to
                                        text mode.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER DeviceHandle is NULL.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_INSTALL_PCI_ROM_SWITCH_TEXT_MODE) (
  IN      EFI_HANDLE         DeviceHandle,
  IN OUT  BOOLEAN           *ForceSwitchTextMode
  );
  
/**
 To set ISA information for common ISA driver utilization.

 @param[in, out]    Function            ISA function value
 @param[in, out]    Device              ISA device value

 @retval            EFI_SUCCESS         This function always return successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_SET_ISA_INFO) (
  IN OUT UINT8    *Function,
  IN OUT UINT8    *Device
  );
  
/**
 To collection information of memory and set to SMBIOS.

 @param[in, out]    ChipsetSmbiosMemData Point to SMBIOS memory data

 @retval            EFI_SUCCESS          This function always return successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_SET_SMBIOS_MEMORY_DATA) (
  IN OUT H2O_SMBIOS_MEM_CONFIG_DATA        *ChipsetSmbiosMemData
  );

/**
 Skip to load PCI ROM for specify device.

 @param[in]         Segment             The PCI segment number for PCI device.
 @param[in]         Bus                 The PCI bus number for PCI device.
 @param[in]         Device              The PCI device number for PCI device.
 @param[in]         Function            The PCI function number for PCI device.
 @param[in]         VendorId            The vendor ID for PCI device.
 @param[in]         DeviceId            The device ID for PCI device.
 @param[in, out]    SkipGetPciRom       If SkipGetPciRom == TRUE means that there is no ROM in this device.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            EFI_PROTOCOL_ERROR  Can't find setup utility protocol.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_SET_SKIP_GET_PCI_ROM) (
  IN  UINTN        Segment,
  IN  UINTN        Bus,
  IN  UINTN        Device,
  IN  UINTN        Function,
  IN  UINT16       VendorId,
  IN  UINT16       DeviceId,
  OUT BOOLEAN     *SkipGetPciRom
  );
  
/**
 Update ACPI table content according to platform specific

 @param[in, out]    TableHeader             Pointer of the table to update
 @param[in, out]    CommonCodeReturnStatus  Return Status from Common Code
*/
typedef
VOID
(EFIAPI *H2O_CS_SVC_UPDATE_ACPI_TABLE) (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER         *TableHeader,
  IN OUT  EFI_STATUS                          *CommonCodeReturnStatus
  );

/**
 Provide IRQ Routing table according to platform specific information.

 @param[OUT]      VirtualBusTablePtr           A pointer to the VirtualBusTable that list all bridges in this platform 
                                               and bridges' virtual secondary bus.
 @param[OUT]      VirtualBusTableEntryNumber   A pointer to the VirtualBusTable entry number.
 @param[OUT]      IrqPoolTablePtr,             A pointer to the IRQ Pool Table.
 @param[OUT]      IrqPoolTableNumber,          A pointer to the IRQ Pool Table entry number.
 @param[OUT]      PirqPriorityTable,           A pointer to the PIRQ priority used Table.
 @param[OUT]      PirqPriorityTableEntryNumber A pointer to the PIRQ priority used Table entry number.
 @param[OUT]      IrqTablePtr                  A pointer to the IRQ routing Table.

 @retval          EFI_SUCCESS                  Get Platform specific IRQ Routing Info successfully..
 @retval          EFI_UNSUPPORTED              If the function is not implemented.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_IRQ_ROUTING_INFORMATION) (
  OUT LEGACY_MODIFY_PIR_TABLE             **VirtualBusTablePtr,
  OUT UINT8                               *VirtualBusTableEntryNumber,
  OUT EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY **IrqPoolTablePtr,
  OUT UINT8                               *IrqPoolTableNumber,
  OUT UINT8                               **PirqPriorityTablePtr,
  OUT UINT8                               *PirqPriorityTableEntryNumber,
  OUT IRQ_ROUTING_TABLE                   **IrqRoutingTablePtr
  );

/**
 Get information of recovery FV.

 @param[out]        RecoveryRegion        recovery region table
 @param[out]        NumberOfRegion        number of recovery region
 @param[out]        ChipsetEmuPeiMaskList Specified PEIMs don't to dispatch in recovery mode
                    
 @retval            EFI_SUCCESS           This function alway return success.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_FV_RECOVERY_INFO) (
  OUT  PEI_FV_DEFINITION       **RecoveryRegion,
  OUT  UINTN                    *NumberOfRegion,
  OUT  EFI_GUID                **ChipsetEmuPeiMaskList
  );
  
/**
 Provide bios protect table for Kernel.

 @param[in, out]        *BiosRegionTable    Pointer to BiosRegion Table.
 @param[in, out]        ProtectRegionNum    The number of Bios protect region instances.

 @retval                EFI_SUCCESS         Provide table for kernel to set protect region and lock flash program registers.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_GET_BIOS_PROTECT_TABLE) (
  IN OUT BIOS_PROTECT_REGION           **BiosRegionTable,
  IN OUT UINT8                         *ProtectRegionNum
  );

/**
 Update ACPI variable set called upon AcpiS3Save driver entry and S3 ready. Chipset can use it to
 update the content of customized ACPI_VARIABLE_SET variable to compliant with individual chipset RC

 @param[in]         IsS3Ready                   Indicates the current phase is initial or S3 ready
 @param[in]         S3AcpiReservedMemorySize    The size of S3AcpiReservedMemory

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     Use ordinary path to update standard ACPI_VARIABLE_SET content
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_UPDATE_ACPI_VARIABLE_SET) (
  IN BOOLEAN     IsS3Ready,
  IN UINT32      S3AcpiReservedMemorySize
  );

typedef struct _H2O_CHIPSET_SERVICES_PROTOCOL {
  UINT32 Size;                          ///< size of this structure, in bytes.
  
  H2O_CS_SVC_SATA_COMRESET                      SataComReset;
  H2O_CS_SVC_ENABLE_FD_WRITES                   EnableFdWrites;
  H2O_CS_SVC_IO_DECODE_CTRL                     IoDecodeControl;
  H2O_CS_SVC_LEGACY_REG_ACCESS_CTRL             LegacyRegionAccessCtrl;
  H2O_CS_SVC_PROGRAM_CHIPSET_SSID               ProgramChipsetSsid;
  
  H2O_CS_SVC_INSTALL_PCI_ROM_SWITCH_TEXT_MODE   InstallPciRomSwitchTextMode;
  H2O_CS_SVC_SET_ISA_INFO                       SetIsaInfo;
  H2O_CS_SVC_SET_SMBIOS_MEMORY_DATA             SetSmbiosMemoryData;
  H2O_CS_SVC_SET_SKIP_GET_PCI_ROM               SkipGetPciRom;
  H2O_CS_SVC_UPDATE_ACPI_TABLE                  UpdateAcpiTable;
  H2O_CS_SVC_IRQ_ROUTING_INFORMATION            IrqRoutingInformation;
  H2O_CS_SVC_FV_RECOVERY_INFO                   FvRecoveryInfo;
  H2O_CS_SVC_GET_BIOS_PROTECT_TABLE             GetBiosProtectTable;
  H2O_CS_SVC_UPDATE_ACPI_VARIABLE_SET           UpdateAcpiVariableSet;

} H2O_CHIPSET_SERVICES_PROTOCOL;

extern EFI_GUID gH2OChipsetServicesProtocolGuid;

#endif
