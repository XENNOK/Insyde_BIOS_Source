//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include <Tiano.h>
#include EFI_PROTOCOL_DEFINITION (RngProtocol)

EFI_GUID gEfiRngProtocolGuid = EFI_RNG_PROTOCOL_GUID;

EFI_GUID gEfiRngAlgorithmSp80090Hash256Guid = EFI_RNG_ALGORITHM_SP800_90_HASH_256_GUID;
EFI_GUID gEfiRngAlgorithmSp80090Hmac256Guid = EFI_RNG_ALGORITHM_SP800_90_HMAC_256_GUID;
EFI_GUID gEfiRngAlgorithmSp80090Ctr256Guid  = EFI_RNG_ALGORITHM_SP800_90_CTR_256_GUID;
EFI_GUID gEfiRngAlgorithmX9313DesGuid       = EFI_RNG_ALGORITHM_X9_31_3DES_GUID;
EFI_GUID gEfiRngAlgorithmX931AesGuid        = EFI_RNG_ALGORITHM_X9_31_AES_GUID;
EFI_GUID gEfiRngAlgorithmRawGuid            = EFI_RNG_ALGORITHM_RAW_GUID;

EFI_GUID_STRING (&gEfiRngProtocolGuid, "EfiRngProtocol", "Efi Rng Protocol");
