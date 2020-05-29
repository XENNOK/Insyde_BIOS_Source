/** @file

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

#ifndef _OEM_DDR3_MANUFACTURE_NAME_TABLE_H_
#define _OEM_DDR3_MANUFACTURE_NAME_TABLE_H_

//
// OEM may add DDR3 DIMM Manufacture name information in OEM_DDR3_MANUFACTURE_NAME_TABLE_DEFINITION if
// 1. Definition in DDR3_MANUFACTURE_NAME_TABLE_DEFINITION is not enough
// 2. Only want to include some specific manufactor name but not whole DDR3_MANUFACTURE_NAME_TABLE_DEFINITION
//
// Example of OEM_DDR3_MANUFACTURE_NAME_TABLE_DEFINITION:
// #define OEM_DDR3_MANUFACTURE_NAME_TABLE_DEFINITION \
//   {0x01, 0x98, L"Kinston"},\
//   {0x02, 0xFE, L"ELPIDA"},  //<- the latest definition should not have "\" at the end
//
//

//
// If OEM define SKIP_DDR3_MANUFACTURE_NAME_TABLE_DEFINITION at below
// then the DDR3_MANUFACTURE_NAME_TABLE_DEFINITION which defined in
// Ddr3ManufactureNameTable.h will not be included in core code
//

// #define SKIP_DDR3_MANUFACTURE_NAME_TABLE_DEFINITION

#endif
