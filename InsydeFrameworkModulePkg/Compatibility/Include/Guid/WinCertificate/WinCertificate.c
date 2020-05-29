/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (WinCertificate)

EFI_GUID  gEfiCertTypeRsa2048Sha256Guid = EFI_CERT_TYPE_RSA2048_SHA256_GUID;

EFI_GUID_STRING(&gEfiCertTypeRsa2048Sha256Guid, "Certification Type RSA2048 and SHA256", "Certification Type RSA2048 and SHA256");
