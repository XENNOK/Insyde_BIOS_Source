/** @file
  Public include file for the HII Extension Library

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*****************************************************************************3*
*/

#ifndef __HII_EX_LIB_H__
#define __HII_EX_LIB_H__

UINT8 *
EFIAPI
HiiCreateActionOpCodeEx (
  IN VOID                                  *OpCodeHandle,
  IN EFI_QUESTION_ID                       QuestionId,
  IN EFI_STRING_ID                         Prompt,
  IN EFI_STRING_ID                         Help,
  IN EFI_STRING_ID                         TextTwo,
  IN UINT8                                 QuestionFlags,
  IN EFI_STRING_ID                         QuestionConfig
  );

UINT8 *
EFIAPI
HiiCreatePasswordOpCodeEx (
  IN VOID                                  *OpCodeHandle,
  IN EFI_QUESTION_ID                       QuestionId,
  IN EFI_VARSTORE_ID                       VarStoreId,
  IN UINT16                                VarOffset,
  IN EFI_STRING_ID                         Prompt,
  IN EFI_STRING_ID                         Help,
  IN EFI_STRING_ID                         TextTwo,
  IN UINT8                                 QuestionFlags,
  IN UINT16                                MinSize,
  IN UINT64                                MaxSize
  );

UINT8 *
EFIAPI
HiiCreateOneOfOptionOpCodeWithExtOp (
  IN VOID    *OpCodeHandle,
  IN UINT16  StringId,
  IN UINT8   Flags,
  IN UINT8   Type,
  IN UINT64  Value,
  IN VOID    *ExtendedOpCodeHandle
  );

UINT8 *
EFIAPI
HiiCreateImageOpCode (
  IN VOID           *OpCodeHandle,
  IN EFI_IMAGE_ID   ImageId
  );

#endif
