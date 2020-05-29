/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchReset.h

@brief
  PCH Reset PPI

**/
#ifndef _PEI_PCH_RESET_H_
#define _PEI_PCH_RESET_H_

#include <Protocol/PchReset.h>

///
/// GUID for the PCH Reset PPI
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define PCH_RESET_PPI_GUID \
  { \
    0x433e0f9f, 0x5ae, 0x410a, 0xa0, 0xc3, 0xbf, 0x29, 0x8e, 0xcb, 0x25, 0xac \
  }

#define PCH_RESET_CALLBACK_PPI_GUID \
  { \
    0x17865dc0, 0xb8b, 0x4da8, 0x8b, 0x42, 0x7c, 0x46, 0xb8, 0x5c, 0xca, 0x4d \
  }
#else
#define PCH_RESET_PPI_GUID \
  { \
    0x433e0f9f, 0x5ae, 0x410a, \
    { \
      0xa0, 0xc3, 0xbf, 0x29, 0x8e, 0xcb, 0x25, 0xac \
    } \
  }
#define PCH_RESET_CALLBACK_PPI_GUID \
  { \
    0x17865dc0, 0xb8b, 0x4da8, \
    { \
      0x8b, 0x42, 0x7c, 0x46, 0xb8, 0x5c, 0xca, 0x4d \
    } \
  }
#endif
//
// Extern the GUID for PPI users.
//
extern EFI_GUID                         gPchResetPpiGuid;
extern EFI_GUID                         gPchResetCallbackPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef PCH_RESET_PROTOCOL              PCH_RESET_PPI;

typedef PCH_RESET_CALLBACK_PROTOCOL     PCH_RESET_CALLBACK_PPI;

#endif
