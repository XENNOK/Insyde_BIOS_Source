//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; ModuleName:
//;
//;   AcpiDxePolicyInit.h
//;
//;------------------------------------------------------------------------------
//;
//; Abstract:
//;
//;   Header file for the AcpiDxePolicyInit Driver.
//;

#ifndef _ACPI_DXE_PLATFORM_POLICY_H_
#define _ACPI_DXE_PLATFORM_POLICY_H_
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
//[-start-121219-IB10820205-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121219-IB10820205-add]//
#include <Protocol/AcpiPlatformPolicy.h>
#include <Protocol/AcpiInfo.h>
#include <Protocol/SetupUtility.h>
#include <ChipsetSetupConfig.h>

#endif
