/** @file
  Library Instance implementation for IRSI FactoryCopy Management

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

#include <Uefi.h>
#include <Pi/PiFirmwareVolume.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/FvRegionAccessLib.h>
#include <Guid/IrsiFeature.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/SystemNvDataGuid.h>
#include "FactoryCopyManagement.h"

VAR_NAME_GUID mSpecificAuthVariables[] =
{
  { EFI_PLATFORM_KEY_NAME,        EFI_GLOBAL_VARIABLE_GUID         },
  { EFI_KEY_EXCHANGE_KEY_NAME,    EFI_GLOBAL_VARIABLE_GUID         },
  { EFI_IMAGE_SECURITY_DATABASE,  EFI_IMAGE_SECURITY_DATABASE_GUID },
  { EFI_IMAGE_SECURITY_DATABASE1, EFI_IMAGE_SECURITY_DATABASE_GUID }
};
  
/**
  Return the variable size (Varible Header size + Variable Name size + Variable Data size)

  @param Variable                The variable pointer started from its VARIABLE_HEADER

  @return                        Size of the variable

**/
UINTN
VariableSize (
  VARIABLE_HEADER *Variable
  )
{
  return sizeof (VARIABLE_HEADER) + Variable->NameSize + Variable->DataSize;
}


/**
  Find the next variable pointer from the current variable

  @param Variable                The variable pointer started from its VARIABLE_HEADER

  @return                        Pointer to the next variable header

**/
VARIABLE_HEADER *
NextVariable (
  VARIABLE_HEADER *Variable
  )
{
  VARIABLE_HEADER *NextVar;

  NextVar = (VARIABLE_HEADER *)((UINT8 *)Variable + VariableSize(Variable));
  return NextVar;
}


/**
  Retrieve the Name of the variable

  @param Variable                The variable pointer started from its VARIABLE_HEADER

  @return                        The starting pointer of the variable name string

**/
CHAR16 *
VariableName (
  VARIABLE_HEADER *Variable
  )
{
  return (CHAR16 *)(Variable + 1);
}


/**
  Check whether a given variable has a valid header or nto

  @param Variable                The variable pointer started from its VARIABLE_HEADER

  @retval TRUE                   The variable header is valid
  @retval FALSE                  The variable header is invalid

**/
BOOLEAN
IsValidVariableHeader (
  VARIABLE_HEADER *Variable
  )
{
  if (Variable == NULL || Variable->StartId != VARIABLE_DATA) {
    return FALSE;
  }
  return TRUE;
}

/**
  Irsi Backup Factory Copy function

  Backup the security related variables to FactoryCopy region

  @param None

  @retval  EFI_SUCCESS           FacotryCopy region is successfully backed up
  @retval  EFI_DEVICE_ERROR      Cannot read Variable or FactoryCopy region
  @return  others                failed to backup FactoryCopy region

**/
BOOLEAN
IsSpecificAuthenticatedVariable (
  VARIABLE_HEADER *Variable
  )
{
  UINTN         Index;

  for (Index = 0; Index < sizeof (mSpecificAuthVariables)/sizeof(mSpecificAuthVariables[0]); Index++) {
    if ( CompareGuid (&Variable->VendorGuid, &mSpecificAuthVariables[Index].VendorGuid) &&
         (StrCmp(VariableName(Variable), mSpecificAuthVariables[Index].VariableName)== 0) ) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Irsi Backup Factory Copy function

  Backup the security related variables to FactoryCopy region

  @param None

  @retval  EFI_SUCCESS           FacotryCopy region is successfully backed up
  @retval  EFI_DEVICE_ERROR      Cannot read Variable or FactoryCopy region
  @return  others                failed to backup FactoryCopy region

**/
EFI_STATUS
CollectSecureBootSettings (
  UINT8 *VariableBuf,
  UINT8 *FactoryCopyBuf,
  UINTN FactoryCopyBufSize
  )
{
  EFI_FIRMWARE_VOLUME_HEADER *FvHeader;
  VARIABLE_HEADER            *Variable;
  UINTN                      VarStoreLen;
  UINTN                      VarSize;
  UINTN                      Index;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)VariableBuf;
  if (!CompareGuid(&FvHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid)) {
    return EFI_NOT_FOUND;
  }
  SetMem(FactoryCopyBuf, FactoryCopyBufSize, 0xff);
  *(UINT32 *)FactoryCopyBuf = FACTORY_COPY_SIGNATURE;
  *(UINT32 *)(FactoryCopyBuf + 4) = (UINT32)FactoryCopyBufSize;

  VarStoreLen = PcdGetBool(PcdUseEcpVariableStoreHeader) ? sizeof(ECP_VARIABLE_STORE_HEADER) : sizeof (VARIABLE_STORE_HEADER);
  Variable = (VARIABLE_HEADER *)(VariableBuf + FvHeader->HeaderLength + VarStoreLen);

  CopyMem (FactoryCopyBuf + 8, VariableBuf, FvHeader->HeaderLength + VarStoreLen);

  Index = FvHeader->HeaderLength + VarStoreLen;
  while (IsValidVariableHeader(Variable)) {
    VarSize = VariableSize (Variable);
    if ( (Variable->State == VAR_ADDED) && IsSpecificAuthenticatedVariable(Variable)) {
      CopyMem (FactoryCopyBuf + Index + 8, Variable, VarSize);
    }
    Index += VarSize;
    if (Index >= FactoryCopyBufSize) {
      break;
    }
    Variable = NextVariable (Variable);
  }

  return EFI_SUCCESS;
}

/**
  Irsi Backup Factory Copy function

  Backup the security related variables to FactoryCopy region

  @param None

  @retval  EFI_SUCCESS           FacotryCopy region is successfully backed up
  @retval  EFI_DEVICE_ERROR      Cannot read Variable or FactoryCopy region
  @return  others                failed to backup FactoryCopy region

**/
EFI_STATUS
BackupFactoryCopy(
  VOID
  )
{
  EFI_STATUS   Status;
  UINT8        *IrsiBuf;
  UINT8        *VariableBuf;
  UINT8        *FactoryCopyBuf;
  UINTN        IrsiBufSize;
  UINTN        VariableImageSize;
  UINTN        FactoryCopyImageSize;

  VariableImageSize     = 0;
  FactoryCopyImageSize  = 0;
  VariableBuf           = NULL;
  FactoryCopyBuf        = NULL;


  Status = ReadFvRegion(&gIrsiVariableImageGuid, &VariableImageSize, VariableBuf, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return EFI_DEVICE_ERROR;
  }
  Status = ReadFvRegion(&gIrsiFactoryCopyImageGuid, &FactoryCopyImageSize, FactoryCopyBuf, NULL);
  if ((Status != EFI_BUFFER_TOO_SMALL)||(VariableImageSize < FactoryCopyImageSize)) {
    return EFI_DEVICE_ERROR;
  }
  
  IrsiGetRuntimeBuffer ((VOID **)&IrsiBuf, &IrsiBufSize);
  if (IrsiBufSize < (VariableImageSize + FactoryCopyImageSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }
  VariableBuf    = IrsiBuf;
  FactoryCopyBuf = IrsiBuf + VariableImageSize;
  Status = ReadFvRegion(&gIrsiVariableImageGuid, &VariableImageSize, VariableBuf, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }


  Status = CollectSecureBootSettings (VariableBuf, FactoryCopyBuf, FactoryCopyImageSize);
  if (Status == EFI_SUCCESS) {
    Status = WriteFvRegion(&gIrsiFactoryCopyImageGuid, FactoryCopyImageSize, FactoryCopyBuf, NULL);
  }
  return Status;
}


/**
  Irsi Restore Factory Copy function

  Restore the factory default security related variables to Variable Region

  @param None

  @retval  EFI_SUCCESS           Security related variable has been successfully restored
  @retval  EFI_NOT_FOUND         Unable to find factory copy region
  @retval  EFI_DEVICE_ERROR      Factory copy restoration failed

**/
EFI_STATUS
RestoreFactoryCopy(
  VOID
  )
{
  EFI_STATUS   Status;
  UINT8        *IrsiBuf;
  UINT8        *VariableBuf;
  UINT8        *FactoryCopyBuf;
  UINTN        IrsiBufSize;
  UINTN        VariableImageSize;
  UINTN        FactoryCopyImageSize;
  
  VariableImageSize     = 0;
  FactoryCopyImageSize  = 0;
  VariableBuf           = NULL;
  FactoryCopyBuf        = NULL;
  Status = ReadFvRegion(&gIrsiVariableImageGuid, &VariableImageSize, VariableBuf, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  Status = ReadFvRegion(&gIrsiFactoryCopyImageGuid, &FactoryCopyImageSize, FactoryCopyBuf, NULL);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (VariableImageSize < FactoryCopyImageSize)) {
    return Status;
  }
  
  IrsiGetRuntimeBuffer ((VOID **)&IrsiBuf, &IrsiBufSize);
  if (IrsiBufSize < (VariableImageSize + FactoryCopyImageSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }
  VariableBuf    = IrsiBuf;
  FactoryCopyBuf = IrsiBuf + VariableImageSize;

  Status = ReadFvRegion(&gIrsiVariableImageGuid, &VariableImageSize, VariableBuf, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = ReadFvRegion(&gIrsiFactoryCopyImageGuid, &FactoryCopyImageSize, FactoryCopyBuf, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if ( (*(UINT32 *)FactoryCopyBuf) == FACTORY_COPY_SIGNATURE) {
    SetMem(VariableBuf, PcdGet32(PcdFlashNvStorageVariableSize), 0xFF);
    CopyMem(
          VariableBuf,
          FactoryCopyBuf + FACTORY_COPY_HEADER_SIZE,
          FactoryCopyImageSize - FACTORY_COPY_HEADER_SIZE
         );

    Status = WriteFvRegion(&gIrsiVariableImageGuid, VariableImageSize, VariableBuf, NULL);
  } else {
    Status = EFI_NOT_FOUND;
  }
  return Status;
}

/**
  Irsi Factory Copy Management function

  @param FactoryCopyManagementBuf  pointer to IRSI_FACTORY_COPY_MANAGEMENT structure
                                   SubFunction 00: Backup factory copy from variable region
                                   SubFunction 01: Restore factory copy setting to variable region

  @retval EFI_SUCCESS      Function returns successfully
  @retval EFI_UNSUPPORTED  The requested Factory Copy Mangement sub-function is
                           not supported
  @return  others          Failed to execute FactoryCopy management

**/
EFI_STATUS
EFIAPI
IrsiFactoryCopyManagement (
  VOID     *FactoryCopyManagementBuf
  )
{
   IRSI_FACTORY_COPY_MANAGEMENT *FactoryCopyManagement;
   EFI_STATUS                   Status;
   
   FactoryCopyManagement = (IRSI_FACTORY_COPY_MANAGEMENT *)FactoryCopyManagementBuf;
   FactoryCopyManagement->Header.StructureSize = sizeof(IRSI_FACTORY_COPY_MANAGEMENT);
   
   if (FactoryCopyManagement->FactoryCopyCmd == BACKUP_FACTORY_COPY) {
     Status = BackupFactoryCopy();
   } else if (FactoryCopyManagement->FactoryCopyCmd == RESTORE_FACTORY_COPY) {
     Status = RestoreFactoryCopy();
   } else {
     Status = EFI_UNSUPPORTED;
   }
   FactoryCopyManagement->Header.ReturnStatus = IRSI_STATUS(Status);
   
   return Status;
}

/**
  Irsi FactoryCopyManagement Initialization

  This routine is a LibraryClass constructor for FactoryCopyManagement, it will
  register IrsiFactoryCopyManagement function to the IRSI function database

  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiFactoryCopyManagementInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_FACTORY_COPY_MANAGEMENT_COMMAND,
               IrsiFactoryCopyManagement
               );
               
  return Status;
}