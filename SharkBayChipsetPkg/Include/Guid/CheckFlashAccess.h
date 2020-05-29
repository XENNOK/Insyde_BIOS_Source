/** @file
 GUID and structure definition of check flash access function.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#ifndef _CHECK_FLASH_ACCESS_H_
#define _CHECK_FLASH_ACCESS_H_


#define CHECK_FLASH_ACCESS_GUID \
 {0x62fe5b98, 0xfda1, 0x4f1b, 0xa4, 0x39, 0x68, 0x4c, 0x6f, 0x22, 0x85, 0xb}

extern EFI_GUID gCheckFlashAccessGuid;

//
// Variable name that contains mail box address
//
#define CHECK_FLASH_ACCESS_VARIABLE   L"CheckFlashAccess"

//
// Chkec function definition
//
#define CHECK_FLASH_ACCESS_CONVERT          0
#define CHECK_FLASH_ACCESS_DO_NOT_CONVERT   1

#endif // _CHECK_FLASH_ACCESS_H_

