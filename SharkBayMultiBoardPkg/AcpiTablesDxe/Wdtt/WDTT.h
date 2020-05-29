/*++

Copyright (c) 2007 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Wdtt.h

Abstract:

  This file describes the contents of the ACPI Watchdog Description
  Table (WDTT).  Some additional ACPI values are defined in Acpi1_0.h and
  Acpi2_0.h.

--*/

#ifndef _WDTT_H
#define _WDTT_H

//
// Statements that include other files
//

#include "AcpiHeaderDefaultValue.h"
#include "EfiCommon.h"
//#include "Platform/Features/HwWatchdogTimer/HwWatchdogCommon.h"
#define MAX_NUMBER_ACTION_ENTRIES          50
//
// ACPI 1.0 Table structure
//

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER     Header;
  UINT32                          WatchdogHeaderLength;
  UINT32                          Reserved1;                       // Not used!
  UINT8                           Reserved2;                       // Not used!
  UINT8                           Reserved3;                       // reserved space
  UINT8                           Reserved3a;                      // reserved space
  UINT8                           WD_OS_Support;                   // Watchdog Timer Support for OS Boot
  UINT32                          TimerPeriod;                     // Timer period (in milliseconds)
  UINT32                          MaximumCount;                    // Maximum value allowed (in counts)
  UINT32                          MinimumCount;                    // Minimum value allowed (in counts)
  UINT32                          Reserved4;                       // reserved space
  UINT32                          WatchdogInstructionEntryCount;   // # of entry counts
  UINT8                           ActionEntries[MAX_NUMBER_ACTION_ENTRIES]; // Reserves space for Action Entries
} EFI_IDCC2_WATCHDOG_DESCRIPTION_TABLE;

//
// WDTT Definitions
//
#define EFI_IDCC2_WDTT_SIGNATURE                       'TTDW'
#define EFI_HIDE_WDTT_SIGNATURE                        'xTDW'
#define EFI_IDCC2_WATCHDOG_DESCRIPTION_TABLE_REVISION  2
#define EFI_ACPI_OEM_WDTT_REVISION                     0x00000001
#define EFI_ACPI_WDTT_SPEC_VERSION                     0x0100
#define EFI_ACPI_WDTT_TABLE_VERSION                    0x0100


#pragma pack ()

#endif
