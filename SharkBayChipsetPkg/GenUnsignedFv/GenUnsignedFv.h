//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; Revision History:
//;
//; $Log: 
//; 
//;------------------------------------------------------------------------------

#ifndef _EFI__GEN_UNSIGNED_FV_H_
#define _EFI__GEN_UNSIGNED_FV_H_

#define STATUS_SUCCESS           0
#define STATUS_NOT_FOUND         1
#define STATUS_INVALID_PARAMETER 2
#define STATUS_MALLOC_FAIL       3

#define STATUS_ERROR(a) (((UINT8) (a) != STATUS_SUCCESS) ? (a) : STATUS_SUCCESS)

#pragma pack(1)

typedef struct _FLASH_AREA_INFO {
  UINT32  FlashAreaBaseAddress;
  UINT32  FlashAreaSize;  
  UINT32  FlashFvMainBase;
  UINT32  FlashFvMainSize;
  UINT32  FlashUnsignedFvRegionBase;
  UINT32  FlashUnsignedFvRegionSize;
  UINT32  EndOfFlashUnsignedFvRegionBase;
  UINT32  EndOfFlashUnsignedFvRegionTotalSize;
} FLASH_AREA_INFO;  

#pragma pack()

#endif
