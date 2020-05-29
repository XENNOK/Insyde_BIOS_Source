/** @file

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

#include <Library/AcpiPlatformCommon.h>

#define AML_OPREGION_OP     0x805b

#ifndef _ACPI_H_
#define AML_METHOD_OP       0x14
#define AML_NAME_OP         0x08
#define AML_BYTE_PREFIX     0x0A
#define AML_WORD_PREFIX     0x0B
#define AML_PACKAGE_OP      0x12
#define AML_STRING_PREFIX   0x0D
#define AML_DWORD_PREFIX    0x0C
#endif

/**
 Load a firmware volume image to buffer given its Guid.

 @param[in]          NameGuid    Guid of image to be found.
 @param[in, out]   Buffer          buffer to place image.
 @param[in, out]   Size             size of image to load.
 
 @retval   EFI_SUCCESS            If image is successfully loaded
 @retval   EFI_NOT_FOUND       If image is not found.
 @retval   EFI_LOAD_ERROR      If we have problems loading the image.
*/
EFI_STATUS
GetFvImage (
  IN EFI_GUID   *NameGuid,
  IN OUT VOID   **Buffer,
  IN OUT UINTN  *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  EFI_SECTION_TYPE              SectionType;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;

  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiFirmwareVolume2ProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer
                );
  if (EFI_ERROR(Status) || HandleCount == 0) {

    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );

    if (EFI_ERROR(Status)) {

      return EFI_LOAD_ERROR;
    }

    *Buffer = NULL;
    *Size = 0;
    SectionType  = EFI_SECTION_RAW;
    Status = Fv->ReadSection (
                      Fv,
                      NameGuid,
                      SectionType,
                      0,
                      Buffer,
                      Size,
                      &AuthenticationStatus
                      );

    if (!EFI_ERROR (Status)) {

      break;
    }

    //
    // Try a raw file, since a PE32 SECTION does not exist
    //
    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadFile (
                        Fv,
                        NameGuid,
                        Buffer,
                        Size,
                        &FileType,
                        &Attributes,
                        &AuthenticationStatus
                        );

    if (!EFI_ERROR (Status)) {

      break;
    }
  }

  //
  // not found image
  //
  if (Index >= HandleCount) {

    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

BOOLEAN
IsAmlOpRegionObject (
  IN UINT8               *DsdtPointer
  )
{
  UINT16                 *Operation = NULL;
  
  Operation = (UINT16*)(DsdtPointer - 2);
  if (*Operation == AML_OPREGION_OP ) {
    
    return TRUE;
  } 
  
  return FALSE;   	
}

BOOLEAN
IsAmlMethodObject (
  IN UINT8               *DsdtPointer,
  OUT UINT64             *PkgLength
  )
{
  UINT8                 *Operation = NULL;
  UINT64                TempLength = 0;
  UINT8                 PackageBytes = 0;
  UINT8                 Index;
  
  for (Operation = DsdtPointer - 5; Operation < DsdtPointer - 1; Operation++) {
    if (*Operation == AML_METHOD_OP) {
      PackageBytes = (Operation[1] >> 6) + 1;
      if (PackageBytes != ((UINTN)DsdtPointer - (UINTN)Operation - 1)) {
        return FALSE;
      }
      break;
    }
  }
  if (PackageBytes == 0) {
    return FALSE;
  }
  Operation++;
  *PkgLength = (UINT64)(*Operation) & 0x3F;
  if (PackageBytes > 1) {
    *PkgLength = (*PkgLength) & 0x0F;
  }
  for (Index = 1; Index < PackageBytes; Index++) {
    TempLength = Operation[Index];
    TempLength = LShiftU64(TempLength, (Index * 8 - 4));
    *PkgLength += TempLength;
  }
  
  
  return TRUE;   	
}

/**
 Update the OperationRegion of a system memory's offset and size
 
 @param[in]   DsdtPointer              A pointer to the address where the Operation's Name. 
                                                  Assume it is four letters in ASL, such as 'GNVS'
 @param[in]   RegionStartAddress   The address where the Operation Region allocated in system memory (AcpiNvsArea).
                                                  It is assume to be a DWORD.
 @param[in]  NumElement             The adress of the package element number.
                                                  On return, the package element number will be updated to the NumElement.

 @retval  TRUE    If the object is Name(ABCD, Package(){}), the *PkgLength, *NumElement will be updated
*/
EFI_STATUS
SetOpRegion (
  IN UINT8               *DsdtPointer,
  IN VOID*               RegionStartAddress,
  IN UINT32              RegionSize
  )
{
  UINT8                         AddressPrefix;
  UINT32                        *Address;
  UINT32                        *DwordSize;
  UINT8                         SizePrefix;
  UINT16                        *WordSize;
  UINT8                         *ByteSize;

  AddressPrefix = *(DsdtPointer + 5);
  ASSERT (AddressPrefix == AML_DWORD_PREFIX);
  Address = (UINT32*) (DsdtPointer + 6);
  ASSERT ((((UINTN)RegionStartAddress) & 0xFFFFFFFF) == (UINTN)RegionStartAddress);
  *Address = (UINT32)(UINTN)RegionStartAddress;
  SizePrefix = *(DsdtPointer + 10);
  if (SizePrefix == AML_DWORD_PREFIX) {
    DwordSize = (UINT32*) (DsdtPointer + 11);
    *DwordSize = RegionSize;
  } else if (SizePrefix == AML_WORD_PREFIX) {
    ASSERT (RegionSize < 0x10000);
    RegionSize &= 0xFFFF;
    WordSize = (UINT16*) (DsdtPointer + 11);
    *WordSize = (UINT16) RegionSize;
  } else if (SizePrefix == AML_BYTE_PREFIX) {
    ASSERT (RegionSize < 0x100);
    RegionSize &= 0xFF;
    ByteSize = (UINT8*) (DsdtPointer + 11);
    *ByteSize = (UINT8) RegionSize;
  } else {
    //EFI_DEADLOOP ();  // Wrong assignment could corrupt the DSDT
    while (TRUE);
  }
  
  return EFI_SUCCESS;
}  

//
// The high 2 bits of the first byte reveal how many follow bytes are in the
// PkgLength.
// If the PkgLength has only one byte, bit 0 through 5 are used to encode the
// package length (in other words, values 0-63). If the package length value is more than
// 63, more than one byte must be used for the encoding in which case bit 4 and 5 of the
// PkgLeadByte are reserved and must be zero. If the multiple bytes encoding is used,
// bits 0-3 of the PkgLeadByte become the least significant 4 bits of the resulting
// package length value. The next ByteData will become the next least significant 8 bits
// of the resulting value and so on, up to 3 ByteData bytes. Thus, the maximum package
// length is 2**28
//
/**
 Check if the object is Name(ABCD, Package(){})
 
 @param[in, out]  Dsdt             The adress of Dsdt Pointer. 
                                             On return, the Dsdt pointer will be updated to the adress of PackageOp plus 1
 @param[out]      PkgLength      The adress of the package length.
                                             On return, the package length will be updated to the PkgLength.
 @param[out]      NumElement   The adress of the package element number.
                                             On return, the package element number will be updated to the NumElement.

 @retval    TRUE    If the object is Name(ABCD, Package(){}), the *PkgLength, *NumElement will be updated
*/
BOOLEAN
IsAmlNamePackageObject (
  IN OUT UINT8           **DsdtPointer,
  OUT UINT64             *PkgLength,
  OUT UINT8              *NumElement
  )
{
  UINT8                 *Operation = NULL;
  UINT64                TempLength = 0;
  UINT8                 PackageBytes;
  UINT8                 Index;
  
  
  Operation = (UINT8*)((*DsdtPointer) - 1);
  if ((*Operation == AML_NAME_OP) && (*(Operation + 5) == AML_PACKAGE_OP)) {
    Operation += 6;
    PackageBytes = ((*Operation) >> 6) + 1;
    *PkgLength = (UINT64)(*Operation) & 0x3F;
    if (PackageBytes > 1) {
      *PkgLength = (*PkgLength) & 0x0F;
    }
    for (Index = 1; Index < PackageBytes; Index++) {
      TempLength = Operation[Index];
      TempLength = LShiftU64(TempLength, (Index * 8 - 4));
      *PkgLength += TempLength;
    }
    *DsdtPointer = Operation;
    *NumElement = Operation[Index];
    return TRUE;
  } 
  return FALSE;     
}

/**
 Check if the object is Name(ABCD, Package(){})
 
 @param[in]     Dsdt    The Dsdt Pointer. 

 @retval          TRUE    If the object is an object such as Name(ABCD, 0x55AA)
*/
BOOLEAN
IsAmlNameWordObject (
  IN UINT8               *DsdtPointer
  )
{
  UINT8                 *Operation = NULL;
  
  Operation = (UINT8*)(DsdtPointer - 1);
  if ((*Operation == AML_NAME_OP) && (*(DsdtPointer + 4) == AML_WORD_PREFIX)) {
    return TRUE;
  } 
  return FALSE;   	
}

/**
 Update the word value such as 0x55AA in Name(ABCD, 0x55AA)
 
 @param[in]     Dsdt               The Dsdt Pointer. 
 @param[in]     NameValue      A word value to update 

 @retval         EFI_SUCCESS    Always
*/
EFI_STATUS
SetNameWordValue (
  IN UINT8               *DsdtPointer,
  IN UINT16              NameValue
  )
{
  UINT16                        *Address;
  
  Address = (UINT16*) (DsdtPointer + 5);
  *Address = NameValue;
  
  return EFI_SUCCESS;
}  

/**
 Search the AML code to see if there is a string match the StringToSearch
 
 @param[in, out]   Dsdt                 The adress of Dsdt Pointer. 
                                                  On return, if string is found, the Dsdt pointer will be updated after Null End of the String
 @param[in, out]   SizeToSearch      The range to search.
                                                  On return, the *SizeToSearch will be updated as the Dsdt is incremented.
 @param[in]          StringToSearch   The string to be searched with a Null End
 
 @retval    EFI_SUCCESS       If the string found, the *Dsdt, *SizeToSearch will be updated
 @retval    EFI_NOT_FOUND  If the string not found, the *Dsdt, *SizeToSearch will not be updated
*/
EFI_STATUS
SearchAmlString (
  IN OUT UINT8           **DsdtPointer,
  IN OUT UINT64          *SizeToSearch,
  IN CHAR8               *StringToSearch
  )
{
  UINT8                   *SearchPointer;
  UINT8                   *CharPointer;
  UINT64                  SearchSize;
  UINT16                  StringIndex;
  
  SearchPointer = *DsdtPointer;
  SearchSize = *SizeToSearch;
  
  while (SearchSize > 0) {
    if (*SearchPointer == AML_STRING_PREFIX) {
      for (StringIndex = 1, CharPointer = SearchPointer + 1; *CharPointer != 0; StringIndex++, CharPointer++) {
        if (*(SearchPointer + StringIndex) != *CharPointer) {
          break;
        }
      }
      if (*CharPointer == 0) {
        //
        // String Null End encountered, It must be matched
        //
        if (SearchSize < StringIndex) {
          //
          // Out of Scope
          //
          return EFI_NOT_FOUND;
        }
        *SizeToSearch = SearchSize - StringIndex - 1;
        *DsdtPointer = CharPointer + 1;
        return EFI_SUCCESS;
      }
    }
    SearchSize--;
    SearchPointer++;
  }
  return EFI_NOT_FOUND;
}

/**
 Search the AML code to see if there is a DWORD, try to get the dword value back
 
 @param[in, out]   Dsdt                 The adress of Dsdt Pointer. 
                                                  On return, if dword is found, the Dsdt pointer will be updated after the DWORD
 @param[in, out]   SizeToSearch     The range to search.
                                                  On return, the *SizeToSearch will be updated as the Dsdt is incremented.
 @param[out]        ValueToReturn   The addess of the value to be returned
 
 @retval   EFI_SUCCESS       If the DWORD found, the *Dsdt, *SizeToSearch, *ValueToReturn will be updated
 @retval   EFI_NOT_FOUND  If the DWORD not found, the *Dsdt, *SizeToSearch, *ValueToReturn will not be updated
*/
EFI_STATUS
SearchAmlDword (
  IN OUT UINT8           **DsdtPointer,
  IN OUT UINT64          *SizeToSearch,
  OUT UINT32             *ValueToReturn
  )
{
  UINT8                   *SearchPointer;
  UINT32                  *DataPointer;
  UINT64                  SearchSize;
  
  SearchPointer = *DsdtPointer;
  SearchSize = *SizeToSearch;
  
  while (SearchSize > 0) {
    if (*SearchPointer == AML_DWORD_PREFIX) {
      DataPointer = (UINT32*)(SearchPointer + 1);
      if (SearchSize < 5) {
        //
        // Out of Scope
        //
        return EFI_NOT_FOUND;
      }
      *SizeToSearch = SearchSize - 5;
      *ValueToReturn = *DataPointer;
      *DsdtPointer = SearchPointer + 5;
      return EFI_SUCCESS;
    }
    SearchSize--;
    SearchPointer++;
  }
  return EFI_NOT_FOUND;
}

/**
 This function calculates and updates an UINT8 checksum.
 
 @param[in]     Buffer                Pointer to buffer to checksum
 @param[in]     Size                   Number of bytes to checksum
 @param[in]     ChecksumOffset  Offset to place the checksum result in
 
 @retval     EFI_SUCCESS           The function completed successfully.
*/
EFI_STATUS
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
{
  UINT8 Sum;
  UINT8 *Ptr;

  Sum = 0;
  //
  // Initialize pointer
  //
  Ptr = Buffer;

  //
  // set checksum to 0 first
  //
  Ptr[ChecksumOffset] = 0;

  //
  // add all content of buffer
  //
  while (Size--) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  //
  // set checksum
  //
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}
