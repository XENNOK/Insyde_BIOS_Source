/** @file
  Common variable supports functions for user can extract whole variables
  (including variable header) from variable store.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VARIABLE_SUPPORT_LIB_H_
#define _VARIABLE_SUPPORT_LIB_H_

#include <Uefi.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <SensitiveVariableFormat.h>

#define GET_VARIABLE_NAME_PTR(a)  (CHAR16 *) ((UINTN) (a) + sizeof (VARIABLE_HEADER))

#define MAX_HARDWARE_ERROR_VARIABLE_SIZE   MAX_VARIABLE_SIZE

//
// Put these globally variables here temporarily. If EDKII updates these variables, we
// should remove these definitions.
//
///
/// Globally "PKDefault" variable for the OEM's default public Platform Key Signature Database.
///
#define EFI_PLATFORM_KEY_DEFAULT_NAME        L"PKDefault"
///
/// Globally "KEKDefault" variable for the OEM's default Key Exchange Key Signature Database
///
#define EFI_KEY_EXCHANGE_KEY_DEFAULT_NAME    L"KEKDefault"

///
/// Globally "dbDefault" variable for the OEM's default secure boot signature store.
///
#define EFI_IMAGE_SECURITY_DEFAULT_DATABASE  L"dbDefault"
///
/// Globally "dbxDefault" variable for the OEM's default secure boot blacklist signature store.
///
#define EFI_IMAGE_SECURITY_DEFAULT_DATABASE1 L"dbxDefault"
///
/// Globally "VendorKeys" variable is an 8-bit unsigned integer that defines whether the PK, KEK, db and
/// dbx databases have been modified by anyone other than the platform vendor or a holder of the
/// vendor-provided keys. A value of 0 indicates that someone other than the platform vendor used a
/// mechanism not defined by the UEFI Specification to transition the system to setup mode or to update
/// PK, KEK, db or dbx. Otherwise, the value will be 1.
///
#define VENDOR_KEYS_NAME                     L"VendorKeys"


#define OS_INDICATIONS_SUPPORTED_NAME        L"OsIndicationsSupported"


typedef
BOOLEAN
(*SPECIFIC_VARIABLE) (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );


/**
  This code gets the current status of Variable Store.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @retval EfiRaw             Variable store status is raw.
  @retval EfiValid           Variable store status is valid.
  @retval EfiInvalid         Variable store status is invalid.
**/
VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN CONST VARIABLE_STORE_HEADER *VarStoreHeader
  );


/**
  This code checks if variable header is valid or not.

  @param[in] Variable  Pointer to the Variable Header.

  @retval TRUE         Variable header is valid.
  @retval FALSE        Variable header is not valid.
**/
BOOLEAN
IsValidVariableHeader (
  IN CONST VARIABLE_HEADER   *Variable
  );

/**
  This code gets the size of name of variable.

  @param[in] Variable  Pointer to the Variable Header.

  @return UINTN        Size of variable in bytes.
**/
UINTN
NameSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  );

/**
  This code gets the pointer to the variable data.

  @param[in] Variable  Pointer to the Variable Header.

  @retval NULL         Variable strat id is incorrect.
  @return UINT8*       Pointer to Variable Data.
**/
UINT8 *
GetVariableDataPtr (
  IN CONST VARIABLE_HEADER   *Variable
  );

/**
  This code gets the pointer to the next variable header.

  @param[in] Variable       Pointer to the Variable Header.

  @return VARIABLE_HEADER*  Pointer to next variable header.
**/
VARIABLE_HEADER *
GetNextVariablePtr (
  IN CONST VARIABLE_HEADER   *Variable
  );

/**
  Gets the pointer to the first variable header in given variable store area.

  @param[in] VarStoreHeader Pointer to the Variable Store Header.

  @return VARIABLE_HEADER*  Pointer to the first variable header.
**/
VARIABLE_HEADER *
GetStartPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  );

/**
  Gets the pointer to the end of the variable storage area.

  This function gets pointer to the end of the variable storage
  area, according to the input variable store header.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return VARIABLE_HEADER*   Pointer to the end of the variable storage area.
**/
VARIABLE_HEADER *
GetEndPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  );

/**
  This code gets the size of variable data.

  @param[in] Variable  Pointer to the Variable Header.

  @return UINTN        Size of variable in bytes.
**/
UINTN
DataSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  );

/**
  This code gets the size of variable store.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return UINT32             Total size of variable store.
**/
UINT32
GetVariableStoreSize (
  IN CONST VARIABLE_STORE_HEADER       *VariableStoreHeader
  );

/**
  Gets the size of variable store header.

  @return UINTN  size by byte of variable store header.
**/
UINTN
GetVariableStoreHeaderSize (
  VOID
  );

/**
  According to variable name and GUID to find this variable is whether in input buffer.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Buffer        Pointer to variable buffer.
  @param[in] BufferSize    the size in bytes of the buffer

  @retval TRUE             The function completed successfully.
  @retval FALSE            The function could not complete successfully.
**/
BOOLEAN
DoesVariableExist (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN CONST  UINT8                              *Buffer,
  IN        UINTN                              BufferSize
  );

/**
  According to variable name and GUID to Determine the variable is secure database relative variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is secure database relative variable.
  @retval     FALSE          This isn't secure database relative variable.
--*/
BOOLEAN
IsSecureDatabaseVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is PK or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is PK variable.
  @retval     FALSE          This isn't PK variable.
--*/
BOOLEAN
IsPkVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is KEK or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is KEK variable.
  @retval     FALSE          This isn't KEK variable.
--*/
BOOLEAN
IsKekVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is db or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is db variable.
  @retval     FALSE          This isn't db variable.
--*/
BOOLEAN
IsDbVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is dbx or not.

  @param  VariableName   Name of Variable to be found.
  @param  VendorGuid     Variable vendor GUID.

  @retval TRUE           This is dbx variable.
  @retval FALSE          This isn't dbx variable.

--*/
BOOLEAN
IsDbxVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is secure database default
  relative variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is secure database default relative variable.
  @retval     FALSE          This isn't secure database default relative variable.
--*/
BOOLEAN
IsSecureDatabaseDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is PKDefault or not.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This is PKDefault variable.
  @retval FALSE            This isn't PKDefault variable.
--*/
BOOLEAN
IsPkDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is KEKDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is KEKDefault variable.
  @retval     FALSE          This isn't KEKDefault variable.
--*/
BOOLEAN
IsKekDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is dbDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is dbDefault variable.
  @retval     FALSE          This isn't dbDefault variable.
--*/
BOOLEAN
IsDbDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  According to variable name and GUID to Determine the variable is dbxDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is dbxDefault variable.
  @retval     FALSE          This isn't dbxDefault variable.
--*/
BOOLEAN
IsDbxDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  Merge all of variables aren't secure boot relative variables and doesn't saved in factory copy
  region from variable store to factory copy region.

  @param[in, out]  FactorycopyBuffer      [in]:The buffer contained all of factory copy data.
                                          [out]:The buffer has been appended data from VariableBuffer.
  @param[in, out]  FactoryBufferLength    [in]:The size of FactorycopyBuffer.
                                          [out]:The total used size in FactorycopyBuffer.
  @param[in]       VariableBuffer         Buffer to save all of variable data.
  @param[in]       VariableBufferLength   The size of VariableBuffer.

  @retval EFI_SUCCESS                     Merge varialbe data to factory copy region successful.
  @retval EFI_INVALID_PARAMETER           FactorycopyBuffer, FactoryBufferLength or VariableBuffer is NULL.
                                          The data in FactorycopyBuffer in incorrect.
  @retval EFI_BUFFER_TOO_SMALL            FactoryBufferLength is too small to contain merged data.

--*/
EFI_STATUS
MergeVariableToFactoryCopy (
  IN OUT   UINT8        *FactorycopyBuffer,
  IN OUT   UINTN        *FactoryBufferLength,
  IN CONST UINT8        *VariableBuffer,
  IN       UINTN        VariableBufferLength
  );

/**
  This code uses to initialize secure variable header.
  User can use this function set variable header to set Insyde secure variable.

  @param[in]  DataSize           Input buffer size by byte.
  @param[out] Data               Buffer to save initialized variable header data.

  @retval EFI_SUCCESS            Setting secure variable header successful.
  @retval EFI_INVALID_PARAMETER  Data is NULL or DataSize is too small.
--*/
EFI_STATUS
InitSecureVariableHeader (
  IN   UINTN          DataSize,
  OUT  VOID           *Data
  );

/**
  According to variable name and GUID to Determine the variable is whether a read-only variable.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             Setting secure variable header successful.
  @retval FALSE            Data is NULL or DataSize is too small.
--*/
BOOLEAN
IsReadOnlyVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
;

/**
  Function to get password combined data.

  @param[in]  Data               Buffer to save input data.
  @param[in]  DataLen            Length of input data in bytes.
  @param[out] CombinedData       Output buffer to save combined data.

  @retval EFI_SUCCESS            Get Combined data successfully.
  @retval EFI_INVALID_PARAMETER  Data is NULL or CombinedData is NULL.
--*/
EFI_STATUS
GetCombinedData (
  IN  CONST UINT8      *Data,
  IN        UINTN      DataLen,
  OUT       CHAR8      *CombinedData
  )
;

/**
  Function uses to get next sensitive variable and return the current variable number in sensitive variable.

  @param[in]       StoreHeader         Pointer to SENSITIVE_VARIABLE_STORE_HEADER instance.
  @param[in]       VariableHeader      Pointer to SENSITIVE_VARIABLE_HEADER instance.
  @param[in, out]  VariableNum         [in]:  Supplies the last variable number returned by GetNextSensitiveVariable().
                                       [out]: Returns the variable number of the current variable.

  @return        Pointer to SENSITIVE_VARIABLE_HEADER instance, or NULL if cannot find next sensitive variable.
  --*/
SENSITIVE_VARIABLE_HEADER *
GetNextSensitiveVariable (
  IN      CONST SENSITIVE_VARIABLE_STORE_HEADER     *StoreHeader,
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader,
  IN OUT        UINT16                              *VariableNum
  )
;


/**
  This code gets the pointer to the sensitive variable data.

  @param[in] Variable  Pointer to SENSITIVE_VARIABLE_HEADER instance.

  @return   Pointer to sensitive variable data, or NULL if VariableHeader is invalid.

**/
UINT8 *
GetSensitiveVariableDataPtr (
  IN CONST SENSITIVE_VARIABLE_HEADER   *Variable
  )
;

/**
  Get whole single sensitive variable size. This size includes header size, name size and data size.

  @param[in] VariableHeader  Pointer to SENSITIVE_VARIABLE_HEADER instance.

  @return    Size of sensitive variable data size in bytes, or 0 if the VariableHeader is invalid.
--*/
UINTN
GetSensitiveVariableSize (
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader
  )
;

#endif