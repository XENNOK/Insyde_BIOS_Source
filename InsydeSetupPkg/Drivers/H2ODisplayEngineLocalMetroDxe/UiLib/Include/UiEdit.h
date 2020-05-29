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

#ifndef _UI_EDIT_H
#define _UI_EDIT_H

typedef struct _UI_EDIT UI_EDIT;
typedef struct _UI_EDIT_CLASS UI_EDIT_CLASS;

typedef enum {
  NOT_VALUE,
  DEC_VALUE,
  HEX_VALUE,
} VALUE_TYPE;

UI_EDIT_CLASS *
EFIAPI
GetEditClass (
  VOID
  );

struct _UI_EDIT {
  UI_LABEL                      Label;

//  INT32    CaretX;

  UINT32                        EditPos;

  UINTN                         Flags;
  UINT32                        CharHeight;

  VALUE_TYPE                    ValueType;

  UINT64                        MaxValue;
  UINT64                        MinValue;
  UINT64                        Step;

  UINT32                        MaxLength;

  BOOLEAN                       IsPasswordMode;
  BOOLEAN                       IsReadOnly;

};

struct _UI_EDIT_CLASS {
  UI_LABEL_CLASS                ParentClass;
};

#endif
