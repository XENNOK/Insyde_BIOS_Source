/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

  The internal header file includes the common header files, defines
  internal structure and functions used by ImageVerificationLib.

Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef __IMAGEVERIFICATIONLIB_H__
#define __IMAGEVERIFICATIONLIB_H__

#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SecurityManagementLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/VariableLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/TrEEMeasureOsLoaderAuthority.h>
#include <Guid/ImageAuthentication.h>
#include <IndustryStandard/PeImage.h>
#include <Guid/DebugMask.h>

#define EFI_CERT_TYPE_RSA2048_SHA256_SIZE 256
#define EFI_CERT_TYPE_RSA2048_SIZE        256
#define MAX_NOTIFY_STRING_LEN             64
#define TWO_BYTE_ENCODE                   0x82
#define SHORT_FORM_MASK                   0x80

//
// Image type definitions
//
#define IMAGE_UNKNOWN                         0x00000000
#define IMAGE_FROM_FV                         0x00000001
#define IMAGE_FROM_OPTION_ROM                 0x00000002
#define IMAGE_FROM_REMOVABLE_MEDIA            0x00000003
#define IMAGE_FROM_FIXED_MEDIA                0x00000004

//
// Authorization policy bit definition
//
#define ALWAYS_EXECUTE                         0x00000000
#define NEVER_EXECUTE                          0x00000001
#define ALLOW_EXECUTE_ON_SECURITY_VIOLATION    0x00000002
#define DEFER_EXECUTE_ON_SECURITY_VIOLATION    0x00000003
#define DENY_EXECUTE_ON_SECURITY_VIOLATION     0x00000004
#define QUERY_USER_ON_SECURITY_VIOLATION       0x00000005

//
// Support hash types
//
#define HASHALG_MD5                            0x00000000
#define HASHALG_SHA1                           0x00000001
#define HASHALG_SHA224                         0x00000002
#define HASHALG_SHA256                         0x00000003
#define HASHALG_SHA384                         0x00000004
#define HASHALG_SHA512                         0x00000005
#define HASHALG_MAX                            0x00000006

#define CERT_BUFFER_SIZE                       0x2000
//
// Set max digest size as SHA256 Output (32 bytes) by far
//
#define MAX_DIGEST_SIZE    SHA256_DIGEST_SIZE
//
//
// PKCS7 Certificate definition
//
typedef struct {
  WIN_CERTIFICATE Hdr;
  UINT8           CertData[1];
} WIN_CERTIFICATE_EFI_PKCS;

//
// SPC INDIRECT DATA
//
typedef struct  {
  UINTN  Length;
  UINT8  *Data;
} SPC_INDIRECT_DATA_CONTENT;


/**
  Retrieves the size, in bytes, of the context buffer required for hash operations.

  @return  The size, in bytes, of the context buffer required for hash operations.

**/
typedef
EFI_STATUS
(EFIAPI *HASH_GET_CONTEXT_SIZE)(
  VOID
  );

/**
  Initializes user-supplied memory pointed by HashContext as hash context for
  subsequent use.

  If HashContext is NULL, then ASSERT().

  @param[in, out]  HashContext  Pointer to  Context being initialized.

  @retval TRUE   HASH context initialization succeeded.
  @retval FALSE  HASH context initialization failed.

**/
typedef
BOOLEAN
(EFIAPI *HASH_INIT)(
  IN OUT  VOID  *HashContext
  );


/**
  Performs digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If HashContext is NULL, then ASSERT().

  @param[in, out]  HashContext  Pointer to the MD5 context.
  @param[in]       Data        Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength  Length of Data buffer in bytes.

  @retval TRUE   HASH data digest succeeded.
  @retval FALSE  Invalid HASH context. After HashFinal function has been called, the
                 HASH context cannot be reused.

**/
typedef
BOOLEAN
(EFIAPI *HASH_UPDATE)(
  IN OUT  VOID        *HashContext,
  IN      CONST VOID  *Data,
  IN      UINTN       DataLength
  );

/**
  Completes hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the context cannot be used again.

  If HashContext is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  HashContext  Pointer to the MD5 context
  @param[out]      HashValue   Pointer to a buffer that receives the HASH digest
                               value (16 bytes).

  @retval TRUE   HASH digest computation succeeded.
  @retval FALSE  HASH digest computation failed.

**/
typedef
BOOLEAN
(EFIAPI *HASH_FINAL)(
  IN OUT  VOID   *HashContext,
  OUT     UINT8  *HashValue
  );


//
// Hash Algorithm Table
//
typedef struct {
  EFI_GUID                 *Index;
  CHAR16                   *Name;           ///< Name for Hash Algorithm
  UINTN                    DigestLength;    ///< Digest Length
  UINT8                    *OidValue;       ///< Hash Algorithm OID ASN.1 Value
  UINTN                    OidLength;       ///< Length of Hash OID Value
} HASH_TABLE;



#endif
