/** @file
 HII Extension Library implementation that uses DXE protocols and services.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalHiiExLib.h"

extern GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8 mHiiDefaultTypeToWidth[];

UINTN
EFIAPI
InternalHiiOpCodeHandlePosition (
  IN VOID  *OpCodeHandle
  );

UINT8 *
EFIAPI
InternalHiiCreateOpCodeExtended (
  IN VOID   *OpCodeHandle,
  IN VOID   *OpCodeTemplate,
  IN UINT8  OpCode,
  IN UINTN  OpCodeSize,
  IN UINTN  ExtensionSize,
  IN UINT8  Scope
  );

UINT8 *
EFIAPI
InternalHiiAppendOpCodes (
  IN VOID  *OpCodeHandle,
  IN VOID  *RawOpCodeHandle
  );

UINT8 *
EFIAPI
InternalHiiOpCodeHandleBuffer (
  IN VOID  *OpCodeHandle
  );

/**
 Create H2O_IFR_GUID_TEXT opcode.

 @param[in] OpCodeHandle              Handle to the buffer of opcodes.
 @param[in] PreviousOpCodeHeader      Pointer to the opcode header of previous opcode
 @param[in] TextTwo                   String ID for Text Two

 @retval NULL   There is not enough space left in Buffer to add the opcode.
 @retval Other  A pointer to the created opcode.
**/
UINT8 *
HiiCreateGuidTextOpCode (
  IN VOID                                  *OpCodeHandle,
  IN EFI_IFR_OP_HEADER                     *PreviousOpCodeHeader,
  IN EFI_STRING_ID                         TextTwo
  )
{
  H2O_IFR_GUID_TEXT                        *GuidText;

  GuidText = (H2O_IFR_GUID_TEXT *) HiiCreateGuidOpCode (
                                     OpCodeHandle,
                                     &gH2OIfrExtGuid,
                                     NULL,
                                     sizeof (H2O_IFR_GUID_TEXT)
                                     );
  if (GuidText == NULL) {
    return NULL;
  }

  GuidText->Function = H2O_IFR_EXT_TEXT;
  GuidText->Text     = TextTwo;

  if (PreviousOpCodeHeader->Scope == 0) {
    PreviousOpCodeHeader->Scope = 1;
    HiiCreateEndOpCode (OpCodeHandle);
  }

  return (UINT8 *) GuidText;
}

/**
 Create EFI_IFR_ACTION_OP opcode with text two.
 If string ID of text two is zero, it will create EFI_IFR_ACTION_OP opcode without text two.

 @param[in] OpCodeHandle    Handle to the buffer of opcodes.
 @param[in] QuestionId      Question ID
 @param[in] Prompt          String ID for Prompt
 @param[in] Help            String ID for Help
 @param[in] TextTwo         String ID for Text Two
 @param[in] QuestionFlags   Flags in Question Header
 @param[in] QuestionConfig  String ID for configuration

 @retval NULL   There is not enough space left in Buffer to add the opcode.
 @retval Other  A pointer to the created opcode.
**/
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
  )
{
  EFI_IFR_ACTION                           *Action;

  Action = (EFI_IFR_ACTION *) HiiCreateActionOpCode (
                                OpCodeHandle,
                                QuestionId,
                                Prompt,
                                Help,
                                QuestionFlags,
                                QuestionConfig
                                );
  if (Action == NULL || TextTwo == 0) {
    return (UINT8 *) Action;
  }

  HiiCreateGuidTextOpCode (OpCodeHandle, &Action->Header, TextTwo);

  return (UINT8 *) Action;
}

/**
 Create EFI_IFR_PASSWORD_OP opcode with text two.
 If string ID of text two is zero, it will create EFI_IFR_PASSWORD_OP opcode without text two.

 @param[in] OpCodeHandle          Handle to the buffer of opcodes.
 @param[in] QuestionId            Question ID
 @param[in] VarStoreId            Storage ID
 @param[in] VarOffset             Offset in Storage
 @param[in] Prompt                String ID for Prompt
 @param[in] Help                  String ID for Help
 @param[in] TextTwo               String ID for Text Two
 @param[in] QuestionFlags         Flags in Question Header
 @param[in] Minimum               Numeric minimum value
 @param[in] Maximum               Numeric maximum value

 @retval NULL   There is not enough space left in Buffer to add the opcode.
 @retval Other  A pointer to the created opcode.
**/
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
  IN UINT16                                MaxSize
  )
{
  EFI_IFR_PASSWORD                         Password;
  UINT8                                    *OpCodePtr;

  ZeroMem (&Password, sizeof (Password));
  Password.Header.OpCode                   = EFI_IFR_PASSWORD_OP;
  Password.Header.Length                   = sizeof (Password);
  Password.Question.QuestionId             = QuestionId;
  Password.Question.VarStoreId             = VarStoreId;
  Password.Question.VarStoreInfo.VarOffset = VarOffset;
  Password.Question.Header.Prompt          = Prompt;
  Password.Question.Header.Help            = Help;
  Password.Question.Flags                  = QuestionFlags;
  Password.MinSize                         = MinSize;
  Password.MaxSize                         = MaxSize;

  OpCodePtr = HiiCreateRawOpCodes (OpCodeHandle, (UINT8 *) &Password, sizeof (Password));
  if (OpCodePtr == NULL || TextTwo == 0) {
    return OpCodePtr;
  }

  HiiCreateGuidTextOpCode (OpCodeHandle, &((EFI_IFR_PASSWORD *) OpCodePtr)->Header, TextTwo);

  return OpCodePtr;
}

/**
 Create EFI_IFR_ONE_OF_OPTION_OP opcode with ext op.

 If OpCodeHandle is NULL, then ASSERT().
 If Type is invalid, then ASSERT().
 If Flags is invalid, then ASSERT().

 @param[in] OpCodeHandle  Handle to the buffer of opcodes.
 @param[in] StringId      StringId for the option
 @param[in] Flags         Flags for the option
 @param[in] Type          Type for the option
 @param[in] Value         Value for the option
 @param[in] ExtendedOpCodeHandle   Handle for a buffer of Extended opcodes.

 @retval NULL   There is not enough space left in Buffer to add the opcode.
 @retval Other  A pointer to the created opcode.
**/
UINT8 *
EFIAPI
HiiCreateOneOfOptionOpCodeWithExtOp (
  IN VOID    *OpCodeHandle,
  IN UINT16  StringId,
  IN UINT8   Flags,
  IN UINT8   Type,
  IN UINT64  Value,
  IN VOID    *ExtendedOpCodeHandle
  )
{
  EFI_IFR_ONE_OF_OPTION    OpCode;
  UINTN                    Position;

  ASSERT (ExtendedOpCodeHandle != NULL);
  ASSERT (Type < EFI_IFR_TYPE_OTHER);

  ZeroMem (&OpCode, sizeof (OpCode));
  OpCode.Option = StringId;
  OpCode.Flags  = (UINT8) (Flags & (EFI_IFR_OPTION_DEFAULT));
  OpCode.Type   = Type;
  CopyMem (&OpCode.Value, &Value, mHiiDefaultTypeToWidth[Type]);

  Position = InternalHiiOpCodeHandlePosition (OpCodeHandle);
  InternalHiiCreateOpCodeExtended (OpCodeHandle, &OpCode, EFI_IFR_ONE_OF_OPTION_OP, sizeof (OpCode), 0, 1);

  InternalHiiAppendOpCodes (OpCodeHandle, ExtendedOpCodeHandle);
  HiiCreateEndOpCode (OpCodeHandle);
  return InternalHiiOpCodeHandleBuffer (OpCodeHandle) + Position;
}

/**
 Create EFI_IFR_IMAGE_OP opcode.

 If OpCodeHandle is NULL, then ASSERT().

 @param[in] OpCodeHandle  Handle to the buffer of opcodes.
 @param[in] ImageId      Image ID for Image

 @retval NULL   There is not enough space left in Buffer to add the opcode.
 @retval Other  A pointer to the created opcode.
**/
UINT8 *
EFIAPI
HiiCreateImageOpCode (
  IN VOID           *OpCodeHandle,
  IN EFI_IMAGE_ID   ImageId
  )
{
  EFI_IFR_IMAGE     OpCode;

  ZeroMem (&OpCode, sizeof (OpCode));
  OpCode.Id = ImageId;

  return InternalHiiCreateOpCodeExtended (
           OpCodeHandle,
           &OpCode,
           EFI_IFR_IMAGE_OP,
           sizeof (OpCode),
           0,
           0
           );
}

