/** @file
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcChipsetLib.h>

/**
 This function offers an interface to modify AZALIA_POLICY data before the system 
 installs AZALIA_POLICY.

 @param[in, out]   *PchAzaliaPolicyPpi            On entry, points to AZALIA_POLICY structure.
                                                  On exit, points to updated AZALIA_POLICY structure.
 @param[in, out]   *PchAzaliaPolicyPpiDesc        On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateAzaliaPolicy (
  IN OUT AZALIA_POLICY                  *PchAzaliaPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR         *PchAzaliaPolicyPpiDesc
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
