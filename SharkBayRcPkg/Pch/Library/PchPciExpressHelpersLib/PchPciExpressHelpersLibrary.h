/** @file
  Header file for PCH Pci Express helps library implementation.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _PCH_PCI_EXPRESS_HELPERS_LIBRARY_H_
#define _PCH_PCI_EXPRESS_HELPERS_LIBRARY_H_

#include <Uefi.h>
#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/Pci23.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/PchPciExpressHelpersLib.h>


#define LTR_VALUE_MASK (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7 + BIT8 + BIT9)
#define LTR_SCALE_MASK (BIT10 + BIT11 + BIT12)

//
// LTR related macros
//
#define LTR_LATENCY_VALUE(x)           ((x) & LTR_VALUE_MASK)
#define LTR_SCALE_VALUE(x)             (((x) & LTR_SCALE_MASK) >> 10)
#define LTR_LATENCY_NS(x)              (LTR_LATENCY_VALUE(x) * (1 << (5 * LTR_SCALE_VALUE(x))))

#endif
