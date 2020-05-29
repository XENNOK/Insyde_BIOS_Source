/** @file
  Intializes all common Hsio structures

@copyright
  Copyright (c) 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

#include "PchHSIO.h"

#ifdef ULT_FLAG

IOBP_MMIO_TABLE_STRUCT PchSerialIoSnoopLptLp[] = {
  { 0xCB000240, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000248, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000250, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000258, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000260, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000268, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000270, (UINT32)~(0x000C0000), 0x00040000 },
  { 0xCB000014, (UINT32)~(0x00006000), 0x00002000 }
};

IOBP_MMIO_TABLE_STRUCT PchSerialIoIntsLptLp[] = {     // Device   INTx  PCI IRQ   ACPI IRQ
  { 0xCB000240, (UINT32)~(0x0000003C), 0x00000008 },  // D21:F0 = INTB  IRQ20     IRQ6    
  { 0xCB000248, (UINT32)~(0x0000003C), 0x0000000C },  // D21:F1 = INTC  IRQ21     IRQ7
  { 0xCB000250, (UINT32)~(0x0000003C), 0x0000000C },  // D21:F2 = INTC  IRQ21     IRQ7
  { 0xCB000258, (UINT32)~(0x0000003C), 0x0000000C },  // D21:F3 = INTC  IRQ21     IRQ7
  { 0xCB000260, (UINT32)~(0x0000003C), 0x0000000C },  // D21:F4 = INTC  IRQ21     IRQ7
  { 0xCB000268, (UINT32)~(0x0000003C), 0x00000010 },  // D21:F5 = INTD  IRQ21     IRQ13
  { 0xCB000270, (UINT32)~(0x0000003C), 0x00000010 }   // D21:F6 = INTD  IRQ21     IRQ13
};                                                    // D23:F0 = INTA  IRQ22     IRQ5

#endif // ULT_FLAG