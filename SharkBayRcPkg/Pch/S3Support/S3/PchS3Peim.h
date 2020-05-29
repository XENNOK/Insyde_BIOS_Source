/** @file
  This is the PEIM that performs the S3 resume tasks.

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
  Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchS3Peim.h
    
@brief
  This is the PEIM that performs the S3 resume tasks.

**/
#ifndef _PCH_S3_PEIM_H_
#define _PCH_S3_PEIM_H_

#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <PchInitVar.h>
#include <Library/PchPciExpressHelpersLib.h>
#include <Protocol/PchS3Support.h>
#include <Ppi/ReadOnlyVariable2.h>

#define EFI_PCH_S3_STALL_INTERVAL 50  ///< us

/**
  Send Codec command on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_DEVICE_ERROR        Device status error, operation failed
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchS3SendCodecCommand (
  EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND      *Parameter
  );

/**
  Perform Init Root Port Downstream devices on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3InitPcieRootPortDownstream (
  EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM     *Parameter
  );

/**
  Perform Root Port Downstream devices PCIE ASPM and LTR override on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3PcieSetPm (
  EFI_PCH_S3_PARAMETER_PCIE_SET_PM     *Parameter
  );

/**
  Perform PCH IOBP programming on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3ProgramIobp (
  EFI_PCH_S3_PARAMETER_PROG_IOBP     *Parameter
  );

#endif
