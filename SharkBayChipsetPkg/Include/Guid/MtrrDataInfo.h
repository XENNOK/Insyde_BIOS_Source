/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef MTRR_DATA_INFO_H_
#define MTRR_DATA_INFO_H_

#define MTRR_DATA_INFO \
  { \
    0x15c57af9, 0x1db3, 0x4cb7, 0x81, 0x2b, 0x71, 0xC2, 0x9B, 0xc3, 0x80, 0x37 \
  }
//
// {15c57af9-1db3-4cb7-812b-71c29bc38037}
//

typedef struct {
	UINT64              TotalMemorySize;	            
	UINT64              MtrrBase[20];	            
	UINT64              MtrrSize[20];	            		
} HOB_MTRR_RESTORE_DATA;

  
typedef struct {
	HOB_MTRR_RESTORE_DATA MtrrData; 		
} HOB_MTRR_RESTORE_INFO;

extern EFI_GUID gMtrrDataInfoGuid;

#endif
