/** @file

  Header file.

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

#ifndef _EFI_AT_GRAPHICS_LIB_H_
#define _EFI_AT_GRAPHICS_LIB_H_

#include <Protocol/ConsoleControl.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/UgaDraw.h>
#include <IndustryStandard/Bmp.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/ExitAtAuth.h>

#define SIZE_4G                            0x100000000
#define DEFAULT_HORIZONTAL_RESOLUTION      800
#define DEFAULT_VERTICAL_RESOLUTION        600
#define DEFAULT_HORIZONTAL_1024_RESOLUTION 1024
#define DEFAULT_VERTICAL_768_RESOLUTION    768
#define AT_GRAPHICS_REVISION_1             5

typedef enum {
  ENTER_NORMAL,
  ENTER_ESCAPED,
  CHECK_RECOVERY,
  ENTER_EXCEEDED,
  ENTER_EXCEEDED2,
//[-start-120419-IB04320191-add]//
  NO_PASSWORD_ENTERED,
  WRONG_PASSWORD_ENTERED
//[-end-120419-IB04320191-add]//
} EFI_ENTER_PASSWORD_TYPE;

typedef enum {
  INITIAL_WWAN,
  INITIAL_WWAN_FAIL,
  INITIAL_WWAN_COMPLETE,
  WAIT_RADIO_NETWORK,
  INITIAL_WWAN_DOWN,
  INITIAL_WWAN_SUCCESS,
  INITIAL_CHECK_SIM_RADIO,
  WAIT_SMS_RECOVERY,
  WWAN_TIMEOUT,
  WWAN_COMPLETE,
  WWAN_RECOVERY_SUCCESS,
  WWAN_RECOVERY_FAIL,
  INTELAT_RECOVERY_FAIL,
  SYSTEM_SHUTDOWN,
  INTELAT_RECOVERY_SUCCESS,
  SYSTEM_BOOT
} EFI_INITIAL_WWAN_TYPE;

typedef enum {
  AT_SUSPEND_STATE,
  EXIT_SUSPEND_MODE,
  STAY_SUSPEND_MODE,
  REQUEST_SUSPEND_MODE,
  ERROR_NONCE_FW,
  RECOVERY_ID,
  ENTER_SERVER_SUSPEND,
  CHECK_AUTHENTICATION,
  FAIL_AUTHENTICATE,
  FAIL_INTO_SUSPEND,
  SUCCESS_INTO_SUSPEND,
  FAIL_AUTHENTICATE2,
  SET_SUSPEND_FAIL,
  SUCCESS_EXIT_SUSPEND,  
  EXCEEDED_MAX_ATTEMPT,
  ENTER_SERVER_RESUME  
} EFI_SUSPEND_MODE_TYPE;


typedef enum {
  ENTER_ADDRESS
} SERVER_ADDRESS_TYPE;

typedef enum {
  AT_SYSTEM_LOCK,
  TIME_LEFT_INPUT1,
  TIME_LEFT_INPUT2,
  RECOVERY_MESSAGE,
  RECOVERY_USER_PASSWORD,
  RECOVERY_SERVER_TOKEN,
  RECOVERY_WWAN_UNLOCK,
  RECOVERY_SELECT_OPTION,
  USER_PASSWORD_SELECTED,
  SERVER_TOKEN_SELECTED,
  WWAN_RECOVERY_SELECTED,
//[-start-120106-IB04320182-modify]//
  INVALID_SELECTION_123,
  INVALID_SELECTION_12,
//[-end-120106-IB04320182-modify]//
  RECOVERY_SCREEN_ID,
  ENTER_SYSTEM_PASSWORD,
  ENTER_SCREEN_RECOVERY
} EFI_RECOVERY_SCREEN_TYPE;

typedef enum {
  RECOVERY_ISVSTRING,
  PLATFROM_ID,
//[-start-120419-IB04320191-modify]//
  SERVICE_ID,
  LOCKED_ID,
  IF_FOUND_ID
//[-end-120419-IB04320191-modify]//
} EFI_ISV_SCREEN_TYPE;

//[-start-120419-IB04320191-add]//
typedef enum {
  WWAN_NOT_DETECT,
  WWAN_DETECT_PENDING,
  WWAN_DETECTED,
  WWAN_UNEXPECTED_STATE,
  CLEAR_INVALID_123
} EFI_DETECT_WWAN_TYPE;
//[-end-120419-IB04320191-add]//

typedef enum {
//[-start-120419-IB04320191-modify]//
  CLEAR_TOP_SCREEN = 0x10,
  CLEAR_BOTTOM_SCREEN = 0x20,
//[-end-120419-IB04320191-modify]//
  SYSTEM_PASSWORD = 0x30,
  SYSTEM_RECOVERY = 0x40,
  INITIALWWAN = 0x50,
  SHOW_PASSWORD = 0x60,
  SHOW_SUSPEND_MODE =0x70,
  SHOW_USER_TIME = 0x80,
  ISV_SCREEN = 0x90,
//[-start-120419-IB04320191-modify]//
  SERVER_ADDRESS = 0xA0,
  DETECT_WWAN = 0xB0
//[-end-120419-IB04320191-modify]//
} EFI_SELECTED_STRING_TYPE;

typedef enum {
  TOP_SCREEN_STRINGS = 1,
  BOTTOM_SCREEN_STRINGS,
  SYSTEM_PASSWORD_STRINGS,
  SYSTEM_RECOVERY_STRINGS,
  INITIALWWAN_STRINGS,
  SHOW_PASSWORD_STRINGS,
  SHOW_SUSPEND_MODE_STRINGS,
  USER_TIMER_LEFT_STRINGS,
  ISV_SCREEN_STRINGS,
//[-start-120419-IB04320191-modify]//
  SERVER_ADDRESS_STRINGS,
  DETECT_WWAN_STRINGS
//[-end-120419-IB04320191-modify]//
} EFI_SHOW_TYPE;

typedef struct {
  UINT32                        SizeOfX;
  UINT32                        SizeOfY;
  UINT32                        ColorDepth;
  UINT32                        RefreshRate;
} EFI_QUIET_BOOT_INFO;


/**
  Return the graphics image file named FileNameGuid into Image and return it's
  size in ImageSize. All Firmware Volumes (FV) in the system are searched for the
  file name.

  @param [in]  FileNameGuid       File Name of graphics file in the FV(s).
  @param [out] Image              Pointer to pointer to return graphics image.  If NULL, a
                                  buffer will be allocated.
  @param [out] ImageSize          Size of the graphics Image in bytes. Zero if no image found.

  @retval EFI_SUCCESS             Image and ImageSize are valid.
  @retval EFI_BUFFER_TOO_SMALL    Image not big enough. ImageSize has required size
  @retval EFI_NOT_FOUND           FileNameGuid not found

**/
EFI_STATUS
GetGraphicsBitMapFromFV (
  IN  EFI_GUID      *FileNameGuid,
  OUT VOID          **Image,
  OUT UINTN         *ImageSize
  );

/**
  Convert a *.BMP graphics image to a GOP/UGA blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param [in]      BmpImage       Pointer to BMP file
  @param [in]      BmpImageSize   Number of bytes in BmpImage
  @param [in, out] GopBlt         Buffer containing GOP version of BmpImage.
  @param [in, out] GopBltSize     Size of GopBlt in bytes.
  @param [out]     PixelHeight    Height of GopBlt/BmpImage in pixels
  @param [out]     PixelWidth     Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS             GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED         BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL    The passed in GopBlt buffer is not big enough.
                                  GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES    No enough buffer to allocate

**/
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID   *BmpImage,
  IN     UINTN  BmpImageSize,
  IN OUT VOID   **GopBlt,
  IN OUT UINTN  *GopBltSize,
  OUT    UINTN  *PixelHeight,
  OUT    UINTN  *PixelWidth
  );

/**
  Use Console Control to turn off UGA based Simple Text Out consoles from going
  to the UGA device. Put up LogoFile on every UGA device that is a console

  @param [in] LogoFile            File name of logo to display on the center of the screen.

  @retval EFI_SUCCESS             ConsoleControl has been flipped to graphics and logo
                                  displayed.
  @retval EFI_UNSUPPORTED         Logo not found

**/
EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
 );

/**
  Use Console Control to turn on UGA based Simple Text Out consoles. The UGA
  Simple Text Out screens will now be synced up with all non UGA output devices

  @param none

  @retval EFI_SUCCESS             UGA devices are back in text mode and synced up.
  @retval EFI_UNSUPPORTED         Logo not found

**/
EFI_STATUS
DisableQuietBoot (
  VOID
  );

/**
  Use Console Control Protocol to lock the Console In Spliter virtual handle.
  This is the ConInHandle and ConIn handle in the EFI system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

  @param [in] Password            Password used to lock ConIn device

  @retval EFI_SUCCESS             ConsoleControl has been flipped to graphics and logo
                                  displayed.
  @retval EFI_UNSUPPORTED         Logo not found

**/
EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  );

/**
  Show String

  @param [in] SelectedStringNum   It will include what type and index of string
                                  Type high nibble byte of SelectedStringNum
                                  Index low nibble byte of SelectedStringNum   

  @retval EFI_SUCCESS             It is successful to show string on screen

**/
EFI_STATUS
ShowOemString (
  IN UINT8  SelectedStringNum  
  );


/**
  Clear Screen String

  @param None
  
  @retval EFI_SUCCESS             It is successful to show string on screen

**/
EFI_STATUS
ClearScreen (
  VOID
  );

/**
  Clear Top of Screen String

  @param None
  
  @retval EFI_SUCCESS             It is successful to show string on screen

**/
EFI_STATUS
ClearTopScreen (
  VOID
  );  

/**
  Clear Top of Screen String

  @param None
  
  @retval EFI_SUCCESS             It is successful to show string on screen

**/
EFI_STATUS
ClearBottomScreen (
  VOID
  );

#endif
