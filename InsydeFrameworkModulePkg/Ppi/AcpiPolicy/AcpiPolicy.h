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
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
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

  AcpiPolicy.h

Abstract:

  ACPI Policy PPI as defined in EFI 2.0

--*/

#ifndef _PEI_ACPI_POLICY_PPI_H
#define _PEI_ACPI_POLICY_PPI_H

#define PEI_ACPI_POLICY_PPI_GUID \
  { \
    0x7a5a0543, 0xe3ac, 0x498a, 0xa9, 0x49, 0xb3, 0x62, 0x66, 0x3d, 0x1b, 0xbb \
  }

EFI_FORWARD_DECLARATION (PEI_ACPI_POLICY_PPI);

typedef struct _PEI_ACPI_POLICY_PPI {
  UINTN BaseAddress;
} PEI_ACPI_POLICY_PPI;

extern EFI_GUID gPeiAcpiPolicyPpiGuid;

#endif
