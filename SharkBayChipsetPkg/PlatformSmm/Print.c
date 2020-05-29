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

#include <Platform.h>
#include <Print.h>

/**
 Ebable/Disable T-segment. If initialize VGA have some problem
 need use this routine.

 @param [in]   Flag    Boolean.

 @retval None.

**/
VOID
EnDisTseg(
  BOOLEAN Flag
  )
{
}

/**
 Initialize VGA option rom.

 @param  None.

 @retval None.

**/
EFI_STATUS
DisplayInit(
  )
{
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;
//   
//   Regs.X.AX = BIOS_SPECIAL_COMMAND;
//   Regs.H.BL = VGA_OPTION_INIT;
// 
// 
// 
//   mSmmThunk->SmmInt10 (mSmmThunk,
//                        &Regs);
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {
    mSmmThunk->FarCall86 (mSmmThunk, 0xC000, 0x0003, &Regs, NULL, 0);

    Regs.H.AH = VGA_SET_VIDEO_MODE;
    Regs.H.AL = DEFAULT_VEDIO_MODE;  
    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs);
  }
//[-end-120731-IB10820094-modify]//  
  return EFI_SUCCESS;
}

/**
 Print String on the screen.

 @param [in]   String           String buffer which character is Unicode
 @param [in]   Attribute        0x07 normal
                                0x70 high light
 @param [in]   CurserX          Print position of row.
 @param [in]   CurserY          Print position of colum.
 @param [in]   MaxColum         max colum

 @return Always return EFI_SUCCESS.

**/
EFI_STATUS
WriteVgaRam(
  IN CHAR16*                String,
  IN UINT8                  Attribute,
  IN UINTN                  CurserX,
  IN UINTN                  CurserY,
  IN UINTN                  MaxColum
)
{
//#if SMM_INT10_DISABLE   
//   EFI_IA32_REGISTER_SET Regs;
//   UINT8                 ReserveBuffer[OCCUPY_MEM_BUFFER_SIZE];
//   UINT8                 *UseBuffer;
//   UINTN                 Index;
// 
//   //
//   // Point to occupy buffer address
//   //
//   UseBuffer = (UINT8*)((UINTN)OCCUPY_MEM_BUFFER_ADDR);
// 
//   //
//   // Keep occupy buffer value
//   //
//   SmmCopyMemory(ReserveBuffer,UseBuffer, sizeof(ReserveBuffer));
// 
//   //
//   // copy string to buffer
//   //
//   Index = 0;
//   while (String[Index] != 0) { 
//        UseBuffer[Index] = (UINT8)String[Index];
//        Index++;
//   }
// 
//   //
//   // set end of string
//   //
//   UseBuffer[Index] = 0;
//   
//   Regs.X.AX = BIOS_SPECIAL_COMMAND;
//   Regs.H.BL = WRITE_TO_VEDIO_RAM; 
//   Regs.H.BH = Attribute; 
// 
//   // Assign position
//   Regs.X.DX = (UINT16)((MaxColum * CurserY) + CurserX);
// 
//   // Liner address change to segment:offset
//   Regs.X.DS = (UINT16)((UINT64)UseBuffer >> 4);
//   Regs.X.SI = (UINT16)((UINT64)UseBuffer & 0x0f);
// 
//   mSmmThunk->SmmInt10 (mSmmThunk,
//                        &Regs
//                       );
// 
//   //
//   // Restore occupy buffer 
//   //
//   
//   SmmCopyMemory(UseBuffer, ReserveBuffer, sizeof(ReserveBuffer));  

  EFI_IA32_REGISTER_SET Regs;
  CHAR16*               TempString;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {
    // Set cursor position
    Regs.H.AH = VGA_SET_CUESOR_POSITION;
    Regs.H.BH = USE_VIDEO_PAGE;
    Regs.H.DH = (UINT8) CurserY;
    Regs.H.DL = (UINT8) CurserX;
    mSmmThunk->SmmInt10 (mSmmThunk, &Regs);

    // Print string
    TempString = String;
    Regs.H.BH = USE_VIDEO_PAGE;
    Regs.H.BL = Attribute;
    Regs.X.CX = 0x01;
    for ( ; (UINT8)*TempString != 0x00; TempString++) {
      Regs.H.AH = VGA_SET_ATTRIBUTE;
      Regs.H.AL = (UINT8)*TempString;
      mSmmThunk->SmmInt10 (mSmmThunk, &Regs);
      Regs.H.AH = VGA_SET_CUESOR_POSITION;
      Regs.H.DL++;
      mSmmThunk->SmmInt10 (mSmmThunk, &Regs);
      }
  }
//[-end-120731-IB10820094-modify]//  
  return EFI_SUCCESS;
}

/**
 Clear Screen.

 @param  None.

 @retval None.

**/
void
ClearScreen(
 )
{
  UINTN                   MaxColum;
  UINTN                   MaxRow;
  UINTN                   Index;
  UINT16                  ReserveBuffer[DEFAULT_MAX_COLUM + 1];

  GetDisplayMode(&MaxColum,&MaxRow);

  //
  // Fill space character in the buffer
  //
  for (Index = 0; Index < MaxColum; Index++) {
     ReserveBuffer[Index] = L' ';
  }
  ReserveBuffer[Index] = 0;

  //
  // Clear every row
  //
  for (Index = 0; Index < MaxRow; Index++) {
     WriteVgaRam(
        ReserveBuffer,
        PRINT_ATTRIBUTE_NORMAL,
        0,
        Index,
        MaxColum
        );
  }

}

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
  )
{
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {

    Regs.H.AH = VGA_GET_CURRENT_VIDEO_MODE;
    Regs.H.AL = 0;
  
    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs
                      );

    *MaxColumn = (UINTN)Regs.H.AH;

    switch (Regs.H.AL) { 
        case 0x00: // Video mode 0x00
        case 0x01:
            *MaxRow = 25;
            *MaxColumn = 40;           
            break;
        case 0x02:
        case 0x03: // Video mode 0x03
        case 0x07:
            *MaxRow = 25;
            *MaxColumn = 80;           
            break;
        case 0x12: // Video mode 0x12
            *MaxRow = 30;
            *MaxColumn = 80;           
            break;

        default:
            *MaxRow = DEFAULT_MAX_ROW;
            *MaxColumn = DEFAULT_MAX_COLUM;           
            break;
    }
  }
//[-end-120731-IB10820094-modify]//
  return EFI_SUCCESS;
}

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
  ) 
{ 
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {

    Regs.H.AH = VGA_SET_VIDEO_MODE;
    Regs.H.AL = DEFAULT_VEDIO_MODE;  
    
    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs);

    *Mode = DEFAULT_VEDIO_MODE;
    *MaxColumn = DEFAULT_MAX_COLUM;
    *MaxRow = DEFAULT_MAX_ROW;   
  }
//[-end-120731-IB10820094-modify]//
  return EFI_SUCCESS;
}        

/**
 Get current curser address .

 @param [in, out] CurserX       Report row address of curser.
 @param [in, out] CurserY       Report column address of curser.

 @retval None.

**/
VOID
GetCurser(
  IN OUT UINTN                  *CurserX,
  IN OUT UINTN                  *CurserY
  ) 
{
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {
    Regs.H.AH = VGA_GET_CUESOR_POSITION;
    Regs.H.BH = USE_VIDEO_PAGE;

    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs);

    *CurserY = (UINTN)Regs.H.DH;  
    *CurserX = (UINTN)Regs.H.DL;
  }
//[-end-120731-IB10820094-modify]//
  return;
}


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
  ) 
{
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {
    Regs.H.AH = VGA_SET_CUESOR_POSITION;
    Regs.H.BH = USE_VIDEO_PAGE;
    Regs.H.DH = (UINT8)CurserY;  
    Regs.H.DL = (UINT8)CurserX;

    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs);
  }
//[-end-120731-IB10820094-modify]//
  return;
}

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
  ) 
{ 
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;
  UINT8                   AscCode;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {

    AscCode = (UINT8)WordChar;

    Regs.H.AH = VGA_PRINT_CHARCTER; 
    Regs.H.AL = AscCode;
    Regs.X.BX = (UINT16)Attribute;

    mSmmThunk->SmmInt10 (mSmmThunk,
                       &Regs);
  }
//[-end-120731-IB10820094-modify]//
}

/**
 Set color of display string.

 @param [in]   WordChar         the unicode character.
 @param [in]   Attribute        set attribute.
 @param [in]   StringLength     set color length.

 @retval None.

**/
VOID
SetAttribute( 
  IN CHAR16                 WordChar,
  IN UINT8                  Attribute,
  IN UINT16                 StringLength
  ) 
{ 
//#if SMM_INT10_DISABLE   
  EFI_IA32_REGISTER_SET   Regs;
  UINT8                   AscCode;

//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdSmmInt10Enable)) {

    AscCode = (UINT8)' ';

    Regs.H.AH = VGA_SET_ATTRIBUTE;
    Regs.H.AL=AscCode;
    Regs.X.BX=(UINT16)Attribute;
    Regs.X.CX = StringLength;

    mSmmThunk->SmmInt10 (mSmmThunk,
                         &Regs);
  }
//[-end-120731-IB10820094-modify]//
}

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
  ) 
{  
  UINTN                 MaxColum; 
  UINTN                 MaxRow;   

  GetDisplayMode(&MaxColum,&MaxRow);

  WriteVgaRam( 
        String,
        PRINT_ATTRIBUTE_NORMAL,
        CurserX,
        CurserY,
        MaxColum
        );
  return;
}

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
  )
{ 
  UINTN                 MaxColum; 
  UINTN                 MaxRow;   

  GetDisplayMode(&MaxColum,&MaxRow);

  WriteVgaRam( 
        String,
        Attribute,
        CurserX,
        CurserY,
        MaxColum
        );

  return;
}


        
