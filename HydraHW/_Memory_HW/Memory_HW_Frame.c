/** @file

  Memory Utility Allocate Page Memory.
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"Memory_HW.h"

/**

  This code Show Page data Which you selected in right frame.

  @param[in]       *InputEx                     Input_ex protocol.
  @param[in]       *MemoryTypeUsage             Memory type.
  @param[in]       *MemoryAllocationService     Memory allocation.
  @param[in]       *AllocationType              allocation type.
  @param[in][out]  *PagesNum                    Pages number.
  @param[in][out]  *AddressNum OPTIONAL         address.

*/
VOID
PageRightDataFrame (
  IN     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN     CHAR16                                 *MemoryTypeUsage,
  IN     CHAR16                                 *MemoryAllocationService,
  IN     CHAR16                                 *AllocationType,
  IN OUT UINTN                                  *PagesNum,
  IN OUT UINTN                                  *AddressNum OPTIONAL
  )
{
  UINT8 BackGroundIndex;
  //
  // Full Background Color
  //
  for (BackGroundIndex = 0; BackGroundIndex < 17; BackGroundIndex++) {

    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + BackGroundIndex)
      );
    
    Print (
      L"                         " //25
      ); 
  }
  //
  // Print right data frame title
  //
  SetCursorPosColor (
  EFI_LIGHTGRAY| EFI_BACKGROUND_BLUE,
  RIGHT_DATA_FRAME_HORI_MIN,
  BASE_TITLE_OFFSET
  );
  
  Print (L"           Data          \n");

  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 1)
    );
  Print (L"Memory Type Usage :");
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 2)
    );
  Print (
    L"  %s",
    MemoryTypeUsage 
    );
  
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 3)
    );
  Print (L"Allocation Service :");
  
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 4)
    );
  Print (
    L"  %s",
    MemoryAllocationService
    );
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 5)
    );
  Print (L"Allocation Type :");
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 6)
    );
  Print (
    L"  %s",
    AllocationType
    );
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLUE,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"How many page(s) you want ?!");
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 7)
    );
  Print (
    L"Page(s) : 0x%02X",
    *PagesNum
    );
  *PagesNum = HDataWrite (
               InputEx,
               1,
               (RIGHT_DATA_FRAME_HORI_MIN + 12),
               (BASE_TITLE_OFFSET + 7)
               );

  if (*PagesNum == 0) {
    *PagesNum = 1;
  }
  
  if ((StrCmp(AllocationType, L"AllocateAddress") == 0) ||
      (StrCmp(AllocationType, L"AllocateMaxAddress") == 0)) {
    SetCursorPosColor (
      EFI_YELLOW| EFI_BACKGROUND_BLUE,
      0,
      (BASE_TITLE_OFFSET + 1)
      );
    Print (L"What Address you want ?!");
    
    SetCursorPosColor (
      EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 8)
      );
    Print (
      L"Address : 0x%08X",
      *AddressNum
      );
    *AddressNum = HDataWrite (
                 InputEx,
                 4,
                 (RIGHT_DATA_FRAME_HORI_MIN + 12),
                 (BASE_TITLE_OFFSET + 8)
                 );
  }
  
}

/**

  This code Show Pool data Which you selected in right frame.

  @param[in]       *InputEx                     Input_ex protocol.
  @param[in]       *MemoryTypeUsage             Memory type.
  @param[in]       *MemoryAllocationService     Memory allocation.
  @param[in][out]  *PoolSize                    Pool size.
  
*/
VOID
PoolRightDataFrame (
  IN     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN     CHAR16                                 *MemoryTypeUsage,
  IN     CHAR16                                 *MemoryAllocationService,
  IN OUT UINTN                                  *PoolSize
  )
{

  UINT8 BackGroundIndex;
  //
  // Full Background Color
  //
  for (BackGroundIndex = 0; BackGroundIndex < 17; BackGroundIndex++) {

    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + BackGroundIndex)
      );
    
    Print (
      L"                         " //25
      ); 
  }
  //
  // Print right data frame title
  //
  SetCursorPosColor (
  EFI_LIGHTGRAY| EFI_BACKGROUND_BLUE,
  RIGHT_DATA_FRAME_HORI_MIN,
  BASE_TITLE_OFFSET
  );
  
  Print (L"           Data          \n");
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 1)
    );
  Print ( L"Memory Type Usage :");
  
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 2)
    );
  Print (
    L"%s",
    MemoryTypeUsage
    );
  
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 3)
    );
  Print (L" Allocation Service :");
  
  SET_CUR_POS (
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 4)
    );
  Print (
    L"%s",
    MemoryAllocationService
    );
  
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLUE,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"Please Input Pool Size !!");
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    RIGHT_DATA_FRAME_HORI_MIN,
    (BASE_TITLE_OFFSET + 5)
    );
  Print (
    L"Size : 0x%04X",
    *PoolSize
    );
  *PoolSize = HDataWrite (
               InputEx,
               2,
               (RIGHT_DATA_FRAME_HORI_MIN + 9),
               (BASE_TITLE_OFFSET + 5)
               );
  
}

/**

  Data Frame.

  @param[in] *DataBuffer                         Data Buffer.

**/
VOID
DataFrame (
  IN UINT8  *DataBuffer
  )
{
  UINT8           HoriPos;
  UINT8           VertPos;

  //
  // print data frame title
  //
  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
  //
  // print 8*16 or 16*16 data frame
  //
  for (VertPos = 0; VertPos < FRAME_MAX_VERT; VertPos++) {
    SetCursorPosColor (
      EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
      0,
      (DATA_FRAME_OFFSET + VertPos)
      );
    Print (
      L"%X0 |",
      VertPos
      );
    for (HoriPos = 0; HoriPos < FRAME_MAX_HOZI; HoriPos++) {
      SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      Print (
        L" %02X",
        DataBuffer[((VertPos * 15) + VertPos) + HoriPos]
        );
    }
    
      SET_COLOR (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY);
      Print (L" |");
      
  }
  SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
}

