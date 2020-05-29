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

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  acpiSlicTable.h

Abstract:

  ACPI slic Table as defined in OEM Activation 2.0 Whitepaper


--*/

#ifndef _ACPI_SLIC_TABLE_H_
#define _ACPI_SLIC_TABLE_H_

//
// Include files
//
#include "Tiano.h"
#include "Acpi2_0.h"

//
// Ensure proper structure formats
//
#define SLIC_PUB_KEY_LENGTH             156
#define SLIC_WINDOWS_MARKER_LENGTH      182
#pragma pack(1)
//
// SLIC structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT8                                   slicPubKey[SLIC_PUB_KEY_LENGTH];
  UINT8                                   slicMarker[SLIC_WINDOWS_MARKER_LENGTH];
} EFI_ACPI_SLIC_TABLE;

//
// SLIC Table signatures
//
#define EFI_ACPI_SLIC_TABLE_SIGNATURE   0x43494C53

#pragma pack()

#endif
