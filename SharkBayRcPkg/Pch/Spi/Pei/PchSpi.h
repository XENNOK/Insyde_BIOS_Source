/** @file
  Header file for the PCH SPI Runtime Driver.

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
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSpi.h

@brief
  Header file for the PCH SPI Runtime Driver.

**/
#ifndef _PCH_SPI_H_
#define _PCH_SPI_H_

#include <Ppi/Spi.h>
#include <SpiCommon.h>

typedef struct {
  EFI_PEI_PPI_DESCRIPTOR  PpiDescriptor;
  SPI_INSTANCE            SpiInstance;
} PEI_SPI_INSTANCE;

/**
  This function is a hook for Spi Pei phase specific initialization

  @param[in] None

  @retval None
**/
VOID
EFIAPI
SpiPhaseInit (
  VOID
  );

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @param[in] None

  @retval None
**/
VOID
EFIAPI
DisableBiosWriteProtect (
  VOID
  );

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @param[in] None

  @retval None
**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  );

#endif
