/** @file

  This file contains definitions required for creation of SPS Info Protocol.

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

#ifndef _SPS_INFO_PROTOCOL_H_
#define _SPS_INFO_PROTOCOL_H_

#include <SpsInfo.h>

#define SPS_INFO_PROTOCOL_GUID \
  { 0xAC6F5F1D, 0x0D53, 0x44A9, { 0xAB, 0xBE, 0xC9, 0x24, 0x57, 0xD3, 0xE3, 0x7A }}

typedef struct _SPS_INFO               SPS_INFO_PROTOCOL;

extern EFI_GUID gSpsInfoProtocolGuid;

#endif
