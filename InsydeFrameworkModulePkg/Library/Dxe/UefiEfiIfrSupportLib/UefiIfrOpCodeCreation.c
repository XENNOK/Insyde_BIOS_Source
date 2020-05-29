//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2007 - 2009, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  UefiIfrOpCodeCreation.c

Abstract:

  Library Routines to create IFR independent of string data - assume tokens already exist
  Primarily to be used for exporting op-codes at a label in pre-defined forms.

Revision History:

--*/

#include "UefiIfrLibrary.h"

EFI_GUID mIfrVendorGuid = EFI_IFR_TIANO_GUID;

STATIC
BOOLEAN
IsValidQuestionFlags (
  IN UINT8                   Flags
  )
{
  return (Flags & (~QUESTION_FLAGS)) ? FALSE : TRUE;
}

STATIC
BOOLEAN
IsValidActionQuestionFlags (
  IN UINT8                   Flags
  )
{
  return (Flags & ACTION_QUESTION_FLAGS_MASK) ? FALSE : TRUE;
}

STATIC
BOOLEAN
IsValidValueType (
  IN UINT8                   Type
  )
{
  return (Type <= EFI_IFR_TYPE_OTHER) ? TRUE : FALSE;
}

STATIC
BOOLEAN
IsValidNumricFlags (
  IN UINT8                   Flags
  )
{
  if (Flags & ~(EFI_IFR_NUMERIC_SIZE | EFI_IFR_DISPLAY)) {
    return FALSE;
  }

  if ((Flags & EFI_IFR_DISPLAY) > EFI_IFR_DISPLAY_UINT_HEX) {
    return FALSE;
  }

  return TRUE;
}

STATIC
BOOLEAN
IsValidCheckboxFlags (
  IN UINT8                   Flags
  )
{
  return (Flags <= EFI_IFR_CHECKBOX_DEFAULT_MFG) ? TRUE : FALSE;
}

EFI_STATUS
CreateSuppressIfCode (
  IN OUT EFI_HII_UPDATE_DATA *Data
  )
{
  EFI_IFR_SUPPRESS_IF         SuppressIf;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);
  
  if (Data->Offset + sizeof (EFI_IFR_SUPPRESS_IF) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }
  SuppressIf.Header.OpCode = EFI_IFR_SUPPRESS_IF_OP;
  SuppressIf.Header.Length = 2;
  SuppressIf.Header.Scope  = 1;
  
  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &SuppressIf, sizeof (EFI_IFR_SUPPRESS_IF));
  Data->Offset += sizeof (EFI_IFR_SUPPRESS_IF);
  
  return EFI_SUCCESS;
}

EFI_STATUS
CreateIdEqualCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINT16               Value,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_EQ_ID_VAL         IdEqual;
  UINT8                     *LocalBuffer;
  
  ASSERT (Data != NULL && Data->Data != NULL);
  
  if (Data->Offset + sizeof (EFI_IFR_EQ_ID_VAL) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }
  
  IdEqual.Header.OpCode = EFI_IFR_EQ_ID_VAL_OP;
  IdEqual.Header.Length = 6;
  IdEqual.Header.Scope = 0;
  IdEqual.QuestionId = QuestionId;
  IdEqual.Value = Value;
  
  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &IdEqual, sizeof (EFI_IFR_EQ_ID_VAL));
  Data->Offset += sizeof (EFI_IFR_EQ_ID_VAL);
  
  return EFI_SUCCESS;
}

EFI_STATUS
CreateIdEqualListCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINT8                OptionNum,
  IN     UINT16               *Value,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_EQ_ID_LIST        *IdEqualList;
  UINT8                     *LocalBuffer;
  UINT8                     Index;
  
  ASSERT (Data != NULL && Data->Data != NULL);
  
  if (Data->Offset + sizeof (EFI_IFR_EQ_ID_LIST) + sizeof (UINT16)*(OptionNum-1) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }
  
  gBS->AllocatePool (EfiRuntimeServicesData, sizeof (EFI_IFR_EQ_ID_LIST) + sizeof (UINT16)*(OptionNum-1), &IdEqualList);
  
  IdEqualList->Header.OpCode = EFI_IFR_EQ_ID_LIST_OP;
  IdEqualList->Header.Length = 6 + OptionNum*2;
  IdEqualList->Header.Scope = 0;
  IdEqualList->QuestionId = QuestionId;
  IdEqualList->ListLength = OptionNum;
  
  for (Index = 0; Index < OptionNum; Index++) {
    IdEqualList->ValueList[Index] = Value[Index];  
  }

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, IdEqualList, sizeof (EFI_IFR_EQ_ID_LIST) + sizeof (UINT16)*(OptionNum-1));
  Data->Offset = Data->Offset + sizeof (EFI_IFR_EQ_ID_LIST) + sizeof (UINT16)*(OptionNum-1);
  
  gBS->FreePool (IdEqualList);
  
  return EFI_SUCCESS;
}

EFI_STATUS
CreateOneOfOptionOpCodeEx (
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     UINT8                Type,
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINTN                OptionCount2,
  IN     UINT8                *PrimaryTypeList,  
  IN     UINT8                *MapList,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  UINTN                       Index;
  UINT8                       *LocalBuffer;
  EFI_IFR_ONE_OF_OPTION       OneOfOption;
  UINT8                       Index2;
  UINT8                       OptionNum;
  UINT8                       TypeIndex;
  UINT16                      *Value;   
                    
  ASSERT (Data != NULL && Data->Data != NULL);
  
  if ((OptionCount != 0) && (OptionsList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (Data->Offset + OptionCount * sizeof (EFI_IFR_ONE_OF_OPTION) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }
  
  for (Index = 0; Index < OptionCount; Index++) {
    TypeIndex = 0;
    for (Index2 = 0; Index2 < OptionCount; Index2++) {
      if (MapList[Index2*2] == OptionsList[Index].Value.u8) {
        TypeIndex = Index2;
        break;
      }
    }
    
    OptionNum = 0;
    for (Index2 = 0; Index2 < OptionCount2; Index2++) {
      if ((MapList[TypeIndex*2+1] & PrimaryTypeList[Index2]) != 0) {
        OptionNum++; 
      }
    }
    
    if (OptionNum > 0) {
      CreateSuppressIfCode (Data);
      gBS->AllocatePool (EfiRuntimeServicesData, OptionNum, &Value);
      OptionNum = 0;
      for (Index2 = 0; Index2 < OptionCount2; Index2++) {
        //
        // If this option need to be suppressed
        //
        if ((MapList[TypeIndex*2+1] & PrimaryTypeList[Index2]) != 0) {
          //
          // Gudge if this suppressed option has many conditions
          //
          Value[OptionNum] = PrimaryTypeList[Index2];
          OptionNum++; 
        }
      }
      if (OptionNum > 1) {
        CreateIdEqualListCode (QuestionId, OptionNum, Value, Data);
      } else {
        CreateIdEqualCode (QuestionId, Value[0], Data);
      }
      gBS->FreePool (Value);
    }
    
    OneOfOption.Header.OpCode = EFI_IFR_ONE_OF_OPTION_OP;
    OneOfOption.Header.Length = sizeof (EFI_IFR_ONE_OF_OPTION);
    OneOfOption.Header.Scope  = 0;
    OneOfOption.Option        = OptionsList[Index].StringToken;
    OneOfOption.Value         = OptionsList[Index].Value;
    OneOfOption.Flags         = OptionsList[Index].Flags & (EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    OneOfOption.Type          = Type;
    
    LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
    EfiCopyMem (LocalBuffer, &OneOfOption, sizeof (EFI_IFR_ONE_OF_OPTION));
    Data->Offset += sizeof (EFI_IFR_ONE_OF_OPTION);
    
    if (OptionNum > 0) {
  	  //
  	  // This end opcode is for suppressif
  	  //	
      CreateEndOpCode (Data);
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
CreateOneOfOpCodeEx (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_VARSTORE_ID      VarStoreId,
  IN     UINT16               VarOffset,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     UINT8                OneOfFlags,
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     EFI_QUESTION_ID      QuestionId2,
  IN     UINTN                OptionCount2, 
  IN     UINT8                *PrimaryTypeList, 
  IN     UINT8                *MapList,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  UINTN                       Length;
  EFI_IFR_ONE_OF              OneOf;
  UINT8                       *LocalBuffer;
  
  ASSERT (Data != NULL && Data->Data != NULL);
  
  if (!IsValidNumricFlags (OneOfFlags) ||
      !IsValidQuestionFlags (QuestionFlags) ||
      ((OptionCount != 0) && (OptionsList == NULL))) {
    return EFI_INVALID_PARAMETER;
  }
  
  Length = sizeof (EFI_IFR_ONE_OF) + OptionCount * sizeof (EFI_IFR_ONE_OF_OPTION) + sizeof (EFI_IFR_END);
  
  if (Data->Offset + Length > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }
  
  OneOf.Header.OpCode                   = EFI_IFR_ONE_OF_OP;
  OneOf.Header.Length                   = sizeof (EFI_IFR_ONE_OF);
  OneOf.Header.Scope                    = 1;
  OneOf.Question.Header.Prompt          = Prompt;
  OneOf.Question.Header.Help            = Help;
  OneOf.Question.QuestionId             = QuestionId;
  OneOf.Question.VarStoreId             = VarStoreId;
  OneOf.Question.VarStoreInfo.VarOffset = VarOffset;
  OneOf.Question.Flags                  = QuestionFlags;
  OneOf.Flags                           = OneOfFlags;
  
  EfiZeroMem ((VOID *) &OneOf.data, sizeof (MINMAXSTEP_DATA));
  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &OneOf, sizeof (EFI_IFR_ONE_OF));
  Data->Offset += sizeof (EFI_IFR_ONE_OF);
  
  CreateOneOfOptionOpCodeEx (
     OptionCount, 
     OptionsList, 
     (OneOfFlags & EFI_IFR_NUMERIC_SIZE),  
     QuestionId2,
     OptionCount2,
     PrimaryTypeList, 
     MapList,  
     Data
     );
     
  CreateEndOpCode (Data);
  
  return EFI_SUCCESS;
}

EFI_STATUS
CreateEndOpCode (
  IN OUT EFI_HII_UPDATE_DATA *Data
  )
{
  EFI_IFR_END                 End;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (Data->Offset + sizeof (EFI_IFR_END) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  End.Header.Length  = sizeof (EFI_IFR_END);
  End.Header.OpCode  = EFI_IFR_END_OP;
  End.Header.Scope   = 0;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &End, sizeof (EFI_IFR_END));
  Data->Offset += sizeof (EFI_IFR_END);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateDefaultOpCode (
  IN     EFI_IFR_TYPE_VALUE  *Value,
  IN     UINT8               Type,
  IN OUT EFI_HII_UPDATE_DATA *Data
  )
{
  EFI_IFR_DEFAULT            Default;
  UINT8                      *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if ((Value == NULL) || !IsValidValueType (Type)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + sizeof (EFI_IFR_DEFAULT) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Default.Header.OpCode = EFI_IFR_DEFAULT_OP;
  Default.Header.Length = sizeof (EFI_IFR_DEFAULT);
  Default.Header.Scope  = 0;
  Default.Type          = Type;
  Default.DefaultId     = EFI_HII_DEFAULT_CLASS_STANDARD;
  EfiCopyMem (&Default.Value, Value, sizeof(EFI_IFR_TYPE_VALUE));

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Default, sizeof (EFI_IFR_DEFAULT));
  Data->Offset += sizeof (EFI_IFR_DEFAULT);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateActionOpCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     EFI_STRING_ID        QuestionConfig,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_ACTION              Action;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidActionQuestionFlags (QuestionFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + sizeof (EFI_IFR_ACTION) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Action.Header.OpCode          = EFI_IFR_ACTION_OP;
  Action.Header.Length          = sizeof (EFI_IFR_ACTION);
  Action.Header.Scope           = 0;
  Action.Question.QuestionId    = QuestionId;
  Action.Question.Header.Prompt = Prompt;
  Action.Question.Header.Help   = Help;
  Action.Question.VarStoreId    = INVALID_VARSTORE_ID;
  Action.Question.Flags         = QuestionFlags;
  Action.QuestionConfig         = QuestionConfig;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Action, sizeof (EFI_IFR_ACTION));
  Data->Offset += sizeof (EFI_IFR_ACTION);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateSubTitleOpCode (
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               Flags,
  IN      UINT8               Scope,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_SUBTITLE            Subtitle;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (Data->Offset + sizeof (EFI_IFR_SUBTITLE) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Subtitle.Header.OpCode    = EFI_IFR_SUBTITLE_OP;
  Subtitle.Header.Length    = sizeof (EFI_IFR_SUBTITLE);
  Subtitle.Header.Scope     = Scope;
  Subtitle.Statement.Prompt = Prompt;
  Subtitle.Statement.Help   = Help;
  Subtitle.Flags            = Flags;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Subtitle, sizeof (EFI_IFR_SUBTITLE));
  Data->Offset += sizeof (EFI_IFR_SUBTITLE);

  return EFI_SUCCESS;
}


EFI_STATUS
CreateTextOpCode (
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     EFI_STRING_ID        TextTwo,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_TEXT                Text;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (Data->Offset + sizeof (EFI_IFR_TEXT) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Text.Header.OpCode    = EFI_IFR_TEXT_OP;
  Text.Header.Length    = sizeof (EFI_IFR_TEXT);
  Text.Header.Scope     = 0;
  Text.Statement.Prompt = Prompt;
  Text.Statement.Help   = Help;
  Text.TextTwo          = TextTwo;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Text, sizeof (EFI_IFR_TEXT));
  Data->Offset += sizeof (EFI_IFR_TEXT);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateGotoOpCode (
  IN     EFI_FORM_ID          FormId,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     EFI_QUESTION_ID      QuestionId,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_REF                 Goto;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidQuestionFlags (QuestionFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + sizeof (EFI_IFR_REF) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Goto.Header.OpCode          = EFI_IFR_REF_OP;
  Goto.Header.Length          = sizeof (EFI_IFR_REF);
  Goto.Header.Scope           = 0;
  Goto.Question.Header.Prompt = Prompt;
  Goto.Question.Header.Help   = Help;
  Goto.Question.VarStoreId    = INVALID_VARSTORE_ID;
  Goto.Question.QuestionId    = QuestionId;
  Goto.Question.Flags         = QuestionFlags;
  Goto.FormId                 = FormId;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Goto, sizeof (EFI_IFR_REF));
  Data->Offset += sizeof (EFI_IFR_REF);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateOneOfOptionOpCode (
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     UINT8                Type,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  UINTN                       Index;
  UINT8                       *LocalBuffer;
  EFI_IFR_ONE_OF_OPTION       OneOfOption;

  ASSERT (Data != NULL && Data->Data != NULL);

  if ((OptionCount != 0) && (OptionsList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + OptionCount * sizeof (EFI_IFR_ONE_OF_OPTION) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  for (Index = 0; Index < OptionCount; Index++) {
    OneOfOption.Header.OpCode = EFI_IFR_ONE_OF_OPTION_OP;
    OneOfOption.Header.Length = sizeof (EFI_IFR_ONE_OF_OPTION);
    OneOfOption.Header.Scope  = 0;

    OneOfOption.Option        = OptionsList[Index].StringToken;
    OneOfOption.Value         = OptionsList[Index].Value;
    OneOfOption.Flags         = OptionsList[Index].Flags & (EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    OneOfOption.Type          = Type;

    LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
    EfiCopyMem (LocalBuffer, &OneOfOption, sizeof (EFI_IFR_ONE_OF_OPTION));
    Data->Offset += sizeof (EFI_IFR_ONE_OF_OPTION);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CreateOneOfOpCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_VARSTORE_ID      VarStoreId,
  IN     UINT16               VarOffset,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     UINT8                OneOfFlags,
  IN     IFR_OPTION           *OptionsList,
  IN     UINTN                OptionCount,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  UINTN                       Length;
  EFI_IFR_ONE_OF              OneOf;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidNumricFlags (OneOfFlags) ||
      !IsValidQuestionFlags (QuestionFlags) ||
      ((OptionCount != 0) && (OptionsList == NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  Length = sizeof (EFI_IFR_ONE_OF) + OptionCount * sizeof (EFI_IFR_ONE_OF_OPTION) + sizeof (EFI_IFR_END);
  if (Data->Offset + Length > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  OneOf.Header.OpCode                   = EFI_IFR_ONE_OF_OP;
  OneOf.Header.Length                   = sizeof (EFI_IFR_ONE_OF);
  OneOf.Header.Scope                    = 1;
  OneOf.Question.Header.Prompt          = Prompt;
  OneOf.Question.Header.Help            = Help;
  OneOf.Question.QuestionId             = QuestionId;
  OneOf.Question.VarStoreId             = VarStoreId;
  OneOf.Question.VarStoreInfo.VarOffset = VarOffset;
  OneOf.Question.Flags                  = QuestionFlags;
  OneOf.Flags                           = OneOfFlags;
  EfiZeroMem ((VOID *) &OneOf.data, sizeof (MINMAXSTEP_DATA));

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &OneOf, sizeof (EFI_IFR_ONE_OF));
  Data->Offset += sizeof (EFI_IFR_ONE_OF);

  CreateOneOfOptionOpCode (OptionCount, OptionsList, (OneOfFlags & EFI_IFR_NUMERIC_SIZE), Data);

  CreateEndOpCode (Data);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateOrderedListOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               OrderedListFlags,
  IN      UINT8               DataType,
  IN      UINT8               MaxContainers,
  IN      IFR_OPTION          *OptionsList,
  IN     UINTN                OptionCount,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  UINTN                       Length;
  EFI_IFR_ORDERED_LIST        OrderedList;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidQuestionFlags (QuestionFlags) ||
      ((OptionCount != 0) && (OptionsList == NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  if ((OrderedListFlags & (~(EFI_IFR_UNIQUE_SET | EFI_IFR_NO_EMPTY_SET))) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  Length = sizeof (EFI_IFR_ORDERED_LIST) + OptionCount * sizeof (EFI_IFR_ONE_OF_OPTION) + sizeof (EFI_IFR_END);
  if (Data->Offset + Length > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  OrderedList.Header.OpCode                   = EFI_IFR_ORDERED_LIST_OP;
  OrderedList.Header.Length                   = sizeof (EFI_IFR_ORDERED_LIST);
  OrderedList.Header.Scope                    = 1;
  OrderedList.Question.Header.Prompt          = Prompt;
  OrderedList.Question.Header.Help            = Help;
  OrderedList.Question.QuestionId             = QuestionId;
  OrderedList.Question.VarStoreId             = VarStoreId;
  OrderedList.Question.VarStoreInfo.VarOffset = VarOffset;
  OrderedList.Question.Flags                  = QuestionFlags;
  OrderedList.MaxContainers                   = MaxContainers;
  OrderedList.Flags                           = OrderedListFlags;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &OrderedList, sizeof (EFI_IFR_ORDERED_LIST));
  Data->Offset += sizeof (EFI_IFR_ORDERED_LIST);

  CreateOneOfOptionOpCode (OptionCount, OptionsList, DataType, Data);

  CreateEndOpCode (Data);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateCheckBoxOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               CheckBoxFlags,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_CHECKBOX            CheckBox;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidQuestionFlags (QuestionFlags) || !IsValidCheckboxFlags (CheckBoxFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + sizeof (EFI_IFR_CHECKBOX) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  CheckBox.Header.OpCode                   = EFI_IFR_CHECKBOX_OP;
  CheckBox.Header.Length                   = sizeof (EFI_IFR_CHECKBOX);
  CheckBox.Header.Scope                    = 0;
  CheckBox.Question.QuestionId             = QuestionId;
  CheckBox.Question.VarStoreId             = VarStoreId;
  CheckBox.Question.VarStoreInfo.VarOffset = VarOffset;
  CheckBox.Question.Header.Prompt          = Prompt;
  CheckBox.Question.Header.Help            = Help;
  CheckBox.Question.Flags                  = QuestionFlags;
  CheckBox.Flags                           = CheckBoxFlags;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &CheckBox, sizeof (EFI_IFR_CHECKBOX));
  Data->Offset += sizeof (EFI_IFR_CHECKBOX);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateNumericOpCode (
  IN     EFI_QUESTION_ID     QuestionId,
  IN     EFI_VARSTORE_ID     VarStoreId,
  IN     UINT16              VarOffset,
  IN     EFI_STRING_ID       Prompt,
  IN     EFI_STRING_ID       Help,
  IN     UINT8               QuestionFlags,
  IN     UINT8               NumericFlags,
  IN     UINT64              Minimum,
  IN     UINT64              Maximum,
  IN     UINT64              Step,
  IN     UINT64              Default,
  IN OUT EFI_HII_UPDATE_DATA *Data
  )
{
  UINTN                       Length;
  EFI_STATUS                  Status;
  EFI_IFR_NUMERIC             Numeric;
  MINMAXSTEP_DATA             MinMaxStep;
  EFI_IFR_TYPE_VALUE          DefaultValue;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidQuestionFlags (QuestionFlags) || !IsValidNumricFlags (NumericFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  Length = sizeof (EFI_IFR_NUMERIC) + sizeof (EFI_IFR_DEFAULT) + sizeof (EFI_IFR_END);
  if (Data->Offset + sizeof (EFI_IFR_CHECKBOX) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Numeric.Header.OpCode                   = EFI_IFR_NUMERIC_OP;
  Numeric.Header.Length                   = sizeof (EFI_IFR_NUMERIC);
  Numeric.Header.Scope                    = 1;
  Numeric.Question.QuestionId             = QuestionId;
  Numeric.Question.VarStoreId             = VarStoreId;
  Numeric.Question.VarStoreInfo.VarOffset = VarOffset;
  Numeric.Question.Header.Prompt          = Prompt;
  Numeric.Question.Header.Help            = Help;
  Numeric.Question.Flags                  = QuestionFlags;
  Numeric.Flags                           = NumericFlags;

  switch (NumericFlags & EFI_IFR_NUMERIC_SIZE) {
  case EFI_IFR_NUMERIC_SIZE_1:
    MinMaxStep.u8.MinValue = (UINT8) Minimum;
    MinMaxStep.u8.MaxValue = (UINT8) Maximum;
    MinMaxStep.u8.Step     = (UINT8) Step;
    break;

  case EFI_IFR_NUMERIC_SIZE_2:
    MinMaxStep.u16.MinValue = (UINT16) Minimum;
    MinMaxStep.u16.MaxValue = (UINT16) Maximum;
    MinMaxStep.u16.Step     = (UINT16) Step;
    break;

  case EFI_IFR_NUMERIC_SIZE_4:
    MinMaxStep.u32.MinValue = (UINT32) Minimum;
    MinMaxStep.u32.MaxValue = (UINT32) Maximum;
    MinMaxStep.u32.Step     = (UINT32) Step;
    break;

  case EFI_IFR_NUMERIC_SIZE_8:
    MinMaxStep.u64.MinValue = Minimum;
    MinMaxStep.u64.MaxValue = Maximum;
    MinMaxStep.u64.Step     = Step;
    break;
  }

  EfiCopyMem (&Numeric.data, &MinMaxStep, sizeof (MINMAXSTEP_DATA));

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Numeric, sizeof (EFI_IFR_NUMERIC));
  Data->Offset += sizeof (EFI_IFR_NUMERIC);

  DefaultValue.u64 = Default;
  Status = CreateDefaultOpCode (&DefaultValue, (NumericFlags & EFI_IFR_NUMERIC_SIZE), Data);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  CreateEndOpCode (Data);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateStringOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               StringFlags,
  IN      UINT8               MinSize,
  IN      UINT8               MaxSize,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_STRING              String;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidQuestionFlags (QuestionFlags) || (StringFlags & (~EFI_IFR_STRING_MULTI_LINE))) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Offset + sizeof (EFI_IFR_STRING) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  String.Header.OpCode                   = EFI_IFR_STRING_OP;
  String.Header.Length                   = sizeof (EFI_IFR_STRING);
  String.Header.Scope                    = 0;
  String.Question.Header.Prompt          = Prompt;
  String.Question.Header.Help            = Help;
  String.Question.QuestionId             = QuestionId;
  String.Question.VarStoreId             = VarStoreId;
  String.Question.VarStoreInfo.VarOffset = VarOffset;
  String.Question.Flags                  = QuestionFlags;
  String.MinSize                         = MinSize;
  String.MaxSize                         = MaxSize;
  String.Flags                           = StringFlags;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &String, sizeof (EFI_IFR_STRING));
  Data->Offset += sizeof (EFI_IFR_STRING);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateBannerOpCode (
  IN      EFI_STRING_ID       Title,
  IN      UINT16              LineNumber,
  IN      UINT8               Alignment,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
{
  EFI_IFR_GUID_BANNER         Banner;
  UINT8                       *LocalBuffer;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (Data->Offset + sizeof (EFI_IFR_GUID_BANNER) > Data->BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Banner.Header.OpCode  = EFI_IFR_GUID_OP;
  Banner.Header.Length  = sizeof (EFI_IFR_GUID_BANNER);
  Banner.Header.Scope   = 0;
  EfiCopyMem (&Banner.Guid, &mIfrVendorGuid, sizeof (EFI_IFR_GUID));
  Banner.ExtendOpCode   = EFI_IFR_EXTEND_OP_BANNER;
  Banner.Title          = Title;
  Banner.LineNumber     = LineNumber;
  Banner.Alignment      = Alignment;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Banner, sizeof (EFI_IFR_GUID_BANNER));
  Data->Offset += sizeof (EFI_IFR_GUID_BANNER);

  return EFI_SUCCESS;
}

EFI_STATUS
CreateGuidTextOpCode (
  IN     EFI_IFR_OP_HEADER                 *PreviousOpCodeHeader,
  IN     EFI_STRING_ID                     Text,
  IN OUT EFI_HII_UPDATE_DATA               *Data
  )
/*++

Routine Description:

  Create H2O_IFR_GUID_TEXT opcode.
  If the scope of PreviousOpCodeHeader is zero, it will be set 1 and append end opcode.

Arguments:

  PreviousOpCodeHeader      - Opcode header of previous opcode
  Text                      - String ID for Text
  Data                      - Destination for the created opcode binary

Returns:

  EFI_SUCCESS            - Opcode create success
  EFI_BUFFER_TOO_SMALL   - The size of Data is not enough

--*/
{
  H2O_IFR_GUID_TEXT                        ExtGuidText;
  UINT8                                    *LocalBuffer;

  if ((PreviousOpCodeHeader->Scope == 0 && Data->Offset + sizeof (H2O_IFR_GUID_TEXT) > Data->BufferSize) ||
      (PreviousOpCodeHeader->Scope == 1 && Data->Offset + sizeof (H2O_IFR_GUID_TEXT) + sizeof (EFI_IFR_END) > Data->BufferSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  EfiZeroMem (&ExtGuidText, sizeof (H2O_IFR_GUID_TEXT));

  ExtGuidText.Header.OpCode = EFI_IFR_GUID_OP;
  ExtGuidText.Header.Length = sizeof (H2O_IFR_GUID_TEXT);
  ExtGuidText.Function      = H2O_IFR_EXT_TEXT;
  ExtGuidText.Text          = Text;
  EfiCopyMem (&ExtGuidText.Guid, &gH2OIfrExtGuid, sizeof (EFI_GUID));

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &ExtGuidText, sizeof (H2O_IFR_GUID_TEXT));
  Data->Offset += sizeof (H2O_IFR_GUID_TEXT);

  if (PreviousOpCodeHeader->Scope == 0) {
    PreviousOpCodeHeader->Scope = 1;
    CreateEndOpCode (Data);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CreateActionOpCodeEx (
  IN     EFI_QUESTION_ID                   QuestionId,
  IN     EFI_STRING_ID                     Prompt,
  IN     EFI_STRING_ID                     Help,
  IN     EFI_STRING_ID                     TextTwo,
  IN     UINT8                             QuestionFlags,
  IN     EFI_STRING_ID                     QuestionConfig,
  IN OUT EFI_HII_UPDATE_DATA               *Data
  )
/*++

Routine Description:

  Create EFI_IFR_ACTION opcode.
  If TextTwo is not zero, it will create extend GUID text opcode for text two string.

Arguments:

  QuestionId      - Question ID
  Prompt          - String ID for Prompt
  Help            - String ID for Help
  TextTwo         - String ID for TextTwo
  QuestionFlags   - Flags in Question Header
  QuestionConfig  - String ID for configuration
  Data            - Destination for the created opcode binary

Returns:

  EFI_SUCCESS            - Opcode create success
  EFI_INVALID_PARAMETER  - Invalid question flag
  EFI_BUFFER_TOO_SMALL   - The size of Data is not enough

--*/
{
  EFI_IFR_ACTION                           *Action;
  EFI_STATUS                               Status;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidActionQuestionFlags (QuestionFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((TextTwo == 0 && Data->Offset + sizeof (EFI_IFR_ACTION) > Data->BufferSize) ||
      (TextTwo != 0 && Data->Offset + sizeof (EFI_IFR_ACTION) + sizeof (H2O_IFR_GUID_TEXT) + sizeof (EFI_IFR_END) > Data->BufferSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Action = (EFI_IFR_ACTION *) (Data->Data + Data->Offset);

  Status = CreateActionOpCode (
             QuestionId,
             Prompt,
             Help,
             QuestionFlags,
             QuestionConfig,
             Data
             );
  if (EFI_ERROR (Status) || TextTwo == 0) {
    return Status;
  }

  Status = CreateGuidTextOpCode (&Action->Header, TextTwo, Data);

  return Status;
}

EFI_STATUS
CreatePasswordOpCode (
  IN     EFI_QUESTION_ID                   QuestionId,
  IN     EFI_VARSTORE_ID                   VarStoreId,
  IN     UINT16                            VarOffset,
  IN     EFI_STRING_ID                     Prompt,
  IN     EFI_STRING_ID                     Help,
  IN     EFI_STRING_ID                     TextTwo,
  IN     UINT8                             QuestionFlags,
  IN     UINT16                            MinSize,
  IN     UINT16                            MaxSize,
  IN OUT EFI_HII_UPDATE_DATA               *Data
  )
/*++

Routine Description:

  Create EFI_IFR_PASSWORD opcode.
  If TextTwo is not zero, it will create extend GUID text opcode for text two string.

Arguments:

  QuestionId      - Question ID
  VarStoreId      - Storage ID
  VarOffset       - Offset in Storage
  Prompt          - String ID for Prompt
  Help            - String ID for Help
  TextTwo         - String ID for TextTwo
  QuestionFlags   - Flags in Question Header
  StringFlags     - Flags for string opcode
  MinSize         - String minimum length
  MaxSize         - String maximum length
  Data            - Destination for the created opcode binary

Returns:

  EFI_SUCCESS            - Opcode create success
  EFI_INVALID_PARAMETER  - Invalid question flag
  EFI_BUFFER_TOO_SMALL   - The size of Data is not enough

--*/
{
  EFI_IFR_PASSWORD                         Password;
  UINT8                                    *LocalBuffer;
  EFI_STATUS                               Status;

  ASSERT (Data != NULL && Data->Data != NULL);

  if (!IsValidActionQuestionFlags (QuestionFlags)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((TextTwo == 0 && Data->Offset + sizeof (EFI_IFR_PASSWORD) > Data->BufferSize) ||
      (TextTwo != 0 && Data->Offset + sizeof (EFI_IFR_PASSWORD) + sizeof (H2O_IFR_GUID_TEXT) + sizeof (EFI_IFR_END) > Data->BufferSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Password.Header.OpCode                   = EFI_IFR_PASSWORD_OP;
  Password.Header.Length                   = sizeof (EFI_IFR_PASSWORD);
  Password.Header.Scope                    = 0;
  Password.Question.QuestionId             = QuestionId;
  Password.Question.Header.Prompt          = Prompt;
  Password.Question.Header.Help            = Help;
  Password.Question.VarStoreId             = VarStoreId;
  Password.Question.VarStoreInfo.VarOffset = VarOffset;
  Password.Question.Flags                  = QuestionFlags;
  Password.MinSize                         = MinSize;
  Password.MaxSize                         = MaxSize;

  LocalBuffer = (UINT8 *) Data->Data + Data->Offset;
  EfiCopyMem (LocalBuffer, &Password, sizeof (EFI_IFR_PASSWORD));
  Data->Offset += sizeof (EFI_IFR_PASSWORD);

  if (TextTwo == 0) {
    return EFI_SUCCESS;
  }

  Status = CreateGuidTextOpCode (&((EFI_IFR_PASSWORD *) LocalBuffer)->Header, TextTwo, Data);

  return Status;
}
