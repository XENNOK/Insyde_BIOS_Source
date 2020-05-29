/** @file

  Framework PEIM to provide Heci.

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

#ifndef _SPS_HECI_PEI_H_
#define _SPS_HECI_PEI_H_

#include <IndustryStandard/Pci.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <SpsMe.h>

#include <SpsMeAccess.h>

#include <Ppi/SpsHeci.h>

///
/// Prototypes
///
/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI
  @param[in, out] HeciMemBar      HECI Memory BAR

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
InitializeHeci (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *HeciMemBar
  );

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_TIMEOUT             If 5 second timeout has expired, return fail.
**/
EFI_STATUS
WaitForMEReady (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      HeciMemBar
  );


/**
  Read the HECI Message from Intel ME with size in Length into
  buffer Message. Set Blocking to BLOCKING and code will wait
  until one message packet is received. When set to
  NON_BLOCKING, if the circular buffer is empty at the time, the
  code not wait for the message packet read.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in] MessageBody          Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             One message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_DEVICE_ERROR        Zero-length message packet read
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
EFIAPI
HeciReadMsg (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      Blocking,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      *MessageBody,
  IN OUT   UINT32                      *Length
  );

/**
  Function to pull one messsage packet off the HECI circular buffer.
  Corresponds to HECI HPS (part of) section 4.2.4

  @param[in]      PeiServices     General purpose services available to every PEIM.
  @param[in]      Blocking        Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in]      HeciMemBar      HECI Memory BAR.
  @param[out]     MessageHeader   Pointer to a buffer for the message header.
  @param[out]     MessageData     Pointer to a buffer to recieve the message in.
  @param[in, out] Length          On input is the size of the callers buffer in bytes. On
                                  output this is the size of the packet in bytes.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer

**/
UINT32
HeciPacketRead (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      Blocking,
  IN       UINT32                      HeciMemBar,
  OUT      SPS_HECI_MESSAGE_HEADER     *MessageHeader,
  OUT      UINT32                      *MessageData,
  IN OUT   UINT32                      *Length
  );

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval    EFI_SUCCESS          One message packet sent.
  @retval    EFI_DEVICE_ERROR     Failed to initialize HECI
  @retval    EFI_TIMEOUT          HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HEC

**/
EFI_STATUS
HeciSendMsg (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MeAddress
  );

/**
  Function sends one messsage packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication

**/
EFI_STATUS
HeciPacketWrite (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      HeciMemBar,
  IN       SPS_HECI_MESSAGE_HEADER     *MessageHeader,
  IN       UINT32                      *MessageData
  );

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in, out] Message         Pointer to the message buffer.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in, out] Length          Length of the message in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the bufferbefore timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
HeciSendwAck (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN OUT   UINT32                      *Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MeAddress
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
  IN      UINT32                       ReadPointer,
  IN      UINT32                       WritePointer,
  IN      UINT32                       BufferDepth
  );

/**
  Used for calculating timeouts

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] Start                Snapshot of the HPET timer
  @param[in] End                  Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

  @retval None
**/
volatile
UINT32 *
StartTimer (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  OUT      UINT32                      *Start,
  OUT      UINT32                      *End,
  IN       UINT32                      Time
  );

/**
  Used to determine if a timeout has occured.

  @param[in] Start                Snapshot of the HPET timer when the timeout period started.
  @param[in] End                  Calculated time when timeout period will be done.
  @param[in] HpetTimer            The value of High Precision Event Timer

  @retval EFI_TIMEOUT             Timeout occured.
  @retval EFI_SUCCESS             Not yet timed out
**/
EFI_STATUS
Timeout (
  IN  UINT32                      Start,
  IN  UINT32                      End,
  IN  volatile UINT32             *HpetTimer
  );

/**
  Get an abstract Intel ME Status from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MeStatus             Pointer for status report,

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *MeStatus
  );

/**
  Get an abstract ME operation mode from firmware status
  register. This is used to control BIOS flow for different
  Intel ME functions.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MeMode               Pointer for ME Mode report,

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *MeMode
  );

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval UINT8                   Number of filled slots.
**/
UINT8
FilledSlots (
  IN  UINT32                      ReadPointer,
  IN  UINT32                      WritePointer
  );

#endif /// _HECI_INIT_H_
