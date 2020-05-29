/** @file
  Platform Level Data Model for SMBIOS Data Transfer implementation

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

#include "PldmSmbios.h"


/**
 Send SBIOS Structure Table Meta Data to the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This
  )
{
  PLDM_CONTEXT *Context;
  SMBIOS_TABLE_ENTRY_POINT *SmbiosEntry;
  EFI_STATUS Status;
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA PldmRequest;


  Context = PLDM_CONTEXT_FROM_THIS(This);
  
  Status = EfiGetSystemConfigurationTable (
           &gEfiSmbiosTableGuid,
           (VOID **)&SmbiosEntry
           );
  EFI_ERROR_RETURN (Status);         

  
  PldmRequest.MajorVersion             = SmbiosEntry->MajorVersion;
  PldmRequest.MinorVersion             = SmbiosEntry->MinorVersion;
  PldmRequest.MaxStructureSize         = SmbiosEntry->MaxStructureSize;
  PldmRequest.TableLength              = Context->SmbiosTableLen; //SmbiosEntry->TableLength;
  PldmRequest.NumberOfSmbiosStructures = SmbiosEntry->NumberOfSmbiosStructures;
  PldmRequest.Crc32                    = Context->SmbiosCrc32;

                                                             
                                                             
  return ProcessSimplePldmMessage( This, 
                                   PLDM_TYPE_SMBIOS, 
                                   PLDM_SET_SMBIOS_STRUCTURE_TABLE_METADATA,
                                   &PldmRequest,
                                   sizeof (PldmRequest),
                                   NULL,
                                   0
                                 );                            
}

/**
 Receive SBIOS Structure Table Meta Data from the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL
 @param        PldmResponse     The received SMBIOS Structure Table Meta Data from the management controller

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This,
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA          *PldmResponse
  )
{
  UINTN      Size;

  Size = sizeof (PLDM_SMBIOS_STRUCTURE_TABLE_METADATA);
  return ProcessSimplePldmMessage( This, 
                                     PLDM_TYPE_SMBIOS, 
                                     PLDM_GET_SMBIOS_STRUCTURE_TABLE_METADATA,
                                     NULL,
                                     0,
                                     PldmResponse,
                                     &Size
                                   );      
}



/**
 Receive SBIOS Structure Table from the management controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [out]  SmbiosBuf        Buffer storing the SMBIOS Sturcture Table
 @param [in, out] Size          Number of bytes received for SmbiosBuf

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTable (
  IN     EFI_PLDM_PROTOCOL *This,
  OUT    UINT8             *SmbiosBuf,
  IN OUT UINTN             *Size
  )
{
  PLDM_TRANSFER_HEADER TransferHeader;
  
  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_SMBIOS, 
                                                PLDM_GET_SMBIOS_STRUCTURE_TABLE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                SmbiosBuf,
                                                Size
                                              );
}

/**
 Send SBIOS Structure Table to the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTable(
  EFI_PLDM_PROTOCOL                              *This
  )
{
  EFI_STATUS Status;
  SMBIOS_TABLE_ENTRY_POINT *SmbiosEntry;
  UINT8 *SmbiosBuf;
  UINTN Size;
  UINTN Index;
  PLDM_TRANSFER_HEADER TransferHeader;
  EFI_CPU_IO_PROTOCOL *CpuIo;


  Status = gBS->LocateProtocol (&gEfiCpuIoProtocolGuid, NULL, (VOID **)&CpuIo);
  EFI_ERROR_RETURN (Status);
  
  Status = EfiGetSystemConfigurationTable (
           &gEfiSmbiosTableGuid,
           (VOID **)&SmbiosEntry
           );
  EFI_ERROR_RETURN (Status);
  
  if ((SmbiosEntry->TableLength % 4) == 0) {
    Size = SmbiosEntry->TableLength;
  } else {
    Size = SmbiosEntry->TableLength + (4 - (SmbiosEntry->TableLength % 4));
  }
   
  Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **)&SmbiosBuf);
  EFI_ERROR_RETURN (Status);

  Status = CpuIo->Mem.Read (CpuIo, EfiCpuIoWidthUint8, SmbiosEntry->TableAddress, SmbiosEntry->TableLength, SmbiosBuf);
  if(EFI_ERROR(Status)) {
    gBS->FreePool(SmbiosBuf);
    return Status;
  }

  for (Index = SmbiosEntry->TableLength; Index < Size; Index++) {
    SmbiosBuf[Index] = 0;
  }


  Status = ProcessPldmMessageWithSplitRequests ( This,
                                               PLDM_TYPE_SMBIOS,
                                               PLDM_SET_SMBIOS_STRUCTURE_TABLE,
                                               &TransferHeader,
                                               sizeof(TransferHeader),
                                               SmbiosBuf,
                                               Size
                                             );

  gBS->FreePool(SmbiosBuf);

  return Status;
  
}

