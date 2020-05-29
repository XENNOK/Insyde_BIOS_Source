/** @file

 SMM KSC library implementation. 

 These functions need to be SMM safe.

 These functions require the SMM IO library (SmmIoLib) to be present.
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

#ifndef _SMM_KSCLIB_H_
#define _SMM_KSCLIB_H_

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
  );

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
  );

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
  );

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
  );

/**
  Receives status from Keyboard System Controller.

 @param[in]       parm1 Data byte received
 
 @retval EFI_SUCCESS Command success.
 @retval Other            Command failed
*/
EFI_STATUS
ReceiveKscStatus (
  OUT UINT8   *KscStatus
  );
#endif
