/** @file
  
  This file contains definitions required for creation of TXT Info HOB.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _TXT_HOB_H_
#define _TXT_HOB_H_

#define TXT_INFO_HOB_GUID \
  { 0x2986883F, 0x88E0, 0x48d0, { 0x4B, 0x82, 0x20, 0xC2, 0x69, 0x48, 0xDD, 0xAC } }

extern EFI_GUID gTxtInfoHobGuid;

#pragma pack(push, 1)


typedef struct {
  BOOLEAN               ChipsetIsTxtCapable;
  UINT8                 TxtMode;
  UINT64                PmBase;
  UINT64                SinitMemorySize;
  UINT64                TxtHeapMemorySize;
  EFI_PHYSICAL_ADDRESS  TxtDprMemoryBase;
  UINT64                TxtDprMemorySize;
  EFI_PHYSICAL_ADDRESS  BiosAcmBase;
  UINT64                BiosAcmSize;
  EFI_PHYSICAL_ADDRESS  McuUpdateDataAddr;
  EFI_PHYSICAL_ADDRESS  SinitAcmBase;
  UINT64                SinitAcmSize;
  UINT64                TgaSize;
  EFI_PHYSICAL_ADDRESS  TxtLcpPdBase;
  UINT64                TxtLcpPdSize;
  UINT64                Flags;
} TXT_INFO_DATA;

#define FLAGS0                  0x1
#define TXT_CPU_RESET_REQUIRED  0x2
#define TPM_INIT_FAILED         0x4

#define CMOS_INDEX_PORT         0x70
#define CMOS_DATA_PORT          0x71

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  TXT_INFO_DATA     Data;
} TXT_INFO_HOB;
#pragma pack(pop)

#endif
