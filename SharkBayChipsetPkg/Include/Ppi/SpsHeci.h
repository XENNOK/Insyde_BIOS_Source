/** @file

  PEI Heci PPI

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

#ifndef _SPS_HECI_PPI_H_
#define _SPS_HECI_PPI_H_

///
/// HECI PPI GUID
/// This PPI provides an interface to communicate with Intel ME in PEI phase
///
#define SPS_PEI_HECI_PPI_GUID \
  { 0xB2AEA087, 0x1445, 0x4569, { 0xB6, 0x17, 0x6F, 0xBD, 0xAD, 0x03, 0x1A, 0xCC }}

typedef struct _SPS_PEI_HECI_PPI  SPS_PEI_HECI_PPI;

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
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_HECI_SENDWACK) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN OUT   UINT32                      *Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MEAddress
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
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_HECI_READ_MESSAGE) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      Blocking,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      *MessageBody,
  IN OUT   UINT32                      *Length
  );

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HEC
**/
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_HECI_SEND_MESSAGE) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MEAddress
  );

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
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_INITIALIZE_HECI) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *HeciMemBar
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
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_HECI_GET_ME_STATUS) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *Status
  );

/**
  Get an abstract ME operation mode from firmware status
  register. This is used to control BIOS flow for different
  Intel ME functions.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[out] MeMode              Pointer for ME Mode report,

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
typedef
EFI_STATUS
(EFIAPI *SPS_PEI_HECI_GET_ME_MODE) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *MeMode
  );

///
/// HECI PPI
/// The interface functions are for sending/receiving HECI messages between host and Intel ME subsystem
/// in PEI phase
///
struct _SPS_PEI_HECI_PPI {
  SPS_PEI_HECI_SENDWACK               SendwAck;       ///< Send HECI message and wait for respond
  SPS_PEI_HECI_READ_MESSAGE           ReadMsg;        ///< Read message from HECI
  SPS_PEI_HECI_SEND_MESSAGE           SendMsg;        ///< Send message to HECI
  SPS_PEI_INITIALIZE_HECI             InitializeHeci; ///< Init HECI
  SPS_PEI_HECI_GET_ME_STATUS          GetMeStatus;    ///< Get Intel ME Status register
  SPS_PEI_HECI_GET_ME_MODE            GetMeMode;      ///< Get Intel ME mode
};

extern EFI_GUID gSpsPeiHeciPpiGuid;

#endif
