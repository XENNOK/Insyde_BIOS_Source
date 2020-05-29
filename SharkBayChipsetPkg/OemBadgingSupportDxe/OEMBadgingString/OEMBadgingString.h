/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++ 
  Module Name:
  
    OEMBadgingString.h
  
  Abstract:
  
    Header file for OEM badging support driver
    
--*/
#ifndef _EFI_OEM_BADGING_STRING_H
#define _EFI_OEM_BADGING_STRING_H

#include <Library/BadgingSupportLib.h>
#include <Library/PcdLib.h>

//[-start-130327-IB10920019-add]//
#include <PostKey.h>
//[-end-130327-IB10920019-add]//

EFI_STATUS
GetId(
  IN OUT UINTN    *CpuId
);

BOOLEAN
GetCpuId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
//#ifdef ME_5MB_SUPPORT
BOOLEAN
GetMeFwType(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);
//#endif
//#ifdef ME_5MB_SUPPORT
#define BADGING_STRINGS_COUNT_ME_5MB                      3
#define BADGING_AFTER_SELECT_COUNT_ME_5MB                 4
#define BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_ME_5MB    4
//#else
#define BADGING_STRINGS_COUNT                             1
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
//[-start-130327-IB10920019-modify]//
//#define BADGING_AFTER_SELECT_COUNT      2
#define BADGING_AFTER_SELECT_COUNT      (MAX_HOT_KEY_SUPPORT - 1)
//[-end-130327-IB10920019-modify]//
//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
#define BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT           2
//#endif
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
#define BADGING_STRINGS_COUNT_TEXT_MODE                      2
//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
#define BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_TEXT_MODE    3
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
#define OEM_STRING_LOCATION_X       50
//[-start-120511-IB07240112-modify]//
//
// If set x or y value of OEM badging string to -1, system will auto place string at
// below boot image when BGRT is present or center of screen when BGRT is not exist.
//
//#define OEM_STRING_GRAPHICAL_LOCATION_X                408
//#define OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X     372
#define OEM_STRING_GRAPHICAL_LOCATION_X                (UINTN) -1
#define OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X     (UINTN) -1
#define OEM_STRING_GRAPHICAL_LOCATION_Y                (UINTN) -1
//[-end-120511-IB07240112-modify]//
//[-start-120514-IB10030013-modify]//
#define OEM_STRING1_LOCATION_Y      500
#define OEM_STRING2_LOCATION_Y      575
#define OEM_STRING3_LOCATION_Y      550
#define OEM_STRING4_LOCATION_Y      625
//[-end-120514-IB10030013-modify]//

//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
#define ME_INFORM_STRING_LOCATION_X                50
#define ME_INFORM_STRING1_LOCATION_Y               0
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
#endif
