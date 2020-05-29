/** @file
  Library for Getting Information of Flash Device Region Layout

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Pi/PiSmmCis.h>
#include <Protocol/Hash.h>
#include <Protocol/SmmBase2.h>
#include <Library/BaseCryptLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/PcdLib.h>

#include <Guid/EventGroup.h>


BOOLEAN                    mInSmm;
BOOLEAN                    mIsAtRuntime                  = FALSE;
EFI_EVENT                  mExitBootServiceEvent         = NULL;
EFI_SMM_SYSTEM_TABLE2      *mSmst                        = NULL;



//
// FDM signature, for not having signature in machine code of this library, assign value one by one byte
//
CHAR8 mSignature[] = {'H', 'F', 'D', 'M'};



STATIC
UINT8
EFIAPI        
Checksum8 (
   UINT8   *Data,
   UINT32  DataSize
 ){

  UINT32 Index;
  UINT16  Sum;

  Sum = 0;

  for (Index = 0; Index < DataSize; Index++, Data++){
    Sum += (*Data);
    Sum &= 0xFF;
  }

  return (UINT8)Sum;
 }
 
 
STATIC
EFI_STATUS
EFIAPI       
VerifyHash (
 IN EFI_HASH_PROTOCOL             *HashProtocol,
 IN UINT8                         HashMethod,
 IN UINT8                         *Hash,
 IN UINT8                         *Message,
 IN UINT64                        MessageSize
 ){
 
  EFI_HASH_OUTPUT                 Digest;
  EFI_STATUS                      Status;
  UINTN                           DigestSize;      
  UINTN                           HashSize;
 
 
  if (HashProtocol == NULL){
    return EFI_INVALID_PARAMETER;
  }
    
  if (Hash == NULL){
    return EFI_INVALID_PARAMETER;
  }  

  if (Message == NULL){
    return EFI_INVALID_PARAMETER;
  }    
  
 
  switch (HashMethod){
      
  case ENTRY_HASH_SHA256:
  
    Status = HashProtocol->GetHashSize (HashProtocol, &gEfiHashAlgorithmSha256Guid, &HashSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }    
    Digest.Sha256Hash =  AllocatePool (HashSize);   

    if (Digest.Sha256Hash == NULL){
      return EFI_OUT_OF_RESOURCES;
    }    
    
    Status = HashProtocol->Hash (
                   HashProtocol,
                   &gEfiHashAlgorithmSha256Guid,
                   FALSE,
                   Message,
                   MessageSize,
                   (EFI_HASH_OUTPUT *) &Digest
                   );

    if (EFI_ERROR(Status)){
      goto done;     
    }
  
    DigestSize =  HashSize;
    break;
    
  default:
  
    return EFI_UNSUPPORTED;  
    break;
  }

  if (CompareMem (Digest.Sha256Hash, Hash, DigestSize)){  
    Status = EFI_SECURITY_VIOLATION;
    goto done;
  } else {    
    Status = EFI_SUCCESS;
    goto done;
  }  
    
done:
  FreePool(Digest.Sha256Hash);
  return Status;    
} 


EFI_STATUS
EFIAPI    
ExitBootServiceCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
) {
  
  gBS->CloseEvent (mExitBootServiceEvent);      
  mIsAtRuntime = TRUE;
  
  return EFI_SUCCESS;

}



 
/**
  Get count of entries that FDM recorded 

  @param[out] Count             Unsigned integer that specifies the count of entries that FDM recorded.
  
  @retval EFI_SUCCESS           Get region count success.
  @retval EFI_NOT_FOUND         Can't find region type in the FDM.
  @retval EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetCount (
  OUT  UINT8  *Count        
)
{

  H2O_FLASH_DEVICE_MAP_HEADER   *FdmHeader;
  UINT32                        FirstEntryOffset;
  UINT32                        EntrySize;
  UINT32                        Size;


  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }
  
  FdmHeader = (H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN)FixedPcdGet32(PcdH2OFlashDeviceMapStart);
  
  if (FdmHeader == NULL){
    return EFI_NOT_FOUND;   
  }     
   
  if ( CompareMem(FdmHeader, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }  

  FirstEntryOffset = FdmHeader->Offset;
  Size        = FdmHeader->Size;
  EntrySize   = FdmHeader->EntrySize;
  
  if ( ((Checksum8 ((UINT8*)FdmHeader, sizeof(H2O_FLASH_DEVICE_MAP_HEADER) - sizeof(FdmHeader->Checksum)) + FdmHeader->Checksum) & 0xFF) != 0) {
    return EFI_SECURITY_VIOLATION;
  }  

  *Count = ((Size - FirstEntryOffset) / EntrySize) & 0xFF;

  return EFI_SUCCESS;
}

/**
  Get Region type record in FDM by specifying order in the list.

  @param[in]   RegionType            GUID that specifies the type of region that search for.
  @param[in]   Instance              Unsigned integer that specifies the N-th Region type instance
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS                find region type success.
  @retval EFI_INVALID_PARAMETER      Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION     Region hash is not correct.    
  @retval EFI_NOT_FOUND              Can't find region type in the FDM.
  @retval EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @retval EFI_UNSUPPORTED            Function called in runtime phase  

**/
EFI_STATUS
EFIAPI
FdmGetNAt (
  IN CONST EFI_GUID  *RegionType,  
  IN CONST UINT8     Instance,
  OUT UINT8          *RegionId,
  OUT UINT64         *RegionOffset,
  OUT UINT64         *RegionSize,
  OUT UINT32         *Attribs
  ){
  
  UINT8                         EntryFormat;
  UINT8                         *EndPtr;
  UINT8                         Matched;
  UINT8                         *Hash;
  UINT32                        FirstEntryOffset;
  UINT32                        EntrySize;
  UINT32                        Size;
  EFI_STATUS                    Status;
  H2O_FLASH_DEVICE_MAP_ENTRY    *Entry;
  UINT8                         *BaseAddr;
  UINT8                         *Fdm;
  EFI_HASH_PROTOCOL             *HashProtocol;  
  
  
  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }  
  
  Fdm      = (UINT8*)(UINTN)FixedPcdGet32(PcdH2OFlashDeviceMapStart);
  BaseAddr = (UINT8*)(UINTN)FixedPcdGet32(PcdFlashAreaBaseAddress);  
  
  if (Fdm == NULL){
    return EFI_NOT_FOUND;   
  }     
   
  if ( CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }  
  
  if (( Checksum8 (Fdm, sizeof(H2O_FLASH_DEVICE_MAP_HEADER) - sizeof(UINT8)) + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Checksum  & 0xFF) != 0) {
    return EFI_SECURITY_VIOLATION;
  }      
  
  if (mInSmm){
    Status = mSmst->SmmLocateProtocol (
                      &gEfiHashProtocolGuid,
                      NULL,
                    (VOID**) &HashProtocol
                      );
    if (EFI_ERROR (Status)){
      return EFI_NOT_READY;
    }
                      
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID**) &HashProtocol
                    );
    if (EFI_ERROR (Status)){
      return EFI_NOT_READY;
    }
  }
  
  //
  // Instance number start from 1  
  //
  if (Instance == 0){
    return EFI_INVALID_PARAMETER;
  }  
  
  FirstEntryOffset = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Offset;
  Size         = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Size;
  EntrySize    = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntrySize;
  EntryFormat  = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntryFormat;  
  Entry        = (H2O_FLASH_DEVICE_MAP_ENTRY*) (Fdm + FirstEntryOffset);

  EndPtr = Fdm + Size;
  Matched = 0;
  
  for (; (UINT8*)Entry < EndPtr;  Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + EntrySize)){
    if (!CompareMem(&Entry->RegionType, RegionType, sizeof(EFI_GUID))){
      Matched++; 
      
      if (Matched == Instance){
        break;
      }          
    }    
  }
  
  if (Matched != Instance){
    return EFI_NOT_FOUND;
  }

  Hash = (UINT8*)(Entry + 1);
  Status = VerifyHash (HashProtocol, EntryFormat, Hash,  (UINT8*)(BaseAddr + Entry->RegionOffset), (UINT64) Entry->RegionSize);
  if (EFI_ERROR(Status)){  
    return EFI_SECURITY_VIOLATION;
  } else {  
    CopyMem (RegionId, Entry->RegionId, FDM_ENTRY_REGION_ID_SIZE);
    *RegionOffset = Entry->RegionOffset;
    *RegionSize   = Entry->RegionSize;
    *Attribs      = Entry->Attribs;

    return EFI_SUCCESS;

  }  
}


/**
  Get entry recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region that N-th entry is.
  @param[out]  Instance              Unsigned integer that specifies entry instance of FDM
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS                find region type success.
  @retval EFI_INVALID_PARAMETER      Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION     Region hash is not correct.    
  @retval EFI_NOT_FOUND              Can't find region type in the FDM.
  @retval EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet  
  @retval EFI_UNSUPPORTED            Function called in runtime phase  

**/
EFI_STATUS
EFIAPI
FdmGetAt (
  IN CONST UINT8   Instance,
  OUT EFI_GUID     *RegionType,    
  OUT UINT8        *RegionId,
  OUT UINT64       *RegionOffset,
  OUT UINT64       *RegionSize,
  OUT UINT32       *Attribs
  ){
  
  UINT8                         EntryFormat;
  UINT8                         *EndPtr;
  UINT8                         Matched;
  UINT8                         *Hash;
  UINT32                        FirstEntryOffset;
  UINT32                        EntrySize;
  UINT32                        Size;
  EFI_STATUS                    Status;
  H2O_FLASH_DEVICE_MAP_ENTRY    *Entry;
  UINTN                         DigestSize;  
  UINT8                         *BaseAddr;
  UINT8                         *Fdm;  
  EFI_HASH_PROTOCOL             *HashProtocol;


  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }
  
  Fdm = (UINT8*)(UINTN)FixedPcdGet32(PcdH2OFlashDeviceMapStart);
  BaseAddr = (UINT8*)(UINTN)FixedPcdGet32(PcdFlashAreaBaseAddress);  
  
  if (Fdm == NULL){
    return EFI_NOT_FOUND;   
  }     
   
  if ( CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }  
  
  if (( Checksum8 (Fdm, sizeof(H2O_FLASH_DEVICE_MAP_HEADER) - sizeof(UINT8)) + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Checksum  & 0xFF) != 0) {
    return EFI_SECURITY_VIOLATION;
  }     
  
  if (mInSmm){
    Status = mSmst->SmmLocateProtocol (
                      &gEfiHashProtocolGuid,
                      NULL,
                    (VOID**) &HashProtocol
                      );
    if (EFI_ERROR (Status)){
      return EFI_NOT_READY;
    }
                      
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID**) &HashProtocol
                    );
    if (EFI_ERROR (Status)){
      return EFI_NOT_READY;
    }
  }

  //
  // Instance number start from 1  
  //  
  if (Instance == 0){
    return EFI_INVALID_PARAMETER;
  }
  
  FirstEntryOffset = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Offset;
  Size        = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Size;
  EntrySize   = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntrySize;
  EntryFormat = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntryFormat;  
  Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*) (Fdm + FirstEntryOffset);

  EndPtr = Fdm + Size;
  Matched = 0;
  DigestSize = 0;

  Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + (Instance - 1) * EntrySize);

  if (Entry > (H2O_FLASH_DEVICE_MAP_ENTRY*)EndPtr){
    return EFI_NOT_FOUND;    
  }

  Hash = (UINT8*)(Entry + 1);
  
  Status = VerifyHash (HashProtocol, EntryFormat, Hash,  (UINT8*)(BaseAddr + Entry->RegionOffset), (UINT64) Entry->RegionSize);
  if (EFI_ERROR(Status)){  
  
    return EFI_SECURITY_VIOLATION;
    
  } else {
  
    CopyMem (RegionType, &Entry->RegionType, sizeof(Entry->RegionType));  
    CopyMem (RegionId, Entry->RegionId, FDM_ENTRY_REGION_ID_SIZE);
    *RegionOffset = Entry->RegionOffset;
    *RegionSize   = Entry->RegionSize;
    *Attribs      = Entry->Attribs;
  
    return EFI_SUCCESS;

  }  
}


/**
  Register ExitBootService Callback for indicate not supported for run time.

  @param  ImageHandle             ImageHandle of the loaded driver.
  @param  SystemTable             Pointer to the EFI System Table.

  @retval  EFI_SUCCESS            Register successfully.

**/
EFI_STATUS
EFIAPI
DxeFlashRegionLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
){

  EFI_STATUS                 Status;
  EFI_SMM_BASE2_PROTOCOL     *SmmBase;  

  
  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    Status = SmmBase->InSmm (SmmBase, &mInSmm);
  } else {
    mInSmm = FALSE;
  }

  if (!mInSmm) {  
                         
    //
    // Register the event to declare not support for runtime.
    //    
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ExitBootServiceCallback,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &mExitBootServiceEvent
                    );   

    
  } else {
  
    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }  
  
  }  
  
  return EFI_SUCCESS;
}


/**
  Close registered ExitBootService event  

  @param  ImageHandle             ImageHandle of the loaded driver.
  @param  SystemTable             Pointer to the EFI System Table.

  @retval  EFI_SUCCESS            Close successfully.

**/
EFI_STATUS
EFIAPI
DxeFlashRegionLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
){
  EFI_STATUS    Status;
  

  if (mExitBootServiceEvent != NULL){
    Status = gBS->CloseEvent (mExitBootServiceEvent);    
  }
  
  return EFI_SUCCESS;

}

