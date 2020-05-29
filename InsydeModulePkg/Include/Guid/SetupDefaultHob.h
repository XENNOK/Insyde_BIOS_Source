/** @file
  Setup Default Hob GUID definition
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_DEFAULT_HOB_H_
#define _SETUP_DEFAULT_HOB_H_

#define SETUP_DEFAULT_HOB_GUID \
  { \
    0xbdbd8c0c, 0xca7e, 0x4293, 0x81, 0x64, 0x28, 0xee, 0xaa, 0xa5, 0x27, 0xd5 \
  }

extern EFI_GUID gSetupDefaultHobGuid;

#endif
