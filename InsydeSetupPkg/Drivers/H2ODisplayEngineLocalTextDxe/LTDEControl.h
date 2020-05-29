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

#ifndef _LTDE_CONTROL_H_
#define _LTDE_CONTROL_H_

#define PANEL_COLOR_ATTRIBUTE_SETUP_PAGE    (EFI_BLUE      | EFI_BACKGROUND_LIGHTGRAY)
#define PANEL_COLOR_ATTRIBUTE_HOT_KEY       (EFI_BLACK     | EFI_BACKGROUND_CYAN)
#define PANEL_COLOR_ATTRIBUTE_HELP_TEXT     (EFI_BLUE      | EFI_BACKGROUND_LIGHTGRAY)
#define PANEL_COLOR_ATTRIBUTE_QUESTION      (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE)

EFI_STATUS
GetScreenMaxXY (
  OUT    UINT32                              *X,
  OUT    UINT32                              *Y
  );

EFI_STATUS
GetClearField (
  IN OUT RECT                                *ResultField,
  IN     RECT                                *SourceField,
  IN     H2O_CONTROL_INFO                    *UsedMenu,
  IN     UINT32                              MenuCount,
  IN     UINT32                              BorderLineWidth
  );

EFI_STATUS
GetStringField (
  IN OUT RECT                                *ResultField,
  IN     RECT                                *Field,
  IN OUT RECT                                *LimitInField OPTIONAL,
  IN     CHAR16                              *Text
  );

EFI_STATUS
GetControlsField (
  IN     H2O_PANEL_INFO                      *StylePanel,
  IN     UINT32                              PanelCount,
  IN     H2O_PANEL_INFO                      *PanelList,
  IN     UINT32                              ControlListCount,
  IN     H2O_CONTROL_LIST                    *ControlListArray
  );

UINT32
ClearField (
  IN     UINT32                              Attribute,
  ...
  );

UINT32
DisplayBorderLine (
  IN     UINT32                              Attribute,
  ...
  );

EFI_STATUS
DisplayPanel (
  IN H2O_PANEL_INFO                          *Panel,
  IN UINT32                                  PseudoClass
  );

EFI_STATUS
DisplayNormalControls (
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  );

EFI_STATUS
DisplayButtonControls (
  IN     BOOLEAN                             BottonStartEndChar,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  );

EFI_STATUS
DisplayHighLightControl (
  IN     BOOLEAN                             BottonStartEndChar,
  IN     H2O_CONTROL_INFO                    *Control
  );

EFI_STATUS
DisplayPseudoClassControl (
  IN     UINT32                              PseudoClass,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  );

EFI_STATUS
DisplayCenteredControl (
  IN     BOOLEAN                             BottonStartEndChar,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  );

EFI_STATUS
DisplayControls (
  IN     CONST BOOLEAN                       IsCentered,
  IN     BOOLEAN                             ButtonStartEndChar,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  );

EFI_STATUS
CalculateRequireSize (
  IN  CHAR16                                 *DisplayString,
  IN  UINT32                                 LimitLineWidth,
  OUT UINT32                                 *RequireWidth,
  OUT UINT32                                 *RequireHeight
  );

#endif
