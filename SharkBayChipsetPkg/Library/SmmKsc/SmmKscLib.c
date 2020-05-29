/** @file

  SMM KSC library implementation.

  These functions need to be SMM safe.
  
  These functions require the SMM IO library to be present.
  Caller must link those libraries and have the proper include path.

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

#include <Library/IoLib.h>
#include <Library/StallLib.h>
#include <KscLib.h>

BOOLEAN mSmmKscLibInitialized = FALSE;

/**
 This function initializes the KSC library. It must be called before using any of the other KSC 
 library functions.

 @param[in]       parm1 None
 
 @retval EFI_SUCCESS    KscLib is successfully initialized.
 @retval !EFI_SUCCESS  Function error.
*/
EFI_STATUS
InitializeKscLib (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Fail if EC doesn't exist.
  //
  if (IoRead8 (KSC_C_PORT) == 0xff) {
    mSmmKscLibInitialized = FALSE;
    Status                = EFI_DEVICE_ERROR;
  } else {
    mSmmKscLibInitialized = TRUE;
    Status                = EFI_SUCCESS;
  }

  return Status;
}

/**
 Send a command to the Keyboard System Controller.

 @param[in]       parm1 Command byte to send
 
 @retval EFI_SUCCESS Command success.
 @retval EFI_TIMEOUT Command timeout.
 @retval Other            Command failed
*/
EFI_STATUS
SendKscCommand (
  IN  UINT8   Command
  )

{
  UINTN Index;
  UINT8 KscStatus;

  KscStatus = 0;
  //
  // Verify if KscLib has been initialized, NOT if EC dose not exist.
  //
  if (mSmmKscLibInitialized == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Send the KSC command
  //
  IoWrite8 (KSC_C_PORT, Command);

  return EFI_SUCCESS;
}

/**
  Sends data to Keyboard System Controller.

 @param[in]       parm1 Data byte to send
 
 @retval EFI_SUCCESS Command success.
 @retval EFI_TIMEOUT Command timeout.
 @retval Other            Command failed
*/
EFI_STATUS
SendKscData (
  IN  UINT8   Data
  )

{
  UINTN Index;
  UINT8 KscStatus;

  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (mSmmKscLibInitialized == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Send the data and return
  ///
  IoWrite8 (KSC_D_PORT, Data);

  return EFI_SUCCESS;
}


/**
  Receives status from Keyboard System Controller.

 @param[in]       parm1 Data byte received
 
 @retval EFI_SUCCESS Command success.
 @retval Other            Command failed
*/
EFI_STATUS
ReceiveKscStatus (
  OUT UINT8   *KscStatus
  )

{
  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (mSmmKscLibInitialized == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Read and return the status
  ///
  *KscStatus = IoRead8 (KSC_C_PORT);

  return EFI_SUCCESS;
}

/**
  Receives data from Keyboard System Controller.

 @param[in]       parm1 Data byte received
 
 @retval EFI_SUCCESS Command success.
 @retval EFI_TIMEOUT Command timeout.
 @retval Other            Command failed
*/
EFI_STATUS
ReceiveKscData (
  OUT UINT8   *Data
  )

{
  UINTN Index;
  UINT8 KscStatus;

  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (mSmmKscLibInitialized == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Read KSC data and return
  //
  *Data = IoRead8 (KSC_D_PORT);

  return EFI_SUCCESS;
}

