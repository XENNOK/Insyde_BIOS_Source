/** @file
  Platform Level Data Model for BIOS Control and Configuration Implementation

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

#include "PldmBios.h"

/**
 Set Date Time command of PLDM for BIOS Control & Configuration Specification.

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmBiosSetDateTime(
  IN EFI_PLDM_PROTOCOL   *This
)
{
  PLDM_DATE_TIME DateTime;
  EFI_TIME       Time;

  gRT->GetTime (&Time, NULL);

  DateTime.Day    = DecimalToBcd8(Time.Day);
  DateTime.Second = DecimalToBcd8 (Time.Second);
  DateTime.Minute = DecimalToBcd8 (Time.Minute);
  DateTime.Hour   = DecimalToBcd8 (Time.Hour);
  DateTime.Month  = DecimalToBcd8 (Time.Month);
  DateTime.Year   = 0x100 * DecimalToBcd8 ((UINT8)(Time.Year/100)) + DecimalToBcd8 ((UINT8)(Time.Year % 100));
  
  return ProcessSimplePldmMessage( This, 
                                   PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION, 
                                   PLDM_SET_DATE_TIME,
                                   &DateTime,
                                   sizeof (DateTime),
                                   NULL,
                                   0
                                 );                            
  
}


/**
 Get BIOS Table stored in Management Controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   TableType        00: BIOS String Table,  01: BIOS Attribute Table,  02: BIOS Attribute Value Table
 @param [out]  Table            The BIOS Table buffer to be received
 @param [out]  TableSize        Number of bytes of Table

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosTable (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             TableType,
  OUT VOID              *Table,
  OUT UINTN             *TableSize
  )
{
  PLDM_BIOS_TABLE_TRANSFER_HEADER TransferHeader;

  TransferHeader.TableType = TableType;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                                PLDM_GET_BIOS_TABLE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                Table,
                                                TableSize
                                               );
                                        
 
}

/**
 Set BIOS Table to the Management Controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   TableType        00: BIOS String Table,  01: BIOS Attribute Table,  02: BIOS Attribute Value Table
 @param [in]   Table            The BIOS Table buffer to be transferred to mangement controller
 @param [in]   TableSize        Number of bytes of Table

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetBiosTable (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             TableType,
  IN  VOID              *Table,
  IN  UINTN             TableSize
)
{
  PLDM_BIOS_TABLE_TRANSFER_HEADER TransferHeader;

  TransferHeader.TableType = TableType;

  return ProcessPldmMessageWithSplitRequests (This,
                                              PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                              PLDM_SET_BIOS_TABLE,
                                              &TransferHeader,
                                              sizeof (TransferHeader),
                                              Table,
                                              TableSize
                                             );
                                             
}


/**
 Get BIOS Table Tags stored in the Management Controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   NumberOfTables   Number of tables
 @param [in]   TableTypes       Byte array to stored the table types
 @param [out]  TableTags        DWORD array of the retrieved BIOS Table Tags

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosTableTags (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8 NumberOfTables,
  IN  UINT8 *TableTypes,
  OUT UINT32 *TableTags
  )
{
  UINT8 *RequestBuf;
  EFI_STATUS Status;
  UINTN ResponseMsgSize;

  Status = gBS->AllocatePool (EfiBootServicesData, NumberOfTables + 1, (VOID **)&RequestBuf);
  EFI_ERROR_RETURN (Status);
  
  RequestBuf[0] = NumberOfTables;
  gBS->CopyMem (RequestBuf + 1, TableTypes, NumberOfTables);

  ResponseMsgSize = (UINTN)NumberOfTables * sizeof (UINT32);
  Status = ProcessSimplePldmMessage( This, 
                                     PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION, 
                                     PLDM_GET_BIOS_TAGS,
                                     RequestBuf,
                                     (UINTN)NumberOfTables + 1,
                                     TableTags,
                                     &ResponseMsgSize
                                   );    

  gBS->FreePool (RequestBuf);
  return Status;

}

/**
 Set BIOS Table Tags to the Management Controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   NumberOfTables   Number of tables
 @param [in]   TableTypes       Byte array to stored the table types
 @param [in]   TableTags        DWORD array of the BIOS Table Tags to be transferred to the management controller

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetBiosTableTags (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8 NumberOfTables,
  IN  UINT8 *TableTypes,
  IN  UINT32 *TableTags
  )
{
  UINT8 *RequestBuf;
  UINTN RequestMsgSize;
  EFI_STATUS Status;
  UINTN Index;

  RequestMsgSize = (UINTN)NumberOfTables * (sizeof(UINT8) + sizeof (UINT32)) + 1;
  Status = gBS->AllocatePool (EfiBootServicesData, RequestMsgSize , (VOID **)&RequestBuf);
  EFI_ERROR_RETURN (Status);

  RequestBuf[0] = NumberOfTables;
  for (Index = 0; Index < NumberOfTables; Index++) {
    RequestBuf[5*Index + 1] = TableTypes[Index];
    *(UINT32 *)(&RequestBuf[5*Index + 2]) = TableTags[Index];
  }
  
  Status = ProcessSimplePldmMessage( This, 
                                     PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION, 
                                     PLDM_SET_BIOS_TAGS,
                                     RequestBuf,
                                     RequestMsgSize,
                                     NULL,
                                     0
                                   );    

  gBS->FreePool (RequestBuf);
  return Status;

}

/**
 Get BIOS Attribute Current Value By Attribute Handle

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   AttrHandle       The Attribute Handle
 @param [out]  AttrData         Attribute Data received from the management controller
 @param [out]  AttrDataSize     Number of bytes for AttrData

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosAttrCurrentValueByHandle (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
  )
{
  PLDM_BIOS_ATTR_TRANSFER_HEADER TransferHeader;

  TransferHeader.AttrHandle = AttrHandle;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                                PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_HANDLE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                AttrData,
                                                AttrDataSize
                                               );
                                        
 
}


/**
 Get BIOS Attribute Current Value By Attribute Handle

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   AttrData         Attribute Data to be transferred to the management controller
 @param [in]   AttrDataSize     Number of bytes for AttrData

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetBiosAttrCurrentValue (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  VOID              *AttrData,
  IN  UINTN             AttrDataSize
  )
{
  PLDM_TRANSFER_HEADER TransferHeader;


  return ProcessPldmMessageWithSplitRequests (This,
                                              PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                              PLDM_SET_BIOS_ATTR_CURRENT_VALUE,
                                              &TransferHeader,
                                              sizeof (TransferHeader),
                                              AttrData,
                                              AttrDataSize
                                             );
}

/**
 Get BIOS Attribute Current Value By Attribute Type

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   AttrType         The Attribute Type
 @param [out]  AttrData         Attribute Data received from the management controller
 @param [out]  AttrDataSize     Number of bytes for AttrData

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosAttrCurrentValueByType (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
  )
{
  PLDM_BIOS_ATTR_TYPE_TRANSFER_HEADER TransferHeader;

  TransferHeader.AttrType = AttrType;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                                PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_TYPE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                AttrData,
                                                AttrDataSize
                                              );
                                        
 
}

/**
 Get BIOS Attribute Pending Value By Attribute Handle

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   AttrHandle       The Attribute Handle
 @param [out]  AttrData         Attribute Data received from the management controller
 @param [out]  AttrDataSize     Number of bytes for AttrData

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosAttrPendingValueByHandle (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
  )
{
  PLDM_BIOS_ATTR_TRANSFER_HEADER TransferHeader;

  TransferHeader.AttrHandle = AttrHandle;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                                PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_HANDLE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                AttrData,
                                                AttrDataSize
                                               );
                                        
 
}


/**
 Get BIOS Attribute Pending Value By Attribute Type

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   AttrType         The Attribute Type
 @param [out]  AttrData         Attribute Data received from the management controller
 @param [out]  AttrDataSize     Number of bytes for AttrData

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetBiosAttrPendingValueByType (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
  )
{
  PLDM_BIOS_ATTR_TYPE_TRANSFER_HEADER TransferHeader;

  TransferHeader.AttrType = AttrType;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                                PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_TYPE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                AttrData,
                                                AttrDataSize
                                              );
                                        
 
}


/**
 Accept BIOS Attribute Pending Values

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   NumOfHandles     Number of BIOS Attribute Handles with Pending Values to be accepted
 @param [in]   AttrHandles      Array of Attribute Handles

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmAcceptBiosAttrPendingValues (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16            NumOfHandles,
  IN  UINT16            *AttrHandles
  )
{
  PLDM_TRANSFER_HEADER TransferHeader;
  UINT16 *HandleData;
  EFI_STATUS Status;
  UINTN AllocateSize;

  AllocateSize = 2 * (NumOfHandles + 1);
  if ((AllocateSize % 4) != 0) {
    AllocateSize += 4 - (AllocateSize % 4);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, AllocateSize, (VOID **)&HandleData);

  HandleData[0] = NumOfHandles;
  gBS->CopyMem (&(HandleData[1]), AttrHandles, 2 * NumOfHandles);

  if ((NumOfHandles % 2) == 0) {
    HandleData[NumOfHandles + 1] = 0;     // Pad Bytes
  }

  Status = ProcessPldmMessageWithSplitRequests (This,
                                              PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION,
                                              PLDM_ACCEPT_BIOS_ATTR_PENDING_VALUES,
                                              &TransferHeader,
                                              sizeof (TransferHeader),
                                              HandleData,
                                              AllocateSize
                                             );

  gBS->FreePool (HandleData);
  return Status;
}


