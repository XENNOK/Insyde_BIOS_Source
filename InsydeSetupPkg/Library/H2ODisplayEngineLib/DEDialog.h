/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DE_DIALOG_H_
#define _DE_DIALOG_H_


EFI_STATUS
GetPopUpSelectionByValue (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     EFI_HII_VALUE                        *HiiValue,
  OUT    H2O_CONTROL_INFO                     **Controls,
  OUT    UINT32                               *ControlIndex OPTIONAL
  );


EFI_STATUS
GetInputWithTab (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     UINT32                               TabIndex,
  OUT    CHAR16                               **ResultStr
  );

UINT32
GetInputMaxTabNumber (
  IN     H2O_CONTROL_LIST                     *PopUpControls
  );

EFI_STATUS
RefreshPopUpSelectionByValue (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     BOOLEAN                              HightLight,
  IN     BOOLEAN                              BottonStartEndChar,
  IN     EFI_HII_VALUE                        *HiiValue
  );

EFI_STATUS
RefreshInput (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     BOOLEAN                              HightLight,
  IN     CHAR16                               *String
  );

EFI_STATUS
RefreshPasswordInput (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   Count,
  IN CHAR16                                   *String
  );

EFI_STATUS
RefreshPassword (
  IN H2O_CONTROL_LIST                         *PopUpControls,
  IN H2O_CONTROL_INFO                         *CurrentControl,
  IN CHAR16                                   *String
  );

EFI_STATUS
RefreshInputWithTab (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     UINT32                               TabIndex,
  IN     CHAR16                               *String
  );

EFI_STATUS
ShutDialog (
  VOID
  );

#endif
