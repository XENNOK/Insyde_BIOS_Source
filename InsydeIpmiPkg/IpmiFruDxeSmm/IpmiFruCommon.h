/** @file
 H2O IPMI FRU module header file.

 This file contains common functions prototype, structure and definitions for
 H2O IPMI FRU module internal use.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_FRU_COMMON_H_
#define _IPMI_FRU_COMMON_H_


#include <Protocol/H2OIpmiFruProtocol.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Library/IpmiCommonLib.h>


//
// MFG Time use
//
#define SECS_FROM_1970_1996                       820454400

//
// Area definitions
//
#define H2O_IPMI_FRU_CHASSIS_START_OFFSET         0x3
#define H2O_IPMI_FRU_BOARD_START_OFFSET           0x6
#define H2O_IPMI_FRU_PRODUCT_START_OFFSET         0x3
#define H2O_IPMI_FRU_AREA_END_LENGTH_BYTE         0xC1
#define H2O_IPMI_FRU_CHASSIS_TYPE_OFFSET          0x2
#define H2O_IPMI_FRU_BOARD_MFG_TIME_OFFSET        0x3
#define H2O_IPMI_FRU_CHASSIS_TYPE_LEN             0x1
#define H2O_IPMI_FRU_BOARD_MFG_TIME_LEN           0x3

//
// Encode definitions
//
#define H2O_IPMI_FRU_TYPE_BINARY                  0x00
#define H2O_IPMI_FRU_TYPE_BCD_PLUS                0x01
#define H2O_IPMI_FRU_TYPE_6BIT_ASCII              0x02
#define H2O_IPMI_FRU_TYPE_DEPEND_ON_LANG_CODE     0x03


#pragma pack(1)

//
// FRU Common Header
//
typedef struct {
  UINT8  Version  :4;
  UINT8  Reserved :4;
  UINT8  InternalAreaOffset;
  UINT8  ChassisAreaOffset;
  UINT8  BoardAreaOffset;
  UINT8  ProductAreaOffset;
  UINT8  MultiRecordAreaOffset;
  UINT8  Pad;
  UINT8  Checksum;
} H2O_IPMI_FRU_COMMON_HEADER;

#pragma pack()


/**
 Fill private data content

*/
VOID
InitialIpmiFruContent (
  VOID
  );


extern H2O_IPMI_FRU_PROTOCOL   mFruProtocol;


#endif

