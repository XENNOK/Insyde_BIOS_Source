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

#ifndef _ACPI_PLATFORM_LIBRARY_H_
#define _ACPI_PLATFORM_LIBRARY_H_
#include <IndustryStandard/Acpi.h>


#define AML_OPREGION_OP     0x805b
// #define AML_METHOD_OP       0x14
// #define AML_NAME_OP         0x08
// #define AML_BYTE_PREFIX     0x0A
// #define AML_WORD_PREFIX     0x0B
// #define AML_PACKAGE_OP      0x12
// #define AML_STRING_PREFIX   0x0D
// #define AML_DWORD_PREFIX    0x0C


BOOLEAN
IsAmlOpRegionObject (
  IN UINT8               *DsdtPointer
  );

BOOLEAN
IsAmlMethodObject (
  IN UINT8               *DsdtPointer,
  OUT UINT64             *PkgLength
  );

/**

  Update the OperationRegion of a system memory's offset and size

  @param  DsdtPointer         A pointer to the address where the Operation's Name. 
                              Assume it is four letters in ASL, such as 'GNVS'
  @param  RegionStartAddress  The address where the Operation Region allocated in system memory (AcpiNvsArea).
                              It is assume to be a DWORD.
  @param  NumElement          The adress of the package element number.
                              On return, the package element number will be updated to the NumElement.

  @return TRUE  If the object is Name(ABCD, Package(){}), the *PkgLength, *NumElement will be updated

**/
EFI_STATUS
SetOpRegion (
  IN UINT8               *DsdtPointer,
  IN VOID*               RegionStartAddress,
  IN UINT32              RegionSize
  );

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

  @param  Dsdt        The adress of Dsdt Pointer. 
                      On return, the Dsdt pointer will be updated to the adress of PackageOp plus 1
  @param  PkgLength   The adress of the package length.
                      On return, the package length will be updated to the PkgLength.
  @param  NumElement  The adress of the package element number.
                      On return, the package element number will be updated to the NumElement.

  @return TRUE        If the object is Name(ABCD, Package(){}), the *PkgLength, *NumElement will be updated

**/
BOOLEAN
IsAmlNamePackageObject (
  IN OUT UINT8           **DsdtPointer,
  OUT UINT64             *PkgLength,
  OUT UINT8              *NumElement
  );

/**

  Check if the object is Name(ABCD, Package(){})

  @param  DsdtPointer  The Dsdt Pointer. 
  
  @return TRUE         If the object is an object such as Name(ABCD, 0x55AA)

**/
BOOLEAN
IsAmlNameWordObject (
  IN UINT8               *DsdtPointer
  );

/**

  Update the word value such as 0x55AA in Name(ABCD, 0x55AA)

  @param  DsdtPointer  The Dsdt Pointer. 
  @param  NameValue    A word value to update 
  
  @return EFI_SUCCESS  Always

**/
EFI_STATUS
SetNameWordValue (
  IN UINT8               *DsdtPointer,
  IN UINT16              NameValue
  );

/**

  Search the AML code to see if there is a string match the StringToSearch

  @param  DsdtPointer    The adress of Dsdt Pointer. 
                         On return, if dword is found, the Dsdt pointer will be updated after the DWORD
  @param  SizeToSearch   The range to search.
                         On return, the *SizeToSearch will be updated as the Dsdt is incremented.
  @param  ValueToReturn  The addess of the value to be returned
  
  @return EFI_SUCCESS   - If the DWORD found, the *Dsdt, *SizeToSearch, *ValueToReturn will be updated
  @return EFI_NOT_FOUND - If the DWORD not found, the *Dsdt, *SizeToSearch, *ValueToReturn will not be updated

**/
EFI_STATUS
SearchAmlString (
  IN OUT UINT8           **DsdtPointer,
  IN OUT UINT64          *SizeToSearch,
  IN CHAR8               *StringToSearch
  );

/**

  Search the AML code to see if there is a DWORD, try to get the dword value back

  @param  DsdtPointer    The adress of Dsdt Pointer. 
                         On return, if dword is found, the Dsdt pointer will be updated after the DWORD
  @param  SizeToSearch   The range to search.
                         On return, the *SizeToSearch will be updated as the Dsdt is incremented.
  @param  ValueToReturn  The addess of the value to be returned
  
  @return EFI_SUCCESS   - If the DWORD found, the *Dsdt, *SizeToSearch, *ValueToReturn will be updated
  @return EFI_NOT_FOUND - If the DWORD not found, the *Dsdt, *SizeToSearch, *ValueToReturn will not be updated

**/
EFI_STATUS
SearchAmlDword (
  IN OUT UINT8           **DsdtPointer,
  IN OUT UINT64          *SizeToSearch,
  OUT UINT32             *ValueToReturn
  );

/**
 This function calculates and updates an UINT8 checksum.

 @param [in]   Buffer           Pointer to buffer to checksum
 @param [in]   Size             Number of bytes to checksum
 @param [in]   ChecksumOffset   Offset to place the checksum result in

 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  );

#endif
