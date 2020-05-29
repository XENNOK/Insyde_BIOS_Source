/** @file
  This file contain OEM/Platform implementation for TXT in DXE phase.  It represents an abstract outline of the
  steps required during DXE for enabling TXT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize 
  the need to modify this file when porting TXT to future platforms. 
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
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

  TxtDxeOem.c

@brief:

  This file contain OEM/Platform implementation for TXT in DXE phase.  It represents an abstract outline of the
  steps required during DXE for enabling TXT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize 
  the need to modify this file when porting TXT to future platforms.  

**/

#include <Txt.h>
#include "TxtDxeLib.h"
#include <Library/PcdLib.h>

/**

  This function gets called before/after run the SCHECK function, intend to avoid platform specific SMIs to interfere in BIOS POST
  if BIOS SMM MP services or synchonization code is not well considered that some of APs are not waken up from Wait-for-SIPI state.
  Function should preserve original SMI enabling setting in augument is TRUE, and then restore it in augurment is FALSE.
  The caller is DisableSmiSources() in TxtDxeLib.c

  @param[in] TxtDxeCtx        - Point to TXT_DXE_LIB_CONTEXT structure
  @param[in] Operation        = TRUE    - Calling before SCHECK to saved and disable platform specific SMIs setting
                   = FALSE   - Calling after SCHECK to restore platform specific SMIs setting

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
EFIAPI
TxtDxeOemDisableSmi (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx,
  IN BOOLEAN             Operation
  )
{
  static UINT16 SavedSmiControlDev29;
  static UINT16 SavedSmiControlDev26;

  if (Operation == TRUE) {
    ///
    /// Save and disable OEM/Platform specific SMIs
    ///
    /// Disable EHCI SMIs before giving control to TXT ACM. EHCI controller generating SMI during TXT ACM results in USB kbd issue
    ///
    SavedSmiControlDev26  = MmioRead16 (MmPciAddress (0, 0, 0, 0x1A, 0x6C));
    SavedSmiControlDev29  = MmioRead16 (MmPciAddress (0, 0, 0, 0x1D, 0x6C));

    MmioWrite16 (MmPciAddress (0, 0, 0, 0x1A, 0x6C), 0);
    MmioWrite16 (MmPciAddress (0, 0, 0, 0x1D, 0x6C), 0);
  } else {
    ///
    /// Restore or re-enable OEM/Platform specific SMIs
    ///
    /// Enable EHCI SMIs before giving control to TXT ACM.
    ///
    MmioWrite16 (MmPciAddress (0, 0, 0, 0x1A, 0x6C), SavedSmiControlDev26);
    MmioWrite16 (MmPciAddress (0, 0, 0, 0x1D, 0x6C), SavedSmiControlDev29);
  }

  return EFI_SUCCESS;
}
