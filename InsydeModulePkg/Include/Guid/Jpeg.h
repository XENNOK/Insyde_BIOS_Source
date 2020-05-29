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

/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Jpeg.h

Abstract:

--*/

#ifndef _JPEG_GUID_H_
#define _JPEG_GUID_H_


//
// Definitions for JPEG files
//                                     

#define BADGE_SIGNATURE SIGNATURE_32('$','L','O','C')


#pragma pack(1)

//
// JPG Comment Entry
//
typedef struct {
  UINT8  FieldStart;
  UINT8  FieldType;
  UINT8  HighSize;
  UINT8  LowSize;
  UINT32 Signature;
  UINT16 BadgeXLocation;
  UINT16 BadgeYLocation;
} JPG_BADGE_COMMENT;

#pragma pack()

#define EFI_DEFAULT_JPEG_LOGO_GUID \
  {0x1FFF93C2, 0x8C76, 0x49e4, 0x8A, 0xB3, 0x43, 0xD9, 0x2F, 0x54, 0x45, 0xEF}
//
// 1FFF93C2-8C76-49e4-8AB343D92F5445EF
//

extern EFI_GUID gEfiDefaultJpegLogoGuid;

#endif
