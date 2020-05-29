/** @file

 Ksc settings in DXE phase

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

#include <Library/DxeKscLib.h>

#include <Uefi.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include <Protocol/CpuIo.h>

EFI_CPU_IO_PROTOCOL   *mDxeKscLibCpuIo;

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
  The DXE library only requires CPU IO protocol, so this will locate CPU IO protocol
  and save it for future use.

Arguments:

  None.

Returns:

  EFI_SUCCESS       - KscLib is successfully initialized.

--*/
{
  EFI_STATUS Status;

  //
  // Locate CpuIo protocol
  //
  Status = gBS->LocateProtocol (&gEfiCpuIoProtocolGuid, NULL, (VOID **)&mDxeKscLibCpuIo);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
SendKscCommand (
  UINT8                 Command
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
  UINTN         Index;
  UINT8         KscStatus = 0;
  EFI_STATUS    Status;

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the KSC command
  //
  Status = mDxeKscLibCpuIo->Io.Write (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_C_PORT,
                             1,
                             &Command
                             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveKscStatus (
  UINT8                 *KscStatus
  )
/*++

Routine Description:

  Receives status from Keyboard System Controller.

Arguments:

  Status  - Status byte to receive

Returns:

  EFI_SUCCESS       - Always success

--*/
{
  EFI_STATUS Status = EFI_SUCCESS;

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  //
  // Read and return the status
  //
  Status = mDxeKscLibCpuIo->Io.Read (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_C_PORT,
                             1,
                             KscStatus
                             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
SendKscData (
  UINT8                 Data
  )
/*++

Routine Description:

  Sends data to Keyboard System Controller.

Arguments:

  Data  - Data byte to send

Returns:

  EFI_SUCCESS       - Success
  EFI_TIMEOUT       - Timeout
  Other             - Failed

--*/
{
  UINTN         Index;
  UINT8         KscStatus;
  EFI_STATUS    Status = EFI_SUCCESS;

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the data and return
  //
  Status = mDxeKscLibCpuIo->Io.Write (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_D_PORT,
                             1,
                             &Data
                             );
  ASSERT_EFI_ERROR (Status);

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
  UINTN         Index;
  UINT8         KscStatus;
  EFI_STATUS    Status = EFI_SUCCESS;

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Read KSC data and return
  //
  Status = mDxeKscLibCpuIo->Io.Read (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_D_PORT,
                             1,
                             Data
                             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
