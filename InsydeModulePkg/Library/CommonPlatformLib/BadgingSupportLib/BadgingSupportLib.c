/** @file
  BadgingSupportLib will produces badging services for Oem badging driver.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BadgingSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SetupUtility.h>
#include <KernelSetupConfig.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/VariableLib.h>
#include <Guid/ImageAuthentication.h>
#include <Protocol/FirmwareVolume.h>

BOOLEAN  mQATestKeyExist = FALSE;
#define SIZE_OF_QA_TEST_KEY         0x311
UINT8    mQACertificateHashData[SHA256_DIGEST_SIZE] = {
           0xF1, 0x47, 0x00, 0xFC, 0xCA, 0x6A, 0xED, 0x9C, 0x5D, 0x33, 0xF3, 0x56, 0xE3, 0xF8, 0xA3, 0x41,
           0x3B, 0xED, 0xA6, 0x6D, 0xAC, 0xAA, 0x13, 0x24, 0xB0, 0x0E, 0x3D, 0x0A, 0x05, 0x44, 0x6b, 0x18
           };

#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI            0x0000000000000001
EFI_HII_HANDLE    mHiiHandle;
extern UINT8      OemBadgingSupportDxeStrings[];


/**
  Initialize and Install OEM Badging support protocol.

  @param[in] ImageHandle                             image handle of this driver
  @param[in] SystemTable                             pointer to standard EFI system table
  @param[in] BadgingData                             Pointer of EFI_OEM_BADGING_LOGO_DATA structure
  @param[in] NumberOfLogo                            Number of LOGO to be used.
  @param[in] OemVidoeModeScreenStringData            Pointer of OEM_VIDEO_MODE_SCR_STR_DATA
  @param[in] NumberOfVidoeModeScreenStringDataCount  Number of OEM_VIDEO_MODE_SCR_STR_DATA data.
  @param[in] OemBadgingString                        Pointer of OEM_BADGING_STRING before user selection
  @param[in] OemBadgingStringAfterSelect             Pointer of OEM_BADGING_STRING after user selection
  @param[in] NumberOfString                          Number of OEM BADGING String

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurred when executing this entry point.

**/
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
)
{
  EFI_STATUS                            Status;
  OEM_BADGING_INFO                      *OemBadgingInfo;

  OemBadgingInfo = AllocateZeroPool (sizeof(OEM_BADGING_INFO));
  if (OemBadgingInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OemBadgingInfo->Signature                      = EFI_OEM_BADGING_INFO_SIGNATURE;
  OemBadgingInfo->BadgingData                    = BadgingData;
  OemBadgingInfo->InstanceCount                  = NumberOfLogo;
  OemBadgingInfo->OemVidoeModeScreenStringData   = OemVidoeModeScreenStringData;
  OemBadgingInfo->VidoeModeScreenStringDataCount = NumberOfVidoeModeScreenStringDataCount;
  OemBadgingInfo->OemBadgingString               = OemBadgingString;
  OemBadgingInfo->OemBadgingStringAfterSelect    = OemBadgingStringAfterSelect;
  OemBadgingInfo->BadginStringCount              = NumberOfString;

  OemBadgingInfo->OemBadging.GetImage            = BadgingGetImage;
  OemBadgingInfo->OemBadging.OemVideoModeScrStrXY= OemVideoModeScrStrLocation;
  OemBadgingInfo->OemBadging.GetOemString        = BadgingGetOemString;
  OemBadgingInfo->OemBadging.GetStringCount      = BadgingGetStringCount;

  OemBadgingInfo->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &OemBadgingInfo->Handle,
                  &gEfiOEMBadgingSupportProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &OemBadgingInfo->OemBadging
                  );

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  mHiiHandle = HiiAddPackages (&gEfiCallerIdGuid, NULL, OemBadgingSupportDxeStrings, NULL);
  ASSERT (mHiiHandle != NULL);

  return Status;
}

/**
  Get the certificate from FV

  @param [in]   NameGuid         The file guid of the certificate
  @param [in, out] Buffer        returned the address of the certificate
  @param [in, out] Size          the size of the certificate

  @retval EFI_SUCCESS            found a certificate
  @retval EFI_NOT_FOUND          did not find one
  @retval EFI_LOAD_ERROR         there is no FV protocol

**/
EFI_STATUS
GetCertificateData (
  IN EFI_GUID                   *NameGuid,
  IN OUT VOID                   **Buffer,
  IN OUT UINTN                  *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&Fv
                    );

    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }

    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadSection (
                   Fv,
                   NameGuid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   Size,
                   &AuthenticationStatus
                   );

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  SHA256 HASH calculation

  @param [in]   Message          The message data to be calculated
  @param [in]   MessageSize      The size in byte of the message data
  @param [out]  Digest           The caclulated HASH digest

  @retval EFI_SUCCESS            The HASH value is calculated
  @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
EFI_STATUS
CalculateHash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID       *HashCtx;
  UINTN      CtxSize;
  EFI_STATUS Status;

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);
  CtxSize = Sha256GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha256Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

/**
  Check if there is a QA test key in Factory Copy.

  @retval TRUE   - found the QA test certificate
  @retval FALSE  - did not found the QA test certificate

**/
BOOLEAN
CheckCertFromFactoryCopy (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         *VarBuffer;
  UINTN                         VariableSize;
  EFI_SIGNATURE_LIST            *SignatureList;
  EFI_SIGNATURE_DATA            *SignatureData;
  UINTN                         Offset;
  UINT8                         Digest[SHA256_DIGEST_SIZE];

  VarBuffer = NULL;
  Status = CommonGetVariableDataAndSize (
             L"dbDefault",
             &gEfiGlobalVariableGuid,
             &VariableSize,
             (VOID **) &VarBuffer
             );
  if (EFI_ERROR (Status) || VarBuffer == NULL) {
    return FALSE;
  }

  Offset = 0;
  while (Offset < VariableSize) {
    SignatureList = (EFI_SIGNATURE_LIST *) (VarBuffer + Offset);
    SignatureData = (EFI_SIGNATURE_DATA *) ((UINT8 *)SignatureList + sizeof (EFI_SIGNATURE_LIST));
    //
    // Compare the certificate size first, if the size doesn't match, skip to the next one.
    //
    if ((SignatureList->SignatureSize - sizeof (EFI_GUID)) == SIZE_OF_QA_TEST_KEY) {
      Status = CalculateHash (SignatureData->SignatureData, SIZE_OF_QA_TEST_KEY, Digest);
      if (CompareMem (Digest, mQACertificateHashData, SHA256_DIGEST_SIZE) == 1) {
        FreePool (VarBuffer);
        return TRUE;
      }
    }
    Offset += SignatureList->SignatureListSize;
  }

  if (VarBuffer != NULL) {
    FreePool (VarBuffer);
  }
  return FALSE;
}

/**
  Check if there is a QA test key. QA test key cannot be used in both FFS and factory copy default.

  @retval TRUE   - found the QA test certificate
  @retval FALSE  - did not found the QA test certificate

**/
BOOLEAN
CheckCertificate (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         *FileBuffer;
  UINTN                         FileSize;
  UINT8                         Digest[SHA256_DIGEST_SIZE];


  //
  // Check the certificate from FFS
  //
  FileBuffer = NULL;
  Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&FileBuffer, &FileSize);
  if ((FileBuffer != NULL) && (FileSize == SIZE_OF_QA_TEST_KEY)) {
    Status = CalculateHash (FileBuffer, FileSize, Digest);
    if (CompareMem (Digest, mQACertificateHashData, SHA256_DIGEST_SIZE) == 1) {
      return TRUE;
    }
  }

  //
  // Check the certificate from factory copy default
  //
  return CheckCertFromFactoryCopy ();
}

/**
  Get Image related information according to Instance and Type data.

  @param[in] This         EFI_OEM_BADGING_SUPPORT_PROTOCOL Interface
  @param[in] Instance     Instace number of mBadgingData structure
  @param[in] Type         Image type
  @param[in] Format       LOGO format
  @param[in] ImageData    Logo data pointer
  @param[in] ImageSize    Logo size
  @param[in] Attribute    Display attribute
  @param[in] CoordinateX  Display location
  @param[in] CoordinateY  Display location

  @retval EFI_SUCCESS      Get Logo Image succesfully
  @retval EFI_NOT_FOUND    Cannot find imaga

**/
EFI_STATUS
EFIAPI
BadgingGetImage (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL      *This,
  IN OUT UINT32                                *Instance,
  IN OUT EFI_BADGING_SUPPORT_IMAGE_TYPE        *Type,
  OUT EFI_BADGING_SUPPORT_FORMAT               *Format,
  OUT UINT8                                    **ImageData,
  OUT UINTN                                    *ImageSize,
  OUT EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE    *Attribute,
  OUT UINTN                                    *CoordinateX,
  OUT UINTN                                    *CoordinateY
  )
{
  EFI_OEM_BADGING_LOGO_DATA  *logos;
  OEM_BADGING_INFO           *OemBadgingInfo;
  EFI_OEM_BADGING_LOGO_DATA  *TempLogo;

  //
  //init locals
  //
  logos = NULL;
  OemBadgingInfo = NULL;
  TempLogo = NULL;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);
  logos = OemBadgingInfo->BadgingData;

  while ((*Instance) < OemBadgingInfo->InstanceCount) {
    if (((logos[*Instance].Visible == NULL) ||
         (logos[*Instance].Visible())) &&
         (logos[*Instance].Type == *Type) &&
         !EFI_ERROR (ReadLogoImage (&logos[*Instance], ImageData, ImageSize))) {

      TempLogo = AllocateCopyPool (sizeof (EFI_OEM_BADGING_LOGO_DATA), &logos[*Instance]);
      if (!mQATestKeyExist) {
        OemSvcChangeDefaultLogoImage (TempLogo, ImageData, ImageSize);
      }

      if ((TempLogo != NULL) && (ImageSize != 0)) {
        *Format       = TempLogo->Format;
        *CoordinateX  = TempLogo->CoordinateX;
        *CoordinateY  = TempLogo->CoordinateY;
        *Attribute    = TempLogo->Attribute;
        (*Instance)++;
        FreePool (TempLogo);
        return EFI_SUCCESS;
      } else {
        FreePool (TempLogo);
        return EFI_NOT_FOUND;
      }
    }
    (*Instance)++;
  }

  return EFI_NOT_FOUND;
}

/**
  Read Logo Image from Firmware Volumn

  @param[in] logo       Logo file GUID
  @param[in] ImageData  Logo data found in ROM
  @param[in] ImageSize  Logo size

  @retval EFI_SUCCESS           Get Logo succesfully
  @retval EFI_BUFFER_TOO_SMALL  Input buffer size too small
  @retval EFI_NOT_FOUND         Cannot find Image in ROM.

**/
EFI_STATUS
EFIAPI
ReadLogoImage (
  IN EFI_OEM_BADGING_LOGO_DATA        *logo,
  OUT UINT8                           **ImageData,
  OUT UINTN                           *ImageSize
  )
{
  EFI_STATUS                       Status;
  EFI_BADGING_SUPPORT_IMAGE_TYPE   BadgeImageType;
  
  //
  // Check if QA certificate is used in BIOS. If QA certificate exists, show warning logo!
  //
  if (FeaturePcdGet(PcdH2OAcpiBgrtSupported) == TRUE) {
    //
    // BGRT feature is enabled.
    //
    BadgeImageType = EfiBadgingSupportImageBoot;
  } else {
    BadgeImageType = EfiBadgingSupportImageBadge;
  }
  
  if ((logo->Type == BadgeImageType) && CheckCertificate ()) {
    CopyMem (&logo->FileName, PcdGetPtr (PcdWarningLogoFile), sizeof (EFI_GUID));
    logo->Format      = EfiBadgingSupportFormatJPEG;
    logo->CoordinateX = 0;
    logo->CoordinateY = 0;
    if (FeaturePcdGet(PcdH2OAcpiBgrtSupported) == TRUE) { 
      logo->Attribute   = EfiBadgingSupportDisplayAttributeCustomized;
    } else {
      logo->Attribute   = EfiBadgingSupportDisplayAttributeCenter;
    }
    mQATestKeyExist   = TRUE;
  }
  Status = GetSectionFromAnyFv (&logo->FileName, EFI_SECTION_RAW, 0, (VOID **)ImageData, ImageSize);

  return Status;
}

/**
  Provide oem logo related services.
  1.) check and return status of supported video resolution mode.
  2.) return XY location of the string based on the video resoultion.

  @param[in] This                 - Protocol instance pointer.
  @param[in] ServiceType          - Type of service needed. Refer to type list.
                                      OemSupportedVideoMode
                                      OemEnterSetupStr
                                      OemPort80CodeStr
                                      OemBuildQualityStr
  @param[in] VideoWidth           - input parameter, screen width in pixel.
  @param[in] VideoHeight          - input parameter, screen height in pixel.
  @param[in] *StringLocationX     - pointer to hold the result of the String X location.
                                      This arguments is ignore for service type "OemSupportedVideoMode".
  @param[in] *StringLocationY     - pointer to hold the result of the String Y location.
                                      This arguments is ignore for service type "OemSupportedVideoMode".

  @retval TRUE   - Video mode is supported or String XY location is found.
  @retval FALSE  - video mode not supported or String XY location not found.

**/
BOOLEAN
EFIAPI
OemVideoModeScrStrLocation (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
  IN EFI_OEM_VIDEO_MODE_SCREEN_XY_TYPE  ServiceType,
  IN UINT32                             VideoWidth,
  IN UINT32                             VideoHeight,
  IN OUT UINTN                          *StringLocationX,
  IN OUT UINTN                          *StringLocationY
)
{
  OEM_VIDEO_MODE_SCR_STR_DATA           *CurrentEntry;
  UINTN                                 EntryCount;
  UINTN                                 Index;
  OEM_BADGING_INFO                      *OemBadgingInfo;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);

  *StringLocationX = 0;
  *StringLocationY = 0;
  EntryCount = OemBadgingInfo->VidoeModeScreenStringDataCount;
  CurrentEntry = OemBadgingInfo->OemVidoeModeScreenStringData;

  switch (ServiceType) {
    case OemSupportedVideoMode:
      for (Index=0; Index < EntryCount; Index++) {
         if ((VideoWidth == CurrentEntry[Index].XPixel) &&
             (VideoHeight == CurrentEntry[Index].YPixel)) {
           return TRUE;
         }
      }
      break;

    case OemEnterSetupStr:
    case OemPort80CodeStr:
    case OemBuildQualityStr:
        for (Index=0; Index < EntryCount; Index++) {
          if ((VideoWidth == CurrentEntry[Index].XPixel) &&
              (VideoHeight == CurrentEntry[Index].YPixel) &&
              (ServiceType == CurrentEntry[Index].ServiceType)) {
            *StringLocationX = CurrentEntry[Index].StrLocX;
            *StringLocationY = CurrentEntry[Index].StrLocY;
            return TRUE;
          }
        }
      break;

    default:
      break;
  }

  return FALSE;
}

/**
  Get String information which should be shown on screen

  @param[in] This                  Protocol instance pointer.
  @param[in] Index                 String Index of mOemBadgingString array
  @param[in] AfterSelect           Indicate the which string should be shown
  @param[in] SelectedStringNum     String number that should be changed according to user hot key selection
  @param[in] StringData            String data to output on screen
  @param[in] CoordinateX           String data location on screen
  @param[in] CoordinateY           String data location on screen
  @param[in] Foreground            Foreground color information of the string
  @param[in] Background            Background color information of the string

  @retval TRUE   - Preferred string has be gotten
  @retval FALSE  - Cannot find preferred string.

**/
BOOLEAN
EFIAPI
BadgingGetOemString (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
     OUT CHAR16                            **StringData,
     OUT UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
)
{
  BOOLEAN                                  Flag = FALSE;
  EFI_BOOT_MODE                            BootMode;
  UINTN                                    Size;
  OEM_BADGING_INFO                         *OemBadgingInfo;
  OEM_BADGING_STRING                       *OemBadgingString;
  OEM_BADGING_STRING                       *OemBadgingStringAfterSelect;
  EFI_STATUS                               Status;
  UINT64                                   OsIndications;
  UINT64                                   OsIndicationsSupported;
  EFI_SETUP_UTILITY_PROTOCOL               *SetupUtility;
  KERNEL_CONFIGURATION                     *SystemConfiguration = NULL;
  UINT8                                    BootType;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);

  BootMode = GetBootModeHob();


  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (KERNEL_CONFIGURATION *)SetupUtility->SetupNvData;
  }

  BootType = DUAL_BOOT_TYPE;
  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &gSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );

    Size = sizeof(OsIndicationsSupported);
    Status = CommonGetVariable (L"OsIndicationsSupported", &gEfiGlobalVariableGuid, &Size, &OsIndicationsSupported);
    if (EFI_ERROR (Status)) {
      OsIndicationsSupported = 0;
    }

    Size = sizeof(OsIndications);
    Status = CommonGetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Size, &OsIndications);
    if (EFI_ERROR (Status)) {
      OsIndications = 0;
    }
    if ((OsIndicationsSupported & OsIndications & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) ||
     (BootMode == BOOT_ON_S4_RESUME && BootType != EFI_BOOT_TYPE)) {

      return FALSE;
    }

  if ( Index > OemBadgingInfo->BadginStringCount ) {
    return FALSE;
  }

  if (!AfterSelect) {
    if (SystemConfiguration == NULL) {
      DEBUG ((EFI_D_INFO, "System get system configuration data failed.\n"));
      return FALSE;
    }
    if ((SystemConfiguration->QuietBoot) && (BootType == EFI_BOOT_TYPE)) {
      return FALSE;
    }
    OemBadgingString = OemBadgingInfo->OemBadgingString;
    CopyMem (Foreground, &(OemBadgingString[Index].Foreground), sizeof (EFI_UGA_PIXEL));
    CopyMem (Background, &(OemBadgingString[Index].Background), sizeof (EFI_UGA_PIXEL));
    *CoordinateX = OemBadgingString[Index].X;
    *CoordinateY = OemBadgingString[Index].Y;
    if (OemBadgingString[Index].Fun == NULL) {
      *StringData = HiiGetPackageString (&gEfiCallerIdGuid, OemBadgingString[Index].StringToken, NULL);
      if (*StringData != NULL) {
        return TRUE;
      }
      return FALSE;
    }

    Flag = OemBadgingString[Index].Fun(&OemBadgingString[Index], StringData);
  } else {
    OemBadgingStringAfterSelect = (OEM_BADGING_STRING*)OemBadgingInfo->OemBadgingStringAfterSelect;
    if (SelectedStringNum != 0) {
      OemBadgingString = (OEM_BADGING_STRING*)&(OemBadgingStringAfterSelect[(SelectedStringNum-1) * OemBadgingInfo->BadginStringCount].X);
      CopyMem (Foreground, &(OemBadgingString[Index].Foreground), sizeof (EFI_UGA_PIXEL));
      CopyMem (Background, &(OemBadgingString[Index].Background), sizeof (EFI_UGA_PIXEL));
      *CoordinateX = OemBadgingString[Index].X;
      *CoordinateY = OemBadgingString[Index].Y;

      if (OemBadgingString[Index].Fun == NULL) {
        *StringData = HiiGetPackageString (&gEfiCallerIdGuid, OemBadgingString[Index].StringToken, NULL);
        if (*StringData != NULL) {
          return TRUE;
        }
        return FALSE;
      }
      Flag = OemBadgingString[Index].Fun(&OemBadgingString[Index], StringData);
    }
  }
  if (*StringData == NULL) {
      return FALSE;
    }

  return Flag;

}

/**
  Get Number of string that should be shown on screen

  @param[in]     This                 Protocol instance pointer.
  @param[in]     StringCount          Number of string that should be shown on screen

  @retval EFI_SUCCESS       Get String Count Succesfully.
  @retval other             There is no string should be shown.

**/
EFI_STATUS
EFIAPI
BadgingGetStringCount (
  IN      EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
     OUT  UINTN                              *StringCount
)
{
  OEM_BADGING_INFO          *OemBadgingInfo;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);

  *StringCount = OemBadgingInfo->BadginStringCount;

  if ( *StringCount == 0 ) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
