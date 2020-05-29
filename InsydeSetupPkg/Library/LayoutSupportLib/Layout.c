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

#include "LayoutSupportLib.h"

STATIC
EFI_STATUS
ParsePseudoClass (
  IN CONST CHAR8                                 *PseudoClassStr,
  IN OUT   UINT32                                *PseudoClass
  )
{
  if (PseudoClass == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (AsciiStrCmp (PseudoClassStr, ":selectable") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_SELECTABLE;
  } else if (AsciiStrCmp (PseudoClassStr, ":grayout") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_GRAYOUT;
  } else if (AsciiStrCmp (PseudoClassStr, ":highlight") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_HIGHLIGHT;
  } else if (AsciiStrCmp (PseudoClassStr, ":link") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_LINK;
  } else if (AsciiStrCmp (PseudoClassStr, ":visited") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_VISITED;
  } else if (AsciiStrCmp (PseudoClassStr, ":hover") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_HOVER;
  } else if (AsciiStrCmp (PseudoClassStr, ":focus") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_FOCUS;
  } else if (AsciiStrCmp (PseudoClassStr, ":active") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_ACTIVE;
  } else if (AsciiStrCmp (PseudoClassStr, ":enabled") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_ENABLED;
  } else if (AsciiStrCmp (PseudoClassStr, ":disabled") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_DISABLED;
  } else if (AsciiStrCmp (PseudoClassStr, ":not") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_NOT;
  } else if (AsciiStrCmp (PseudoClassStr, ":help") == 0) {
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_HELP;
  } else {
    //
    // default
    //
    *PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseLayoutPkg (
  IN     UINT8                                 *PkgBuffer,
  OUT    LIST_ENTRY                            **LayoutListHead
  )
{
  EFI_STATUS                                   Status;

  H2O_LAYOUT_PACKAGE_HDR                       *CurrentLayoutPkg;
  H2O_LAYOUT_INFO                              *CurrentLayout;
  H2O_PANEL_INFO                               *CurrentPanel;
  H2O_STYLE_INFO                               *CurrentStyle;
  H2O_PROPERTY_INFO                            *CurrentProperty;

  UINT8                                        *BlockDataBuffer;
  UINT32                                       BlockDataBufferLength;
  UINT8                                        *BlockData;
  UINT8                                        BlockType;
  UINT8                                        BlockSize;
  UINT32                                       BlockOffset;

  H2O_LAYOUT_ID                                OldLayoutId;
  H2O_PANEL_ID                                 OldPanelId;
  LIST_ENTRY                                   *Link;
  H2O_LAYOUT_INFO                              *TempLayout;
  H2O_PANEL_INFO                               *TempPanel;
  CHAR8                                        *PseudoClassStr;


  if (PkgBuffer == NULL || LayoutListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;

  CurrentLayoutPkg = NULL;
  CurrentLayout = NULL;
  CurrentPanel = NULL;
  CurrentStyle = NULL;

  TempLayout = NULL;
  TempPanel = NULL;

  //
  // Layout Pkg
  //
  CurrentLayoutPkg = (H2O_LAYOUT_PACKAGE_HDR *) (PkgBuffer + sizeof(UINT32));
  if (CurrentLayoutPkg->Header.Type != H2O_HII_PACKAGE_LAYOUTS) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Block Data Buffer
  //
  BlockDataBuffer = (UINT8 *)(CurrentLayoutPkg + 1);
  BlockDataBufferLength = CurrentLayoutPkg->Header.Length - CurrentLayoutPkg->HdrSize;

  //
  // Init Layout List
  //
  *LayoutListHead = AllocateZeroPool (sizeof (LIST_ENTRY));
  if (*LayoutListHead == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeListHead (*LayoutListHead);

  //
  // Parse each layout block
  //
  BlockOffset = 0;
  while (BlockOffset < BlockDataBufferLength) {
    BlockData = BlockDataBuffer + BlockOffset;

    BlockType = ((H2O_HII_LAYOUT_BLOCK *) BlockData)->BlockType;
    BlockSize = ((H2O_HII_LAYOUT_BLOCK *) BlockData)->BlockSize;

    if (BlockSize == 0) {
      break;
    }

    BlockOffset += BlockSize;

    switch (BlockType) {

    case H2O_HII_LIBT_LAYOUT_BEGIN:
      CurrentLayout = AllocateZeroPool (sizeof (H2O_LAYOUT_INFO));
      if (CurrentLayout == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CurrentLayout->Signature = H2O_LAYOUT_INFO_NODE_SIGNATURE;
      CurrentLayout->LayoutId = ((H2O_HII_LIBT_LAYOUT_BEGIN_BLOCK *)BlockData)->LayoutId;
      InitializeListHead (&CurrentLayout->PanelListHead);
      InitializeListHead (&CurrentLayout->StyleListHead);
      InsertTailList (*LayoutListHead, &CurrentLayout->Link);
      break;

    case H2O_HII_LIBT_LAYOUT_END:
      CurrentLayout = NULL;
      break;

    case H2O_HII_LIBT_PANEL_BEGIN:
      CurrentPanel = AllocateZeroPool (sizeof (H2O_PANEL_INFO));
      if (CurrentPanel == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CurrentPanel->Signature = H2O_PANEL_INFO_NODE_SIGNATURE;
      CurrentPanel->PanelId = ((H2O_HII_LIBT_PANEL_BEGIN_BLOCK *)BlockData)->PanelId;
      CurrentPanel->PanelType = ((H2O_HII_LIBT_PANEL_BEGIN_BLOCK *)BlockData)->PanelType;
      CopyMem (&CurrentPanel->PanelGuid, &((H2O_HII_LIBT_PANEL_BEGIN_BLOCK *)BlockData)->PanelGuid, sizeof (EFI_GUID));
      CurrentPanel->ParentLayout = CurrentLayout;
      InitializeListHead (&CurrentPanel->StyleListHead);
      InsertTailList (&CurrentLayout->PanelListHead, &CurrentPanel->Link);
      break;

    case H2O_HII_LIBT_PANEL_END:
      CurrentPanel = NULL;
      break;

    case H2O_HII_LIBT_STYLE_BEGIN:
      if (CurrentPanel == NULL && CurrentLayout == NULL) {
        CurrentStyle = NULL;
        break;
      }
      CurrentStyle = AllocateZeroPool (sizeof (H2O_STYLE_INFO));
      if (CurrentStyle == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CurrentStyle->Signature = H2O_STYLE_INFO_NODE_SIGNATURE;
      CurrentStyle->StyleType = ((H2O_HII_LIBT_STYLE_BEGIN_BLOCK *)BlockData)->Type;
      CurrentStyle->ClassName = (CHAR8 *)BlockData + ((H2O_HII_LIBT_STYLE_BEGIN_BLOCK *)BlockData)->ClassNameOffset;
      PseudoClassStr = (CHAR8 *)BlockData + ((H2O_HII_LIBT_STYLE_BEGIN_BLOCK *)BlockData)->PseudoClassOffset;
      ParsePseudoClass (PseudoClassStr, &CurrentStyle->PseudoClass);
      InitializeListHead (&CurrentStyle->PropertyListHead);
      if (CurrentPanel != NULL) {
        InsertTailList (&CurrentPanel->StyleListHead, &CurrentStyle->Link);
      } else if (CurrentLayout != NULL) {
        InsertTailList (&CurrentLayout->StyleListHead, &CurrentStyle->Link);
      }
      break;

    case H2O_HII_LIBT_STYLE_END:
      CurrentStyle = NULL;
      break;

    case H2O_HII_LIBT_PROPERTY:
      if (CurrentStyle == NULL && CurrentPanel == NULL) {
        CurrentProperty = NULL;
        break;
      }
      if (CurrentStyle == NULL) {
        CurrentStyle = AllocateZeroPool (sizeof (H2O_STYLE_INFO));
        if (CurrentStyle == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CurrentStyle->Signature = H2O_STYLE_INFO_NODE_SIGNATURE;
        CurrentStyle->StyleType = H2O_IFR_STYLE_TYPE_PANEL;
        InitializeListHead (&CurrentStyle->PropertyListHead);
        InsertTailList (&CurrentPanel->StyleListHead, &CurrentStyle->Link);
      }
      CurrentProperty = AllocateZeroPool (sizeof (H2O_PROPERTY_INFO));
      if (CurrentProperty == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CurrentProperty->Signature = H2O_PROPERTY_INFO_NODE_SIGNATURE;
      CurrentProperty->Identifier = (CHAR8 *)BlockData + ((H2O_HII_LIBT_PROPERTY_BLOCK *)BlockData)->IdentifierOffset;
      CurrentProperty->Value = (CHAR8 *)BlockData + ((H2O_HII_LIBT_PROPERTY_BLOCK *)BlockData)->ValueOffset;
      InsertTailList (&CurrentStyle->PropertyListHead, &CurrentProperty->Link);
      break;

    case H2O_HII_LIBT_LAYOUT_DUP:
      OldLayoutId = ((H2O_HII_LIBT_LAYOUT_DUP_BLOCK *)BlockData)->OldLayoutId;
      Link = *LayoutListHead;
      Link = Link->ForwardLink;
      do {
        TempLayout = H2O_LAYOUT_INFO_NODE_FROM_LINK (Link);
        if (OldLayoutId == TempLayout->LayoutId) {
          CurrentLayout = AllocateCopyPool (sizeof (H2O_LAYOUT_INFO), TempLayout);
          if (CurrentLayout == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          //
          // New LayoutId
          //
          CurrentLayout->LayoutId = ((H2O_HII_LIBT_LAYOUT_DUP_BLOCK *)BlockData)->LayoutId;
          InitializeListHead (&CurrentLayout->PanelListHead);
          InsertTailList (*LayoutListHead, &CurrentLayout->Link);
          break;
        }
        Link = Link->ForwardLink;
      } while (IsNodeAtEnd (*LayoutListHead, Link));
      break;

    case H2O_HII_LIBT_PANEL_DUP:
      OldLayoutId = ((H2O_HII_LIBT_PANEL_DUP_BLOCK *)BlockData)->OldLayoutId;
      Link = *LayoutListHead;
      Link = Link->ForwardLink;
      do {
        //
        // Found OldLayoutId
        //
        TempLayout = H2O_LAYOUT_INFO_NODE_FROM_LINK (Link);
        if (OldLayoutId == TempLayout->LayoutId) {

          OldPanelId = ((H2O_HII_LIBT_PANEL_DUP_BLOCK *)BlockData)->OldPanelId;
          Link = &TempLayout->PanelListHead;
          Link = Link->ForwardLink;
          do {
            //
            // Found OldPanelId
            //
            TempPanel = H2O_PANEL_INFO_NODE_FROM_LINK (Link);
            if (OldPanelId == TempPanel->PanelId) {
              CurrentPanel = AllocateCopyPool (sizeof (H2O_PANEL_INFO), TempPanel);
              //
              // New PanelId
              //
              CurrentPanel->PanelId = ((H2O_HII_LIBT_PANEL_DUP_BLOCK *)BlockData)->PanelId;
              InsertTailList (&CurrentLayout->PanelListHead, &CurrentPanel->Link);
              break;
            }
            Link = Link->ForwardLink;
          } while (IsNodeAtEnd (&TempLayout->PanelListHead, Link));

          break;
        }
        Link = Link->ForwardLink;
      } while (IsNodeAtEnd (*LayoutListHead, Link));
      break;

    case H2O_HII_LIBT_EXT2:
      break;

    case H2O_HII_LIBT_EXT4:
      break;

    default:
      break;
    }
  }

  return Status;
}

STATIC
EFI_STATUS
GetLayoutPackage (
  IN     EFI_HII_HANDLE                  HiiHandle,
  OUT    UINT8                           **LayoutPackage
  )
{
  EFI_STATUS                             Status;

  EFI_HII_DATABASE_PROTOCOL              *HiiDatabase;
  EFI_HII_PACKAGE_LIST_HEADER            *HiiPackageList;
  EFI_HII_PACKAGE_HEADER                 *Package;
  UINT32                                 PackageLength;
  UINTN                                  BufferSize;
  UINTN                                  Offset;

  if (LayoutPackage == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *LayoutPackage = NULL;

  //
  // Locate HiiDatabase Protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the HII package list by HiiHandle
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HiiPackageList = (EFI_HII_PACKAGE_LIST_HEADER *) AllocatePool (BufferSize);
    if (HiiPackageList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
    if (EFI_ERROR (Status)) {
      SafeFreePool ((VOID **) &HiiPackageList);
      return Status;
    }
  } else {
    return Status;
  }

  //
  // Find layout package by package type
  //
  Status = EFI_NOT_FOUND;
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageLength = HiiPackageList->PackageLength;
  while (Offset < PackageLength) {
    Package = (EFI_HII_PACKAGE_HEADER *) (((UINT8 *) HiiPackageList) + Offset);

    if (Package->Type == H2O_HII_PACKAGE_LAYOUTS) {
      *LayoutPackage = (UINT8 *) AllocatePool (PackageLength);
      if (*LayoutPackage != NULL) {
        CopyMem (*LayoutPackage, Package, PackageLength);
        Status = EFI_SUCCESS;
      } else {
        Status = EFI_BUFFER_TOO_SMALL;
      }
      break;
    }
    Offset += Package->Length;
  }

  if (Offset >= PackageLength) {
    Status = EFI_NOT_FOUND;
  }

  SafeFreePool ((VOID **) &HiiPackageList);

  return Status;
}

EFI_STATUS
GetLayoutById (
  IN     UINT32                                  LayoutId,
  OUT    H2O_LAYOUT_INFO                         **Layout
  )
{
  EFI_STATUS                                     Status;

  LAYOUT_DATABASE_PROTOCOL                       *LayoutDatabase;
  UINT8                                          *PkgBuffer;

  LIST_ENTRY                                     *LayoutListHead;
  LIST_ENTRY                                     *LayoutLink;
  H2O_LAYOUT_INFO                                *TempLayout;


  //
  // Get HII layout pkg buffer
  //
  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PkgBuffer = NULL;
  Status = GetLayoutPackage (LayoutDatabase->LayoutPkgHiiHandle, &PkgBuffer);
  //
  // BUGBUG: HiiDatabase not support layout pkg yet
  //
  if (EFI_ERROR (Status)) {
    //return Status;
  }
  PkgBuffer = (UINT8 *)(UINTN *)LayoutDatabase->LayoutPkgAddr;
  if (PkgBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  LayoutListHead = LayoutDatabase->LayoutListHead;
  if (LayoutListHead == NULL) {
    //
    // Parse layout pkg into layout list
    //
    Status = ParseLayoutPkg(PkgBuffer, &LayoutListHead);
    if (Status != EFI_SUCCESS) {
      return Status;
    }
    LayoutDatabase->LayoutListHead = LayoutListHead;
  }

  //
  // Get Layout
  //
  LayoutLink = LayoutListHead;
  if (IsNull (LayoutLink, LayoutLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  *Layout = NULL;
  do {
    LayoutLink = LayoutLink->ForwardLink;
    TempLayout = H2O_LAYOUT_INFO_NODE_FROM_LINK (LayoutLink);

    if (TempLayout->LayoutId == LayoutId) {
      *Layout = TempLayout;
      break;
    }
  } while (!IsNodeAtEnd (LayoutListHead, LayoutLink));

  if (*Layout == NULL) {
    *Layout = TempLayout;
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

