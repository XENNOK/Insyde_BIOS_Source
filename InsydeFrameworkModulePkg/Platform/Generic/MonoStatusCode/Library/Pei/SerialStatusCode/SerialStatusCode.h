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

  SerialStatusCode.h

Abstract:

  Lib to provide Serial I/O status code routines

--*/

#ifndef _PEI_SERIAL_STATUS_CODE_H_
#define _PEI_SERIAL_STATUS_CODE_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "SimpleCpuIoLib.h"
#include "SerialStatusCodeLib.h"
#include "EfiCommonLib.h"

//
// GUID consumed
//
#include EFI_GUID_DEFINITION (StatusCodeDataTypeId)

//
// ---------------------------------------------
// UART Register Offsets
// ---------------------------------------------
//
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

//
// ---------------------------------------------
// UART Register Bit Defines
// ---------------------------------------------
//
#define LSR_TXRDY 0x20
#define LSR_RXDA  0x01
#define DLAB      0x01

//
// Globals for Serial Port settings
//
extern UINT16 gComBase;
extern UINTN  gBps;
extern UINT8  gData;
extern UINT8  gStop;
extern UINT8  gParity;
extern UINT8  gBreakSet;

VOID
DebugSerialPrint (
  IN UINT8    *OutputString
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  OutputString  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

VOID
DebugSerialWrite (
  IN UINT8  Character
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Character - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
