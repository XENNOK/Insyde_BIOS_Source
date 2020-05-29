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

EFI_STATUS
SeparateStringByFont (
  IN     CHAR16                                *String,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  IN     UINT32                                LimitWidth,
  OUT    EFI_IMAGE_OUTPUT                      **CharImageOutArray OPTIONAL,
  OUT    UINT32                                *SeparateStrCount,
  OUT    UINT32                                **SeparateStrOffsetArray OPTIONAL,
  OUT    UINT32                                **SeparateStrWidthArray OPTIONAL,
  OUT    UINTN                                 **SeparateStrArray
  )
{
  EFI_STATUS                                   Status;
  EFI_HII_FONT_PROTOCOL                        *HiiFont;

  EFI_IMAGE_OUTPUT                             *ImageOut;
  EFI_IMAGE_OUTPUT                             *ImageOutArray;
  UINT32                                       Index;
  UINT32                                       Stringlen;
  UINT32                                       *BaselineArray;

  UINT32                                       TotalWidth;
  UINT32                                       TotalHeight;

  UINT32                                       StrCount;
  UINT32                                       *StrOffsetArray;
  UINT32                                       *StrWidthArray;
  UINTN                                        *StrAddrArray;
  UINT32                                       CurrentWidth;
  CHAR16                                       *CurrentStr;
  UINT32                                       CurrentStrLen;


  Status = EFI_SUCCESS;

  if (String == NULL || FontDisplayInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (SeparateStrCount == NULL || SeparateStrArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiHiiFontProtocolGuid, NULL, (VOID **) &HiiFont);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ImageOutArray  = NULL;
  BaselineArray  = NULL;
  StrOffsetArray = NULL;
  StrWidthArray  = NULL;
  StrAddrArray   = NULL;
  Stringlen = (UINT32)(StrLen (String));
  ImageOutArray = AllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT) * Stringlen);
  if (ImageOutArray == NULL) {
    goto Error;
  }
  BaselineArray = AllocateZeroPool (sizeof (UINT32) * Stringlen);
  if (BaselineArray == NULL) {
    goto Error;
  }
  //
  // Get ImageOut->Width and ImageOut->Height
  //
  TotalWidth = 0;
  TotalHeight = 0;
  for (Index = 0; Index < Stringlen; Index ++) {
    ImageOut = NULL;
    Status = HiiFont->GetGlyph (
                         HiiFont,
                         String[Index],
                         FontDisplayInfo,
                         &ImageOut,
                         (UINTN *)&BaselineArray[Index]
                         );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Save each ImageOut
    //
    if (ImageOut != NULL) {
      if (ImageOut->Image.Bitmap != NULL) {
        CopyMem (&ImageOutArray[Index], ImageOut, sizeof (EFI_IMAGE_OUTPUT));
        SafeFreePool ((VOID **) &ImageOut);
      }
    }

    TotalWidth += ImageOutArray[Index].Width;
    TotalHeight = (UINT32)MAX (TotalHeight, ImageOutArray[Index].Height);
    if (BaselineArray != NULL) {
      TotalHeight = (UINT32)MAX (TotalHeight, BaselineArray[Index]);
    }
  }

  //
  // Separate line by width
  //
  StrCount = 1;
  if (LimitWidth != 0) {
    if (TotalWidth > LimitWidth) {
      //
      // Get separate string count
      //
      CurrentWidth = 0;
      Index = 0;
      while (Index < Stringlen) {
        CurrentWidth += ImageOutArray[Index].Width;
        if (CurrentWidth > LimitWidth || String[Index] == CHAR_CARRIAGE_RETURN) {
          if (ImageOutArray[Index].Width > LimitWidth) {
            //
            // Can't fill one of words
            //
            SafeFreePool ((VOID **) &ImageOutArray);
            SafeFreePool ((VOID **) &BaselineArray);
            return EFI_BUFFER_TOO_SMALL;
          }
          StrCount ++;
          CurrentWidth = 0;
          Index --;
        }
        Index ++;
      }
    }
  }
  StrOffsetArray = AllocateZeroPool (sizeof (UINT32) * StrCount);
  if (StrOffsetArray == NULL) {
    goto Error;
  }
  StrWidthArray = AllocateZeroPool (sizeof (UINT32) * StrCount);
  if (StrWidthArray == NULL) {
    goto Error;
  }
  StrAddrArray = AllocateZeroPool (sizeof (UINTN) * StrCount);
  if (StrAddrArray == NULL) {
    goto Error;
  }
  if (StrCount == 1) {
    for (Index = 0, CurrentWidth = 0;Index < Stringlen; Index ++) {
      CurrentWidth += ImageOutArray[Index].Width;
    }
    StrWidthArray[0] = CurrentWidth;
    StrAddrArray[0] = (UINTN)(UINTN *)CatSPrint (NULL, String);
  } else {
    //
    // Get each separate string index
    //
    StrCount = 0;
    CurrentWidth = 0;
    Index = 0;
    for (Index = 0, CurrentWidth = 0;Index < Stringlen; Index ++) {
      CurrentWidth += ImageOutArray[Index].Width;
      StrWidthArray[StrCount] = CurrentWidth;
      if (CurrentWidth > LimitWidth || String[Index] == CHAR_CARRIAGE_RETURN) {
        StrWidthArray[StrCount] -= ImageOutArray[Index].Width;
        StrCount ++;
        StrOffsetArray[StrCount] = Index;
        CurrentWidth = 0;
        Index --;
      }
    }
    StrCount ++;

    //
    // Get separate strings
    //
    Index = 0;
    while (Index < StrCount) {
      CurrentStr = String + StrOffsetArray[Index];
      if (Index == StrCount - 1) {
        CurrentStrLen = (UINT32)(StrLen (CurrentStr));
      } else {
        CurrentStrLen = StrOffsetArray[Index + 1] - StrOffsetArray[Index];
      }
      StrAddrArray[Index] = (UINTN)(UINTN *)AllocateCopyPool (sizeof (CHAR16) * (CurrentStrLen + 1), CurrentStr);
      CurrentStr = (CHAR16 *)(UINTN *)StrAddrArray[Index];
      CurrentStr[CurrentStrLen] = '\0';
      Index ++;
    }
  }

  SafeFreePool ((VOID **) &BaselineArray);

  if (CharImageOutArray != NULL) {
    *CharImageOutArray = ImageOutArray;
  } else {
    SafeFreePool ((VOID **) &ImageOutArray);
  }
  if (SeparateStrCount != NULL) {
    *SeparateStrCount = StrCount;
  }
  if (SeparateStrOffsetArray != NULL) {
    *SeparateStrOffsetArray = StrOffsetArray;
  } else {
    SafeFreePool ((VOID **) &StrOffsetArray);
  }
  if (SeparateStrWidthArray != NULL) {
    *SeparateStrWidthArray = StrWidthArray;
  } else {
    SafeFreePool ((VOID **) &StrWidthArray);
  }
  if (SeparateStrArray != NULL) {
    *SeparateStrArray = StrAddrArray;
  } else {
    SafeFreePool ((VOID **) &StrAddrArray);
  }

  return EFI_SUCCESS;

Error:

  SafeFreePool ((VOID **) &ImageOutArray);
  SafeFreePool ((VOID **) &BaselineArray);
  SafeFreePool ((VOID **) &StrOffsetArray);
  SafeFreePool ((VOID **) &StrWidthArray);
  SafeFreePool ((VOID **) &StrAddrArray);
  return EFI_OUT_OF_RESOURCES;

}

EFI_STATUS
GetStringImageByFont (
  IN     CHAR16                                *String,
  IN     RECT                                  *Field,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  OUT    EFI_IMAGE_OUTPUT                      **ImageOutput
  )
{
  EFI_STATUS                                   Status;

  EFI_IMAGE_OUTPUT                             *CharImageOutArray;
  UINT32                                       SeparateStringNum;
  UINT32                                       *SeparateStringOffset;
  UINT32                                       *SeparateStringWidth;
  UINTN                                        *SeparateStrings;
  CHAR16                                       *SeparateString;
  UINT32                                       StrIndex;
  UINT32                                       CharIndex;
  UINT32                                       CurrentWidth;

  UINT32                                       LimitWidth;
  UINT32                                       LimitHeight;

  EFI_IMAGE_OUTPUT                             *ImageOut;


  Status = EFI_SUCCESS;

  if (String == NULL || FontDisplayInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  LimitWidth = Field->right - Field->left;
  LimitHeight = Field->bottom - Field->top;

  Status = SeparateStringByFont (
             String,
             FontDisplayInfo,
             LimitWidth,
             &CharImageOutArray,
             &SeparateStringNum,
             &SeparateStringOffset,
             &SeparateStringWidth,
             &SeparateStrings
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    *ImageOutput = NULL;
    return Status;
  } else if (EFI_ERROR (Status)) {
    *ImageOutput = NULL;
    return Status;
  }

  ImageOut = AllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT));
  if (ImageOut == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ImageOut->Width  = (UINT16)LimitWidth;
  ImageOut->Height = (UINT16)LimitHeight;
  ImageOut->Image.Bitmap = AllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * ImageOut->Width * ImageOut->Height);
  CurrentWidth = 0;
  StrIndex = 0;
  CharIndex = 0;
  while (String[CharIndex] != '\0') {
    if (CharImageOutArray[CharIndex].Width == 0) {
      CharIndex ++;
      continue;
    }
    if (CharIndex >= SeparateStringOffset[StrIndex + 1]) {
      StrIndex ++;
      CurrentWidth = 0;
      continue;
    }
    Status = BltCopy(
               CharImageOutArray[CharIndex].Image.Bitmap,
               0,
               0,
               (UINTN)(CharImageOutArray[CharIndex].Width),
               ImageOut->Image.Bitmap,
               (UINTN)(CurrentWidth),
               (UINTN)(StrIndex * FontDisplayInfo->FontInfo.FontSize),
               (UINTN)(ImageOut->Width),
               (UINTN)(ImageOut->Height),
               (UINTN)(CharImageOutArray[CharIndex].Width),
               (UINTN)(CharImageOutArray[CharIndex].Height)
               );
    if (EFI_ERROR (Status)) {
      *ImageOutput = NULL;
      return EFI_BUFFER_TOO_SMALL;
    }
    CurrentWidth += CharImageOutArray[CharIndex].Width;
    CharIndex ++;
  }

  ImageOut->Width = 0;
  for (StrIndex = 0; StrIndex < SeparateStringNum; StrIndex ++) {
    ImageOut->Width = MAX (ImageOut->Width, (UINT16)(SeparateStringWidth[StrIndex]));
    SeparateString = (CHAR16 *)(UINTN *)(UINTN)(SeparateStrings[StrIndex]);
    //SafeFreePool (&SeparateString);
  }
  SafeFreePool ((VOID **) &SeparateStrings);

  SafeFreePool ((VOID **) &CharImageOutArray);
  SafeFreePool ((VOID **) &SeparateStringOffset);
  SafeFreePool ((VOID **) &SeparateStringWidth);

  ImageOut->Height = (UINT16)SeparateStringNum * FontDisplayInfo->FontInfo.FontSize;

  *ImageOutput = ImageOut;

  return EFI_SUCCESS;
}

