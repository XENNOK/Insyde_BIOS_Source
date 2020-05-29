//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CacheControl.h

Abstract:

  CacheControl PPI defined in EFI 2.0

  This code abstracts Cache control (enable/disable)

--*/

#ifndef _PEI_CACHE_CONTROL_PPI_H_
#define _PEI_CACHE_CONTROL_PPI_H_

#define PEI_CACHE_CONTROL_PPI_GUID \
  { \
    0xb1561971, 0x8daa, 0x4725, 0xb5, 0x74, 0xfa, 0x9d, 0xab, 0x33, 0xbd, 0x59 \
  }

EFI_FORWARD_DECLARATION (PEI_CACHE_CONTROL_PPI);

//
// *******************************************************
// PEI_ENABLE_CACHE_PPI
// *******************************************************
//
typedef
EFI_STATUS
(EFIAPI *PEI_ENABLE_CACHE_PPI) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN PEI_CACHE_CONTROL_PPI              * This,
  IN  UINT8                             CacheLevel,
  IN  BOOLEAN                           Enable
  );

//
// *******************************************************
// PEI_CACHE_CONTROL_PPI
// *******************************************************
//
typedef struct _PEI_CACHE_CONTROL_PPI {
  PEI_ENABLE_CACHE_PPI  EnableCache;
} PEI_CACHE_CONTROL_PPI;

extern EFI_GUID gPeiCacheControlPpiGuid;

#endif
