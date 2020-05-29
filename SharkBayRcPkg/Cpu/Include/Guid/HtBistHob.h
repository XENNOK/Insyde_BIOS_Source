/** @file

  GUID used for HT BIST Status HOB entries in the HOB list.         

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

#ifndef _HT_BIST_HOB_GUID_H_
#define _HT_BIST_HOB_GUID_H_

#define EFI_HT_BIST_HOB_GUID \
  { 0xbe644001, 0xe7d4, 0x48b1, { 0xb0, 0x96, 0x8b, 0xa0, 0x47, 0xbc, 0x7a, 0xe7 } }

extern EFI_GUID gEfiHtBistHobGuid;

#endif
