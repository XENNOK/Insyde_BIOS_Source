/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcIoDecodeControl().
 The function DxeCsSvcIoDecodeControl() use chipset services to set
 IO decode region.
	
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
#include <Library/DxeChipsetSvcLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

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
 @retval            EFI_UNSUPPORTED       If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcIoDecodeControl (
  IN IO_DECODE_TYPE            Type,
  IN IO_DECODE_ADDRESS         Address,
  IN UINT16                    Length
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, IoDecodeControl) + sizeof(VOID*)) || 
      mChipsetSvc->IoDecodeControl == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member IoDecodeControl() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->IoDecodeControl (Type, Address, Length);   
}  
