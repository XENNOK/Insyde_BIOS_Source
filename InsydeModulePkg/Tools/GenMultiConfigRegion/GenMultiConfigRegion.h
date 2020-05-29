/** @file
 Multi Config Tool header file.

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


#ifndef _GEN_MULTI_CONFIG_REGION_H_
#define _GEN_MULTI_CONFIG_REGION_H_


#include <stdio.h>

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/ToolLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MultiConfigBaseLib.h>

#include <Pi/PiFirmwareVolume.h>


#define STATUS_SUCCESS           0
#define STATUS_NOT_FOUND         1
#define STATUS_INVALID_PARAMETER 2
#define STATUS_MALLOC_FAIL       3
#define STATUS_BAD_BUFFER_SIZE   4
#define STATUS_BUFFER_TOO_SMALL  5
#define STATUS_VOLUME_CORRUPTED  10
#define STATUS_CRC_ERROR         27

#define STATUS_ERROR(a) (((UINT8) (a) != STATUS_SUCCESS) ? (a) : STATUS_SUCCESS)

#endif

