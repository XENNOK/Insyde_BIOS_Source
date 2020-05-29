/** @file

  Implementation file for SPS HECI functionality in PEI Phase.
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

#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

#include <SpsMe.h>
#include <Ppi/SpsHeci.h>

/**
  Retrieves the HECI Ppi for internal ues.
**/
EFI_STATUS
SpsInternalGetPeiHeciPpi (
  IN OUT SPS_PEI_HECI_PPI              **SpsHeciPpi
  )
{
  EFI_STATUS                 Status;
  SPS_PEI_HECI_PPI           *TempSpsHeciPpi;

  if (SpsHeciPpi == NULL) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: SpsHeciPpi is NULL.\n"));
    return EFI_INVALID_PARAMETER;
  }
  TempSpsHeciPpi = NULL;

  Status = PeiServicesLocatePpi (
             &gSpsPeiHeciPpiGuid,       // GUID
             0,                         // INSTANCE
             NULL,                      // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &TempSpsHeciPpi  // PPI
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Locate HECI PPI Fail! (%r).\n", Status));
    return Status;
  }

  *SpsHeciPpi = TempSpsHeciPpi;
  if ((TempSpsHeciPpi == NULL) || (*SpsHeciPpi == NULL)) {
    ///
    ///BUGBUG: Can not Locate SPS PEI Heci PPI.
    ///  Status     = EFI_SUCCESS
    ///  SpsHeciPpi = 0x00000000;
    ///
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: SpsHeciPpi Locate Success but address invaild (%x)\n", *SpsHeciPpi));
    return EFI_NOT_STARTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SpsInternalGetHeciMemBar (
  OUT UINT32                           *HeciMemBar
  )
{
  CONST EFI_PEI_SERVICES     **PeiServices;
  SPS_PEI_HECI_PPI           *SpsHeciPpi;
  EFI_STATUS                 Status;

  PeiServices = GetPeiServicesTablePointer ();

  Status = SpsInternalGetPeiHeciPpi (&SpsHeciPpi);
  if ((EFI_ERROR (Status)) || (SpsHeciPpi == NULL)) {
    return EFI_NOT_STARTED;
  }

  if (HeciMemBar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsHeciPpi->InitializeHeci (PeiServices, SpsHeciPpi, HeciMemBar);
  if ((EFI_ERROR (Status)) || (*HeciMemBar == 0)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: HECI initialization (%r)\n", Status));
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: HeciMemBar = 0x%08x\n", HeciMemBar));
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Heci MMIO Bar not programmed\n"));
    return EFI_NOT_READY;
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
  CONST EFI_PEI_SERVICES     **PeiServices;
  SPS_PEI_HECI_PPI           *SpsHeciPpi;
  EFI_STATUS                 Status;
  UINT32                     HeciMemBar;

  if ((Message == NULL) || (RecLength == NULL)) {
    return EFI_INVALID_LANGUAGE;
  }

  PeiServices = GetPeiServicesTablePointer ();

  Status = SpsInternalGetPeiHeciPpi (&SpsHeciPpi);
  if ((EFI_ERROR (Status)) || (SpsHeciPpi == NULL)) {
    return EFI_NOT_STARTED;
  }

  Status = SpsInternalGetHeciMemBar (&HeciMemBar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SpsHeciPpi->SendwAck (
                         PeiServices,
                         SpsHeciPpi,
                         Message,
                         HeciMemBar,
                         &Length,
                         HostAddress,
                         MEAddress
                         );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Send HECI Message and wait for response => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
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
  CONST EFI_PEI_SERVICES     **PeiServices;
  SPS_PEI_HECI_PPI           *SpsHeciPpi;
  EFI_STATUS                 Status;
  UINT32                     HeciMemBar;

  PeiServices = GetPeiServicesTablePointer ();

  Status = SpsInternalGetPeiHeciPpi (&SpsHeciPpi);
  if ((EFI_ERROR (Status)) || (SpsHeciPpi == NULL)) {
    return EFI_NOT_STARTED;
  }

  Status = SpsInternalGetHeciMemBar (&HeciMemBar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SpsHeciPpi->ReadMsg (
                         PeiServices,
                         SpsHeciPpi,
                         Blocking,
                         HeciMemBar,
                         MessageBody,
                         Length
                         );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Read HECI Message => (%r)\n", Status));
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
  CONST EFI_PEI_SERVICES     **PeiServices;
  SPS_PEI_HECI_PPI           *SpsHeciPpi;
  EFI_STATUS                 Status;
  UINT32                     HeciMemBar;

  PeiServices = GetPeiServicesTablePointer ();

  Status = SpsInternalGetPeiHeciPpi (&SpsHeciPpi);
  if ((EFI_ERROR (Status)) || (SpsHeciPpi == NULL)) {
    return EFI_NOT_STARTED;
  }

  Status = SpsInternalGetHeciMemBar (&HeciMemBar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SpsHeciPpi->SendMsg (
                         PeiServices,
                         SpsHeciPpi,
                         Message,
                         HeciMemBar,
                         Length,
                         HostAddress,
                         MeAddress
                         );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Send HECI Message => (%r)\n", Status));
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
  CONST EFI_PEI_SERVICES     **PeiServices;
  SPS_PEI_HECI_PPI           *SpsHeciPpi;
  EFI_STATUS                 Status;
  UINT32                     HeciMemBar;

  PeiServices = GetPeiServicesTablePointer ();

  Status = SpsInternalGetPeiHeciPpi (&SpsHeciPpi);
  if ((EFI_ERROR (Status)) || (SpsHeciPpi == NULL)) {
    return Status;
  }

  Status = SpsInternalGetHeciMemBar (&HeciMemBar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SpsHeciPpi->InitializeHeci (PeiServices, SpsHeciPpi, &HeciMemBar);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: HECI initialize => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}
