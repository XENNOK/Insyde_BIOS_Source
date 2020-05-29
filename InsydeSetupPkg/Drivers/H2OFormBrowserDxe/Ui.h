/** @file
Private structure, MACRO and function definitions for User Interface related functions.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _UI_H_
#define _UI_H_

#define UI_ACTION_NONE               0
#define UI_ACTION_REFRESH_FORM       1
#define UI_ACTION_REFRESH_FORMSET    2
#define UI_ACTION_EXIT               3

typedef struct _UI_MENU_LIST UI_MENU_LIST;

typedef struct _UI_MENU_SELECTION {
  EFI_HII_HANDLE  Handle;

  //
  // Target FormSet/Form/Question information
  //
  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;
  UINTN           Sequence;  // used for time/date only.

  UINT32           Action;               ///< Action for Browser to taken:
                                         ///<   UI_ACTION_NONE            - navigation inside a Form
                                         ///<   UI_ACTION_REFRESH_FORM    - re-evaluate expressions and repaint Form
                                         ///<   UI_ACTION_REFRESH_FORMSET - re-parse FormSet IFR binary  

  FORM_BROWSER_FORMSET    *FormSet;      ///< Current selected FomSet/Form/Question
  FORM_BROWSER_FORM       *Form;         ///< 
  FORM_BROWSER_STATEMENT  *Statement;    ///<

  BOOLEAN                 FormEditable;  ///< Whether the Form is editable

  UI_MENU_LIST            *CurrentMenu;
} UI_MENU_SELECTION;

#define UI_MENU_OPTION_SIGNATURE  SIGNATURE_32 ('u', 'i', 'm', 'm')
#define UI_MENU_LIST_SIGNATURE    SIGNATURE_32 ('u', 'i', 'm', 'l')

typedef struct {
  UINTN                   Signature;
  LIST_ENTRY              Link;

  EFI_HII_HANDLE          Handle;
  FORM_BROWSER_STATEMENT  *ThisTag;
  UINT16                  EntryNumber;

  UINTN                   Row;
  UINTN                   Col;
  UINTN                   OptCol;
  CHAR16                  *Description;
  UINTN                   Skip;           // Number of lines

  //
  // Display item sequence for date/time
  //  Date:      Month/Day/Year
  //  Sequence:  0     1   2
  //
  //  Time:      Hour : Minute : Second
  //  Sequence:  0      1        2
  //
  //
  UINTN                   Sequence;

  BOOLEAN                 GrayOut;
  BOOLEAN                 ReadOnly;

  //
  // Whether user could change value of this item
  //
  BOOLEAN                 IsQuestion;
} UI_MENU_OPTION;

#define MENU_OPTION_FROM_LINK(a)  CR (a, UI_MENU_OPTION, Link, UI_MENU_OPTION_SIGNATURE)

struct _UI_MENU_LIST {
  UINTN           Signature;
  LIST_ENTRY      Link;

  EFI_HII_HANDLE  HiiHandle;
  EFI_GUID        FormSetGuid;
  EFI_FORM_ID     FormId;
  EFI_QUESTION_ID QuestionId;
  UINTN           Sequence;    // used for time/date only.

  UI_MENU_LIST    *Parent;
  LIST_ENTRY      ChildListHead;
};

#define UI_MENU_LIST_FROM_LINK(a)  CR (a, UI_MENU_LIST, Link, UI_MENU_LIST_SIGNATURE)

typedef struct _MENU_REFRESH_ENTRY MENU_REFRESH_ENTRY;
struct _MENU_REFRESH_ENTRY {
  MENU_REFRESH_ENTRY          *Next;
  UI_MENU_OPTION              *MenuOption;  // Describes the entry needing an update
  UI_MENU_SELECTION           *Selection;
  UINTN                       CurrentColumn;
  UINTN                       CurrentRow;
  UINTN                       CurrentAttribute;
  EFI_EVENT                   Event;
};

extern LIST_ENTRY          mMenuOption;               // Global variable for menu option
extern LIST_ENTRY          mMenuList;                 // Global variable for menu list
extern MENU_REFRESH_ENTRY  *mMenuRefreshHead;         // Global variable for menu refresh list head
extern UI_MENU_SELECTION   *mCurrentSelection;        // Global variable for current selection
extern BOOLEAN             mHiiPackageListUpdated;    // Global variable for menu option

//
// Global Functions
//


/**
  Create a menu with specified FormSet GUID and Form ID, and add it as a child
  of the given parent menu.

  @param  Parent                 The parent of menu to be added.
  @param  HiiHandle              HII handle related to this FormSet.
  @param  FormSetGuid            The FormSet Guid of menu to be added.
  @param  FormId                 The Form ID of menu to be added.

  @return A pointer to the newly added menu or NULL if memory is insufficient.

**/
UI_MENU_LIST *
UiAddMenuList (
  IN OUT UI_MENU_LIST     *Parent,
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

/**
  Search Menu with given FormId, FormSetGuid and Handle in all cached menu list.

  @param  Parent                 The parent of menu to search.
  @param  Handle                 HII handle related to this FormSet.
  @param  FormSetGuid            The FormSet GUID of the menu to search.
  @param  FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
UI_MENU_LIST *
UiFindChildMenuList (
  IN UI_MENU_LIST         *Parent,
  IN EFI_HII_HANDLE       Handle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

/**
  Search Menu with given Handle, FormSetGuid and FormId in all cached menu list.

  @param  FormSetGuid            The FormSet GUID of the menu to search.
  @param  Handle                 HII handle related to this FormSet.
  @param  FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
UI_MENU_LIST *
UiFindMenuList (
  IN EFI_HII_HANDLE       Handle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

/**
  Free Menu list linked list.

  @param  MenuListHead    One Menu list point in the menu list.

**/
VOID
UiFreeMenuList (
  IN LIST_ENTRY   *MenuListHead
  );

/**
  Process the goto op code, update the info in the selection structure.

  @param Statement    The statement belong to goto op code.
  @param Selection    The selection info.
  @param Repaint      Whether need to repaint the menu.
  @param NewLine      Whether need to create new line.

  @retval EFI_SUCCESS    The menu process successfully.
  @return Other value if the process failed.
**/
EFI_STATUS
ProcessGotoOpCode (
  IN OUT   FORM_BROWSER_STATEMENT      *Statement,
  IN OUT   UI_MENU_SELECTION           *Selection,
  OUT      BOOLEAN                     *Repaint,
  OUT      BOOLEAN                     *NewLine
  );

/**
  Process Question Config.

  @param  Selection              The UI menu selection.
  @param  Question               The Question to be peocessed.

  @retval EFI_SUCCESS            Question Config process success.
  @retval Other                  Question Config process fail.

**/
EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  );

/**
  Search an Option of a Question by its value.

  @param  Question               The Question
  @param  OptionValue            Value for Option to be searched.

  @retval Pointer                Pointer to the found Option.
  @retval NULL                   Option not found.

**/
QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  );

/**
  Return data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.

  @retval Value                  The data to be returned

**/
UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT32                    Index
  );

/**
  Set value of a data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.
  @param  Value                  The value to be set.

**/
VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT32                   Index,
  IN UINT64                   Value
  );

/**
  Count the storage space of a Unicode string.

  This function handles the Unicode string with NARROW_CHAR
  and WIDE_CHAR control characters. NARROW_HCAR and WIDE_CHAR
  does not count in the resultant output. If a WIDE_CHAR is
  hit, then 2 Unicode character will consume an output storage
  space with size of CHAR16 till a NARROW_CHAR is hit.

  If String is NULL, then ASSERT ().

  @param String          The input string to be counted.

  @return Storage space for the input string.

**/
UINTN
GetStringWidth (
  IN CHAR16               *String
  );

/**
  Concatenate a narrow string to another string.

  @param Destination The destination string.
  @param Source      The source string. The string to be concatenated.
                     to the end of Destination.

**/
VOID
NewStrCat (
  IN OUT CHAR16               *Destination,
  IN     CHAR16               *Source
  );

/**
  Reset stack pointer to begin of the stack.

**/
VOID
ResetScopeStack (
  VOID
  );

/**
  Push the expression options by ExpressOption onto the Stack.

  @param  Pointer                Pointer to the current expression.

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PushExpressionByExpressOption (
  IN FORM_EXPRESSION   *Pointer
  )
  ;

/**
  Push the expression options by ExpressStatement onto the Stack.

  @param  Pointer                Pointer to the current expression.

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PushExpressionByExpressStatement (
  IN FORM_EXPRESSION   *Pointer
  )
  ;

/**
  Push the expression options by ExpressForm onto the Stack.

  @param  Pointer                Pointer to the current expression.

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PushExpressionByExpressForm (
  IN FORM_EXPRESSION   *Pointer
  )
  ;

/**
  Pop the expression options by ExpressOption from the Stack

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PopExpressionByExpressOption (
  VOID
  )
  ;

/**
  Pop the expression options by ExpressStatement from the Stack

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PopExpressionByExpressStatement (
  VOID
  )
  ;

/**
  Pop the expression options by ExpressForm from the Stack

  @retval EFI_SUCCESS            The value was pushed onto the stack.

**/
EFI_STATUS
PopExpressionByExpressForm (
  VOID
  )
  ;

/**
  Get the expression Buffer pointer by ExpressOption.

  @retval  The start pointer of the expression buffer.

**/
FORM_EXPRESSION **
GetExpressionListByExpressOption (
  VOID
  )
  ;

/**
  Get the expression Buffer pointer by ExpressStatement.

  @retval  The start pointer of the expression buffer.

**/
FORM_EXPRESSION **
GetExpressionListByExpressStatement (
  VOID
  )
  ;

/**
  Get the expression Buffer pointer by ExpressForm.

  @retval  The start pointer of the expression buffer.

**/
FORM_EXPRESSION **
GetExpressionListByExpressForm (
  VOID
  )
  ;

/**
  Get the expression list count by ExpressOption.

  @retval                        The expression count

**/
INTN
GetExpressionCountByExpressOption (
  VOID
  )
  ;

/**
  Get the expression list count by ExpressStatement.

  @retval                        The expression count

**/
INTN
GetExpressionCountByExpressStatement (
  VOID
  )
  ;
/**
  Get the expression list count by ExpressForm.

  @retval                        The expression count

**/
INTN
GetExpressionCountByExpressForm (
  VOID
  )
  ;

/**
  Push an Operand onto the Stack

  @param  Operand                Operand to push.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.

**/
EFI_STATUS
PushScope (
  IN UINT8   Operand
  );

/**
  Pop an Operand from the Stack

  @param  Operand                Operand to pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.

**/
EFI_STATUS
PopScope (
  OUT UINT8     *Operand
  );

/**
  Init stack pointer to begin of the stack.

**/
VOID
InitCurrentExpressionStack (
  VOID
  );

/**
  Push current expression onto the Stack

  @param  Pointer                Pointer to current expression.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushCurrentExpression (
  IN VOID  *Pointer
  );

/**
  Pop current expression from the Stack

  @param  Pointer                Pointer to current expression to be pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PopCurrentExpression (
  OUT VOID    **Pointer
  );

/**
  Reset stack pointer to begin of the stack.

**/
VOID
ResetMapExpressionListStack (
  VOID
  );

/**
  Push the list of map expression onto the Stack

  @param  Pointer                Pointer to the list of map expression to be pushed.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushMapExpressionList (
  IN VOID  *Pointer
  );

/**
  Pop the list of map expression from the Stack

  @param  Pointer                Pointer to the list of map expression to be pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PopMapExpressionList (
  OUT VOID    **Pointer
  );

/**
  Get Form given its FormId.

  @param  FormSet                The FormSet which contains this Form.
  @param  FormId                 Id of this Form.

  @retval Pointer                The Form.
  @retval NULL                   Specified Form is not found in the FormSet.

**/
FORM_BROWSER_FORM *
IdToForm (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT16                FormId
  );

/**
  Search a Question in FormSet scope using its QuestionId.

  @param  FormSet                The FormSet which contains this Form.
  @param  Form                   The Form which contains this Question.
  @param  QuestionId             Id of this Question.

  @retval Pointer                The Question.
  @retval NULL                   Specified Question not found in the Form.

**/
FORM_BROWSER_STATEMENT *
IdToQuestion (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN UINT16                QuestionId
  );

/**
 Search a Question in Form scope using its QuestionId.

 @param[in] Form                The Form which contains this Question.
 @param[in] QuestionId          Id of this Question.

 @retval Pointer                The Question.
 @retval NULL                   Specified Question not found in the Form.
**/
FORM_BROWSER_STATEMENT *
IdToQuestion2 (
  IN FORM_BROWSER_FORM  *Form,
  IN UINT16             QuestionId
  );

/**
  Zero extend integer/boolean/date/time to UINT64 for comparing.

  @param  Value                  HII Value to be converted.

**/
VOID
ExtendValueToU64 (
  IN  EFI_HII_VALUE   *Value
  );

/**
  Compare two HII value.

  @param  Value1                 Expression value to compare on left-hand.
  @param  Value2                 Expression value to compare on right-hand.
  @param  Result                 Return value after compare.
                                 retval 0                      Two operators equal.
                                 return Positive value if Value1 is greater than Value2.
                                 retval Negative value if Value1 is less than Value2.
  @param  HiiHandle              Only required for string compare.

  @retval other                  Could not perform compare on two values.
  @retval EFI_SUCCESS            Compare the value success.

**/
EFI_STATUS
CompareValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  );

/**
  Evaluate the result of a HII expression

  If Expression is NULL, then ASSERT.

  @param  FormSet                FormSet associated with this expression.
  @param  Form                   Form associated with this expression.
  @param  Expression             Expression to be evaluated.

  @retval EFI_SUCCESS            The expression evaluated successfuly
  @retval EFI_NOT_FOUND          The Question which referenced by a QuestionId
                                 could not be found.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.
  @retval EFI_ACCESS_DENIED      The pop operation underflowed the stack
  @retval EFI_INVALID_PARAMETER  Syntax error with the Expression

**/
EFI_STATUS
EvaluateExpression (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN OUT FORM_EXPRESSION   *Expression
  );

/**
  Return the result of the expression list. Check the expression list and
  return the highest priority express result.
  Priority: DisableIf > SuppressIf > GrayOutIf > FALSE

  @param  ExpList             The input expression list.
  @param  Evaluate            Whether need to evaluate the expression first.
  @param  FormSet             FormSet associated with this expression. Only
                              needed when Evaluate is TRUE
  @param  Form                Form associated with this expression. Only
                              needed when Evaluate is TRUE

  @retval EXPR_RESULT      Return the higher priority express result.
                              DisableIf > SuppressIf > GrayOutIf > FALSE

**/
EXPR_RESULT
EvaluateExpressionList (
  IN FORM_EXPRESSION_LIST *ExpList,
  IN BOOLEAN              Evaluate,
  IN FORM_BROWSER_FORMSET *FormSet OPTIONAL,
  IN FORM_BROWSER_FORM    *Form OPTIONAL
  );

/**
  Transfer the device path string to binary format.

  @param   StringPtr     The device path string info.

  @retval  Device path binary info.

**/
EFI_DEVICE_PATH_PROTOCOL *
ConvertDevicePathFromText (
  IN CHAR16  *StringPtr
  );

#endif // _UI_H
