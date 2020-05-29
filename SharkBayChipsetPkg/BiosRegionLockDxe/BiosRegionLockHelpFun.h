/** @file
 
;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _BIOS_REGION_LOCK_HELP_FUN_H_
#define _BIOS_REGION_LOCK_HELP_FUN_H_

//EFI_STATUS
//GetBiosRegionAddress (
//  IN  BIOS_REGION_TYPE   Type,
//  OUT UINTN             *Base,
//  OUT UINTN             *Length
//);

/**
 This function merges the requested BIOS region to the private protected BIOS region record.

 @param[in]         BiosLock            Private protected BIOS region record.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be merged.
 @param[in]         Length              The Length of the BIOS region which need to be merged.

 @retval EFI_OUT_OF_RESOURCES The max number of BIOS protect regions have been reached and the requested region 
                              can not be merge to existing protected region.
 @retval EFI_SUCCESS          Merge successfully
*/
EFI_STATUS
MergeToBase (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
);


/**
 This function removes the requested BIOS region from the private protected BIOS region record.

 @param[in]         BiosLock            Private protected BIOS region record.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be removed.
 @param[in]         Length              The Length of the BIOS region which need to be removed.

 @retval EFI_OUT_OF_RESOURCES The BIOS protect region registers are not enough to set for all discontinuous BIOS region
 @retval EFI_SUCCESS          Remove successfully
*/
EFI_STATUS
RemoveFromBase (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
);


/**
 This function programs the BIOS regions described in BiosLock record to the BIOS protect region registers.

 @param[in]         BiosLock            Private protected BIOS region record.

 @retval EFI_SUCCESS
*/
EFI_STATUS
ProgramRegister (
  IN BIOS_REGION_LOCK_BASE *BiosLock
);

#endif //_BIOS_REGION_LOCK_HELP_FUN_H_

