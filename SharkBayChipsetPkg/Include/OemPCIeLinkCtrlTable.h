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

#ifndef _PCIE_LINK_CONTROL_TABLE_H_
#define _PCIE_LINK_CONTROL_TABLE_H_


#define PCIE_ASPM_DEV_END_OF_TABLE  0xFFFF

//
// Device List Structure
//
typedef struct _PCIE_ASPM_DEV_INFO {
  UINT16    VendorId;           // offset 0
  UINT16    DeviceId;           // offset 2
  UINT8     RevId;              // offset 8; 0xFF means all steppings
  UINT8     RootAspm;
  UINT8     EndpointAspm;
} PCIE_ASPM_DEV_INFO;

#endif
