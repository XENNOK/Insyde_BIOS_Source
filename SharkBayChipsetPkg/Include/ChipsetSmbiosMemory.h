/** @file
  Define how many channels and slots do the Chipset supported.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SMBIOS_MEMORY_H_
#define _CHIPSET_SMBIOS_MEMORY_H_

/*******************************
Define Max channels and Max slots
*******************************/
//
// Maximum number of SDRAM channels supported by the memory controller
//
#ifndef MAX_CHANNELS
#define MAX_CHANNELS   2
#endif
//
// Maximum number of DIMM sockets supported by each channel
//
#ifndef MAX_SLOTS
#define MAX_SLOTS      2
#endif
///
// Maximum number of DIMM sockets supported by the memory controller
//
#ifndef MAX_SOCKETS
#define MAX_SOCKETS   MAX_CHANNELS * MAX_SLOTS
#endif


#endif
