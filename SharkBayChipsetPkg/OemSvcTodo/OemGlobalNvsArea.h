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

#ifndef _OEM_GLOBAL_NVS_AREA_H_
#define _OEM_GLOBAL_NVS_AREA_H_

//
// OEM Global NVS Area definition
//
#pragma pack (1)
typedef struct {

  //
  // The definitions below need to be matched OGNS definitions in OemGlobalNvs.ASL
  // and can be modified by OEM
  //
  UINT8       OemGlobalNvsData00;    // 00
  UINT8       OemGlobalNvsData01;    // 01
  UINT8       OemGlobalNvsData02;    // 02
  UINT8       OemGlobalNvsData03;    // 03
  UINT8       OemGlobalNvsData04;    // 04
  UINT8       OemGlobalNvsData05;    // 05
  UINT8       OemGlobalNvsData06;    // 06
  UINT8       OemGlobalNvsData07;    // 07
  UINT8       OemGlobalNvsData08;    // 08
  UINT8       OemGlobalNvsData09;    // 09
  UINT8       OemGlobalNvsData10;    // 10
  UINT8       OemEmbeddedControllerFlag;   
} EFI_OEM_GLOBAL_NVS_AREA;
#pragma pack ()

#endif
