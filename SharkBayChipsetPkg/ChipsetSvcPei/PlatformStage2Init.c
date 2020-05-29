/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcPlatformStage2Init().
 The function PeiCsSvcPlatformStage2Init() use chipset services to initialization 
 chipset in stage 2 of PEi phase.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Pi/PiPeiCis.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/CmosLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
//#include <Ppi/PchProductionFlag.h>
//#include <Ppi/CpuProductionFlag.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/Stall.h>
#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/AtaPolicy.h>
#include <Ppi/EcpAtaController.h>
#include <Ppi/PchUsbPolicy.h>
#include <Ppi/PchInit.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/S3RestoreAcpiCallback.h>
#include <Ppi/Reset.h>
#include <Ppi/SaPlatformPolicy.h>
#include <Ppi/PeiBootScriptDone.h>
#include <Ppi/RapidStart.h>
#include <Guid/SwitchableGraphicsVariable.h>
#include <Guid/AcpiVariableSet.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>
#include <ChipsetCmos.h>
#include <CpuRegs.h>
#include <PchPlatformLib.h>
#include <SwitchableGraphicsDefine.h>
#include <Ppi/CpuPlatformPolicy.h>

//[-start-120705-IB05330352-add]//
#define FLASH_DEVICE_TYPE_SPI 0
#define FLASH_DEVICE_TYPE_LPC 1
//[-end-120705-IB05330352-add]//

VOID
PeiTxtExecute (
  IN     CONST EFI_PEI_SERVICES       **PeiServices,
  IN     CHIPSET_CONFIGURATION   *SystemConfiguration,
  IN     BOOLEAN                IsVtSupport,
  IN OUT UINT8                  *CmosVmxSmxFlag
  );

//[-start-121129-IB03780468-modify]//
EFI_STATUS
InitialXtuInfoData (
  IN  EFI_PEI_SERVICES           **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN  VOID                       *Ppi
  );

EFI_STATUS
InitialXmpProfileData (
  IN  EFI_PEI_SERVICES           **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN  VOID                       *Ppi
  );
//[-end-121129-IB03780468-modify]//

//[-start-120920-IB06460442-add]//
EFI_STATUS
ResetEhciController (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_STALL_PPI                    *StallPpi,
  IN UINTN                                ControllerAddress
  );
//[-end-120920-IB06460442-add]//

STATIC
VOID
CloseHdAudio (
  IN CONST EFI_PEI_SERVICES                   **PeiServices
  );

EFI_STATUS
RestoreAcpiVariableHob (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  );

//[-start-120704-IB05330352-add]//
EFI_STATUS
EFIAPI
SetFlashType (
  IN  UINT32         FlashType
);
//[-end-120704-IB05330352-add]//

//[-start-121129-IB03780468-modify]//
static EFI_PEI_NOTIFY_DESCRIPTOR       mNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPeiCpuPlatformPolicyPpiGuid,
    InitialXtuInfoData                               
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    InitialXmpProfileData
  }
};
//[-end-121129-IB03780468-modify]//

static EFI_PEI_PPI_DESCRIPTOR mPeiBootScriptDonePpi = {
  ( EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST ),
  &gPeiBootScriptDonePpiGuid,
  NULL
  };

static EFI_PEI_NOTIFY_DESCRIPTOR       mMemoryDisCoverNotifyDispatchList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    RestoreAcpiVariableHob
  }
};

STATIC
EFI_STATUS
EFIAPI
EnableAtaChannel (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI        *This,
  IN UINT8                         ChannelMask
  )
{
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetAtaChannelIoSpace (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI       *This,
  IN  UINT8                        ChannelIndex,
  OUT IDE_BASE_REGISTERS           *ChannelIoSpace
  )
{
  switch (ChannelIndex) {
    
  case 0:
    ChannelIoSpace->Data          = 0x2158;
    ChannelIoSpace->Reg1.Error    = 0x2159;
    ChannelIoSpace->SectorCount   = 0x215A;
    ChannelIoSpace->SectorNumber  = 0x215B;
    ChannelIoSpace->CylinderLsb   = 0x215C;
    ChannelIoSpace->CylinderMsb   = 0x215D;
    ChannelIoSpace->Head          = 0x215E;
    ChannelIoSpace->Reg.Command   = 0x215F;
    ChannelIoSpace->Alt.AltStatus = 0x2176;
    ChannelIoSpace->DriveAddress  = 0x2177;
    break;
    
  case 1:
    ChannelIoSpace->Data          = 0x2150;
    ChannelIoSpace->Reg1.Error    = 0x2151;
    ChannelIoSpace->SectorCount   = 0x2152;
    ChannelIoSpace->SectorNumber  = 0x2153;
    ChannelIoSpace->CylinderLsb   = 0x2154;
    ChannelIoSpace->CylinderMsb   = 0x2155;
    ChannelIoSpace->Head          = 0x2156;
    ChannelIoSpace->Reg.Command   = 0x2157;
    ChannelIoSpace->Alt.AltStatus = 0x2172;
    ChannelIoSpace->DriveAddress  = 0x2173;
    break;

  case 2:
    ChannelIoSpace->Data          = 0x2148;
    ChannelIoSpace->Reg1.Error    = 0x2149;
    ChannelIoSpace->SectorCount   = 0x214A;
    ChannelIoSpace->SectorNumber  = 0x214B;
    ChannelIoSpace->CylinderLsb   = 0x214C;
    ChannelIoSpace->CylinderMsb   = 0x214D;
    ChannelIoSpace->Head          = 0x214E;
    ChannelIoSpace->Reg.Command   = 0x214F;
    ChannelIoSpace->Alt.AltStatus = 0x216E;
    ChannelIoSpace->DriveAddress  = 0x216F;
    break;

  case 3:
    ChannelIoSpace->Data          = 0x2140;
    ChannelIoSpace->Reg1.Error    = 0x2141;
    ChannelIoSpace->SectorCount   = 0x2142;
    ChannelIoSpace->SectorNumber  = 0x2143;
    ChannelIoSpace->CylinderLsb   = 0x2144;
    ChannelIoSpace->CylinderMsb   = 0x2145;
    ChannelIoSpace->Head          = 0x2146;
    ChannelIoSpace->Reg.Command   = 0x2147;
    ChannelIoSpace->Alt.AltStatus = 0x216A;
    ChannelIoSpace->DriveAddress  = 0x216B;
    break;

  default:
    return EFI_NOT_FOUND;
  }
  
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetSataControllerBase (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI         *This,
  IN EFI_PHYSICAL_ADDRESS           *ControllerAddr
  )
{
  *ControllerAddr = EFI_PEI_PCI_CFG_ADDRESS(
                      DEFAULT_PCI_BUS_NUMBER_PCH, 
                      PCI_DEVICE_NUMBER_PCH_SATA, 
                      PCI_FUNCTION_NUMBER_PCH_SATA, 
                      0
                      );
  return EFI_SUCCESS;
}

static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi = {
  EnableAtaChannel,
  GetAtaChannelIoSpace,
  GetSataControllerBase
  };

static EFI_PEI_PPI_DESCRIPTOR mAtaControllerPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEcpPeiAtaControllerPpiGuid,
  &mAtaControllerPpi
  };

/**
 Service to enable primary and secondary ATA channels

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         AtaPolicyPpi        A pointer to AtaPolicyPpi PPI

 @retval            EFI_SUCCESS         Procedure complete.
*/
STATIC
EFI_STATUS
EFIAPI
AtaInit (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *AtaPolicyPpi
  )
{
  EFI_STATUS                    Status;
  UINT16                        ClassCode;
  UINT32                        Index;
  UINT16                        RegData16;
  UINT16                        WordReg;
  UINT32                        SataPortsEnabled;
  EFI_PEI_STALL_PPI             *StallPpi;
  PCH_PLATFORM_POLICY_PPI       *PchPlatformPolicyPpi;
  
  RegData16 = 0;

  //
  // The code is moved from ChipsetInitPlatformStage2.c
  // BUG HERE: As PCH EDS description, software must not attempt to re-enable the disabled function.
  // A disabled function can only be re-enabled by a platform reset.
  //
  Status = PeiServicesLocatePpi (
             &gPchPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&PchPlatformPolicyPpi
             );

  MmioAnd32 (PchPlatformPolicyPpi->Rcba + R_PCH_RCRB_FUNC_DIS, ((UINT32) (~B_PCH_RCRB_FUNC_DIS_SATA2)));

  //
  // Check SATA Class code
  //
  ClassCode = PchSataPciCfg16 (R_PCH_SATA_SUB_CLASS_CODE);
  if (ClassCode != 0x0101) {
    //
    // This is not in IDE mode. Return directly, because PEI IDE BUS driver
    // only support IDE Legacy Mode.
    //
    return EFI_SUCCESS;
  } 
  //
  // Configure Sata 
  //

  PchSataPciCfg32AndThenOr (R_PCH_SATA_PCMD_BAR, 0, 0x2158);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_PCNL_BAR, 0, 0x2176);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_SCMD_BAR, 0, 0x2150);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_SCNL_BAR, 0, 0x2170);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_BUS_MASTER_BAR, 0, 0x2130);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_AHCI_BAR, 0, 0x2120);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_PI_REGISTER, 0, 0xF);

  PchSataPciCfg32AndThenOr (R_PCH_SATA_PCMD_BAR, 0, 0x2148);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_PCNL_BAR, 0, 0x216E);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_SCMD_BAR, 0, 0x2140);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_SCNL_BAR, 0, 0x2168);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_BUS_MASTER_BAR, 0, 0x2110);
  PchSataPciCfg32AndThenOr (R_PCH_SATA_AHCI_BAR, 0, 0x2100);
  
  //
  // Set operation mode
  //
  //
  
  // Set MAP to IDE for SATA #1, the Sub Class Code will be set to 1
  // no need to set for SATA #2 since MAP & SCC of SATA #2 is RO to IDE.
  //  default is enable all port, skip
  //
  RegData16 |= V_PCH_SATA_MAP_SMS_IDE;
  RegData16 &= ~B_PCH_SATA_PORT_TO_CONTROLLER_CFG;

  //
  // Now time to set the MAP register
  //
  PchSataPciCfg16AndThenOr ( R_PCH_SATA_MAP, 0, RegData16);
  
  //
  // Set D31:F2 PCS
  // default is enable all
  //
//[-start-120628-IB06460410-modify]//
  for (Index = 0; Index < LPTH_AHCI_MAX_PORTS; Index++) {
//[-end-120628-IB06460410-modify]//
      PchSataPciCfg16Or (R_PCH_SATA_PCS, (UINT16) (B_PCH_SATA_PCS_PORT0_EN << Index));
  }

  //
  // Set D31:F5 PCS
  // default is enable all
  //

  for (Index = 0; Index < PCH_AHCI_2_MAX_PORTS; Index++) {
    PchSata2PciCfg16Or (R_PCH_SATA_PCS, (UINT16) (B_PCH_SATA2_PCS_PORT4_EN << Index));
  }

  //
  // Set legacy IDE decoding
  //
  PchSataPciCfg16Or (R_PCH_SATA_TIMP , (UINT16) (B_PCH_SATA_TIM_IDE));

  PchSataPciCfg16Or (R_PCH_SATA_TIMS , (UINT16) (B_PCH_SATA_TIM_IDE));

  PchSata2PciCfg16Or (R_PCH_SATA_TIMP , (UINT16) (B_PCH_SATA_TIM_IDE));

  PchSata2PciCfg16Or (R_PCH_SATA_TIMS , (UINT16) (B_PCH_SATA_TIM_IDE));

  //
  // D31:F2 PCS: only enable ports that either have a device attached or have
  // hot plug enabled
  //
  // bugbug: do we need a time delay to allow device to show up after we enable the
  // port
  //
    Status = PeiServicesLocatePpi (
               &gEfiPeiStallPpiGuid,
               0,
               NULL,
               (VOID **)&StallPpi
               );
  
  if (!EFI_ERROR (Status)) {
    StallPpi->Stall (
                (CONST EFI_PEI_SERVICES **)PeiServices,
                StallPpi,
                3000000
                );
  }
  //
  // use default setting, 3, 4, 5
  //
  SataPortsEnabled = 0;
  WordReg = PchSataPciCfg16 (R_PCH_SATA_PCS);
//[-start-120628-IB06460410-modify]//
  for (Index = 0; Index < LPTH_AHCI_MAX_PORTS; Index++) {
//[-end-120628-IB06460410-modify]//
    if ((Index <= 2 || Index >=5) || (WordReg & (BIT0 << (8 + Index)))) {
      SataPortsEnabled |= (0x1 << Index);
    }
  }

  PchSataPciCfg16AndThenOr (
    R_PCH_SATA_PCS, 
    (UINT16) (~(B_PCH_SATA_PCS_PORT5_EN |
                B_PCH_SATA_PCS_PORT4_EN |
                B_PCH_SATA_PCS_PORT3_EN |
                B_PCH_SATA_PCS_PORT2_EN |
                B_PCH_SATA_PCS_PORT1_EN |
                B_PCH_SATA_PCS_PORT0_EN
               )
             ),
    (UINT16) (SataPortsEnabled)
    );

  //
  // D31:F5 PCS: only enable ports that either have a device attached or have
  // hot plug enabled
  //
  SataPortsEnabled  = 0;
  WordReg           = PchSata2PciCfg16 (R_PCH_SATA_PCS);
  for (Index = 0; Index < PCH_AHCI_2_MAX_PORTS; Index++) {
    SataPortsEnabled |= 0x1 << Index;
  }

  PchSata2PciCfg16AndThenOr (
    R_PCH_SATA_PCS, 
    (UINT16) (~(B_PCH_SATA2_PCS_PORT5_EN | B_PCH_SATA2_PCS_PORT4_EN)), 
    (UINT16) (SataPortsEnabled)
    );
  //
  // set the BAR for SIDE Controller
  //
  PchSataPciCfg32Or (R_PCH_SATA_BUS_MASTER_BAR, ((PEI_ATA_POLICY_PPI *)AtaPolicyPpi)->BaseAddress);
  PchSata2PciCfg32Or (R_PCH_SATA_BUS_MASTER_BAR, ((PEI_ATA_POLICY_PPI *)AtaPolicyPpi)->BaseAddress);
  //
  // Enable SIDE Controller
  //
  PchSataPciCfg32Or (R_PCH_SATA_COMMAND ,  (B_PCH_SATA_COMMAND_IOSE | B_PCH_SATA_COMMAND_BME));
  PchSata2PciCfg32Or (R_PCH_SATA_COMMAND ,  (B_PCH_SATA_COMMAND_IOSE | B_PCH_SATA_COMMAND_BME));

  Status = PeiServicesInstallPpi (&mAtaControllerPpiList);
  
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}

/**
 Initialize PCH USB in early stage for recovery usage.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         Ppi                 A pointer to AtaPolicyPpi PPI.

 @retval            EFI_SUCCESS         Procedure complete.
 @retval            others              Error occurs
*/
STATIC
EFI_STATUS
EFIAPI
UsbInit (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN VOID                            *Ppi
  )
{
  EFI_STATUS         Status;
  PCH_INIT_PPI       *PchPeiInitPpi;
  
  Status = PeiServicesLocatePpi (&gPchInitPpiGuid, 0, NULL, (VOID **)&PchPeiInitPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Init PCH USB
  //
  Status = PchPeiInitPpi->UsbInit((CONST EFI_PEI_SERVICES **)PeiServices);

  return Status;
}

static
EFI_PEI_NOTIFY_DESCRIPTOR mRecoveryNotifyList[] = {
  { 
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    AtaInit 
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
//[-start-120327-IB06460380-modify]//
//    &gPchUsbPolicyPpiGuid
    &gEfiPeiMemoryDiscoveredPpiGuid,
//[-end-120327-IB06460380-modify]//
    UsbInit
  }
};

/**
 Register notify ppi to reset the EHCI.
 CAUTION!!! This function may NOT be called due to the
 MAX_PPI_DESCRIPTORS(in PeiCore.h) exceed 64 and caused
 EHCI malfunction in DXE phase. For this situation, put
 the EHCI HC reset code into DXE chipset init driver

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         Ppi                 A pointer to the PPI interface in notify descriptor.
                    
 @retval            EFI_SUCCESS         Procedure complete.
 @retval            others              Error occurs
*/
STATIC
EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  EFI_STATUS        Status;
  EFI_PEI_STALL_PPI *StallPpi;
//[-start-120920-IB06460442-remove]//
//  UINTN             Index;
//  UINT32            *BaseAddress;
//  UINT32            Command;
//[-end-120920-IB06460442-remove]//
  
  Status = PeiServicesLocatePpi (
             &gEfiPeiStallPpiGuid,
             0,
             NULL,
             (VOID **)&StallPpi
             );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }
//[-start-120920-IB06460442-modify]//
//  //
//  // Reset the HC
//  //
//  for (Index = 0; Index < PchEhciControllerMax; Index ++) {
//    BaseAddress = (UINT8*) (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB, PCI_FUNCTION_NUMBER_PCH_EHCI, R_PCH_EHCI_MEM_BASE) 
//                   + (Index * V_PCH_EHCI_MEM_LENGTH) );
//    BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);
//    //
//    // Halt HC first
//    //
//    Command = *(UINT32*)(UINTN)BaseAddress;
//    Command &= ~0x01;
//    *(UINT32*)(UINTN)BaseAddress = Command;
//    //
//    // Stall 1 millisecond
//    //
//    StallPpi->Stall (
//                PeiServices,
//                StallPpi,
//                1000
//                );
//    //
//    // HCReset
//    //
//    Command = *(UINT32*)(UINTN)BaseAddress;
//    Command |= 0x02;
//    *(UINT32*)(UINTN)BaseAddress = Command;
//
//  }
  //
  // Reset EHCI Controller #1 (B0:D29:F0)
  //
  ResetEhciController ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB, PCI_FUNCTION_NUMBER_PCH_EHCI, 0));
  //
  // Reset EHCI Controller #2 (B0:D26:F0)
  //
  ResetEhciController ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB_EXT, PCI_FUNCTION_NUMBER_PCH_EHCI, 0));
//[-end-120920-IB06460442-modify]//
  return EFI_SUCCESS;
}

static
EFI_PEI_NOTIFY_DESCRIPTOR mResetEHCI[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    EndOfPeiPpiNotifyCallback
  }
};
//[-start-121012-IB06460456-remove]//
///**
// Reset the SATA mode to correct value.
//
// @param[in]         PeiServices         General purpose services available to every PEIM.
// @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
// @param[in]         Ppi                 A pointer to EndOfSignal PPI
//
// @retval            EFI_SUCCESS         Procedure complete.
// @retval            others              Error occurs
//*/
//EFI_STATUS
//EFIAPI
//EndOfPeiSataPpiNotifyCallback (
//  IN EFI_PEI_SERVICES           **PeiServices,
//  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
//  IN VOID                       *Ppi
//  )
//{
//  EFI_STATUS                    Status;
//  PCH_PLATFORM_POLICY_PPI       *PchPlatformPolicyPpi;
//  UINTN                         PciD31F2RegBase;
//  
//  Status = (**PeiServices).LocatePpi (
//                             PeiServices,
//                             &gPchPlatformPolicyPpiGuid,
//                             0,
//                             NULL,
//                             &PchPlatformPolicyPpi
//                             );
//                             
//  PciD31F2RegBase = PchPciDeviceMmBase (
//                      PchPlatformPolicyPpi->BusNumber,
//                      PCI_DEVICE_NUMBER_PCH_SATA,
//                      PCI_FUNCTION_NUMBER_PCH_SATA
//                      );
//                                                   
//  if (PchPlatformPolicyPpi->SataConfig->SataMode != PchSataModeIde) {
//    //
//    // AHCI and RAID mode, SATA Port 0 - Port 5 are all for D31:F2, D31:F5 is disabled
//    //
//    MmioOr8 (PciD31F2RegBase + R_PCH_SATA_MAP, (UINT8) (B_PCH_SATA_PORT_TO_CONTROLLER_CFG));
//    MmioOr32 (PchPlatformPolicyPpi->Rcba + R_PCH_RCRB_FUNC_DIS, (UINT32) B_PCH_RCRB_FUNC_DIS_SATA2);
//
//    if (PchPlatformPolicyPpi->SataConfig->SataMode == PchSataModeAhci) {      
//      MmioOr8 (PciD31F2RegBase + R_PCH_SATA_MAP, (UINT8) (V_PCH_SATA_MAP_SMS_AHCI));
//    } else if (PchPlatformPolicyPpi->SataConfig->SataMode == PchSataModeRaid) {
//      MmioOr8 (PciD31F2RegBase + R_PCH_SATA_MAP, (UINT8) (V_PCH_SATA_MAP_SMS_RAID));
//    }
//    
//    //
//    // Re-Enable the SATA port4 and port5.
//    //
//    MmioOr8 (PciD31F2RegBase + R_PCH_SATA_PCS, (UINT8) (B_PCH_SATA_PCS_PORT5_EN | B_PCH_SATA_PCS_PORT4_EN));
//  }
//
//  return EFI_SUCCESS;
//}
//[-end-121012-IB06460456-remove]//
//[-start-121012-IB06460456-remove]//
//static
//EFI_PEI_NOTIFY_DESCRIPTOR mResetSata[] = {
//  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
//  &gEfiEndOfPeiSignalPpiGuid,
//  EndOfPeiSataPpiNotifyCallback
//  };
//[-end-121012-IB06460456-remove]//
//No this on SHB
//EFI_PEI_PPI_DESCRIPTOR  mPpiListCpuProductionFlag = {
//  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
//  &gPeiCpuProductionFlagPpiGuid,
//  NULL
//};
// 
//EFI_PEI_PPI_DESCRIPTOR  mPpiListPchProductionFlag = {
//  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
//  &gPeiPchProductionFlagPpiGuid,
//  NULL
//};

/**
 Check on the processor if VT is supported.

 @param             None

 @retval            TRUE                VT is supported
 @retval            FALSE               VT is not supported
*/
STATIC
BOOLEAN
IsVmxSupported (
  VOID
  )
{
  UINT32                   RegEcx;
  BOOLEAN                  Support;

  //
  // Default is not supported.
  //
  Support = FALSE;

  //
  // Get CPUID to check if the processor supports Vanderpool Technology.
  //
  AsmCpuid ( CPUID_VERSION_INFO, NULL, NULL, &RegEcx, NULL );
  if ( ( RegEcx & B_CPUID_VERSION_INFO_ECX_VME ) != 0 ) {
    //
    // VT is supported.
    //
    Support = TRUE;
  }

  return Support;
}

/**
 Clear any SMI status or wake status left over from boot.

 @param[in]         CpuIo               A pointer to CPU IO services interface address

 @retval            EFI_SUCCESS         Procedure complete.
*/
STATIC
EFI_STATUS
ClearPchSmiAndWake (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  UINT16      Pm1Sts;
  UINT32      Value;
  UINT32      Gpe0aSts;
  UINT32      Gpe0bSts;
  UINT32      SmiSts;
  UINTN       Base = 0;
//[-start-121105-IB03780470-add]//
  UINT32      Gpe0Sts;
  UINT32      Gpe0Sts2;
  UINT32      Gpe0Sts3;
  UINT32      Gpe0Sts4;
  PCH_SERIES  PchSeries;
//[-end-121105-IB03780470-add]//

  Value = 0;
  SmiSts = 0;
//[-start-121105-IB03780470-add]//
  Gpe0Sts  = 0;
  Gpe0Sts2 = 0;
  Gpe0Sts3 = 0;
  Gpe0Sts4 = 0;
  Gpe0aSts = 0;
  Gpe0bSts = 0;
//[-end-121105-IB03780470-add]//

  //
  // Clear any SMI or wake state from the boot
  //
//[-start-121008-IB11410008-modify]//
//[-start-121105-IB03780470-modify]//
  PchSeries = GetPchSeries ();
  if (PchSeries == PchLp) {
    Gpe0Sts  = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96);
    Gpe0Sts2 = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x04);
    Gpe0Sts3 = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x08);
    Gpe0Sts4 = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x0C);
  } else {
    Gpe0aSts = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0a_STS);
    Gpe0bSts = IoRead32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0b_STS);
  }
//[-end-121008-IB11410008-modify]//
  Pm1Sts =(B_PCH_ACPI_PM1_STS_PRBTNOR | B_PCH_ACPI_PM1_STS_PWRBTN);

//[-start-121105-IB03780470-modify]//
  if (PchSeries == PchLp) {
    Gpe0Sts |= (
               B_PCH_ACPI_GPE0_STS_127_96_GP27     |
               B_PCH_ACPI_GPE0_STS_127_96_PME_B0   |
               B_PCH_ACPI_GPE0_STS_127_96_PME      |
               B_PCH_ACPI_GPE0_STS_127_96_BATLOW   |
               B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP  |
               B_PCH_ACPI_GPE0_STS_127_96_RI       |
               B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK  |
               B_PCH_ACPI_GPE0_STS_127_96_TC0SCI   |
               B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG
               );
  } else {
    Gpe0aSts |= (
                B_PCH_ACPI_GPE0a_STS_PME_B0 |
                B_PCH_ACPI_GPE0a_STS_PME |
                B_PCH_ACPI_GPE0a_STS_BATLOW |
                B_PCH_ACPI_GPE0a_STS_PCI_EXP |
                B_PCH_ACPI_GPE0a_STS_RI |
                B_PCH_ACPI_GPE0a_STS_SMB_WAK |
                B_PCH_ACPI_GPE0a_STS_TC0SCI |
                B_PCH_ACPI_GPE0a_STS_HOT_PLUG
                );
    Gpe0bSts |= (B_PCH_ACPI_GPE0b_STS_GP27);
  }
//[-end-121105-IB03780470-modify]//

  SmiSts |= (
            B_PCH_SMI_STS_GPIO_UNLOCK |
            B_PCH_SMI_STS_SMBUS |
            B_PCH_SMI_STS_PERIODIC |
            B_PCH_SMI_STS_TCO |
            B_PCH_SMI_STS_MCSMI |
            B_PCH_SMI_STS_SWSMI_TMR |
            B_PCH_SMI_STS_APM |
            B_PCH_SMI_STS_ON_SLP_EN |
            B_PCH_SMI_STS_BIOS
            );

  //
  // Write them back
  //

//[-start-121008-IB11410008-modify]//
  IoWrite16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS, Pm1Sts);
//[-start-121105-IB03780470-modify]//
  if (PchSeries == PchLp) {
    //
    // Gpe0Sts  : Clear status GPE0_STS[127:96](PME, PCIE, TCO...etc)
    // Gpe0Sts2 : Clear status GPE0_STS[94:64](GPI[94:64])
    // Gpe0Sts3 : Clear status GPE0_STS[63:32](GPI[63:32])
    // Gpe0Sts4 : Clear status GPE0_STS[31:0](GPI[31:0])
    //
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96, Gpe0Sts);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x04, Gpe0Sts2);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x08, Gpe0Sts3);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96 - 0x0C, Gpe0Sts4);
  } else {
    //
    // Gpe0aSts : Clear status GPE0_STS[31:0](PME, PCIE, TCO, GPI[15:0]...etc)
    // Gpe0bSts : Clear status GPE0_STS[63:32]
    //
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0a_STS, Gpe0aSts);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0b_STS, Gpe0bSts);
  }
//[-end-121105-IB03780470-modify]//
  IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_SMI_STS, SmiSts);
//[-end-121008-IB11410008-modify]//

  //
  // Clear Power Failure (PWR_FLR), It was used to check S3_Resume State.
  //
  PchLpcPciCfg8Or (R_PCH_LPC_GEN_PMCON_3, B_PCH_LPC_GEN_PMCON_PWR_FLR);

  //
  // Clear SLP_S4 state which is used to detect a timing violation during reset.
  //
  PchLpcPciCfg8Or (R_PCH_LPC_GEN_PMCON_2, B_PCH_LPC_GEN_PMCON_MIN_SLP_S4);

  //
  // Clear EHCI register EHCI_LEGEXT_CS
  //
  Value = B_PCH_EHCI_LEGEXT_CS_SMIBAR | B_PCH_EHCI_LEGEXT_CS_SMIPCI | B_PCH_EHCI_LEGEXT_CS_SMIOS;

  Base = MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB, PCI_FUNCTION_NUMBER_PCH_EHCI, 0);
  MmioWrite32 (Base + R_PCH_EHCI_LEGEXT_CS, Value);

//[-start-121105-IB03780471-add]//
  if (PchSeries == PchH) {
    Base = MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB_EXT, PCI_FUNCTION_NUMBER_PCH_EHCI2, 0);
    MmioWrite32 (Base + R_PCH_EHCI_LEGEXT_CS, Value);
  }
//[-end-121105-IB03780471-add]//

  return EFI_SUCCESS;
}

/**
 set VT capability.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         SystemConfiguration A pointer to setup variables.
 @param[in]         IsVtSupport         VT support status

 @retval            No Status Return.
*/
STATIC
VOID
PeiVTExecute (
  IN CONST  EFI_PEI_SERVICES       **PeiServices,
  IN        CHIPSET_CONFIGURATION    *SystemConfiguration,
  IN        BOOLEAN                  IsVtSupport
  )
{
  UINT32                        IA32FeatureCntrl;
  UINT32                        Data32;

  IA32FeatureCntrl = AsmReadMsr32 ( MSR_IA32_FEATURE_CONTROL );
  if (((SystemConfiguration->VTSupport) && (IsVtSupport)) && (!((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_EVT) >> 2)) \
      || ((!((SystemConfiguration->VTSupport) && (IsVtSupport))) & ((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_EVT)>> 2)) 
     ) {
    if (((UINT32)AsmReadMsr32 ( MSR_IA32_FEATURE_CONTROL )) & B_MSR_IA32_FEATURE_CONTROL_LOCK) {
//[-start-120731-IB10820094-modify]//
      Data32 = IoRead32(PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GP_RST_SEL);
//[-end-120731-IB10820094-modify]//
      Data32 |= BIT30;
//[-start-121109-IB05280008-modify]//
      IoWrite32 ((UINT32)(PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GP_RST_SEL), Data32);
//[-end-121109-IB05280008-modify]//

      PchLpcPciCfg32Or (R_PCH_LPC_PMIR, B_PCH_LPC_PMIR_CF9GR);
      IoWrite32 (R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
    } else {
      if ((SystemConfiguration->VTSupport) && (IsVtSupport)) {
        IA32FeatureCntrl |= B_MSR_IA32_FEATURE_CONTROL_EVT;
      } else {
        IA32FeatureCntrl &= ~B_MSR_IA32_FEATURE_CONTROL_EVT;
      }
      AsmWriteMsr32 ( MSR_IA32_FEATURE_CONTROL, IA32FeatureCntrl );
    }
  }

  return;
}

/**
 A callback function is triggered by gPeiS3RestoreAcpiCallbackPpiGuid PPI installation.

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         Ppi                 A pointer to S3RestoreAcpiCallback PPI

 @retval            EFI_SUCCESS         Procedure complete.
*/
STATIC
EFI_STATUS
S3RestoreAcpiNotifyCallback (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  )
{
  EFI_STATUS                     Status;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdTXTSupported)) {
    Status = PeiServicesInstallPpi (&mPeiBootScriptDonePpi);
    ASSERT_EFI_ERROR (Status);
  }

//[-start-121214-IB07250300-modify]//
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    CloseHdAudio ((CONST EFI_PEI_SERVICES **)PeiServices);
  }
//[-end-121214-IB07250300-modify]//
//[-end-120731-IB10820094-modify]//

  return EFI_SUCCESS;
}

//[-start-121214-IB07250300-modify]//
/**
 Close HD Audio device expect S3/S4.
 When S3/S4 resume, check SG variable "OptimusFlag" to on/off HD audio device.
 Bus 1 Dev 0 Fun 0 Reg 488 Bit 25 is HD audio device power enable bit.

 @param [in]   PeiServices      General purpose services available to every PEIM.

 @retval None.

**/
STATIC
VOID
CloseHdAudio (
  IN CONST EFI_PEI_SERVICES                   **PeiServices
  )
{
  EFI_BOOT_MODE                               BootMode;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI             *VariableServices;
  EFI_STATUS                                  Status;
  SG_VARIABLE_CONFIGURATION                   SgData;
  UINT8                                       HdAudioFlag;
  UINT8                                       PegBus;
  UINTN                                       VariableSize;

  //
  // DEVEN register bit 4 is Internal Graphics Engine (D2EN).
  //
  if ((MmPci8 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_DEVEN) & B_SA_DEVEN_D2EN_MASK) == 0) {
    return;
  }

  Status = PeiServicesLocatePpi (
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             (VOID **)&VariableServices
                             );
  if (EFI_ERROR (Status)) {
   	return;
  }

  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"SwitchableGraphicsVariable",
                               &gH2OSwitchableGraphicsVariableGuid,
                               NULL,
                               &VariableSize,
                               &SgData
                               );
  if (EFI_ERROR (Status)) {
    return;
  }
  PegBus = SgData.OptimusVariable.MasterDgpuBus;

  if (MmPci16 (0, PegBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, PCI_VID) == NVIDIA_VID) {
    Status = PeiServicesGetBootMode (&BootMode);
    if (EFI_ERROR (Status)) {
      return;
    }

    if ((BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_S4_RESUME)) {
      MmPci32And (0, PegBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER, ~(BIT25));
      return;
    }

    if (BootMode == BOOT_ON_S3_RESUME) {
      HdAudioFlag = SgData.OptimusVariable.OptimusFlag;
      if ((HdAudioFlag & BIT0) == 0) {
        MmPci32And (0, PegBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER, ~(BIT25));
      } else {
        MmPci32Or (0, PegBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER, BIT25);
      }
    }
  }
}
//[-end-121214-IB07250300-modify]//
static
EFI_PEI_NOTIFY_DESCRIPTOR mSetS3RestoreAcpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiS3RestoreAcpiCallbackPpiGuid,
  S3RestoreAcpiNotifyCallback
  };

/**
 This routine try to generate a hard reset.

 @param[in]         PeiServices         The PEI core services table.

 @retval            EFI_DEVICE_ERROR    Reset failed, return EFI_DEVICE_ERROR
*/
STATIC
EFI_STATUS
PchReset (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  //
  // The theory here is to write a 0x02, then a 0x06 to
  // RESET_GENERATOR_PORT to generate a hard reset.
  //
  EFI_PEI_CPU_IO_PPI  *CpuIo;

  CpuIo = (**PeiServices).CpuIo;

  CpuIo->IoWrite8 (
          PeiServices,
          CpuIo,
          R_PCH_RST_CNT,
          V_PCH_RST_CNT_HARDSTARTSTATE
          );

  CpuIo->IoWrite8 (
          PeiServices,
          CpuIo,
          R_PCH_RST_CNT,
          V_PCH_RST_CNT_HARDRESET
          );

  //
  // System should have reset at this point
  //
  ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);

  return EFI_DEVICE_ERROR;
}

static EFI_PEI_RESET_PPI                mResetPpi = { PchReset };
static EFI_PEI_PPI_DESCRIPTOR           mResetPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiResetPpiGuid,
    &mResetPpi
  }
};
  
/**
 Chipset initialization code in stage2 of PEI phase.

 @param[in]         None

 @retval            EFI_SUCCESS         Procedure complete.
*/
EFI_STATUS
PlatformStage2Init (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         CmosVmxSmxFlag = 0;
  UINT8                         TempCmosVmxSmxFlag;
  UINT32                        Data32 = 0;
  UINT32                        ChipsetIsTxtCapable = 0;
  UINT8                         TxtFlag;
  UINT8                         CmosPlatformSetting;
  UINT8                         CmosPlatformConfig;
  UINT8                         TempCmosPlatformConfig;
  BOOLEAN                       IsVtSupport;
  EFI_BOOT_MODE                 BootMode;
  UINT8                         RTCReg;
  RAPID_START_PPI               *RapidStartFlagPpi = NULL;
  CONST EFI_PEI_SERVICES             **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *VariableServices;
  UINTN                                VariableSize;
//[-start-130710-IB05160465-modify]//
  VOID                                 *SystemConfiguration;
//[-end-130710-IB05160465-modify]//
  EFI_GUID                             SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//[-start-121105-IB03780470-add]//
  PCH_SERIES                    PchSeries;
//[-end-121105-IB03780470-add]//

  PeiServices            = GetPeiServicesTablePointer ();
  CmosPlatformSetting    = 0;
  CmosPlatformConfig     = 0;
  TempCmosPlatformConfig = 0;
  Status = PeiServicesGetBootMode(&BootMode);

  Status = PeiServicesInstallPpi(&mResetPpiList[0]);
  ASSERT_EFI_ERROR (Status);
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdRapidStartSupported)) {
    Status = PeiServicesLocatePpi(&gRapidStartPpiGuid,0,NULL,(VOID **)&RapidStartFlagPpi);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-120731-IB10820094-modify]//
  //
  // Get Setup Variable
  //
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             (VOID **)&VariableServices
                             );
  ASSERT_EFI_ERROR (Status);
  
//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VariableSize);
//[-start-140623-IB05080432-modify]//
  if (SystemConfiguration == NULL) {
    DEBUG ((EFI_D_INFO, "Fail to allocate memory for SystemConfiguration!!!"));
    ASSERT (SystemConfiguration != NULL);
  } else {
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"Setup",
                                 &SystemConfigurationGuid,
                                 NULL,
                                 &VariableSize,
                                 SystemConfiguration
                                 );
  }
//[-end-140623-IB05080432-modify]//
//[-end-130709-IB05160465-modify]//
  
  //
  // Power Failure Consideration PPT BIOS SPEC 19.3
  //
  if ((PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_RTC_PWR_STS) == B_PCH_LPC_GEN_PMCON_RTC_PWR_STS) {
    //
    // When the RTC_PWR_STS is set
    // software should clear this bit. Then
    // 2. Set RTC Register 0Ah[6:4] to 110b or 111b
    // 3. Set RTC Register 0Bh[7] 
    // 4. Set RTC Register 0Ah[6:4] to 010b
    // 5. Clear RTC Register 0Bh[7]
    //
    PchLpcPciCfg8And (
        R_PCH_LPC_GEN_PMCON_3, 
        (UINT8)~B_PCH_LPC_GEN_PMCON_RTC_PWR_STS
        );
    RTCReg = ReadCmos8 (R_PCH_RTC_REGA);
    RTCReg &= ~(B_PCH_RTC_REGB_PIE | B_PCH_RTC_REGB_AIE |B_PCH_RTC_REGB_UIE);
    RTCReg |= B_PCH_RTC_REGB_PIE | B_PCH_RTC_REGB_AIE |B_PCH_RTC_REGB_UIE;  
    WriteCmos8 (R_PCH_RTC_REGA, RTCReg);
    
    RTCReg = ReadCmos8 (R_PCH_RTC_REGB);
    RTCReg |= B_PCH_RTC_REGB_SET;
    WriteCmos8 (R_PCH_RTC_REGB, RTCReg);
    
    RTCReg = ReadCmos8 (R_PCH_RTC_REGA);
    RTCReg &= ~(B_PCH_RTC_REGB_PIE | B_PCH_RTC_REGB_AIE |B_PCH_RTC_REGB_UIE);
    RTCReg |= B_PCH_RTC_REGB_AIE;  
    WriteCmos8 (R_PCH_RTC_REGA, RTCReg);

    RTCReg = ReadCmos8 (R_PCH_RTC_REGB);
    RTCReg &= ~B_PCH_RTC_REGB_SET;
    WriteCmos8 (R_PCH_RTC_REGB, RTCReg);
  }

//[-start-130710-IB05160465-modify]//
//[-start-140623-IB05080432-modify]//
  if ((SystemConfiguration != NULL) &&
      (((CHIPSET_CONFIGURATION *)SystemConfiguration)->VTSupport)) {
//[-end-140623-IB05080432-modify]//
//[-end-130710-IB05160465-modify]//
    CmosVmxSmxFlag = CmosVmxSmxFlag | B_VMX_SETUP_FLAG;
  }

  IsVtSupport = IsVmxSupported ();
  if (IsVtSupport) {
    CmosVmxSmxFlag = CmosVmxSmxFlag | B_VMX_CPU_FLAG;
  }
  
  //
  // TXT feature initialization in PEI.
  //
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdTXTSupported)) {
//[-start-130710-IB05160465-modify]//
//[-start-140623-IB05080432-modify]//
    if (SystemConfiguration!= NULL) {
      PeiTxtExecute (PeiServices, SystemConfiguration, IsVtSupport, &CmosVmxSmxFlag);
    } else {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Get Setup value fail, skip running PeiTxtExecute function!\n"));
    }
//[-end-140623-IB05080432-modify]//
//[-end-130710-IB05160465-modify]//
  } else {
//[-start-130710-IB05160465-modify]//
//[-start-140623-IB05080432-modify]//
    if (SystemConfiguration != NULL) {
      PeiVTExecute (PeiServices, SystemConfiguration, IsVtSupport);
    } else {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Get Setup value fail, skip running PeiVTExecute function!\n"));
    }
//[-end-140623-IB05080432-modify]//
//[-end-130710-IB05160465-modify]//
    //
    // Check PCH TXT capability
    //
    Data32 = MmioRead32 (PcdGet32 (PcdTxtPublicBase) + 0x10);
    if (Data32 & BIT0) {
      ChipsetIsTxtCapable = TRUE;
    } else {
      ChipsetIsTxtCapable = FALSE;
    }

    Status = PeiServicesGetBootMode (&BootMode);

    if ((ChipsetIsTxtCapable == 0) && (BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_S4_RESUME)) {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Warm boot or S3 !\n"));
    
      TxtFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag);
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Read TXT Flag By CMOS: %x\n", TxtFlag));

      if (TxtFlag & B_SMX_CHIPSET_FLAG){
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "ChipsetIsTxtCapable == TURE\n"));
        ChipsetIsTxtCapable = TRUE;
      }
    }

    if (ChipsetIsTxtCapable) {
      CmosVmxSmxFlag = CmosVmxSmxFlag | B_SMX_CHIPSET_FLAG;
    }

    TempCmosVmxSmxFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag);
    if (CmosVmxSmxFlag != TempCmosVmxSmxFlag) {
      WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag, CmosVmxSmxFlag);
    }
  }
//[-end-120731-IB10820094-modify]//
  TempCmosPlatformConfig = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, PlatformConfigFlag);
  CmosPlatformConfig     = TempCmosPlatformConfig;
//[-start-120404-IB05300309-modify]//
  if ( MmPci16 (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_VID) == 0xFFFF ) {
    CmosPlatformConfig = CmosPlatformConfig & ~B_CONFIG_IGD_PRESENT_FLAG;
  } else {
    CmosPlatformConfig = CmosPlatformConfig | B_CONFIG_IGD_PRESENT_FLAG;
  }
//[-end-120404-IB05300309-modify]//
  if ( CmosPlatformConfig != TempCmosPlatformConfig ) {
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, PlatformConfigFlag, CmosPlatformConfig);

    CmosPlatformSetting = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag);
    CmosPlatformSetting = CmosPlatformSetting | B_SETTING_MEM_REFRESH_FLAG;
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag, CmosPlatformSetting);

    IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
    CpuDeadLoop ();
  }

//[-start-121008-IB11410008-modify]//
  IoWrite16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_EN, 0);
//[-start-121105-IB03780470-modify]//
  PchSeries = GetPchSeries ();
  if (PchSeries == PchLp) {
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_EN_127_96, 0x00040000);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_EN_127_96 - 0x04, 0);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_EN_127_96 - 0x08, 0);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0_EN_127_96 - 0x0C, 0);
  } else {
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0a_EN, 0);
    IoWrite32 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_GPE0a_EN + 4, 0);
  }
//[-end-121105-IB03780470-modify]//
//[-end-121008-IB11410008-modify]//

//[-start-120705-IB05330352-add]//
  SetFlashType (FLASH_DEVICE_TYPE_SPI);
//[-end-120705-IB05330352-add]//

//[-start-120316-IB06460376-remove]//
//  if (SystemConfiguration->RmhMode) {
//    //
//    // According to RMH setting in setup menu Enable RMH
//    //
//    PchMmRcrb32And (0x3598, (UINT32)(~BIT0));
//  } else {
//    //
//    // Disable RMH
//    //
//    PchMmRcrb32Or (0x3598, (UINT32)(BIT0));
//  }
//[-end-120316-IB06460376-remove]//


//[-start-121201-IB03780468-modify]//
  Status = PeiServicesNotifyPpi (mNotifyDesc);
  ASSERT_EFI_ERROR (Status);
//[-end-121201-IB03780468-modify]//

//[-start-120412-IB06460388-add]//
  if (FeaturePcdGet (PcdRapidStartSupported)) {
    if (RapidStartFlagPpi->IsEnabled (RapidStartFlagPpi)) {
      Status = PeiServicesNotifyPpi (&mMemoryDisCoverNotifyDispatchList[0]);
      ASSERT_EFI_ERROR (Status);
    }
  }
//[-end-120412-IB06460388-add]//

//
//when RapidStart exit status , also need install it  and treat it as S3 resume in this code at this moment 
//
  if (FeaturePcdGet(PcdRapidStartSupported)) {
    if ((BootMode == BOOT_ON_S3_RESUME) || (RapidStartFlagPpi->IsEnabled(RapidStartFlagPpi) && RapidStartFlagPpi->GetMode(RapidStartFlagPpi) == RapidStartExit)) {
      Status = PeiServicesNotifyPpi(&mSetS3RestoreAcpiNotify);
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  } else {
    if ((BootMode == BOOT_ON_S3_RESUME)) {
      Status = PeiServicesNotifyPpi(&mSetS3RestoreAcpiNotify);
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }
//[-end-120731-IB10820094-modify]//
  //
  // Clear all pending SMI. On S3 clear power button enable so it wll not generate an SMI
  //
  ClearPchSmiAndWake (PeiServices);

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    Status = PeiServicesNotifyPpi (&mRecoveryNotifyList[0]);
    ASSERT_EFI_ERROR (Status);
    Status = PeiServicesNotifyPpi (&mResetEHCI[0]);
    ASSERT_EFI_ERROR (Status);
//[-start-121012-IB06460456-remove]//
//
// Remove this unnecessary SATA reset, because this SATA reset was used when kerenl did not support AHCI PEIM.
//
//    Status = PeiServicesNotifyPpi(&mResetSata[0]);
//    ASSERT_EFI_ERROR (Status);
//[-end-121012-IB06460456-remove]//
  }

  return EFI_SUCCESS;
}

//[-start-120412-IB06460388-add]//
/**
 Provide a callback when the MemoryDiscovered PPI is installed.

 @param             PeiServices         The PEI core services table.
 @param             NotifyDescriptor    The descriptor for the notification event.
 @param             Ppi                 Pointer to the PPI in question.

 @retval            EFI_SUCCESS         The function is successfully processed.

*/
EFI_STATUS
RestoreAcpiVariableHob (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_BOOT_MODE                         BootMode;
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *Variable;
  ACPI_VARIABLE_SET                     *AcpiVariableSetHob = NULL;
  ACPI_VARIABLE_SET                     *AcpiVariableSet = NULL;
  UINT64                                AcpiVariableSet64 = 0;
  UINTN                                 VarSize;
  EFI_PEI_HOB_POINTERS                  Hob;
  RAPID_START_PPI                       *RapidStartFlagPpi = NULL;
  //
  // ReProduce ACPI S3 hob when boot path is S3
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
//[-start-130823-IB05160482-modify]//
  if (FeaturePcdGet(PcdRapidStartSupported)) {
    Status = PeiServicesLocatePpi(&gRapidStartPpiGuid,0,NULL,(VOID **)&RapidStartFlagPpi);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-130823-IB05160482-modify]//
  //
  // S3 resume and RST resume do this
  //
//[-start-130823-IB05160482-modify]//
  if (BootMode == BOOT_ON_S3_RESUME && (!FeaturePcdGet(PcdRapidStartSupported) || ( RapidStartFlagPpi->GetMode(RapidStartFlagPpi) == RapidStartExit)) ) {
//[-end-130823-IB05160482-modify]//
    //
    // Initial PEI Variable service.
    //
    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid,
               0,
               NULL,
               (VOID **)&Variable
               );
    ASSERT_EFI_ERROR (Status);

    VarSize = sizeof (AcpiVariableSet64);

    Status = Variable->GetVariable (
                         Variable,
                         ACPI_GLOBAL_VARIABLE,
                         &gAcpiVariableSetGuid,
                         NULL,
                         &VarSize,
                         &AcpiVariableSet64
                         );
    ASSERT_EFI_ERROR (Status);
    AcpiVariableSet = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;
    //
    // find Acpivariable hob and udpate it again
    //
    Status = PeiServicesGetHobList ((VOID **)&Hob.Raw);
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && CompareGuid (&Hob.Guid->Name, &gAcpiVariableSetGuid)) {
        AcpiVariableSetHob = (ACPI_VARIABLE_SET *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
        break;
      }

      Hob.Raw = GET_NEXT_HOB (Hob);
    }

    if (AcpiVariableSetHob == NULL) {
      ASSERT_EFI_ERROR(FALSE);
      return EFI_NOT_FOUND;
    }
    //
    // update Acpivariable hob from AcpiVariable variable 
    //
    CopyMem( AcpiVariableSetHob,AcpiVariableSet,sizeof(ACPI_VARIABLE_SET) );

  }

  return EFI_SUCCESS;
}
//[-end-120412-IB06460388-add]//
//[-start-120704-IB05330352-add]//
EFI_STATUS
EFIAPI
SetFlashType (
  IN UINT32         FlashType
  )
{
  PCH_SERIES PchSeries;
  PchSeries = GetPchSeries();
  ///
  /// Set the requested state
  ///
  switch (FlashType) {

  case FLASH_DEVICE_TYPE_SPI:
    if (PchSeries == PchH) {
      MmioAndThenOr16 (
        (UINTN) (PcdGet32 (PcdRcbaBaseAddress)+ R_PCH_RCRB_GCS),
        (UINT16) (~B_PCH_H_RCRB_GCS_BBS),
        (UINT16) (V_PCH_H_RCRB_GCS_BBS_SPI)
        );
    }
    if (PchSeries == PchLp) {
      MmioAndThenOr16 (
        (UINTN) (PcdGet32 (PcdRcbaBaseAddress) + R_PCH_RCRB_GCS),
        (UINT16) (~B_PCH_LP_RCRB_GCS_BBS),
        (UINT16) (V_PCH_LP_RCRB_GCS_BBS_SPI)
        );
    }
    break;
  case FLASH_DEVICE_TYPE_LPC:
    if (PchSeries == PchH) {
      MmioAndThenOr16 (
        (UINTN) (PcdGet32 (PcdRcbaBaseAddress) + R_PCH_RCRB_GCS),
        (UINT16) (~B_PCH_H_RCRB_GCS_BBS),
        (UINT16) (V_PCH_H_RCRB_GCS_BBS_LPC)
        );
    }
    if (PchSeries == PchLp) {
      MmioAndThenOr16 (
        (UINTN) (PcdGet32 (PcdRcbaBaseAddress) + R_PCH_RCRB_GCS),
        (UINT16) (~B_PCH_LP_RCRB_GCS_BBS),
        (UINT16) (V_PCH_LP_RCRB_GCS_BBS_LPC)
        );
    }
    break;

  default:
    ///
    /// This is an invalid use of the protocol
    /// See definition, but caller must call with valid value
    ///
    ASSERT (!EFI_UNSUPPORTED);
    break;
  }
  return EFI_SUCCESS;
}
//[-end-120704-IB05330352-add]//
