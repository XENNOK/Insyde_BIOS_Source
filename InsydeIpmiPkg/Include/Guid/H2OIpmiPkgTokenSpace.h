/** @file
 GUID definition of H2O IPMI package token space GUID.

 This token space GUID is for all PCDs that are contained in H2O IPMI package.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _H2OIPMIPKG_TOKEN_SPACE_GUID_H_
#define _H2OIPMIPKG_TOKEN_SPACE_GUID_H_


#define H2OIPMIPKG_TOKEN_SPACE_GUID \
  {0x03a6b6ab, 0xe143, 0x40f4, 0xaf, 0xba, 0x2a, 0xec, 0x8c, 0x3b, 0x9b, 0xc9}

extern EFI_GUID gH2OIpmiPkgTokenSpaceGuid;


#endif

