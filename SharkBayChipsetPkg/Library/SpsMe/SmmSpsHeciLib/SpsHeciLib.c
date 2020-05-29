/** @file

  Implementation file for SPS HECI functionality in SMM Phase.
  Note: Only for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <SpsMe.h>
#include <Protocol/SpsHeci.h>

SPS_HECI_PROTOCOL            *mHeciSmm = NULL;

/**
  Retrieves the Smm HECI protocol from the SMST handle database.
**/
EFI_STATUS
SpsInternalGetSmmHeciProtocol (
  VOID
  )
{
  EFI_STATUS            Status;

  if (mHeciSmm != NULL) {
    return EFI_SUCCESS;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gSpsSmmHeciProtocolGuid,
                    NULL,
                    (VOID **)&mHeciSmm
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SMM SPS HECI LIB] ERROR: Locate SPS HECI Protocol => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in, out] Message         Pointer to the message buffer.
  @param[in]      SendLength      Length of the message in bytes.
  @param[in, out] RecLength       Length of the message response in bytes.
  @param[in]      HostAddress     Address of the sending entity.
  @param[in]      MeAddress       Address of the ME entity that should receive the message.

  @exception EFI_SUCCESS          Command succeeded
  @exception EFI_DEVICE_ERROR     HECI Device error, command aborts abnormally
  @exception EFI_TIMEOUT          HECI does not return the buffer before timeout
  @exception EFI_BUFFER_TOO_SMALL Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
SpsHeciSendwAck (
  IN OUT UINT32                        *Message,
  IN OUT UINT32                        Length,
  IN OUT UINT32                        *RecLength,
  IN     UINT8                         HostAddress,
  IN     UINT8                         MEAddress
  )
{
  EFI_STATUS                 Status;

  if ((Message == NULL) || (RecLength == NULL)) {
    return EFI_INVALID_LANGUAGE;
  }

  Status = SpsInternalGetSmmHeciProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  Status = mHeciSmm->SendwACK (
                       Message,
                       Length,
                       RecLength,
                       HostAddress,
                       MEAddress
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/**
  Read the HECI Message from Intel ME with size in Length into
  buffer MessageBody. Set Blocking to BLOCKING and code will
  wait until one message packet is received. When set to
  NON_BLOCKING, if the circular buffer is empty at the time, the
  code will not wait for the message packet.

  @param[in]      Blocking        Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
SpsHeciReadMsg (
  IN     UINT32                        Blocking,
  IN OUT UINT32                        *MessageBody,
  IN OUT UINT32                        *Length
  )
{
  EFI_STATUS                 Status;

  if ((MessageBody == NULL) || (Length == NULL) || (Blocking > BLOCKING)) {
    return EFI_INVALID_LANGUAGE;
  }

  Status = SpsInternalGetSmmHeciProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  Status = mHeciSmm->ReadMsg (
                       Blocking,
                       (UINT32 *) MessageBody,
                       Length
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
SpsHeciSendMsg (
  IN UINT32                            *Message,
  IN UINT32                            Length,
  IN UINT8                             HostAddress,
  IN UINT8                             MeAddress
  )
{
  EFI_STATUS                 Status;

  if ((Message == NULL)) {
    return EFI_INVALID_LANGUAGE;
  }

  Status = SpsInternalGetSmmHeciProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  Status = mHeciSmm->SendMsg (
                       (UINT32 *) Message,
                       Length,
                       HostAddress,
                       MeAddress
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Determines if the HECI device is present and, if present, initializes it for use by the BIOS.

  @param None.

  @retval EFI_SUCCESS             HECI device is present and initialized
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported
**/
EFI_STATUS
SpsHeciInitialize (
  VOID
  )
{
  EFI_STATUS                 Status;

  Status = SpsInternalGetSmmHeciProtocol ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mHeciSmm->InitHeci ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SMM SPS HECI LIB] ERROR: HECI initialize => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

