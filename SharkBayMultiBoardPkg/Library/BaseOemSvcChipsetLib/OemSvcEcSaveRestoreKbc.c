/** @file
  Provide hook function for OEM to implement save and restore KBC. 
  
;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseOemSvcChipsetLib.h>

#include <Uefi.h>
#include <OemEc.h>
#include <Library/IoLib.h>
#include <Library/CommonEcLib.h>

UINT8 KbcCmdByte;


/**
 This function offers an interface to modify FLASH_DEVICE matrix before the system detects FlashDevice.
 
 @param[in, out]    ***OemFlashDevice   On entry, points to FLASH_DEVICE matrix.
                                        On exit , the size of updated FLASH_DEVICE matrix.
 @param[in, out]    *Size               On entry, the size of FLASH_DEVICE matrix.
                                        On exit , the size of updated FLASH_DEVICE matrix.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcEcSaveRestoreKbc (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  UINT8                              KbdIrqState = 0;
  UINT8                              SaveKbdIrqState = 0;

  if (SaveRestoreFlag) {
    //
    // Restore Keyboard command byte
    //
    IoWrite8 (KEY_CMD_STATE, KBC_WRITE_CMD_BYTE);
    WaitKbcIbe (KEY_CMD_STATE);

    IoWrite8 (KEY_DATA, KbcCmdByte);
    WaitKbcIbe (KEY_CMD_STATE);
  } else {
    //
    // Save Keyboard command byte
    //

    // Disable KBD IRQ
    KbdIrqState = IoRead8 (IRQ_8259_MASK);
    SaveKbdIrqState = KbdIrqState;
    KbdIrqState |= 2;
    IoWrite8 (IRQ_8259_MASK, KbdIrqState);

    WaitKbcIbe (KEY_CMD_STATE);

    // Send read command
    IoWrite8 (KEY_CMD_STATE, KBC_READ_CMD_BYTE);

    WaitKbcObf (KEY_CMD_STATE);

    KbcCmdByte = IoRead8 (KEY_DATA);
    IoWrite8 (IRQ_8259_MASK, SaveKbdIrqState);
  }

  return EFI_UNSUPPORTED;
}
