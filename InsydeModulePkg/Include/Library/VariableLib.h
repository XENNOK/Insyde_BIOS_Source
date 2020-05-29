/** @file
  Header file for Variable Library Class

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*****************************************************************************3*
*/

#ifndef _VARIABLE_LIB_H_
#define _VARIABLE_LIB_H_

#include <SensitiveVariableFormat.h>

/**
  Read the EFI variable (VariableName/VendorGuid) according to input varialbe data size.

  @param[in]      VariableName       String part of EFI variable name.
  @param[in]      VendorGuid         GUID part of EFI variable name.
  @param[in, out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out]     VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                     Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
  @return Other                      Other errors cause get variable failed.
**/
EFI_STATUS
CommonGetVariable (
  IN     CHAR16        *VariableName,
  IN     EFI_GUID      *VendorGuid,
  IN OUT UINTN         *VariableDataSize,
  OUT    VOID          *VariableData
  );

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated buffer.

  @param[in]  VariableName  String part of EFI variable name.
  @param[in]  VendorGuid    GUID part of EFI variable name.

  @retval NULL              The variable could not be retrieved.
  @retval NULL              There are not enough resources available for the variable contents.
  @retval Other             A pointer to allocated buffer containing the variable contents.
**/
VOID *
CommonGetVariableData (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VendorGuid
  );

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName       String part of EFI variable name.
  @param[in]  VendorGuid         GUID part of EFI variable name.
  @param[out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out] VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                 Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS            Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                 is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific EFI variable.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory for read variable data failed.
  @return Other                  Other errors cause get variable failed.
**/
EFI_STATUS
CommonGetVariableDataAndSize (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData
  );

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName             String part of EFI variable name.
  @param[in]  VendorGuid               GUID part of EFI variable name.
  @param[out] VariableDataSize         Returns the size of the EFI variable that was read.
  @param[out] VariableData             Dynamically allocated memory that contains a copy of the EFI variable.
                                       Caller is responsible freeing the buffer.
  @param[in]  DefaultVariableDataSize  Default variable data size.
  @param[in]  DefautlVariableData      Pointer to default variable data.

  @retval TRUE                         The variable exists and the VariableDataSize and VariableData is filled by variable
                                       data size and variable data respectively.
  @retval FALSE                        The variable doesn't exit and the VariableDataSize and VariableData is filled by
                                       default variable data size and default variable data respectively.
**/
BOOLEAN
CommonGetVariableWithDefault (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData,
  IN  UINTN               DefaultVariableDataSize,
  IN  VOID                *DefautlVariableData
  );

/**
  Enumerates the current variable names.

  @param[in,out]  VariableNameSize    The size of the VariableName buffer.
  @param[in,out]  VariableName        On input, supplies the last VariableName that was returned by GetNextVariableName().
                                      On output, returns the Null-terminated string of the current variable.
  @param[in,out]  VendorGuid          On input, supplies the last VendorGuid that was returned by GetNextVariableName().
                                      On output, returns the VendorGuid of the current variable.

  @retval EFI_SUCCESS                 The function completed successfully.
  @retval EFI_NOT_FOUND               The next variable was not found.
  @retval EFI_BUFFER_TOO_SMALL        The VariableNameSize is too small for the result. VariableNameSize has been updated
                                      with the size needed to complete the request.
  @retval EFI_INVALID_PARAMETER       VariableNameSize is NULL or VariableName is NULL or VendorGuid is NULL.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
**/
EFI_STATUS
CommonGetNextVariableName (
  IN OUT UINTN       *VariableNameSize,
  IN OUT CHAR16      *VariableName,
  IN OUT EFI_GUID    *VendorGuid
  );

/**
  Sets the value of a variable.

  @param[in] VariableName         String part of EFI variable name.
  @param[in] VendorGuid           GUID part of EFI variable name.
  @param[in] Attributes           Attributes bitmask to set for the variable
  @param[in] DataSize             The size in bytes of the Data buffer.
  @param[in] Data                 The contents for the variable.

  @retval EFI_SUCCESS                 The firmware has successfully stored the variable and its data as defined by
                                      the Attributes.
  @retval EFI_INVALID_PARAMETER       An invalid combination of attribute bits, name, and GUID  was supplied, or the
                                      DataSize exceeds the maximum allowed or name is a empty string.
  @retval EFI_OUT_OF_RESOURCES        Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
  @retval EFI_WRITE_PROTECTED         The variable in question is read-only or the variable cannot be deleted.
  @retval EFI_SECURITY_VIOLATION      The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS or
                                      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACESS being set, but the AuthInfo does
                                      NOT pass the validation check carried out by the firmware.
**/
EFI_STATUS
CommonSetVariable (
  IN CHAR16      *VariableName,
  IN EFI_GUID    *VendorGuid,
  IN UINT32      Attributes,
  IN UINTN       DataSize,
  IN VOID        *Data
  );

/**
  Set changed sensitive setup data to sensitive setup variable.

  @param[in]  VariableName       A pointer to a null-terminated string that is the variable's name.
  @param[in]  VendorGuid         A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                 VendorGuid and VariableName must be unique.
  @param[in]  Attributes         Attributes bitmask to set for the variable.
  @param[out] DataSize           The data size by byte of variable
  @param[out] Data               Pointer to the start address of data.

  @retval EFI_SUCCESS            Set variable from sensitive variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval EFI_NOT_FOUND          Cannot set specific variable in sensitive setup variable.
**/
EFI_STATUS
SetVariableToSensitiveVariable (
  IN   CONST   CHAR16                           *VariableName,
  IN   CONST   EFI_GUID                         *VendorGuid,
  IN           UINT32                           Attributes,
  IN           UINTN                            DataSize,
  IN           VOID                             *Data
  );

#endif
