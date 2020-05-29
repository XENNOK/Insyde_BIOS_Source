//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  SecureBootFormGuid.h

Abstract:

  Formset guids, form id and VarStore data structure for Boot Maintenance Manager.

--*/
#ifndef _SECURE_BOOT_FORM_GUID_H_
#define _SECURE_BOOT_FORM_GUID_H_


#define SECURE_BOOT_FORMSET_GUID \
  { \
    0xaa1305b9, 0x1f3, 0x4afb, 0x92, 0xe, 0xc9, 0xb9, 0x79, 0xa8, 0x52, 0xfd \
  }
//
// Secure Boot Manager rlative definitions
//
#define SECURE_BOOT_FORM_ID            0x1002

#define FORM_ENROLL_HASH_ID            0x1000

#define KEY_ENROLL_HASH                0x4432

#define SECURE_BOOT_CLASS              0x00
#define SECURE_BOOT_SUBCLASS           0x03

#define VARSTORE_ID_SECURE_BOOT        0x1003


#define FORM_ENROLL_FILE_ID            0x1034
#define FORM_ENROLL_FILE_END_ID        0x10FF

typedef struct {
  UINT8   SetupMode;
  UINT8   SecureBoot;
  UINT8   CustomSecurity;
  UINT8   EnforceSecureBoot;
  UINT8   ClearSecureSettings;
  UINT8   ResotreFactorySettings;
  UINT8   ResotreBackupSettings;
} SECURE_BOOT_NV_DATA;

#endif

