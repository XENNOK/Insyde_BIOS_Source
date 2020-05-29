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
//[-start-130424-IB05400400-add]//
#include <Library/PcdLib.h>
#include <OemBoardId.h>
//[-end-130424-IB05400400-add]//

/**
 This function offers an interface to modify SA_PLATFORM_POLICY_PPI data before the system 
 installs SA_PLATFORM_POLICY_PPI.

 @param[in, out]   *SaPlatformPolicyPpi           On entry, points to SA_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated SA_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *SaPlatformPolicyPpiDesc       On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateSaPlatformPolicy (
  IN OUT SA_PLATFORM_POLICY_PPI                   *SaPlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *SaPlatformPolicyPpiDesc
  )
{
//[-start-130424-IB05400400-add]//
  switch (PcdGet8 (PcdCurrentBoardId)) {
  case V_EC_BOARD_ID_HADDOCK_CREEK:
    SaPlatformPolicyPpi->PlatformData->SpdAddressTable[0] = 0xA2;
    SaPlatformPolicyPpi->PlatformData->SpdAddressTable[1] = 0xA0;
    SaPlatformPolicyPpi->PlatformData->SpdAddressTable[2] = 0xA6;
    SaPlatformPolicyPpi->PlatformData->SpdAddressTable[3] = 0xA4;
    break;
  };
//[-end-130424-IB05400400-add]//

  return EFI_UNSUPPORTED;
}
