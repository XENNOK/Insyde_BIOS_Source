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

#ifndef _HPET_H_
#define _HPET_H_

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

//
// HPET Definitions
//



#define EFI_ACPI_EVENT_TIMER_BLOCK_ID                   0x8086A201

//
// Event Timer Block Base Address Information
//
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS_SPACE_ID     EFI_ACPI_3_0_SYSTEM_MEMORY
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_WIDTH            0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_OFFSET           0x00
#define EFI_ACPI_EVENT_TIMER_ACCESS_SIZE                0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS              0x00000000FED00000

#define EFI_ACPI_HPET_NUMBER                            0x00

#define EFI_ACPI_MIN_CLOCK_TICK                         0x0080

#define EFI_ACPI_HPET_ATTRIBUTES                        0x00

#endif
