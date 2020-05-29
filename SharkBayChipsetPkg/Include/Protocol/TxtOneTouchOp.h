/** @file

  Txt specific PPI operation definition.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

*/

#ifndef _TXT_PPI_OPERATION_H_
#define _TXT_PPI_OPERATION_H_

///
/// EDK and EDKII have different GUID formats
///
#define TXT_ONE_TOUCH_OP_PROTOCOL_GUID \
  { 0xFA2338AD, 0x80DF, 0x49D0, { 0x93, 0x96, 0xCF, 0x71, 0x45, 0xD0, 0x3A, 0x76 }}
///
/// Extern the GUID for protocol users.
///
extern EFI_GUID                           gTxtOneTouchOpProtocolGuid;

///
/// Forward reference for ANSI C compatibility
///
typedef struct _TXT_ONE_TOUCH_OP_PROTOCOL TXT_ONE_TOUCH_OP_PROTOCOL;

///
/// Member functions
///
/**
  Extend PPI operation for TxT. 
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT

**/
typedef
EFI_STATUS
(EFIAPI *TXT_PPI_EXEC_OPERATION) (
  IN TXT_ONE_TOUCH_OP_PROTOCOL         *This,
  IN UINT8                             Command
  );

/**
  Confirmation dialog for TxT PPI
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT
  @param [in, out] Confirm             User confirm

**/
typedef
EFI_STATUS
(EFIAPI *TXT_CONFIRMATION_DIALOG) (
  IN     TXT_ONE_TOUCH_OP_PROTOCOL     *This,
  IN     UINT8                         Command,
  IN OUT BOOLEAN                       *Confirm
  );

/**
  Reset system. 
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
typedef
EFI_STATUS
(EFIAPI *TXT_RESET_SYSTEM) (
  IN TXT_ONE_TOUCH_OP_PROTOCOL         *This,
  IN UINT8                             Command
  );

struct _TXT_ONE_TOUCH_OP_PROTOCOL {
  TXT_PPI_EXEC_OPERATION  ExecuteOperation;
  TXT_CONFIRMATION_DIALOG ConfirmationDialog;
  TXT_RESET_SYSTEM        ResetSystem;
};

#endif
