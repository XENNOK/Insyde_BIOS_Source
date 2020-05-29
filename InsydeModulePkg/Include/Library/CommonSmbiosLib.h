/** @file

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

/*++
 
Module Name:

  CommonSmbiosLib.h
  
Abstract: 

  Provide some smbios function.

--*/

#ifndef __SMBIOS_COMMON_LIB__
#define __SMBIOS_COMMON_LIB__

#include <Protocol/Smbios.h>

/**
  Logs SMBIOS record.

 @param [in]   SmbiosType     Get this Type from SMBIOS table
 @param [out]  HandleArray    Pointer to Hadndler array with has been free by caller
 @param [out]  HandleCount    Pointer to Hadndler Counter

**/
VOID
GetLinkTypeHandle(
  IN  UINT8                 SmbiosType,
  OUT UINT16                **HandleArray,
  OUT UINTN                 *HandleCount
  );

/**
  Logs SMBIOS record.

 @param [in]   ObjHandle     Device handle into Base Board Info

**/
EFI_STATUS
UpdateBaseBoardContainHandle(
  IN UINT16  ObjHandle
  );

/**
  Logs SMBIOS record.

 @param [in]   Buffer         Pointer to the data buffer.
 @param [in]   SmbiosHandle   Pointer for retrieve handle.

**/
EFI_STATUS
LogSmbiosData (
  IN       UINT8                      *Buffer,
  IN  OUT  EFI_SMBIOS_HANDLE          *SmbiosHandle
  );

/*++
Routine Description: 

  Get the SMBIOS context from SMBIOS handle.
    
Arguments:  

  
  

Returns:  

  EFI_SUCCESS   - Got the context of SMBIOS
  EFI_NOT_FOUND - Not found the handle with SMBIOS DB
  
--*/  
/**
 Get the SMBIOS context from SMBIOS handle.

 @param [in]   Handle  - The handle of this callback, obtained when registering
 @param [in]   Buffer  - Point to a buffer pointer that it will contain the context of SMBIOS after return

 @retval EFI_STATUS  EFI_SUCCESS   - Got the context of SMBIOS
 @retval EFI_STATUS  EFI_NOT_FOUND - Not found the handle with SMBIOS DB

**/
EFI_STATUS
GetTSmbiosHandleData(
  IN  EFI_SMBIOS_HANDLE        Handle,
  OUT EFI_SMBIOS_TABLE_HEADER  **Buffer
  );
#endif

