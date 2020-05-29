/** @file
  Execute XTU settings in pei phase.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/CmosLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//

//[-start-121129-IB03780468-remove]//
//#include <Ppi/PchReset/PchReset.h>
//[-end-121129-IB03780468-remove]//
#include <Ppi/ReadOnlyVariable2.h>
//[-start-121129-IB03780468-remove]//
//#include <Ppi/SaPlatformPolicy.h>
//[-end-121129-IB03780468-remove]//
#include <Ppi/Reset.h>
//[-start-121129-IB03780468-remove]//
//#include <Ppi/Wdtt/Wdtt.h>
//[-end-121129-IB03780468-remove]//

#include <ChipsetSetupConfig.h>
//[-start-121212-IB10820191-modify]//
#include <CpuRegs.h>
//[-end-121212-IB10820191-modify]//
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <MemInfoHob.h>
#include <ChipsetCmos.h>
#include <Guid/XTUInfoHob.h>
//[-start-121212-IB10820191-add]//
#include <PchAccess.h>
#include <Cpu.h>
//[-end-121212-IB10820191-add]//

EFI_GUID  gMemRestoreDataGuid = EFI_MEMORY_RESTORE_DATA_GUID;
//[-start-121212-IB10820191-remove]//
//#pragma pack(1)
//typedef union _MSR_REGISTER {
// UINT64  Qword;
//
//  struct _DWORDS {
//    UINT32  Low;
//    UINT32  High;
//  } Dwords;
//
//  struct _BYTES {
//    UINT8 FirstByte;
//    UINT8 SecondByte;
//    UINT8 ThirdByte;
//    UINT8 FouthByte;
//    UINT8 FifthByte;
//    UINT8 SixthByte;
//    UINT8 SeventhByte;
//    UINT8 EighthByte;
//  } Bytes;
//
//} MSR_REGISTER;
//#pragma pack()
//[-end-121212-IB10820191-remove]//

//[-start-121129-IB03780468-modify]//
/**
 This routine is for XTU PEI phase Porting.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         Ppi                 A pointer to AtaPolicyPpi PPI.

 @retval            EFI_SUCCESS         Porting Success
*/

EFI_STATUS
EFIAPI
InitialXtuInfoData (
  IN  CONST EFI_PEI_SERVICES           **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN  VOID                             *Ppi
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_CPU_IO_PPI            *CpuIo;
  UINT64                        MsrValue;
  XTU_INFO_DATA                 XTUInfoHobData;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi;
  UINTN                         VariableSize;
  CHIPSET_CONFIGURATION         *SystemConfiguration;
//[-start-130710-IB05160465-modify]//
  VOID                          *SystemConfigurationData;
//[-end-130710-IB05160465-modify]//
  EFI_GUID                      SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8                         OverclockingBins;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariablePpi
             );
  ASSERT_EFI_ERROR (Status);

//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfigurationData = AllocateZeroPool (VariableSize);

  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          L"Setup",
                          &SystemConfigurationGuid,
                          NULL,
                          &VariableSize,
                          SystemConfigurationData
                          );
  ASSERT_EFI_ERROR (Status);

  SystemConfiguration = SystemConfigurationData;
//[-end-130709-IB05160465-modify]//

  CpuIo = (**PeiServices).CpuIo;

  //
  // Create XTU InfoHob, for store some registers' default value or get some dynamical range of specific registers
  //

  // save 1core ~ 4core limit default value
  MsrValue = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  XTUInfoHobData.TurboRatioLimitDefault.LimitCore1 = (UINT8)(MsrValue >> N_MSR_TURBO_RATIO_LIMIT_1C);
  XTUInfoHobData.TurboRatioLimitDefault.LimitCore2 = (UINT8)(MsrValue >> N_MSR_TURBO_RATIO_LIMIT_2C);
  XTUInfoHobData.TurboRatioLimitDefault.LimitCore3 = (UINT8)(MsrValue >> N_MSR_TURBO_RATIO_LIMIT_3C);
  XTUInfoHobData.TurboRatioLimitDefault.LimitCore4 = (UINT8)(MsrValue >> N_MSR_TURBO_RATIO_LIMIT_4C);

  //
  // Overclocking availablity for unlocked processor
  //
  MsrValue  = AsmReadMsr64 (MSR_FLEX_RATIO);
  OverclockingBins = (UINT8) ((MsrValue >> 17) & 0x07);

  if (OverclockingBins < 7) {
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore1 = XTUInfoHobData.TurboRatioLimitDefault.LimitCore1 + OverclockingBins;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore2 = XTUInfoHobData.TurboRatioLimitDefault.LimitCore2 + OverclockingBins;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore3 = XTUInfoHobData.TurboRatioLimitDefault.LimitCore3 + OverclockingBins;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore4 = XTUInfoHobData.TurboRatioLimitDefault.LimitCore4 + OverclockingBins;
  } else {
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore1 = 0x50;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore2 = 0x50;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore3 = 0x50;
    XTUInfoHobData.TurboRatioLimitMaximum.LimitCore4 = 0x50;
  }

  // save range of Non-Turbo Ratio
  MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
  XTUInfoHobData.CpuNonTurboRatio.MaxRatio = (UINT8)(MsrValue >> N_PLATFORM_INFO_MAX_RATIO);
  XTUInfoHobData.CpuNonTurboRatio.MinRatio = (UINT8)(MsrValue >> N_PLATFORM_INFO_MIN_RATIO);

  //save Turbo Limit1, Limit2 TDP default value
  MsrValue = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  XTUInfoHobData.TruboTdpLimitDefault.PowerLimit1Default = (UINT16)((MsrValue >> N_EFI_MSR_TURBO_POWER_LIMIT_1_LIMIT) & POWER_LIMIT_MASK);
  XTUInfoHobData.TruboTdpLimitDefault.PowerLimit2Default = (UINT16)((MsrValue >> N_EFI_MSR_TURBO_POWER_LIMIT_2_LIMIT) & POWER_LIMIT_MASK);
  MsrValue = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  XTUInfoHobData.TruboTdpLimitDefault.PowerLimit1Default /= (1 << (MsrValue & PACKAGE_POWER_UNIT_MASK));
  XTUInfoHobData.TruboTdpLimitDefault.PowerLimit2Default /= (1 << (MsrValue & PACKAGE_POWER_UNIT_MASK));

  BuildGuidDataHob (&gXTUInfoHobGuid, &XTUInfoHobData, sizeof (XTU_INFO_DATA));

  return EFI_SUCCESS;
}
//[-end-121129-IB03780468-modify]//

//[-start-121129-IB03780468-modify]//
EFI_STATUS
InitialXmpProfileData (
  IN  CONST EFI_PEI_SERVICES           **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN  VOID                             *Ppi
  )
{
  XTU_INFO_DATA                *XtuInfoHobData = NULL;
  HOB_SAVE_MEMORY_DATA         *MrcHobData = NULL;
  EFI_PEI_HOB_POINTERS         Hob;
  EFI_STATUS                   Status;
  UINT8                        Index;
  UINT8                        Profile;

  Status = PeiServicesGetHobList ((VOID **)&Hob.Raw);
  if (!EFI_ERROR (Status)) {
    while (!END_OF_HOB_LIST (Hob)) {
      if ((Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid (&Hob.Guid->Name, &gXTUInfoHobGuid))) {
        XtuInfoHobData = (XTU_INFO_DATA *) (Hob.Raw + sizeof (EFI_HOB_GUID_TYPE));
        break;
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  }

  Status = PeiServicesGetHobList ((VOID **)&Hob.Raw);
  if (!EFI_ERROR (Status)) {
    while (!END_OF_HOB_LIST (Hob)) {
      if ((Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid (&Hob.Guid->Name, &gMemRestoreDataGuid))) {
        MrcHobData = (HOB_SAVE_MEMORY_DATA *) Hob.Raw;
        break;
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  }

  if (XtuInfoHobData != NULL && MrcHobData != NULL) {
    for (Index = 0; Index < MAX_CHANNEL; Index++) {
      if (MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Status == (UINTN)DIMM_PRESENT) {
        break;
      }
    }
//[-start-140620-IB05080432-modify]//
    if (Index < MAX_CHANNEL) {
      //
      // Default Profile
      //
      Profile = STD_PROFILE;
      XtuInfoHobData->XmpProfile.Default.DdrMultiplier  = MrcHobData->MrcData.SysOut.Outputs.Ratio;
      XtuInfoHobData->XmpProfile.Default.DdrRefClk      = MrcHobData->MrcData.SysOut.Outputs.RefClk;
      XtuInfoHobData->XmpProfile.Default.tCL            = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCL;
      XtuInfoHobData->XmpProfile.Default.tRCD           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRCD;
      XtuInfoHobData->XmpProfile.Default.tRP            = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRP;
      XtuInfoHobData->XmpProfile.Default.tRAS           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRAS;  
      XtuInfoHobData->XmpProfile.Default.tWR            = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWR;
      XtuInfoHobData->XmpProfile.Default.tRFC           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRFC;
      XtuInfoHobData->XmpProfile.Default.tRRD           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRRD;
      XtuInfoHobData->XmpProfile.Default.tWTR           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWTR;
      XtuInfoHobData->XmpProfile.Default.tRTP           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRTP;
      XtuInfoHobData->XmpProfile.Default.tRC            = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRC;  
      XtuInfoHobData->XmpProfile.Default.tFAW           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tFAW;
      XtuInfoHobData->XmpProfile.Default.tCWL           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCWL;
      XtuInfoHobData->XmpProfile.Default.tREFI          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tREFI;
      XtuInfoHobData->XmpProfile.Default.tRPAB          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRPab;
      XtuInfoHobData->XmpProfile.Default.NMode          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].NMode;
  
      //
      // XMP Profile 1
      //
      Profile = XMP_PROFILE1;
      XtuInfoHobData->XmpProfile.Profile1.DdrMultiplier = MrcHobData->MrcData.SysOut.Outputs.Ratio;
      XtuInfoHobData->XmpProfile.Profile1.DdrRefClk     = MrcHobData->MrcData.SysOut.Outputs.RefClk;
      XtuInfoHobData->XmpProfile.Profile1.tCL           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCL;
      XtuInfoHobData->XmpProfile.Profile1.tRCD          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRCD;
      XtuInfoHobData->XmpProfile.Profile1.tRP           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRP;
      XtuInfoHobData->XmpProfile.Profile1.tRAS          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRAS;  
      XtuInfoHobData->XmpProfile.Profile1.tWR           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWR;
      XtuInfoHobData->XmpProfile.Profile1.tRFC          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRFC;
      XtuInfoHobData->XmpProfile.Profile1.tRRD          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRRD;
      XtuInfoHobData->XmpProfile.Profile1.tWTR          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWTR;
      XtuInfoHobData->XmpProfile.Profile1.tRTP          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRTP;
      XtuInfoHobData->XmpProfile.Profile1.tRC           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRC;  
      XtuInfoHobData->XmpProfile.Profile1.tFAW          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tFAW;
      XtuInfoHobData->XmpProfile.Profile1.tCWL          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCWL;
      XtuInfoHobData->XmpProfile.Profile1.tREFI         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tREFI;
      XtuInfoHobData->XmpProfile.Profile1.tRPAB         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRPab;
      XtuInfoHobData->XmpProfile.Profile1.NMode         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].NMode;
  
      //
      // XMP Profile 2
      //
      Profile = XMP_PROFILE2;
      XtuInfoHobData->XmpProfile.Profile2.DdrMultiplier = MrcHobData->MrcData.SysOut.Outputs.Ratio;
      XtuInfoHobData->XmpProfile.Profile2.DdrRefClk     = MrcHobData->MrcData.SysOut.Outputs.RefClk;
      XtuInfoHobData->XmpProfile.Profile2.tCL           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCL;
      XtuInfoHobData->XmpProfile.Profile2.tRCD          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRCD;
      XtuInfoHobData->XmpProfile.Profile2.tRP           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRP;
      XtuInfoHobData->XmpProfile.Profile2.tRAS          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRAS;  
      XtuInfoHobData->XmpProfile.Profile2.tWR           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWR;
      XtuInfoHobData->XmpProfile.Profile2.tRFC          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRFC;
      XtuInfoHobData->XmpProfile.Profile2.tRRD          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRRD;
      XtuInfoHobData->XmpProfile.Profile2.tWTR          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tWTR;
      XtuInfoHobData->XmpProfile.Profile2.tRTP          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRTP;
      XtuInfoHobData->XmpProfile.Profile2.tRC           = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRC;  
      XtuInfoHobData->XmpProfile.Profile2.tFAW          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tFAW;
      XtuInfoHobData->XmpProfile.Profile2.tCWL          = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tCWL;
      XtuInfoHobData->XmpProfile.Profile2.tREFI         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tREFI;
      XtuInfoHobData->XmpProfile.Profile2.tRPAB         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].tRPab;
      XtuInfoHobData->XmpProfile.Profile2.NMode         = MrcHobData->MrcData.SysOut.Outputs.Controller[0].Channel[Index].Timing[Profile].NMode;      
    }
//[-end-140620-IB05080432-modify]//
  }
  
  return EFI_SUCCESS;
}
//[-end-121129-IB03780468-modify]//
