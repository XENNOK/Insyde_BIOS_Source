/** @file
 
  Header file with all common Hsio information
 
@copyright
  Copyright (c) 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
 
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 
**/

#ifndef _PCH_HSIO_H_
#define _PCH_HSIO_H_

#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <IobpDefinitions.h>
#ifdef TRAD_FLAG
#include "PchHSIOLptHB0.h"
#include "PchHsioLptHCx.h"
#endif //TRAD_FLAG
#ifdef ULT_FLAG
#include "PchHsioLptLpBx.h"
#endif //ULT_FLAG



#ifdef ULT_FLAG
extern IOBP_MMIO_TABLE_STRUCT PchSerialIoSnoopLptLp[8];
extern IOBP_MMIO_TABLE_STRUCT PchSerialIoIntsLptLp[7];
#endif // ULT_FLAG

#endif //_PCH_HSIO_H_