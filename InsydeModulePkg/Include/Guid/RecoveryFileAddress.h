/** @file
  GUIDs used for System Non Volatile HOB to save the recovery file address
  other phase to get revovery file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _RECOVERY_FILE_ADDRESS_GUID_H_
#define _RECOVERY_FILE_ADDRESS_GUID_H_

#define EFI_RECOVERY_FILE_ADDRESS_GUID \
  { \
    0xc717d94f, 0xb2b9, 0x4859, 0xb5, 0x77, 0xae, 0xd7, 0x99, 0x56, 0x55, 0x1f \
  }

extern EFI_GUID gEfiRecoveryFileAddressGuid;

#endif
