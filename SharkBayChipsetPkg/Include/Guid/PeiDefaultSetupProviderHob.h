/** @file

  PEI Default Setup Provider GUID.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _PEI_DEFAULT_SETUP_PROVIDER_HOB_GUID_H_
#define _PEI_DEFAULT_SETUP_PROVIDER_HOB_GUID_H_

#define PEI_DEFAULT_SETUP_PROVIDER_HOB_GUID  \
  { \
    0x10C0ED9, 0x4137, 0x4D25, 0xB0, 0xBE, 0xA0, 0xD9, 0x62, 0xE0, 0x67, 0xD1 \
  }

extern EFI_GUID gPeiDefaultSetupProviderGuid;

#endif
