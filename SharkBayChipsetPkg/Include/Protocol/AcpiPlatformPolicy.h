/** @file
  Interface definition details between ACPI and platform drivers during DXE phase.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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

#ifndef _ACPI_PLATFORM_POLICY_H_
#define _ACPI_PLATFORM_POLICY_H_

///
///  ACPI policy provided by platform for DXE phase
///
#define ACPI_PLATFORM_POLICY_PROTOCOL_GUID \
     { 0xe9a6ca5a, 0x85bc, 0x4840, { 0x87, 0x84, 0x94, 0xcf, 0xc9, 0xed, 0x67, 0x66 } }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gAcpiPlatformPolicyProtocolGuid;

///
/// Forward reference for ANSI C compatibility
///
typedef struct _ACPI_PLATFORM_POLICY_PROTOCOL ACPI_PLATFORM_POLICY_PROTOCOL;

///
/// Protocol revision number
///
#define ACPI_PLATFORM_POLICY_PROTOCOL_REVISION_1  1

///
/// ACPI DXE Platform Policy
///
struct _ACPI_PLATFORM_POLICY_PROTOCOL {
  UINT8                   Revision;
  UINT8                   BoardId;
  UINT8                   EnableDptf;            ///< 0=Disable; 1=Enable; DEFAULT=1
  UINT8                   EnableCppc;             ///< 0=Disable; 1=Enable; DEFAULT=1
  UINT8                   EnableCppcPlatformSCI;  ///< 0=Disable; 1=Enable; DEFAULT=1
};

#endif
