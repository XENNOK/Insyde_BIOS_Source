/** @file
  Platform Level Data Model for SMBIOS Data Transfer

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

#ifndef _DASH_PLDM_SMBIOS_H_
#define _DASH_PLDM_SMBIOS_H_

#include "Pldm.h"


#pragma pack (1)


typedef struct {
  UINT32             NextDataTransferHandle;
  PLDM_TRANSFER_FLAG TransferFlag;
  UINT8              SmBiosTableData[1];
} PLDM_SMBIOS_STRUCTURE_TABLE;

#pragma pack ()

#endif