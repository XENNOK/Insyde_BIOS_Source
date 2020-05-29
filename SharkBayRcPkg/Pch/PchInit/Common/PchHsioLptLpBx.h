/** @file

  Header file with all LPTLPBx Hsio information

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
  license agreement.

**/


#ifdef ULT_FLAG
#ifndef _PCH_HSIO_LPTLPBX_H_
#define _PCH_HSIO_LPTLPBX_H_

#define PCH_LPTLP_HSIO_VER_BX  0x19

extern UINT8 PchChipsetInitTableLptLp_Bx[229];
extern IOBP_MMIO_TABLE_STRUCT PchUsb3HsioLptLp_Bx[20];
extern IOBP_MMIO_TABLE_STRUCT PchUsb3SharedHsioLptLp_Bx[22];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptLp_Bx[73];
extern IOBP_MMIO_TABLE_STRUCT PchGbeSharedHsioLptLp_Bx[6];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptLp_MB_Bx[4];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptLp_DT_Bx[4];
extern IOBP_SATA_RXEQ_TABLE PchSataRxEqSharedHsioLptLp_Bx[20];

#endif //_PCH_HSIO_LPTLPBX_H_
#endif //ULT_FLAG

