/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
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

  CommonSmbiosLib.c
  
Abstract: 

  Provide some smbios function.

--*/

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#define MAX_HANDLE_COUNT  0x10

EFI_SMBIOS_PROTOCOL  *mSmbios = NULL;

#define  SEARCH_EQU_END_STR(p)            while(p && (*p != 0 || *(p + 1) != 0)){ p++;}

EFI_STATUS
EFIAPI
SmbiosBaseLibConstructor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&mSmbios);
}

VOID
GetLinkTypeHandle(
  IN  UINT8                 SmbiosType,
  OUT UINT16                **HandleArray,
  OUT UINTN                 *HandleCount
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER          *LinkTypeData = NULL;

  if(mSmbios == NULL)
    return ;
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  *HandleArray = AllocateZeroPool(sizeof(UINT16)*MAX_HANDLE_COUNT);
  if (*HandleArray == NULL) {
    DEBUG ((EFI_D_INFO, "HandleArray allocate memory resource failed.\n"));
    return;
  }

  *HandleCount = 0;
  
  do{
    
    Status = mSmbios->GetNext(
                        mSmbios, 
                        &SmbiosHandle, 
                        &SmbiosType,
                        &LinkTypeData,
                        NULL
                        );
  
    if(!EFI_ERROR(Status)){
      (*HandleArray)[*HandleCount] = LinkTypeData->Handle;
      (*HandleCount)++;
    }else{
      break;
    }

  }while(1);

}

EFI_STATUS
LogSmbiosData (
  IN       UINT8                      *Buffer,
  IN  OUT  EFI_SMBIOS_HANDLE          *SmbiosHandle
  )
{
  EFI_STATUS         Status;
  
  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  
  Status = mSmbios->Add (
                     mSmbios,
                     NULL,
                     SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)Buffer
                     );
  FreePool(Buffer);  

  return Status;
}

EFI_STATUS
UpdateBaseBoardContainHandle(
  IN UINT16  ObjHandle
  )
{
  UINT8                    AppendixSize = 0;
  UINT8                    *Ptr, *NewPtr;
  UINT16                   *ContainedObjectHandles;
  UINTN                    TotalStrLen = 0;
  EFI_STATUS               Status;
  EFI_SMBIOS_TYPE          SmbiosType;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  SMBIOS_TABLE_TYPE2       *SmbiosRecord;
  SMBIOS_TABLE_TYPE2       *NewSmbiosRecord;
  
  if(mSmbios == NULL)
    return EFI_NOT_READY;

  SmbiosType = 2;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  
  Status = mSmbios->GetNext(
                      mSmbios, 
                      &SmbiosHandle, 
                      &SmbiosType,
                      (EFI_SMBIOS_TABLE_HEADER**)&SmbiosRecord,
                      NULL
                      );

  //
  // SMBIOS type buffer Expand
  //
  if(!EFI_ERROR(Status)){
    

    NewPtr = Ptr = ((UINT8*)SmbiosRecord) + ((EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord)->Length;
    
    SEARCH_EQU_END_STR(NewPtr);
    NewPtr++;
    
    TotalStrLen = ((UINTN)NewPtr) - ((UINTN)Ptr) + 1;

    if(SmbiosRecord->NumberOfContainedObjectHandles != 0){
      AppendixSize = sizeof(UINT16);
    }
    
    NewSmbiosRecord = AllocateCopyPool( ((EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord)->Length + AppendixSize + TotalStrLen, SmbiosRecord);
    
    NewSmbiosRecord->Hdr.Length += AppendixSize;
    
    ContainedObjectHandles = NewSmbiosRecord->ContainedObjectHandles;
    ContainedObjectHandles[NewSmbiosRecord->NumberOfContainedObjectHandles] = ObjHandle;
    NewSmbiosRecord->NumberOfContainedObjectHandles++;

    if(SmbiosRecord->NumberOfContainedObjectHandles != 0){
      CopyMem( ((UINT8*)NewSmbiosRecord) + ((EFI_SMBIOS_TABLE_HEADER*)NewSmbiosRecord)->Length, Ptr, TotalStrLen);
    }
    
    Status = mSmbios->Remove(
                        mSmbios, 
                        SmbiosHandle
                        );

    if(!EFI_ERROR(Status)){
      LogSmbiosData( (UINT8*)NewSmbiosRecord, &SmbiosHandle);
    }
  }

  return Status;    
}


EFI_STATUS
GetTSmbiosHandleData(
  IN  EFI_SMBIOS_HANDLE        Handle,
  OUT EFI_SMBIOS_TABLE_HEADER  **Buffer
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER  *SmbiosBuffer;

  Status       = EFI_SUCCESS;
  SmbiosBuffer = NULL;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  if (mSmbios == NULL) {
    return EFI_NOT_READY;  
  }
  
  do {
    Status = mSmbios->GetNext (
                      mSmbios,
                      &SmbiosHandle,
                      NULL,
                      &SmbiosBuffer,
                      NULL
                      );
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Handle == SmbiosHandle) {
      *Buffer = (EFI_SMBIOS_TABLE_HEADER*)SmbiosBuffer;
      return EFI_SUCCESS;
    }  
  }while(1);

  return EFI_NOT_FOUND;
}

