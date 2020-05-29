/** @file
  Returns platform specific MP Table information to the caller.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  Returns platform specific MP Table information to the caller.

  @param[out]  *CharNumOfOemIdString      The characters number of OEM ID string.
  @param[out]  **pOemIdStringEntry        The address of mOemIdString[].
  
  @param[out]  *CharNumOfProductIdString  The characters number of Product ID string
  @param[out]  **pProductIdStringEntry    The address of mProductIdString[].
  
  @param[out]  *NumOfIoApic               Total entry number of mIoApicDefault[].
  @param[out]  **pIoApicEntry             The address of mIoApicDefault[].
  
  @param[out]  *NumOfIoApicIntLegacy      Total entry number of mIoApicIntLegacy[].
  @param[out]  **pIoApicIntLegacyEntry    The address of mIoApicIntLegacy[].

  @param[out]  *NumOfIoApicInt            Total entry number of mIoApicIntDefault[].
  @param[out]  **pIoApicIntEntry          The address of mIoApicIntDefault[].

  @param[out]  *NumOfIoApicIntSlot        Total entry number of mIoApicIntSlotDefault[].
  @param[out]  **pIoApicIntSlotEntry      The address of mIoApicIntSlotDefault[].

  @param[out]  *NumOfLocalApicInt         Total entry number of mLocalApicIntDefault[].
  @param[out]  **pLocalApicIntEntry       The address of mLocalApicIntDefault[].
  
  @retval      EFI_UNSUPPORTED            Returns unsupported by default.
  @retval      EFI_SUCCESS                MP Table is supported.
  @retval      EFI_MEDIA_CHANGED          The value of IN OUT parameter is changed. 
  @retval      Others                     Depends on customization.
**/
EFI_STATUS 
OemSvcPrepareInstallMpTable (
  OUT UINTN                               *CharNumOfOemIdString,  
  OUT CHAR8                               **pOemIdStringEntry,
  OUT UINTN                               *CharNumOfProductIdString,  
  OUT CHAR8                               **pProductIdStringEntry,
  OUT UINTN                               *NumOfIoApic,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC    **pIoApicEntry,
  OUT UINTN                               *NumOfIoApicIntLegacy,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT    **pIoApicIntLegacyEntry,  
  OUT UINTN                               *NumOfIoApicInt,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT    **pIoApicIntEntry,
  OUT UINTN                               *NumOfIoApicIntSlot,  
  OUT MP_TABLE_ENTRY_IO_INT_SLOT          **pIoApicIntSlotEntry,
  OUT UINTN                               *NumOfLocalApicInt,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT **pLocalApicIntEntry
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/  

  return EFI_UNSUPPORTED;
}      
