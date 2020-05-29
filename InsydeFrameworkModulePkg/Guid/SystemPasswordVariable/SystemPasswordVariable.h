//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   GUIDs used for System password drivers
//;

#ifndef _SYS_GUID_H_
#define _SYS_GUID_H_

#define EFI_SYS_SUPERVISOR_VAR_GUID \
  { \
    0x7f9102df, 0xe999, 0x4740, 0x80, 0xa6, 0xb2, 0x3, 0x85, 0x12, 0x21, 0x7b \
  }

#define EFI_SYS_USER_VAR_GUID \
  { \
    0x8cf3cfd3, 0xd8e2, 0x4e30, 0x83, 0xff, 0xb8, 0x6f, 0xc, 0x52, 0x2a, 0x5e \
  }

#define EFI_PEI_SYS_SUPERVISOR_HOB_GUID \
  { \
    0x92ee44d0, 0x2a8, 0x48de, 0xa2, 0x23, 0xfa, 0x13, 0xbf, 0xff, 0xac, 0xe2 \
  }

#define EFI_INSYDE_SECURE_FIRMWARE_PASSWORD_HOB_GUID \
  { \
    0xfef120d0, 0x847a, 0x476d, 0x80, 0x40, 0x95, 0xa1, 0xfa, 0x7c, 0xde, 0xf \
  }

#define EFI_PEI_SYS_USER_HOB_GUID \
  { \
    0x353310ce, 0xbb1f, 0x4682, 0xac, 0x14, 0xd4, 0x16, 0xe8, 0xa3, 0xb8, 0xf \
  }

#define EFI_PEI_SYS_SCU_REFERENCE_HOB_GUID \
  { \
    0xf636f50e, 0x3214, 0x4dc7, 0xa5, 0xc6, 0x1e, 0xc8, 0x29, 0xef, 0x43, 0x2c \
  }

extern EFI_GUID gEfiSupervisorPwHobGuid;
extern EFI_GUID gInsydeSecureFirmwarePasswordHobGuid;
extern EFI_GUID gEfiUserPwHobGuid;
extern EFI_GUID gEfiPowerOnPwSCUHobGuid;
extern EFI_GUID gEfiSupervisorPwGuid;
extern EFI_GUID gEfiUserPwGuid;

#endif



