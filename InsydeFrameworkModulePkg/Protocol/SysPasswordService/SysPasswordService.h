//;******************************************************************************
//;* Copyright (c) 1983-2010, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EFI_SYS_PASSWORD_SERVICE_PROTOCOL_H
#define _EFI_SYS_PASSWORD_SERVICE_PROTOCOL_H

#define EFI_SYS_PASSWORD_SERVICE_PROTOCOL_GUID \
  { \
    0x55d662ce, 0x2ec6, 0x4451, 0xa3, 0x5, 0xba, 0x6, 0xb5, 0x7, 0x86, 0xc5 \
  }

EFI_FORWARD_DECLARATION (EFI_SYS_PASSWORD_SERVICE_PROTOCOL);

//
//  If SYS_PASSWORD_IN_CMOS = YES
//  The maximum length is 10.
//  The password would be stored 
//  if SYS_PASSWORD_IN_CMOS = YES and DEFAULT_PASSWORD_MAX_LENGTH > 10.
//
#define SYS_PASSWORD_MAX_NUMBER            10


typedef enum {
  SystemSupervisor     = 0,
  SystemUser
} PASSWORD_TYPE;

typedef enum {
  GetLength            = 0,
  SetLength
} PASSWORD_LENGTH_TYPE;

typedef struct _SYS_PASSWORD_INFO_DATA{
  UINTN                Flag;
  CHAR16               *InputString;
  UINTN                StringLength;
  EFI_STATUS           SystemPasswordStatus;
  UINTN                NumOfEntry;
} SYS_PASSWORD_INFO_DATA;

typedef enum {
  NoUserType             = 0,
  SystemSupervisorType,
  SystemUserType
} CURRENT_USER_TYPE;

//
//  System Password Setup Information
//
#define BITS(a)                        (1 << (a))
#define MAX_SYS_PASSWORD_LENGTH_BIT    BITS(0)
#define CURRENT_USER_BIT               BITS(1)
#define SYS_PASSWORD_CHECK_BIT         BITS(2)

#define SYS_PW_SETUP_INFO_DATA_MASK    (BITS(3) - 1)

typedef struct {
  UINT32                                SetupDataValidBits;
  UINTN                                 MaxSysPasswordLength;
  CURRENT_USER_TYPE                     CurrentUser;
  BOOLEAN                               HaveSysPasswordCheck;
} SYS_PASSWORD_SETUP_INFO_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_PASSWORD_STATUS) (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  BOOLEAN                              SupervisorOrUser
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_PASSWORD) (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  VOID                                 *PasswordPtr,
    IN  UINTN                                PasswordLength,
    IN BOOLEAN                               SupervisorOrUser
);

typedef
EFI_STATUS
(EFIAPI *EFI_CHECK_PASSWORD) (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  VOID                                 *PasswordPtr,
    IN  UINTN                                PasswordLength,
    IN  BOOLEAN                              SupervisorOrUser
);

typedef
EFI_STATUS
(EFIAPI *EFI_DISABLE_PASSWORD) (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  BOOLEAN                              SupervisorOrUser
);

typedef
EFI_STATUS
(EFIAPI *EFI_LOCK_PASSWORD) (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This
)
/*++

Routine Description:

  Change password state from unlocked state to locked state.

Arguments:

  This                  - Pointer to EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.

Returns:

  EFI_SUCCESS           - Change password state to locked state.
  EFI_UNSUPPORTED       - The password doesn't saved in variable storage.
  EFI_ALREADY_STARTED   - The password state is already in locked state.

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_UNLOCK_PASSWORD) (
   IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
   IN  VOID                                 *PasswordPtr,
   IN  UINTN                                PasswordLength
)
/*++

Routine Description:

  Change the Insyde password state to unlocked state

Arguments:

  This                  - Pointer to EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  PasswordPtr           - The null-terminated string that is the input password.
  PasswordLength        - The password length in bytes of PasswordPtr.

Returns:

  EFI_SUCCESS           - Change password state to unlocked state.
  EFI_UNSUPPORTED       - The password doesn't saved in variable storage.
  EFI_ALREADY_STARTED   - The password state is already in unlocked state.

--*/
;

typedef
EFI_STATUS
(EFIAPI* EFI_GET_SYS_PSWD_SETUP_INFO_DATA) (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  OUT SYS_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer
)
/*++

  Routine Description:
  
    Get the setup info. data

  Arguments:

    This                       - EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
    SetupInfoBuffer            - buffer to store the setup info. data

  Returns:

    EFI_SUCCESS                - get finish

--*/
;

typedef
EFI_STATUS
(EFIAPI* EFI_SET_SYS_PSWD_SETUP_INFO_DATA) (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  SYS_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer,
  IN  UINT32                            SetupBits
)
/*++

  Routine Description:
  
    Set the setup info. data

  Arguments:

    This                       - EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
    SetupInfoBuffer            - buffer to store the setup info. data
    SetupBits                  - which setup to be modified

  Returns:

    EFI_SUCCESS                - set finish

--*/
;

typedef struct _EFI_SYS_PASSWORD_SERVICE_PROTOCOL {
  EFI_GET_PASSWORD_STATUS               GetStatus;
  EFI_SET_PASSWORD                      SetPassword;
  EFI_CHECK_PASSWORD                    CheckPassword;
  EFI_DISABLE_PASSWORD                  DisablePassword;
  EFI_LOCK_PASSWORD                     LockPassword;
  EFI_UNLOCK_PASSWORD                   UnlockPassword;
  EFI_GET_SYS_PSWD_SETUP_INFO_DATA      GetSysPswdSetupInfoData;
  EFI_SET_SYS_PSWD_SETUP_INFO_DATA      SetSysPswdSetupInfoData;
} EFI_SYS_PASSWORD_SERVICE_PROTOCOL;

extern EFI_GUID gEfiSysPasswordServiceProtocolGuid;
#endif
