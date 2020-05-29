/** @file
  Supports functions that saves and restores to the global register table
  information of PIC, KBC, PCI, CpuState, Edge Level, GPIO, and MTRR.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <DeviceRegTable.h>
//[-start-121029-IB10820145-remove]//
//#include <DeviceRegOemTable.h>
//[-end-121029-IB10820145-remove]//
#include <PchAccess.h>
#include <SaAccess.h>
#include <CpuRegs.h>
#include <ChipsetCmos.h>
//[-start-121214-IB10820195-remove]//
//#include <PlatformBaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <CpuDataStruct.h>
//[-start-121212-IB10820191-add]//
#include <Cpu.h>
//[-end-121212-IB10820191-add]//
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/CmosLib.h>
//[-start-121211-IB05300367-add]//
#include <Library/PchPlatformLib.h>
//[-end-121211-IB05300367-add]//
//[-start-121022-IB10820142-add]//
#include <Library/SmmOemSvcChipsetLib.h>
//[-end-121022-IB10820142-add]//
//[-start-121213-IB10820193-remove]//
//#define EFI_APIC_GLOBAL_ENABLE                0x800
//[-end-121213-IB10820193-remove]//
#define APIC_REGISTER_TPR_OFFSET              0x000000080
//[-start-121213-IB10820193-modify]//
#define APIC_REGISTER_TIMER_VECTOR_OFFSET     0x000000320
//[-end-121213-IB10820193-modify]//
//[-start-121213-IB10820193-remove]//
//#ifndef EFI_CACHE_MTRR_VALID
//#define EFI_CACHE_MTRR_VALID                  0x800
//#endif
//#ifndef EFI_CACHE_FIXED_MTRR_VALID
//#define EFI_CACHE_FIXED_MTRR_VALID            0x400
//#endif
//[-end-121213-IB10820193-remove]//
//[-start-121212-IB10820191-add]//
#define UNCORE_DEV                            0
#define SAD_FUNC                              1

#define UNCORE_CR_MCSEG_BASE                  0x060
#define UNCORE_CR_MCSEG_BASE_LOWER            ( UNCORE_CR_MCSEG_BASE )
#define UNCORE_CR_MCSEG_BASE_UPPER            ( UNCORE_CR_MCSEG_BASE + 0x04 )
#define UNCORE_CR_MCSEG_MASK                  0x068
#define UNCORE_CR_MCSEG_MASK_LOWER            ( UNCORE_CR_MCSEG_MASK )
#define UNCORE_CR_MCSEG_MASK_UPPER            ( UNCORE_CR_MCSEG_MASK + 0x04 )
#define B_UNCORE_CR_MCSEG_MASK_VALID          BIT11
#define B_UNCORE_CR_MCSEG_MASK_LOCK           BIT10
//[-end-121212-IB10820191-add]//
UINT32 ApicTPR;
UINT32 ApicDCR;
UINT32 ApicLVT[6];
//[-start-120808-IB10820098-modify]//
#include <CpuRegs.h>
#include <ChipsetSmiTable.h>
//[-end-120808-IB10820098-modify]//

UINT32
GetApicID (
  VOID
)
{
//[-start-120808-IB10820098-modify]//
  EFI_CPUID_REGISTER  CpuidRegisters;

  AsmCpuid (CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  return (UINT32)(CpuidRegisters.RegEbx >> 24);
//[-end-120808-IB10820098-modify]//  
}

VOID
VTSupport (
VOID
  )
{
  UINT32 Ia32FeatCtrl;

  Ia32FeatCtrl = (UINT32)AsmReadMsr32 ( MSR_IA32_FEATURE_CONTROL );
  if ((ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag ) & (B_VMX_SETUP_FLAG | B_VMX_CPU_FLAG)) == (B_VMX_SETUP_FLAG | B_VMX_CPU_FLAG)) {
    if ((Ia32FeatCtrl & B_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
      Ia32FeatCtrl |=  B_MSR_IA32_FEATURE_CONTROL_EVT;
      AsmWriteMsr32 (MSR_IA32_FEATURE_CONTROL, Ia32FeatCtrl);
      Ia32FeatCtrl |= B_MSR_IA32_FEATURE_CONTROL_LOCK;
      AsmWriteMsr32 (MSR_IA32_FEATURE_CONTROL, Ia32FeatCtrl);
    }
  } else {
    if ((Ia32FeatCtrl & B_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
      Ia32FeatCtrl |= B_MSR_IA32_FEATURE_CONTROL_LOCK;
      AsmWriteMsr32 (MSR_IA32_FEATURE_CONTROL, Ia32FeatCtrl);
    }
  }

}

/**
 This function either writes to or read from PM IO registers.

 @param [in]   SaveRestoreFlag  True: write data to PM IO.
                                False: read data from IO to global registers.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestorePmIo (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{

  UINTN                                  Index;

  Index = 0;
  while (PMIO_REG[Index] != 0xFF) {
    if (SaveRestoreFlag) {
      IoWrite8 (mAcpiBaseAddr + PMIO_REG [Index], PMIO_REG_SAVE [Index]);
   } else {
      PMIO_REG_SAVE [Index] = IoRead8 (mAcpiBaseAddr + PMIO_REG [Index]);
   }
   Index++;
 }

  return EFI_SUCCESS;
}

/**
 This function either writes to or read from PCI registers.

 @param [in]        SaveRestoreFlag     True: write data to PCI registers.
                                        False: read data from PCI registers to global registers.

 @retval            EFI_SUCCESS         if read or write is successful.
*/
EFI_STATUS
SaveRestorePci (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                                 PciRegNum;
  UINT8                                 *RegTable = NULL;
  UINTN                                 Index;
  UINT8                                 PciRegister;
  UINT32                                *RegTableSave32 = NULL;
//[-start-140625-IB05080432-modify]//
  SR_DEVICE                             *SRDevice = NULL;
  SR_OEM_DEVICE                         *SROemDev = NULL;
//[-end-140625-IB05080432-modify]//
  UINT8                                 UpdateBusNum = 0;
  SR_TABLE                              *PciResTablePtr;
//[-start-121022-IB10820142-add]//
  SR_OEM_DEVICE                         *PciDeviceOemList;
  EFI_STATUS                            Status;
//[-start-121022-IB10820142-add]//

//[-start-130110-IB10870066-add]//
  PciDeviceOemList = NULL;
//[-end-130110-IB10870066-add]//
//[-start-121120-IB05280016-add]//
  if (!FeaturePcdGet (PcdUltFlag)) {
//[-end-121120-IB05280016-add]//
    PciResTablePtr = &PciResTable;
//[-start-121120-IB05280016-add]//
  } else {
    PciResTablePtr = &PciResTableUlt;
  }
//[-end-121120-IB05280016-add]//
//[-start-121109-IB10820156-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcGetSaveRestorePciDeviceOemList (&PciDeviceOemList);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcGetSaveRestorePciDeviceOemList, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_MEDIA_CHANGED) {
    PciResTablePtr->Oem= PciDeviceOemList;
  }
//[-end-121109-IB10820156-modify]//

  if (!SaveRestoreFlag) {
    SROemDev = PciResTablePtr->Oem;
//[-start-140625-IB05080432-modify]//
    if (SROemDev != NULL) {
      while (SROemDev->Device.RegNum != 0) {
        if (SROemDev->P2PB.Bus + SROemDev->P2PB.Dev + SROemDev->P2PB.Fun != 0x00) {
          UpdateBusNum = MmPci8 (0, SROemDev->P2PB.Bus, SROemDev->P2PB.Dev, SROemDev->P2PB.Fun, R_PCH_PCIE_SCBN);
          SROemDev->Device.PciBus = UpdateBusNum;
        }
        SROemDev = SROemDev + 1;
      }
    }
//[-end-140625-IB05080432-modify]//
  }

  SRDevice = PciResTablePtr->Chipset;
  SROemDev = PciResTablePtr->Oem;
//[-start-140625-IB05080432-modify]//
  if ((SRDevice != NULL) && (SROemDev != NULL)) {
    if (SRDevice->RegNum == 0) {
      SRDevice = (SR_DEVICE*)SROemDev;
      SROemDev = SROemDev + 1;
    }
  
    while (SRDevice->RegNum != 0) {
  
      RegTableSave32 = SRDevice->PciRegTableSave;
  
      if (MmPci32 (0, SRDevice->PciBus, SRDevice->PciDev, SRDevice->PciFun, 0) != 0xFFFFFFFF) {
        if (SRDevice->PciRegTable == NULL) {
          //
          // Save/Restore Full Page
          //
          for (PciRegister = 0xfc, Index = 0; Index < FILL_PCI_REG_NUM; PciRegister -= 4, Index +=4) {
            if (SaveRestoreFlag) {
              MmPci32 (
                0,
                SRDevice->PciBus,
                SRDevice->PciDev,
                SRDevice->PciFun,
                PciRegister
                ) = RegTableSave32 [Index/4];
            } else {
              RegTableSave32 [Index/4] = MmPci32 (
                                           0,
                                           SRDevice->PciBus,
                                           SRDevice->PciDev,
                                           SRDevice->PciFun,
                                           PciRegister
                                           );
            }
          }
  
        } else {
          RegTable = SRDevice->PciRegTable;
          PciRegNum = SRDevice->RegNum;
  
          for (Index = 0; Index < PciRegNum; Index++) {
            //
            // Save/Restore  Pci configuration registers to SmRam
            //
            if (SaveRestoreFlag) {
              MmPci32 (
                0,
                SRDevice->PciBus,
                SRDevice->PciDev,
                SRDevice->PciFun,
                RegTable[Index]
                ) = RegTableSave32 [Index];
            } else {
              RegTableSave32 [Index] = MmPci32 (
                                         0,
                                         SRDevice->PciBus,
                                         SRDevice->PciDev,
                                         SRDevice->PciFun,
                                         RegTable[Index]
                                         );
            }
          }
        }
      }
  
      if (SROemDev == PciResTablePtr->Oem) {
        SRDevice = SRDevice + 1;
      }
      if (SROemDev != PciResTablePtr->Oem || SRDevice->RegNum == 0) {
        SRDevice = (SR_DEVICE*)SROemDev;
        SROemDev = SROemDev + 1;
      }
    }
  }
//[-end-140625-IB05080432-modify]//

//[-start-120808-IB10820098-modify]//
  //
  // Genererate Thunderbolt SMM handler
  //
  if (FeaturePcdGet (PcdThunderBoltSupported)) {
//[-start-121015-IB10820135-modify]//
    IoWrite8 (PcdGet16 (PcdSoftwareSmiPort), THUNDERBOLT_SW_SMI);
//[-end-121015-IB10820135-modify]//
  }
//[-end-120808-IB10820098-modify]//


  // Restore Function Disable register.

  return EFI_SUCCESS;
}

/**
 This function either writes to or read from AHCI BAR register.
 SaveRestoreFlag - True: write data to AHCI BAR registers.
 False: read data from AHCI BAR registers to global registers.

 @param [in]   SaveRestoreFlag

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestoreAbar (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINT8  Map_Register;
  UINTN Index;

  Map_Register = MmPci8 (0,
                         DEFAULT_PCI_BUS_NUMBER_PCH,
                         PCI_DEVICE_NUMBER_PCH_SATA,
                         PCI_FUNCTION_NUMBER_PCH_SATA,
                         R_PCH_SATA_MAP);
  if ((((Map_Register) & (B_PCH_SATA_MAP_SMS_MASK)) == V_PCH_SATA_MAP_SMS_AHCI) ||
      (((Map_Register) & (B_PCH_SATA_MAP_SMS_MASK)) == V_PCH_SATA_MAP_SMS_RAID)) {
//[-start-121120-IB05280016-add]//
    if (!FeaturePcdGet(PcdUltFlag)) {
//[-end-121120-IB05280016-add]//
      for (Index = 0; AHCIBAR_IO[Index] != 0xffffffff; Index++) {
        if (SaveRestoreFlag) {
          McAhci32(AHCIBAR_IO[Index]) = AHCIBAR_IO_SAVE[Index];
        } else {
          AHCIBAR_IO_SAVE[Index] = McAhci32(AHCIBAR_IO[Index]);
        }
      }
//[-start-121120-IB05280016-add]//
    } else {
      for (Index = 0; AHCIBAR_IO_ULT[Index] != 0xffffffff; Index++) {
        if (SaveRestoreFlag) {
          McAhci32(AHCIBAR_IO_ULT[Index]) = AHCIBAR_IO_SAVE_ULT[Index];
        } else {
          AHCIBAR_IO_SAVE_ULT[Index] = McAhci32(AHCIBAR_IO_ULT[Index]);
        }
      }
    }
//[-end-121120-IB05280016-add]//
  }
  return  EFI_SUCCESS;
}


/**
 This function either writes to or read from Pic IO registers.

 @param [in]   SaveRestoreFlag  True: write data to Pic registers.
                                False: read data from Pic registers to global registers.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestorePic (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                             Index;
  UINT8                             PicBuffer = 0;

  if (SaveRestoreFlag) {
    //PIC2
    IoWrite8 (I8259A_CTRL, 0x11);
    IoWrite8 (I8259A_MASK, 0x70);
    IoWrite8 (I8259A_MASK, 0x02);
    IoWrite8 (I8259A_MASK, 0x01);
    IoWrite8 (I8259A_MASK, 0x00);
    // PIC1
    IoWrite8 (I8259_CTRL, 0x11);
    IoWrite8 (I8259_MASK, 0x08);
    IoWrite8 (I8259_MASK, 0x04);
    IoWrite8 (I8259_MASK, 0x01);
    IoWrite8 (I8259_MASK, 0x00);

//    IoWrite8 (I8259_CTRL, 0x11);
//    IoWrite8 (I8259_MASK, PIC1_ICW2);
//    IoWrite8 (I8259_MASK, 0x04);
//    IoWrite8 (I8259_CTRL, 0x01);
//    IoWrite8 (I8259_CTRL, I8259_MASK);
//    // PIC2    
//    IoWrite8 (I8259_CTRL, 0x11);
//    IoWrite8 (I8259_MASK, PIC1_ICW2);
//    IoWrite8 (I8259_MASK, 0x02);
//    IoWrite8 (I8259_CTRL, 0x01);
//    IoWrite8 (I8259_CTRL, I8259_MASK);
  } else {

    // Disable Interrupt Mask
    I8259Mask = IoRead8 (I8259_MASK);
    I8259AMask = IoRead8 (I8259A_MASK);

    IoWrite8 (I8259_MASK, 0xFF);
    IoWrite8 (I8259A_MASK, 0xFF);

    // Save PIC1 and PCI2 ICW2
    PicBuffer = IoRead8 (I8259_CTRL);
    PicBuffer |= 1;
    PicBuffer &= 0xFD;
    PIC1_ICW2 = PicBuffer;

    for (Index = 0, PicBuffer = 0; Index < 6; Index++) {
      PicBuffer = IoRead8 (I8259_CTRL);
    }

    PicBuffer |= 1;
    PicBuffer &= 0xFD;
    PIC2_ICW2 = PicBuffer;

    IoWrite8 (I8259_MASK, I8259Mask);
    IoWrite8 (I8259A_MASK, I8259AMask);

  }

  return  EFI_SUCCESS;
}

/**
 This function either writes to or read from GPIO registers.

 @param [in]   SaveRestoreFlag  True: write data to GPIO registers.
                                False: read data from GPIO registers to global registers.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestoreGpio (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                                  Index;
//[-start-121211-IB05300367-modify]//
//[-start-121212-IB05300368-modify]//
  PCH_SERIES                        PchSeries;

  Index = 0;
  PchSeries = GetPchSeries ();

  if (PchSeries == PchH) {
//[-start-120731-IB10820094-modify]//
    while (GPIO_REG[Index] != 0xFF) {
      if (SaveRestoreFlag) {
        IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + GPIO_REG [Index], GPIO_REG_SAVE [Index]);
     } else {
        GPIO_REG_SAVE [Index] = IoRead32 (PcdGet16(PcdPchGpioBaseAddress) + GPIO_REG [Index]);
     }
//[-end-120731-IB10820094-modify]//
     Index++;
    }
  }

  if (PchSeries == PchLp) {
    while (GPIO_REG_ULT[Index] != 0xFFFF) {
      if (SaveRestoreFlag) {
        IoWrite32 ((UINT16) PcdGet16(PcdPchGpioBaseAddress) + GPIO_REG_ULT[Index], GPIO_REG_SAVE_ULT[Index]);
      } else {
        GPIO_REG_SAVE_ULT[Index] = IoRead32 ((UINT16) PcdGet16(PcdPchGpioBaseAddress) + GPIO_REG_ULT[Index]);
      }
      Index++;
    }
  }
//[-end-121212-IB05300368-modify]//
//[-end-121211-IB05300367-modify]//

  return EFI_SUCCESS;
}

/**
 This function either writes to or read from LevelEdge IO registers.

 @param [in]   SaveRestoreFlag  True: write data to IO registers.
                                False: read data from IO registers to global registers.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestoreLevelEdge (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  if (SaveRestoreFlag) {
    IoWrite8 (ELCR_PORT, ELCR[0]);
    IoWrite8 (ELCR_PORT + 1, ELCR[1]);
  } else {
    ELCR[0] = IoRead8 (ELCR_PORT);
    ELCR[1] = IoRead8 (ELCR_PORT + 1);
  }

  return  EFI_SUCCESS;
}

/**
 This function either saves or restores CPU state.

 @param [in]   SaveRestoreFlag  True: Restores CPU state.
                                False: Save CPU state.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestoreBSPState (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINT32                         MsrIndex;
  UINT32                         CurrentCpu;
  UINTN                          Index;
  UINT32                         *Cr0Address = NULL;
  EFI_CPUID_REGISTER             Cpuid;
  UINT64                         MsrValue = 0;
  UINT32                      CpreApicID;

  CpreApicID = GetApicID ();
  AsmCpuid (CPUID_VERSION_INFO,&Cpuid.RegEax,&Cpuid.RegEbx,&Cpuid.RegEcx,&Cpuid.RegEdx);
  CurrentCpu = Cpuid.RegEax;

  //
  //  Save/ Restore L1 State
  //
  if (SaveRestoreFlag) {
    ResCR0 (BSP_CR0_Register);

    Cr0Address = (UINT32 *)(UINTN)(SMM_HANDLER_CR0);
    *Cr0Address = BSP_CR0_Register;

  } else {
    SaveCR0 (&BSP_CR0_Register);
  }

  // if cpu is Pentium-M cpu, then save these msr registers
  if ((CurrentCpu & 0x600) == 0x600) {
    Index = 0;

    while (CPU_MSR_REG[Index] != 0xFFFF) {
      MsrIndex = (UINT32)CPU_MSR_REG[Index];
      if (SaveRestoreFlag) {

         if (MsrIndex == MSR_IA32_FEATURE_CONFIG) { 
           if (Cpuid.RegEcx & B_CPUID_VERSION_INFO_ECX_AES) {
             MsrValue = AsmReadMsr64(MSR_IA32_FEATURE_CONFIG);
             if (MsrValue == 0) { 
               AsmWriteMsr64 (MsrIndex, BSP_MSR_REG_SAVE[Index]);
             }
           }
         } else {
           AsmWriteMsr64 (MsrIndex, BSP_MSR_REG_SAVE[Index]);
         }
      } else {
        if (MsrIndex == MSR_IA32_FEATURE_CONFIG) {
          if ( Cpuid.RegEcx & B_CPUID_VERSION_INFO_ECX_AES) {
            BSP_MSR_REG_SAVE [Index] = AsmReadMsr64(MsrIndex);
          }
        } else {
          BSP_MSR_REG_SAVE [Index] = AsmReadMsr64(MsrIndex);
        }
      }

      Index++;
    }
  }
  
  if (SaveRestoreFlag) {
    VTSupport ();
  }
  return  EFI_SUCCESS;
}

/**
 This function either saves or restores CPU state.

 @param [in]   FlagBuffer   True: Restores CPU state.
                            False: Save CPU state.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
EFIAPI
SaveRestoreAPState (
  IN  VOID        *FlagBuffer
  )
{
  UINT32                         MsrIndex;
  UINTN                          CurrentCpu;
  UINTN                          Index;
  UINT32                         *Cr0Address = NULL;
  BOOLEAN                        SaveRestoreFlag;
  UINT64                         MsrValue;
  UINT32                         CpreApicID;
  EFI_CPUID_REGISTER             Cpuid;
  CpreApicID = GetApicID ();
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  

  SaveRestoreFlag = *(BOOLEAN *)FlagBuffer;

  CurrentCpu = mSmst->CurrentlyExecutingCpu;

  //
  //  Save/ Restore L1 State
  //
  if (SaveRestoreFlag) {
    ResCR0 (AP_CR0_Register[CurrentCpu]);

    Cr0Address = (UINT32 *)(UINTN)(SMM_HANDLER_CR0);
    *Cr0Address = AP_CR0_Register[CurrentCpu];

  } else {
    SaveCR0 (&AP_CR0_Register[CurrentCpu]);
  }

    Index = 0;

    while (CPU_MSR_REG[Index] != 0xFFFF) {
      MsrIndex = (UINT32)CPU_MSR_REG[Index];
      if (SaveRestoreFlag) {
        
        if (MsrIndex == MSR_IA32_FEATURE_CONFIG) {
          if (Cpuid.RegEcx & B_CPUID_VERSION_INFO_ECX_AES) {
            MsrValue = AsmReadMsr64 (MSR_IA32_FEATURE_CONFIG);
            if (MsrValue == 0) { 
              if ((CpreApicID == 0x2) || (CpreApicID == 0x4) || (CpreApicID == 0x6)) {
                AsmWriteMsr64 (MsrIndex, AP_MSR_REG_SAVE [Index][CurrentCpu]);
              }
            }
          }
        } else {
          AsmWriteMsr64 (MsrIndex, AP_MSR_REG_SAVE [Index][CurrentCpu]);
        }
      } else {
        if (MsrIndex == MSR_IA32_FEATURE_CONFIG) {
          if (Cpuid.RegEcx & B_CPUID_VERSION_INFO_ECX_AES) {
            AP_MSR_REG_SAVE [Index][CurrentCpu] = AsmReadMsr64(MsrIndex);
          }
        } else {
          AP_MSR_REG_SAVE [Index][CurrentCpu] = AsmReadMsr64(MsrIndex);
        }
      }

      Index++;
    }
  if (SaveRestoreFlag) {
    VTSupport ();
  }

  return  EFI_SUCCESS;
}

/**
 Disable APIC

 @param None.

 @retval None.
 
**/
STATIC
VOID
DisableAPIC (
  )
{
  UINT64          ApicBaseReg;
  UINT8           *ApicBase;
  UINTN           Index;
  UINTN           Offset;
  
  ApicBaseReg = AsmReadMsr64(MSR_IA32_APIC_BASE);
//[-start-121213-IB10820193-modify]//
  if (ApicBaseReg & APIC_GLOBAL_ENABLE) {
//[-end-121213-IB10820193-modify]//
    ApicBase = (UINT8*)(UINTN)(ApicBaseReg & 0xffffff000);
    //
    // Backup the whole LVTs due to software disable APIC will set the mask value of LVTs
    //
    for (Index = 0, Offset = APIC_REGISTER_TIMER_VECTOR_OFFSET; Index < 6; Index ++, Offset += 0x10) {
      ApicLVT[Index] = *(UINT32*)(UINTN)(ApicBase + Offset);
    }
    //
    // Backup the TPR
    //
    ApicTPR = *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET);
    //
    // Set the TPR to 0xff to block whole queuing interrupts(in the IRR)
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET) = 0xff;
    //
    // Software disable APIC
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET) &= ~0x100;
  }
}

/**
 Enable APIC

 @param None.

 @retval None.

**/
STATIC
VOID
EnableAPIC (
  )
{
  UINT64 ApicBaseReg;
  UINT8  *ApicBase;
  UINTN  Index;
  UINTN  Offset;

  ApicBaseReg = AsmReadMsr64(MSR_IA32_APIC_BASE);
//[-start-121213-IB10820193-modify]//
  if (ApicBaseReg & APIC_GLOBAL_ENABLE) {
//[-end-121213-IB10820193-modify]//
    ApicBase = (UINT8*)(UINTN)(ApicBaseReg & 0xffffff000);
    //
    // Software enable APIC
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET) |= 0x100;
    //
    // Restore whole LVTs
    //
    for (Index = 0, Offset = APIC_REGISTER_TIMER_VECTOR_OFFSET; Index < 6; Index ++, Offset += 0x10) {
      *(UINT32*)(UINTN)(ApicBase + Offset) = ApicLVT[Index];
    }
    //
    // Restore the TPR
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET) = ApicTPR;
  }
}
/**
 Before setting MTRR, we need to disable interrupt and cache.

 @param None.

 @retval None

**/
EFI_STATUS
EfiPreMtrrChange()
{
  UINT64                      TempQword;

  //
  // Disable Interrupt
  //
//  TempQword = EfiReadMsr(EFI_MSR_IA32_APIC_BASE);
//  EfiWriteMsr(EFI_MSR_IA32_APIC_BASE, TempQword & ~EFI_APIC_GLOBAL_ENABLE);
  DisableAPIC ();
  //
  // if it is P4 processor, Disable Cache, else don't disable cache,
  // otherwise the system will hang
  //
  AsmDisableCache ();

  //
  // Disable Cache MTRR
  //
  TempQword = AsmReadMsr64(CACHE_IA32_MTRR_DEF_TYPE);
  AsmWriteMsr64(CACHE_IA32_MTRR_DEF_TYPE, TempQword & ~B_CACHE_MTRR_VALID);

  return EFI_SUCCESS;
}

/**
 Enable cache after we set MTRR.

 @param None.

 @retval None

**/
EFI_STATUS
EfiPostMtrrChange()
{
  UINT64                      TempQword=0;

  //
  // Enable Cache MTRR
  //
  TempQword = AsmReadMsr64(CACHE_IA32_MTRR_DEF_TYPE);
  AsmWriteMsr64(CACHE_IA32_MTRR_DEF_TYPE, TempQword | B_CACHE_MTRR_VALID);

  //
  // if it is P4 processor, re- enable L2 cache
  //
  AsmEnableCache ();
  
  //
  // Enable Interrupt
  //
  EnableAPIC();  
//  TempQword = EfiReadMsr(EFI_MSR_IA32_APIC_BASE);
//  EfiWriteMsr(EFI_MSR_IA32_APIC_BASE, TempQword | EFI_APIC_GLOBAL_ENABLE);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveRestoreMtrr (
  IN  VOID        *FlagBuffer
  )
///*++
//
//Routine Description:
//
//  This function either saves or restores Mtrr registers.
//
//Arguments:
//
//  Smst - instance of SMM system table.
//
//  SaveRestoreFlag - True: Restores Mtrr registers.
//                    False: Save Mtrr registers.
//
//Returns:
//
//  EFI_SUCCESS - if read or write is successful.
//
//--*/
{
  UINT32                           Index;
  BOOLEAN                          SaveRestoreFlag;

  SaveRestoreFlag = *(BOOLEAN *)FlagBuffer;
  
  if (SaveRestoreFlag) {
    EfiPreMtrrChange();
  }

  // Save/Restore Fixed Mtrr
  Index = 0;
  while (FixedMtrr[Index] != 0xFFFFFFFF) {
    if (SaveRestoreFlag) {
      AsmWriteMsr64 (FixedMtrr[Index], FixedMtrrSave[Index]);
    } else {
      FixedMtrrSave[Index] = AsmReadMsr64 (FixedMtrr[Index]);
    }
    Index++;
  }
   for (Index = CACHE_VARIABLE_MTRR_BASE; Index <(CACHE_VARIABLE_MTRR_BASE + (V_MAXIMUM_VARIABLE_MTRR_NUMBER * 2)); Index += 2) {
     if (SaveRestoreFlag) {
       AsmWriteMsr64 (Index, VariableMtrrSave[Index - CACHE_VARIABLE_MTRR_BASE]);
       AsmWriteMsr64 (Index + 1, VariableMtrrSave[Index - CACHE_VARIABLE_MTRR_BASE + 1]);
     } else {
       VariableMtrrSave [Index - CACHE_VARIABLE_MTRR_BASE] = AsmReadMsr64 (Index);
       VariableMtrrSave [Index - CACHE_VARIABLE_MTRR_BASE + 1] = AsmReadMsr64 (Index + 1);
     }
   }
  if (SaveRestoreFlag) {
    //
    // Set MTRRdefType register
    //
    AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, EFI_MTRR_DEF_TYPE_ENABLE);

    EfiPostMtrrChange();
  }

  return  EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveRestoreMCSEG (
  IN      VOID    *FlagBuffer
  )
{
  UINT64     MtrrCap;
  BOOLEAN    SaveRestoreFlag;
  UINT8      MaxBus;

  MtrrCap         = 0;
  SaveRestoreFlag = *( BOOLEAN * )FlagBuffer;
  MaxBus          = 0;

  MtrrCap = AsmReadMsr64 ( IA32_MTRR_CAP );
  if ( !( MtrrCap & B_IA32_MTRR_CAP_SMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }
  if ( !( MtrrCap & B_IA32_MTRR_CAP_EMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }

  MaxBus = GetMaxBusNumber ();
  if ( SaveRestoreFlag ) {
    if ( !( MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_LOWER ) & B_UNCORE_CR_MCSEG_MASK_LOCK ) ) {
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_BASE_UPPER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_UPPER_SAVE;
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_BASE_LOWER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_LOWER_SAVE;
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_UPPER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_UPPER_SAVE;
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_LOWER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE & 0xFFFFF000;
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_LOWER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE & 0xFFFFF800;
      MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_LOWER ) = SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE;
    }
  } else {
    SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_UPPER_SAVE = MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_BASE_UPPER );
    SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_LOWER_SAVE = MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_BASE_LOWER );
    SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_UPPER_SAVE = MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_UPPER );
    SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE = MmPci32 ( 0, MaxBus, UNCORE_DEV, SAD_FUNC, UNCORE_CR_MCSEG_MASK_LOWER );
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveRestoreRegisterEMRR (
  IN      VOID    *FlagBuffer
  )
{
  UINT64     MtrrCap;
  BOOLEAN    SaveRestoreFlag;

  MtrrCap         = 0;
  SaveRestoreFlag = *( BOOLEAN * )FlagBuffer;

  MtrrCap = AsmReadMsr64 ( IA32_MTRR_CAP );
  if ( !( MtrrCap & B_IA32_MTRR_CAP_SMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }
  if ( !( MtrrCap & B_IA32_MTRR_CAP_EMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }

  if ( SaveRestoreFlag ) {
    if ( !( AsmReadMsr64 ( EMRR_PHYS_MASK ) & B_MSR_EMRR_PHYS_MASK_LOCK ) ) {
      AsmWriteMsr64 ( EMRR_PHYS_BASE, MSR_EMRR_PHYS_BASE_SAVE & 0xFFFFFFFFFFFFF000 );
      AsmWriteMsr64 ( EMRR_PHYS_BASE, MSR_EMRR_PHYS_BASE_SAVE );
      AsmWriteMsr64 ( EMRR_PHYS_MASK, MSR_EMRR_PHYS_MASK_SAVE & 0xFFFFFFFFFFFFF000 );
//      EfiWriteMsr ( EFI_EMRR_PHYS_MASK, EfiReadMsr ( EFI_EMRR_PHYS_MASK ) | B_EFI_EMRR_PHYS_MASK_LOCK );
    }
  } else {
    MSR_EMRR_PHYS_BASE_SAVE = AsmReadMsr64 ( EMRR_PHYS_BASE );
    MSR_EMRR_PHYS_MASK_SAVE = AsmReadMsr64 ( EMRR_PHYS_MASK );
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveRestoreLockEMRR (
  IN      VOID    *FlagBuffer
  )
{
  UINT64     MtrrCap;
  BOOLEAN    SaveRestoreFlag;

  MtrrCap         = 0;
  SaveRestoreFlag = *( BOOLEAN * )FlagBuffer;

  MtrrCap = AsmReadMsr64 ( IA32_MTRR_CAP );
  if ( !( MtrrCap & B_IA32_MTRR_CAP_SMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }
  if ( !( MtrrCap & B_IA32_MTRR_CAP_EMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }

  if ( SaveRestoreFlag ) {
    if ( !( AsmReadMsr64 ( EMRR_PHYS_MASK ) & B_MSR_EMRR_PHYS_MASK_LOCK ) ) {
      AsmWriteMsr64 ( EMRR_PHYS_MASK, AsmReadMsr64 ( EMRR_PHYS_MASK ) | B_MSR_EMRR_PHYS_MASK_LOCK );
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveRestoreEMRR (
  IN      VOID    *FlagBuffer
  )
{
  EFI_STATUS    Status;
  UINTN         CpuIndex;
  UINT64        MtrrCap;
  BOOLEAN       SaveRestoreFlag;

  CpuIndex        = 0;
  MtrrCap         = 0;
  SaveRestoreFlag = *( BOOLEAN * )FlagBuffer;

  MtrrCap = AsmReadMsr64 ( IA32_MTRR_CAP );
  if ( !( MtrrCap & B_IA32_MTRR_CAP_SMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }
  if ( !( MtrrCap & B_IA32_MTRR_CAP_EMRR_SUPPORT ) ) {
    return EFI_UNSUPPORTED;
  }

  Status = SaveRestoreMCSEG ( &SaveRestoreFlag );
  if ( EFI_ERROR ( Status ) ) {
    return Status;
  }

  Status = SaveRestoreRegisterEMRR ( &SaveRestoreFlag );
  if ( EFI_ERROR ( Status ) ) {
    return Status;
  }
  if ( SaveRestoreFlag ) {
    for ( CpuIndex = 0 ; CpuIndex < mSmst->NumberOfCpus ; CpuIndex = CpuIndex + 1 ) {
      MicroSecondDelay ( 100 );
      mSmst->SmmStartupThisAp ( SaveRestoreRegisterEMRR, CpuIndex, &SaveRestoreFlag );
    }
  }

  if ( SaveRestoreFlag ) {
    Status = SaveRestoreLockEMRR ( &SaveRestoreFlag );
    if ( EFI_ERROR ( Status ) ) {
      return Status;
    }
    for ( CpuIndex = 0 ; CpuIndex < mSmst->NumberOfCpus ; CpuIndex = CpuIndex + 1 ) {
      MicroSecondDelay ( 10 );
      mSmst->SmmStartupThisAp ( SaveRestoreLockEMRR, CpuIndex, &SaveRestoreFlag );
    }
  }

  return EFI_SUCCESS;
}

/**
 This function initializes the DMA controller.

 @param None.

 @retval EFI_SUCCESS            always returned.

**/
EFI_STATUS
DmaInit ()
{
  UINTN                      Index;

  IoWrite8 ((DMA1_BASE + DMA_CMD), 0);
  IoWrite8 ((DMA2_BASE + DMA_CMD * 2), 0);

  for (Index = 0; Index < 4 ;Index++) {
    IoWrite8 ((DMA1_BASE + DMA_CMD), (UINT8)(0x40 + Index));
    IoWrite8 ((DMA2_BASE + DMA_CMD * 2), (UINT8)(0x40 + Index));
  }

  IoWrite8 ((DMA2_BASE + DMA_MODE * 2), 0xC0);

  IoWrite8 ((DMA2_BASE + DMA_REQ * 2), 0);
  IoWrite8 ((DMA2_BASE + DMA_MASK * 2), 0);

  return  EFI_SUCCESS;
}

EFI_STATUS
SaveRestoreDmi (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                       Index;
  UINT64                      DmiBar;

  DmiBar = 0;

//[-start-120404-IB05300309-modify]//
  DmiBar = McD0PciCfg64 (R_SA_DMIBAR) & ~BIT0;
//[-end-120404-IB05300309-modify]//

  for ( Index = 0 ; DMIBAR_IO[Index] != 0xFFFFFFFF ; Index++ ) {
    if ( SaveRestoreFlag ) {
      Mmio32 ( DmiBar, DMIBAR_IO[Index] ) = DMIBAR_IO_SAVE[Index];
    } else {
      DMIBAR_IO_SAVE[Index] = Mmio32 ( DmiBar, DMIBAR_IO[Index] );
    }
  }

  return  EFI_SUCCESS;
}

//[-start-121218-IB08050189-remove]//
//EFI_STATUS
//SaveRestoreRcba (
//  IN  BOOLEAN                       SaveRestoreFlag
//  )
//{
//  volatile UINT8             *Mmio;
//  UINTN                      Index;
//
//  //
//  // SaveRestoreFlag = TRUE : Restore
//  //                 = FALSE: Save
//  //
//  for (Index=0; Index<0x2330; Index++) {
//    Mmio = (UINT8 *) (PCH_RCBA_ADDRESS + Index);
//    if (SaveRestoreFlag) {
//      if (Index != R_PCH_SPI_HSFS + 1) {
//        *Mmio = RCBABAR_IO_SAVE[Index];
//      }
//    } else {
//      RCBABAR_IO_SAVE[Index] = *Mmio;
//    }
//  }
//  
//  //
//  // PPT EDS 1.0: 2330h ~ 2340h is IOBP, RC already handle this part
//  //
//  
//  for (Index=0x2340; Index<0x4000; Index++) {
//    Mmio = (UINT8 *) (PCH_RCBA_ADDRESS + Index);
//    if (SaveRestoreFlag) {
//      if (Index != R_PCH_SPI_HSFS + 1) {
//        *Mmio = RCBABAR_IO_SAVE[Index];
//      }
//    } else {
//      RCBABAR_IO_SAVE[Index] = *Mmio;
//    }
//  }
//
////[-start-120719-IB07360209-add]//
////
//// Clear HSFS AEL (Hardware Sequencing Flash Status Register - Access Error Log).
////
//  if (SaveRestoreFlag) {
//    Mmio = (UINT8 *) (UINTN)(PCH_RCBA_ADDRESS + R_PCH_SPI_HSFS);
//    *Mmio |= B_PCH_SPI_HSFS_AEL;
//  }
////[-end-120719-IB07360209-add]//
//
//  //
//  // If (FLOCKDN) value is retored on sequence, it will cause the rest of SPI reg. failed to restore
//  // until HW reset. Hence, we don't retore FLOCKDN until everything restored.
//  // Plz refer to IBX-PK EDS v1.1 p.865
//  // (FLOCKDN): Flash Configuration Lock-Down @ SPIBAR + 04h bit [15]
//  //
//  if (SaveRestoreFlag) {
//    Mmio = (UINT8 *) (PCH_RCBA_ADDRESS + (UINTN)(R_PCH_SPI_HSFS + 1));
//    *Mmio = RCBABAR_IO_SAVE[R_PCH_SPI_HSFS + 1];
//  }
//
//  return  EFI_SUCCESS;
//}
//[-end-121218-IB08050189-remove]//

EFI_STATUS
SaveRestoreMch (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                      Index;

  for (Index = 0; MCHBAR_IO[Index] != 0xffffffff; Index++) {
    if (SaveRestoreFlag) {
      McMmio32(MCHBAR_IO[Index]) = MCHBAR_IO_SAVE[Index];
    } else {
      MCHBAR_IO_SAVE[Index] = McMmio32(MCHBAR_IO[Index]);
    }
  }

  return  EFI_SUCCESS;
}

EFI_STATUS
SaveRestoreEp (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  UINTN                       Index;
  UINT64                      mEgressPortBar;
//[-start-121214-IB10820194-modify]//
  mEgressPortBar  = PcdGet64 (PcdPciExpressBaseAddress);
//[-end-121214-IB10820194-modify]//
  for (Index = 0; EPBAR_IO[Index] != 0xffffffff; Index++) {
    if (SaveRestoreFlag) {
      Mmio32(mEgressPortBar, EPBAR_IO[Index]) = EPBAR_IO_SAVE[Index];
    } else {
      EPBAR_IO_SAVE[Index] = Mmio32(mEgressPortBar, EPBAR_IO[Index]);
    }
  }

  return  EFI_SUCCESS;
}

EFI_STATUS
SaveRestoreAzalia (
  IN  BOOLEAN                           SaveRestoreFlag
  )
{
  UINTN                                 Index;

  for (Index = 0; AZALIA_MMIO[Index] != 0xffffffff; Index++) {
    if (SaveRestoreFlag) {
      MmPci32(0,
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_AZALIA,
              PCI_FUNCTION_NUMBER_PCH_AZALIA,
              AZALIA_MMIO[Index]) = AZALIA_MMIO_SAVE[Index];
    } else {
      AZALIA_MMIO_SAVE[Index] = MmPci32(0,
                                        DEFAULT_PCI_BUS_NUMBER_PCH,
                                        PCI_DEVICE_NUMBER_PCH_AZALIA,
                                        PCI_FUNCTION_NUMBER_PCH_AZALIA,
                                        AZALIA_MMIO[Index]);
    }
  }

  return  EFI_SUCCESS;
}

/**

 @param [in]   SaveRestoreMsrFlag   True: Restores CPU MSR.
                                    False: Save CPU MSR.

 @retval EFI_SUCCESS            if read or write is successful.

**/
EFI_STATUS
SaveRestoreMsr (
  IN SAVE_RESTORE_MSR_PARAMETER  *SaveRestoreMsrFlag
  )
{
  UINT16      Index;
  UINT64      *MsrValueIndex;
  EFI_STATUS  Status;

  if (SaveRestoreMsrFlag->SaveRestoreFlag) {
    if (MsrValue [SaveRestoreMsrFlag->CpuLogicalID] == NULL)
      return EFI_SUCCESS;
    MsrValueIndex = MsrValue[SaveRestoreMsrFlag->CpuLogicalID];
    for (Index = 0; MsrIndex[Index] != 0xffff; Index++, MsrValueIndex++) {
      AsmWriteMsr64 (MsrIndex[Index], *MsrValueIndex);
    }
  } else {
    Index = 0;
    while (MsrIndex [Index] != 0xFFFF)
      Index ++;
    if (Index == 0)
      return EFI_SUCCESS;
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData,
                                     Index * 8,
                                     (VOID **) &MsrValue[SaveRestoreMsrFlag->CpuLogicalID]);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    MsrValueIndex = MsrValue[SaveRestoreMsrFlag->CpuLogicalID];
    for (Index = 0; MsrIndex[Index] != 0xffff; Index++, MsrValueIndex++) {
      *MsrValueIndex = AsmReadMsr64(MsrIndex[Index]);
    }
  }
  return EFI_SUCCESS;
}

UINT8
GetMaxBusNumber (
  VOID
  )
{
  UINT8    MaxBusNumber;

  MaxBusNumber = 0;

  //
  // EFI_PCIEXBAR MSR 0x300[3:1]
  //
  switch ( ( ( ( UINT8 )AsmReadMsr64 ( 0x300 ) ) & ( 0xE ) ) >> 1 ) {
    case 0x06:
      MaxBusNumber = 0x3F;
      break;

    case 0x07:
      MaxBusNumber = 0x7F;
      break;

    case 0x00:
    default:
      MaxBusNumber = 0xFF;
      break;
  }

  return MaxBusNumber;
}

EFI_STATUS
SaveEDPReg (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  if (SaveRestoreFlag) {

    McD1PciCfg8Or(0xDFC, EDP_PCI_SAVE[0]);
    McD1PciCfg8Or(0xF8A, EDP_PCI_SAVE[1]);
    McD1PciCfg8And(0xF8B, EDP_PCI_SAVE[2]);
    McD1PciCfg8Or(0xFAA, EDP_PCI_SAVE[3]);
    McD1PciCfg8And(0xFAB, EDP_PCI_SAVE[4]);
    McD1PciCfg8Or(0xFCA, EDP_PCI_SAVE[5]);
    McD1PciCfg8And(0xFCB, EDP_PCI_SAVE[6]);
    McD1PciCfg8Or(0xFEA, EDP_PCI_SAVE[7]);
    McD1PciCfg8And(0xFEB, EDP_PCI_SAVE[8]);

  } else {
    
    EDP_PCI_SAVE[0] = McD1PciCfg8(0xDFC);
    EDP_PCI_SAVE[1] = McD1PciCfg8(0xF8A);
    EDP_PCI_SAVE[2] = McD1PciCfg8(0xF8B);
    EDP_PCI_SAVE[3] = McD1PciCfg8(0xFAA);
    EDP_PCI_SAVE[4] = McD1PciCfg8(0xFAB);
    EDP_PCI_SAVE[5] = McD1PciCfg8(0xFCA);
    EDP_PCI_SAVE[6] = McD1PciCfg8(0xFCB);
    EDP_PCI_SAVE[7] = McD1PciCfg8(0xFEA);
    EDP_PCI_SAVE[8] = McD1PciCfg8(0xFEB);

  }
  return  EFI_SUCCESS;
}

//[-start-121204-IB08050187-add]//
/*++

 This function save the specific SPI registers and restore it after resume from S3.

 @param [in]    SaveRestoreFlag    True : Restore these SPI registers.
                                   False: Save these SPI registers.

 @retval        EFI_SUCCESS

--*/
EFI_STATUS 
SaveRestoreSpiReg (
  IN  BOOLEAN    SaveRestoreFlag
  )
{
  UINTN           Index;

  //
  // Save SPI Registers for S3 resume usage
  //
  for (Index = 0; SPI_MMIO[Index] != 0xFFFFFFFF; Index++) {
    if (SaveRestoreFlag) {
      //
      // Clear HSFS AEL (Hardware Sequencing Flash Status Register - Access Error Log)
      //
      if (SPI_MMIO[Index] == R_PCH_SPI_HSFS) {
        SPI_MMIO_SAVE[Index] |= B_PCH_SPI_HSFS_AEL;
      }      

      //
      // Set the Vendor Component Lock (VCL) bits
      //
      if (SPI_MMIO[Index] == R_PCH_SPI_VSCC0) {
        SPI_MMIO_SAVE[Index] |= B_PCH_SPI_VSCC0_VCL;
      }

      //
      // Restore these SPI register after S3 resume.
      //
      Mmio32 (FixedPcdGet32 (PcdRcbaBaseAddress), SPI_MMIO[Index]) = SPI_MMIO_SAVE[Index];
    } else {
      //
      // Save these SPI registers
      //
      SPI_MMIO_SAVE[Index] = Mmio32 (FixedPcdGet32 (PcdRcbaBaseAddress), SPI_MMIO[Index]);
    }
  }
  return EFI_SUCCESS;
}
//[-end-121204-IB08050187-add]//
