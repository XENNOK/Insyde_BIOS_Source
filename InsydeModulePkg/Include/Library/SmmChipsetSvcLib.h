/**
 SMM Chipset Services Library.
 
  This file provides the prototype of all SMM Chipset Services Library function.

***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _SMM_CHIPSET_SVC_LIB_H_
#define _SMM_CHIPSET_SVC_LIB_H_
#include <Uefi.h>
#include <ChipsetSvc.h>
  
/**
 Platform specific function to enable/disable flash device write access. 

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
SmmCsSvcEnableFdWrites (
  IN  BOOLEAN           EnableWrites
  );

/**
 Legacy Region Access Control. 
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                If LEGACY_REGION_ACCESS_LOCK, then
                                        LegacyRegionAccessCtrl() lock the specific
                                        legacy region.
                    
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED       If the function is not implemented.
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
EFI_STATUS
SmmCsSvcLegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  );

/**
 Resets the system.

 @param[in]         ResetType           The type of reset to perform.
                                
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     1. If the function is not implemented.
                                        2. ResetType is invalid.
 @return            others              Error occurs
*/
EFI_STATUS
EFIAPI
SmmCsSvcResetSystem (
  IN EFI_RESET_TYPE   ResetType
  );

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
SmmCsSvcSataComReset (
  IN  UINTN         PortNumber
  );

#endif
