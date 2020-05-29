/** @file
  
  GUIDs used for SMRAM CPU DATA Header signature
  
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

#ifndef _SMRAM_CPU_DATA_HEADER_H_
#define _SMRAM_CPU_DATA_HEADER_H_

///
/// SMRAM CPU DATA Header for search by CpuS3Peim
///
#define SMRAM_CPU_DATA_HEADER_GUID \
  { 0x5848fd2d, 0xd6af, 0x474b, { 0x82, 0x75, 0x95, 0xdd, 0xe7, 0x0a, 0xe8, 0x23 } }

extern EFI_GUID gSmramCpuDataHeaderGuid;

#endif
