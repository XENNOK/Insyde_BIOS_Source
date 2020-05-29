/** @file
  PlatformBdsLib

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

/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  2005 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  AsfSupport.h

Abstract:

  ASF BDS Support include file

--*/
#ifndef _ASF_SUPPORT_H_
#define _ASF_SUPPORT_H_

#include "BdsPlatform.h"

typedef union {
  UINT32  Data32;
  UINT16  Data16[2];
} DATA32_UNION;

EFI_STATUS
BdsAsfInitialization (
  IN  VOID
  );

EFI_STATUS
BdsBootViaAsf (
  IN  VOID
  );

VOID
PrintBbsTable (
  IN BBS_TABLE                      *LocalBbsTable
  );

EFI_STATUS
BdsLibDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  );

BOOLEAN
BdsCheckIderMedia (
  IN  VOID
  );

EFI_DEVICE_PATH_PROTOCOL  *
BdsCreateShellDevicePath (
  IN VOID
  );

BDS_COMMON_OPTION         *
BdsCreateBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description
  );

BOOLEAN
BdsCheckAsfBootCmd (
  IN  VOID
  );

#endif
