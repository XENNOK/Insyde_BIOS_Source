/** @file
 H2O IPMI SEL Info module header file.

 This file contains common functions prototype, structure and definitions for
 H2O IPMI SEL Info module internal use.
 
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

#ifndef _IPMI_SEL_INFO_COMMON_H_
#define _IPMI_SEL_INFO_COMMON_H_

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiSdrLib.h>
#include <Library/BaseLib.h>

#include <Protocol/H2OIpmiSelInfoProtocol.h>


//
// Common Definitions
//
#define ALL_OFFSETS_SPECIFIED             0xff
#define DATA_BYTE2_SPECIFIED_MASK         0xc0
#define DATA_BYTE3_SPECIFIED_MASK         0x30
#define MAX_SENSOR_NUMBER                 0x100
#define MAX_SENSOR_GENDEATOR_ID_DEFINED   0x48
#define SEL_DEFAULT_SENSOR_NAME           L"Unknow Sensor"
#define MAX_VALUE_STRING_LEN              0x20
#define MAX_DESC_STRING_LEN               0x100
#define HAVE_TRIGGER_READING              0x40
#define HAVE_TRIGGER_THRESHOLD            0x10
#define DEFAULT_PRECISION                 0x03

#pragma pack(1)

enum {
  SENSOR_EVENT_CLASS_DISCRETE,
  SENSOR_EVENT_CLASS_DIGITAL,
  SENSOR_EVENT_CLASS_THRESHOLD,
  SENSOR_EVENT_CLASS_OEM,
} SENSOR_EVENT_CLASS;

//
// Sensor and Event Code Table Structure
//
typedef struct {
  UINT8   Code;
  UINT8   Offset;
  UINT8   Data;
  UINT8   Class;
  CHAR16  *Type;
  CHAR16  *Desc;
} SENSOR_EVENT_TYPE_ENTRY;

//
// Generator ID Structure
//
typedef struct {
  UINT8   Offset;
  UINT8   EndOffset;
  CHAR16  *Name;
} SEL_GENERATORID_OFFSET;

//
// Sensor Name and Unit Table Structure
//
typedef struct {
  CHAR16  *Name;
  CHAR16  *Unit;
}SENSOR_NAME_AND_UNIT_TABLE;

#pragma pack()

//
// Function prototype
//

/**
 Fill private data content.

*/
VOID
InitialIpmiSelInfoContent (
  VOID
  );


extern H2O_IPMI_SEL_INFO_PROTOCOL    mSelInfoProtocol;


#endif

