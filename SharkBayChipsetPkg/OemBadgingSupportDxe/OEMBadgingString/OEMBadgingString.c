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
  
    OEMBadgingString.c
  
  Abstract:
  
    OEM can define badging string in this file. 
    
--*/
#include <ChipsetSetupConfig.h>
#include <OEMBadgingString.h>
#include <Protocol/SetupUtility.h>
#include <Library/HiiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

//
// OEM can modify the background and foreground color of the OEM dadging string through through the below data
// for example:
//   { 50, 280, { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
//     { 0x00, 0x00, 0x00, 0x00 } indicate the foreground color { Blue, Green, Red, Reserved }
//     { 0xFF, 0xFF, 0xFF, 0x00 } indicate the background color { Blue, Green, Red, Reserved }
//
//[-start-130319-IB01831009-modify]//
//[-start-121017-IB05300350-remove]//
OEM_BADGING_STRING mOemBadgingStringMe5Mb[] = {
//[-start-120511-IB07240112-modify]//
  { OEM_STRING_GRAPHICAL_LOCATION_X,   OEM_STRING_GRAPHICAL_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL },
//[-end-120511-IB07240112-modify]//
  {       OEM_STRING_LOCATION_X,       OEM_STRING3_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER   ), NULL  },
  {       OEM_STRING_LOCATION_X,       OEM_STRING4_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER   ), NULL  },
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL }
};
OEM_BADGING_STRING mOemBadgingString[] = {
//[-start-120511-IB07240112-modify]//
  { OEM_STRING_GRAPHICAL_LOCATION_X,   OEM_STRING_GRAPHICAL_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL },
//[-end-120511-IB07240112-modify]//
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL }
};

//[-end-121017-IB05300350-remove]//
//[-start-121015-IB05300350-modify]//
OEM_BADGING_STRING mOemBadgingStringInTextMode[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID      ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC        ), NULL     },
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL}
};
//[-end-121015-IB05300350-modify]//

//[-start-121017-IB05300350-remove]//
OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMe5Mb[BADGING_AFTER_SELECT_COUNT_ME_5MB][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_ME_5MB] = {
{
//[-start-120511-IB07240112-modify]//
  { OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X, OEM_STRING_GRAPHICAL_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT ), NULL},
//[-end-120511-IB07240112-modify]//
  {       OEM_STRING_LOCATION_X,       OEM_STRING3_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING4_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL},  
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL}
},
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_F10_SELECT ), GetMeFwType},
  {       OEM_STRING_LOCATION_X,       OEM_STRING3_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL       },
  {       OEM_STRING_LOCATION_X,       OEM_STRING4_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL       },      
  {       OEM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL        }
}, 
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_F9_SELECT ),GetMeFwType},
  {       OEM_STRING_LOCATION_X,       OEM_STRING3_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL       },
  {       OEM_STRING_LOCATION_X,       OEM_STRING4_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL       },      
  {       OEM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL       }
},
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS   ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING3_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING4_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL},  
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL}
}
};

OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMe[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT] = {
{
//[-start-120511-IB07240112-modify]//
  { OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X, OEM_STRING_GRAPHICAL_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT ), NULL},
//[-end-120511-IB07240112-modify]//
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL}
},
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS   ), NULL},
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING_COVER        ), NULL}
}
};

OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMeInTextMode[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_TEXT_MODE] = {
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT), NULL},
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT), NULL}
},
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID  ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS), NULL},
  { ME_INFORM_STRING_LOCATION_X, ME_INFORM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS), NULL}
}
};
OEM_BADGING_STRING mOemBadgingStringAfterSelect[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_COUNT] = {
{
//[-start-120511-IB07240112-modify]//
  { OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X, OEM_STRING_GRAPHICAL_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT ), NULL}
//[-end-120511-IB07240112-modify]//
},
{
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS    ), NULL     }    
}
};
//[-end-121017-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
//[-start-130327-IB10920019-modify]//
OEM_BADGING_STRING mOemBadgingStringAfterSelectInTextMode[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_COUNT_TEXT_MODE] = {
  {
    {     OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID      ), GetCpuId },
    {     OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT ), NULL     }
  },
  {
    {     OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID      ), GetCpuId },
    {     OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_F10_SELECT ), NULL     }
  },
  {
    {     OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID      ), GetCpuId },
    {     OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_F9_SELECT  ), NULL     }
  }  
};
//[-end-130327-IB10920019-modify]//

EFI_STATUS
GetId (
  IN OUT UINTN    *CpuId
  )
{
  UINT32 RegEax;
  AsmCpuid (0x01, &RegEax, NULL, NULL, NULL);

  *CpuId = RegEax;

  return EFI_SUCCESS;
}

BOOLEAN
GetCpuId (
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
  )
{
  CHAR16                                *Str2;
  UINTN                                 CpuId;
  EFI_STATUS                            Status;

  CpuId = 0;

  *StringData = HiiGetPackageString (&gEfiCallerIdGuid, Structure->StringToken, NULL);
//[-start-140625-IB05080432-add]//
  if (*StringData == NULL) {
    return FALSE;
  }
//[-end-140625-IB05080432-add]//

  Str2 = AllocateZeroPool (100);
//[-start-130104-IB10870064-add]//
  ASSERT (Str2 != NULL);
  if (Str2 == NULL) {
    return FALSE;
  }
//[-end-130104-IB10870064-add]//

  Status = GetId (&CpuId);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  UnicodeValueToString (Str2, RADIX_HEX, CpuId, 0);

  StrCat (*StringData, Str2);

  FreePool (Str2);

  return TRUE;
}
//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
/**
 Check the platform ME fireware type and decide the F9 / F10 should be displayed or not.

 @param [in]   Structure        Array of Oem Badging String.
 @param [out]  StringData       Hot Key string data.

 @retval TRUE                   The ME fireware type is 5MB, the MEBx relative hot key message should be displayed.
 @retval FALSE                  The ME fireware type is 1.5MB, BIOS will not show any MEBx relative hot key message.

**/
BOOLEAN
GetMeFwType(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
{
  EFI_STATUS                        Status;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;

  *StringData = HiiGetPackageString (&gEfiCallerIdGuid, Structure->StringToken, NULL);

  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );

  if (SetupVariable->MeFwImageType == FW_IMAGE_TYPE_5MB) {
    return TRUE;
  }
  
  return FALSE;
}
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
