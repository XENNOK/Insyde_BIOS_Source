/** @file
  Functions for H2O display engine driver.

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


#include "H2ODisplayEngineLib.h"


EFI_STATUS
FreePropertyListHead (
  IN     LIST_ENTRY                            *PropertyListHead
  )
{
  LIST_ENTRY                                   *Link;
  H2O_PROPERTY_INFO                            *CurrentProperty;

  Link = PropertyListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  CurrentProperty = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentProperty = H2O_PROPERTY_INFO_NODE_FROM_LINK (Link);

    FreePool (CurrentProperty);
  } while (!IsNodeAtEnd (PropertyListHead, Link));

  return EFI_SUCCESS;
}

EFI_STATUS
FreeStyleListHead (
  IN     LIST_ENTRY                            *StyleListHead
  )
{
  LIST_ENTRY                                   *Link;
  H2O_STYLE_INFO                               *CurrentStyle;

  Link = StyleListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  CurrentStyle = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentStyle = H2O_STYLE_INFO_NODE_FROM_LINK (Link);

    FreePropertyListHead (&CurrentStyle->PropertyListHead);

    FreePool (CurrentStyle);
  } while (!IsNodeAtEnd (StyleListHead, Link));

  return EFI_SUCCESS;
}

/**
 Init control list

 @param [in] PromptControls             Control list

**/
VOID
InitControlList (
 IN     H2O_CONTROL_LIST                    *ControlList
 )
{
  ZeroMem (ControlList, sizeof (H2O_CONTROL_LIST));
}

EFI_STATUS
FreeControlList (
  IN H2O_CONTROL_LIST                       *ControlList
  )
{
  UINT32                                     Index;

  if (ControlList->ControlArray != NULL) {
    for (Index = 0; Index < ControlList->Count; Index ++) {
      SafeFreePool ((VOID **) &ControlList->ControlArray[Index].Text.String);
    }
    ControlList->Count = 0;
    FreePool (ControlList->ControlArray);
    ControlList->ControlArray = NULL;
  }

  return EFI_SUCCESS;
}

VOID
FreeSetupMenuData (
  IN H2O_FORM_BROWSER_SM                     *SetupMenuData
  )
{
  if (SetupMenuData == NULL) {
    return;
  }

  SafeFreePool ((VOID **) &SetupMenuData->TitleString);
  SafeFreePool ((VOID **) &SetupMenuData->CoreVersionString);
  SafeFreePool ((VOID **) &SetupMenuData->SetupMenuInfoList);
}

EFI_STATUS
FreePanel (
  IN     H2O_PANEL_INFO                      *Panel
  )
{
  //
  // Free Panel
  //
  if (Panel != NULL) {
    SafeFreePool ((VOID **) &Panel->BackgroundImage.CurrentBlt);
    SafeFreePool ((VOID **) &Panel->ContentsImage.CurrentBlt);
    FreeControlList (&Panel->ControlList);
    //FreeStyleListHead (&Panel->StyleListHead);
  }

  return EFI_SUCCESS;
}

/**
 Copy panel.

 @param [in] DestinationPanel         Destination panel
 @param [in] SourcePanel              Source panel

**/
VOID
CopyPanel(
  IN   H2O_PANEL_INFO                      *DestinationPanel,
  IN   H2O_PANEL_INFO                      *SourcePanel
)
{
  CopyMem (DestinationPanel, SourcePanel, sizeof (H2O_PANEL_INFO));
}

VOID
SafeFreePool(
  IN   VOID                                **Buffer
)
{
  if (*Buffer != NULL) {
    FreePool (*Buffer);
  }
  *Buffer = NULL;
}

H2O_PANEL_INFO *
GetPanelInfoByType (
  IN LIST_ENTRY                              *PanelListHead,
  IN UINT32                                  PanelType
  )
{
  LIST_ENTRY                                 *PanelLink;
  H2O_PANEL_INFO                             *Panel;

  if (PanelListHead == NULL || IsNull (PanelListHead, PanelListHead->ForwardLink)) {
    return NULL;
  }

  PanelLink = PanelListHead;
  do {
    PanelLink = PanelLink->ForwardLink;
    if (PanelLink == NULL) {
      break;
    }
    Panel = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);

    if (Panel->PanelType == PanelType) {
      return Panel;
    }
  } while (!IsNodeAtEnd (PanelListHead, PanelLink));

  return NULL;
}

