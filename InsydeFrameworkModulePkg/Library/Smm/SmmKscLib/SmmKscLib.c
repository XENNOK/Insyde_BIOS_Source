//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   SMM KSC library implementation.
//;
//;   These functions need to be SMM safe.
//;
//;   These functions require the SMM IO library (SmmIoLib) to be present.
//;   Caller must link those libraries and have the proper include path.
//;

#include "KscLib.h"
#include "SmmIoLib.h"

//
// Function implemenations
//
EFI_STATUS
InitializeKscLib (
  VOID
  )
/*++

Routine Description:

  Initialize the library.
  The SMM library only requires SMM IO library and has no initialization.
  However, this must be called prior to use of any other KSC library functions
  for future compatibility.

Arguments:

  None.

Returns:

  EFI_SUCCESS       - KscLib is successfully initialized.

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
SendKscCommand (
  UINT8   Command
  )
/*++

Routine Description:

  Sends command to Keyboard System Controller.

Arguments:

  Command  - Command byte to send

Returns:

  EFI_SUCCESS       - Command success
  EFI_DEVICE_ERROR  - Command error

--*/
{
  UINTN   Index;
  UINT8   KscStatus = 0;

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    SmmStall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the KSC command
  //
  SmmIoWrite8 (KSC_C_PORT, Command);

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveKscStatus (
  UINT8   *KscStatus
  )
/*++

Routine Description:

  Receives status from Keyboard System Controller.

Arguments:

  KscStatus  - Status byte to receive

Returns:

  EFI_SUCCESS       - Always success

--*/
{
  //
  // Read and return the status
  //
  *KscStatus = SmmIoRead8 (KSC_C_PORT);

  return EFI_SUCCESS;
}

EFI_STATUS
SendKscData (
  UINT8   Data
  )
/*++

Routine Description:

  Sends data to Keyboard System Controller.

Arguments:

  Data  - Data byte to send

Returns:

  EFI_SUCCESS       - Success
  EFI_DEVICE_ERROR  - Error

--*/
{
  UINTN   Index;
  UINT8   KscStatus;

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    SmmStall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the data and return
  //
  SmmIoWrite8 (KSC_D_PORT, Data);

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveKscData (
  UINT8   *Data
 )
/*++

Routine Description:

  Receives data from Keyboard System Controller.

Arguments:

  Data  - Data byte received

Returns:

  EFI_SUCCESS       - Read success
  EFI_DEVICE_ERROR  - Read error

--*/
{
  UINTN   Index;
  UINT8   KscStatus;

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    SmmStall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Read KSC data and return
  //
  *Data = SmmIoRead8 (KSC_D_PORT);

  return EFI_SUCCESS;
}
