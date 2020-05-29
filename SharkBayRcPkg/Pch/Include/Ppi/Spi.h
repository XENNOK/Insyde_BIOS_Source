/** @file
  This file defines the EFI SPI PPI which implements the Intel(R) PCH SPI Host Controller Compatibility Interface.

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

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2006 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Spi.h

@brief
  This file defines the EFI SPI PPI which implements the
  Intel(R) PCH SPI Host Controller Compatibility Interface.

**/
#ifndef _PEI_SPI_H_
#define _PEI_SPI_H_

#include <Protocol/Spi.h>

///
/// Define the SPI PPI GUID
///
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define PEI_SPI_PPI_GUID \
  { \
    0xfbf26154, 0x4e55, 0x4bdc, 0xaf, 0x7b, 0xd9, 0x18, 0xac, 0x44, 0x3f, 0x61 \
  }
#else
#define PEI_SPI_PPI_GUID \
  { \
    0xfbf26154, 0x4e55, 0x4bdc, \
    { \
      0xaf, 0x7b, 0xd9, 0x18, 0xac, 0x44, 0x3f, 0x61 \
    } \
  }
#endif
//
// Extern the GUID for PPI users.
//
extern EFI_GUID           gPeiSpiPpiGuid;

///
/// Reuse the EFI_SPI_PROTOCOL definitions
/// This is possible becaues the PPI implementation does not rely on a PeiService pointer,
/// as it uses EDKII Glue Lib to do IO accesses
///
typedef EFI_SPI_PROTOCOL  PEI_SPI_PPI;

#endif
