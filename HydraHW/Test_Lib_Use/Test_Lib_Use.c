#include"Test_Lib_Use.h"

EFI_STATUS Status;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
UINTN                                  Index;
EFI_KEY_DATA                           H_key = {0};

EFI_STATUS
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  UINTN    TextModeNum;
  UINTN    Columns;
  UINTN    Rows;
  INT32    OringinalMode;
  UINTN    OrgColumns;
  UINTN    OrgRows;

  UINT8    Hori;
  UINT8    Vert;
  
  OringinalMode = gST->ConOut->Mode->Mode;
  Status = gST->ConOut->QueryMode (gST->ConOut, OringinalMode, &OrgColumns, &OrgRows);
  ChkTextModeNum (
    80,
    25,
    &TextModeNum
    );
  Status = gST->ConOut->SetMode (gST->ConOut, TextModeNum);
  Status = gST->ConOut->QueryMode (gST->ConOut, TextModeNum, &Columns, &Rows);
  if(EFI_ERROR(Status)){
    Print(L"SetMode ERROR\n");
  }
  Print(L"Oringinal Mode = %d,Columns=%d,Rows=%d\n",OringinalMode,OrgColumns,OrgRows);
  Print(L"Set Mode = %d,Columns=%d,Rows=%d\n",TextModeNum,Columns,Rows);
//  for(Vert=0; Vert<Rows-1;Vert++){
//    for(Hori=0; Hori<Columns; Hori++){
//      Print(L"X");
//    }
//    Print(L"\n");
//  }
  return EFI_SUCCESS;
}
