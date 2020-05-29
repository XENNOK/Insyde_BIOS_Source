/** @file
  General IOBP data structure and register definitions.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
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
#ifndef _IOBP_DEFINITIONS_H_
#define _IOBP_DEFINITIONS_H_

#include <Library/PchPlatformLib.h>

#define IOBP_ADDR(portid, type, lane, block, offset) \
    ((UINT32) (((portid) << 24) + ((type) << 14) + ((lane) << 8) + (block << 6) + offset))

#define IOBP_PLP_ADDR(portid, type, lane, offset) \
    ((UINT32) (((portid) << 24) + ((type) << 14) + ((lane) << 8) + offset))

#define PCH_SATA_RXEQ_ID(port, genspeed) \
    ((UINT32) (((port) << 8) + genspeed))

typedef struct _IOBP_MMIO_TABLE_STRUCT {
  UINT32      Address;
  UINT32      AndMask;
  UINT32      OrMask;
} IOBP_MMIO_TABLE_STRUCT;

typedef struct IOBP_MMIO_ADDRESS_STRUCT {
  UINT32      Address;
} IOBP_MMIO_ADDRESS;

typedef struct _IOBP_SATA_TRACE_TABLE {
  UINT32     TraceId;
  UINT32     Address;
  UINT32     AndMask;
} IOBP_SATA_RXEQ_TABLE;

#endif
