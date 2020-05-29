/** @file
  BadgingSupportLib library include file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BADGING_SUPPORT_LIB_H_
#define _BADGING_SUPPORT_LIB_H_

#include <Uefi.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/OEMBadgingSupport.h>

#define EFI_OEM_BADGING_INFO_SIGNATURE SIGNATURE_32('O','e','m','b')
extern EFI_GUID  gEfiGenericVariableGuid;
extern EFI_GUID  gEfiGlobalVariableGuid;

#pragma pack(1)

typedef struct _OEM_BADGING_STRING {
  UINTN                                 X;// Location X
  UINTN                                 Y;// Location Y
  EFI_UGA_PIXEL                         Foreground;// Strings Foreground
  EFI_UGA_PIXEL                         Background;// Strings Background
  EFI_STRING_ID                         StringToken;
  BOOLEAN                               (*Fun) () ;
} OEM_BADGING_STRING;


typedef struct {
  EFI_GUID                              FileName;
  EFI_BADGING_SUPPORT_FORMAT            Format;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE Attribute;
  UINTN                                 CoordinateX;
  UINTN                                 CoordinateY;
  BOOLEAN                               (*Visible)();
  EFI_BADGING_SUPPORT_IMAGE_TYPE        Type;
} EFI_OEM_BADGING_LOGO_DATA;

typedef struct {
  UINT32                                XPixel;         //supported video X resolution
  UINT32                                YPixel;         //supported video Y resolution
  EFI_OEM_VIDEO_MODE_SCREEN_XY_TYPE     ServiceType;    //type of services
  UINT32                                StrLocX;        //on screen string x location
  UINT32                                StrLocY;        //on screen string y location
} OEM_VIDEO_MODE_SCR_STR_DATA;

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;

  EFI_OEM_BADGING_SUPPORT_PROTOCOL      OemBadging;

  EFI_OEM_BADGING_LOGO_DATA             *BadgingData;
  UINTN                                 InstanceCount;
  OEM_VIDEO_MODE_SCR_STR_DATA           *OemVidoeModeScreenStringData;
  UINTN                                 VidoeModeScreenStringDataCount;
  OEM_BADGING_STRING                    *OemBadgingString;
  OEM_BADGING_STRING                    **OemBadgingStringAfterSelect;
  UINTN                                 BadginStringCount;

} OEM_BADGING_INFO;

#define EFI_OEM_BADGING_INFO_FROM_THIS(a) BASE_CR (a, OEM_BADGING_INFO, OemBadging)

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
  );

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
  );

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
);

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
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
     OUT CHAR16                            **StringData,
     OUT UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
) ;

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
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
     OUT UINTN                             *StringCount
);

#pragma pack()

#endif
