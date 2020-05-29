/** @file
  Source file for EFI OEM badging support driver.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <ChipsetSetupConfig.h>
#include <OEMBadgingString.h>
#include <Guid/Pcx.h>
#include <Protocol/SetupUtility.h>
#include <OEMBadgingSupportDxe.h>
#include <Library/BadgingSupportLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
//[-start-121115-IB10820164-add]//
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/DebugLib.h>
//[-end-121115-IB10820164-add]//
//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
extern OEM_BADGING_STRING mOemBadgingString[];
extern OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMe[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT];
extern OEM_BADGING_STRING mOemBadgingStringAfterSelect[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_COUNT];
extern OEM_BADGING_STRING mOemBadgingStringMe5Mb[];
extern OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMe5Mb[BADGING_AFTER_SELECT_COUNT_ME_5MB][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_ME_5MB];
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
extern OEM_BADGING_STRING mOemBadgingStringInTextMode[];
//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-remove]//
extern OEM_BADGING_STRING mOemBadgingStringAfterSelectWithMeInTextMode[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_TEXT_MODE];
//[-end-121015-IB05300350-remove]//
//[-end-130319-IB01831009-modify]//
extern OEM_BADGING_STRING mOemBadgingStringAfterSelectInTextMode[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_COUNT_TEXT_MODE];

EFI_STATUS
InitializeBadgingSupportProtocol (
  IN    EFI_HANDLE                      ImageHandle,
  IN    EFI_SYSTEM_TABLE                *SystemTable,
  IN    EFI_OEM_BADGING_LOGO_DATA       *BadgingData,
  IN    UINTN                           NumberOfLogo,
  IN    OEM_VIDEO_MODE_SCR_STR_DATA     *OemVidoeModeScreenStringData,
  IN    UINTN                           NumberOfVidoeModeScreenStringDataCount,
  IN    OEM_BADGING_STRING              *OemBadgingString,
  IN    OEM_BADGING_STRING              **OemBadgingStringAfterSelect,
  IN    UINTN                           NumberOfString
);

BOOLEAN
IntelInside (
  VOID
  );

BOOLEAN
IntelDuCore (
  VOID
  );

EFI_OEM_BADGING_LOGO_DATA mBadgingData[] = {
  {EFI_DEFAULT_PCX_LOGO_GUID,
   EfiBadgingSupportFormatPCX,
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageLogo
  },
  //
  // BIOS Vendor Insyde Badge
  //
  {EFI_INSYDE_BOOT_BADGING_GUID,
//[-start-140310-IB05160562-modify]//
#ifdef UNSIGNED_FV_SUPPORT
   EfiBadgingSupportFormatJPEG,
#else
   EfiBadgingSupportFormatBMP,
#endif
//[-end-140310-IB05160562-modify]//
   EfiBadgingSupportDisplayAttributeCustomized,
   0,
   0,
   NULL,
   EfiBadgingSupportImageBoot
  },
  {EFI_INSYDE_BOOT_BADGING_GUID,
#ifdef UNSIGNED_FV_SUPPORT
   EfiBadgingSupportFormatJPEG,
#else
   EfiBadgingSupportFormatBMP,
#endif
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageBadge
  }  
};

OEM_VIDEO_MODE_SCR_STR_DATA mOemVidoeModeScreenStringData[] = {
  //Bios Setup String
  {
    640,
    480,
    OemEnterSetupStr,
    400,
    392
  },
  {
    800,
    600,
    OemEnterSetupStr,
    500,
    490
  },
  {
    1024,
    768,
    OemEnterSetupStr,
    640,
    628
  },
  //Port80String
  {
    640,
    480,
    OemPort80CodeStr,
    600,
    440
  },
  {
    800,
    600,
    OemPort80CodeStr,
    750,
    550
  },
  {
    1024,
    768,
    OemPort80CodeStr,
    960,
    704
  },
  //Quality String
  {
    640,
    480,
    OemBuildQualityStr,
    280,
    232
  },
  {
    800,
    600,
    OemBuildQualityStr,
    350,
    290
  },
  {
    1024,
    768,
    OemBuildQualityStr,
    448,
    372
  }
};


/**
  Entry point of EFI OEM Badging Support driver

  @param[in]  ImageHandle - image handle of this driver
  @param[in]  SystemTable - pointer to standard EFI system table

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurred when executing this entry point.

**/
EFI_STATUS
EFIAPI
OEMBadgingEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SETUP_UTILITY_PROTOCOL            *EfiSetupUtility;
  CHIPSET_CONFIGURATION                  *SetupVariable;
  UINTN                                 StringCount;
  OEM_BADGING_STRING                    *OemBadgingString;
//[-start-121115-IB10820164-add]//
  EFI_OEM_BADGING_LOGO_DATA             *BadgingDataPointer;
  UINTN                                 BadgingDataSize;
  OEM_BADGING_STRING                    *OemBadgingStringPointer;
  OEM_BADGING_STRING                    *OemBadgingStringInTextModePointer;
  OEM_BADGING_STRING                    *OemBadgingStringAfterSelectWithMePointer;
  OEM_BADGING_STRING                    *OemBadgingStringAfterSelectWithMeInTextModePointer;
//[-end-121115-IB10820164-add]//

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&EfiSetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SetupVariable = (CHIPSET_CONFIGURATION *)(EfiSetupUtility->SetupNvData);

//[-start-130319-IB01831009-modify]//
//[-start-121015-IB05300350-modify]//
  if(FeaturePcdGet (PcdMe5MbSupported)) {
    StringCount = SetupVariable->QuietBoot ? BADGING_STRINGS_COUNT_ME_5MB : BADGING_STRINGS_COUNT_TEXT_MODE;
  } else {
    StringCount = SetupVariable->QuietBoot ? BADGING_STRINGS_COUNT : BADGING_STRINGS_COUNT_TEXT_MODE;
  }
//[-start-121115-IB10820164-modify]//
  BadgingDataPointer = mBadgingData;
  BadgingDataSize = sizeof (mBadgingData) / sizeof (EFI_OEM_BADGING_LOGO_DATA);
  if(FeaturePcdGet (PcdMe5MbSupported)) {
    OemBadgingStringPointer = mOemBadgingStringMe5Mb;
  } else {
    OemBadgingStringPointer = mOemBadgingString;
  }
  OemBadgingStringInTextModePointer = mOemBadgingStringInTextMode;
  if(FeaturePcdGet (PcdMe5MbSupported)) {
    OemBadgingStringAfterSelectWithMePointer = &mOemBadgingStringAfterSelectWithMe5Mb[0][0];
  } else {
    OemBadgingStringAfterSelectWithMePointer = &mOemBadgingStringAfterSelectWithMe[0][0];
  }
  OemBadgingStringAfterSelectWithMeInTextModePointer = &mOemBadgingStringAfterSelectWithMeInTextMode[0][0];

//[-start-121128-IB05280020-modify]//
  Status = OemSvcUpdateOemBadgingLogoData (
             &BadgingDataPointer,
             &BadgingDataSize,
             &OemBadgingStringPointer,
             &OemBadgingStringInTextModePointer,
             &StringCount,
             &OemBadgingStringAfterSelectWithMePointer,
             &OemBadgingStringAfterSelectWithMeInTextModePointer
             );
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcUpdateOemBadgingLogoData, Status : %r\n", Status));
//[-end-121128-IB05280020-modify]//
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  OemBadgingString = SetupVariable->QuietBoot ? OemBadgingStringAfterSelectWithMePointer : OemBadgingStringAfterSelectWithMeInTextModePointer;

  Status = InitializeBadgingSupportProtocol (
            ImageHandle,
            SystemTable,
            BadgingDataPointer,
            BadgingDataSize,
            mOemVidoeModeScreenStringData,
            sizeof (mOemVidoeModeScreenStringData) / sizeof (OEM_VIDEO_MODE_SCR_STR_DATA),
            SetupVariable->QuietBoot ? OemBadgingStringPointer : OemBadgingStringInTextModePointer,
            (OEM_BADGING_STRING**) OemBadgingString,
            StringCount
            );
//[-end-121115-IB10820164-modify]//
//[-end-121015-IB05300350-modify]//
//[-end-130319-IB01831009-modify]//

  return Status;
}

BOOLEAN
IntelInside (
  VOID
  )
{
  UINT32 RegEax;
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;

  //
  // Is a GenuineIntel CPU?
  //
  AsmCpuid (0x0, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  if ((RegEbx != 'uneG') || (RegEdx != 'Ieni') || (RegEcx != 'letn')) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
IntelDuCore (
  VOID
  )
{
  UINT32 RegEax;
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;
  UINT32 NumOfThreading = 1;
  UINTN NumOfLogicalProcess = 0;
  UINTN NumOfCores = 0;
  // Feature Information
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, &RegEbx, &RegEcx, &RegEdx);

  if (((RegEax & FAMILYMODEL_MASK) >= FAMILYMODEL_PRESCOTT_F4x) ||((RegEax & FAMILYMODEL_MASK) == FAMILY_CONROE)) {

    NumOfLogicalProcess = (UINTN) ((RegEbx >> 16) & 0xf);
    NumOfThreading = (UINT32) ((RegEbx >> 16) & 0xf);
    AsmCpuid (EFI_CPUID_CACHE_PARAMS, &RegEax, &RegEbx, &RegEcx, &RegEdx);

    NumOfCores = (UINTN) ((RegEax >> 26) & 0x3f);

    NumOfCores += 1;

    if (NumOfCores >= 1)
      return TRUE;
    else
      return FALSE;
    }

  return  FALSE;
}
