/** @file

  ACPI Func HW
  
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

#include"ACPI_HW.h"
#include"HugeHeader.h"

VOID
ShowFacsData (
  IN EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *Facs
)
{
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    4,
    25,
    0,
    54
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    4
    );
  Print (
    L"Signature   : [%c%c%c%c]\n",
    Facs->Signature & 0xFF,
    (Facs->Signature >> 8) & 0xFF,
    (Facs->Signature >> 16) & 0xFF,
    (Facs->Signature >> 24) & 0xFF
    );
  SET_CUR_POS (0,5);
  Print (L"Length : 0x%08x\n", Facs->Length);
  SET_CUR_POS (0,6);
  Print (L"HardwareSignature : 0x%08x\n", Facs->HardwareSignature);
  SET_CUR_POS (0,7);
  Print (L"FirmwareWakingVector : 0x%08x\n", Facs->FirmwareWakingVector);
  SET_CUR_POS (0,8);
  Print (L"GlobalLock : 0x%08x\n", Facs->GlobalLock);
  SET_CUR_POS (0,9);
Print (L"Flags : 0x%08x\n", Facs->Flags);
  SET_CUR_POS (0,10);
  Print (
    L"XFirmwareWakingVector : 0x%08x%08x \n", 
    Facs->XFirmwareWakingVector,
    (Facs->XFirmwareWakingVector >> 32)
    );
  SET_CUR_POS (0,11);
  Print (
    L"Version   : 0x%02x\n",
    Facs->Version
    );
}
/**
  To find Facs in FADT.

  @param Fadt   FADT table pointer
  
  @return  Facs table pointer.
**/
EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *
FindAcpiFacsFromFadt (
  IN EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt
  )
{
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *Facs;
  UINT64                                        Data64;

  if (Fadt == NULL) {
    return NULL;
  }

  if (Fadt->Header.Revision < EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
    Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Fadt->FirmwareCtrl;
  } else {
    if (Fadt->FirmwareCtrl != 0) {
      Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Fadt->FirmwareCtrl;
    } else {
      CopyMem (&Data64, &Fadt->XFirmwareCtrl, sizeof(UINT64));
      Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Data64;
    }
  }
  return Facs;
}

VOID
ShowFadtFlags (
  IN EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt
)
{
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    4,
    25,
    0,
    54
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    4
    );
  Print (
    L"Flags              : 0x%x \n", 
    Fadt->Flags
    );
  SET_CUR_POS (0,5);
  Print (
    L"  WBINVD           : %d, WBINVD_Flush     : %d\n", 
    ((Fadt->Flags) & 1),
    (((Fadt->Flags) >> 1) & 1)
    );
  SET_CUR_POS (0,6);
  Print (
    L"  PROC_C1          : %d, P_LVL2_UP        : %d\n",
    (((Fadt->Flags) >> 2) & 1), 
    (((Fadt->Flags) >> 3) & 1)
    );
  SET_CUR_POS (0,7);
  Print (
    L"  PWR_BUTTON       : %d, SLP_BUTTON       : %d\n",
    (((Fadt->Flags) >> 4) & 1), 
    (((Fadt->Flags) >> 5) & 1)
    );
  SET_CUR_POS (0,8);  
  Print (
    L"  FIX_RTC          : %d, RTC_S4           : %d\n", 
    (((Fadt->Flags) >> 6) & 1), 
    (((Fadt->Flags) >> 7) & 1)
    );
  SET_CUR_POS (0,9);  
  Print (
    L"  TMR_VAL_EXT      : %d, DCK_CAP          : %d\n", 
    (((Fadt->Flags) >> 8) & 1), 
    (((Fadt->Flags) >> 9) & 1)
    );
  SET_CUR_POS (0,10);  
  Print (
    L"  RESET_REG_SUP    : %d, SEALED_CASE      : %d\n", 
    (((Fadt->Flags) >> 10) & 1), 
    (((Fadt->Flags) >> 11) & 1)
    );
  SET_CUR_POS (0,11);  
  Print (
    L"  HEADLESS         : %d, CPU_SW_SLP       : %d\n", 
    (((Fadt->Flags) >> 12) & 1), 
    (((Fadt->Flags) >> 13) & 1)
    );
  SET_CUR_POS (0,12);
  Print (
    L"  PCI_EXP_WAK      : %d, USE_PLATFORM_CLOCK : %d\n", 
    (((Fadt->Flags) >> 14) & 1), 
    (((Fadt->Flags) >> 15) & 1)
    );
  SET_CUR_POS (0,13);
  Print (
    L"  REMOTE_POWER_ON_CAPABLE : %d \n", 
    (((Fadt->Flags) >> 16) & 1)
    );  
}

VOID
ShowFadtHeader (
  IN EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt
)
{
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    4,
    25,
    0,
    54
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    4
    );
  Print (
    L"Signature   : [%c%c%c%c]\n",
    Fadt->Header.Signature & 0xFF,
    (Fadt->Header.Signature >> 8) & 0xFF,
    (Fadt->Header.Signature >> 16) & 0xFF,
    (Fadt->Header.Signature >> 24) & 0xFF
    );
  SET_CUR_POS (0,5);
  Print (L"Length      : 0x%08x  (%d)\n", Fadt->Header.Length, Fadt->Header.Length);
  SET_CUR_POS (0,6);
  Print (L"Revision    : 0x%08x  (%d)\n", Fadt->Header.Revision, Fadt->Header.Revision);
  SET_CUR_POS (0,7);
  Print (L"Checksum    : 0x%02x  (%d)\n", Fadt->Header.Checksum, Fadt->Header.Checksum);
  SET_CUR_POS (0,8);
  Print (L"OemId       : [%c%c%c%c%c%c]\n", 
           Fadt->Header.OemId[0], 
           Fadt->Header.OemId[1], 
           Fadt->Header.OemId[2],
           Fadt->Header.OemId[3],
           Fadt->Header.OemId[4],
           Fadt->Header.OemId[5]
           );
  SET_CUR_POS (0,9);
  Print (L"OEM Table ID : %c%c%c%c%c%c%c%c\n",
      Fadt->Header.OemTableId & 0xFF,
      (Fadt->Header.OemTableId >> 8 ) & 0xFF,
      (Fadt->Header.OemTableId >> 16 ) & 0xFF,
      (Fadt->Header.OemTableId >> 24 ) & 0xFF,
      (Fadt->Header.OemTableId >> 32 ) & 0xFF,
      (Fadt->Header.OemTableId >> 40 ) & 0xFF,
      (Fadt->Header.OemTableId >> 48 ) & 0xFF,
      (Fadt->Header.OemTableId >> 56 ) & 0xFF
      );
  SET_CUR_POS (0,10);
  Print (L"OemRevision : 0x%08x \n", Fadt->Header.OemRevision);
  SET_CUR_POS (0,11);
  Print (L"CreatorId   : [%c%c%c%c]\n",
           Fadt->Header.CreatorId & 0xff,
           (Fadt->Header.CreatorId >> 8) & 0xff,
           (Fadt->Header.CreatorId >> 16) & 0xff,
           (Fadt->Header.CreatorId >> 24) & 0xff
           );
  SET_CUR_POS (0,12);
  Print (L"CreatorRev  : 0x%08x \n", Fadt->Header.CreatorRevision);
}

VOID
ShowFadtData (
  IN EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL             *InputEx
)
{
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *Facs;
  EFI_ACPI_DESCRIPTION_HEADER                   *Dsdt;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xdsdt;
  EFI_ACPI_DESCRIPTION_HEADER                   *ShowDsdt = NULL;
  EFI_KEY_DATA HKey;

  CleanFrame (
    EFI_BACKGROUND_BLACK,
    3,
    25,
    0,
    99
    );
  SetCursorPosColor (
    EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY,
    0,
    3
    );
  Print (L"Press F1 to show Header , F2 to show Flags");
  Facs = FindAcpiFacsFromFadt (Fadt);
  if (Facs != NULL) {
    Print (L", F3 to Show FACS");
  }

  Dsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Fadt->Dsdt;
  Xdsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Fadt->XDsdt;
  if (Xdsdt != NULL) {
    Print (L", F4 to Show DSDT");
    ShowDsdt = Xdsdt;
  } else if (Dsdt != NULL){
    Print (L", F4 to Show DSDT");
    ShowDsdt = Dsdt;
  }

  ShowFadtHeader (Fadt);

  while (TRUE) {
    HKey = keyRead(InputEx);
     if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
          CleanFrame (
          EFI_BACKGROUND_BLACK,
          3,
          25,
          0,
          99
          );
        break;
      }
      
      switch (HKey.Key.ScanCode) {
        case SCAN_F1:
          ShowFadtHeader (Fadt);
        break;
        case SCAN_F2:
          ShowFadtFlags (Fadt);
        break;
        case SCAN_F3:
          if (Facs != NULL ) {
            ShowFacsData (Facs);
          }
        break;
        case SCAN_F4:
          if (ShowDsdt != NULL) {
            ShowXRsdtData (
              ShowDsdt,
              Xdsdt == NULL ? ENUM_RSDT : ENUM_XSDT
            );
          }
        break;
      }

  }
}

VOID
ShowXRsdtData (
  IN EFI_ACPI_DESCRIPTION_HEADER                   *XRsdt,
  IN UINT8                                         XsdtOrRsdt
)
{
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    4,
    25,
    0,
    54
    );
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    4
    );
  if (XsdtOrRsdt == ENUM_RSDT) {
    Print (L"Address : 0x%08X\n",XRsdt);
  }  else {
    Print (L"Address : 0x%016LX\n",XRsdt);
  }
  
  SET_CUR_POS (0,5);
  Print (L"Signature : %c%c%c%c\n",
          XRsdt->Signature & 0xFF,
          (XRsdt->Signature >> 8) & 0xFF,
          (XRsdt->Signature >> 16) & 0xFF,
          (XRsdt->Signature >> 24) & 0xFF
          );
  SET_CUR_POS (0,6);
    Print (L"Length : 0x%04X\n",XRsdt->Length);

  SET_CUR_POS (0,7);
  Print (L"Revision : 0x%02X\n",
        XRsdt->Revision
        );
  SET_CUR_POS (0,8);
  Print (L"Checksum : 0x%02X\n",
      XRsdt->Checksum
      );
  SET_CUR_POS (0,9);
  Print(L"OEM ID : %c%c%c%c%c%c\n",
        XRsdt->OemId[0],
        XRsdt->OemId[1],
        XRsdt->OemId[2],
        XRsdt->OemId[3],
        XRsdt->OemId[4],
        XRsdt->OemId[5]
        );
  SET_CUR_POS (0,10);
  Print (L"OEM Table ID : %c%c%c%c%c%c%c%c\n",
        XRsdt->OemTableId & 0xFF,
        (XRsdt->OemTableId >> 8 ) & 0xFF,
        (XRsdt->OemTableId >> 16 ) & 0xFF,
        (XRsdt->OemTableId >> 24 ) & 0xFF,
        (XRsdt->OemTableId >> 32 ) & 0xFF,
        (XRsdt->OemTableId >> 40 ) & 0xFF,
        (XRsdt->OemTableId >> 48 ) & 0xFF,
        (XRsdt->OemTableId >> 56 ) & 0xFF
        );
  SET_CUR_POS (0,11);
  Print (L"OEM Revision : 0x%02X\n",
      XRsdt->OemRevision
      );
  SET_CUR_POS (0,12);
  Print (L"Creator ID : %c%c%c%c\n",
         XRsdt->CreatorId & 0xFF,
         (XRsdt->CreatorId >> 8 ) & 0xFF,
         (XRsdt->CreatorId >> 16 ) & 0xFF,
         (XRsdt->CreatorId >> 24 ) & 0xFF
         );
  SET_CUR_POS (0,13);
  Print(L"CreatorRev : 0x%08X\n",XRsdt->CreatorRevision);
}


VOID
ShowRsdPtrData (
  IN EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp
)
{
  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    4
    );
  Print (L"RSD PTR Address = %08X\n",Rsdp);
  SET_CUR_POS (0,5);
  Print (L"Signature = %c%c%c%c%c%c%c%c\n",
          Rsdp->Signature & 0xFF,
          (Rsdp->Signature >> 8) & 0xFF,
          (Rsdp->Signature >> 16) & 0xFF,
          (Rsdp->Signature >> 24) & 0xFF,
          (Rsdp->Signature >> 32) & 0xFF,
          (Rsdp->Signature >> 40) & 0xFF,
          (Rsdp->Signature >> 48) & 0xFF,
          (Rsdp->Signature >> 56) & 0xFF
          );
  SET_CUR_POS (0,6);
  Print (L"Checksum = 0x%02X\n",
         Rsdp->Checksum
         );
  SET_CUR_POS (0,7);
  Print (L"OEM Table ID: %c%c%c%c%c%c\n",
         Rsdp->OemId[0],
         Rsdp->OemId[1],
         Rsdp->OemId[2],
         Rsdp->OemId[3],
         Rsdp->OemId[4],
         Rsdp->OemId[5]
         );
  SET_CUR_POS (0,8);
  Print (L"Revision = 0x%02X\n",
         Rsdp->Revision
         );
  SET_CUR_POS (0,9);
  Print (L"RSDT Address = 0x%04X\n",Rsdp->RsdtAddress);
  SET_CUR_POS (0,10);
  Print (L"Length = 0x%04X\n",Rsdp->Length);
  SET_CUR_POS (0,11);
  Print (L"XSDT Address = 0x%08X%08X\n",(Rsdp->XsdtAddress >> 32),Rsdp->XsdtAddress);
  SET_CUR_POS (0,12);
  Print (L"ExtendedChecksum = 0x%02X\n",Rsdp->ExtendedChecksum);
}


/**

  This function scan ACPI table in RSDT.

  @param Rsdt      ACPI RSDT
  @param Signature ACPI table signature

  @return ACPI table

**/
VOID *
ScanRSDTTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Rsdt,
  IN UINT32                         Signature
  )
{
  UINTN                              Index;
  UINT32                             EntryCount;
  UINT32                             *EntryPtr;
  EFI_ACPI_DESCRIPTION_HEADER        *Table;

  if (Rsdt == NULL) {
    return NULL;
  }

  EntryCount = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
  
  EntryPtr = (UINT32 *)(Rsdt + 1);
  for (Index = 0; Index < EntryCount; Index ++, EntryPtr ++) {
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(*EntryPtr));
    if (Table->Signature == Signature) {
      return Table;
    }
  }
  
  return NULL;
}

/**
 * RSDT Table Selection.
  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  
*/
UINT32
SelectRSDTTables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_ACPI_DESCRIPTION_HEADER            *Rsdt
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ItemIndex;
  UINT8              ItemCounts;
  UINT8              PageNumber;
  UINT8              ItemPosition;
  UINT8              MaxPages;
  UINT8              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;

  UINT32                                         *EntryPtr;
  EFI_ACPI_DESCRIPTION_HEADER                    *Table;
    EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE    *FacpTable;
  UINT32                                         SelectedSignature;

  ItemsPerPage = TABLES_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedSignature = NO_SELECT_SIGN;
  
  Table = NULL;

  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage);
    MaxPageItemNumber = TABLES_PER_HUGEPAGE;
  } else {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

   
  while (TRUE) {
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * TABLES_PER_HUGEPAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = TABLES_PER_HUGEPAGE;
    }

    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
      EntryPtr = (UINT32 *)(Rsdt + 1);
      EntryPtr += ItemCounts + ItemIndex;
      Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(*EntryPtr));

      SetCursorPosColor (
        EFI_LIGHTGRAY,
        HUGE_TABLE_HORI_MAX,
        (ItemIndex + ITEMLIST_OFFSET)
        );
       Print(L"   %02d %c%c%c%c, Entry : %LX\n",
         (ItemCounts + ItemIndex),
         Table->Signature & 0xFF,
         (Table->Signature >> 8 ) & 0xFF,
         (Table->Signature >> 16 ) & 0xFF,
         (Table->Signature >> 24 ) & 0xFF,
         Table
         );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (TABLES_PER_HUGEPAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
          CleanFrame (
            EFI_BACKGROUND_BLACK,
            (CleanLine + ITEMLIST_OFFSET),
            ((CleanLine + ITEMLIST_OFFSET) + 1),
            55,
            99
            );
        }
      }
    }
    
    SetCursorPosColor (
      EFI_WHITE,
      HUGE_TABLE_HORI_MAX,
      (ItemPosition + ITEMLIST_OFFSET)
      );
    EntryPtr = (UINT32 *)(Rsdt + 1);
    EntryPtr += ItemCounts + ItemPosition;
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(*EntryPtr));
    Print (L"-> %02d %c%c%c%c, Entry : %LX\n",
         (ItemCounts + ItemPosition),
         Table->Signature & 0xFF,
         (Table->Signature >> 8 ) & 0xFF,
         (Table->Signature >> 16 ) & 0xFF,
         (Table->Signature >> 24 ) & 0xFF,
         Table
         );

    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        ItemPosition--;
        
        if (ItemPosition == 0xFF) {
          
          PageNumber --;
          
          if (PageNumber == 0xFF) {
            PageNumber = MaxPages-1;
            ItemPosition = MaxPageItemNumber-1;
          }else{
            ItemPosition = TABLES_PER_HUGEPAGE-1;
          }
        }
			  break;
			case SCAN_DOWN:
        ItemPosition++;
        
        if (ItemPosition >= ItemsPerPage) {
          ItemPosition=0;
          PageNumber++;
          
          if (PageNumber >= MaxPages) {
            PageNumber = 0;
          }
        }				  
			  break;

		  }
    } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

      //
      // return ItemCounts+ItemPosition
      //
      SelectedSignature = Table->Signature;
      if (SelectedSignature == EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE){
        FacpTable = (EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *)((UINTN)(*EntryPtr));
        ShowFadtData (
          FacpTable,
          InputEx
          );
      } else {
        ShowXRsdtData (
          Table,
          ENUM_RSDT
          );
      }
      // break;
		} else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	   SelectedSignature = NO_SELECT_SIGN;
       break;
		}
  }
  
      CleanFrame (
        EFI_BACKGROUND_BLACK,
        3,
        25,
        55,
        99
        );
  return SelectedSignature;
}

/**

  This function scan ACPI table in XSDT.

  @param Xsdt      ACPI XSDT
  @param Signature ACPI table signature

  @return ACPI table

**/
VOID *
ScanXSDTTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Xsdt,
  IN UINT32                         Signature
  )
{
  UINTN                          Index;
  UINT32                         EntryCount;
  UINT64                         EntryPtr;
  UINTN                          BasePtr;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;

  if (Xsdt == NULL) {
    return NULL;
  }

  EntryCount = (Xsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
  
  BasePtr = (UINTN)(Xsdt + 1);
  for (Index = 0; Index < EntryCount; Index ++) {
    CopyMem (&EntryPtr, (VOID *)(BasePtr + Index * sizeof(UINT64)), sizeof(UINT64));
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(EntryPtr));
    if (Table->Signature == Signature) {
      return Table;
    }
  }
  
  return NULL;
}

/**
 * XSDT Table Selection.
  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  
*/
UINT32
SelectXSDTTables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_ACPI_DESCRIPTION_HEADER            *Xsdt
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ItemIndex;
  UINT8              ItemCounts;
  UINT8              PageNumber;
  UINT8              ItemPosition;
  UINT8              MaxPages;
  UINT8              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;

  UINT64                                       EntryPtr;
  UINTN                                        BasePtr;
  EFI_ACPI_DESCRIPTION_HEADER                  *Table;
  EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE    *FacpTable;
  UINT32                                       SelectedSignature;

  ItemsPerPage = TABLES_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedSignature = NO_SELECT_SIGN;
  
  Table = NULL;

  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage);
    MaxPageItemNumber = TABLES_PER_HUGEPAGE;
  } else {
    MaxPages = (UINT8)(TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

   
  while (TRUE) {
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * TABLES_PER_HUGEPAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = TABLES_PER_HUGEPAGE;
    }

    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
      BasePtr = (UINTN)(Xsdt + 1);
      CopyMem (&EntryPtr, (VOID *)(BasePtr + (ItemCounts + ItemIndex) * sizeof(UINT64)), sizeof(UINT64));
      Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(EntryPtr));

      SetCursorPosColor (
        EFI_LIGHTGRAY,
        HUGE_TABLE_HORI_MAX,
        (ItemIndex + ITEMLIST_OFFSET)
        );
       Print(L"   %02d %c%c%c%c, Entry : %LX\n",
         (ItemCounts + ItemIndex),
         Table->Signature & 0xFF,
         (Table->Signature >> 8 ) & 0xFF,
         (Table->Signature >> 16 ) & 0xFF,
         (Table->Signature >> 24 ) & 0xFF,
         Table
         );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (TABLES_PER_HUGEPAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {
            CleanFrame (
              EFI_BACKGROUND_BLACK,
              (CleanLine + ITEMLIST_OFFSET),
              ((CleanLine + ITEMLIST_OFFSET) + 1),
              55,
              99
              );
        }
      }
    }
    
    SetCursorPosColor (
      EFI_WHITE,
      HUGE_TABLE_HORI_MAX,
      (ItemPosition + ITEMLIST_OFFSET)
      );
    BasePtr = (UINTN)(Xsdt + 1);
    CopyMem (&EntryPtr, (VOID *)(BasePtr + (ItemCounts + ItemPosition) * sizeof(UINT64)), sizeof(UINT64));
    Table = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(EntryPtr));
    Print (L"-> %02d %c%c%c%c, Entry : %LX\n",
         (ItemCounts + ItemPosition),
         Table->Signature & 0xFF,
         (Table->Signature >> 8 ) & 0xFF,
         (Table->Signature >> 16 ) & 0xFF,
         (Table->Signature >> 24 ) & 0xFF,
         Table
         );

    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        ItemPosition--;
        
        if (ItemPosition == 0xFF) {
          
          PageNumber --;
          
          if (PageNumber == 0xFF) {
            PageNumber = MaxPages-1;
            ItemPosition = MaxPageItemNumber-1;
          }else{
            ItemPosition = TABLES_PER_HUGEPAGE-1;
          }
        }
			  break;
			case SCAN_DOWN:
        ItemPosition++;
        
        if (ItemPosition >= ItemsPerPage) {
          ItemPosition=0;
          PageNumber++;
          
          if (PageNumber >= MaxPages) {
            PageNumber = 0;
          }
        }				  
			  break;

		  }
    } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

      //
      // return ItemCounts+ItemPosition
      //
		  SelectedSignature = Table->Signature;
      if (SelectedSignature == EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE){
        FacpTable = (EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *)((UINTN)(EntryPtr));
        ShowFadtData (
          FacpTable,
          InputEx
          );
      } else {
        ShowXRsdtData (
          Table,
          ENUM_RSDT
          );
      }
      // break;
		} else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	   SelectedSignature = NO_SELECT_SIGN;
       break;
		}
  }
  
  CleanFrame (
    EFI_BACKGROUND_BLACK,
    3,
    25,
    55,
    99
    );
  return SelectedSignature;
}
