/** @file

  This file contains definitions required for creation of 
  Memory Info hob.
  
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

///
/// TODO: Delete #define's and use definitions from SaCommonDefitions.h
///
#ifndef _MEM_HOB_H_
#define _MEM_HOB_H_

#pragma pack (push, 1)
#include <MrcGlobal.h>

#define EFI_MEMORY_RESTORE_DATA_GUID \
  {0x87f22dcb,0x7304,0x4105,0xbb,0x7c,0x31,0x71,0x43,0xcc,0xc2,0x3b }

#define MAX_NODE        1
#define MAX_CH          2
#define MAX_DIMM        2

#define DDR3_FREQ_AUTO  0
#define DDR3_FREQ_800   1
#define DDR3_FREQ_1000  2
#define DDR3_FREQ_1067  3
#define DDR3_FREQ_1200  4
#define DDR3_FREQ_1333  5
#define DDR3_FREQ_1400  6
#define DDR3_FREQ_1600  7
#define DDR3_FREQ_1800  8
#define DDR3_FREQ_1867  9
#define DDR3_FREQ_2000  10
#define DDR3_FREQ_2133  11
#define DDR3_FREQ_2200  12
#define DDR3_FREQ_2400  13
#define DDR3_FREQ_2600  14
#define DDR3_FREQ_2667  15

///
/// Host reset states from MRC.
///
#define  WARM_BOOT        2

#define MCPciD4F0RegBase  PCIEX_BASE_ADDRESS + (UINT32) (((PCIEX_LENGTH >> 20) - 1) << 20) + (UINT32) (4 << 15) + (UINT32) (0 << 12)
#define MCPciD5F0RegBase  PCIEX_BASE_ADDRESS + (UINT32) (((PCIEX_LENGTH >> 20) - 1) << 20) + (UINT32) (5 << 15) + (UINT32) (0 << 12)

#define R_MC_CHNL_RANK_PRESENT  0x7C
#define   B_RANK0_PRS           BIT0
#define   B_RANK1_PRS           BIT1
#define   B_RANK2_PRS           BIT4
#define   B_RANK3_PRS           BIT5

#ifndef __PI_HOB_H__
#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  ///
  /// Guid specific data goes here
  ///
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif
///
/// HOB to save MRC Output data and Memory S3_RestoreData
///
#define MRC_HOB_SIZE_TOTAL  (63 * 1024)
#if ((defined SSA_FLAG) || (defined MRC_DEBUG_PRINT))
#define MRC_HOB_SIZE_BUFFER (MRC_HOB_SIZE_TOTAL - sizeof (EFI_HOB_GUID_TYPE) - sizeof (MrcParameters))
#else
#define MRC_HOB_SIZE_BUFFER (1)
#endif
typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  MrcParameters     MrcData;
  UINT8             Buffer[MRC_HOB_SIZE_BUFFER];
} HOB_SAVE_MEMORY_DATA;

#pragma pack (pop)

extern EFI_GUID gMemRestoreDataGuid;
#endif