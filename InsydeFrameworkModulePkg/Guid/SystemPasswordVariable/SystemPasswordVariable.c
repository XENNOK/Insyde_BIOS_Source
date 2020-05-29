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

#include "Tiano.h"
#include EFI_GUID_DEFINITION (SystemPasswordVariable)

EFI_GUID  gEfiSupervisorPwGuid = EFI_SYS_SUPERVISOR_VAR_GUID;

EFI_GUID_STRING (&gEfiSupervisorPwGuid, "Guid for System Supervisor password", "Guid for System Supervisor password");

EFI_GUID  gEfiUserPwGuid = EFI_SYS_USER_VAR_GUID;

EFI_GUID_STRING (&gEfiUserPwGuid, "Guid for System User password", "Guid for System User password");
EFI_GUID  gEfiSupervisorPwHobGuid = EFI_PEI_SYS_SUPERVISOR_HOB_GUID;

EFI_GUID_STRING (&gEfiSupervisorPwHobGuid, "Guid for System Supervisor password Hob", "Guid for System Supervisor password Hob");

EFI_GUID  gInsydeSecureFirmwarePasswordHobGuid = EFI_INSYDE_SECURE_FIRMWARE_PASSWORD_HOB_GUID;

EFI_GUID_STRING (&gInsydeSecureFirmwarePasswordHobGuid, "Guid for Insyde Secure Firmware password", "Guid for Insyde Secure Firmware password");

EFI_GUID  gEfiUserPwHobGuid = EFI_PEI_SYS_USER_HOB_GUID;

EFI_GUID_STRING (&gEfiUserPwHobGuid, "Guid for System User password Hob", "Guid for System User password Hob");

EFI_GUID  gEfiPowerOnPwSCUHobGuid = EFI_PEI_SYS_SCU_REFERENCE_HOB_GUID;

EFI_GUID_STRING (&gEfiPowerOnPwSCUHobGuid, "Guid for System SCU information Hob", "Guid for System SCU information Hob");

