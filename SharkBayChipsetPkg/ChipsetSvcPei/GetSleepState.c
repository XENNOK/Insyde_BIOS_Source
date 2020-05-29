/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcGetSleepState().
 The function PeiCsSvcGetSleepState() use chipset services to identify
 sleep state.

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
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/CmosLib.h>
#include <Library/PeiServicesLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <Ppi/CpuIo.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>
#include <ChipsetCmos.h>

/**
 Query ICH2 to determine if S3 resume
 
 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         CpuIo               A pointer to CPU IO Services PPI.

 @retval            TRUE                This is an S3 restart
 @retval            FALSE               This is not an S3 restart
*/
STATIC
BOOLEAN
IsS3Wakeup (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_CPU_IO_PPI                 *CpuIo
  )
{
  UINT16                            Pm1Sts;
  UINT16                            Pm1Cnt;
  UINT8                             RapidStartFlag = 0;
  EFI_STATUS                        Status;
//[-start-120604-IB06150223-modify]//
  UINT8                             ISCTEnable = 0;
//[-end-120604-IB06150223-modify]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  UINTN                             VariableSize;
  VOID                              *SystemConfiguration;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  //
  // Locate Variable Ppi
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariableServices
             );
  ASSERT_EFI_ERROR ( Status );

//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VariableSize);
  
//[-start-140625-IB05080432-modify]//
  if(SystemConfiguration != NULL) {

    //
    // Get Setup Variable
    //
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"Setup",
                                 &SystemConfigurationGuid,
                                 NULL,
                                 &VariableSize,
                                 SystemConfiguration
                                 );
    if (!EFI_ERROR (Status)) {
//[-start-120604-IB06150223-modify]//
      ISCTEnable = ((CHIPSET_CONFIGURATION *)SystemConfiguration)->ISCTEnable;
//[-end-120604-IB06150223-modify]//
    }
//[-end-130709-IB05160465-modify]//
  }
//[-end-140625-IB05080432-modify]//

  //
  // Read the ACPI registers
  //
  Pm1Sts = CpuIo->IoRead16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS);
  Pm1Cnt = CpuIo->IoRead16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT);

  //
  // No power failure while S3_Resume, and only enter S3 boot mode once after OS standby.
  //
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0 && (Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S3) {
//[-start-120731-IB10820094-modify]//
    if (FeaturePcdGet(PcdRapidStartSupported)) {
      RapidStartFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, FFS_NV_CONFIG_REG);
      if (RapidStartFlag != 0) {
        //
        // Do not turn on display devices during RapidStart S3 resume.
        //
        return TRUE;
      }
    }
//[-end-120731-IB10820094-modify]//
//[-start-120604-IB06150223-modify]//
    if (ISCTEnable == 1) {
      //
      // Skip turning on display devices when iSCT is enabled.
      //
      return TRUE;
    }
//[-end-120604-IB06150223-modify]//
    //
    // turn on screen
    //
    Pm1Cnt  = CpuIo->IoRead16 (PeiServices, CpuIo,  PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GPIO_GPI_INV);
    Pm1Cnt  &= ~BIT14;
    CpuIo->IoWrite16 (PeiServices, CpuIo,  PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GPIO_GPI_INV, Pm1Cnt);
    return  TRUE;
  }
  return  FALSE;
}

/**
 Query ICH2 to determine if S4 resume

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         CpuIo               A pointer to CPU IO Services PPI.

 @retval            TRUE                This is an S4 restart
 @retval            FALSE               This is not an S4 restart

**/
STATIC
BOOLEAN
IsS4Wakeup (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI     *CpuIo
  )
{
  UINT16               Pm1Sts;
  UINT16               Pm1Cnt;
  
  //
  // Read the ACPI registers
  //
  Pm1Sts  = CpuIo->IoRead16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS);
  Pm1Cnt  = CpuIo->IoRead16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT);
  
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0 && (Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S4) {
    return  TRUE;
  } else {
    return  FALSE;
  }
}

/**
 To identify sleep state.

 @param[in, out]    SleepState          3 - This is an S3 restart
                                        4 - This is an S4 restart

 @retval            EFI_SUCCESS         Function always returns successfully
*/
EFI_STATUS
GetSleepState (
  IN OUT  UINT8              *SleepState
  )
{
  CONST EFI_PEI_SERVICES  **PeiServices;
  
  PeiServices = GetPeiServicesTablePointer ();
  
  if (IsS3Wakeup (PeiServices, (**PeiServices).CpuIo)) {
    *SleepState = 3;
  } else if (IsS4Wakeup (PeiServices, (**PeiServices).CpuIo)) {
    *SleepState = 4;
  }
  return EFI_SUCCESS;
}
