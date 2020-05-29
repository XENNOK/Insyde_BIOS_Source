/** @file
  GenericUtilityLib

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

#ifndef _GENERIC_UTILITY_LIB_H_
#define _GENERIC_UTILITY_LIB_H_

#include <Protocol/LegacyBios.h>
#include <Guid/BdsHii.h>

extern CHAR16   mFileExplorerStorageName[];
extern CHAR16   mBootMaintStorageName[];
extern BOOLEAN  gConnectAllHappened;

VOID
BdsBuildLegacyDevNameString (
  IN BBS_TABLE                 *CurBBSEntry,
  IN HDD_INFO                  *HddInfo,
  IN UINTN                     Index,
  IN UINTN                     BufSize,
  OUT CHAR16                   *BootString
  );

EFI_STATUS
InitializeBootManager (
  VOID
  );

VOID
CallBootManager (
  VOID
  );

EFI_STATUS
InitializeDeviceManager (
  VOID
  );

UINTN
CallDeviceManager (
  VOID
  );

EFI_STATUS
InitializeSecureBoot (
  VOID
  );

VOID
CallSecureBootMgr (
  VOID
  );

EFI_STATUS
BdsStartBootMaint (
  VOID
  );

#endif
