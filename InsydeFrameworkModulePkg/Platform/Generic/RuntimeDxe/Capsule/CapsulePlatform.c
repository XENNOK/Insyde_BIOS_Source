//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Capsule.c

Abstract:

  Capsule Runtime Service Initialization

--*/


#include "Tiano.h"


//
//Max size capsule services support are platform policy,to populate capsules we just need
//memory to maintain them across reset,it is not a problem. And to special capsules ,for
//example,update flash,it is mostly decided by the platform. Here is a sample size for
//different type capsules.
//
#define MAX_SIZE_POPULATE              (100*1024*1024)
#define MAX_SIZE_NON_POPULATE          (10*1024*1024)




BOOLEAN
EFIAPI
SupportUpdateCapsuleRest (
  VOID
  )
/*++

Routine Description:

  This function returns if the platform supports update capsule across a system reset.

Arguments:

Returns:

--*/
{
  //
  //If the platform has a way to guarantee the memory integrity across a system reset, return
  //TRUE, else FALSE.
  //
  return TRUE;
}



VOID
EFIAPI
SupportCapsuleSize (
  IN OUT UINT32 *MaxSizePopulate,
  IN OUT UINT32 *MaxSizeNonPopulate
  )
/*++

Routine Description:

  This code returns the max size capsule the platform supports.

Arguments:

Returns:


--*/

{
  //
  //Here is a sample size, different platforms have different sizes.
  //
  *MaxSizePopulate    = MAX_SIZE_POPULATE;
  *MaxSizeNonPopulate = MAX_SIZE_NON_POPULATE;
  return;
}



