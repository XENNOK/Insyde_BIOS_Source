/** @file
  Header file for the PCH Common Init PEIM.

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
@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PCH_INIT_COMMON_PEIM_H_
#define _PCH_INIT_COMMON_PEIM_H_

#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/PchUsbPolicy.h>

#define PCH_INIT_COMMON_SCRIPT_IO_WRITE(TableName, Width, Address, Count, Buffer)

#define PCH_INIT_COMMON_SCRIPT_IO_READ_WRITE(TableName, Width, Address, Data, DataMask)

#define PCH_INIT_COMMON_SCRIPT_MEM_WRITE(TableName, Width, Address, Count, Buffer)

#define PCH_INIT_COMMON_SCRIPT_MEM_READ_WRITE(TableName, Width, Address, Data, DataMask)

#define PCH_INIT_COMMON_SCRIPT_PCI_CFG_WRITE(TableName, Width, Address, Count, Buffer)

#define PCH_INIT_COMMON_SCRIPT_PCI_CFG_READ_WRITE(TableName, Width, Address, Data, DataMask)

#define PCH_INIT_COMMON_SCRIPT_STALL(TableName, Duration)

#define PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM(RootComplexBar, Address, AndMask, OrMask) \
          EFI_SUCCESS

#ifdef USB_PRECONDITION_ENABLE_FLAG
///
/// Execute function when running in PEI
///
#define USB_RUN_IN_PEI TRUE

///
/// Execute function when running in DXE
/// It is always FALSE for PEI phase check
///
#define USB_RUN_IN_DXE FALSE

///
/// USB precondition policy check
///
#define USB_PRECONDITION_POLICY_SUPPORT(UsbPolicy) \
          ((UsbPolicy)->UsbPrecondition)

#endif  // USB_PRECONDITION_ENABLE_FLAG

///
/// USB3 port setting policy check
///
#define USB3PORT_SETTING_POLICY_SUPPORT(Revision) \
          ((Revision >= PCH_PLATFORM_POLICY_PPI_REVISION_3))

#endif
