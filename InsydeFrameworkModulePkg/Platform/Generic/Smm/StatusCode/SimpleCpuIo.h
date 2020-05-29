//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
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

  SimpleCpuIo.h

Abstract:

  Light weight monolithic Cpu Io Lib to support PEI Modules.

--*/

#ifndef _SIMPLE_CPU_IO_H_
#define _SIMPLE_CPU_IO_H_

//
//  Base IO Class Functions
//
UINT8
IoRead8 (
  IN  UINT64  Address
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Address - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

/*++
Routine Description:
  I/O read data from port address
Arguments:
   Address: - Port address
Returns:
   Return the 8 bit data value
--*/
VOID
IoWrite8 (
  IN  UINT64  Address,
  IN  UINT8   Data
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Address - GC_TODO: add argument description
  Data    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

/*++
Routine Description:
  I/O write 8bit data to port address
Arguments:
   Address: - Port address
   Data:    - Data to write
Returns:
   None
--*/
#endif
