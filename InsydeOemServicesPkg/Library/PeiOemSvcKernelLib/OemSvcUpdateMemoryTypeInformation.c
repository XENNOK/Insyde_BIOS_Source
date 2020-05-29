/** @file
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

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

#include <Library/PeiOemSvcKernelLib.h>


/**
 The OemUpdateMemoryTypeInformation of OemService is used in function GetPlatformMemorySize 
 which locates in Memory.c. It provides an interface for setting MemoryTypeInformation table 
 including memory type and memory page, the default MemoryTypeInformation get from PcdPreserveMemoryTable.
 In the new MemoryTypeInformation table(or defaultMemoryTypeInformation) table will generate its information(s)
 into the Hob which provide a reserve space for memory allocation for DXE phase.
 

  @param[in, out]  *MemoryTypeInformation A pointer to MemoryTypeInformationTable 
  @param[in, out]  *MemoryTableSize       Size of MemoryTypeInformationTable 

  @retval          EFI_SUCCESS           MemoryTypeInformation pointer will point to OEM customization Table and 
  @retval                                Build into GUID HOB for DXE in OemGetMemoryTypeInformation.
  @retval          EFI_UNSUPPORTED 	     DefaultMemoryTypeInformation table.
  @retval          EFI_MEDIA_CHANGED     MemoryTypeInformation pointer will point to OEM customization Table. 
 
**/
EFI_STATUS
OemSvcUpdateMemoryTypeInformation (
  IN OUT  EFI_MEMORY_TYPE_INFORMATION  **MemoryTypeInformation,
  IN OUT  UINT32                       *MemoryTableSize
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  
  return EFI_UNSUPPORTED;
}

