/** @file

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

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to  
  the additional terms of the license agreement               
**/

/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  RapidStartDxeLib.h

@brief:

  RapidStart Dxe Platform Library header file.


**/
#ifndef _RAPID_START_DXE_LIB_H_
#define _RAPID_START_DXE_LIB_H_

#include <Protocol/RapidStartPlatformPolicy.h>


typedef struct {
  VENDOR_DEVICE_PATH              DevicePath;
  UINT8                           LegacyDriveLetter;
} UNKNOWN_DEVICE_VENDOR_DEVICE_PATH;


/**
  Search if any type of RapidStart Store partition present

  This function will search all connected/initialized SSD and see if any valid and
  supported Rapid Start Store present or not. It will scan both GPT and MBR type
  partitions and return the required information for accessing Rapid Start Store.
  If no valid/supported Rapid Start Store found the Rapid Start will be disabled.
  The method and algorithm for searching Rapid Start Store could be implemented by
  platform design.

  @param[in] RapidStartPolicy  - RapidStart Platform Policy protocol
  @param[out] StoreSectors     - Size of RapidStart store partition
  @param[out] StoreLbaAddr     - Address of RapidStart store partition
  @param[out] StoreSataPort    - Port number for RapidStart store partition

  @retval EFI_SUCCESS   - GPT or MBR type RapidStart Store found
  @retval EFI_NOT_FOUND - GPT or MBR type RapidStart Store not found
**/
EFI_STATUS
SearchRapidStartStore (
  IN     RAPID_START_PLATFORM_POLICY_PROTOCOL *RapidStartPolicy,
  OUT    UINT32                               *StoreSectors,
  OUT    UINT64                               *StoreLbaAddr,
  OUT    UINT8                                *StoreSataPort
  );

/**
  Generate RapidStart Store UID

  This function will generate a unique ID which will be stored into Rapid Start
  Store for identifying Rapid Start Store changing condition. When Rapid Start
  Store changed during Rapid Start Resume, the Rapid Start Resume should be
  aborted. By default this ID is combining the CPU TSC and HPET counter. This UID
  will only be generated when Rapid Start Persistent Data not present or Rapid
  Start Store has changed in previous resume.

  @retval UINT64 as RapidStart Store UID
**/
UINT64
GenerateRapidStartStoreUid (
  VOID
  );

/**
  Re-enable Hibernation when RapidStart enabled but RapidStart Store not present.

  It's recommended to disable ACPI Hibernate support when Rapid Start feature
  enabled. In this case, if Rapid Start Store not present or invalid (Rapid Start
  will not be executed), ACPI Hibernate should be re-enabled. This function
  required porting to fit platform implementation.
**/
VOID
EnableHibernate (
  VOID
  );
#endif
