/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcHdaInitHook().
 The function PeiCsSvcHdaInitHook() use chipset services to provide
 a interface for chipset porting on different HDA controller.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/IoLib.h>
#include <PchAccess.h>

/**
 To provide a interface for chipset porting on different HDA controller.

 @param[in]         HdaBar              Base address for HDA

 @return            EFI_SUCCESS         This function always return successfully
*/
EFI_STATUS
HdaInitHook (
  IN UINT32          HdaBar
  )
{
  UINT16              Data16;
  
  //
  // To add your chipset code at here.
  //
  //
  // PCH BIOS Spec Rev 1.1.0 Section 9.1.3 Codec Initialization Programming Sequence
  // Read GCAP and write the same value back to the register once after Controller Reset# bit is set
  //
  Data16  = MmioRead16 (HdaBar + R_HDA_GCAP);
  MmioOr16 (HdaBar + R_HDA_GCAP, Data16);
  
  return EFI_SUCCESS;
}