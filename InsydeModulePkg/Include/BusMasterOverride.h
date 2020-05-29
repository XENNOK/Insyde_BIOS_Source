/** @file
  Define Bus Master Override structure.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BUS_MASTER_OVERRIDE_H_
#define _BUS_MASTER_OVERRIDE_H_

#pragma pack (1)
typedef struct _BUS_MASTER_OVERRIDE_DATA{
  UINT16        VendorID;
  UINT16        DeviceID;
  UINT8         BMSwitch;
} BUS_MASTER_OVERRIDE; 
#pragma pack ()

#endif

