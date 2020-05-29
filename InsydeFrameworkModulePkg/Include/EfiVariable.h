/*++

Copyright (c) 2004, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  EfiVariable.h

Abstract:

  Header file for EFI Variable Services

--*/

#ifndef _EFI_VARIABLE_H_
#define _EFI_VARIABLE_H_

#ifdef SECURE_BOOT_SUPPORT
#include EFI_GUID_DEFINITION (WinCertificate)

#define VARIABLE_STORE_SIGNATURE  EFI_SIGNATURE_32 ('$', 'V', 'S', 'S')
//
// According to Logo requirement, the maximum variable size is 56K
//
#define MAX_VARIABLE_SIZE         0xE000

#define MAX_VARIABLE_NAME_SIZE    0x100

#define VARIABLE_DATA             0x55AA

//
// Variable Store Header flags
//
#define VARIABLE_STORE_FORMATTED  0x5a
#define VARIABLE_STORE_HEALTHY    0xfe

//
// Variable Store Status
//
typedef enum {
  EfiRaw,
  EfiValid,
  EfiInvalid,
  EfiUnknown
} VARIABLE_STORE_STATUS;

//
// Variable State flags
//
#define VAR_IN_DELETED_TRANSITION     0xfe  // Variable is in obsolete transistion
#define VAR_DELETED                   0xfd  // Variable is obsolete
#define VAR_HEADER_VALID_ONLY         0x7f  ///< Variable header has been valid
#define VAR_ADDED                     0x3f  ///< Variable has been completely added
// #define VAR_ADDED                     0x7f  // Variable has been completely added
// #define IS_VARIABLE_STATE(_c, _Mask)  (BOOLEAN) (((~_c) & (~_Mask)) != 0)

#pragma pack(1)

typedef struct {
  UINT32  Signature;
  UINT32  Size;
  UINT8   Format;
  UINT8   State;
  UINT16  Reserved;
  UINT32  Reserved1;
} VARIABLE_STORE_HEADER;

typedef struct {
  UINT16    StartId;
  UINT8     State;
  UINT8     Reserved;
  UINT32    Attributes;
  UINT64    MonotonicCount;
  EFI_TIME  TimeStamp;
  UINT32    PubKeyIndex;
  UINT32    NameSize;
  UINT32    DataSize;
  EFI_GUID  VendorGuid;
} VARIABLE_HEADER;

typedef struct {
  UINT64    MonotonicCount;
  EFI_TIME  TimeStamp;
  UINT32    PubKeyIndex;
} VARIABLE_AUTH_HEADER;

///
/// This attribute is identified by the mnemonic 'HR'
/// elsewhere in this specification.
///
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS   0x00000010

/// NOTE - there is another UefiMultiPhase.h file in the code base.  Not sure
///             when used in a build.
///
/// Add defines for UEFI 2.3.1 coding
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS \
	                                              0x00000020
#define EFI_VARIABLE_APPEND_WRITE                 0x00000040

typedef struct {
  ///
  /// Included in the signature of
  /// AuthInfo.Used to ensure freshness/no
  /// replay. Updated during each
  /// "Write" access.
  ///
  EFI_TIME                     TimeStamp;
  ///
  /// Provides the authorization for the variable
  /// access. It is a signature across the
  /// variable data and the  Monotonic Count
  /// value. Caller uses Private key that is
  /// associated with a public key that has been
  /// provisioned via the key exchange.
  ///
  WIN_CERTIFICATE_UEFI_GUID   AuthInfo;
} EFI_VARIABLE_AUTHENTICATION_2;


///
/// AuthInfo is a WIN_CERTIFICATE using the wCertificateType
/// WIN_CERTIFICATE_UEFI_GUID and the CertType
/// EFI_CERT_TYPE_RSA2048_SHA256. If the attribute specifies
/// authenticated access, then the Data buffer should begin with an
/// authentication descriptor prior to the data payload and DataSize
/// should reflect the the data.and descriptor size. The caller
/// shall digest the Monotonic Count value and the associated data
/// for the variable update using the SHA-256 1-way hash algorithm.
/// The ensuing the 32-byte digest will be signed using the private
/// key associated w/ the public/private 2048-bit RSA key-pair. The
/// WIN_CERTIFICATE shall be used to describe the signature of the
/// Variable data *Data. In addition, the signature will also
/// include the MonotonicCount value to guard against replay attacks.
///
typedef struct {
  ///
  /// Included in the signature of
  /// AuthInfo.Used to ensure freshness/no
  /// replay. Incremented during each
  /// "Write" access.
  ///
  UINT64                      MonotonicCount;
  ///
  /// Provides the authorization for the variable
  /// access. It is a signature across the
  /// variable data and the  Monotonic Count
  /// value. Caller uses Private key that is
  /// associated with a public key that has been
  /// provisioned via the key exchange.
  ///
  WIN_CERTIFICATE_UEFI_GUID   AuthInfo;
} EFI_VARIABLE_AUTHENTICATION;

#pragma pack()
#else

#define VARIABLE_STORE_SIGNATURE  EFI_SIGNATURE_32 ('$', 'V', 'S', 'S')

#define MAX_VARIABLE_SIZE         0xE000

#define VARIABLE_DATA             0x55AA

//
// Variable Store Header flags
//
#define VARIABLE_STORE_FORMATTED  0x5a
#define VARIABLE_STORE_HEALTHY    0xfe

//
// Variable Store Status
//
typedef enum {
  EfiRaw,
  EfiValid,
  EfiInvalid,
  EfiUnknown
} VARIABLE_STORE_STATUS;

//
// Variable State flags
//
#define VAR_IN_DELETED_TRANSITION     0xfe  // Variable is in obsolete transistion
#define VAR_DELETED                   0xfd  // Variable is obsolete
#define VAR_ADDED                     0x7f  // Variable has been completely added
#define IS_VARIABLE_STATE(_c, _Mask)  (BOOLEAN) (((~_c) & (~_Mask)) != 0)

#pragma pack(1)

typedef struct {
  UINT32  Signature;
  UINT32  Size;
  UINT8   Format;
  UINT8   State;
  UINT16  Reserved;
  UINT32  Reserved1;
} VARIABLE_STORE_HEADER;

typedef struct {
  UINT16    StartId;
  UINT8     State;
  UINT8     Reserved;
  UINT32    Attributes;
  UINT32    NameSize;
  UINT32    DataSize;
  EFI_GUID  VendorGuid;
} VARIABLE_HEADER;

#pragma pack()

#endif

typedef struct {
  EFI_GUID  VendorGuid;
  CHAR16    *VariableName;
} PRESERVED_VARIABLE_TABLE;

#endif // _EFI_VARIABLE_H_
