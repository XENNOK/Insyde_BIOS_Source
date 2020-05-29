//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/** @file
Implementation of CPU driver for PEI phase.

This PEIM is to expose the CPUIO ppi, Cache Ppi and BIST hob build notification

  Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.                                       

**/

#ifndef _PEI_CPUPEIM_H_
#define _PEI_CPUPEIM_H_


#include <PiPei.h>

#include <Ppi/Cache.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/SecPlatformInformation.h>
#include <Guid/HtBistHob.h>

#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MtrrLib.h>
#include <Library/PcdLib.h>
#include <Library/SocketLga775Lib.h>

#endif
