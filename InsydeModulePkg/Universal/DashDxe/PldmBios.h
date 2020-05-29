/** @file
  Platform Level Data Model for BIOS Control and Configuration

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

#ifndef _DASH_PLDM_BIOS_H_
#define _DASH_PLDM_BIOS_H_

#include "Pldm.h"


#pragma pack (1)

typedef struct {
  UINT32 TransferHandle;
  UINT8  TransferFlag;
  UINT8  TableType;
} PLDM_BIOS_TABLE_TRANSFER_HEADER;

typedef struct {
    UINT32 TransferHandle;
    UINT8  TransferFlag;
    UINT16 AttrHandle;
  } PLDM_BIOS_ATTR_TRANSFER_HEADER;

typedef struct {
  UINT32 TransferHandle;
  UINT8  TransferFlag;
  UINT8  AttrType;
} PLDM_BIOS_ATTR_TYPE_TRANSFER_HEADER;

#pragma pack ()

#endif
