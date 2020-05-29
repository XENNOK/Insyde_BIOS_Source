/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcSetIsaInfo().
 The function DxeCsSvcSetIsaInfo() use chipset services to set ISA
 information for common ISA driver utilization.
	
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

/**
 To set ISA information for common ISA driver utilization.

 @param[in, out]    Function            ISA function value
 @param[in, out]    Device              ISA device value

 @retval            EFI_SUCCESS         This function always return successfully.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcSetIsaInfo (
  IN OUT UINT8    *Function,
  IN OUT UINT8    *Device
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, SetIsaInfo) + sizeof(VOID*)) || 
      mChipsetSvc->SetIsaInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member SetIsaInfo() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->SetIsaInfo (Function, Device);   
}  
