//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _ADMINISTER_SECURE_BOOT_H_
#define _ADMINISTER_SECURE_BOOT_H_

//
// Globally "AdministerSecureBoot" variable to specify whether the system is currently booting
// in administer secure boot enable (1) or not (0). All other values are reserved.
//
#define EFI_ADMINISTER_SECURE_BOOT_NAME        L"AdministerSecureBoot"

//
// Globally "SecureBootEnforce" variable to specify whether the system is currently booting
// with image verification (1) or not (0) in user mode. All other values are reserved.
//
#define EFI_SECURE_BOOT_ENFORCE_NAME           L"SecureBootEnforce"

//
// Globally "CustomSecurity" variable to specify whether the security database has been customized
// by user. Security database has been customized (1) or not (0). All other values are reserved.
//
#define EFI_CUSTOM_SECURITY_NAME               L"CustomSecurity"

//
// Globally "RestoreFactoryDefault" variable to specify whether the system is currently booting
// with resotring to factory default (1) or not (0). All other values are reserved.
//
#define EFI_RESTORE_FACOTRY_DEFAULT_NAME       L"RestoreFactoryDefault"


#define ADD_HASH_IMAGE_FUN_NUM                 0x01
#define SECURE_BOOT_ENFORCE_FUN_NUM            0x02
#define CLEAR_ALL_SECURE_SETTINGS_FUN_NUM      0x03
#define RESTORE_FACOTRY_DEFAULT_FUN_NUM        0x04
#define UPDATE_PK_FUN_NUM                      0x05
#define UPDATE_KEK_FUN_NUM                     0x06
#define UPDATE_DB_FUN_NUM                      0x07
#define UPDATE_DBX_FUN_NUM                     0x08

#define UPDATE_AUTHENTICATED_VARIABLE          0x00
#define APPEND_AUTHENTICATED_VARIABLE          0x01


#define PLATFORM_DISABLE_SECURE_BOOT_SMI_GUID \
  {0xf6168400, 0x2d9e, 0x4725, 0x93, 0x80, 0xf9, 0x1b, 0x7b, 0x78, 0x3c, 0x2e}



#endif
