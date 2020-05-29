/** @file
 The structure consumed in InsydeModule Pkg.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _PORT_NUMBER_MAP_H_
#define _PORT_NUMBER_MAP_H_

//*******************************************************
// Port number mapping table
//*******************************************************
// PrimarySecondary = 0 this is primary
// PrimarySecondary = 1 this is secondary
// SlaveMaster      = 0 this is Master
// SlaveMaster      = 1 this is Slave
//
typedef struct {
  UINT8            Bus;
  UINT8            Device;
  UINT8            Function;
  UINT8            PrimarySecondary;
  UINT8            SlaveMaster;
  UINT8            PortNum;
} PORT_NUMBER_MAP;
									
#endif
