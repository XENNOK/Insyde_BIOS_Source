/** @file
  Defination GUID of H2O HII font file guid

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

#ifndef _H2O_HII_FONT_FILE_H_
#define _H2O_HII_FONT_FILE_H_

#define H2O_HII_FONT_FILE_GUID \
  { 0x2132D258, 0xDCB6, 0x42E9, 0xA2, 0x43, 0x06, 0x4A, 0x95, 0x68, 0xD5, 0x12 }

#define TTF_FONT_FILE_GUID  \
  { 0x17772369, 0xD262, 0x4b90, 0x9f, 0x31, 0xBD, 0xc4, 0x1F, 0x26, 0x63, 0xB0 }

extern EFI_GUID gH2OHiiFontFileGuid;
extern EFI_GUID gTtfFontFileGuid;

#endif //_H2O_HII_FONT_FILE_H_
