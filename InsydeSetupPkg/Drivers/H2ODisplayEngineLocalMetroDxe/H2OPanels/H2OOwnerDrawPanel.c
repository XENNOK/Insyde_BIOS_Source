/** @file
  UI Common Controls

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
#include "H2ODisplayEngineLocalMetro.h"
#include "H2OPanels.h"
#include "MetroUi.h"
#include <Guid/H2ODisplayEngineType.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/LayoutLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#define OWNER_DRAW_TIMER_ID 1
STATIC H2O_OWNER_DRAW_PANEL_CLASS        *mH2OOwnerDrawPanelClass = NULL;
#define CURRENT_CLASS                    mH2OOwnerDrawPanelClass


UI_CONTROL *
CreateOwnerDrawPanelChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *Dummy;
  UI_CONTROL                    *OwnerDrawBackground;
  UI_CONTROL                    *OwnerDrawList;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *OwnerDrawSystemInfo;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *OwnerDrawDateValue;
  UI_CONTROL                    *OwnerDrawWeekdayValue;
  UI_CONTROL                    *OwnerDrawTimeValue;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *DummyVerticalLayout3;
  UI_CONTROL                    *OwnerDrawMainboardName;
  UI_CONTROL                    *OwnerDrawSystemName;
  UI_CONTROL                    *OwnerDrawSystemTemperature;
  UI_CONTROL                    *DummyControl6;
  UI_CONTROL                    *DummyHorizontalLayout4;
  UI_CONTROL                    *DummyControl7;
  UI_CONTROL                    *DummyVerticalLayout4;
  UI_CONTROL                    *OwnerDrawCpuName;
  UI_CONTROL                    *OwnerDrawTemperatureName;
  UI_CONTROL                    *OwnerDrawCpuTemperature;
  UI_CONTROL                    *DummyControl8;
  UI_CONTROL                    *DummyControl9;


  Dummy = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, Dummy);
  UiApplyAttributeList (Dummy, L"name='Dummy'");

  OwnerDrawBackground = CreateControl (L"Texture", Dummy);
  CONTROL_CLASS(Dummy)->AddChild (Dummy, OwnerDrawBackground);
  UiApplyAttributeList (OwnerDrawBackground, L"bkcolor='0xFF333333' bkimage='@OwnerDrawBkg' float='true' height='-1' name='OwnerDrawBackground' width='-1'");

  OwnerDrawList = CreateControl (L"HorizontalLayout", Dummy);
  CONTROL_CLASS(Dummy)->AddChild (Dummy, OwnerDrawList);
  UiApplyAttributeList (OwnerDrawList, L"name='OwnerDrawList' padding='13,30,19,35'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", OwnerDrawList);
  CONTROL_CLASS(OwnerDrawList)->AddChild (OwnerDrawList, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1' width='180'");

  DummyControl1 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"bkcolor='0x0' bkimage='@OwnerDrawInsyde' height='31' name='DummyControl1' width='117'");

  OwnerDrawSystemInfo = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OwnerDrawSystemInfo);
  UiApplyAttributeList (OwnerDrawSystemInfo, L"bkcolor='0x0' fontsize='11' name='OwnerDrawSystemInfo' textcolor='0xFFB3B3B3'");

  DummyControl2 = CreateControl (L"Control", OwnerDrawList);
  CONTROL_CLASS(OwnerDrawList)->AddChild (OwnerDrawList, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", OwnerDrawList);
  CONTROL_CLASS(OwnerDrawList)->AddChild (OwnerDrawList, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"name='DummyHorizontalLayout1' padding='20,0,10,0' width='700'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"name='DummyHorizontalLayout2' width='240'");

  DummyControl3 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"bkcolor='0x0' bkimage='@OwnerDrawTime' height='35' name='DummyControl3' width='33'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"name='DummyVerticalLayout2' width='95'");

  OwnerDrawDateValue = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, OwnerDrawDateValue);
  UiApplyAttributeList (OwnerDrawDateValue, L"bkcolor='0x0' fontsize='18' name='OwnerDrawDateValue' textcolor='0xFFF2F2F2'");

  OwnerDrawWeekdayValue = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, OwnerDrawWeekdayValue);
  UiApplyAttributeList (OwnerDrawWeekdayValue, L"bkcolor='0x0' fontsize='18' name='OwnerDrawWeekdayValue' textcolor='0xFF999999'");

  OwnerDrawTimeValue = CreateControl (L"Label", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, OwnerDrawTimeValue);
  UiApplyAttributeList (OwnerDrawTimeValue, L"bkcolor='0x0' fontsize='28' name='OwnerDrawTimeValue' textcolor='0xFFFFFFFF'");

  DummyControl4 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"name='DummyHorizontalLayout3' width='230'");

  DummyControl5 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"bkcolor='0x0' bkimage='@OwnerDrawTemperature' height='42' name='DummyControl5' width='21'");

  DummyVerticalLayout3 = CreateControl (L"VerticalLayout", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyVerticalLayout3);
  UiApplyAttributeList (DummyVerticalLayout3, L"name='DummyVerticalLayout3' width='120'");

  OwnerDrawMainboardName = CreateControl (L"Label", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, OwnerDrawMainboardName);
  UiApplyAttributeList (OwnerDrawMainboardName, L"bkcolor='0x0' fontsize='18' name='OwnerDrawMainboardName' textcolor='0xFFF2F2F2'");

  OwnerDrawSystemName = CreateControl (L"Label", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, OwnerDrawSystemName);
  UiApplyAttributeList (OwnerDrawSystemName, L"bkcolor='0x0' fontsize='18' name='OwnerDrawSystemName' textcolor='0xFF999999'");

  OwnerDrawSystemTemperature = CreateControl (L"Label", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, OwnerDrawSystemTemperature);
  UiApplyAttributeList (OwnerDrawSystemTemperature, L"bkcolor='0x0' fontsize='43' name='OwnerDrawSystemTemperature' textcolor='0xFFFFFFFF'");

  DummyControl6 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl6);
  UiApplyAttributeList (DummyControl6, L"name='DummyControl6'");

  DummyHorizontalLayout4 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyHorizontalLayout4);
  UiApplyAttributeList (DummyHorizontalLayout4, L"name='DummyHorizontalLayout4' width='230'");

  DummyControl7 = CreateControl (L"Control", DummyHorizontalLayout4);
  CONTROL_CLASS(DummyHorizontalLayout4)->AddChild (DummyHorizontalLayout4, DummyControl7);
  UiApplyAttributeList (DummyControl7, L"bkcolor='0x0' bkimage='@OwnerDrawTemperature' height='42' name='DummyControl7' width='21'");

  DummyVerticalLayout4 = CreateControl (L"VerticalLayout", DummyHorizontalLayout4);
  CONTROL_CLASS(DummyHorizontalLayout4)->AddChild (DummyHorizontalLayout4, DummyVerticalLayout4);
  UiApplyAttributeList (DummyVerticalLayout4, L"name='DummyVerticalLayout4' width='120'");

  OwnerDrawCpuName = CreateControl (L"Label", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, OwnerDrawCpuName);
  UiApplyAttributeList (OwnerDrawCpuName, L"bkcolor='0x0' fontsize='18' name='OwnerDrawCpuName' textcolor='0xFFF2F2F2'");

  OwnerDrawTemperatureName = CreateControl (L"Label", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, OwnerDrawTemperatureName);
  UiApplyAttributeList (OwnerDrawTemperatureName, L"bkcolor='0x0' fontsize='18' name='OwnerDrawTemperatureName' textcolor='0xFF999999'");

  OwnerDrawCpuTemperature = CreateControl (L"Label", DummyHorizontalLayout4);
  CONTROL_CLASS(DummyHorizontalLayout4)->AddChild (DummyHorizontalLayout4, OwnerDrawCpuTemperature);
  UiApplyAttributeList (OwnerDrawCpuTemperature, L"bkcolor='0x0' fontsize='43' name='OwnerDrawCpuTemperature' textcolor='0xFFFFFFFF'");

  DummyControl8 = CreateControl (L"Control", OwnerDrawList);
  CONTROL_CLASS(OwnerDrawList)->AddChild (OwnerDrawList, DummyControl8);
  UiApplyAttributeList (DummyControl8, L"bkcolor='0x0' bkimage='@OwnerDrawSeparator' height='86' name='DummyControl8' width='13'");

  DummyControl9 = CreateControl (L"Control", OwnerDrawList);
  CONTROL_CLASS(OwnerDrawList)->AddChild (OwnerDrawList, DummyControl9);
  UiApplyAttributeList (DummyControl9, L"bkcolor='0x0' bkimage='@OwnerDrawInsydeH2O' height='73' name='DummyControl9' padding='10,0,10,0' width='62'");

  return Dummy;
}

CHAR16  mWeekdayStr[7][4] = {L"SUN", L"MON", L"TUE", L"WED", L"THU", L"FRI", L"SAT"};

CHAR16 *
GetWeekdayStr (
  IN EFI_TIME                             *Time
  )
{
  UINT32                                   CenturyOffset;
  UINT16                                   Year;
  UINT32                                   YearOffset;
  UINT32                                   MonthOffset[] = {0,3,3,6,1,4,6,2,5,0,3,5};
  UINT32                                   Weekday;

  if (Time == NULL) {
    return L"ERROR";
  }

  if (Time->Year < 1900 || Time->Year > 2099) {
    return L"ERROR";
  }

  if (Time->Year >= 2000) {
    CenturyOffset = 6;
    Year = Time->Year - 2000;
  } else {
    CenturyOffset = 0;
    Year = Time->Year - 1900;
  }

  YearOffset = Year + Year / 4;

  Weekday = CenturyOffset + YearOffset + MonthOffset[Time->Month - 1] + (UINTN) Time->Day;
  if (IsLeapYear (Time->Year) && Time->Month <= 2) {
    Weekday--;
  }
  Weekday %= 7;

  return mWeekdayStr[Weekday];
}

/**
  Acquire the string associated with the Index from smbios structure and return it.
  The caller is responsible for free the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS            Get index string successfully.
  @retval   EFI_INVALID_PARAMETER  Index is zero. It is invalid value.
  @retval   EFI_ABORTED            Get missing string fail .
  @retval   EFI_OUT_OF_RESOURCES   Allocate memory fail.
  @retval   Other                  Get setup browser data fail.

**/
EFI_STATUS
GetOptionalStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  )
{
  UINTN                                 StrSize;
  CHAR16                                *StringBuffer;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    //
    return EFI_ABORTED;
  }

  StringBuffer = AllocatePool (StrSize * sizeof (CHAR16));
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AsciiStrToUnicodeStr (OptionalStrStart, StringBuffer);

  *String = StringBuffer;

  return EFI_SUCCESS;
}

CHAR16 *
GetSystemInfoStr (
  VOID
  )
{
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_STATUS                  Status;
  SMBIOS_TABLE_TYPE4          *Type4Record;
  SMBIOS_TABLE_TYPE17         *Type17Record;
  SMBIOS_TABLE_TYPE19         *Type19Record;
  CHAR16                      *ProcessorVerStr;
  CHAR16                      *MemClockSpeedStr;
  CHAR16                      *MemSizeStr;
  CHAR16                      *InfoStr;
  CHAR16                      *OrgInfoStr;
  UINTN                       InfoStrSize;
  UINT64                      MemorySize;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  InfoStr      = NULL;
  InfoStrSize  = 0;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  ProcessorVerStr  = NULL;
  MemClockSpeedStr = NULL;
  MemSizeStr       = NULL;

  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      GetOptionalStringByIndex ((CHAR8*)((UINT8*) Type4Record + Type4Record->Hdr.Length), Type4Record->ProcessorVersion, &ProcessorVerStr);
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;

     if (Type17Record->ConfiguredMemoryClockSpeed != 0) {
        MemClockSpeedStr = CatSPrint (NULL, L"DRAM Frequency: %d MHz", Type17Record->ConfiguredMemoryClockSpeed);
      }
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS) {
      Type19Record = (SMBIOS_TABLE_TYPE19 *) Record;

      if (Type19Record->StartingAddress == 0xFFFFFFFF && Type19Record->EndingAddress == 0xFFFFFFFF) {
        MemorySize = (RShiftU64((Type19Record->ExtendedEndingAddress - Type19Record->ExtendedStartingAddress), 30) + 1);
        MemSizeStr = CatSPrint (NULL, L"Memory Size: %d GB", MemorySize);
      } else {
        MemorySize = (RShiftU64((Type19Record->EndingAddress - Type19Record->StartingAddress), 10) + 1);
        MemSizeStr = CatSPrint (NULL, L"Memory Size: %d MB", MemorySize);
      }
    }
  } while(Status == EFI_SUCCESS);

  if (ProcessorVerStr != NULL) {
    InfoStr = ProcessorVerStr;
  }
  if (MemClockSpeedStr != NULL) {
    if (InfoStr == NULL) {
      InfoStr = MemClockSpeedStr;
    } else {
      OrgInfoStr = InfoStr;
      InfoStr = CatSPrint (NULL, L"%s\n%s", OrgInfoStr, MemClockSpeedStr);
      FreePool (OrgInfoStr);
    }
  }
  if (MemSizeStr != NULL) {
    if (InfoStr == NULL) {
      InfoStr = MemSizeStr;
    } else {
      OrgInfoStr = InfoStr;
      InfoStr = CatSPrint (NULL, L"%s\n%s", OrgInfoStr, MemSizeStr);
      FreePool (OrgInfoStr);
    }
  }

  return InfoStr;
}

EFI_STATUS
UpdateOwnerDrawText (
  IN UI_CONTROL                            *PanelControl,
  IN CHAR16                                *ControlName,
  IN CHAR16                                *TextStr
  )
{
  UI_MANAGER                               *Manager;
  UI_CONTROL                               *Control;

  if (PanelControl == NULL || ControlName == NULL || TextStr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Manager = PanelControl->Manager;

  Control = Manager->FindControlByName (Manager, ControlName);
  UiSetAttribute (Control, L"text", TextStr);

  return EFI_SUCCESS;
}

EFI_STATUS
OwnerDrawTimerFunc (
  IN UI_CONTROL                            *PanelControl
  )
{
  EFI_STATUS                               Status;
  EFI_TIME                                 Time;
  CHAR16                                   String[20];

  Status = gRT->GetTime (&Time, NULL);
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (String, sizeof (String), L"%04d/%02d/%02d", Time.Year, Time.Month, Time.Day);
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawDateValue", String);

    UnicodeSPrint (String, sizeof (String), L"%s", GetWeekdayStr (&Time));
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawWeekdayValue", String);

    UnicodeSPrint (String, sizeof (String), L"%02d:%02d:%02d", Time.Hour, Time.Minute, Time.Second);
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawTimeValue", String);
  }

  UpdateOwnerDrawText (PanelControl, L"OwnerDrawMainboardName"    , L"MAINBOARD");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawSystemName"       , L"SYSTEM");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawSystemTemperature", L"27\xB0" L"C");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawCpuName"          , L"CPU");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawTemperatureName"  , L"TEMPERATURE");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawCpuTemperature"   , L"30\xB0" L"C");

  return EFI_SUCCESS;
}


/**
  Set attribute to UI control data of UI owner draw

  @param[in] Control               Pointer to UI control
  @param[in] Name                  Attribute name
  @param[in] Value                 Attribute string

  @retval EFI_SUCCESS              Perform owner draw success
  @retval EFI_ABORTED              Control or manager data is not found
**/
BOOLEAN
EFIAPI
H2OOwnerDrawPanelSetAttribute (
  IN UI_CONTROL                 *Control,
  IN CHAR16                     *Name,
  IN CHAR16                     *Value
  )
{
  H2O_OWNER_DRAW_PANEL          *This;
  EFI_STATUS                    Status;

  This = (H2O_OWNER_DRAW_PANEL *) Control;

  if (StrCmp (Name, L"RefreshInterval") == 0) {
    This->RefreshInterval = (UINT32) StrToUInt (Value, 10, &Status);
    if (This->RefreshInterval != 0) {
      SetTimer (Control->Wnd, OWNER_DRAW_TIMER_ID, (UINT) This->RefreshInterval, NULL);
    }
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  return TRUE;
}


LRESULT
EFIAPI
H2OOwnerDrawPanelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_OWNER_DRAW_PANEL          *This;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  CHAR16                        *SystemInfoStr;

  Manager = NULL;

  This = (H2O_OWNER_DRAW_PANEL *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_OWNER_DRAW_PANEL *) AllocateZeroPool (sizeof (H2O_OWNER_DRAW_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Manager = Control->Manager;
    CreateOwnerDrawPanelChilds (Control);
    This->OwnerDrawList = Manager->FindControlByName (Manager, L"OwnerDrawList");
    OwnerDrawTimerFunc (Control);

    SystemInfoStr = GetSystemInfoStr ();
    if (SystemInfoStr != NULL) {
      UpdateOwnerDrawText (Control, L"OwnerDrawSystemInfo", SystemInfoStr);
      FreePool (SystemInfoStr);
    }
    break;

  case WM_TIMER:
    if (WParam == OWNER_DRAW_TIMER_ID) {
      OwnerDrawTimerFunc (Control);
    }
    break;

  case WM_DESTROY:
    if (This->RefreshInterval != 0) {
      KillTimer (Hwnd, OWNER_DRAW_TIMER_ID);
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    break;


  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}


H2O_OWNER_DRAW_PANEL_CLASS *
EFIAPI
GetH2OOwnerDrawPanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OOwnerDrawPanel", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = H2OOwnerDrawPanelProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = H2OOwnerDrawPanelSetAttribute;

  return CURRENT_CLASS;
}

