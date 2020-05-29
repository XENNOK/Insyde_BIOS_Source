/** @file

  AmtReadyToBoot Protocol definitions

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

#ifndef _AMT_READY_TO_BOOT_PROTOCOL_H_
#define _AMT_READY_TO_BOOT_PROTOCOL_H_

///
/// AMT Readt to Boot Protocol
/// This protocol performs all Management Engine task
///
#define AMT_READY_TO_BOOT_PROTOCOL_GUID \
  { 0x40B09B5A, 0xF0EF, 0x4627, { 0x93, 0xD5, 0x27, 0xF0, 0x4B, 0x75, 0x4D, 0x05 }}

typedef struct _AMT_READY_TO_BOOT_PROTOCOL  AMT_READY_TO_BOOT_PROTOCOL;

/**
  Signal an event for Amt ready to boot.

  @param[in] None

  @retval EFI_SUCCESS             Mebx launched or no controller
**/
typedef
EFI_STATUS
(EFIAPI *AMT_READY_TO_BOOT_PROTOCOL_SIGNAL) (
  VOID
  );

///
/// AMT Readt to Boot Protocol
/// The interface functions are for Performing all Management Engine task
///
struct _AMT_READY_TO_BOOT_PROTOCOL {
  AMT_READY_TO_BOOT_PROTOCOL_SIGNAL Signal; ///< Performs all Management Engine task
};

extern EFI_GUID gAmtReadyToBootProtocolGuid;

#endif
