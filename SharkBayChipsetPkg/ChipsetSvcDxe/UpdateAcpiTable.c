/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return 
 a PCI ROM image for the device is represented or not.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/ChipsetCpuLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DxeOemSvcChipsetLib.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/FrameworkMpService.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/MemInfo.h>
#include <Protocol/PlatformInfo.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/PerfTune.h>
#include <Protocol/AcpiSupport.h>
#include <PchAccess.h>
#include <ChipsetSetupConfig.h>
#include <CpuRegs.h>
//[-start-130327-IB05160426-modify]//
#include <Aspt.h>
//[-end-130327-IB05160426-modify]//

//#define LVT1                            0xfee00350
//#define LVT2                            0xfee00360
//#define SVR                             0xfee000f0
//#define APIC_BAR                        0xfee00900
#define APIC_ENABLE                     0x00000100
//#define MSR_XAPIC_BASE                  0x1b

STATIC
EFI_STATUS
SetApicBase (
  VOID
  )
{
//[-start-121213-IB10820193-modify]//
  AsmWriteMsr64 (MSR_IA32_APIC_BASE, PcdGet32 (PcdCpuLocalApicBaseAddress) | (B_MSR_IA32_APIC_BASE_G_XAPIC+B_MSR_IA32_APIC_BASE_BSP));
//[-end-121213-IB10820193-modify]//

  return EFI_SUCCESS;
}

/**
 This functions initializes APIC.

 @param[in]         CpuIo               Instance of EFI_CPU_IO_PROTOCOL.
                    
 @retval            EFI_SUCCESS         Initial APIC Success
*/
STATIC
EFI_STATUS
InitialApic (
  IN  EFI_CPU_IO2_PROTOCOL               *CpuIo
  )
{
  EFI_STATUS                            Status;
  UINT16                                Mask;
  UINT32                                *ApicAddr;
  UINT32                                Buff;
  UINT8                                 Buff8;
  UINT8                                 Temp;
  EFI_LEGACY_8259_PROTOCOL              *Legacy8259;

  Status = gBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, (VOID **)&Legacy8259);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetApicBase();

  //disable NMI
  Temp = B_PCH_NMI_EN_NMI_EN;
  CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, R_PCH_NMI_EN, 1, &Temp);

  //save current mask
  Mask = 0xffff;
  Status = Legacy8259->SetMode (Legacy8259,
                                Efi8259ProtectedMode,
                                &Mask,
                                NULL
                                );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //Enable APIC
  Buff8 = PchMmRcrb8 (R_PCH_RCRB_OIC + 1);
  Buff8 |= B_PCH_RCRB_OIC_AEN;
  PchMmRcrb8AndThenOr (R_PCH_RCRB_OIC + 1, 0, Buff8);
  //APIC Enable
//[-start-121213-IB10820193-modify]//
  ApicAddr = (UINT32 *)(UINTN)(PcdGet32 (PcdCpuLocalApicBaseAddress) + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
//[-end-121213-IB10820193-modify]//
  Buff = *ApicAddr;
  Buff &= 0xffffff0f;
  Buff |= (UINT32)APIC_ENABLE;
  *ApicAddr = Buff;

  //Set LVT1
//[-start-121213-IB10820193-modify]//
  ApicAddr = (UINT32 *)(UINTN)(PcdGet32 (PcdCpuLocalApicBaseAddress) + APIC_REGISTER_LINT0_VECTOR_OFFSET);
//[-end-121213-IB10820193-modify]//
  Buff = *ApicAddr;
  Buff &= 0xfffe00ff;
  Buff |= 0x00005700;
  *ApicAddr = Buff;

  //Set LVT2
//[-start-121213-IB10820193-modify]//
  ApicAddr = (UINT32 *)(UINTN)(PcdGet32 (PcdCpuLocalApicBaseAddress) + APIC_REGISTER_LINT1_VECTOR_OFFSET);
//[-end-121213-IB10820193-modify]//
  Buff = *ApicAddr;
  Buff &= 0xfffE00ff;
  Buff |= 0x00005400;
  *ApicAddr = Buff;


  return EFI_SUCCESS;
}

/**
 Update MADT table content
 Depends on MPService return data to setup MADT core enable flag and APIC ID.

 @param[in, out]    Table               The table to update

 @retval            EFI_SUCCESS         Update table success
 @retval            EFI_UNSUPPORTED     APIC Mode disabled
*/
STATIC
EFI_STATUS
UpdateMadt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table
  )
{
  EFI_STATUS                                      Status;
  UINT8                                           *CurrPtr = NULL;
  UINT8                                           *EndPtr = NULL;
  EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE     *ApicPtr = NULL;
  UINT8                                           CurrProcessor = 0;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL              *MpService;
  UINTN                                           NumberOfCPUs = 1;
  UINTN                                           MaximumNumberOfCPUs;
  UINTN                                           NumberOfEnabledCPUs;
  UINTN                                           RendezvousIntNumber;
  UINTN                                           RendezvousProcLength;
  EFI_MP_PROC_CONTEXT                             MpContext;
  UINTN                                           BufferSize;
  EFI_CPU_IO2_PROTOCOL                             *CpuIo;
  volatile UINT8                                  *IoapicIndex;
  volatile UINT32                                 *IoapicData;

  Status = EFI_SUCCESS;

  //
  // Find the MP Protocol. This is an MP platform, so MP protocol must be
  // there.
  //
  Status = gBS->LocateProtocol (
    &gFrameworkEfiMpServiceProtocolGuid,
    NULL,
    (VOID **)&MpService
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Determine the number of processors
  //
  MpService->GetGeneralMPInfo (
    MpService,
    &NumberOfCPUs,
    &MaximumNumberOfCPUs,
    &NumberOfEnabledCPUs,
    &RendezvousIntNumber,
    &RendezvousProcLength
    );

  CurrPtr = (UINT8*) &((EFI_ACPI_DESCRIPTION_HEADER*) (Table))[1];
  CurrPtr = CurrPtr + 8; // Size of Local APIC Address & Flag
  EndPtr = (UINT8*) (Table);
  EndPtr = EndPtr + (Table)->Length;
  while (CurrPtr < EndPtr) {
    ApicPtr = (EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC_STRUCTURE*) CurrPtr;
      //
      // Check table entry type
      //
      if (ApicPtr->Type == EFI_ACPI_3_0_PROCESSOR_LOCAL_APIC) {
        BufferSize = sizeof (EFI_MP_PROC_CONTEXT);
        ApicPtr->Flags = 0;
        ApicPtr->ApicId = 0;

        //
        // Look up this processor information
        //
        Status = MpService->GetProcessorContext (
          MpService,
          CurrProcessor,
          &BufferSize,
          &MpContext
          );
        if (!EFI_ERROR (Status)) {
          if (MpContext.Enabled) {
             ApicPtr->Flags = EFI_ACPI_3_0_LOCAL_APIC_ENABLED;
          }
          ApicPtr->ApicId = (UINT8) MpContext.ApicID;
        }

        //
        // Increment the procesor count
        //
        CurrProcessor++;
      } else if (ApicPtr->Type == EFI_ACPI_3_0_IO_APIC) {
        //
        // Get IOAPIC base
        //
        IoapicIndex = (UINT8 *)(UINTN)(R_PCH_IO_APIC_INDEX + ((PchMmRcrb16 (R_PCH_RCRB_OIC) & 0x0ff) << N_PCH_IO_APIC_ASEL));
        IoapicData  = (UINT32 *)(UINTN)(R_PCH_IO_APIC_DATA + ((PchMmRcrb16 (R_PCH_RCRB_OIC) & 0x0ff) << N_PCH_IO_APIC_ASEL));

        //
        // Get APIC ID from Identification Register (Index = 0)
        //
        *IoapicIndex = 0;
        ((EFI_ACPI_3_0_IO_APIC_STRUCTURE *)ApicPtr)->IoApicId = (UINT8)((*IoapicData >> 24) & 0x0F);
      }
      //
      // Go to the next structure in the APIC table
      //
      CurrPtr = CurrPtr + ApicPtr->Length;
    }

  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);
  if (!EFI_ERROR (Status)) {
    // Initialize Apic mode.
    InitialApic (CpuIo);
  }

  return Status;
}

//[-start-130314-IB10820261-add]//
/**
  Update FACP table content

  @param [In Out]   Table           The table to update
  @param [In]       SetupVariable   SETUP Variable pointer

  @return EFI_SUCCESS               Update table success

**/
EFI_STATUS
UpdateFacp (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER    *Table,
  IN CHIPSET_CONFIGURATION               *SetupVariable
  )
{
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE   *FadtPointer5;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE   *FadtPointer3;
  EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE   *FadtPointer1;

  FadtPointer1 = (EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE*) Table;
  FadtPointer3 = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE*) Table;
  FadtPointer5 = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE*) Table;
  if (SetupVariable->AcpiVer == 3) {
//[-start-120718-IB06460422-modify]//
    if (SetupVariable->EnableLowPowerS0Cap) {
      FadtPointer5->Flags |= BIT21;
      FadtPointer5->Flags &= ~BIT9;
    }
//[-end-120718-IB06460422-modify]//
  }
  return EFI_SUCCESS;
}
//[-end-130314-IB10820261-add]//

/**
 Update Dsdt Table according to platform specific.

 @param [in, out]   Table               The table to update
 @param [in]        SetupVariable       SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateDsdt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN CHIPSET_CONFIGURATION             *SetupVariable
  )
{
//[-start-121004-IB10370017-add]//
  PLATFORM_INFO_PROTOCOL                *PlatformInfoProtocol;
//[-end-121004-IB10370017-add]//
  EFI_STATUS                            Status;
  UINT8                                 *DsdtPointer;
  UINT32                                *Signature;
  EFI_GLOBAL_NVS_AREA_PROTOCOL          *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA                   *mGlobalNvsArea;
  EFI_OEM_GLOBAL_NVS_AREA               *mOemGlobalNvsArea;
  VOID                                  *IgdOpRegionPtr;
//[-start-121012-IB03780464-remove]//
//  UINTN                                 BufferSize;
//[-end-121012-IB03780464-remove]//
//[-start-120607-IB06460402-remove]//
//  BOOLEAN                               CpuStatus = FALSE;
//[-end-120607-IB06460402-remove]//
  UINT8                                 Stepping;
  UINT32                                FamilyModel;
  UINT8                                 ThreadCount;
//[-start-120604-IB06150223-modify]//
  UINT8                                 ISCTFlag = 0;
//[-end-120604-IB06150223-modify]//
//[-start-120606-IB06460402-add]//
  UINT64                                TempMsr;
  UINT8                                 MaxRefTemp = 0;
//[-end-120606-IB06460402-add]//
//[-start-121012-IB03780464-remove]//
////[-start-120606-IB03610423-add]//
//  MEM_INFO_PROTOCOL                     *MemoryInfo;
//  UINT8                                 UnitDVMT;
//  UINT8                                 MaxDVMT;
////[-end-120606-IB03610423-add]//
//[-end-121012-IB03780464-remove]//
//[-start-120720-IB06460422-add]//
  UINT32                                TempSignature;
//[-end-120720-IB06460422-add]//
//[-start-121004-IB10370017-modify]//
//[-start-120725-IB05330361-add]//
  UINT8                                 BoardID;
//  UINT8                                 BoardID2;
//[-end-120725-IB05330361-add]//
//[-end-121004-IB10370017-modify]//
  UINT32                                RegEax;
//[-start-121119-IB05330387-add]//
  UINT8                                 PciePortMax;
  UINT8                                 Index;
  DXE_PCH_PLATFORM_POLICY_PROTOCOL      *PchPlatformPolicy;
//[-end-121119-IB05330387-add]//

  Stepping = 0;
  FamilyModel = 0;
//[-start-121120-IB05330387-add]//
  PciePortMax = 0;
  Index = 0;
  PchPlatformPolicy = NULL;
//[-end-121120-IB05330387-add]//
//[-start-121012-IB03780464-remove]//
////[-start-120606-IB03610423-add]//
//  MemoryInfo  = NULL;
//  UnitDVMT    = 0;
//  MaxDVMT     = 0;
////[-end-120606-IB03610423-add]//
//[-end-121012-IB03780464-remove]//
//[-start-120607-IB06460402-remove]//
//  CpuStatus = CheckProcessor ( ProcessorNehalem, &FamilyModel, &Stepping );
//[-end-120607-IB06460402-remove]//
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  IgdOpRegionPtr = NULL;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mGlobalNvsArea = GlobalNvsAreaProtocol->Area;
  mOemGlobalNvsArea = GlobalNvsAreaProtocol->OemArea;

  //
  // Initial virtual battery status.
  //
//[-start-121218-IB10820203-modify]//
  mGlobalNvsArea->BatteryCapacity0 = PcdGet8 (PcdGlobalNvsAreaBatteryCapacity0);
  mGlobalNvsArea->BatteryStatus0 = PcdGet8 (PcdGlobalNvsAreaBatteryStatus0);
//[-end-121218-IB10820203-modify]//
//[-start-121012-IB03780464-remove]//
////[-start-120606-IB03610423-add]//
//  Status = gBS->LocateProtocol ( &gMemInfoProtocolGuid, NULL, &MemoryInfo );
//  ASSERT_EFI_ERROR ( Status );
//
//  //
//  // Follow DVMT 5.0 White Paper to limit the Maximum DVMT
//  //
//  UnitDVMT = 128;    // 128 MB as the unit
//  MaxDVMT  = 128 / UnitDVMT;
//  if ( MemoryInfo->MemInfoData.memSize >= 1024 ) MaxDVMT = ( UINT8 )(  512 / UnitDVMT );
//  if ( MemoryInfo->MemInfoData.memSize >= 1536 ) MaxDVMT = ( UINT8 )(  768 / UnitDVMT );
//  if ( MemoryInfo->MemInfoData.memSize >= 2048 ) MaxDVMT = ( UINT8 )( 1024 / UnitDVMT );
////[-end-120606-IB03610423-add]//
//
////[-start-120606-IB03610423-modify]//
//  mGlobalNvsArea->IgdDvmtMemSize = ( ( SetupVariable->DvmtTotalGfxMemSize == 0xFF ) || ( SetupVariable->DvmtTotalGfxMemSize > MaxDVMT ) ) ? MaxDVMT : SetupVariable->DvmtTotalGfxMemSize;    // Real Size : IgdDvmtMemSize << 17
////[-end-120606-IB03610423-modify]//
//  mGlobalNvsArea->IgdState = PRIMARY_DISPLAY;
//
//  mGlobalNvsArea->DeviceId1              = 0x00000100;           // Device ID 1
//  mGlobalNvsArea->DeviceId2              = 0x80000410;           // Device ID 2
//  mGlobalNvsArea->DeviceId3              = 0x80000300;           // Device ID 3
//  mGlobalNvsArea->DeviceId4              = 0x00000301;           // Device ID 4
//  mGlobalNvsArea->DeviceId5              = 0x05;                 // Device ID 5
//  mGlobalNvsArea->NumberOfValidDeviceId  = 4;                    // Number of Valid Device IDs
//  mGlobalNvsArea->CurrentDeviceList      = 0x0F;                 // Default setting
//  mGlobalNvsArea->PreviousDeviceList     = 0x0F;
//[-end-121012-IB03780464-remove]//

  Status = GetTheadCoreCount(&ThreadCount,NULL);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsArea->ThreadCount = ThreadCount;

  mGlobalNvsArea->ApicEnable = SetupVariable->IoApicMode;
  mGlobalNvsArea->OnboardCom = SetupVariable->ComPortA;
  mGlobalNvsArea->OnboardComCir = SetupVariable->ComPortB;

//mGlobalNvsArea->PlatformCpuId = FamilyModel | (UINT32)Stepping;
  mGlobalNvsArea->PlatformCpuId = RegEax;
//[-start-121012-IB03780464-remove]//
//  if ( !mGlobalNvsArea->IgdOpRegionAddress ) {
//    BufferSize = 0x2000;
//    Status = gBS->AllocatePool (
//                    EfiACPIMemoryNVS,
//                    BufferSize,
//                    &IgdOpRegionPtr
//                    );
//    if ( EFI_ERROR ( Status ) ) {
//      return Status;
//    }
//    ZeroMem ( IgdOpRegionPtr, BufferSize );
//
//    mGlobalNvsArea->IgdOpRegionAddress = ( UINT32 )( UINTN )( IgdOpRegionPtr );
//  }
//[-end-121012-IB03780464-remove]//
//[-start-120607-IB06460402-modify]//
  if (SetupVariable->AutoThermalReporting) {
    TempMsr = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr &= B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK;
    MaxRefTemp  = (UINT8) RShiftU64 (TempMsr, N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET);
    mGlobalNvsArea->CriticalThermalTripPoint = MaxRefTemp + 5;
  } else {
    mGlobalNvsArea->CriticalThermalTripPoint = SetupVariable->CriticalThermalTripPoint;
  }
//[-end-120607-IB06460402-modify]//
  mGlobalNvsArea->Ac0FanSpeed = SetupVariable->Ac0FanSpeed;
  mGlobalNvsArea->Ac1FanSpeed = SetupVariable->Ac1FanSpeed;
//[-start-120607-IB06460402-modify]//
//[-start-120403-IB05300305-modify]//
  if (SetupVariable->AutoThermalReporting) {
    TempMsr = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr &= B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET;
//[-start-120731-IB07360213-remove]//
//    mGlobalNvsArea->Ac1TripPoint = MaxRefTemp - (UINT8) RShiftU64 (TempMsr, N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET);
//[-end-120731-IB07360213-remove]//
    mGlobalNvsArea->Ac0TripPoint = MaxRefTemp - (UINT8) RShiftU64 (TempMsr, N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET);
  } else {
//[-start-120731-IB07360213-remove]//
//    mGlobalNvsArea->Ac1TripPoint = SetupVariable->ActiveTripPointLowFanSpeed;
//[-end-120731-IB07360213-remove]//
    mGlobalNvsArea->Ac0TripPoint = SetupVariable->ActiveTripPointHighFanSpeed;
  }

//[-start-120731-IB07360213-add]//
  mGlobalNvsArea->Ac1TripPoint = SetupVariable->ActiveTripPointLowFanSpeed;
//[-end-120731-IB07360213-add]//
  
//[-end-120403-IB05300305-modify]//
  if (SetupVariable->AutoThermalReporting) {
    mGlobalNvsArea->PassiveThermalTripPoint = mGlobalNvsArea->CriticalThermalTripPoint+3;
  } else {
    mGlobalNvsArea->PassiveThermalTripPoint = SetupVariable->PassiveThermalTripPoint;
  }
//[-end-120607-IB06460402-modify]//
  mGlobalNvsArea->PassiveTc1Value = SetupVariable->PassiveTc1Value;
  mGlobalNvsArea->PassiveTc2Value = SetupVariable->PassiveTc2Value;
  mGlobalNvsArea->PassiveTspValue = SetupVariable->PassiveTspValue;
//[-start-121218-IB10820203-modify]//
  mGlobalNvsArea->PlatformFlavor = PcdGet8 (PcdGlobalNvsAreaPlatformFlavor); //0x1 = FMBL defined in token.asl
//[-end-121218-IB10820203-modify]//
  mGlobalNvsArea->WakeOnPME= SetupVariable->WakeOnPME;
  mGlobalNvsArea->ComPortCMode = SetupVariable->ComPortCMode;
  mGlobalNvsArea->ComPortD = SetupVariable->ComPortD;
  mGlobalNvsArea->Lpt1 = SetupVariable->Lpt1;
  mGlobalNvsArea->PSType = SetupVariable->PSType;
  mGlobalNvsArea->WakeOnModemRing = SetupVariable->WakeOnModemRing; 
//[-start-120731-IB10820094-modify]//
//[-start-121218-IB10820203-modify]//
//  if (FeaturePcdGet(PcdMe5MbSupported)) {
    mGlobalNvsArea->IdeREnable = PcdGet8 (PcdGlobalNvsAreaIdeREnable);
//  } else {
//    mGlobalNvsArea->IdeREnable = 0;
//  }
//[-end-121218-IB10820203-modify]//
//[-end-120731-IB10820094-modify]//
  mGlobalNvsArea->PcieRootPortPmeInt0 = SetupVariable->PcieRootPortPmeInt0;
  mGlobalNvsArea->PcieRootPortPmeInt1 = SetupVariable->PcieRootPortPmeInt1;
  mGlobalNvsArea->CStates = SetupVariable->CStates;
//[-start-121218-IB10820203-modify]//
  mGlobalNvsArea->PeciAccessMethod = PcdGet8 (PcdGlobalNvsAreaPeciAccessMethod);
//[-end-121218-IB10820203-modify]//
  mGlobalNvsArea->WakeOnLan= SetupVariable->WakeOnLan;
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdRapidStartSupported)) {
    mGlobalNvsArea->RapidStartEnable = SetupVariable->EnableRapidStart;
  }
//[-end-120731-IB10820094-modify]//
//[-start-120831-IB05330376-modify]//
  // Bits   Description       
  // 0      ISCT Configured: 0 = Disabled, 1 = Enabled 
  // 1      ISCT Notification Control: 0 = Unsupported, 1 = Supported 
  // 2      ISCT WLAN Power Control : 0 = Unsupported, 1 = Supported 
  // 3      ISCT WWAN Power Control : 0 = Unsupported, 1 = Supported 
  // 4      Must be set to 1 (EC Timer Support)
  // 5      Sleep duration value format: 0 = Actual time, 1 = duration in seconds
  // 6      Reserved (must set to 0)
  // 7      Reserved (must set to 0)
//[-end-120831-IB05330376-modify]//
  if( SetupVariable->ISCTEnable != 0 ) {
     ISCTFlag |= BIT0;
     if ( SetupVariable->ISCTNotify ) {
        ISCTFlag |= BIT1; 
     }
      if ( SetupVariable->ISCTWLanPower) {
        ISCTFlag |= BIT2; 
     }
      if ( SetupVariable->ISCTWWLanPower) {
        ISCTFlag |= BIT3; 
     }
//[-start-120831-IB05330376-modify]//
     ISCTFlag |= (BIT4 | BIT5);
     ISCTFlag &= ~(BIT6 | BIT7);
  } else {
    ISCTFlag = 0;
  }
//[-start-121211-IB06460478-modify]//
  mGlobalNvsArea->IsctCfg = ISCTFlag ;
//[-end-121211-IB06460478-modify]//
//[-end-120831-IB05330376-modify]//
//[-start-121102-IB03780469-add]//
  mGlobalNvsArea->PepDevice = (SetupVariable->PepSata << 1) | (SetupVariable->PepGfx);
//[-end-121102-IB03780469-add]//
//[-start-121218-IB10540025-add]//
//  mGlobalNvsArea->ECLowPowerS0IdleEnable = SetupVariable->ECLowPowerS0IdleEnable;
//[-end-121218-IB10540025-add]//

  for (DsdtPointer = (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER)); DsdtPointer <= (UINT8 *)((UINTN)Table + (UINTN)(Table->Length)); DsdtPointer++) {
    Signature = (UINT32*) DsdtPointer;
    switch (*Signature) {

    case (SIGNATURE_32 ('G', 'N', 'V', 'S')):
      if (IsAmlOpRegionObject (DsdtPointer)) {
        SetOpRegion (DsdtPointer, mGlobalNvsArea, sizeof (EFI_GLOBAL_NVS_AREA));
      }
      break;

    case (SIGNATURE_32 ('O', 'G', 'N', 'S')):
      if (IsAmlOpRegionObject (DsdtPointer)) {
        SetOpRegion (DsdtPointer, mOemGlobalNvsArea, sizeof (EFI_OEM_GLOBAL_NVS_AREA));
      }
      break;

//[-start-130314-IB10820261-add]//
    case (SIGNATURE_32 ('M', 'D', 'B', 'G')):
      if (*(UINT8*) (DsdtPointer + 5) == 0x0C ){                    // Check if it's OperationRegion(\MDBG,SystemMemory,0x55AA55AA, 0x55AA55AA) in MemDbg.asl
        SetupVariable->ACPIDebugAddr = *(UINT32*) (DsdtPointer + 6); // Get offset value of OperationRegion(\MDBG,SystemMemory,0x55AA55AA, 0x55AA55AA)
      }
      break;
//[-end-130314-IB10820261-add]//
      
//[-start-120720-IB06460422-add]//
    case (SIGNATURE_32 ('D', 'C', 'K', '0')):
      if (SetupVariable->EnableLowPowerS0Cap) {
        TempSignature = SIGNATURE_32 ('X', 'E', 'J', '0');
        *(UINT32*) DsdtPointer = TempSignature;
      } else {
        TempSignature = SIGNATURE_32 ('_', 'E', 'J', '0');
        *(UINT32*) DsdtPointer = TempSignature;
      }
      break;

    case (SIGNATURE_32 ('D', 'C', 'K', '1')):
      if (SetupVariable->EnableLowPowerS0Cap) {
        TempSignature = SIGNATURE_32 ('X', 'S', 'T', 'A');
        *(UINT32*) DsdtPointer = TempSignature;
      } else {
        TempSignature = SIGNATURE_32 ('_', 'S', 'T', 'A');
        *(UINT32*) DsdtPointer = TempSignature;
      }
      break;

    case (SIGNATURE_32 ('D', 'C', 'K', '2')):
      if (SetupVariable->EnableLowPowerS0Cap) {
        TempSignature = SIGNATURE_32 ('X', 'D', 'C', 'K');
        *(UINT32*) DsdtPointer = TempSignature;
      } else {
        TempSignature = SIGNATURE_32 ('_', 'D', 'C', 'K');
        *(UINT32*) DsdtPointer = TempSignature;
      }
      break;

    case (SIGNATURE_32 ('D', 'C', 'K', '3')):
      if (SetupVariable->EnableLowPowerS0Cap) {
        TempSignature = SIGNATURE_32 ('X', 'E', 'D', 'L');
        *(UINT32*) DsdtPointer = TempSignature;
      } else {
        TempSignature = SIGNATURE_32 ('_', 'E', 'D', 'L');
        *(UINT32*) DsdtPointer = TempSignature;
      }
      break;
//[-end-120720-IB06460422-add]//
    }
  }

//[-start-120605-IB06460402-modify]//
//[-start-120426-IB06150217-add]//
  mGlobalNvsArea->EnableDptf          = SetupVariable->EnableDptf;
//  mGlobalNvsArea->EnableSaDevice      = SetupVariable->DptfProcessorThermalDevice;
//  mGlobalNvsArea->EnablePchDevice     = SetupVariable->DptfPchThermalDevice;
//  mGlobalNvsArea->EnableCtdpPolicy    = SetupVariable->cTDP;
//  mGlobalNvsArea->EnableLpmPolicy     = SetupVariable->LPM;
  mGlobalNvsArea->CurrentLowPowerMode = SetupVariable->CurrentLowPowerMode;
  mGlobalNvsArea->EnableCurrentExecutionUnit   = 1;
  mGlobalNvsArea->TargetGfxFreq                = 400;
  mGlobalNvsArea->EnablePowerPolicy            = 0;

//[-end-120426-IB06150217-add]//
  mGlobalNvsArea->EnableSaDevice      = SetupVariable->DptfProcessorThermalDevice;
//[-start-121211-IB06460478-modify]//
  mGlobalNvsArea->ActiveThermalTripPointSA = SetupVariable->ActiveThermalTripPointMCH;
  mGlobalNvsArea->PassiveThermalTripPointSA = SetupVariable->PassiveThermalTripPointMCH;
  mGlobalNvsArea->CriticalThermalTripPointSa = SetupVariable->CriticalThermalTripPointSa;
  mGlobalNvsArea->HotThermalTripPointSa = SetupVariable->HotThermalTripPointSa;
  mGlobalNvsArea->PpccStepSize = SetupVariable->PPCCStepSize;
//[-end-121211-IB06460478-modify]//
  mGlobalNvsArea->LPOEnable = SetupVariable->LPOEnable;
  mGlobalNvsArea->LPOStartPState = SetupVariable->LPOStartPState;
  mGlobalNvsArea->LPOStepSize = SetupVariable->LPOStepSize;
  mGlobalNvsArea->LPOPowerControlSetting = SetupVariable->LPOPowerControlSetting;
  mGlobalNvsArea->LPOPerformanceControlSetting = SetupVariable->LPOPerformanceControlSetting;

  mGlobalNvsArea->EnablePchDevice     = SetupVariable->DptfPchThermalDevice;
//[-start-120712-IB07360204-add]//
  mGlobalNvsArea->FullGenericParticipant = SetupVariable->FullGenericParticipant;
//[-end-120712-IB07360204-add]//
  mGlobalNvsArea->ActiveThermalTripPointPCH = SetupVariable->ActiveThermalTripPointPCH;
  mGlobalNvsArea->PassiveThermalTripPointPCH = SetupVariable->PassiveThermalTripPointPCH;
//[-start-121211-IB06460478-modify]//
  mGlobalNvsArea->CriticalThermalTripPointPch = SetupVariable->CriticalThermalTripPointPch;
  mGlobalNvsArea->HotThermalTripPointPch = SetupVariable->HotThermalTripPointPch;
//[-end-121211-IB06460478-modify]//
  mGlobalNvsArea->EnableMemoryDevice = SetupVariable->EnableMemoryDevice;
  mGlobalNvsArea->ActiveThermalTripPointTMEM = SetupVariable->ActiveThermalTripPointTMEM;
  mGlobalNvsArea->PassiveThermalTripPointTMEM = SetupVariable->PassiveThermalTripPointTMEM;
  mGlobalNvsArea->CriticalThermalTripPointTMEM = SetupVariable->CriticalThermalTripPointTMEM;
  mGlobalNvsArea->HotThermalTripPointTMEM = SetupVariable->HotThermalTripPointTMEM;
  mGlobalNvsArea->EnableFan1Device = SetupVariable->EnableFan1Device;
  mGlobalNvsArea->EnableFan2Device = SetupVariable->EnableFan2Device;
  mGlobalNvsArea->EnableAmbientDevice = SetupVariable->EnableAmbientDevice;
  mGlobalNvsArea->ActiveThermalTripPointAmbient = SetupVariable->ActiveThermalTripPointAmbient;
  mGlobalNvsArea->PassiveThermalTripPointAmbient = SetupVariable->PassiveThermalTripPointAmbient;
  mGlobalNvsArea->CriticalThermalTripPointAmbient = SetupVariable->CriticalThermalTripPointAmbient;
  mGlobalNvsArea->HotThermalTripPointAmbient = SetupVariable->HotThermalTripPointAmbient;
  mGlobalNvsArea->EnableSkinDevice = SetupVariable->EnableSkinDevice;
  mGlobalNvsArea->ActiveThermalTripPointSkin = SetupVariable->ActiveThermalTripPointSkin;
  mGlobalNvsArea->PassiveThermalTripPointSkin = SetupVariable->PassiveThermalTripPointSkin;
  mGlobalNvsArea->CriticalThermalTripPointSkin = SetupVariable->CriticalThermalTripPointSkin;
  mGlobalNvsArea->HotThermalTripPointSkin = SetupVariable->HotThermalTripPointSkin;
  mGlobalNvsArea->EnableExhaustFanDevice = SetupVariable->EnableExhaustFanDevice;
  mGlobalNvsArea->ActiveThermalTripPointExhaustFan = SetupVariable->ActiveThermalTripPointExhaustFan;
  mGlobalNvsArea->PassiveThermalTripPointExhaustFan = SetupVariable->PassiveThermalTripPointExhaustFan;
  mGlobalNvsArea->CriticalThermalTripPointExhaustFan = SetupVariable->CriticalThermalTripPointExhaustFan;
  mGlobalNvsArea->HotThermalTripPointExhaustFan = SetupVariable->HotThermalTripPointExhaustFan;
  mGlobalNvsArea->EnableVRDevice = SetupVariable->EnableVRDevice;
  mGlobalNvsArea->ActiveThermalTripPointVR = SetupVariable->ActiveThermalTripPointVR;
  mGlobalNvsArea->PassiveThermalTripPointVR = SetupVariable->PassiveThermalTripPointVR;
  mGlobalNvsArea->CriticalThermalTripPointVR = SetupVariable->CriticalThermalTripPointVR;
  mGlobalNvsArea->HotThermalTripPointVR = SetupVariable->HotThermalTripPointVR;
  mGlobalNvsArea->EnableCtdpPolicy    = SetupVariable->cTDP;
  mGlobalNvsArea->EnableLpmPolicy     = SetupVariable->LPM;
  mGlobalNvsArea->EnableActivePolicy = SetupVariable->EnableActivePolicy;
  mGlobalNvsArea->EnablePassivePolicy = SetupVariable->EnablePassivePolicy;
  mGlobalNvsArea->TrtRevision = SetupVariable->TrtRevision;
  mGlobalNvsArea->EnableCriticalPolicy = SetupVariable->EnableCriticalPolicy;
  mGlobalNvsArea->EnableCoolingModePolicy = SetupVariable->EnableCoolingModePolicy;
//[-start-120907-IB07360220-add]//
//[-start-120910-IB07360221-modify]//
//[-start-121211-IB06460478-modify]//
  mGlobalNvsArea->NFCEnable = SetupVariable->NfcSelection;
//[-end-121211-IB06460478-modify]//
//[-end-120910-IB07360221-modify]//
//[-end-120907-IB07360220-add]//
//[-end-120605-IB06460402-modify]//
//[-start-120718-IB05330361-add]//
////[-start-120809-IB10820101-modify]//
//  if (!FeaturePcdGet(PcdUltFlag)) {
////[-start-121114-IB052800010-modify]//
//    GetBoardId ( &BoardID1 );  
////[-end-121114-IB052800010-modify]//
////[-start-120809-IB10370015-modify]//
//  mGlobalNvsArea->BoardId = (BoardID1 & B_EC_GET_CRB_BOARD_ID_BOARD_ID);
////[-end-120809-IB10370015-modify]//
//  } else {
////[-start-121114-IB052800010-modify]//
//    GetUltBoardId ( &BoardID1,&BoardID2);
////[-end-121114-IB052800010-modify]//
//    mGlobalNvsArea->BoardId = BoardID2;
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);
  BoardID = PlatformInfoProtocol->PlatInfo.BoardId;
//[-start-130422-IB05400398-modify]//
//[-start-130410-IB05160433-modify]//
  //mGlobalNvsArea->BoardId = BoardID;
  mGlobalNvsArea->BoardId = PcdGet8 (PcdCurrentBoardId);
//[-end-130410-IB05160433-modify]//
//[-end-130422-IB05400398-modify]//
  if (PlatformInfoProtocol->PlatInfo.UltPlatform) {
//[-start-121002-IB06150249-add]//
//[-start-121211-IB06460478-modify]//
  mGlobalNvsArea->LowPowerS0Idle =  SetupVariable->EnableLowPowerS0Cap;
  mGlobalNvsArea->AudioDspCodec =  SetupVariable->AudioCodecSelect;
//[-end-121211-IB06460478-modify]//
//[-end-121002-IB06150249-add]//
  }
//[-start-120809-IB10370015-modify]//
  mGlobalNvsArea->Rtd3Support = SetupVariable->RuntimeDevice3;
  if (SetupVariable->RuntimeDevice3) {
//[-start-121218-IB10820203-modify]//
    mGlobalNvsArea->Rtd3P0dl = PcdGet8 (PcdGlobalNvsAreaRtd3P0dl);
    mGlobalNvsArea->Rtd3P3dl = PcdGet8 (PcdGlobalNvsAreaRtd3P3dl);
//[-end-121218-IB10820203-modify]//
  }
//[-end-120809-IB10370015-modify]//
//[-end-120718-IB05330361-add]//
//[-start-120816-IB05330371-add]//
  mGlobalNvsArea->EnableACPIDebug = SetupVariable->EnableACPIDebug;
//[-end-120816-IB05330371-add]//
//[-start-121213-IB10540025-modify]//
//[-start-121102-IB06150254-add]//
  mGlobalNvsArea->I2c0SensorDeviceSelection =  (SetupVariable->I2c0IntelSensorHub | SetupVariable->I2c0WITTDevice);
  mGlobalNvsArea->I2c1SensorDeviceSelection =  (UINT8)(SetupVariable->I2c1AtmelTouchPanel | SetupVariable->I2c1ElantechTouchPanel |
                                                SetupVariable->I2c1ElantechTouchPad |SetupVariable->I2c1SynapticsTouchPad |
                                                SetupVariable->I2c1WITTDevice | SetupVariable->I2c1NTrigDigitizer |
                                                SetupVariable->I2c1EETITouchPanel |  SetupVariable->I2c1AlpsTouchPad |
                                                SetupVariable->I2c1CypressTouchPad);

//[-end-121102-IB06150254-add]//
//[-end-121213-IB10540025-modify]//
//[-start-121119-IB05330387-add]//
  //
  // Update OBFF and LTR
  //
  Status = gBS->LocateProtocol (&gDxePchPlatformPolicyProtocolGuid, NULL, (VOID **)&PchPlatformPolicy);
  if (PlatformInfoProtocol->PlatInfo.UltPlatform){
    PciePortMax = LPTLP_PCIE_MAX_ROOT_PORTS;
  } else {
    PciePortMax = LPTH_PCIE_MAX_ROOT_PORTS;
  }
  for (Index = 0; Index < PciePortMax; Index ++) {
    mGlobalNvsArea->LtrEnable[Index] = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Index].LtrEnable;
    mGlobalNvsArea->ObffEnable[Index] = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Index].ObffEnable;
  }
//[-end-121119-IB05330387-add]//

//[-start-130422-IB05400398-remove]//
////[-start-121126-IB06460466-add]//
//  if (PlatformInfoProtocol->PlatInfo.BoardId == V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH) {
//    //
//    // This setting is only working on Harris Beach
//    //
//    mGlobalNvsArea->I2c0SensorDeviceSelection =  1;
//    mGlobalNvsArea->I2c1SensorDeviceSelection =  9;
////[-start-121211-IB06460478-modify]//
//    mGlobalNvsArea->AudioDspCodec          =  0xFF;
////[-end-121211-IB06460478-modify]//
//  }
////[-end-121126-IB06460466-add]//
//[-end-130422-IB05400398-remove]//

//[-start-121218-IB10540025-add]//
// mGlobalNvsArea->IuerDockEnable = SetupVariable->IuerDockEnable;
// mGlobalNvsArea->IuerButtonEnable = SetupVariable->IuerButtonEnable;
//[-end-121218-IB10540025-add]//
////[-start-121213-IB10540025-modify]//
//  if (PlatformInfoProtocol->PlatInfo.UltPlatform) {
//    mGlobalNvsArea->Rtd3AudioDeviceDelay = SetupVariable->Rtd3AudioDeviceDelay;
//    mGlobalNvsArea->Rtd3AdspDeviceDelay = SetupVariable->Rtd3AdspDeviceDelay;
//    mGlobalNvsArea->I2c0DeviceDelayTiming = SetupVariable->I2c0DeviceDelayTiming;
//    mGlobalNvsArea->I2c1DeviceDelayTiming = SetupVariable->I2c1DeviceDelayTiming;
//    mGlobalNvsArea->PStateCapping = SetupVariable->PStateCapping;
//
//    if (PlatformInfoProtocol->PlatInfo.BoardId != V_EC_GET_CRB_BOARD_ID_BOARD_ID_WHITE_TIP_MOUNTAIN) {
////[-start-121212-IB10540024-add]//
//      mGlobalNvsArea->Rtd3CongigSetting = 1;
////[-end-121212-IB10540024-add]//
//    } else {
//      mGlobalNvsArea->Rtd3CongigSetting = SetupVariable->Rtd3CongigSetting;
//    }
//  }
////[-end-121213-IB10540025-modify]//
//[-start-121220-IB10540026-add]//
  mGlobalNvsArea->NativePCIESupport = SetupVariable->NativePCIESupport;
//[-end-121220-IB10540026-add]//
  mGlobalNvsArea->PL1LimitCS        = 0;
  mGlobalNvsArea->PL1LimitCSValue   = 4500;
  mGlobalNvsArea->DmaOsDetection    = SetupVariable->EnableSerialIoDma;
//[-start-121218-IB10820203-add]//
//[-start-130524-IB05160451-modify]//
//[-start-121219-IB10820205-modify]//
  Status = OemSvcUpdateGlobalNvs (mGlobalNvsArea, mOemGlobalNvsArea);
//[-end-121219-IB10820205-modify]//
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcUpdateGlobalNvs, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }
//[-end-121218-IB10820203-add]//
  return  EFI_SUCCESS;
}

/**
 Update Ssdt Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupVariable       SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success

*/
STATIC
EFI_STATUS
UpdateSsdt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN CHIPSET_CONFIGURATION             *SetupVariable
  )
{
  EFI_STATUS Status;
//[-start-121004-IB10370017-add]//
  PLATFORM_INFO_PROTOCOL                *PlatformInfoProtocol;
//[-end-121004-IB10370017-add]//
//[-start-120726-IB06460426-add]//
  UINT8                                 BoardId;
//[-start-121004-IB10370017-remove]//
//  UINT8                                 BoardID1;
//[-end-121004-IB10370017-remove]//
//[-end-120726-IB06460426-add]//
//[-start-130613-IB05400415-add]//
  EFI_STATUS                            NewStatus;
  EFI_STATUS                            FuncStatus;
//[-end-130613-IB05400415-add]//

  Status = EFI_UNSUPPORTED;  

  switch (Table->OemTableId) {
  case SIGNATURE_64 ('S', 'a', 't', 'a', 'P', 'r', 'i', '\0'):
    if (((PchMmRcrb32 ((UINTN)R_PCH_RCRB_FUNC_DIS) & B_PCH_RCRB_FUNC_DIS_SATA1) == 0) &&
        (SetupVariable->SataCnfigure == 0)) {
      Status = EFI_SUCCESS;
    } 
    break;

  case SIGNATURE_64 ('S', 'a', 't', 'a', 'S', 'e', 'c', '\0'):
    if (((PchMmRcrb32 ((UINTN)R_PCH_RCRB_FUNC_DIS) & B_PCH_RCRB_FUNC_DIS_SATA2) == 0) &&
        (SetupVariable->SataCnfigure == 0)) {
      Status = EFI_SUCCESS;
    } 
    break;

  case SIGNATURE_64 ('S', 'a', 't', 'a', 'A', 'h', 'c', 'i'):
    if (((PchMmRcrb32 ((UINTN)R_PCH_RCRB_FUNC_DIS) & B_PCH_RCRB_FUNC_DIS_SATA2) != 0) &&
        (SetupVariable->SataCnfigure != 0)) {
      Status = EFI_SUCCESS;
    } 
    break;
  case SIGNATURE_64 ('P', 't', 'i', 'd', 'D', 'e', 'v', 'c'):
      Status = EFI_SUCCESS;
    break;
//[-start-120604-IB06150223-modify]//
//[-start-120828-IB05330376-remove]//
//  case EFI_SIGNATURE_32('I','S','C','T'):
//    if (SetupVariable->ISCTEnable == 1) {
//      Status = EFI_SUCCESS;
//    }   
//[-end-120828-IB05330376-remove]//
//[-end-120604-IB06150223-modify]//
    break;
//[-start-120209-IB03780421-modify]//
  case SIGNATURE_32 ('X', 'T', 'U', '\0'):
//[-start-120731-IB10820094-modify]//
    if (FeaturePcdGet(PcdXtuSupported)) {
      Status = EFI_SUCCESS;
    }
//[-end-120731-IB10820094-modify]//
    break;
//[-start-120726-IB06460426-modify]//
//[-start-120713-IB05330359-add]//
//[-start-120820-IB10820110-modify]//
//[-start-120809-IB10370015-modify]//
  case SIGNATURE_64 ('U', 'l', 't', '0', 'R', 't', 'd', '3'):
//[-end-120809-IB10370015-modify]//
//[-start-121004-IB10370017-modify]//
//    if (!FeaturePcdGet (PcdUltFlag)) {
////[-start-121114-IB052800010-modify]//
//      GetBoardId (&BoardId);
////[-end-121114-IB052800010-modify]//
//    } else {
////[-start-121114-IB052800010-modify]//
//      GetUltBoardId (&BoardID1, &BoardId);
////[-end-121114-IB052800010-modify]//
//    }
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);
//[-start-130111-IB05280033-add]//
  Status = EFI_UNSUPPORTED;
//[-end-130111-IB05280033-add]//
  BoardId = PlatformInfoProtocol->PlatInfo.BoardId;
//[-end-121004-IB10370017-modify]//
//[-start-130422-IB05400398-remove]//
//    if (SetupVariable->RuntimeDevice3 && 
//        (BoardId == V_EC_GET_CRB_BOARD_ID_BOARD_ID_WHITE_TIP_MOUNTAIN ||
//         BoardId == V_EC_GET_CRB_BOARD_ID_BOARD_ID_SAWTOOTH_PEAK)) {
//      Status = EFI_SUCCESS;
//    }
//[-end-130422-IB05400398-remove]//
    break;

//[-start-121114-IB06460466-add]//
  case SIGNATURE_64 ('H', 'S', 'W', '-', 'F', 'F', 'R', 'D'):
    Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
    ASSERT_EFI_ERROR (Status);
    BoardId = PlatformInfoProtocol->PlatInfo.BoardId;
    Status = EFI_UNSUPPORTED;
//[-start-130422-IB05400398-remove]//
//    if (SetupVariable->RuntimeDevice3 &&
//        (BoardId == V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH)) {
//      Status = EFI_SUCCESS;
//    }
//[-end-130422-IB05400398-remove]//
    break;
//[-end-121114-IB06460466-add]//

  case SIGNATURE_64 ('B', 'R', '0', '_', 'R', 't', 'd', '3'):
//[-start-121004-IB10370017-modify]//
//    if (!FeaturePcdGet (PcdUltFlag)) {
////[-start-121114-IB052800010-modify]//
//      GetBoardId (&BoardId);
////[-end-121114-IB052800010-modify]//
//    } else {
////[-start-121114-IB052800010-modify]//
//      GetUltBoardId (&BoardID1, &BoardId);
////[-end-121114-IB052800010-modify]//
//    }
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);
  BoardId = PlatformInfoProtocol->PlatInfo.BoardId;
//[-start-130111-IB05280033-add]//
  Status = EFI_UNSUPPORTED;
//[-end-130111-IB05280033-add]//
//[-end-121004-IB10370017-modify]//
//[-start-130422-IB05400398-remove]//
//    if (SetupVariable->RuntimeDevice3 && 
//        ((BoardId & B_EC_GET_CRB_BOARD_ID_BOARD_ID) == V_EC_GET_CRB_BOARD_ID_BOARD_ID_BASKING_RIDGE)) {
//      Status = EFI_SUCCESS;
//    }
//[-end-130422-IB05400398-remove]//
    break;
//[-end-120820-IB10820110-modify]//
//[-end-120713-IB05330359-add]//
//[-end-120726-IB06460426-modify]//

//   default:
//     *Version = EFI_ACPI_TABLE_VERSION_NONE;
//     Status = EFI_SUCCESS;
  }

//[-start-130613-IB05400415-modify]//
//[-start-130422-IB05400398-add]//
  FuncStatus = OemSvcUpdateSsdtStatus (Table->OemTableId, SetupVariable, &NewStatus);
  if (FuncStatus == EFI_MEDIA_CHANGED) {
    Status = NewStatus;
  }
//[-end-130422-IB05400398-add]//
//[-end-130613-IB05400415-modify]//
  return Status;
}


STATIC
EFI_STATUS
SpttDataCollector (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                            Status;
  SPTT_DATA_PROTOCOL                    *SpttDataProtocol;
  UINT8                                 *Buffer;
  UINTN                                 BufferLen;
  UINTN                                 TotalLen;
  UINT32                                Pages;
  EFI_PHYSICAL_ADDRESS                  Type4Addr;
  UINT8                                 *SpttPtr;
  UINT8                                 *TmpPtr;
  SYSTEM_PERFORMANCE_TUNING_TABLE       *Sptt;
  UINT8                                 CheckSum;
  UINT8                                 *Ptr;
  UINT8                                 *PtrEnd;
  DEVICE_DESCRIPTION_TABLE              *EndOfDevices;
  VOID                                  *TableBuffer;
  EFI_ACPI_SUPPORT_PROTOCOL             *AcpiSupport;
  EFI_ACPI_TABLE_VERSION                AcpiTableVersion;
  INTN                                  Index;
  UINTN                                 Handle;
  EFI_ACPI_ASPT_TABLE                   *pSpttTable;
  UINTN                                 HandleCount;
  UINTN                                 Iter;
  EFI_HANDLE*                           Handles;

  TotalLen = 0;
  AcpiSupport = NULL;
  AcpiTableVersion = EFI_ACPI_TABLE_VERSION_1_0B;
  pSpttTable = NULL;
  Index = 0;

  DEBUG ((EFI_D_ERROR, "Enter SpttDataCollector ...\n"));
  ///////////////////////////////////////////
  // Begin creation of the SPTT Table
  // These tables are being built based on the
  // Iron City BIOS Interface Specification Revision 0.89
  //

  //
  // If the feature is not supported on this sku then return unsupported.
  //

  Sptt = AllocateZeroPool (sizeof (SYSTEM_PERFORMANCE_TUNING_TABLE));
//[-start-130207-IB10870073-add]//
  ASSERT (Sptt != NULL);
  if (Sptt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  EndOfDevices = AllocateZeroPool (sizeof (DEVICE_DESCRIPTION_TABLE));
//[-start-130207-IB10870073-add]//
  ASSERT (EndOfDevices != NULL);
  if (EndOfDevices == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  // Allocate an arbitrarily large amount of space for the SPTT tables
  SpttPtr = AllocateZeroPool (0x18000);
//[-start-130207-IB10870073-add]//
  ASSERT (SpttPtr != NULL);
  if (SpttPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  // Place the temporary pointer at the end of the SPTT Header
  TmpPtr = SpttPtr + sizeof (SYSTEM_PERFORMANCE_TUNING_TABLE);

  // Initialize the length of the entire table with the length of the SPTT and the size of the End of Devices entry.
  TotalLen = sizeof (SYSTEM_PERFORMANCE_TUNING_TABLE) + sizeof (DEVICE_DESCRIPTION_TABLE);

  // Initialize the SPTT Header and copy it to the buffer
  Sptt->Signature = SPTT_TABLE_SIG;                       // Signature = "SPTT"
  Sptt->Length = 0;                                       // To be updated upon completion of data collection
  Sptt->MajorVer = SPTT_TABLE_MAJOR_VERSION;
  Sptt->MinorVer = SPTT_TABLE_MINOR_VERSION;
  Sptt->SwSmiPort = PcdGet16 (PcdSoftwareSmiPort);
  Sptt->SwSmiCmd = EFI_PERF_TUNE_SW_SMI;
  Sptt->Checksum = 0;                                     // To be updated upon completion of data collection
  Sptt->FeatureFlags = 0;

  CopyMem (SpttPtr, Sptt, sizeof(SYSTEM_PERFORMANCE_TUNING_TABLE));
  gBS->FreePool (Sptt);

  //
  // Find all the SPTT Data Interface Protocols
  //
  Status = gBS->LocateHandleBuffer (
    ByProtocol,
    &gSpttDataProtocolGuid,
    NULL,
    &HandleCount,
    &Handles
    );
    DEBUG ((EFI_D_ERROR, "lINE 830 HandleCount : %x \n", HandleCount));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "LocateHandleBuffer Error. Status: %x \n", Status));
    return Status;
  }

  //
  // Load references to each Protocol Instance and associate it with a Generic HW Device ID
  //
  for (Iter = 0; Iter < HandleCount; Iter++) {

    //
    // Get protocol instance based on the handle
    //
    DEBUG ((EFI_D_ERROR, "lINE 843 Iter : %x \n", Iter));
    Status = gBS->HandleProtocol (
      Handles[Iter],
      &gSpttDataProtocolGuid,
      (VOID **)&SpttDataProtocol
      );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "HandleProtocol Error. Status: %x \n", Status));
      gBS->FreePool (Handles);
      gBS->FreePool (Sptt);
      gBS->FreePool (EndOfDevices);
      return Status;
    }

    // Now gather all data provided by this instance of the protocol
    SpttDataProtocol->GetData (&Buffer, &BufferLen);
    TotalLen += BufferLen;

    CopyMem (TmpPtr, Buffer, BufferLen);
    TmpPtr += BufferLen;
    gBS->FreePool (Buffer);
  }

  // Fill in the End of Devices structure and copy it at the end of the buffer
  EndOfDevices->Signature = DDD_TABLE_SIG;                // Signature = "$DDD"
  EndOfDevices->DeviceType = 0x00;
  EndOfDevices->ControlType = 0x00;
  EndOfDevices->ImplementationType = 0x00;

  CopyMem (TmpPtr, EndOfDevices, sizeof (DEVICE_DESCRIPTION_TABLE));
  TmpPtr += sizeof (DEVICE_DESCRIPTION_TABLE);
  gBS->FreePool (EndOfDevices);

  // Use the SPTT ptr at the beginning of the buffer
  Sptt = (SYSTEM_PERFORMANCE_TUNING_TABLE*)(VOID*)SpttPtr;

  // Update the overall length of the data,
  Sptt->Length = (UINT32)TotalLen;

  // Now Calculate the Checksum
  Ptr = (UINT8*)(VOID*)Sptt;
  PtrEnd = Ptr + TotalLen;
  for (CheckSum = 0; Ptr < PtrEnd; Ptr++) {
    CheckSum = (UINT8) (CheckSum +*Ptr);
  }
  Sptt->Checksum = 0 - CheckSum;

  //
  // SPTT Table Creation is now complete
  ///////////////////////////////////////////

  // Create type 4 memory to take the entire table
  Pages = EFI_SIZE_TO_PAGES ((UINT32)TotalLen ) + 1;
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiACPIMemoryNVS,
                  Pages,
                  &Type4Addr
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Line 895 AllocatePages Error. Status: %x \n", Status));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Now copy the table to the type 4 memory location
  //
  CopyMem ((VOID*)(UINTN)Type4Addr, SpttPtr, TotalLen);

  //
  // Now write the address of the type 4 memory location to the SSDT table
  // To do that we need to find the AcpiSupport protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&AcpiSupport);
  DEBUG ((EFI_D_ERROR, "Line 943 LocateProtocol. Status: %x \n", Status));
  if (!EFI_ERROR (Status))
  {
    // Search for ACPI ASPT table
    do {
      Status = AcpiSupport->GetAcpiTable (AcpiSupport, Index, &TableBuffer, &AcpiTableVersion, &Handle);
      Index++;
    } while ((!EFI_ERROR (Status)) && (((EFI_ACPI_DESCRIPTION_HEADER *) TableBuffer)->Signature != EFI_ACPI_ASPT_TABLE_SIGNATURE));

    pSpttTable = (EFI_ACPI_ASPT_TABLE *) TableBuffer;
    pSpttTable->Type4MemPtr = Type4Addr;
    AcpiSupport->SetAcpiTable (AcpiSupport, TableBuffer, TRUE, AcpiTableVersion, &Handle);
  }
  gBS->FreePool (SpttPtr);
  DEBUG ((EFI_D_ERROR, "<-- SpttDataCollector ...\n", Status));
  return Status;
}

/**
 Update Sptt Table according to platform specific.

 @param[in, out]    Table               The table to update

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateSptt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table
  )
{
  EFI_STATUS                          Status;
  EFI_ACPI_ASPT_TABLE                 *pSpttTable;
  EFI_EVENT                           mReadyToBootEvent;

  pSpttTable = (EFI_ACPI_ASPT_TABLE *) Table;
  // Fixup the SPTT table
  pSpttTable->Header.Length = sizeof (EFI_ACPI_ASPT_TABLE);
  pSpttTable->Header.OemTableId = SIGNATURE_64 ('P', 'e', 'r', 'f', 'T', 'u', 'n', 'e');
  //
  //  -- Creates an event to call SpttDataCollector near the end of POST
  //
  Status = EfiCreateEventReadyToBootEx(
             TPL_CALLBACK,
             SpttDataCollector,
             NULL,
             &mReadyToBootEvent
             );
  return Status;
}


/**
 Update ACPI table content according to platform specific

 @param[in, out]    TableHeader             Pointer of the table to update
 @param[in, out]    CommonCodeReturnStatus  Return Status from Common Code
*/
VOID
UpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader,
  IN OUT EFI_STATUS                       *CommonCodeReturnStatus
)
{
  EFI_STATUS                              Status;
  EFI_SETUP_UTILITY_PROTOCOL              *EfiSetupUtility;
  CHIPSET_CONFIGURATION                    *SetupVariable;
  
  Status = EFI_SUCCESS;
  
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);
  if (EFI_ERROR (Status)) {
    *CommonCodeReturnStatus = EFI_PROTOCOL_ERROR;
    return;
  }

  SetupVariable = (CHIPSET_CONFIGURATION *)EfiSetupUtility->SetupNvData;
  switch ((TableHeader)->Signature) {

  case EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
//[-start-121114-IB08460014-modify]//
    *CommonCodeReturnStatus = UpdateMadt (TableHeader);
//[-end-121114-IB08460014-modify]//
    break;

//[-start-130314-IB10820261-add]//
  case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateFacp (TableHeader, SetupVariable);
    break;
//[-end-130314-IB10820261-add]//

  case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateDsdt (TableHeader, SetupVariable);
    break;

  case EFI_ACPI_3_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateSsdt (TableHeader, SetupVariable);
    break;

  case EFI_ACPI_ASPT_TABLE_SIGNATURE:
    //
    // supported only when Enable OverClocking
    //
    if (FeaturePcdGet (PcdXtuSupported)) {
//[-start-121114-IB08460014-modify]//
      *CommonCodeReturnStatus = UpdateSptt (TableHeader);
//[-end-121114-IB08460014-modify]//
    }
    break;
  case SIGNATURE_32 ('W', 'D', 'A', 'T'):
    if (SetupVariable->TCOWatchDog == FALSE || SetupVariable->WatchDogAcpiTable == FALSE) {
      //
      // Won't add WDAT to acpi table
      //
      *CommonCodeReturnStatus = EFI_UNSUPPORTED;
    }
    break;

  }
  //*CommonCodeReturnStatus = Status;
}

