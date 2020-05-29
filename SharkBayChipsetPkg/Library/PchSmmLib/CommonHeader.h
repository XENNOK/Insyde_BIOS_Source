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

/**
  Common header file shared by all source files.

  This file includes package header files, library classes and protocol, PPI & GUID definitions.

  Copyright (c) 2007, Intel Corporation.
  All rights reserved.
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.
**/

#ifndef __COMMON_HEADER_H_
#define __COMMON_HEADER_H_


#include <Base.h>

#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>

#define BOOT_SERVICE_SOFTWARE_SMI_DATA          0
#define RUNTIME_SOFTWARE_SMI_DATA               1
#define LPC_BUS                 0
#define LPC_DEVICE              31
#define LPC_FUNCTION            0
#define R_ACPI_PM_BASE          0x40
#define ACPI_PM_BASE_MASK       0xFFF8
#define R_PCH_ACPI_PM1_TMR      0x08
#define R_PCH_ACPI_SMI_EN       0x30
#define B_PCH_ACPI_APMC_EN      0x00000020
#define B_PCH_ACPI_EOS          0x00000002
#define B_PCH_ACPI_GBL_SMI_EN   0x00000001
#define R_PCH_ACPI_SMI_STS      0x34
#define B_PCH_ACPI_APM_STS      0x00000020
#define R_PCH_SMM_DATA_PORT     0xb3
#define R_PCH_SMM_ACTIVATE_PORT 0xb2

#define BASE_TICKS              (3.579545)


#endif
