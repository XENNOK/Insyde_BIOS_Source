/** @file

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

#ifndef _SMM_PASSWORD_H_
#define _SMM_PASSWORD_H_

#include <Uefi.h>

#define BITS(a)   (1 << (a))
#define HDD_SECURITY_SUPPORT BITS(0)
#define HDD_SECURITY_ENABLE  BITS(1)
#define HDD_SECURITY_LOCK    BITS(2)
#define HDD_SECURITY_FROZEN  BITS(3)
#define HDD_SECURITY_EXPIRED BITS(4)

/**
 S3 resume password entry.

 @param [in]   ResumeCheckFlag  if ResumeCheckFlag == 0x01 the show password dialog
                                and waiting user pass current password.
                                if  != 0x01 only call unlock harddisk
                                servie routine

 @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
CheckPasswordEntry(
  IN UINT8 ResumeCheckFlag
  );
#endif

