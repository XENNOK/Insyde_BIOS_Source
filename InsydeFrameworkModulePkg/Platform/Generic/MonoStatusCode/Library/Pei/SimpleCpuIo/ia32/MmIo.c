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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    MmIo.c

Abstract:

  Light weight monolithic PEI lib functions that implement MemRead ()
  and MemWrite ().

--*/

#include "Pei.h"
#include "SimpleCpuIoLib.h"

UINT32
MemRead32 (
  IN UINT64     Address
  )
/*++

Routine Description:
  Do a four byte Memory mapped IO read

Arguments:
  Address - Memory mapped IO address to read

Returns:
  Data read

--*/
{
  UINT32  Buffer;
  UINT32  *DataAddr;

  DataAddr = (UINT32 *) ((UINT32) Address);
  MEMORY_FENCE ();
  Buffer = *DataAddr;
  MEMORY_FENCE ();

  return Buffer;

}

UINT64
MemRead64 (
  IN UINT64     Address
  )
/*++

Routine Description:
  Do a eight byte Memory mapped IO read

Arguments:
  Address - Memory mapped IO address to read

Returns:
  Data read

--*/
{
  return 0;
}

VOID
MemWrite32 (
  IN UINT64     Address,
  IN UINT32     Data
  )
/*++

Routine Description:
  Do a four byte Memory mapped IO write

Arguments:
  Address - Memory mapped IO address to write
  Data    - Data to write to Address

Returns:
  NONE

--*/
{
  UINT32  *DataAddr;

  DataAddr = (UINT32 *) ((UINT32) Address);
  MEMORY_FENCE ();
  *DataAddr = Data;
  MEMORY_FENCE ();

  return ;
}

VOID
MemWrite64 (
  IN UINT64     Address,
  IN UINT64     Data
  )
/*++

Routine Description:
  Do a eight byte Memory mapped IO write

Arguments:
  Address - Memory mapped IO address to write
  Data    - Data to write to Address

Returns:
  NONE

--*/
{
  return ;
}
