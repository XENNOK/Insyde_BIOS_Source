/** @file
  CPU_11 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CPU_11.h"
#include "tool.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS     Status;
  EFI_INPUT_KEY  InputKey;

  ShowMainPage();

  //
  // choose loop
  //
  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode == SCAN_ESC) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }

    if (InputKey.ScanCode == SCAN_F1) {
      gST->ConOut->ClearScreen(gST->ConOut);
      ShowMainPage();
      continue;
    }

    switch (InputKey.UnicodeChar) {
    
    case '0':
      Function_0();
      SetBranchPageSupplementAppearance();
      break;
    case '1':
      Function_1();
      SetBranchPageSupplementAppearance();
      break;
    case '2':
      Function_2();
      SetBranchPageSupplementAppearance();
      break;
    case '3':
      Function_3();
      SetBranchPageSupplementAppearance();
      break;
    case '4':
      Function_4();
      SetBranchPageSupplementAppearance();
      break;
    case '5':
      Function_5();
      SetBranchPageSupplementAppearance();
      break;
    case '6':
      Function_6();
      SetBranchPageSupplementAppearance();
      break;
    case '7':
      Function_7();
      SetBranchPageSupplementAppearance();
      break;
    case '8':
      Function_8();
      SetBranchPageSupplementAppearance();
      break;
    case '9':
      Function_9();
      SetBranchPageSupplementAppearance();
      break;
    case 'a':
    case 'A':
      Function_A();
      SetBranchPageSupplementAppearance();
      break;
    case 'b':
    case 'B':
      Function_B();
      SetBranchPageSupplementAppearance();
      break;
    case 'c':
    case 'C':
      Function_C();
      SetBranchPageSupplementAppearance();
      break;
    case 'd':
    case 'D':
      Function_D();
      SetBranchPageSupplementAppearance();
      break;
    case 'e':
    case 'E':
      Function_E0();
      SetBranchPageSupplementAppearance();
      break;
    case 'f':
    case 'F':
      Function_E1();
      SetBranchPageSupplementAppearance();
      break;
    case 'g':
    case 'G':
      Function_E2();
      SetBranchPageSupplementAppearance();
      break;
    case 'h':
    case 'H':
      Function_E3();
      SetBranchPageSupplementAppearance();
      break;
    case 'i':
    case 'I':
      Function_E4();
      SetBranchPageSupplementAppearance();
      break;
    case 'j':
    case 'J':
      Function_E5();
      SetBranchPageSupplementAppearance();
      break;
    case 'k':
    case 'K':
      Function_E6();
      SetBranchPageSupplementAppearance();
      break;
    case 'l':
    case 'L':
      Function_E7();
      SetBranchPageSupplementAppearance();
      break;
    case 'm':
    case 'M':
      Function_E8();
      SetBranchPageSupplementAppearance();
      break; 
    case 'n':
    case 'N':
      MsrRead();
      SetBranchPageSupplementAppearance();
      break; 
    case 'o':
    case 'O':
      MsrWrite();
      SetBranchPageSupplementAppearance();
      break; 
    case 'p':
    case 'P':
      ShowFullBrandString();
      SetBranchPageSupplementAppearance();
      break;
    }
  }

  Status = EFI_SUCCESS;

  return Status;
}

/**
  CPUID Function 0.

**/
VOID
Function_0 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  ShiftBit;
  UINT32  i;          // for loop count
  
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 0 - Vender-ID and Largest Standard Function\n\n");

  Index = 0;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // eax store largest standard function
  //
  Print(L"Largest Standard Function: %x\n\n", Eax);

  //
  // vendor ID store order : ebx->edx->ecx
  //
  ShiftBit = 8;
  Print(L"Vender ID: ");
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ebx >> i * ShiftBit));    
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Edx >> i * ShiftBit));    
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ecx >> i * ShiftBit));    
  }
  
  Print(L"\n");

  return;
}

/**
  CPUID Function 1.

**/
VOID
Function_1 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  ShiftBit;
  UINT32  GetBit;

  UINT8   Stepping;
  UINT8   Model;
  UINT8   Family;
  UINT8   ProcessorType;
  UINT8   ExtendedModel;
  UINT8   ExtendedFamily;
  UINT8   FamilyValue;
  UINT8   ModelValue;
  
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 1 - Feature Information\n\n");

  Index = 1;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // feature information
  //
  GetBit = 0xf; // [3:0]
  ShiftBit = 0;
  Stepping = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Stepping       : 0x%.2x\n", Stepping);

  GetBit = 0xf; // [7:4]
  ShiftBit = 4;
  Model = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Model          : 0x%.2x\n", Model);

  GetBit = 0xf; // [11:8]
  ShiftBit = 8;
  Family = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Family         : 0x%.2x\n", Family);

  GetBit = 0x3; // [13:12]
  ShiftBit = 12;
  ProcessorType = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Processor Type : 0x%.2x\n", ProcessorType);

  GetBit = 0xf; // [19:16]
  ShiftBit = 16;
  ExtendedModel = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Extended Model : 0x%.2x\n", ExtendedModel);

  GetBit = 0xff; // [27:20]
  ShiftBit = 20;
  ExtendedFamily = (UINT8)((Eax >> ShiftBit) & GetBit);
  Print(L"Extended Family: 0x%.2x\n", ExtendedFamily);

  ModelValue = (ExtendedModel << 4 ) + Model;
  FamilyValue = ExtendedFamily + Family;
  Print(L"Model Value    : 0x%.2x\n", ModelValue);
  Print(L"Family Value   : 0x%.2x\n", FamilyValue);

  Print(L"Processor Type : ");
  switch (ProcessorType) {
  
  case 0:
    Print(L"Original OEM Processor\n");
    break;
  case 1:
    Print(L"OverDrive® Processor\n");
    break;
  case 2:
    Print(L"Dual Processor\n");
    break;
  case 3:
    Print(L"Intel reserved\n");
    break;
  }

  return;
}

/**
  CPUID Function 2.

**/
VOID
Function_2 (
  VOID
  ) 
{
  UINT32      Index;
  UINT32      Eax;
  UINT32      Ebx;
  UINT32      Ecx;
  UINT32      Edx;
  UINT8       CPUIDTimes;
  UINT32      i;
  UINT8       IndexValue;
  DESCRIPTOR  Descriptor;
  
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 2 - Cache Descriptors\n\n");

  Index = 2;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  CPUIDTimes = (UINT8)Eax;

  for (i = 0; i < CPUIDTimes; i++) {
    if (((Eax >> 31) & 0x1) == 0) {
      IndexValue = (UINT8)(Eax);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Eax >> 8);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Eax >> 16);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Eax >> 24);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);
    }

    if (((Ebx >> 31) & 0x1) == 0) {
      IndexValue = (UINT8)(Ebx);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ebx >> 8);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ebx >> 16);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ebx >> 24);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);
    }

    if (((Ecx >> 31) & 0x1) == 0) {
      IndexValue = (UINT8)(Ecx);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ecx >> 8);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ecx >> 16);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Ecx >> 24);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);
    }

    if (((Edx >> 31) & 0x1) == 0) {
      IndexValue = (UINT8)(Edx);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Edx >> 8);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Edx >> 16);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);

      IndexValue = (UINT8)(Edx >> 24);
      Descriptor = GetDescription(IndexValue);
      Print (L"Description: %a\n", Descriptor.Description);
    }

    Index = 2;
    AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  }

  return;
}

/**
  CPUID Function 3.

**/
VOID
Function_3 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  ShiftBit;
  UINT32  GetBit;
  
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 3 - Processor Serial Number\n\n");

  Index = 1;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);

  //
  // check Function_1 edx bit 18
  // if it support(1) or not(0)
  //
  ShiftBit = 18;
  GetBit = 1;
  if (((Edx >> ShiftBit) & GetBit) == 1) {
    Index = 3;
    AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  } else {
    Print(L"PSN Unsupport!!\n");
  }

  return;
}

/**
  CPUID Function 4.

**/
VOID
Function_4 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  SubIndex;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  Count;

  UINT32  NumberOfProcessorCores;
  UINT32  CacheType;
  UINT32  CacheLevel;
  UINT32  WaysOfAssociativity;
  UINT32  PhysicalLinePartitions;
  UINT32  SystemCoherencyLineSize;
  UINT32  NumberOfSets;
  UINT32  CacheSize;
  
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 4 - Processor Serial Number\n\n");

  Index = 4;
  SubIndex = 0;
  AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);

  //
  // number of processor cores ("plus 1") -> EAX[31:26] + 1
  //
  NumberOfProcessorCores = (Eax >> 26) & 0x3f;
  Print(L"The number of cores: %d\n\n", NumberOfProcessorCores + 1); 

  Index = 4;
  SubIndex = 0;
  Count = 0;
  while (TRUE) {
    AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n", Eax, Ebx, Ecx, Edx);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print(L"The cache %d information: \n", Count);
    
    //
    // Cache Type -> EAX[4:0]
    //
    CacheType = Eax & 0x1f;
    Print(L"Cache Type : ");
    switch (CacheType) {

    case 0:
      Print(L"Null, no more caches\n");
      return;
    case 1:
      Print(L"Data Cache\n");
      break;
    case 2:
      Print(L"Instruction Cache\n");
      break;
    case 3:
      Print(L"Unified Cache\n");
      break;
    default:
      Print(L"Reserved\n");
      break;
    }

    //
    // Cache Level -> EAX[7:5]
    //
    CacheLevel = (Eax >> 5) & 0x7;
    Print(L"Cache Level: %d", CacheLevel);

    //
    // Ways of Associativity ("plus 1") -> EBX[31:22]
    //
    WaysOfAssociativity = (Ebx >> 22) & 0x3ff;
    Print(L"Ways of Associativity: %d\n", WaysOfAssociativity + 1);

    //
    // Physical Line partitions ("plus 1") -> EBX[21:12]
    //
    PhysicalLinePartitions = (Ebx >> 12) & 0x3ff;
    Print(L"Physical Line partitions  : %d\n", PhysicalLinePartitions + 1);

    //
    // System Coherency Line Size ("plus 1") -> EBX[11:0]
    //
    SystemCoherencyLineSize = Ebx & 0xfff;
    Print(L"System Coherency Line Size: %d\n", SystemCoherencyLineSize + 1);
    
    //
    // Number of Sets ("plus 1") -> ECX[31:0]
    //
    NumberOfSets = Ecx;
    Print(L"Number of Sets: %d\n", NumberOfSets + 1);

    //
    // Cache Size -> (Way + 1) * (Partitions + 1) * (Line_Size + 1)  * (Sets + 1)
    //
    CacheSize = (WaysOfAssociativity + 1) * (PhysicalLinePartitions + 1) * \
                (SystemCoherencyLineSize + 1) * (NumberOfSets + 1);
    Print(L"Cache Size: %d\n\n", CacheSize); 

    //
    // continue to show other caches
    //
    SubIndex++;
    Count++;
  }

  return;
}

/**
  CPUID Function 5.

**/
VOID
Function_5 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  SmallestMonitorLineSize;
  UINT32  LargestMonitorLineSize;
  UINT32  SupportForTreatingInterrupts;
  UINT32  ExtensionsSupported;
  UINT32  C0;
  UINT32  C1;
  UINT32  C2;
  UINT32  C3;
  UINT32  C4;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 5 - MONITOR / MWAIT Parameters\n\n");

  Index = 5;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  
  //
  // Smallest monitor line size in bytes -> EAX[15:0]
  //
  SmallestMonitorLineSize = Eax & 0xffff;
  Print(L"Smallest monitor line size: %d (Byte)\n", SmallestMonitorLineSize);

  //
  // Largest monitor line size in bytes -> EBX[15:0]
  //
  LargestMonitorLineSize = Ebx & 0xffff;
  Print(L"Largest monitor line size : %d (Byte)\n", LargestMonitorLineSize);

  //
  // Support for treating interrupts as break-events for MWAIT -> ECX[1]
  //
  SupportForTreatingInterrupts = (Ecx >> 1) & 0x1;
  Print(L"Support for treating interrupts as break-events for MWAIT: %a\n", \
    SupportForTreatingInterrupts ? "Yes" : "No");

  //
  // MONITOR / MWAIT Extensions supported -> ECX[0]
  //
  ExtensionsSupported = Ecx & 0x1;
  Print(L"MONITOR / MWAIT Extensions supported: %a\n", ExtensionsSupported ? "Yes" : "No");

  //
  // Number of C0* sub-states supported using MONITOR / MWAIT -> EDX[3:0]
  //
  C0 = Edx & 0xf;
  Print(L"Number of C0* sub-states supported using MONITOR / MWAIT: %d\n", C0);

  //
  // Number of C1* sub-states supported using MONITOR / MWAIT -> EDX[7:4]
  //
  C1 = (Edx >> 4) & 0xf;
  Print(L"Number of C1* sub-states supported using MONITOR / MWAIT: %d\n", C1);

  //
  // Number of C2* sub-states supported using MONITOR / MWAIT -> EDX[11:8]
  //
  C2 = (Edx >> 8) & 0xf;
  Print(L"Number of C2* sub-states supported using MONITOR / MWAIT: %d\n", C2);

  //
  // Number of C3* sub-states supported using MONITOR / MWAIT -> EDX[15:12]
  //
  C3 = (Edx >> 12) & 0xf;
  Print(L"Number of C3* sub-states supported using MONITOR / MWAIT: %d\n", C3);

  //
  // Number of C4* sub-states supported using MONITOR / MWAIT -> EDX[19:16]
  //
  C4 = (Edx >> 16) & 0xf;
  Print(L"Number of C4* sub-states supported using MONITOR / MWAIT: %d\n", C4);

  return;
}

/**
  CPUID Function 6.

**/
VOID
Function_6 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  DTS;
  UINT32  IntelTurboBoost;
  UINT32  ARAT;
  UINT32  PLN;
  UINT32  ECMD;
  UINT32  PTM;

  UINT32  NumberOfInterruptThresholds;
  
  UINT32  PerformanceEnergyBiasCapability;
  UINT32  ACNT2Capability;
  UINT32  HardwareCoordinationFeedbackCapability;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 6 - Digital Thermal Sensor and Power Management Parameters\n\n");

  Index = 6;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Digital Thermal Sensor (DTS) capability -> EAX[0]
  //
  DTS = Eax & 0x1;
  Print(L"Digital Thermal Sensor (DTS) capability: %a\n", DTS ? "Yes" : "No");

  //
  // Intel® Turbo Boost Technology capability -> EAX[1]
  //
  IntelTurboBoost = (Eax >> 1) & 0x1;
  Print(L"Intel Turbo Boost Technology capability: %a\n", IntelTurboBoost ? "Yes" : "No");

  //
  // Always Running APIC Timer (ARAT) capability -> EAX[2]
  //
  ARAT = (Eax >> 2) & 0x1;
  Print(L"Always Running APIC Timer (ARAT) capability: %a\n", ARAT ? "Yes" : "No");

  //
  // Power Limit Notification (PLN) capability -> EAX[4]
  //
  PLN = (Eax >> 4) & 0x1;
  Print(L"Power Limit Notification (PLN) capability  : %a\n", PLN ? "Yes" : "No");

  //
  // Extended Clock Modulation Duty (ECMD) capability -> EAX[5]
  //
  ECMD = (Eax >> 5) & 0x1;
  Print(L"Extended Clock Modulation Duty (ECMD) capability: %a\n", ECMD ? "Yes" : "No");

  //
  // Package Thermal Management (PTM) capability -> EAX[6]
  //
  PTM = (Eax >> 6) & 0x1;
  Print(L"Package Thermal Management (PTM) capability: %a\n", PTM ? "Yes" : "No");

  //
  // Number of Interrupt Thresholds -> EBX[3:0]
  //
  NumberOfInterruptThresholds = Ebx & 0xf;
  Print(L"Number of Interrupt Thresholds: %d\n", NumberOfInterruptThresholds);

  //
  // Performance-Energy Bias capability -> ECX[3]
  //
  PerformanceEnergyBiasCapability = (Ecx >> 3) & 0x1;
  Print(L"Performance-Energy Bias capability: %a\n", PerformanceEnergyBiasCapability ? "Yes" : "No");

  //
  // ACNT2 Capability -> ECX[1]
  //
  ACNT2Capability = (Eax >> 1) & 0x1;
  Print(L"ACNT2 Capability: %a\n", ACNT2Capability ? "Yes" : "No");

  //
  // Hardware Coordination Feedback capability -> ECX[0]
  //
  HardwareCoordinationFeedbackCapability = Ecx & 0x1;
  Print(L"Hardware Coordination Feedback capability: %a\n",       \
          HardwareCoordinationFeedbackCapability ? "Yes" : "No");

  return;
}

/**
  CPUID Function 7.

**/
VOID
Function_7 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  SubIndex;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  MaximumSupportedLeaf;

  UINT32  INVPCID;
  UINT32  EnhancedRepMovsbStosb;
  UINT32  SMEP;
  UINT32  FSGSBASE;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 7 - Structured Extended Feature Flags Enumeration\n\n");

  Index = 7;
  SubIndex = 0;
  AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  if ((Eax == 0) && (Ebx == 0) && (Ecx == 0) && (Edx == 0)) {
    Print(L"No sub-leaves are supported.\n");
  } else {
    //
    // Reports the maximum supported leaf 7 sub-leaf -> EAX[31:0]
    //
    MaximumSupportedLeaf = Eax;
    Print(L"Reports the maximum supported leaf 7 sub-leaf: %d\n", MaximumSupportedLeaf);

    //
    // INVPCID. If 1,
    // supports INVPCID instruction for system software that manages processcontext identifiers -> EBX[10]
    //
    INVPCID = (Ebx >> 10) & 0x1;
    Print(L"Supports INVPCID instruction: %a\n", INVPCID ? "Yes" : "No");

    //
    // Supports Enhanced REP MOVSB/STOSB if 1 -> EBX[9]
    //
    EnhancedRepMovsbStosb = (Ebx >> 9) & 0x1;
    Print(L"Supports Enhanced REP MOVSB/STOSB: %a\n", EnhancedRepMovsbStosb ? "Yes" : "No");

    //
    // SMEP. Supports Supervisor Mode Execution Protection if 1 -> EBX[7]
    //
    SMEP = (Ebx >> 7) & 0x1;
    Print(L"Supports Supervisor Mode Execution Protection: %a\n", SMEP ? "Yes" : "No");

    //
    // FSGSBASE. Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE if 1 -> EBX[0]
    //
    FSGSBASE = Ebx & 0x1;
    Print(L"Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE : %a\n", FSGSBASE ? "Yes" : "No");
  }

  return;
}

/**
  CPUID Function 8.

**/
VOID
Function_8 (
  VOID
  ) 
{
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 8 - Reserved\n\n");

  Print(L"This function is reserved\n");

  return;
}

/**
  CPUID Function 9.

**/
VOID
Function_9 (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  ValueOfPlatformDacCapMsrBits;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function 9 - Direct Cache Access (DCA) Parameters\n\n");

  Index = 9;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Value of PLATFORM_DCA_CAP MSR Bits -> EAX[31:0]
  //
  ValueOfPlatformDacCapMsrBits = Eax;
  Print(L"Value of PLATFORM_DCA_CAP MSR Bits: %d\n", ValueOfPlatformDacCapMsrBits);

  return;
}

/**
  CPUID Function A.

**/
VOID
Function_A (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  Length;
  UINT32  BitWidth;
  UINT32  Number;
  UINT32  VersionId;

  UINT32  BranchMispredictsRetired;
  UINT32  BranchInstructionsRetired;
  UINT32  LastLevelCacheMisses;
  UINT32  LastLevelCacheReferences;
  UINT32  ReferenceCycles;
  UINT32  InstructionsRetired;
  UINT32  CoreCycles;

  UINT32  NumberOfBitsInTheFixedCounters;
  UINT32  NumberOfFixedCounters;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function A - Architectural Performance Monitor Features\n\n");

  Index = 0xa;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Length of EBX bit vector to enumerate architectural performance monitoring events -> EAX[31:24]
  //
  Length = (Eax >> 24) & 0xff;
  Print(L"Length of EBX bit vector to enumerate architectural performance monitoring events: %d\n", Length);

  //
  // Bit width of general-purpose performance monitoring counters -> EAX[23:16]
  //
  BitWidth = (Eax >> 16) & 0xff;
  Print(L"Bit width of general-purpose performance monitoring counters: %d\n", BitWidth);

  //
  // Number of general-purpose performance monitoring counters per logical processor -> EAX[15:8]
  //
  Number = (Eax >> 8) & 0xff;
  Print(L"Number of general-purpose performance monitoring counters per logical processor: %d\n", Number);

  //
  // Version ID of architectural performance monitoring -> EAX[7:0]
  //
  VersionId = Eax & 0xff;
  Print(L"Version ID of architectural performance monitoring: %d\n", VersionId);

  //
  // Branch Mispredicts Retired; 0 = supported -> EBX[6]
  //
  BranchMispredictsRetired = (Ebx >> 6) & 0x1;
  Print(L"Branch Mispredicts Retired : %a\n", BranchMispredictsRetired ? "Unsupported" : "Supported");

  //
  // Branch Instructions Retired; 0 = supported -> EBX[5]
  //
  BranchInstructionsRetired = (Ebx >> 5) & 0x1;
  Print(L"Branch Instructions Retired: %a\n", BranchInstructionsRetired ? "Unsupported" : "Supported");

  //
  // Last Level Cache Misses; 0 = supported -> EBX[4]
  //
  LastLevelCacheMisses = (Ebx >> 4) & 0x1;
  Print(L"Last Level Cache Misses: %a\n", LastLevelCacheMisses ? "Unsupported" : "Supported");

  //
  // Last Level Cache References; 0 = supported -> EBX[3]
  //
  LastLevelCacheReferences = (Ebx >> 3) & 0x1;
  Print(L"Last Level Cache References: %a\n", LastLevelCacheReferences ? "Unsupported" : "Supported");

  //
  // Reference Cycles; 0 = supported -> EBX[2]
  //
  ReferenceCycles = (Ebx >> 2) & 0x1;
  Print(L"Reference Cycles: %a\n", ReferenceCycles ? "Unsupported" : "Supported");

  //
  // Instructions Retired; 0 = supported -> EBX[1]
  //
  InstructionsRetired = (Ebx >> 1) & 0x1;
  Print(L"Instructions Retired: %a\n", InstructionsRetired ? "Unsupported" : "Supported");

  //
  // Core Cycles; 0 = supported -> EBX[0]
  //
  CoreCycles = Ebx & 0x1;
  Print(L"Core Cycles: %a\n", CoreCycles ? "Unsupported" : "Supported");

  //
  // Number of Bits in the Fixed Counters -> EDX[12:5]
  //
  NumberOfBitsInTheFixedCounters = (Edx >> 5) & 0xff;
  Print(L"Number of Bits in the Fixed Counters: %d\n", NumberOfBitsInTheFixedCounters);
  
  //
  // Number of Fixed Counters -> EDX[4:0]
  //
  NumberOfFixedCounters = Edx & 0x1f;
  Print(L"Number of Fixed Counters: %d\n", NumberOfFixedCounters);

  return;
}

/**
  CPUID Function B.

**/
VOID
Function_B (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  SubIndex;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  LevelType;
  UINT32  LevelNumber;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function B - x2APIC Features / Processor Topology\n");  

  SubIndex = 0;
  while (TRUE) {
    Index = 0xb;
    AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);

    if ((Eax == 0) && (Ebx == 0)) {
      break;
    } else {

      gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print(L"\nEAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x", Eax, Ebx, Ecx, Edx);
      gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

      if (SubIndex == 0) {
        Print(L"\nThread Level Processor Topology\n");
      } else if (SubIndex == 1) {
        Print(L"\nCore Level Processor Topology\n");
      } else if (SubIndex >= 2) {
        Print(L"\nCore Level Processor Topology\n");
      }

      //
      // Level Type (0=Invalid, 1=Thread, 2=Core) -> ECX[15:8]
      //
      LevelType = (Ecx >> 8) & 0xff;
      Print(L"Level Type  : ", LevelType);
      switch(LevelType) {

      case 0:
        Print(L"Invalid\n");
        break;

      case 1:
        Print(L"Thread\n");
        break;

      case 2:
        Print(L"Core\n");
        break;
      }

      //
      // Level Number -> ECX[7:0]
      //
      LevelNumber = Ecx & 0xff;
      Print(L"Level Number: %d\n", LevelNumber);

      SubIndex++;
    }
  }

  return;
}

/**
  CPUID Function C.

**/
VOID
Function_C (
  VOID
  ) 
{
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function C - Reserved\n\n");

  Print(L"This function is reserved\n");

  return;
}

/**
  CPUID Function D.

**/
VOID
Function_D (
  VOID
  ) 
{
  UINT32  Index;
  UINT32  SubIndex;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  LowerValidBit;
  UINT32  MaximumSizeRequiredByEnabledFeatures;
  UINT32  MaximumSizeOfTheXsaveXrstorSaveArea;
  UINT32  UpperValidBit;

  UINT32  XSAVEOPT;

  UINT32  SizeOfTheSaveAreaForAnExtendedState;
  UINT32  TheOffsetInBytesOfTheSaveArea;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Standard Function D - XSAVE Features");

  //
  // EAX = 0xd, ECX = 0
  //
  Index = 0xd;
  SubIndex = 0;
  AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"\n\nEAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Reports the valid bit fields of the lower 32 bits of the XFEATURE_ENABLED_MASK register (XCR0) -> EAX[31:0]
  // If a bit is 0, the corresponding bit field in XCR0 is reserved.
  //
  LowerValidBit = Eax;
  Print(L"The valid bit fields of the lower 32 bits of the XCR0: 0x%.8x\n", LowerValidBit);

  //
  // Maximum size (bytes) required by enabled features in XFEATURE_ENABLED_MASK (XCR0) -> EBX[31:0]
  // May be different than ECX when features at the end of the save area are not enabled.
  //
  MaximumSizeRequiredByEnabledFeatures = Ebx;
  Print(L"Maximum size (bytes) required by enabled features in XFEATURE_ENABLED_MASK: 0x%.8x\n", \
          MaximumSizeRequiredByEnabledFeatures);

  //
  // Maximum size (bytes) of the XSAVE/XRSTOR save area required by all supported features in the processor,
  // i.e all the valid bit fields in XFEATURE_ENABLED_MASK -> ECX[31:0]
  // This includes the size needed for the XSAVE.HEADER.
  //
  MaximumSizeOfTheXsaveXrstorSaveArea = Ecx;
  Print(L"Maximum size (bytes) of the XSAVE/XRSTOR save area required by all supported features in the processor: 0x%.8x\n", \
          MaximumSizeOfTheXsaveXrstorSaveArea);

  //
  // Reports the valid bit fields of the upper 32 bits of the XFEATURE_ENABLED_MASK register (XCR0) -> EDX[31:0]
  // If a bit is 0, the corresponding bit field in XCR0 is reserved.
  //
  UpperValidBit = Edx;
  Print(L"The valid bit fields of the upper 32 bits of the XCR0: 0x%.8x\n", UpperValidBit);

  //
  // EAX = 0xd, ECX = 1
  //
  Index = 0xd;
  SubIndex = 1;
  AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"\nEAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // A value of 1 indicates the processor supports the XSAVEOPT instruction -> EAX[0]
  //
  XSAVEOPT = Eax & 0x1;
  Print(L"Supports the XSAVEOPT instruction: %a\n", XSAVEOPT ? "Yes" : "No");

  //
  // EAX = 0xd, ECX > 1
  //
  SubIndex = 2;
  while (TRUE) {
    Index = 0xd;
    AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);

    if ((Eax == 0) && (Ebx == 0)) {
      break;
    } else {
      gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print(L"\nEAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n", Eax, Ebx, Ecx, Edx);
      gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

      //
      // The size in bytes of the save area for an extended state feature associated with a valid sub-leaf index,
      // n. Each valid sub-leaf index maps to a valid bit in the XFEATURE_ENABLED_MASK register (XCR0) starting 
      // at bit position 2 -> EAX[31:0]
      // This field reports 0 if the sub-leaf index, n, is invalid.
      //
      SizeOfTheSaveAreaForAnExtendedState = Eax;
      Print(L"The size in bytes of the save area for an extended state feature: 0x%.8x\n", \
              SizeOfTheSaveAreaForAnExtendedState);

      //
      // The offset in bytes of the save area from the beginning of the XSAVE/XRSTOR area -> EBX[31:0]
      // This field reports 0 if the sub-leaf index, n, is invalid.
      //
      TheOffsetInBytesOfTheSaveArea = Ebx;
      Print(L"The offset in bytes of the save area from the beginning of the XSAVE/XRSTOR area: 0x%.8x\n", \
              TheOffsetInBytesOfTheSaveArea);

      SubIndex++;
    }
  }  

  return;
}

/**
  CPUID Extended Function 0.

**/
VOID
Function_E0 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  LargestExtendedFunctionNumber;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 0 (E) - Largest Extended Function\n\n");

  Index = 0x80000000;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Largest extended function number supported -> EAX[31:0]
  //
  LargestExtendedFunctionNumber = Eax;
  Print(L"Largest extended function: %x\n", LargestExtendedFunctionNumber);

  return;
}

/**
  CPUID Extended Function 1.

**/
VOID
Function_E1 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  LahfSahf;

  UINT32  Intel64;
  UINT32  RDTSCP;
  UINT32  OneGbPages;
  UINT32  XdBit;
  UINT32  SyscallSysret;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 1 (F) - Extended Feature Bits\n\n");

  Index = 0x80000001;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // LAHF / SAHF -> ECX[0]
  //
  LahfSahf = Ecx & 0x1;
  Print(L"LAHF / SAHF: %a\n", LahfSahf ? "Supported" : "Unsupported");

  //
  // Intel® 64 -> EDX[29]
  //
  Intel64 = (Edx >> 29) & 0x1;
  Print(L"The processor supports Intel 64 Architecture extensions to the IA-32 Architecture: %a\n", 
          Intel64 ? "Yes" : "No");

  //
  // RDTSCP -> EDX[27]
  //
  RDTSCP = (Edx >> 27) & 0x1;
  Print(L"RDTSCP: %a\n", RDTSCP ? "Supported" : "Unsupported");

  //
  // 1 GB Pages -> EDX[26]
  //
  OneGbPages = (Edx >> 26) & 0x1;
  Print(L"The processor supports 1-GB pages: %a\n", OneGbPages ? "Yes" : "No");

  //
  // XD Bit -> EDX[20]
  //
  XdBit = (Edx >> 20) & 0x1;
  Print(L"Execution Disable Bit: %a\n", XdBit ? "Supported" : "Unsupported");
  
  //
  // SYSCALL / SYSRET -> EDX[11]
  //
  SyscallSysret = (Edx >> 11) & 0x1;
  Print(L"The processor supports the SYSCALL and SYSRET instructions: %a\n", SyscallSysret ? "Yes" : "No");

  return;
}

/**
  CPUID Extended Function 2.

**/
VOID
Function_E2 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  i;
  UINT32  ShiftBit;

  ShiftBit = 8;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 2 (G) - Processor Brand String\n\n");

  Index = 0x80000002;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  
  Print(L"Processor Name Part1: "); 

  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Eax >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ebx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ecx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Edx >> ShiftBit * i));
  }

  Print(L"\n");

  return;
}

/**
  CPUID Extended Function 3.

**/
VOID
Function_E3 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  i;
  UINT32  ShiftBit;

  ShiftBit = 8;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 3 (H) - Processor Brand String\n\n");

  Index = 0x80000003;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  
  Print(L"Processor Name Part2: "); 

  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Eax >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ebx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ecx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Edx >> ShiftBit * i));
  }

  Print(L"\n");

  return;
}

/**
  CPUID Extended Function 4.

**/
VOID
Function_E4 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  i;
  UINT32  ShiftBit;

  ShiftBit = 8;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 4 (I) - Processor Brand String\n\n");

  Index = 0x80000004;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  
  Print(L"Processor Name Part3: "); 

  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Eax >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ebx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Ecx >> ShiftBit * i));
  }
  for (i = 0; i < 4; i++) {
    Print(L"%c", (UINT8)(Edx >> ShiftBit * i));
  }

  Print(L"\n");

  return;
}

/**
  CPUID Extended Function 5.

**/
VOID
Function_E5 (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 5 (J) - Reserved\n\n");

  Print(L"This function is reserved\n");

  return;
}

/**
  CPUID Extended Function 6.

**/
VOID
Function_E6 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  CacheSize;
  UINT32  CacheAssociativity;
  UINT32  CacheLineSize;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 6 (K) - Extended L2 Cache Features\n\n");

  Index = 0x80000006;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // L2 Cache size described in 1-KB units -> ECX[31:16]
  //
  CacheSize = (Ecx >> 16) & 0xffff;
  Print(L"L2 Cache size: %d (KB)\n", CacheSize);


  //
  // L2 Cache Associativity -> ECX[15:12]
  //
  CacheAssociativity = (Ecx >> 12) & 0xf;
  Print(L"L2 Cache Associativity: ");
  switch (CacheAssociativity) {
  
  case 0x0:
    Print(L"Disabled\n");
    break;
  case 0x1:
    Print(L"Direct mapped\n");
    break;
  case 0x2:
    Print(L"2-Way\n");
    break;
  case 0x4:
    Print(L"4-Way\n");
    break;
  case 0x6:
    Print(L"8-Way\n");
    break;
  case 0x8:
    Print(L"16-Way\n");
    break;
  case 0xf:
    Print(L"Fully associative\n");
    break;
  }

  //
  // L2 Cache Line Size in bytes -> ECX[7:0]
  //
  CacheLineSize = Ecx & 0xff;
  Print(L"L2 Cache Line size: %d (Byte)\n", CacheLineSize);

  return;
}

/**
  CPUID Extended Function 7.

**/
VOID
Function_E7 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  TscInvariance;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 7 (L) - Advanced Power Management\n\n");

  Index = 0x80000007;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // TSC Invariance -> EDX[8]
  //
  TscInvariance = (Edx >> 8) & 0x1;
  Print(L"TSC Invariance: %a\n", TscInvariance ? "Available" : "Not available");

  return;
}

/**
  CPUID Extended Function 8.

**/
VOID
Function_E8 (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  UINT32  VirtualAddressSize;
  UINT32  PhysicalAddressSize;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"CPUID Extended Function 8 (M) - Virtual and Physical Address Sizes\n\n");

  Index = 0x80000008;
  AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"EAX: %.8x, EBX: %.8x, ECX: %.8x, EDX: %.8x\n\n", Eax, Ebx, Ecx, Edx);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Virtual Address Size -> EAX[15:8]
  //
  VirtualAddressSize = (Eax >> 8) & 0xff;
  Print(L"Virtual Address Size : %d (bits)\n", VirtualAddressSize);

  //
  // Physical Address Size -> EAX[7:0]
  //
  PhysicalAddressSize = Eax & 0xff;
  Print(L"Physical Address Size: %d (bits)\n", PhysicalAddressSize);

  return;
}

/**
  To read MSR.

**/
VOID
MsrRead(
  VOID
  )
{
  POSITION      CursorPosition;
  WRITE_BUFFER  WriteBuffer[InputAmount];
  UINT64        MsrValue;
  UINT32        AddressValue;

  InitializeArray(WriteBuffer);
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  Print(L"Model Specific Registers (MSR) Read (N)\n\n");
  Print(L"Please input the MSR address: 0x");

  AddressValue = InputValue(WriteBuffer, &CursorPosition);
  MsrValue = AsmReadMsr64(AddressValue);

  Print(L"\nEDX: %.8x, EAX: %.8x\n", (UINT32)(MsrValue >> 32), (UINT32)MsrValue);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  return;
}

/**
  To write MSR.

**/
VOID
MsrWrite(
  VOID
  )
{
  POSITION      CursorPosition;
  WRITE_BUFFER  WriteBuffer[InputAmount];
  UINT64        MsrValue;
  UINT64        WriteValue;
  UINT32        AddressValue;
  UINT64        EaxValue;  
  UINT64        EdxValue;

  InitializeArray(WriteBuffer);
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  Print(L"Model Specific Registers (MSR) Write (O)\n\n");
  Print(L"Please input the MSR address: 0x");
  AddressValue = InputValue(WriteBuffer, &CursorPosition);
  MsrValue = AsmReadMsr64(AddressValue);

  Print(L"\nOriginal MSR Value...\n");
  Print(L"EDX: %.8x, EAX: %.8x\n\n", (UINT32)(MsrValue >> 32), (UINT32)MsrValue);

  Print(L"Please input the MSR EDX value: 0x");
  EdxValue = InputValue(WriteBuffer, &CursorPosition);
  
  InitializeArray(WriteBuffer);
  Print(L"\n");

  Print(L"Please input the MSR EAX value: 0x");
  EaxValue = InputValue(WriteBuffer, &CursorPosition);

  WriteValue = EaxValue | (EdxValue << 32);

  AsmWriteMsr64(AddressValue, WriteValue);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  return;
}

/**
  For Function_2 to search descriptor and description.

  @param  DESCRIPTOR    A structure with value, type and description.

  @param  IndexValue    The search index.

**/
DESCRIPTOR
GetDescription(
  UINT8 IndexValue
  )
{
  UINT32 i;
  i = 0;

  while (Descriptors[i].Value != 0xff) {
    if (Descriptors[i].Value == IndexValue) {
      return Descriptors[i];
    } else {
      i++;
    }
  }

  return Descriptors[i];
}

/**
  To show full brand string.

**/
VOID
ShowFullBrandString (
  VOID
  )
{
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  i;
  UINT32  ShiftBit;

  ShiftBit = 8;

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"Full Processor Brand String (P)\n\n");

  for (Index = 0x80000002; Index <= 0x80000004; Index++) {
    AsmCpuid (Index, &Eax, &Ebx, &Ecx, &Edx);

    for (i = 0; i < 4; i++) {
      Print(L"%c", (UINT8)(Eax >> ShiftBit * i));
    }
    for (i = 0; i < 4; i++) {
      Print(L"%c", (UINT8)(Ebx >> ShiftBit * i));
    }
    for (i = 0; i < 4; i++) {
      Print(L"%c", (UINT8)(Ecx >> ShiftBit * i));
    }
    for (i = 0; i < 4; i++) {
      Print(L"%c", (UINT8)(Edx >> ShiftBit * i));
    }
  }

  Print(L"\n");

  return;
}