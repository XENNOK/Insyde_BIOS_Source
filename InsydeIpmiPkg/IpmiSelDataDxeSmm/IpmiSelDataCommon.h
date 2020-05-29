/** @file
 H2O IPMI SEL Data module header file.

 This file contains common functions prototype, structure and definitions for
 H2O IPMI SEL Data module internal use.
 
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SEL_DATA_COMMON_H_
#define _IPMI_SEL_DATA_COMMON_H_


#include <Protocol/H2OIpmiSelDataProtocol.h>

#include <Library/IpmiInterfaceLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>



//
// Function prototype
//

/**
 Fill private data content.

*/
VOID
InitialIpmiSelDataContent (
  VOID
  );



extern H2O_IPMI_SEL_DATA_PROTOCOL   mSelDataProtocol;

#endif

