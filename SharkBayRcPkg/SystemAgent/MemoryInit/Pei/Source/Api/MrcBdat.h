/** @file

  Copies the memory related timing and configuration information into the
  Compatible BIOS data (BDAT) table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MRC_BDAT_H_
#define _MRC_BDAT_H_

#pragma pack(push, 1)

#include <MrcTypes.h>
#include <MrcApi.h>

/**
  Fill the compatible data structure RMT with the information provided by
  the memory initialization code.

  @param[in, out] MrcData - Constant pointer to the Mrc data structure which conatins the Rmt structure to fill.

  @retval Nothing.
*/
extern
MrcStatus
MrcFillRmtStructure (
  IN OUT MrcParameters *const MrcData
  );

#pragma pack(pop)
#endif // _MrcBdat_h_
