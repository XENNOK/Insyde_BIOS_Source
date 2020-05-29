//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  HtBistHob.h
    
Abstract:

  GUID used for HT BIST Status HOB entries in the HOB list.

**/

#ifndef _HT_BIST_HOB_GUID_H_
#define _HT_BIST_HOB_GUID_H_

#define EFI_HT_BIST_HOB_GUID \
  { \
    0xbe644001, 0xe7d4, 0x48b1, {0xb0, 0x96, 0x8b, 0xa0, 0x47, 0xbc, 0x7a, 0xe7 } \
  }

extern EFI_GUID gEfiHtBistHobGuid;

#endif
