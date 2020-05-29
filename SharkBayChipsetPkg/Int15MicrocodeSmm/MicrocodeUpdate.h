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
//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** 
  Header file for MCU SMI handlers.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _MICROCODE_UPDATE_H_
#define _MICROCODE_UPDATE_H_

#define INT15_0D042H                          0x44

#define EFI_CPUID_VERSION_INFO                0x1

#define EFI_MSR_IA32_PLATFORM_ID              0x17
#define EFI_MSR_IA32_BIOS_UPDT_TRIG           0x79
#define EFI_MSR_IA32_BIOS_SIGN_ID             0x8B

#endif
