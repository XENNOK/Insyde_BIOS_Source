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
#include <Library/BaseCryptLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/PcdLib.h>

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
 

 /**
 SHA256 HASH calculation

 @param [in]   Message          The message data to be calculated
 @param [in]   MessageSize      The size in byte of the message data
 @param [out]  Digest           The caclulated HASH digest

 @retval EFI_SUCCESS            The HASH value is calculated
 @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
STATIC
EFI_STATUS
EFIAPI
CalculateSha256Hash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID       *HashCtx;
  UINTN      CtxSize;
  EFI_STATUS Status;

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);
  CtxSize = Sha256GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha256Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

STATIC
EFI_STATUS
EFIAPI       
VerifyHash (
 IN UINT8                         HashMethod,
 IN UINT8                         *Hash,
 IN UINT8                         *Message,
 IN UINTN                         MessageSize
 ){
 

  EFI_STATUS    Status;
  UINT8         *Digest;
  UINTN                           DigestSize;         

  
  if (Hash == NULL){
    return EFI_INVALID_PARAMETER;
  }  
  
  if (Message == NULL){
    return EFI_INVALID_PARAMETER;
  }      
 
  switch (HashMethod){
      
  case ENTRY_HASH_SHA256:
  default:
  
      Digest = AllocatePool (SHA256_DIGEST_SIZE);
      if (Digest == NULL){
        return EFI_OUT_OF_RESOURCES;
      }
                  
      Status = CalculateSha256Hash (Message, MessageSize, Digest);  
      if (EFI_ERROR(Status)){
        return Status;     
      }
  
      DigestSize =  SHA256_DIGEST_SIZE;
      break;
   
  }  
 

  if (CompareMem (Digest, Hash, DigestSize)){  
    Status = EFI_SECURITY_VIOLATION;
    goto done;
  } else {    
    Status = EFI_SUCCESS;
    goto done;
  }  
 
done:
  FreePool (Digest);
  return Status;  
 }
 
/**
  Get count of entries that FDM recorded 

  @param  Count                 Unsigned integer that specifies the count of entries that FDM recorded.
  @retval EFI_SUCCESS           find region type success.
  @retval EFI_NOT_FOUND         Can't find region type in the FDM.

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

  @param  RegionType            GUID that specifies the type of region that search for.
  @param  Instance              Unsigned integer that specifies the N-th Region type instance
  @param  RegionId              Identifier that specifies the identifier of this region..
  @param  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                base of the flash device image.
  @param  RegionSize            Unsigned integer that specifies the region size.
  @param  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS           find region type success.
  @retval EFI_INVALID_PARAMETER Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION  Region hash is not correct.    
  @retval EFI_NOT_FOUND         Can't find region type in the FDM.

**/
EFI_STATUS
EFIAPI
FdmGetNAt (
  CONST IN  EFI_GUID  *RegionType,  
  CONST IN  UINT8     Instance,
  OUT UINT8    *RegionId,
  OUT UINT64   *RegionOffset,
  OUT UINT64   *RegionSize,
  OUT UINT32   *Attribs
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
  UINT64                        BaseAddr; 
  UINT8                         *Fdm;

  
  Fdm = (UINT8*)(UINTN)FixedPcdGet32(PcdH2OFlashDeviceMapStart);
  BaseAddr =(UINT64)FixedPcdGet32(PcdFlashAreaBaseAddress);  
  

  if (Fdm == NULL){
    return EFI_NOT_FOUND;   
  }     
   
  if ( CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }  
  
  if (( Checksum8 (Fdm, sizeof(H2O_FLASH_DEVICE_MAP_HEADER) - sizeof(UINT8)) + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Checksum  & 0xFF) != 0) {
    return EFI_SECURITY_VIOLATION;
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
  Status = VerifyHash (EntryFormat, Hash,  (UINT8*)(UINTN) (BaseAddr + Entry->RegionOffset), (UINTN)Entry->RegionSize);
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

  @param  RegionType            GUID that specifies the type of region that N-th entry is.
  @param  Instance              Unsigned integer that specifies entry instance of FDM
  @param  RegionId              Identifier that specifies the identifier of this region..
  @param  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                base of the flash device image.
  @param  RegionSize            Unsigned integer that specifies the region size.
  @param  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS           find region type success.
  @retval EFI_INVALID_PARAMETER Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION  Region hash is not correct.    
  @retval EFI_NOT_FOUND         Can't find region type in the FDM.

**/
EFI_STATUS
EFIAPI
FdmGetAt (
  CONST IN  UINT8     Instance,
  OUT EFI_GUID *RegionType,    
  OUT UINT8    *RegionId,
  OUT UINT64   *RegionOffset,
  OUT UINT64   *RegionSize,
  OUT UINT32   *Attribs
  ){
  
  UINT8        EntryFormat;
  UINT8        *EndPtr;
  UINT8        Matched;
  UINT8        *Hash;
  UINT32       FirstEntryOffset;
  UINT32       EntrySize;
  UINT32       Size;
  EFI_STATUS   Status;
  H2O_FLASH_DEVICE_MAP_ENTRY   *Entry;
  UINTN        DigestSize;  
  UINT64       BaseAddr;
  UINT8        *Fdm;  


  
  Fdm = (UINT8*)(UINTN)FixedPcdGet32(PcdH2OFlashDeviceMapStart);
  BaseAddr =(UINT64)FixedPcdGet32(PcdFlashAreaBaseAddress);  
  
  if (Fdm == NULL){
    return EFI_NOT_FOUND;   
  }     
   
  if ( CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }  
  
  if (( Checksum8 (Fdm, sizeof(H2O_FLASH_DEVICE_MAP_HEADER) - sizeof(UINT8)) + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Checksum  & 0xFF) != 0) {
    return EFI_SECURITY_VIOLATION;
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
  
  Status = VerifyHash (EntryFormat, Hash,  (UINT8*)(UINTN) (BaseAddr + Entry->RegionOffset), (UINTN) Entry->RegionSize);
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
