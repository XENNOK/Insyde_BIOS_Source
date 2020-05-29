/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _OEM_SPD_DATA_GUID_H_
#define _OEM_SPD_DATA_GUID_H_

#define OEM_SPD_DATA_GUID \
  { 0xa1e81f04, 0x2596, 0x42f9, 0xa5, 0xfe, 0x0c, 0x77, 0x17, 0x23, 0xb1, 0xbd}

//
// MAX SPD data size
//
#define MAX_SPD_DATA_SIZE    0x100 

//
// The Infomation of the SPD Data
//
typedef struct {
  UINT8  Channel;
  UINT8  Dimm;
  UINT8  SpdData[MAX_SPD_DATA_SIZE];
} CUSTOMER_SPD_DATA;

extern EFI_GUID gOemSpdDataGuid;

#endif
