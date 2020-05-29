/** @file

  Provide Wake Reason for ISCT.
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include <PchAccess.h>
#include <ChipsetSetupConfig.h>
#include <KscLib.h>

#include <Library/PchPlatformLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//

#include <Guid/IsctPersistentData.h>

#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/EndOfPeiPhase.h>


#define KSC_GETWAKE_STATUS    0x35
#define KSC_CLEARWAKE_STATUS  0x36

EFI_STATUS
EFIAPI
IsctGetWakeReason (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDesc,
  IN VOID                       *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mIsctGetWakeReasonNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
//[-start-121130-IB10920002-modify]//
//[-Start-121002-IB10820131-modify]//
// &gEfiPeiEndOfPeiPhasePpiGuid, //&gPeiMemoryDiscoveredPpiGuid, //problem encountered with FFS used this to fix: &gEfiPeiEndOfPeiPhasePpiGuid,
  &gEfiEndOfPeiSignalPpiGuid,
//  &gEfiPeiMemoryDiscoveredPpiGuid,
//[-end-121002-IB10820131-modify]//
//[-end-121130-IB10920002-modify]//
  IsctGetWakeReason
};

/**
  Get system Wake Reason and save into CMOS 72/73 for ACPI ASL to use. 

  @param[in]  PeiServices   General purpose services available to every PEIM.

  @retval  None
**/
EFI_STATUS
EFIAPI
IsctGetWakeReason (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDesc,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                  Status;
  UINT32                      PmBase;  
  UINT16                      PM1STS;
  UINT16                      USB29VID;
  UINT16                      USB29STS;
  UINT16                      USB26VID;
  UINT16                      USB26STS;
  UINT16                      xHCIVID;
  UINT16                      xHCISTS;
  UINT16                      LanVID;
  UINT16                      LanSTS;
  UINT16                      PCIeRtVID[8];
  UINT16                      PCIeRtSTS[8];
  UINT8                       WakeReason;
  UINT8                       Index;
  UINT8                       KscStatus;
  UINT8                       WakeStatus;
  UINTN                       Count;
  UINTN                       Size;
  ISCT_PERSISTENT_DATA        IsctData;
  EFI_PEI_READ_ONLY_VARIABLE_PPI  *ReadOnlyVariable;
  EFI_GUID                    IsctPersistentDataGuid = ISCT_PERSISTENT_DATA_GUID;
  UINT8                       *IsctNvsPtr;
  PCH_SERIES                  PchSeries;

  //
  // Locate PEI Read Only Variable PPI.
  //
  Status = (*PeiServices)->LocatePpi (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            &gEfiPeiReadOnlyVariablePpiGuid,
                            0,
                            NULL,
                            (VOID **)&ReadOnlyVariable
                            );
  ASSERT_EFI_ERROR (Status);

  Size = sizeof (ISCT_PERSISTENT_DATA);
  Status = ReadOnlyVariable->PeiGetVariable (
                              PeiServices,
                              ISCT_PERSISTENT_DATA_NAME,
                              &IsctPersistentDataGuid,
                              NULL,
                              &Size,
                              &IsctData
                              );
  DEBUG ((EFI_D_INFO, "IsctPei: GetVariable for IsctData Status = %x \n", Status));                                
//[-start-121203-IB10920002-modify]//
//  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "IsctPei: Get IsctData failed!\n"));
    return Status;
  }
//[-end-121203-IB10920002-modify]//

  DEBUG ((EFI_D_INFO, "IsctPei: IsctNvsPtr = %x \n", IsctData.IsctNvsPtr));

  //
  // Clear Isct Wake Reason
  //
  DEBUG ((EFI_D_INFO, "IsctPei: Current Isct Wake Reason = %x \n", *(UINT8 *) IsctData.IsctNvsPtr));
  IsctNvsPtr  = (UINT8 *) IsctData.IsctNvsPtr;
  *IsctNvsPtr = 0;
  WakeReason = 0; 

  DEBUG ((EFI_D_INFO, "IsctPei: Address for Isct Nvs Region = %x \n", IsctNvsPtr));
  DEBUG ((EFI_D_INFO, "IsctPei: Timer Value saved for RTC timer = %x \n", *(UINT32 *)(IsctData.IsctNvsPtr + 0x8)));

  PchSeries = GetPchSeries();
  //
  // Initialize base address for Power Management 
  //
  PmBase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE)  & B_PCH_LPC_ACPI_BASE_BAR;

  PM1STS  = IoRead16(PmBase + R_PCH_ACPI_PM1_STS);
  PM1STS &= (B_PCH_ACPI_PM1_STS_PWRBTN | B_PCH_ACPI_PM1_STS_RTC | BIT14);

  //
  // Check PM1_STS
  //
  switch (PM1STS){
    case B_PCH_ACPI_PM1_STS_PWRBTN:
      WakeReason |= 0x01; //User event
      break;
    case B_PCH_ACPI_PM1_STS_RTC:
      WakeReason |= 0x04; //RTC Timer
      break;
    case BIT14:
      WakeReason |= 0x08; //Due to PME
      break;
    default:
      WakeReason = 0;
      break;
  }

  //
  // EHCI PME : Offset 0x54(15)
  //
  USB29VID = MmioRead16 (
             MmPciAddress (
               0,
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_USB,
               PCI_FUNCTION_NUMBER_PCH_EHCI,
               R_PCH_USB_VENDOR_ID
               ));

  USB29STS = MmioRead16 (
             MmPciAddress (
               0,
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_USB,
               PCI_FUNCTION_NUMBER_PCH_EHCI,
               R_PCH_EHCI_PWR_CNTL_STS
               )) & (B_PCH_EHCI_PWR_CNTL_STS_PME_STS | B_PCH_EHCI_PWR_CNTL_STS_PME_EN);

  if (USB29VID != 0xFFFF && USB29VID != 0){
    if (USB29STS == 0x8100){
      WakeReason |= 0x01; //User event
    }
  }

  if (PchSeries == PchH) {
    USB26VID = MmioRead16 (
                 MmPciAddress (
                   0,
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_USB_EXT,
                   PCI_FUNCTION_NUMBER_PCH_EHCI,
                   R_PCH_USB_VENDOR_ID
                 ));

    USB26STS = MmioRead16 (
                 MmPciAddress (
                   0,
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_USB_EXT,
                   PCI_FUNCTION_NUMBER_PCH_EHCI,
                   R_PCH_EHCI_PWR_CNTL_STS
                 )) & (B_PCH_EHCI_PWR_CNTL_STS_PME_STS | B_PCH_EHCI_PWR_CNTL_STS_PME_EN);

    if (USB26VID != 0xFFFF && USB26VID != 0){
      if (USB26STS == 0x8100){
        WakeReason |= 0x01; //User Event
      }
    }
  }

  //
  // Intel GigaLAN PME : Offset 0xCC(15)
  //
  LanVID = MmioRead16 (
           MmPciAddress (
             0,
             PCI_BUS_NUMBER_PCH_LAN,
             PCI_DEVICE_NUMBER_PCH_LAN,
             PCI_FUNCTION_NUMBER_PCH_LAN,
             R_PCH_LAN_VENDOR_ID
             ));

  LanSTS = MmioRead16 (
           MmPciAddress (
             0,
             PCI_BUS_NUMBER_PCH_LAN,
             PCI_DEVICE_NUMBER_PCH_LAN,
             PCI_FUNCTION_NUMBER_PCH_LAN,
             R_PCH_LAN_PMCS
             )) & (B_PCH_LAN_PMCS_PMES | B_PCH_LAN_PMCS_PMEE);

  
  if (LanVID != 0xFFFF && LanVID != 0){
     if (LanSTS == 0x8100){
        WakeReason |= 0x08; //PME
     }
  }

  //
  // xHCI PME : Offset 0x74(15)
  //
  xHCIVID = MmioRead16 (
           MmPciAddress (
             0,
             DEFAULT_PCI_BUS_NUMBER_PCH,
             PCI_DEVICE_NUMBER_PCH_XHCI,
             PCI_FUNCTION_NUMBER_PCH_XHCI,
             R_PCH_USB_VENDOR_ID
             ));

  xHCISTS = MmioRead16 (
           MmPciAddress (
             0,
             DEFAULT_PCI_BUS_NUMBER_PCH,
             PCI_DEVICE_NUMBER_PCH_XHCI,
             PCI_FUNCTION_NUMBER_PCH_XHCI,
             R_PCH_XHCI_PWR_CNTL_STS
             )) & (B_PCH_XHCI_PWR_CNTL_STS_PME_STS | B_PCH_XHCI_PWR_CNTL_STS_PME_EN);

     
  if (xHCIVID != 0xFFFF && xHCIVID != 0){
     if (xHCISTS == 0x8100){
        WakeReason |= 0x08; //PME
     }
  }

  //
  // PCIe Root PME : Offset 0xA4(15)
  //
  for (Index=0; Index <= PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8; Index++){
    PCIeRtVID[Index] = MmioRead16 (
              MmPciAddress (
                0,
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 + Index),
                R_PCH_PCIE_VENDOR_ID
                ));

    PCIeRtSTS[Index] = MmioRead16 (
              MmPciAddress (
                0,
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 + Index),
                R_PCH_PCIE_PMCS
                )) & (B_PCH_PCIE_PMCS_PMES | B_PCH_PCIE_PMCS_PMEE);
  
    if (PCIeRtVID[Index] != 0xFFFF && PCIeRtVID[Index] != 0){
       if (PCIeRtSTS[Index] == 0x8100){
          WakeReason |= 0x08; //PME
       }
    }
  }
  
  //
  // If the platform does not support EC Firmware, do not use it.
  // NOTE: when using desktop system, RTC timer is forced. 
  //
  
  //
  // Check if IsctTimerChoice is set to EC timer
  //
  if (*(UINT8 *)((UINTN)(IsctData.IsctNvsPtr + 0x3)) == 0) {
    DEBUG ((EFI_D_INFO, "IsctPei: EC timer is being used\n")); 
    //
    // Check KSC Input Buffer 
    //
    Count     = 0;
    KscStatus = IoRead8 (KSC_C_PORT);

    while (((KscStatus & KSC_S_IBF) != 0) && (Count < KSC_TIME_OUT)) {
      KscStatus = IoRead8 (KSC_C_PORT);
      Count++;
    }

    //
    // Send EC GetWakeStatus command
    //
    IoWrite8(KSC_C_PORT, KSC_GETWAKE_STATUS);

    //
    // Check KSC Output Buffer 
    //
    Count     = 0;
    KscStatus = IoRead8 (KSC_C_PORT);

    while (((KscStatus & KSC_S_OBF) == 0) && (Count < KSC_TIME_OUT)) {
      KscStatus = IoRead8 (KSC_C_PORT);
      Count++;
    }

    //
    // Receive wake reason
    //
    WakeStatus = IoRead8 (KSC_D_PORT);

    //
    // Check KSC Input Buffer 
    //
    Count     = 0;
    KscStatus = IoRead8 (KSC_C_PORT);

    while (((KscStatus & KSC_S_IBF) != 0) && (Count < KSC_TIME_OUT)) {
      KscStatus = IoRead8 (KSC_C_PORT);
      Count++;
    }

    //
    // Send EC ClearWakeStatus command
    //
    IoWrite8(KSC_C_PORT, KSC_CLEARWAKE_STATUS);

    switch (WakeStatus){
      case BIT1:  // Lid Wake
        WakeReason |= 0x01; //Bit0 is user event wake
        break;
      case BIT2:  // Keyboard/Mouse Wake
        WakeReason |= 0x01; //Bit0 is user event wake
        break;
      case BIT3: // EC Timer Wake
        WakeReason |= 0x02; //Bit1 is EC timer wake
        break;
      case BIT4: // PCIe Wake
        WakeReason |= 0x08; //Wake due to PME
        break;
      default: // Low Battery Wake
        WakeReason |= 0x00;
        break;
    }
    //
    // Override because of EC timer wake from FFS_S3 or S4 (Need EC support it!)
    //
    if ((PM1STS == B_PCH_ACPI_PM1_STS_PWRBTN) && (WakeStatus == BIT3)) {
      WakeReason = 0x02;
    }
  }
  //
  //If RTC wake, check if IsctOverWrite is set to OS
  //
  if (WakeReason == 0x04) {
    if (*(UINT8 *)((UINTN)(IsctData.IsctNvsPtr + 0x10)) == 0) { 
      WakeReason = 0x04;
    } else {
      WakeReason = 0x02;      
    }      
  }
 

  //
  // Set Isct Wake Reason
  //  
  DEBUG ((EFI_D_INFO, "IsctPei: Wake Reason = %x \n", WakeReason));                             
  *(UINT8 *)IsctData.IsctNvsPtr = WakeReason;                                  

  return EFI_SUCCESS;
}

/**
  Set up

  @param[in]  PeiServices  General purpose services available to every PEIM.

  @retval  None
**/
EFI_STATUS
EFIAPI
IsctPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                      Status;
  UINT8                           IsctEnabled;
  EFI_GUID                        SetupGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_READ_ONLY_VARIABLE_PPI  *ReadOnlyVariable;
//[-start-130710-IB05160465-modify]//
  VOID                            *SetupData;
  UINTN                           VariableSize;

  DEBUG ((EFI_D_INFO, "IsctPei Entry Point\n"));
  IsctEnabled = 0;

  //
  // Locate PEI Read Only Variable PPI.
  //
  Status = (**PeiServices).LocatePpi (
                              PeiServices,
                              &gEfiPeiReadOnlyVariablePpiGuid,
                              0,
                              NULL,
                              (VOID **)&ReadOnlyVariable
                              );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Setup Variable
  //
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SetupData = AllocateZeroPool (VariableSize);
//[-start-140625-IB05080432-add]//
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//

  Status = ReadOnlyVariable->PeiGetVariable (
                              (EFI_PEI_SERVICES **)PeiServices,
                              L"Setup",
                              &SetupGuid,
                              NULL,
                              &VariableSize,
                              SetupData
                              );
//[-start-140625-IB05080432-modify]//
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//[-end-140625-IB05080432-modify]//

  IsctEnabled = ((CHIPSET_CONFIGURATION *)SetupData)->ISCTEnable;
//[-end-130710-IB05160465-modify]//
  
  if (IsctEnabled == 0) {
    DEBUG ((EFI_D_INFO, "Isct Disabled\n"));
    return EFI_SUCCESS;
  }
  else {
    Status = PeiServicesNotifyPpi (&mIsctGetWakeReasonNotifyDesc);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "IsctPei: Notify  EFI_PEI_END_OF_PEI_PHASE_PPI_GUID Status = %x \n", Status));
  }
  
  return EFI_SUCCESS;
}
