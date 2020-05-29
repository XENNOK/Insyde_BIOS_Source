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

Copyright (c)  2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  EfiVpdData.h

Abstract:

  Constants and declarations that are common accross PEI and DXE.
--*/

#ifndef _EFI_VPD_DATA_H_
#define _EFI_VPD_DATA_H_

#include "Tiano.h"

#pragma pack(1)
//
// Firmware ID
//
#define FIRMWARE_ID_MAX_LENGTH  35

typedef struct {
  CHAR8 Acsii[FIRMWARE_ID_MAX_LENGTH];
} FIRMWARE_ID_DATA;

#define FIRMWARE_ID_GUID \
  { \
    0xefc071ae, 0x41b8, 0x4018, 0xaf, 0xa7, 0x31, 0x4b, 0x18, 0x5e, 0x57, 0x8b \
  }

extern EFI_GUID gFirmwareIdGuid;
extern CHAR16   gFirmwareIdName[];

//
// DMI data
//
typedef struct {

  CHAR8 DmiGpnvHeader[4];             // $DMI
  CHAR8 SystemInfoManufacturer[0x20]; // Structure Type 1 String 1
  CHAR8 SystemInfoProductName[0x20];  // Structure Type 1 String 2
  CHAR8 SystemInfoVersion[0x18];      // Structure Type 1 String 3
  CHAR8 SystemInfoSerialNumber[0x20]; // Structure Type 1 String 4
  CHAR8 BaseBoardManufacturer[0x20];  // Structure Type 2 String 1
  CHAR8 BaseBoardProductName[0x20];   // Structure Type 2 String 2
  CHAR8 BaseBoardVersion[0x18];       // Structure Type 2 String 3
  CHAR8 BaseBoardSerialNumber[0x20];  // Structure Type 2 String 4
  CHAR8 ChassisManufacturer[0x20];    // Structure Type 3 String 1
  UINT8 ChassisType;                  // Enumerated
  CHAR8 ChassisVersion[0x18];         // Structure Type 3 String 2
  CHAR8 ChassisSerialNumber[0x20];    // Structure Type 3 String 3
  CHAR8 ChassisAssetTag[0x20];        // Structure Type 3 String 4
  UINT8 MfgAccessKeyWorkspace;

  UINT8 ChecksumFixupPool[0xd];       // Checksum Fix-ups
  UINT8 SwitchboardData[4];           // 32 switch switchboard
  UINT8 IntelReserved;                // Reserved for Future Use
} DMI_DATA;

#define DMI_DATA_GUID \
  { \
    0x70e56c5e, 0x280c, 0x44b0, 0xa4, 0x97, 0x09, 0x68, 0x1a, 0xbc, 0x37, 0x5e \
  }

#define DMI_DATA_NAME       (L"DmiData")
#define ASCII_DMI_DATA_NAME ("DmiData")

extern EFI_GUID gDmiDataGuid;
extern CHAR16   gDmiDataName[];

//
// UUID - universally unique system id
//
#define UUID_VARIABLE_GUID \
  { \
    0xd357c710, 0x0ada, 0x4717, 0x8d, 0xba, 0xc6, 0xad, 0xc7, 0xcd, 0x2b, 0x2a \
  }

#define UUID_VARIABLE_NAME        (L"UUID")
#define ASCII_UUID_VARIABLE_NAME  ("UUID")

extern EFI_GUID gUuidVariableGuid;
extern CHAR16   gUuidVariableName[];

#pragma pack()

#endif
