/** @file

  This protocol converts from standard EFI key data structures to strings and
  from strings to standard EFI key data structures.

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

#ifndef __H2O_KEY_DESC_PROTOCOL_H__
#define __H2O_KEY_DESC_PROTOCOL_H__

#define H2O_KEY_DESC_PROTOCOL_GUID \
  { \
    0xe911342e, 0x7b6b, 0x4257, {0x83, 0xfb, 0x4f, 0x0f, 0x41, 0xd9, 0x31, 0xca} \
  }

typedef struct _H2O_KEY_DESC_PROTOCOL  H2O_KEY_DESC_PROTOCOL;

/**
  Converts from standard EFI key data structures to strings.

  @param[in]      This             Pointer to current instance of this protocol.
  @param[in]      KeyData          The key data user want to change.
  @param[out]     String           The string has been changed.

  @retval EFI_SUCCESS            Function completed successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_KEY_DESC_TO_STRING)(
  IN     H2O_KEY_DESC_PROTOCOL                *This,
  IN     EFI_KEY_DATA                         *KeyData,
  OUT    CHAR16                               **String
  );

/**
  Converts from strings to standard EFI key data structures.

  @param[in]      This             Pointer to current instance of this protocol.
  @param[in]      String           The string user want to change.
  @param[out]     KeyData          The key data has been changed.

  @retval EFI_SUCCESS            Function completed successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_KEY_DESC_FROM_STRING)(
  IN     H2O_KEY_DESC_PROTOCOL                *This,
  IN     CHAR16                               *String,
  OUT    EFI_KEY_DATA                         *KeyData
  );

/**
  Size              Unsigned integer specifies the size of the structure, in bytes.
  KeyToString       Attach a specific console to this display engine.
  KeyFromString     Detach a specific console from this display engine.
*/
struct _H2O_KEY_DESC_PROTOCOL {
  UINT32                                       Size;

  H2O_KEY_DESC_TO_STRING                       KeyToString;
  H2O_KEY_DESC_FROM_STRING                     KeyFromString;
};

extern EFI_GUID gH2OKeyDescProtocolGuid;

#endif
