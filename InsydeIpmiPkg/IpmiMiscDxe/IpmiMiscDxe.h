/** @file
 H2O IPMI Misc module header file.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_MISC_DXE_H_
#define _IPMI_MISC_DXE_H_


#include <ServerProcessorManagementInterfaceTable.h>

#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Pci.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>
#include <Protocol/Smbios.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/LegacyBios.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/IpmiHobLib.h>
#include <Library/DxeIpmiPackageLib.h>
#include <Library/DxeOemIpmiPackageLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>

#define INVALID_BOOT_NUMBER  0xFFFF

#define BOOT_VAR_ATTR        (EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS|EFI_VARIABLE_NON_VOLATILE)

#define FixedPcdGetPtrSize(TokenName)        (UINTN)_PCD_PATCHABLE_##TokenName##_SIZE


#endif

