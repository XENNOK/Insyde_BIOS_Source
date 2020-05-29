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

#ifndef _OEM_SMI_TABLE_DEFINE_H_
#define _OEM_SMI_TABLE_DEFINE_H_
//[-start-120419-IB10820038-modify]//
#include <Uefi.h>
//[-end-120419-IB10820038-modify]//
typedef enum { 
//
// OEM must use 0xc0~0xDF as SW_SMI command number.
//

    OEM_EXAMPLE_SW_SMI  = 0xc0  
}OEM_SW_SMI_PORT_TABLE;

#endif 
