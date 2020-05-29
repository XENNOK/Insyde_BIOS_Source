//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include EFI_GUID_DEFINITION (ImageAuthentication)

EFI_GUID  gEfiImageSecurityDatabaseGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;

EFI_GUID_STRING(&gEfiImageSecurityDatabaseGuid, "Image Security Database", "Image Security Database Guid");

EFI_GUID  gEfiCertSha256Guid = EFI_CERT_SHA256_GUID;

EFI_GUID_STRING(&gEfiCertSha256Guid, "Certification SHA256", "Certification SHA256 Guid");

EFI_GUID  gEfiCertRsa2048Guid = EFI_CERT_RSA2048_GUID;

EFI_GUID_STRING(&gEfiCertRsa2048Guid, "Certification RSA2048", "Certification RSA2048 Guid");

EFI_GUID  gEfiCertRsa2048Sha256Guid = EFI_CERT_RSA2048_SHA256_GUID;

EFI_GUID_STRING(&gEfiCertRsa2048Sha256Guid, "Certification RSA2048 and SHA256", "Certification RSA2048 and SHA256 Guid");

EFI_GUID  gEfiCertSha1Guid = EFI_CERT_SHA1_GUID;

EFI_GUID_STRING(&gEfiCertSha1Guid, "Certification SHA1", "Certification SHA1 Guid");

EFI_GUID  gEfiCertRsa2048Sha1Guid = EFI_CERT_RSA2048_SHA1_GUID;

EFI_GUID_STRING(&gEfiCertRsa2048Sha1Guid, "Certification RSA2048 and SHA1", "Certification RSA2048 and SHA1 Guid");

EFI_GUID  gEfiCertX509Guid = EFI_CERT_X509;

EFI_GUID_STRING(&gEfiCertX509Guid, "Certification X509", "Certification X509 Guid");

EFI_GUID  gEfiCertPkcs7Guid = EFI_CERT_PKCS7_GUID;

EFI_GUID_STRING(&gEfiCertPkcs7Guid, "Certification PKCS7", "Certification PKCS7 Guid");