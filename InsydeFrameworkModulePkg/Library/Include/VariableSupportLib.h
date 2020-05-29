//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   VariableSupport.h
//;
//; Abstract:
//;
//;  Common variable supports functions for user can extract whole variables
//;  (including variable header) from variable store.
//;
//;

#ifndef _VARIABLE_SUPPORT_LIB_H_
#define _VARIABLE_SUPPORT_LIB_H_

#include "Tiano.h"
#include "EfiVariable.h"
#include "SensitiveVariableFormat.h"

#ifndef ALIGNMENT
#define ALIGNMENT             1
#endif

#if ((ALIGNMENT == 0) || (ALIGNMENT == 1))
#define GET_PAD_SIZE(a) (0)
#else
#define GET_PAD_SIZE(a) (((~a) + 1) & (ALIGNMENT - 1))
#endif

#define GET_VARIABLE_NAME_PTR(a)  (CHAR16 *) ((UINTN) (a) + sizeof (VARIABLE_HEADER))

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#define MAX_HARDWARE_ERROR_VARIABLE_SIZE   MAX_VARIABLE_SIZE
#endif

#define OS_INDICATIONS_SUPPORTED_NAME          L"OsIndicationsSupported"


typedef
BOOLEAN
(EFIAPI *SPECIFIC_VARIABLE) (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  );

VARIABLE_STORE_STATUS
EFIAPI
GetVariableStoreStatus (
  IN CONST VARIABLE_STORE_HEADER *VarStoreHeader
  )
/*++

Routine Description:

  This code gets the pointer to the variable name.

Arguments:

  VarStoreHeader  - Pointer to the Variable Store Header.

Returns:

  EfiHealthy      - Variable store is healthy.
  EfiRaw          - Variable store is raw.
  EfiInvalid      - Variable store is invalid.

--*/
;

BOOLEAN
EFIAPI
IsValidVariableHeader (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code checks if variable header is valid or not.

Arguments:

  Variable   - Pointer to the Variable Header.

Returns:

  TRUE       - Variable header is valid.
  FALSE      - Variable header is not valid.

--*/
;

UINT8 *
EFIAPI
GetVariableDataPtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the pointer to the variable data.

Arguments:

  Variable    - Pointer to the Variable Header.

Returns:

  UINT8*      - Pointer to Variable Data.

--*/
;

VARIABLE_HEADER *
EFIAPI
GetNextVariablePtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the pointer to the next variable header.

Arguments:

  Variable             - Pointer to the Variable Header.

Returns:

  VARIABLE_HEADER*     - Pointer to next variable header.

--*/
;

VARIABLE_HEADER *
GetStartPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
/*++

Routine Description:

  Gets the pointer to the first variable header in given variable store area.

Arguments:

  VarStoreHeader        - Pointer to the Variable Store Header.

Returns:

  VARIABLE_HEADER*      - Pointer to the first variable header.

--*/
;

VARIABLE_HEADER *
EFIAPI
GetEndPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
/*++

Routine Description:

  This code gets the pointer to the last variable memory pointer byte

Arguments:

  VarStoreHeader        - Pointer to the Variable Store Header.

Returns:

  VARIABLE_HEADER*      - Pointer to last unavailable Variable Header.

--*/
;

UINTN
DataSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the data size of specific variable.

Arguments:

  Variable       - Pointer to the Variable Header.

Returns:

  UINTN          - Data size of specific variable.

--*/
;

BOOLEAN
DoesVariableExist (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     UINT8                              *Buffer,
  IN     UINTN                              BufferSize
  )
/*++

Routine Description:

  According to variable name and GUID to find this variable is whether in input buffer.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.
  Buffer             - Pointer to variable buffer.
  BufferSize         - the size in bytes of the buffer.

Returns:

  TRUE               - This is in input buffer.
  FALSE              - This isn't in input buffer.

--*/
;

BOOLEAN
IsSecureDatabaseVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is secure database relative variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is secure database relative variable.
  FALSE              - This isn't secure database relative variable.

--*/
;

BOOLEAN
IsPkVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is PK or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is PK variable.
  FALSE              - This isn't PK variable.

--*/
;

BOOLEAN
IsKekVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is KEK or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is KEK variable.
  FALSE              - This isn't KEK variable.

--*/
;

BOOLEAN
IsDbVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is db or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is db variable.
  FALSE              - This isn't db variable.

--*/
;

BOOLEAN
IsDbxVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbx or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbx variable.
  FALSE              - This isn't dbx variable.

--*/
;

BOOLEAN
IsSecureDatabaseDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is secure database default relative variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is secure database default relative variable.
  FALSE              - This isn't secure database default relative variable.

--*/
;

BOOLEAN
IsPkDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is PKDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is PKDefault variable.
  FALSE              - This isn't PKDefault variable.

--*/
;

BOOLEAN
IsKekDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is KEKDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is KEKDefault variable.
  FALSE              - This isn't KEKDefault variable.

--*/
;

BOOLEAN
IsDbDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbDefault variable.
  FALSE              - This isn't dbDefault variable.

--*/
;

BOOLEAN
IsDbxDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbxDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbxDefault variable.
  FALSE              - This isn't dbxDefault variable.

--*/
;

EFI_STATUS
MergeVariableToFactoryCopy (
  IN     UINT8        *FactorycopyBuffer,
  IN OUT UINTN        *FactoryBufferLength,
  IN     UINT8        *VariableBuffer,
  IN     UINTN        VriableBufferLength
  )
/*++

Routine Description:

  Merge all of variables aren't secure boot relative variables and doesn't saved in
  factory copy region from variable store to factory copy region.

Arguments:

  FactorycopyBuffer       - On input, the buffer contained all of factory copy data.
                            On output, the buffer has been appended data from VariableBuffer.
  FactoryBufferLength     - On input, the size of FactorycopyBuffer.
                            On output, the total used size in FactorycopyBuffer.
  VariableBuffer          - Buffer to save all of variable data.
  VariableBufferLength    - The size of VariableBuffer.


Returns:

  EFI_SUCCESS             - Merge varialbe data to factory copy region successful.
  EFI_INVALID_PARAMETER   - FactorycopyBuffer, FactoryBufferLength or VariableBuffer is NULL.
                            The data in FactorycopyBuffer in incorrect.
  EFI_BUFFER_TOO_SMALL    - FactoryBufferLength is too small to contain merged data.

--*/
;

EFI_STATUS
InitSecureVariableHeader (
  IN   UINTN          DataSize,
  OUT  VOID          *Data
  )
/*++

Routine Description:

  This code uses to initialize secure variable header.
  User can use this function set variable header to set Insyde secure variable.

Arguments:

  DataSize               - Input buffer size by byte.
  Data                   - Buffer to save initialized variable header data.

Returns:

  EFI_SUCCESS            - Setting secure variable header successful.
  EFI_INVALID_PARAMETER  - Data is NULL or DataSize is too small.

--*/
;

BOOLEAN
IsReadOnlyVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is whether a read-only variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is a read-only variable.
  FALSE              - This isn't a read-only variable.

--*/
;

EFI_STATUS
GetCombinedData (
  IN  UINT8      *Data,
  IN  UINTN      DataLen,
  OUT CHAR8      *CombinedData
  )
/*++

Routine Description:

  Function to get password combined data.

Arguments:

  Data                   - Buffer to save input data.
  DataLen                - Length of input data by bytes.
  CombinedData           - Output buffer to save combined data.

Returns:

  EFI_SUCCESS            - Get Combined data successfully.
  EFI_INVALID_PARAMETER  - Data is NULL or CombinedData is NULL.

--*/
;

SENSITIVE_VARIABLE_HEADER *
GetNextSensitiveVariable (
  IN      CONST SENSITIVE_VARIABLE_STORE_HEADER     *StoreHeader,
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader,
  IN OUT        UINT16                              *VariableNum
  )
/*++

Routine Description:

  Function uses to get next sensitive variable and return the current variable number in sensitive variable.

Arguments:

  StoreHeader                  - Pointer to SENSITIVE_VARIABLE_STORE_HEADER instance.
  VariableHeader               - Pointer to SENSITIVE_VARIABLE_HEADER instance.
  VariableNum                  - [in]:  Supplies the last variable number returned by GetNextSensitiveVariable().
                                 [out]: Returns the variable number of the current variable.

Returns:

  Pointer to SENSITIVE_VARIABLE_HEADER instance, or NULL if cannot find next sensitive variable.

--*/
;

UINT8 *
GetSensitiveVariableDataPtr (
  IN CONST SENSITIVE_VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the pointer to the sensitive variable data.

Arguments:

  Variable  - Name of the variable to be found.

Returns:

  Pointer to sensitive variable data, or NULL if VariableHeader is invalid.

--*/
;

UINTN
GetSensitiveVariableSize (
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader
  )
/*++

Routine Description:

  Get whole single sensitive variable size. This size includes header size, name size and data size.

Arguments:

  VariableHeader  - Pointer to SENSITIVE_VARIABLE_HEADER instance.

Returns:

  Size of sensitive variable data size in bytes, or 0 if the VariableHeader is invalid.

--*/
;

#endif