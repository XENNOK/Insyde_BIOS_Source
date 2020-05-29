/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Pcct.asl

Abstract:

  Intel ACPI Reference Code for ACPI 5.0 Platform Communications Channel Table (PCCT)

--*/

DefinitionBlock (
	"Pcct.aml",
	"PCCT",
	5,
	"PcctTa",
	"PcctTabl",
	0x1000 )
{

/// The 12 byte EFI_ACPI_5_0_PLATFORM_COMMUNICATION_CHANNEL_TABLE_HEADER structure is patched during POST.
/// The 62 byte EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC structure is patched during POST.  
/// Allocate a buffer of 64 bytes plus the size of the 10 byte Name structure for a total of 74 bytes.
///
Name(PCCT,Buffer() {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 10
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 20
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 30
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 40
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 50
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 60
0x00, 0x00, 0x00, 0x00 // 64
})
	
}// end DefinitionBlock
