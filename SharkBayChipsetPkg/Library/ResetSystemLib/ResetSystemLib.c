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

/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** 

Copyright (c) 2007 - 2008, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IdeBus.h

Abstract:

  System reset Library Services.  This library class provides a set of
  methods to reset whole system with manipulate ICH.

**/


#include <Base.h>


#include <Library/ResetSystemLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>

//
// Reset Generator I/O Port
//
#define R_PCH_RST_CNT                 0xCF9
#define B_PCH_RST_CNT_FULL_RST        BIT3
#define B_PCH_RST_CNT_RST_CPU         BIT2
#define B_PCH_RST_CNT_SYS_RST         BIT1
#define V_PCH_RST_CNT_FULLRESET       0x0E
#define V_PCH_RST_CNT_HARDRESET       0x06
#define V_PCH_RST_CNT_SOFTRESET       0x04
#define V_PCH_RST_CNT_HARDSTARTSTATE  0x02
#define V_PCH_RST_CNT_SOFTSTARTSTATE  0x00



/**
  Calling this function causes a system-wide reset. This sets
  all circuitry within the system to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  System reset should not return, if it returns, it means the system does
  not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE);
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
}

/**
  Calling this function causes a system-wide initialization. The processors
  are set to their initial state, and pending cycles are not corrupted.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTSTARTSTATE);
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTRESET);
}

/**
  Calling this function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  System shutdown should not return, if it returns, it means the system does
  not support shut down reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  ASSERT (FALSE);
}

/**
  Calling this function causes the system to enter a power state for capsule
  update.

  Reset update should not return, if it returns, it means the system does
  not support capsule update.

**/
VOID
EFIAPI
EnterS3WithImmediateWake (
  VOID
  )
{
  ASSERT (FALSE);
}

