/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
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
/*++

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IchRegTable.c

Abstract:

  Register initialization table for Pch.

Revision History

--*/
//[-start-120808-IB10820098-modify]//
#include <Platform.h>
#include <SaAccess.h>
#include <PchAccess.h>
//[-end-120808-IB10820098-modify]//

EFI_REG_TABLE gPchRegs [] = {

  //
  // All devices on bus 0.
  // Device 27:
  //    FNC 0: Azalia
  // Device 28: PCIe
  //    FNC 0: PCIe port 0
  //    FNC 1: PCIe port 1
  //    FNC 2: PCIe port 2
  //    FNC 3: PCIe port 3
  // Device 29: USB
  //    FNC 0: UHCI0
  //    FNC 1: UHCI1
  //    FNC 2: UHCI2
  //    FNC 3: UHCI3
  //    FNC 7: EHCI
  // Device 30:
  //    FNC 0: Hub-PCI bridge
  //    FNC 2: AC'97 Audio
  //    FNC 3: AC'97 Modem
  // Device 31:
  //    FNC 0: PCI-LPC Bridge
  //    FNC 1: IDE
  //    FNC 2: SATA
  //    FNC 3: SMBus controller
  //


  //
  // FWH Decode Enable
  //

  //
  // GPI ROUT
  //  GPI13 -> SMC_WAKE_SCI#
  //  GPI8  -> SMC_EXTSMI#
  //  GPI7  -> SMC_RUNTIME_SCI#
  //  GPI3  -> SATA_ODD_DA#
  //
  PCI_WRITE (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    PCI_FUNCTION_NUMBER_PCH_LPC,
    R_PCH_LPC_GPI_ROUT,
    EfiPciWidthUint32,
    PCH_LPC_GPI_ROUT,
    OPCODE_FLAG_S3SAVE
    ),

  TERMINATE_TABLE

};

