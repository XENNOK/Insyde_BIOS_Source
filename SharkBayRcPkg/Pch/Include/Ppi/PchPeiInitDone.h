/** @file
  PCH Init Done PPI

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
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchPeiInitDone.h

@brief
  PCH Init Done PPI

**/

#ifndef _PCH_PEI_INIT_DONE_PPI_H_
#define _PCH_PEI_INIT_DONE_PPI_H_

///
/// GUID for the PCH PEI Init Done PPI
///
#define PCH_PEI_INIT_DONE_PPI_GUID \
  { \
    0x1edcbdf9, 0xffc6, 0x4bd4, \
    { \
      0x94, 0xf6, 0x19, 0x5d, 0x1d, 0xe1, 0x70, 0x56 \
    } \
  }
//
// Extern the GUID for PPI users.
//
extern EFI_GUID gPchPeiInitDonePpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_PEI_INIT_DONE_PPI PCH_PEI_INIT_DONE_PPI;

#endif
