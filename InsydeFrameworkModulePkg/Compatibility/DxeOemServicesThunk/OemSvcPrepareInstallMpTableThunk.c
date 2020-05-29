/** @file
  Returns platform specific MP Table information to the caller.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  Returns platform specific MP Table information to the caller.

  @param  *CharNumOfOemIdString      The characters number of OEM ID string.
  @param  **pOemIdStringEntry        The address of mOemIdString[].
  
  @param  *CharNumOfProductIdString  The characters number of Product ID string
  @param  **pProductIdStringEntry    The address of mProductIdString[].
  
  @param  *NumOfIoApic               Total entry number of mIoApicDefault[].
  @param  **pIoApicEntry             The address of mIoApicDefault[].
  
  @param  *NumOfIoApicIntLegacy      Total entry number of mIoApicIntLegacy[].
  @param  **pIoApicIntLegacyEntry    The address of mIoApicIntLegacy[].

  @param  *NumOfIoApicInt            Total entry number of mIoApicIntDefault[].
  @param  **pIoApicIntEntry          The address of mIoApicIntDefault[].

  @param  *NumOfIoApicIntSlot        Total entry number of mIoApicIntSlotDefault[].
  @param  **pIoApicIntSlotEntry      The address of mIoApicIntSlotDefault[].

  @param  *NumOfLocalApicInt         Total entry number of mLocalApicIntDefault[].
  @param  **pLocalApicIntEntry       The address of mLocalApicIntDefault[].
  
  @retval EFI_SUCCESS           MP Table is supported.
  @retval EFI_UNSUPPORTED       MP Table is NOT supported.
**/
EFI_STATUS 
OemSvcPrepareInstallMpTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;

  UINTN                                 *CharNumOfOemIdString;  
  CHAR8                                 **pOemIdStringEntry;
  
  UINTN                                 *CharNumOfProductIdString;  
  CHAR8                                 **pProductIdStringEntry;
  
  UINTN                                 *NumOfIoApic;  
  EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC      **pIoApicEntry;

  UINTN                                 *NumOfIoApicIntLegacy;  
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT      **pIoApicIntLegacyEntry;  
  UINTN                                 *NumOfIoApicInt;  
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT      **pIoApicIntEntry;
  UINTN                                 *NumOfIoApicIntSlot;  
  MP_TABLE_ENTRY_IO_INT_SLOT            **pIoApicIntSlotEntry;

  UINTN                                 *NumOfLocalApicInt;  
  EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT   **pLocalApicIntEntry;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  
  CharNumOfOemIdString     = VA_ARG (Marker, UINTN *);  
  pOemIdStringEntry        = VA_ARG (Marker, CHAR8 **); 
  
  CharNumOfProductIdString = VA_ARG (Marker, UINTN *);  
  pProductIdStringEntry    = VA_ARG (Marker, CHAR8 **); 
  
  NumOfIoApic              = VA_ARG (Marker, UINTN *);  
  pIoApicEntry             = VA_ARG (Marker, EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC **); 
  
  NumOfIoApicIntLegacy     = VA_ARG (Marker, UINTN *);  
  pIoApicIntLegacyEntry    = VA_ARG (Marker, EFI_LEGACY_MP_TABLE_ENTRY_IO_INT **); 
  NumOfIoApicInt           = VA_ARG (Marker, UINTN *);  
  pIoApicIntEntry          = VA_ARG (Marker, EFI_LEGACY_MP_TABLE_ENTRY_IO_INT **);  
  NumOfIoApicIntSlot       = VA_ARG (Marker, UINTN *);  
  pIoApicIntSlotEntry      = VA_ARG (Marker, MP_TABLE_ENTRY_IO_INT_SLOT **);  

  NumOfLocalApicInt        = VA_ARG (Marker, UINTN *);  
  pLocalApicIntEntry       = VA_ARG (Marker, EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT **);  

  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcPrepareInstallMpTable (
             CharNumOfOemIdString,  
             pOemIdStringEntry,
             CharNumOfProductIdString,  
             pProductIdStringEntry,
             NumOfIoApic,  
             pIoApicEntry,
             NumOfIoApicIntLegacy,  
             pIoApicIntLegacyEntry,  
             NumOfIoApicInt,  
             pIoApicIntEntry,
             NumOfIoApicIntSlot,  
             pIoApicIntSlotEntry,
             NumOfLocalApicInt,  
             pLocalApicIntEntry
             );

  return Status;
}      
