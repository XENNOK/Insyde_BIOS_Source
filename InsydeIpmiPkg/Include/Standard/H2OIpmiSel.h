/** @file
 H2O IPMI SEL header file.

 This files contains SEL related definitions and structures defined in IPMI
 specification.

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

#ifndef _H2O_IPMI_SEL_H_
#define _H2O_IPMI_SEL_H_


#pragma pack(1)

//
// SEL Device Command
//
#define H2O_IPMI_CMD_GET_SEL_INFO                0x40
#define H2O_IPMI_CMD_GET_SEL_ALLOCTION_INFO      0x41
#define H2O_IPMI_CMD_RESERVE_SEL_ENTRY           0x42
#define H2O_IPMI_CMD_GET_SEL_ENTRY               0x43
#define H2O_IPMI_CMD_ADD_SEL_ENTRY               0x44
#define H2O_IPMI_CMD_PARTIAL_ADD_SEL_ENTRY       0x45
#define H2O_IPMI_CMD_DELETE_SEL_ENTRY            0x46
#define H2O_IPMI_CMD_CLEAR_SEL_ENTRY             0x47
#define H2O_IPMI_CMD_GET_SEL_TIME                0x48
#define H2O_IPMI_CMD_SET_SEL_TIME                0x49

//
// Structure to store System Event Log information
//
typedef struct {
  UINT8   SelVer;
  UINT16  SelEntries;
  UINT16  FreeSpace;
  UINT32  RecentAdditionTimeStamp;
  UINT32  RecentEraseTimeStamp;
  UINT8   OperationSupport;
} H2O_IPMI_SEL_INFO;

//
// Standard System Event Log Structure
//
typedef struct {
  UINT16  RecordId;
  UINT8   RecordType;
  UINT32  TimeStamp;
  UINT16  GeneratorId;
  UINT8   EvMRev;
  UINT8   SensorType;
  UINT8   SensorNum;
  UINT8   EventType :7;
  UINT8   EventDir  :1;
  UINT8   EventData1;
  UINT8   EventData2;
  UINT8   EventData3;
} H2O_IPMI_SEL_DATA;

//
// Structure to store System Event Log entry
//
typedef struct {
  UINT16             NextRecordId;
  H2O_IPMI_SEL_DATA  Data;
} H2O_IPMI_SEL_ENTRY;

#pragma pack()


#endif

