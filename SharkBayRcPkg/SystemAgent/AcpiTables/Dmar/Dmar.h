/**
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             

@copyright
Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  Dmar.h

@brief
  This file describes the contents of the ACPI DMA address Remapping  
  Some additional ACPI values are defined in Acpi1_0.h and Acpi2_0.h.
**/
#ifndef _SA_DMAR_H_
#define _SA_DMAR_H_

///
/// Include standard ACPI table definitions
///
#include <Uefi.h>
#include <IndustryStandard/Acpi30.h>
#include <DmaRemappingTable.h>

#pragma pack(1)

#define EFI_ACPI_DMAR_OEM_TABLE_ID    0x20575348  ///< "HSW "
#define EFI_ACPI_DMAR_OEM_CREATOR_ID  0x4C544E49  ///< "INTL"
#pragma pack()

#endif
