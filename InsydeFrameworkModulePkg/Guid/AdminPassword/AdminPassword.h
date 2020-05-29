//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   AdminPassword.h
//;
//; Abstract:
//;
//;   The GUID definition for Insyde secure boot of admin password
//;

#ifndef __EFI_ADMIN_PASSWORD_H__
#define __EFI_ADMIN_PASSWORD_H__
#include "Tiano.h"
#include "EfiVariable.h"
//
// Globally "CheckAdminPassword" variable uses to unlock/lock insyde secure variable write access.
//
#define EFI_CHECK_ADMIN_PASSWORD_NAME    L"CheckAdminPassword"

//
// Globally "AdminPassword" variable uses to save current admin password. If this variable doesn't exist,
// it indicates system needn't check admin password while unlocking insyde secure variable write access.
//
#define EFI_ADMIN_PASSWORD_NAME          L"AdminPassword"

#define LOCK_SECURE_VARIABLE             0
#define UNLOCK_SECURE_VARIABLE           1

#define DELETE_ADMIN_PASSWORD            0
#define REPLACE_ADMIN_PASSWORD           1

//
// The variable size of Insyde secure variable relative definitions
//
// INSYDE_VARIABLE_PASSWORD_HEADER_SIZE is used for unlock or replace password.
// Total size of these two commands are INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + PasswordSize+ Action
// Password is encoded as (UINT8) PasswordLen + (CHAR8 *) PasswordStr.
// Action is a UINT8 data.
//
#define INSYDE_VARIABLE_PASSWORD_HEADER_SIZE  (sizeof (UINT64) + sizeof (WIN_CERTIFICATE) + sizeof (EFI_GUID))
#define CERT_DATA_SIZE                        0x10
#define INSYDE_VARIABLE_HEADER_SIZE           (INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + CERT_DATA_SIZE)
#define LOCK_PASSWORD_VARIABLE_SIZE           (INSYDE_VARIABLE_HEADER_SIZE + sizeof (UINT8))
#define DELETE_PASSWORD_VARIABLE_SIZE          INSYDE_VARIABLE_HEADER_SIZE

#define IBV_CERT_TYPE_INSYDE_SECURE_GUID \
  { 0xc107cfcf, 0xd0c6, 0x4590, 0x82, 0x27, 0xf9, 0xd7, 0xfb, 0x69, 0x44 ,0xb4 }

#define INSYDE_SECURE_FIRMWARE_PASSWORD_GUID \
  { 0x52775a6a, 0xf251, 0x4ef8, 0x84, 0xfa, 0xf0, 0x95, 0xbd, 0xeb, 0x61 ,0x74 }

extern EFI_GUID gIbvCertTypeInsydeSecureGuid;
extern EFI_GUID gInsydeSecureFirmwarePasswordGuid;

#endif