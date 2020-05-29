//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuConfig.h

Abstract:

  Header for the status code data hub logging component

**/

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#include <PiDxe.h>

#include <Protocol/MpService.h>

#include <Library/CpuConfigLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/SocketLga775Lib.h>

/**
  Worker function to create a new feature entry.
  
  This is a worker function to create a new feature entry. The new entry is added to
  the feature list by other functions in the library.

  @param  FeatureID   The ID of the feature.
  @param  Attribute   Feature-specific data.
  
  @return  The address of the created entry.

**/
CPU_FEATURE_ENTRY *
CreateFeatureEntry (
  IN  CPU_FEATURE_ID      FeatureID,
  IN  VOID                *Attribute
  );

/**
  Worker function to search for a feature entry in processor feature list.
  
  This is a worker function to search for a feature entry in processor feature list.

  @param  ProcessorNumber Handle number of specified logical processor
  @param  FeatureIndex    The index of the new node in feature list.
  
  @return Pointer to the feature node. If not found, NULL is returned.

**/
CPU_FEATURE_ENTRY *
SearchFeatureEntry (
  IN  UINTN               ProcessorNumber,
  IN  UINTN               FeatureIndex
  );

#endif
