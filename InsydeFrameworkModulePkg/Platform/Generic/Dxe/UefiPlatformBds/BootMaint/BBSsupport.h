//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  BBSsupport.h

Abstract:

  declares interface functions

Revision History

--*/

#ifndef _EFI_BDS_BBS_SUPPORT_H
#define _EFI_BDS_BBS_SUPPORT_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "EfiPrintLib.h"
#include "GenericBdsLib.h"
#include "bootmaint.h"
#include EFI_PROTOCOL_DEFINITION (IdeControllerInit)

VOID
BdsBuildLegacyDevNameString (
  IN BBS_TABLE                     *CurBBSEntry,
  IN HDD_INFO                      *HddInfo,
  IN UINTN                         Index,
  IN UINTN                         BufSize,
  OUT CHAR16                       *BootString
  );

EFI_STATUS
BdsDeleteAllInvalidLegacyBootOptions (
  VOID
  );

EFI_STATUS
BdsAddNonExistingLegacyBootOptions (
  VOID
  )
/*++

Routine Description:

  Add the legacy boot options from BBS table if they do not exist.

Arguments:

  None.

Returns:

  EFI_SUCCESS       - The boot options are added successfully or they are already in boot options.
  others            - An error occurred when creating legacy boot options.

--*/
;

EFI_STATUS
BdsUpdateLegacyDevOrder (
  VOID
  );

EFI_STATUS
BdsRefreshBbsTableForBoot (
  IN BDS_COMMON_OPTION        *Entry
  );

#endif
