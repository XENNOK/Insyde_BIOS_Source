/** @file
  Header file for the PCH USB PEIM

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
  PchUsb.h

@brief
  Header file for the PCH USB PEIM

**/
#ifndef _PCH_USB_H_
#define _PCH_USB_H_

#include <PchAccess.h>
#include <Ppi/PchUsbPolicy.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>


/**
  Initialize PCH EHCI PEIM

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] UsbPolicyPpi         PCH Usb Policy PPI

  @retval EFI_SUCCESS             The PCH EHCI PEIM is initialized successfully
  @retval EFI_INVALID_PARAMETER   UsbControllerId is out of range
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
InitForEHCI (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN PCH_USB_POLICY_PPI         *UsbPolicyPpi
  );

#endif
