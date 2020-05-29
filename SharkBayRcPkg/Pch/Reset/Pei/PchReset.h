/** @file
  Header file for PCH RESET PEIM Driver.

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
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchReset.h

@brief
  Header file for PCH RESET PEIM Driver.

**/
#ifndef _PCH_RESET_H
#define _PCH_RESET_H

#include <Ppi/PchReset.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PchResetCommon.h>
#include <PchAccess.h>

typedef struct {
  EFI_PEI_PPI_DESCRIPTOR  PpiDescriptor;
  PCH_RESET_INSTANCE      PchResetInstance;
} PEI_PCH_RESET_INSTANCE;

/**
  Installs PCH RESET PPI

  @param[in] FfsHeader            Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
InstallPchReset (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  );

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @exception EFI_UNSUPPORTED      Do not do any reset from PCH
**/
EFI_STATUS
EFIAPI
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  );
#endif
