/** @file
 H2O IPMI event header file.

 This files contains event related definitions and structures defined in IPMI
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

#ifndef _H2O_IPMI_EVENT_H_
#define _H2O_IPMI_EVENT_H_


#pragma pack(1)

//
// Event Command
//
#define H2O_IPMI_CMD_SET_EVENT_RECEIVER     0x00
#define H2O_IPMI_CMD_GET_EVENT_RECEIVER     0x01
#define H2O_IPMI_CMD_EVENT_MESSAGE          0x02

//
//  Structure to "Platform Event" (a.k.a. Event Message)
//
typedef struct {
  UINT8 Reserved    :1;
  UINT8 SoftwareID  :7;
  UINT8 EvMRev;
  UINT8 SensorType;
  UINT8 SensorNumber;
  UINT8 EventDirType;
  UINT8 EventData1;
  UINT8 EventData2;
  UINT8 EventData3;
} H2O_IPMI_EVENT_MESSAGE;

#pragma pack()


#endif

