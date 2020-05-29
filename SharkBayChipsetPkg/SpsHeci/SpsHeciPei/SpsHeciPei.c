/** @file

  Framework PEIM to HECI.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

///
/// Statements that include other files
///
#include "SpsHeciPei.h"

#include <SpsHeciRegs.h>
#include <SpsChipset.h>

///
/// Function Declarations
///
static SPS_PEI_HECI_PPI      mHeciPpi = {
  HeciSendwAck,
  HeciReadMsg,
  HeciSendMsg,
  InitializeHeci,
  HeciGetMeStatus,
  HeciGetMeMode
};

static EFI_PEI_PPI_DESCRIPTOR mInstallHeciPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gSpsPeiHeciPpiGuid,
  &mHeciPpi
};

///
/// Function Implementations
///
/**
  Internal function performing Heci platform PPIs init needed in PEI phase

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        ME FPT is bad
**/
EFI_STATUS
EFIAPI
PeiInstallHeciPpi (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;

  ///
  /// Check for ME FPT Bad
  ///
  if ((SpsHeciPciRead32 (R_FWSTATE) & 0x0020) != 0) {

    return EFI_DEVICE_ERROR;
  }
  ///
  /// Initialize Heci platform PPIs
  /// Do not use EdkIIGlueLib here because PeiService pointer used in GlueLib
  /// is not updated after memory installed.
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, &mInstallHeciPpi);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "Install Heci Ppi Complete.\n"));
  return Status;
}

/**
  Internal function performing PM register initialization for Me

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
MePmInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  UINT16  PmBase;
  UINT16  Pm1Cnt;

  ///
  /// Before system memory initialization, BIOS should check the WAK_STS bit in PM1_STS[15] (PCH register PMBASE+00h)
  /// to determine if Intel Management Engine has reset the system while the host was in a sleep state. If WAK_STS is
  /// not set, BIOS should ensure a non-sleep exit path is taken. One way to accomplish this is by overwriting
  /// PM1_CNT[12:10] (PCH register PMBASE+04h) to 111b to force an S5 exit path by the BIOS.
  ///
  PmBase = PciRead16 (
            PCI_LIB_ADDRESS (SPS_PCI_BUS_NUMBER_PCH,
            SPS_PCI_DEVICE_NUMBER_PCH_LPC,
            SPS_PCI_FUNCTION_NUMBER_PCH_LPC,
            R_SPS_LPC_ACPI_BASE)
            ) & B_SPS_LPC_ACPI_BASE_BAR;

  ASSERT (PmBase != 0);

  if (PmBase != 0) {
    if ((IoRead16 (PmBase + R_SPS_ACPI_PM1_STS) & B_SPS_ACPI_PM1_STS_WAK) == 0) {
      Pm1Cnt = IoRead16 (PmBase + R_SPS_ACPI_PM1_CNT) | V_SPS_ACPI_PM1_CNT_S5;
      IoWrite16 (PmBase + R_SPS_ACPI_PM1_CNT, Pm1Cnt);
      DEBUG ((EFI_D_ERROR, "Force an S5 exit path.\n"));
    }
  }

  return EFI_SUCCESS;
}

/**
  Internal function performing Me initialization.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Heci initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
MeInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = MePmInit (PeiServices);
  return Status;
}

/**
  Initialize ME after reset

  @param[in] FileHandle           FileHandle  Handle of the file being invoked.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Heci initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PeimHeciInit (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;

  ///
  /// Performing Me initialization
  ///
  Status = MeInit (PeiServices);
  ASSERT_EFI_ERROR (Status);

  Status = PeiInstallHeciPpi (PeiServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) Failed to PeiInstallHeciPpi! Status = %r\n", Status));
  } else {
    DEBUG ((EFI_D_INFO, "HeciInit Complete.\n"));
  }

  return Status;
}
