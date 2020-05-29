/** @file
  Management Component Transport Protocol

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

#include "Mctp.h"



/**
 Set MCTP transport binding protocol

 @param [in]   This             Pointer to EFI_MCTP_PROTOCOL
 @param        MctpBus

 @retval EFI_SUCCESS            Successful
 @return Other values      Errror

**/
EFI_STATUS
EFIAPI
MctpSetMctpBus (
  IN EFI_MCTP_PROTOCOL   *This,
  EFI_MCTP_BUS_PROTOCOL  *MctpBus
)
{
  MCTP_CONTEXT *MctpContext;
  
  if (MctpBus == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  MctpContext = MCTP_CONTEXT_FROM_THIS (This); 
  MctpContext->MctpBus = MctpBus;
  
  return EFI_SUCCESS;
}


VOID
EFIAPI
MctpSetResponseMode(
  IN EFI_MCTP_PROTOCOL *This
)
{
  MCTP_CONTEXT *Context;
  Context = MCTP_CONTEXT_FROM_THIS (This);
  Context->ResponseMode = TRUE;
}

VOID
EFIAPI
MctpSetRequestMode(
  IN EFI_MCTP_PROTOCOL *This
)
{
  MCTP_CONTEXT *Context;
  Context = MCTP_CONTEXT_FROM_THIS (This);
  Context->ResponseMode = FALSE;
}


BOOLEAN
EFIAPI
MctpNewMessageReceived (
  IN EFI_MCTP_PROTOCOL *This
)
{
  MCTP_CONTEXT *MctpContext;
  MctpContext = MCTP_CONTEXT_FROM_THIS (This);

  return MctpContext->MctpBus->NewMessageReceived(MctpContext->MctpBus);

}

/**
 Send MCTP Message to the management controller

 @param [in]   This             Pointer to EFI_MCTP_PROTOCOL
 @param [in]   Message          MCTP Message not including MCTP Header
 @param [in]   MessageSize      Message Size not including MCTP Header

 @retval EFI_SUCCESS            Successful
 @return Other values      Errror

**/
EFI_STATUS
EFIAPI
MctpSendMessage (
  IN EFI_MCTP_PROTOCOL *This,
  IN VOID              *Message,
  IN UINTN             MessageSize
)
{
  UINTN        Index;
  UINTN        RemainingBytes;
  MCTP_HEADER  MctpHeader;
  MCTP_CONTEXT *MctpContext;
  UINTN        PayloadSize;
  UINT8        *Buffer;
  EFI_STATUS   Status;
     
  MctpContext = MCTP_CONTEXT_FROM_THIS (This);

  if (MctpContext->ResponseMode) {
    MctpHeader = MctpContext->ResponseHeader;    
    MctpHeader.TagOwner = 0;
  } else {
    MctpHeader.Reserved = 0;
    MctpHeader.HeaderVersion = MCTP_HEADER_VERSION;
    MctpHeader.DestinationEid = MctpContext->McEndpointId;
    MctpHeader.SourceEid = MctpContext->HostEndpointId;
    MctpHeader.TagOwner = 1;
    MctpContext->MessageTag = (MctpContext->MessageTag + 1) % 8;
    MctpHeader.MessageTag = MctpContext->MessageTag;
  } 

  MctpContext->PacketSequence = 0;  
  Index = 0;
  if (Message == NULL) {
    RemainingBytes = 0;
  } else {
    RemainingBytes = MessageSize;
  }
  
  do {
    if (Index == 0) {
      MctpHeader.StartOfMessage = 1;        
    } else {
      MctpHeader.StartOfMessage = 0;
    }

    if (RemainingBytes > MCTP_BASELINE_TRANSMISSION_UNIT) {
      PayloadSize = MCTP_BASELINE_TRANSMISSION_UNIT;
      MctpHeader.EndOfMessage = 0;
    } else {
      PayloadSize = RemainingBytes ;
      MctpHeader.EndOfMessage = 1;
    }
    
    MctpHeader.PacketSequence = MctpContext->PacketSequence;
    MctpContext->PacketSequence = (MctpContext->PacketSequence + 1) % 4;

    if (Message == NULL) {
      Buffer = NULL;
    } else {
      Buffer = &(((UINT8 *)Message)[Index]);
    }
    Status = MctpContext->MctpBus->SendMctpPacket (MctpContext->MctpBus,
                                                   NULL,
                                                   MctpHeader,
                                                   Buffer,
                                                   PayloadSize
                                                 );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (RemainingBytes <= MCTP_BASELINE_TRANSMISSION_UNIT) {
      break;
    }
    
    RemainingBytes -= MCTP_BASELINE_TRANSMISSION_UNIT;
    Index          += MCTP_BASELINE_TRANSMISSION_UNIT;
  } while (1);
  
  return Status;
  
}

/**
 Receiving MCTP message from management controller

 @param [in]   This
 @param [in, out] Message       The received message
 @param [in, out] MessageSize   Number of bytes received

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpReceiveMessage (
  IN EFI_MCTP_PROTOCOL    *This,
  IN OUT VOID             *Message,
  IN OUT UINTN            *MessageSize
)

{
  MCTP_HEADER  MctpHeader;
  MCTP_CONTEXT *MctpContext;
  UINTN        PayloadSize;
  UINTN        TransferSize;
  UINT8        PacketSequence = 0;
  UINT8        MessageTag = 0;
  EFI_STATUS   Status;
  UINT8        Buffer[MCTP_BASELINE_TRANSMISSION_UNIT];
  
  MctpContext = MCTP_CONTEXT_FROM_THIS (This);
  
  TransferSize = 0;
  do {
    PayloadSize = MCTP_BASELINE_TRANSMISSION_UNIT;
    Status = MctpContext->MctpBus->ReceiveMctpPacket (MctpContext->MctpBus,
                                                              NULL,
                                                              &MctpHeader,
                                                              Buffer,
                                                              &PayloadSize
                                                             );
    if (EFI_ERROR (Status)) {
      break;
    }
    
    if (TransferSize + PayloadSize > *MessageSize) {
      return EFI_BUFFER_TOO_SMALL;
    }
    
    if (TransferSize == 0) {
      //
      // The First Packet
      // 
      if (MctpHeader.StartOfMessage != 1) {
        return EFI_PROTOCOL_ERROR;
      }
      PacketSequence = MctpHeader.PacketSequence;
      MessageTag = MctpHeader.MessageTag;
      if (MctpContext->ResponseMode) {
        MctpContext->ResponseHeader = MctpHeader;
      }
    } else {
      PacketSequence = (PacketSequence + 1) % 4;
      if ( (MctpHeader.PacketSequence != PacketSequence) || 
           (MctpHeader.MessageTag != MessageTag) ) {
        return EFI_PROTOCOL_ERROR;
      } 
    }
    gBS->CopyMem ((UINT8 *)Message + TransferSize, Buffer, PayloadSize);
    TransferSize += PayloadSize;                                                                                                                
  } while (MctpHeader.EndOfMessage == 0);
  *MessageSize = TransferSize;
  
  return Status;
}


/**
 Set Endpoint ID of the management controller

 @param [in]   This             pointer to EFI_MCTP_PROTOCOL
 @param [in]   Address          MCTP physical address
 @param [in]   Operation        00: Set EID,  01: Force EID,  10: Reset EID,  11: Set Discovered Flag
 @param [in]   EndpointId       Endpoint ID to be assigned
 @param [out]  Response

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpSetEndpointId (
  IN  EFI_MCTP_PROTOCOL     *This,
  IN  VOID                  *Address,
  IN  UINT8                 Operation,
  IN  UINT8                 EndpointId,
  OUT MCTP_SET_EID_RESPONSE_DATA *Response
)
{
  EFI_STATUS Status;
  MCTP_HEADER MctpHeader = {
                             MCTP_HEADER_VERSION,   // Header version
                             0,                     // reserved
                             0,                     // Destination Endpoint ID
                             0,                     // Source Endpoint ID
                             0,                     // Message Tag
                             1,                     // Tag Owner
                             0,                     // Packet Sequence
                             1,                     // Start of Message
                             1                      // End of Message                         
                           };
                           
  MCTP_SET_EID_REQUEST_DATA MctpRequest = {
	                          {
	                           MCTP_MESSAGE_TYPE_MCTP_CONTROL,  // Message Type
	                           0,                               // Instance ID
	                           0,                               // Reserved
	                           0,                               // Datagram bit
	                           1,                               // Request bit
	                           MCTP_CONTROL_SET_ENDPOINT_ID     // Command code
	                          },
	                          0,                                // Operation
	                          0                                 // Endpoint ID
	                         };
  MCTP_CONTEXT *MctpContext;
  EFI_MCTP_BUS_PROTOCOL *MctpBus;
  UINTN TransferSize;


  MctpContext = MCTP_CONTEXT_FROM_THIS (This);
  MctpBus = MctpContext->MctpBus;
  MctpContext->TransactionInProgress = TRUE;

  MctpHeader.SourceEid = MctpContext->HostEndpointId;
  MctpContext->MessageTag = (MctpContext->MessageTag + 1) % 8;
  MctpHeader.MessageTag = MctpContext->MessageTag;
  MctpContext->PacketSequence = (MctpContext->PacketSequence + 1) % 4;
  MctpHeader.PacketSequence = MctpContext->PacketSequence;

  MctpRequest.Operation = Operation;
  MctpRequest.EndpointId = EndpointId;
  
  This->SetRequestMode(This);
  Status = MctpBus->SendMctpPacket ( MctpBus,
                                     Address, 
                                     MctpHeader,
                                     &MctpRequest,
                                     sizeof (MctpRequest)
                                   );

  if (EFI_ERROR (Status)) { 
    MctpContext->TransactionInProgress = FALSE;
    return Status;
  }

  TransferSize = sizeof (MCTP_SET_EID_RESPONSE_DATA);
  Status = MctpBus->ReceiveMctpPacket ( MctpBus,
                                        Address, 
                                        &MctpHeader,
                                        Response,
                                        &TransferSize
                                      );

  if (Status == EFI_SUCCESS) {
    MctpContext->McEndpointId = EndpointId;
  }
  MctpContext->TransactionInProgress = FALSE;
  
  return Status;
}

/**
 Get Endpoint ID of the management controller

 @param [in]   This             Pointer to EFI_MCTP_PROTOCOL
 @param [in]   Address          MCTP physical address
 @param [out]  Response         The resonse data

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpGetEndpointId (
  IN  EFI_MCTP_PROTOCOL     *This,
  IN  VOID                  *Address,
  OUT MCTP_GET_EID_RESPONSE_DATA *Response
)
{
  EFI_STATUS Status;
  MCTP_HEADER MctpHeader = {
                             MCTP_HEADER_VERSION,   // Header version
                             0,                     // reserved
                             0,                     // Destination Endpoint ID
                             0,                     // Source Endpoint ID
                             0,                     // Message Tag
                             1,                     // Tag Owner
                             0,                     // Packet Sequence
                             1,                     // Start of Message
                             1                      // End of Message                         
                           };

                           
  MCTP_CONTROL_HEADER MctpRequest = {
                                       MCTP_MESSAGE_TYPE_MCTP_CONTROL,  // Message Type
                                       0,                               // Instance ID
                                       0,                               // Reserved
                                       0,                               // Datagram bit
                                       1,                               // Request bit
                                       MCTP_CONTROL_GET_ENDPOINT_ID     // Command code
                                     };
  MCTP_CONTEXT *MctpContext;
  EFI_MCTP_BUS_PROTOCOL *MctpBus;
  UINTN TransferSize;


  MctpContext = MCTP_CONTEXT_FROM_THIS (This);
  MctpContext->TransactionInProgress = TRUE;  
  MctpBus = MctpContext->MctpBus;

  MctpHeader.SourceEid = MctpContext->HostEndpointId;
  MctpContext->MessageTag = (MctpContext->MessageTag + 1) % 8;
  MctpHeader.MessageTag = MctpContext->MessageTag;
  MctpContext->PacketSequence = (MctpContext->PacketSequence + 1) % 4;
  MctpHeader.PacketSequence = MctpContext->PacketSequence;

  This->SetRequestMode(This);  
  Status = MctpBus->SendMctpPacket ( MctpBus,
                                     Address, 
                                     MctpHeader,
                                     &MctpRequest,
                                     sizeof (MctpRequest)
                                   );

  if (EFI_ERROR (Status)) {
    MctpContext->TransactionInProgress = FALSE;
    return Status;
  }

  TransferSize = sizeof (MCTP_GET_EID_RESPONSE_DATA);
  Status = MctpBus->ReceiveMctpPacket ( MctpBus,
                                        Address, 
                                        &MctpHeader,
                                        Response,
                                        &TransferSize
                                      );

  if (Status == EFI_SUCCESS) {
    MctpContext->McEndpointId = Response->EndpointId;
  }
  MctpContext->TransactionInProgress = FALSE;

  return Status;
}

/**
 Get supported MCTP versions of the management controller

 @param [in]   This             Pointer to EFI_MCTP_PROTOCOL
 @param [in]   MessageType      The specified MCTP Message Type
 @param [out]  Response         MCTP response for MCTP Get Version Support

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpGetVersionSupport (
  IN  EFI_MCTP_PROTOCOL     *This,
  IN  UINT8                 MessageType,
  OUT MCTP_GET_VERSION_RESPONSE_DATA *Response
)
{

  MCTP_GET_VERSION_REQUEST_DATA MctpRequest = 
  {
    {
      MCTP_MESSAGE_TYPE_MCTP_CONTROL,           // Message Type
      0,                                        // Instance ID
      0,                                        // Reserved
      0,                                        // Datagram bit
      1,                                        // Request bit
      MCTP_CONTROL_GET_MCTP_VERSION_SUPPORT     // Command code
    },
    0
  };
  MCTP_CONTEXT *MctpContext;
  UINTN TransferSize;
  EFI_STATUS Status;

  MctpRequest.MessageType = MessageType;
  MctpContext = MCTP_CONTEXT_FROM_THIS (This);
  MctpContext->TransactionInProgress = TRUE;

  This->SetRequestMode(This);
  Status = This->SendMessage(This, &MctpRequest, sizeof (MctpRequest));

  if (EFI_ERROR (Status)) {
    MctpContext->TransactionInProgress = FALSE;
    return Status;
  }

  TransferSize = sizeof(MCTP_GET_VERSION_RESPONSE_DATA);
  Status = This->ReceiveMessage (This, Response, &TransferSize);

  MctpContext->TransactionInProgress = FALSE;

  return Status;
}


/**
 Get supported MCTP Types of the management controller

 @param [in]   This             Pointer to EFI_MCTP_PROTOCOL
 @param [out]  Response         MCTP response for MCTP Get Message Type Support

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpGetMessgeTypeSupport (
  IN  EFI_MCTP_PROTOCOL     *This,
  OUT MCTP_GET_MESSAGE_TYPE_SUPPORT_RESPONSE_DATA *Response
)
{
  MCTP_CONTEXT        *MctpContext;
  MCTP_CONTROL_HEADER MctpRequest = 
  {
    MCTP_MESSAGE_TYPE_MCTP_CONTROL,       // Message Type
    0,                                    // Instance ID
    0,                                    // Reserved
    0,                                    // Datagram bit
    1,                                    // Request bit
    MCTP_CONTROL_GET_MESSAGE_TYPE_SUPPORT // Command code
  };
  UINTN TransferSize;
  EFI_STATUS Status;

  This->SetRequestMode(This);
  MctpContext = MCTP_CONTEXT_FROM_THIS (This);
  MctpContext->TransactionInProgress = TRUE;
  Status = This->SendMessage(This, &MctpRequest, sizeof (MctpRequest));

  if (EFI_ERROR (Status)) {
    return Status;
  }

  TransferSize = sizeof (MCTP_GET_MESSAGE_TYPE_SUPPORT_RESPONSE_DATA);
  Status = This->ReceiveMessage (This, Response, &TransferSize);

  MctpContext->TransactionInProgress = FALSE;

  return Status;
}




/**
 Driver entry point

 @param        MctpContext
 @param        MctpBus

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value              Unknown error

**/
EFI_STATUS
EFIAPI 
SetupMctpProtocol (
  MCTP_CONTEXT           *MctpContext,
  EFI_MCTP_BUS_PROTOCOL  *MctpBus
)
{
  EFI_STATUS                  Status;
  DASH_IO_CFG_PROTOCOL         *DashIoCfg;
  MCTP_SET_EID_RESPONSE_DATA  Response;
  
  Status = gBS->LocateProtocol (&gDashIoCfgProtocolGuid, NULL, (VOID **)&DashIoCfg);
  if(EFI_ERROR(Status)) {
    return Status;
  }
  
  MctpContext->Signature                  = MCTP_SIGNATURE;
  MctpContext->HostEndpointId             = DashIoCfg->GetHostEndpointId();
  MctpContext->McEndpointId               = DashIoCfg->GetMcEndpointId();
  MctpContext->Mctp.SetRequestMode        = MctpSetRequestMode;
  MctpContext->Mctp.SetResponseMode       = MctpSetResponseMode;
  MctpContext->Mctp.NewMessageReceived    = MctpNewMessageReceived;
  MctpContext->Mctp.ReceiveMessage        = MctpReceiveMessage;
  MctpContext->Mctp.SendMessage           = MctpSendMessage;
  MctpContext->Mctp.SetMctpBus            = MctpSetMctpBus;
  MctpContext->Mctp.SetEndpointId         = MctpSetEndpointId;
  MctpContext->Mctp.GetEndpointId         = MctpGetEndpointId;
  MctpContext->Mctp.GetVersionSupport     = MctpGetVersionSupport;
  MctpContext->Mctp.GetMessageTypeSupport = MctpGetMessgeTypeSupport;
  MctpContext->MctpBus                    = MctpBus;
  MctpContext->ResponseMode               = FALSE;
  
  Status = MctpSetEndpointId(&MctpContext->Mctp, NULL, 1, MctpContext->McEndpointId, &Response);
  
  return Status;
}

                                                                                                                      

