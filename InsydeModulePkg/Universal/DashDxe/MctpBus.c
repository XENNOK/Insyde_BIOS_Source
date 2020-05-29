/** @file
  Management Component Transport Binding Protocol Implementation

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

#include "MctpBus.h"


BOOLEAN
EFIAPI
MctpBusNewMessageRecieved (
  EFI_MCTP_BUS_PROTOCOL *This
)
{
  MCTP_BUS_CONTEXT *Context;

  
  Context = MCTP_BUS_CONTEXT_FROM_THIS (This);

  return Context->MctpSmbus->BufferFull(Context->MctpSmbus);
}



/**
 Send MCTP packet to the management controller

 @param [in]   This             Pointer to EFI_MCTP_BUS_PROTOCOL
 @param [in]   PhysicalAddress
 @param [in]   MctpHeader       The MCTP header
 @param [in]   Payload          The MCTP data (with MCTP header excluded) to be transfered
 @param [in]   PayloadSize      Number of bytes of Payload data

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpBusSendMctpPacket (
  IN  EFI_MCTP_BUS_PROTOCOL *This,
  IN  VOID                  *PhysicalAddress,
  IN  MCTP_HEADER           MctpHeader,
  IN  VOID                  *Payload,
  IN  UINTN                 PayloadSize
)
{
  UINT8 Packet[MAX_MCTP_PACKET_SIZE];
  MCTP_BUS_CONTEXT *Context;
  EFI_STATUS Status;
  UINTN     Len;

  Context = MCTP_BUS_CONTEXT_FROM_THIS (This);
  
  if (PhysicalAddress != NULL &&  *((UINT8 *)PhysicalAddress) != Context->McAddr)
  {
    return EFI_PROTOCOL_ERROR;
  } 
  
  if (PayloadSize > MCTP_BASELINE_TRANSMISSION_UNIT) {
    return EFI_PROTOCOL_ERROR;
  }

  Len = PayloadSize + 5;
  Packet[0] = Context->HostSlaveAddr | 0x01;
  
  gBS->CopyMem (Packet + 1, &MctpHeader, sizeof (MCTP_HEADER));
  gBS->CopyMem (Packet + sizeof (MCTP_HEADER) + 1, Payload, PayloadSize);

  Status = Context->MctpSmbus->MasterSend (Context->MctpSmbus, Context->McAddr, Packet, Len);
                                           

  return Status;
  
}


/**
 Receive MCTP packet from the response data of the management controller

 @param [in]   This             Pointer to EFI_MCTP_BUS_PROTOCOL
 @param [in]   PhysicalAddress
 @param [out]  MctpHeader       The MCTP header
 @param [out]  Payload          The MCTP data (with MCTP header excluded) to be received
 @param [out]  PayloadSize      Number of bytes recieived for Payload data

 @retval EFI_SUCCESS            successful
 @return others        Error

**/
EFI_STATUS
EFIAPI
MctpBusReceiveMctpPacket (
  IN  EFI_MCTP_BUS_PROTOCOL *This,
  IN  VOID                  *PhysicalAddress,
  OUT MCTP_HEADER           *MctpHeader,  
  OUT VOID                  *Payload,
  OUT UINTN                 *PayloadSize
)
{
  EFI_STATUS Status;
  MCTP_BUS_CONTEXT *Context;  
  UINT8 Packet[MAX_MCTP_PACKET_SIZE];
  UINTN Len;

  
  Context = MCTP_BUS_CONTEXT_FROM_THIS (This);

  if (PhysicalAddress != NULL &&  *((UINT8 *)PhysicalAddress) != Context->McAddr) {
    return EFI_PROTOCOL_ERROR;
  }

  Len = MAX_MCTP_PACKET_SIZE;
  Status = Context->MctpSmbus->SlaveReceive (Context->MctpSmbus, Packet, &Len);

  if (!EFI_ERROR(Status)) {
    gBS->CopyMem (MctpHeader, Packet + 1, sizeof (MCTP_HEADER));
    *PayloadSize =  Len - (sizeof(MCTP_HEADER) + 1);
    gBS->CopyMem (Payload, Packet +5, *PayloadSize);
  }
  return Status;
}




/**
 EFI_MCTP_BUS_PROTOCOL Initialization

 @param        MctpBusContext   pointer to MCTP_BUS_CONTEXT
 @param        MctpSmbus        pointer to MCTP_SMBUS_PROTOCOL

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value              Unknown error

**/
EFI_STATUS
EFIAPI 
SetupMctpBusProtocol (
  MCTP_BUS_CONTEXT    *MctpBusContext,
  MCTP_SMBUS_PROTOCOL *MctpSmbus
  )
{
  EFI_STATUS                  Status;
  DASH_IO_CFG_PROTOCOL         *DashIoCfg;
  
  Status = gBS->LocateProtocol (&gDashIoCfgProtocolGuid, NULL, (VOID **)&DashIoCfg);
  if(EFI_ERROR(Status)) {
    return Status;
  }



  MctpBusContext->Signature                 = MCTP_BUS_SIGNATURE;
  MctpBusContext->McAddr                    = DashIoCfg->GetMcSmbusAddr();
  MctpBusContext->HostSlaveAddr             = DashIoCfg->GetHostSmbusSlaveAddr();
  MctpBusContext->MctpBus.NewMessageReceived= MctpBusNewMessageRecieved;
  MctpBusContext->MctpBus.ReceiveMctpPacket = MctpBusReceiveMctpPacket;
  MctpBusContext->MctpBus.SendMctpPacket    = MctpBusSendMctpPacket;
  MctpBusContext->MctpSmbus                 = MctpSmbus;
  
  MctpBusContext->MctpSmbus->SetListenAddress (MctpBusContext->MctpSmbus, MctpBusContext->HostSlaveAddr);


  return EFI_SUCCESS;
}
