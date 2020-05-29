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
//;   AdminPassword.c
//;
//; Abstract:
//;
//;   The GUID definition for Insyde secure boot of admin password
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (AdminPassword)

EFI_GUID  gIbvCertTypeInsydeSecureGuid = IBV_CERT_TYPE_INSYDE_SECURE_GUID;
EFI_GUID  gInsydeSecureFirmwarePasswordGuid = INSYDE_SECURE_FIRMWARE_PASSWORD_GUID;

EFI_GUID_STRING(&gIbvCertTypeInsydeSecureGuid, "IBV Certification Type Insyde Secure GUID", "IBV Certification Type Insyde Secure GUID");
EFI_GUID_STRING(&gInsydeSecureFirmwarePasswordGuid, "Insyde Secure Firmware Password GUID", "Insyde Secure Firmware Password GUID");
