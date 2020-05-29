/** @file

  Definitions for HECI driver

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

#ifndef _SPS_HECI_CORE_DXE_H_
#define _SPS_HECI_CORE_DXE_H_

#include <SpsMe.h>

///
/// Prototypes
///
/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] None.

  @retval EFI_SUCCESS             HECI device is present and initialized.
  @retval EFI_DEVICE_ERROR        No HECI controller.
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported because ME MODE is in ME ALT Disabled &
                                  SECOVR JMPR
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
InitializeHeciPrivate (
  VOID
  );

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] None.

  @retval EFI_SUCCESS             ME is ready
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
WaitForMEReady (
  VOID
  );

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] None.

  @retval EFI_SUCCESS             HECI device is present and initialized
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  VOID
  );

/**
  Heci Re-initializes it for Host

  @param[in] None.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_STATUS              Status code returned by ResetHeciInterface
**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  VOID
  );

/**
  Heci Re-initializes it for Me

  @param[in] None.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Re-initialization done
**/
EFI_STATUS
EFIAPI
HeciReInitialize2 (
  VOID
  );

/**
  Function to pull one messsage packet off the HECI circular buffer.
  Corresponds to HECI HPS (part of) section 4.2.4

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageHeader       Pointer to a buffer for the message header.
  @param[in] MessageData          Pointer to a buffer to recieve the message in.
  @param[in, out] Length          On input is the size of the callers buffer in bytes. On
                                  output this is the size of the packet in bytes.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer
**/
EFI_STATUS
HECIPacketRead (
  IN     UINT32                          Blocking,
  OUT    SPS_HECI_MESSAGE_HEADER         *MessageHeader,
  OUT    UINT32                          *MessageData,
  IN OUT UINT32                          *Length
  );

/**
  Reads a message from the ME across HECI.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN     UINT32                        Blocking,
  IN OUT UINT32                        *MessageBody,
  IN OUT UINT32                        *Length
  );

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
EFIAPI
HeciSend (
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**
  Function sends one messsage packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
EFI_STATUS
HeciPacketWrite (
  IN SPS_HECI_MESSAGE_HEADER          *MessageHeader,
  IN UINT32                            *MessageData
  );

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in, out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in, out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the bufferbefore timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
EFIAPI
HeciSendwACK (
  IN OUT UINT32                        *Message,
  IN     UINT32                        Length,
  IN OUT UINT32                        *RecLength,
  IN     UINT8                         HostAddress,
  IN     UINT8                         MeAddress
  );

/**
  Me reset and waiting for ready

  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
MeResetWait (
  IN UINT32                            Delay
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS 0.90 doc 4-17-06 njy

  @param[in] none

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  VOID
  );

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval                         Number of filled slots.
**/
UINT8
FilledSlots (
  IN UINT32                            ReadPointer,
  IN UINT32                            WritePointer
  );

/**
  Calculate if the circular buffer has overflowed
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Value read from host/me read pointer
  @param[in] WritePointer         Value read from host/me write pointer
  @param[in] BufferDepth          Value read from buffer depth register

  @retval EFI_DEVICE_ERROR        The circular buffer has overflowed
  @retval EFI_SUCCESS             The circular buffer does not overflowed
**/
EFI_STATUS
OverflowCB (
  IN UINT32                            ReadPointer,
  IN UINT32                            WritePointer,
  IN UINT32                            BufferDepth
  );

/**
  Get an abstract Intel ME State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions

  @param[out] MeStatus            Pointer for status report

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  OUT UINT32                      *MeStatus
  );

/**
  Return ME Mode

  @param[out] MeMode              Pointer for ME Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                       *MeMode
  );

#endif
