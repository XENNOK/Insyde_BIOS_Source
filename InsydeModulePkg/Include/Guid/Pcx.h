/** @file
  PCX logo definition file

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

#ifndef _PCX_GUID_H_
#define _PCX_GUID_H_

//
// Definitions for PCX files
//

#pragma pack(1)

//
//  Pcx format file header
//
typedef struct {
  UINT8                       Manufacturer;
  UINT8                       Version;
  UINT8                       Encoding;
  UINT8                       BitsPerPixel;
  UINT16                      Xmin;
  UINT16                      Ymin;
  UINT16                      Xmax;
  UINT16                      Ymax;
  UINT16                      DHpi;
  UINT16                      VHpi;
  UINT8                       Colormap[48];
  UINT8                       Reserved;
  UINT8                       NPlanes;
  UINT16                      BytesPerLine;
  UINT16                      PaletteInfo;
  UINT16                      HscreenSize;
  UINT16                      VscreenSize;
  UINT8                       Blank[54];
} PCX_IMAGE_HEADER;

#pragma pack()

#define EFI_DEFAULT_PCX_LOGO_GUID \
  {0x6f0cf054, 0xae6a, 0x418c, 0xa7, 0xce, 0x3c, 0x7a, 0x7c, 0xd7, 0x4e, 0xc0}
//
// {6F0CF054-AE6A-418c-A7CE-3C7A7CD74EC0}
//

extern EFI_GUID gEfiDefaultPcxLogoGuid;

#endif
