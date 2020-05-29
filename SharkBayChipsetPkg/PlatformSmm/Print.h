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

#ifndef  _SMM_PRINT_STRING_
#define  _SMM_PRINT_STRING_

#include <Uefi.h>

// Mode 3 80 * 25
#define DEFAULT_VEDIO_MODE         3
#define DEFAULT_MAX_COLUM          80
#define DEFAULT_MAX_ROW            25

#define USE_VIDEO_PAGE             0

//
// Define INT 10h function
//
#define VGA_SET_VIDEO_MODE         0x00
#define VGA_SET_CUESOR_POSITION    0x02
#define VGA_GET_CUESOR_POSITION    0x03
#define VGA_SCROLL_UP_FUNC         0x06
#define VGA_SCROLL_DOWN_FUNC       0x07
#define VGA_SET_ATTRIBUTE          0x09
#define VGA_PRINT_CHARCTER         0x0e
#define VGA_GET_CURRENT_VIDEO_MODE 0x0f

//
// BIOS HOOK Display Function
// Regarding, BIOS_SPECIAL_COMMAND value and function define
// refer to "Cpu\Pentium\Core\Smm\Ia32\flat.asm" file.
//
#define OCCUPY_MEM_BUFFER_ADDR 0x70000
#define OCCUPY_MEM_BUFFER_SIZE 0x800
#define BIOS_SPECIAL_COMMAND   0x0e101
#define VGA_OPTION_INIT        0x01
#define WRITE_TO_VEDIO_RAM     0x02

#define PRINT_ATTRIBUTE_BLOCK  0x70
#define PRINT_ATTRIBUTE_NORMAL 0x07


//
// Function
//
/**
 Get current display mode.

 @param [in, out] MaxColumn     Report Maximum Column.
 @param [in, out] MaxRow        Report Maximum Row.

 @retval None.

**/
EFI_STATUS
GetDisplayMode(
  IN OUT UINTN              *MaxColumn,
  IN OUT UINTN              *MaxRow
  );

/**
 Set display mode. please reference int10 function specification.

 @param [in, out] Mode          Select mode.
 @param [in, out] MaxColumn     Set Maximum Column.
 @param [in, out] MaxRow        Set Maximum Row

 @retval None.

**/
EFI_STATUS
SetDisplayMode(
  IN OUT UINTN              *Mode,
  IN OUT UINTN              *MaxColumn,
  IN OUT UINTN              *MaxRow
  );

/**
 Get current curser address .

 @param [in, out] CurserX       Report row address of curser.
 @param [in, out] CurserY       Report column address of curser.

 @retval None.

**/
VOID
GetCurser(
  IN OUT UINTN              *CurserX,
  IN OUT UINTN              *CurserY
  );

/**
 Set current curser address .

 @param [in]   CurserX          Set row address of curser.
 @param [in]   CurserY          Set column address of curser.

 @retval None.

**/
VOID
SetCurser(
  IN UINTN                  CurserX,
  IN UINTN                  CurserY
  );

/**
 Print a character to display screen.

 @param [in]   WordChar         the unicode character.
 @param [in]   Attribute        set attribute.

 @retval None.

**/
VOID
DisplayCharacter(
  IN CHAR16                 WordChar,
  IN UINT8                  Attribute
  );

/**
 Print string at setting address of display screen.

 @param [in]   CurserX          Set row address of curser.
 @param [in]   CurserY          Set column address of curser.
 @param [in]   String           String buffer address.

 @retval None.

**/
VOID
PrintAt(
  IN UINTN                  CurserX,
  IN UINTN                  CurserY,
  IN CHAR16*                String
  );

/**
 Print string at setting address of display screen.
 And setting display color

 @param [in]   CurserX          Set row address of curser.
 @param [in]   CurserY          Set column address of curser.
 @param [in]   String           String buffer address.
 @param [in]   Attribute        setting color of display string.

 @retval None.

**/
VOID
ColorPrintAt(
  IN UINTN                  CurserX,
  IN UINTN                  CurserY,
  IN CHAR16*                String,
  IN UINT8                  Attribute
  );

/**
 Initialize VGA option rom.

 @param None.

 @retval None.

**/
EFI_STATUS
DisplayInit(
  );

/**
 Select output screen. This function need VGA chipset support.

 @param        DisPlaydevie     Select display device.

 @retval None.

**/
EFI_STATUS
SetDevDisPwrStatus(
  UINT8     DisPlaydevie
  );

void
ClearScreen(
 )
;
#endif

