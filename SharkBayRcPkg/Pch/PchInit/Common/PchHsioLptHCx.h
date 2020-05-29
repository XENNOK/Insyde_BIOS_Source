/** @file
  Header file with all LPTHCx Hsio information

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
  license agreement.

**/


#ifdef TRAD_FLAG
#ifndef _PCH_HSIO_LPTHCX_H_
#define _PCH_HSIO_LPTHCX_H_

#define PCH_LPTH_HSIO_VER_CX  0x2C

extern UINT8 PchChipsetInitTableLptH_Cx[292];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptH_Cx[36];
extern IOBP_MMIO_TABLE_STRUCT PchSataHsioLptH_Cx[73];
extern IOBP_MMIO_TABLE_STRUCT PchUsb3SharedHsioLptH_Cx[20];
extern IOBP_MMIO_TABLE_STRUCT PchUsb3HsioLptH_Cx[36];
extern IOBP_MMIO_TABLE_STRUCT PchGbeSharedHsioLptH_Cx[8];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptH_MB_Cx[2];
extern IOBP_MMIO_TABLE_STRUCT PchSataHsioLptH_MB_Cx[4];
extern IOBP_MMIO_TABLE_STRUCT PchSataSharedHsioLptH_DT_Cx[2];
extern IOBP_MMIO_TABLE_STRUCT PchSataHsioLptH_DT_Cx[4];
extern IOBP_SATA_RXEQ_TABLE PchSataRxEqSharedHsioLptH_Cx[10];
extern IOBP_SATA_RXEQ_TABLE PchSataRxEqHsioLptH_Cx[20];


#endif //_PCH_HSIO_LPTHCX_H_
#endif //TRAD_FLAG

