/** @file
 PEI OEM IPMI Package library implement code - After Install Interface PPI.

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


#include <Library/PeiOemIpmiPackageLib.h>


/**
 Reserved for OEM to implement something additional requirements after PEI IPMI Interface PPI has been installed.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PPI structure.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiPeiAfterInstallInterface (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_INTERFACE_PPI       *Ipmi
  )
{
  return EFI_UNSUPPORTED;
}

