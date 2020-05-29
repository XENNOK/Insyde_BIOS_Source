/** @file

  EFI HECI Protocol

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_HECI_H
#define _EFI_HECI_H

#include "MeState.h"

#define HECI_PROTOCOL_GUID \
  { 0xCFB33810, 0x6E87, 0x4284, { 0xB2, 0x03, 0xA6, 0x6A, 0xBE, 0x07, 0xF6, 0xE8 }}

#define EFI_HECI_PROTOCOL_GUID  HECI_PROTOCOL_GUID
typedef struct _EFI_HECI_PROTOCOL  EFI_HECI_PROTOCOL;
//[-start-120808-IB10820098-modify]//
//EFI_FORWARD_DECLARATION (EFI_HECI_PROTOCOL);
//[-end-120808-IB10820098-modify]//

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in][out] Message         Pointer to the message buffer.
  @param[in] SendLength           Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @exception EFI_SUCCESS          Command succeeded
  @exception EFI_DEVICE_ERROR     HECI Device error, command aborts abnormally
  @exception EFI_TIMEOUT          HECI does not return the buffer before timeout
  @exception EFI_BUFFER_TOO_SMALL Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_SENDWACK) (
  IN OUT  UINT32                  *Message,
  IN OUT  UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MEAddress
  );

/**
  Read the HECI Message from Intel ME with size in Length into
  buffer MessageBody. Set Blocking to BLOCKING and code will
  wait until one message packet is received. When set to
  NON_BLOCKING, if the circular buffer is empty at the time, the
  code will not wait for the message packet.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in][out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_READ_MESSAGE) (
  IN      UINT32                  Blocking,
  IN      UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
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
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_SEND_MESSAGE) (
  IN      UINT32                  *Message,
  IN      UINT32                  Length,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MEAddress
  )
;

/**
  Reset the HECI Controller with algorithm defined in the RS -
  Intel(R) Management Engine - Host Embedded Controller
  Interface Hardware Programming Specification (HPS)
  @param[in] none

  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_SUCCESS             Interface reset
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_RESET) (
  VOID
  );

/**
  Initialize the HECI Controller with algorithm defined in the
  RS - Intel(R) Management Engine - Host Embedded Controller
  Interface Hardware Programming Specification (HPS).
  Determines if the HECI device is present and, if present,
  initializes it for use by the BIOS.

  @param[in] None.

  @retval EFI_SUCCESS             HECI device is present and initialized
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_INIT) (
  VOID
  );

/**
  Re-initialize the HECI Controller with algorithm defined in the RS - Intel(R) Management Engine
  - Host Embedded Controller Interface Hardware Programming Specification (HPS).
  Heci Re-initializes it for Host

  @param[in] None.

  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_STATUS              Status code returned by ResetHeciInterface
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_REINIT) (
  VOID
  );

/**
  Reset Intel ME and timeout if Ready is not set after Delay timeout

  @param[in] Delay                Timeout value in microseconds

  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_SUCCESS             Me is ready
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_RESET_WAIT) (
  IN UINT32                            Delay
  );

/**
  Get an abstract Intel ME State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions.
  The ME status information is obtained by sending HECI messages
  to Intel ME and is used both by the platform code and
  reference code. This will optimize boot time because system
  BIOS only sends each HECI message once. It is recommended to
  send the HECI messages to Intel ME only when ME mode is normal
  (Except for HMRFPO Disable Message) and ME State is NORMAL or
  RECOVERY (Suitable for AT and Kernel Messaging only).

  @param[out] MeStatus            Pointer for abstract status report,
                                  see MeState.h - Abstract ME status definitions.

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_GET_ME_STATUS) (
  OUT UINT32                      *Status
  );

/**
  Get an abstract ME operation mode from firmware status
  register. This is used to control BIOS flow for different
  Intel ME functions.

  @param[out] MeMode              Pointer for ME Mode report,
                                  see MeState.h - Abstract ME Mode definitions.

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
typedef
EFI_STATUS
(EFIAPI *EFI_HECI_GET_ME_MODE) (
  OUT UINT32                       *Mode
  );

///
/// HECI protocol provided for DXE phase
/// The interface functions are for sending/receiving HECI messages between host and Intel ME subsystem.
/// There is also support to control HECI Initialization and get Intel ME status.
///
struct _EFI_HECI_PROTOCOL {
  EFI_HECI_SENDWACK              SendwACK;    ///< Send HECI message and wait for respond
  EFI_HECI_READ_MESSAGE          ReadMsg;     ///< Receive HECI message
  EFI_HECI_SEND_MESSAGE          SendMsg;     ///< Send HECI message
  EFI_HECI_RESET                 ResetHeci;   ///< Reset HECI device
  EFI_HECI_INIT                  InitHeci;    ///< Initialize HECI device
  EFI_HECI_RESET_WAIT            MeResetWait; ///< Intel ME Reset Wait Timer
  EFI_HECI_REINIT                ReInitHeci;  ///< Re-initialize HECI
  EFI_HECI_GET_ME_STATUS         GetMeStatus; ///< Get Intel ME Status register
  EFI_HECI_GET_ME_MODE           GetMeMode;   ///< Get Intel ME mode
};

extern EFI_GUID gEfiHeciProtocolGuid;

#endif /// _EFI_HECI_H
