/** @file
  Platform Level Data Model implementation

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

#include "Pldm.h"


PLDM_CONTEXT mPldmContext;

VOID
EFIAPI
PushManagementDataToMC(
  IN EFI_EVENT        Event,
  IN VOID             *Context
);


VOID
EFIAPI
SetPldmResponseMode(
    EFI_PLDM_PROTOCOL    *This
)
{
  PLDM_CONTEXT *Context;
  
  Context = PLDM_CONTEXT_FROM_THIS(This);
  Context->Mctp->SetResponseMode(Context->Mctp);
  Context->ResponseMode = TRUE;
}

VOID
EFIAPI
SetPldmRequestMode(
  EFI_PLDM_PROTOCOL    *This
)
{
  PLDM_CONTEXT *Context;

  Context = PLDM_CONTEXT_FROM_THIS(This);
  Context->Mctp->SetRequestMode(Context->Mctp);
  Context->ResponseMode = FALSE;
}


/**
 Set binding MCTP protcol

 @param [in]   This             Point to EFI_PLDM_PROTOCOL
 @param [in]   Mctp             Binding MCTP protocol

 @retval EFI_SUCCESS            Successful
 @retval EFI_INVALID_PARAMETER  Invalid parameter error

**/
EFI_STATUS
EFIAPI
PldmSetMctpBinding (
  IN EFI_PLDM_PROTOCOL   *This,
  IN EFI_MCTP_PROTOCOL   *Mctp
)
{
  PLDM_CONTEXT *PldmContext;
  
  if (Mctp == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  PldmContext = PLDM_CONTEXT_FROM_THIS (This); 
  PldmContext->Mctp = Mctp;
  
  return EFI_SUCCESS;  
/**
 Set Data Transfer Handle and Data Transfer Flag for the transfer header of a PLDM request message

 @param [out]  RequestMessage   The PLDM request message
 @param [in]   TransferHandle   The Transfer Handle
 @param [in]   Flag             The Transfer Flag

 @retval none

**/
}

VOID 
EFIAPI
PldmSetDataTransferHandle (
  OUT VOID   *RequestMessage, 
  IN  UINT32 TransferHandle, 
  IN  UINT8  Flag
)
{
  ((UINT32 *)RequestMessage)[0] = TransferHandle;
  ((UINT8 *)RequestMessage)[4]   = Flag;
}

/**
 Retrieve Data Transfer Handle and Data Transfer Flag from PLDM response data

 @param [in]   ResponseMessage  The PLDM response message
 @param [out]  TransferHandle   The Transfer Handle
 @param [out]  Flag             The Transfer Flag

 @retval none

**/
VOID 
EFIAPI
PldmGetNextDataTransferHandle (
  IN  VOID   *ResponseMessage, 
  OUT UINT32 *TransferHandle, 
  OUT UINT8  *Flag
)
{
  *TransferHandle = ((UINT32 *)(UINT8 *)ResponseMessage)[0];
  if (Flag != NULL) {
    *Flag = ((UINT8 *)ResponseMessage)[4];
  }
}


/**
 Process a complete PLDM request and response transaction with single request and single response

 @param [in]   This
 @param [in]   PldmType         00: PLDM Base,   01: SMBIOS,  02: Monitoring & Control,  03: BIOS Control & Configuration
 @param [in]   Command          The PLDM command according to the PLDM Type
 @param [in]   RequestMessage   PLDM Request Message with PLDM header excluded
 @param [in]   RequestMessageSize  Number of bytes for RequestMessage
 @param [in]   ResponseMessage  PLDM Response Message with PLDM header excluded
 @param [in, out] ResponseMessageSize  Number of bytes for ResponseMessage

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
ProcessSimplePldmMessage (
  IN     EFI_PLDM_PROTOCOL *This,
  IN     UINT8              PldmType,
  IN     UINT8              Command,
  IN     VOID               *RequestMessage,
  IN     UINTN              RequestMessageSize,
  IN     VOID               *ResponseMessage,
  IN OUT UINTN              *ResponseMessageSize
  )
{
  EFI_STATUS Status;
  PLDM_HEADER Header = PLDM_HEADER_COMMON;
  PLDM_CONTEXT *PldmContext;

  PldmContext = PLDM_CONTEXT_FROM_THIS (This);

  This->SetRequestMode(This);
  PldmContext->TransactionInProgress = TRUE;

  Header.PldmType = PldmType;
  Header.PldmCmdCode = Command;
  
  PldmContext->InstanceId = (PldmContext->InstanceId + 1) % 32;
  Header.InstanceId = PldmContext->InstanceId;

  Status = This->SendMessage(This, Header, RequestMessage,RequestMessageSize);

  if (Status == EFI_SUCCESS) {
    Status = This->ReceiveMessage (This, &Header, ResponseMessage, ResponseMessageSize);
  }
  PldmContext->TransactionInProgress = FALSE;;

  
  return Status;


}

/**
 Process a complete PLDM request and response transaction with multiple requests for large request data transfer

 @param [in]   This
 @param [in]   PldmType         00: PLDM Base,   01: SMBIOS,  02: Monitoring & Control,  03: BIOS Control & Configuration
 @param [in]   Command          The PLDM command according to the PLDM Type
 @param [in]   TransferHeader   The Request Transfer Header according to the PLDM command
 @param [in]   TransferHeaderSize  Number of bytes for TransferHeader
 @param [in]   RequestMessage   PLDM Request Message with PLDM header and Transfer Header excluded
 @param [in]   RequestMessageSize  Number of bytes for RequestMessage

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
ProcessPldmMessageWithSplitRequests (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8              PldmType,
  IN UINT8              Command,
  IN VOID               *TransferHeader,
  IN UINTN              TransferHeaderSize,
  IN VOID               *RequestMessage,
  IN UINTN              RequestMessageSize
  )
{

  UINT8        Payload[MAX_PLDM_PAYLOAD_SIZE];
  UINT8        TransferFlag;
  UINT32       TransferHandle;
  UINTN        TransferIndex;
  UINTN        PayloadSize;
  UINTN        MsgSize;
  UINTN        RemainingBytes;
  PLDM_HEADER  RequestHeader = PLDM_HEADER_COMMON;
  PLDM_HEADER  ResponseHeader;
  PLDM_CONTEXT *PldmContext;
  EFI_STATUS   Status;
  UINT32       Crc32;

  PldmContext = PLDM_CONTEXT_FROM_THIS(This); 
  This->SetRequestMode(This);
  PldmContext->TransactionInProgress = TRUE;

  RequestHeader.PldmType = PldmType;
  RequestHeader.PldmCmdCode = Command;
  PldmContext->InstanceId = (PldmContext->InstanceId + 1) % 32;
  RequestHeader.InstanceId = PldmContext->InstanceId;

  Status = EFI_SUCCESS;
  TransferHandle = 0;
  TransferIndex = 0;
  Crc32 = 0;
  gBS->CalculateCrc32(RequestMessage, RequestMessageSize, &Crc32);
  
  do {
    //
    // Add additinal 4 CRC32 bytes
    //
    RemainingBytes = RequestMessageSize - TransferIndex + 4;
    
    if (RemainingBytes + TransferHeaderSize > PldmContext->MaxPayloadSize) {
      if (RemainingBytes + TransferHeaderSize - 4 >= PldmContext->MaxPayloadSize) {
        PayloadSize = PldmContext->MaxPayloadSize;
      } else {
        //
        // CRC bytes will be sent in the next transfer loop
        //
        PayloadSize = RemainingBytes + TransferHeaderSize - 4;
      }
      MsgSize = PayloadSize - TransferHeaderSize;
      if (TransferIndex == 0) {
        TransferFlag = PldmTransferStart;
      } else {
        TransferFlag = PldmTransferMiddle;
      }
    } else {
      PayloadSize = RemainingBytes + TransferHeaderSize;
      MsgSize = RemainingBytes - 4;
      if (TransferIndex == 0) {
        TransferFlag = PldmTransferStartAndEnd;
      } else {
        TransferFlag = PldmTransferEnd;
      }
    }

    PldmSetDataTransferHandle (TransferHeader, TransferHandle, TransferFlag);
    gBS->CopyMem (Payload, TransferHeader, TransferHeaderSize);        
    gBS->CopyMem (Payload + TransferHeaderSize, (UINT8 *)RequestMessage + TransferIndex, MsgSize);
    if ((TransferFlag & PldmTransferEnd) == PldmTransferEnd) {
      gBS->CopyMem (Payload + PayloadSize - 4, &Crc32, 4);
    }
    
  
    Status = This->SendMessage (This, RequestHeader, Payload, PayloadSize );
    if (EFI_ERROR (Status)) {
      break;
    }
    TransferIndex += PayloadSize - TransferHeaderSize;



    PayloadSize = PldmContext->MaxPayloadSize;
    Status = This->ReceiveMessage (This, &ResponseHeader, Payload, &PayloadSize);
    if (EFI_ERROR (Status)) {
      break;
    }

    PldmGetNextDataTransferHandle(Payload, &TransferHandle, NULL);
      
  }while ((TransferFlag & PldmTransferEnd)!= PldmTransferEnd);  
  PldmContext->TransactionInProgress = FALSE;;

  return Status;
}

/**
 Process a complete PLDM request and response transaction with multiple responses for large response data transfer

 @param [in]   This
 @param [in]   PldmType         00: PLDM Base,   01: SMBIOS,  02: Monitoring & Control,  03: BIOS Control & Configuration
 @param [in]   Command          The PLDM command according to the PLDM Type
 @param [in]   TransferHeader   The Request Transfer Header according to the PLDM command
 @param [in]   TransferHeaderSize  Number of bytes for TransferHeader
 @param [out]  ResponseMessage  PLDM Response Message with PLDM header and Transfer Header excluded
 @param [in, out] ResponseMessageSize  Number of bytes for ResponseMessage

 @retval EFI_SUCCESS            Successful
 @return Other values            Unsuccessful

**/
EFI_STATUS
EFIAPI
ProcessPldmMessageWithSplitResponses (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8              PldmType,
  IN UINT8              Command,
  IN VOID               *TransferHeader,
  IN UINTN              TransferHeaderSize,
  OUT VOID              *ResponseMessage,
  IN OUT UINTN          *ResponseMessageSize
)
{

  UINT8        Payload[MAX_PLDM_PAYLOAD_SIZE];
  UINT8        TransferOperationFlag; 
  UINT8        TransferFlag;
  UINT32       TransferHandle;
  UINTN        TransferIndex;
  UINTN        PayloadSize;
  UINTN        MsgSize;
  PLDM_HEADER  RequestHeader = PLDM_HEADER_COMMON;
  PLDM_HEADER  ResponseHeader;
  PLDM_CONTEXT *PldmContext;
  EFI_STATUS   Status;
  UINT32       Crc32;


  PldmContext = PLDM_CONTEXT_FROM_THIS(This); 
  This->SetRequestMode(This);
  PldmContext->TransactionInProgress = TRUE;

  RequestHeader.PldmType = PldmType;
  RequestHeader.PldmCmdCode = Command;
  PldmContext->InstanceId = (PldmContext->InstanceId + 1) % 32;
  RequestHeader.InstanceId = PldmContext->InstanceId;

  TransferHandle = 0;
  TransferOperationFlag = PldmGetFirstPart;
  TransferIndex = 0;
  Status = EFI_SUCCESS;
  do {
    PldmSetDataTransferHandle (TransferHeader, TransferHandle, TransferOperationFlag);
    gBS->CopyMem (Payload , TransferHeader, TransferHeaderSize);
    PayloadSize = TransferHeaderSize;
    Status = This->SendMessage (This, RequestHeader, Payload, PayloadSize);
    if (EFI_ERROR(Status)) {
      break;
    }

    
    PayloadSize = PldmContext->MaxPayloadSize;
    Status = This->ReceiveMessage (This, &ResponseHeader, Payload, &PayloadSize);
    if (EFI_ERROR(Status)) {
      break;
    }
    
    PldmGetNextDataTransferHandle(Payload, &TransferHandle, &TransferFlag);
    if ((TransferFlag & 0xF8) !=0x00) {
      Status = EFI_PROTOCOL_ERROR;
      break;
    }

    if ((TransferFlag & PldmTransferEnd) == PldmTransferEnd) {
      MsgSize = PayloadSize - (RESPONSE_TRANSFER_HEADER_SIZE + 4);
    } else {
      MsgSize = PayloadSize - RESPONSE_TRANSFER_HEADER_SIZE;
    }

    gBS->CopyMem ((UINT8 *)ResponseMessage + TransferIndex, Payload + RESPONSE_TRANSFER_HEADER_SIZE, MsgSize);
    
    TransferIndex += MsgSize;
    TransferOperationFlag = PldmGetNextPart;
    
  }while ((TransferFlag & PldmTransferEnd) != PldmTransferEnd);

  if (Status == EFI_SUCCESS) {
  
    *ResponseMessageSize = TransferIndex;

    Crc32 = 0;
    gBS->CalculateCrc32 (ResponseMessage, *ResponseMessageSize, &Crc32);

    if ( *(UINT32 *)(Payload + PayloadSize - 4) != Crc32) {
      Status = EFI_CRC_ERROR;
    }
  }
  PldmContext->TransactionInProgress = FALSE;

  return Status;
  
  
}


/**
 Send a single request message to the management controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   Header           The PLDM header
 @param [in]   Message          The Request Message without PLDM header
 @param [in]   MessageSize

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSendMessage (
  IN EFI_PLDM_PROTOCOL *This,
  IN PLDM_HEADER       Header,
  IN VOID              *Message,
  IN UINTN             MessageSize
)
{
  PLDM_CONTEXT *PldmContext;
  EFI_STATUS   Status;
  UINT8        *MctpMsg;

  
  PldmContext = PLDM_CONTEXT_FROM_THIS (This);
  MctpMsg = PldmContext->Buffer;

  if (PldmContext->ResponseMode) {
    Header.RequestBit = 0;
  }
  gBS->CopyMem (MctpMsg, &Header, sizeof (PLDM_HEADER));
  if ((MessageSize != 0) && (Message != NULL)) {
    gBS->CopyMem (MctpMsg + sizeof (PLDM_HEADER), Message, MessageSize);
  }
                                                
  Status = PldmContext->Mctp->SendMessage (PldmContext->Mctp,
                                           MctpMsg,
                                           MessageSize + sizeof (PLDM_HEADER)
                                          );  

  return Status;
}

/**
 Receive a single response message from the management controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [out]  Header           The PLDM header
 @param [out]  Message          The Response Message without PLDM header
 @param [in, out] MessageSize

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmReceiveMessage (
  IN  EFI_PLDM_PROTOCOL *This,
  OUT PLDM_HEADER       *Header,
  OUT VOID              *Message,
  IN OUT UINTN          *MessageSize
)
{
  PLDM_CONTEXT *PldmContext;
  EFI_STATUS   Status;
  UINT8        *MctpMsg;
  UINTN        MctpMsgSize;
  UINTN        MsgOffset;
  
  PldmContext = PLDM_CONTEXT_FROM_THIS (This);
  MctpMsg = PldmContext->Buffer;
  MctpMsgSize = *MessageSize + sizeof (PLDM_HEADER) + 1;
  Status = PldmContext->Mctp->ReceiveMessage (PldmContext->Mctp,
                                              MctpMsg,
                                              &MctpMsgSize
                                             );  

  if (Status == EFI_SUCCESS) {
    if ((!PldmContext->ResponseMode) && (MctpMsg[sizeof(PLDM_HEADER)] != PLDM_SUCCESS)) {
      return EFI_PLDM_COMPLETION_ERROR;
    }
    if (PldmContext->ResponseMode) {
      MsgOffset = sizeof (PLDM_HEADER);
    } else {
      MsgOffset = sizeof (PLDM_HEADER) + 1;
    }    
    
    gBS->CopyMem (Header, MctpMsg, sizeof (PLDM_HEADER));
    *MessageSize = MctpMsgSize - MsgOffset;
    gBS->CopyMem (Message, MctpMsg + MsgOffset, *MessageSize);    
  }

    
  return Status;
}



/**
 Get PLDM Target ID

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [out]  Tid              The Target ID

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value      Unknown error

**/
EFI_STATUS
EFIAPI
PldmGetTid (
  IN EFI_PLDM_PROTOCOL *This,
  OUT UINT8            *Tid
)
{
  EFI_STATUS Status;
  UINTN      Size;

  Size = 1;
  Status = ProcessSimplePldmMessage(This,
                                    PLDM_TYPE_MESSAGE_CONTROL_AND_DISCOVERY,
                                    PLDM_BASE_GET_TID,
                                    NULL,
                                    0,
                                    Tid,
                                    &Size
                                   );
  return Status;
}

/**
 Get PLDM supported versions

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [in]   PldmType
 @param [out]  VersionInfo      The supported version information
 @param [out]  VersionInfoSize  Number of bytes for VersionInfo

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value      Unknown error

**/
EFI_STATUS
EFIAPI
GetPldmVersion (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             PldmType,
  OUT UINT32            *VersionInfo,
  OUT UINTN             *VersionInfoSize
)
{
  EFI_STATUS                    Status;
  PLDM_GET_VERSION_REQUEST_DATA PldmRequest;

  PldmRequest.DataTransferHandle = 0;
  PldmRequest.TransferOperationFlag = PldmGetFirstPart;
  PldmRequest.PldmType = PldmType;
  Status = ProcessPldmMessageWithSplitResponses(This,
                                                PLDM_TYPE_MESSAGE_CONTROL_AND_DISCOVERY,
                                                PLDM_BASE_GET_PLDM_VERSION,
                                                &PldmRequest,
                                                sizeof (PldmRequest),
                                                VersionInfo,
                                                VersionInfoSize
                                               );
                                               
  return Status;
}

/**
 Get supported PLDM Types

 @param        This             Pointer to EFI_PLDM_PROTOCOL
 @param        SupportedTypes   Bit mask for the supported types,
                                if Bit No. N of the Mth Byte is set, then command M*8+N is supported

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value      Unknown error

**/
EFI_STATUS
EFIAPI
GetPldmSupportedTypes (
  EFI_PLDM_PROTOCOL            *This,
  UINT8                        *SupportedTypes              
)
{
  EFI_STATUS Status;
  UINTN ResponseSize;

  ResponseSize = 8; //sizeof (PLDM_SUPPORTED_TYPES);
  Status = ProcessSimplePldmMessage(This,
                                    PLDM_TYPE_MESSAGE_CONTROL_AND_DISCOVERY,
                                    PLDM_BASE_GET_PLDM_TYPES,
                                    NULL,
                                    0,
                                    SupportedTypes,
                                    &ResponseSize
                                   );
  return Status;
}

/**
 Get supported PLDM commands for a specified PLDM type

 @param        This             Pointer to EFI_PLDM_PROTOCOL
 @param        PldmType         00: PLDM Base,    01: SMBIOS,   02: Monitoring & Control,   03: BIOS Control & Configuration
 @param        PldmTypeVersion  Version for the PLDM type, it should be 0xF1F0F000
 @param        SupportedCommands

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value              Unknown error

**/
EFI_STATUS
EFIAPI
GetPldmSupportedCommands (
  EFI_PLDM_PROTOCOL *This,
  UINT8             PldmType,
  UINT32            PldmTypeVersion,
  UINT8             *SupportedCommands
)
{
  EFI_STATUS Status;
  UINTN ResponseSize;
  PLDM_GET_COMMANDS_REQUEST_DATA PldmRequest;

  

  PldmRequest.PldmType = PldmType;
  PldmRequest.Version  = PldmTypeVersion;
  ResponseSize         = 32; //sizeof (PLDM_SUPPORTED_COMMANDS);
  Status = ProcessSimplePldmMessage(This,
                                    PLDM_TYPE_MESSAGE_CONTROL_AND_DISCOVERY,
                                    PLDM_BASE_GET_PLDM_COMMANDS,
                                    &PldmRequest,
                                    sizeof (PLDM_GET_COMMANDS_REQUEST_DATA),
                                    SupportedCommands,
                                    &ResponseSize
                                   );
  return Status;
}


/**
 EFI_PLDM_PROTOCOL initialization

 @param        PldmContext      pointer to PLDM_CONTEXT structure
 @param        Mctp             pointer to EFI_MCTP_PROTOCOL structure

 @retval SUCCESS                command complete successfully
 @return other values             command failed

**/
EFI_STATUS
EFIAPI 
EFIAPI
SetupPldmProtocol (
  PLDM_CONTEXT      *PldmContext,
  EFI_MCTP_PROTOCOL *Mctp
)
{
  EFI_STATUS   Status;
  EFI_EVENT    ReadyToBootEvent;

  PldmContext->Signature                            = PLDM_SIGNATURE;
  PldmContext->Mctp                                 = Mctp;
  PldmContext->InstanceId                           = 0;
  PldmContext->MaxPayloadSize                       = MAX_PLDM_PAYLOAD_SIZE;
  PldmContext->Pldm.SetRequestMode                       = SetPldmRequestMode;
  PldmContext->Pldm.SetResponseMode                      = SetPldmResponseMode;
  PldmContext->Pldm.SendMessage                     = PldmSendMessage;
  PldmContext->Pldm.ReceiveMessage                  = PldmReceiveMessage;
  PldmContext->Pldm.GetTid                          = PldmGetTid;
  PldmContext->Pldm.GetVersion                      = GetPldmVersion;
  PldmContext->Pldm.GetSupportedTypes               = GetPldmSupportedTypes;
  PldmContext->Pldm.GetSupportedCommands            = GetPldmSupportedCommands;
  PldmContext->Pldm.GetSmbiosStructureTableMetaData = PldmGetSmbiosStructureTableMetaData;
  PldmContext->Pldm.SetSmbiosStructureTableMetaData = PldmSetSmbiosStructureTableMetaData;
  PldmContext->Pldm.GetSmbiosStructureTable         = PldmGetSmbiosStructureTable;
  PldmContext->Pldm.SetSmbiosStructureTable         = PldmSetSmbiosStructureTable;
  PldmContext->Pldm.SetDateTime                     = PldmBiosSetDateTime;
  PldmContext->Pldm.GetBiosTable                    = PldmGetBiosTable;
  PldmContext->Pldm.SetBiosTable                    = PldmSetBiosTable;
  PldmContext->Pldm.GetBiosTableTags                = PldmGetBiosTableTags;
  PldmContext->Pldm.SetBiosTableTags                = PldmSetBiosTableTags;
  PldmContext->Pldm.GetBiosAttrCurrentValueByHandle = PldmGetBiosAttrCurrentValueByHandle;
  PldmContext->Pldm.GetBiosAttrCurrentValueByType   = PldmGetBiosAttrCurrentValueByType;
  PldmContext->Pldm.SetBiosAttrCurrentValue         = PldmSetBiosAttrCurrentValue;
  PldmContext->Pldm.GetBiosAttrPendingValueByHandle = PldmGetBiosAttrPendingValueByHandle;
  PldmContext->Pldm.GetBiosAttrPendingValueByType   = PldmGetBiosAttrPendingValueByType;
  PldmContext->Pldm.AcceptBiosAttrPendingValues     = PldmAcceptBiosAttrPendingValues;
  PldmContext->Pldm.PlatformEventMessage            = PldmPlatformEventMessage;
  PldmContext->Pldm.AppendBiosString                = AppendBiosString;
  PldmContext->Pldm.AppendBiosAttr                  = AppendBiosAttr;
  PldmContext->Pldm.AppendBiosAttrValue             = AppendBiosAttrValue;
  PldmContext->Pldm.ResetBiosTables                 = ResetBiosTables;
  PldmContext->Pldm.PushBiosTables                  = PushBiosTables;
  PldmContext->Pldm.RegisterSensorReadingFunction   = RegisterSensorReadingFunction;
  PldmContext->SensorReadingEvent                   = NULL;
  PldmContext->SensorReadingCheckEvent              = NULL;

  PldmContext->Pldm.SetRequestMode(&PldmContext->Pldm);
  
  Status = gBS->AllocatePool (EfiBootServicesData, PLDM_BUFFER_SIZE, (VOID **)&PldmContext->Buffer);
  EFI_ERROR_RETURN (Status);

  Status = InitBiosTables (PldmContext);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PldmContext->Buffer);
    return Status;
  }


  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK - 1,
             PushManagementDataToMC,
             PldmContext,
             &ReadyToBootEvent
             );
    
  return Status;

}



