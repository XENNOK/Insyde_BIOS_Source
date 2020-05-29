/** @file
  Provide hook function for OEM to implement EC ACPI Mode. 

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

#include <Library/BaseOemSvcKernelLib.h>
#include <Library/CommonEcLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <OemEc.h>

/**
  Platform specific function to enable/disable EC ACPI Mode

  @param[in]      EnableEcAcpiMode      Boolean to enable/disable ACPI mode.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
**/
EFI_STATUS
OemSvcEcAcpiMode (
  IN  BOOLEAN         EnableEcAcpiMode
  )
{
  EFI_STATUS                            Status;
  UINT8                                 EcCommand;
  
  if (EnableEcAcpiMode) {
    EcCommand = EC_ACPI_MODE_EN_CMD;    
  } else {
    EcCommand = EC_ACPI_MODE_DIS_CMD;
  }
  
  Status = WaitKbcIbe (SMC_CMD_STATE);
  ASSERT_EFI_ERROR(Status);
  
  IoWrite8 (SMC_CMD_STATE, EcCommand);
  
  Status = WaitKbcIbe (SMC_CMD_STATE);
  ASSERT_EFI_ERROR(Status);

  return EFI_UNSUPPORTED;
}
