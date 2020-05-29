//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************


#ifndef _ACPI_WATCH_DOG_H_
#define _ACPI_WATCH_DOG_H_

#include <PchAccess.h>
#include <PchRegsLpc.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Include/ChipsetCmos.h>
#include <AcpiWatchDog/WatchdogActionTable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/XtuPolicy/XtuPolicy.h>

#define EFI_ACPI_OEM_ID               'I','N','S','Y','D','E' // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID         SIGNATURE_64('I','N','S','Y','D','E',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION         0x00000001
#define EFI_ACPI_CREATOR_ID           SIGNATURE_32('M','S','F','T')
#define EFI_ACPI_CREATOR_REVISION     0x01000013

#define WATCHDOG_DEFAULT_STATUS_VALUE 0x55

#endif
