/** @file
  This module provides default Setup variable data if Setupvariable is not found
  and also provides a Setup variable cache mechanism in PEI phase

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _SETUP_DEFAULT_PROVIDER_H_
#define _SETUP_DEFAULT_PROVIDER_H_

#define SETUP_VARIABLE_NAME    L"Setup"
#define MAX_VFR_NUMBER    6

typedef struct {
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VariablePpi;
  UINTN                              SetupDataSize;
  UINT8                              SetupData[1];
} SETUP_DATA_PROVIDER_CONTEXT;

/**
  Provide the get next variable functionality of the variable services.

  @param  This              A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
  @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                            On return, points to the next variable's null-terminated name string.

  @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID.
                            On return, a pointer to the next variable's GUID.

  @retval EFI_SUCCESS       The interface could be successfully installed
  @retval EFI_NOT_FOUND     The variable could not be discovered

**/
EFI_STATUS
EFIAPI
NewPeiNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  );

/**
  Provide the revised read variable functionality of the variable services.

  @param  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.

  @retval EFI_SUCCESS           The interface could be successfully installed
  @retval EFI_NOT_FOUND         The variable could not be discovered
  @retval EFI_BUFFER_TOO_SMALL  The caller buffer is not large enough

**/
EFI_STATUS
EFIAPI
NewPeiGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *This,
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT UINT32                                *Attributes OPTIONAL,
  IN OUT UINTN                              *DataSize,
  OUT VOID                                  *Data
  );

#pragma pack(push, 1)
typedef struct {
  UINT8  Id[7];
  UINT8  EZH2OVersion;
  UINT32 NumberOfVfrs;
  UINT64 VfrDefaultOffset[MAX_VFR_NUMBER];
} IFR_SETUP_DEFAULT;
#pragma pack(pop)

extern UINT8  BootVfrSystemConfigDefault0000[];
extern UINT8  PowerVfrSystemConfigDefault0000[];
extern UINT8  SecurityVfrSystemConfigDefault0000[];
extern UINT8  AdvanceVfrSystemConfigDefault0000[];
extern UINT8  MainVfrSystemConfigDefault0000[];


#endif

