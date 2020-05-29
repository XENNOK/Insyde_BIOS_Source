/** @file
  EFI SysPassword Protocol

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SYS_PASSWORD_DXE_H_
#define _SYS_PASSWORD_DXE_H_

#include <Uefi.h>

#include <Guid/HobList.h>
#include <Guid/SystemPasswordVariable.h>
#include <Guid/AdminPassword.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PostCodeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/VariableSupportLib.h>

#include <Protocol/SysPasswordService.h>
#include <Protocol/Hash.h>

typedef struct {
  UINTN                                   Signature;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       SysPasswordProtocol;
  SYS_PASSWORD_SETUP_INFO_DATA            *SysPasswordSetupInfoData;
} SYS_PASSWORD_PRIVATE;

#define SYS_PASSWORD_SIGNATURE            SIGNATURE_32 ('s', 'y', 'p', 'w')
#define GET_PRIVATE_FROM_SYS_PASSWORD(a)  CR (a, SYS_PASSWORD_PRIVATE, SysPasswordProtocol, SYS_PASSWORD_SIGNATURE)
#define PASSWORD_KIND_MAX_LENGTH          50

//
//  If SYS_PASSWORD_IN_CMOS = YES
//  The maximum length is 10.
//  The password would be stored
//  if SYS_PASSWORD_IN_CMOS = YES and DEFAULT_PASSWORD_MAX_LENGTH > 10.
//
#define SYS_PASSWORD_MAX_NUMBER           FixedPcdGet32 (PcdDefaultSysPasswordMaxLength)

#endif // _SYS_PASSWORD_DXE_H_