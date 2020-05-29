/** @file
  Execute XTU settings in DXE phase.

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

#include "Platform.h"
#include "CpuIA32.h"
#include "CpuRegs.h"
#include "SaAccess.h"
#include "SaRegs.h"
#include "EfiScriptLib.h"
#include "IccOverClocking.h"

EFI_GUID              gIccOverClockingProtocolGuid1 = ICC_OVERCLOCKING_PROTOCOL_GUID;

#include EFI_GUID_DEFINITION (XTUInfoHob)
#include EFI_PROTOCOL_CONSUMER (PchS3Support)

//
// GACI structure definition
//
typedef struct ControlIdData
{
  UINT32 ControlId;
  UINT16 NumberOfValues;
  UINT8 Precision;
  UINT8 Flags;
  UINT32 DefaultDataValue;
  UINT32 MinDataValue;
  UINT32 MaxDataValue;
  UINT32 MinDisplayValue;
  UINT32 MaxDisplayValue;
} CONTROLID_DATA;

//[-start-121130-IB03780468-modify]//
#define SUPPORTED_CONTROLID_COUNT 48 // Count of 6 is an example
//[-end-121130-IB03780468-modify]//
//[-start-120521-IB03780442-add]//
#define MEMORY_CLOCK_MULTIPLIER   133
//[-end-120521-IB03780442-add]//

typedef struct CtlBufer
{
  CONTROLID_DATA CtrlID[SUPPORTED_CONTROLID_COUNT];
} CONTROLID_BUFF;

EFI_STATUS
PassXTUDataToAsl (
  IN XTU_INFO_DATA                    *XTUInfoHobData
);

typedef struct {   
UINT16 ControlID;
UINT8  Reserved;
UINT8  Precision;
UINT32 DisplayValue;
} XMP_DISPLAY_VALUE;

EFI_STATUS
IccHostClocking (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
);

EFI_STATUS
GtOverclocking (
  IN EFI_HANDLE                        ImageHandle,
  IN CHIPSET_CONFIGURATION              *SetupVariable
);

BOOLEAN
DetectGtOverClockCap (
  IN UINT64                            GTTMMADR 
);

EFI_STATUS
ProcessGtOverclocking (
  IN UINT64                            GTTMMADR,
  IN UINT8                             GtVoltage,
  IN UINT8                             GtFreq
);

EFI_STATUS
SetPollStatusS3Item (
  IN   UINT64                          MmioAddress,
  IN   EFI_BOOT_SCRIPT_WIDTH           Width,
  IN   UINT64                          Mask,
  IN   UINT64                          Value
);

/**
 This routine is for XTU DXE phase Porting.

 @param [in]   ImageHandle
 @param [in, out] SetupVariable

 @retval EFI_SUCCESS            Porting Success

**/
EFI_STATUS
DxeXtuExecute (
  IN EFI_HANDLE                        ImageHandle,
  IN OUT CHIPSET_CONFIGURATION          *SetupVariable
  )
{
  EFI_STATUS            Status;
//[-start-121130-IB03780468-remove]//
//  UINT8                 XtuFlag;
//[-end-121130-IB03780468-remove]//
  EFI_EVENT             ProcessIccHostClockExEvent;
  VOID                  *Registration;

//[-start-121130-IB03780468-remove]//
////[-start-120215-IB03780423-modify]//
//  //
//  // Detect overclocking(XE or unlocked processors) capability:
//  //
//  // SandyBridge BWG 0.6 section 14.15.2
//  // XeCap detect for CPUID 206A3 (C0) and above: 
//  // 1. MSR 0xCE[28] == 1     (Programmable Ratio Limits for Turbo Mode)
//  // 2. MSR 0x194[19:17] == 7 (Number of overclocking bins supported)
//  //
//  if ((EfiReadMsr (EFI_PLATFORM_INFORMATION) & BIT28) 
//    && ((EfiReadMsr (EFI_IA32_CLOCK_FLEX_MAX) & (BIT17|BIT18|BIT19)) != 0)) {
//    SetupVariable->XECap = 1;
//  } else {
//    SetupVariable->XECap = 0;
//  }
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "CPU XECap : %x\n", SetupVariable->XECap ) );
////[-end-120215-IB03780423-modify]//
//[-end-121130-IB03780468-remove]//


//[-start-121130-IB03780468-remove]//
//  //
//  //======================
//  // GT Overclocking
//  //======================
//  //
//  GtOverclocking (ImageHandle, SetupVariable);
//[-end-121130-IB03780468-remove]//


  //
  //======================
  // Host Clock Frequency 
  //======================
  //
  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_CALLBACK,
                  IccHostClocking,
                  (VOID *)SetupVariable,
                  &ProcessIccHostClockExEvent
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gIccOverClockingProtocolGuid1,
                    ProcessIccHostClockExEvent,
                    &Registration
                    );
  }

//[-start-121130-IB03780468-add]//
  Status = gBS->SignalEvent (ProcessIccHostClockExEvent);
  ASSERT_EFI_ERROR (Status);
//[-end-121130-IB03780468-add]//

//[-start-121130-IB03780468-remove]//
//  //
//  // Detect if system crash with XTU enabled
//  //
//  XtuFlag = EfiReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus);
//  if (XtuFlag & B_XTU_FLAG_FAIL) {
//    //SetupVariable->OverClocking = 0;
//    SetupVariable->CpuBurnInEnable      = 0;
//    SetupVariable->ExtremeEdition       = 0;
//    SetupVariable->EnableMemoryOverride = 0;
//    SetupVariable->XmpProfileSetting    = 0;
//    SetupVariable->GtOverclockSupport   = 0;
//    SetupVariable->CpuBurnInEnable      = 0;
//    SetupVariable->ExtremeEdition       = 0;
//    SetupVariable->GtOverclockSupport   = 0;
//    XtuFlag &= ~B_XTU_FLAG_FAIL;
//    EfiWriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, XtuFlag);
//  }
//  
//
//  //
//  // Init SCU settings
//  //
//  if (!SetupVariable->OverClocking) {
//    SetupVariable->CpuBurnInEnable      = 0;
//    SetupVariable->ExtremeEdition       = 0;
//    SetupVariable->EnableMemoryOverride = 0;
//    SetupVariable->XmpProfileSetting    = 0;
//    SetupVariable->GtOverclockSupport   = 0;
//  }
//  if (!SetupVariable->XECap) {
//    SetupVariable->CpuBurnInEnable      = 0;
//    SetupVariable->ExtremeEdition       = 0;
//  }
//  if (!SetupVariable->GtOverclockCap) {
//    SetupVariable->GtOverclockSupport   = 0;
//  }
//
//  //
//  // check XMP status, if XMP WatchDog triggered, set SCU to default
//  // 
//  XtuFlag = EfiReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus );
//  if ( XtuFlag & B_XTU_WATCH_DOG_FLAG_XMP ) {
//    SetupVariable->XmpProfileSetting = 0; // Default
//    
//    XtuFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_XMP;
//    EfiWriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, XtuFlag); // clear status
//  }
//[-end-121130-IB03780468-remove]//

  return EFI_SUCCESS;
}

EFI_STATUS
IccHostClocking (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                       Status;
  CHIPSET_CONFIGURATION             *SetupVariable;
  VOID                             *HobList;
  XTU_INFO_DATA                    *XTUInfoHobData;
  ICC_OVERCLOCKING_PROTOCOL        *IccProtocol;
  ICC_LIB_STATUS                   ICCStatus;
  ICC_LIB_VERSION                  ICCLibVersion;
  UINT8                            NumberOfClockCount;
  ICC_CLOCK_ID                     BCLKIndex;
  ICC_CLOCK_RANGES                 CurrentClockRangeDef;
  ICC_CLOCK_SETTINGS               CurrentClockSettings;
  ICC_CLOCK_FREQUENCY              FreqHz;
  EFI_GUID                         GuidId = SYSTEM_CONFIGURATION_GUID;
  BOOLEAN                          SaveSetupVariable = TRUE, ApplyToIcc = FALSE;

  BCLKIndex                         = 0;
  CurrentClockRangeDef.UsageMask    = 0;
  CurrentClockRangeDef.FrequencyMin = 0;
  CurrentClockRangeDef.FrequencyMax = 0;

//[-start-121130-IB03780468-add]//
  Status = gBS->LocateProtocol (&gIccOverClockingProtocolGuid1, NULL, (VOID **)&IccProtocol);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
//[-end-121130-IB03780468-add]//

  SetupVariable = (CHIPSET_CONFIGURATION *)Context;

  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  Status = GetNextGuidHob ( &HobList, &gXTUInfoHobGuid, &XTUInfoHobData, NULL );
  ASSERT_EFI_ERROR (Status);

//[-start-121130-IB03780468-remove]//
//  Status = gBS->LocateProtocol ( &gIccOverClockingProtocolGuid1, NULL, &IccProtocol );
//  ASSERT_EFI_ERROR (Status);
//[-end-121130-IB03780468-remove]//

  IccProtocol->GetInfo ( &ICCLibVersion, &NumberOfClockCount, &ICCStatus );
  ASSERT ( ICCStatus == ICC_LIB_STATUS_SUCCESS );

  //
  // Get BCLK : max, min, current. write to XtuInfo for SPTT dynamically modified 
  //
  for ( BCLKIndex = 0 ; BCLKIndex < NumberOfClockCount ; BCLKIndex = BCLKIndex + 1 ) {
    IccProtocol->GetClockRanges ( BCLKIndex, &CurrentClockRangeDef, &ICCStatus );
    ASSERT ( ICCStatus == ICC_LIB_STATUS_SUCCESS );

    if ( CurrentClockRangeDef.UsageMask & ( 1 << ICC_CLOCK_USAGE_BCLK ) ) {
      break;
    }
  }
  if ( BCLKIndex == NumberOfClockCount ) {
    return EFI_ABORTED;
  }
  // max, min
  FreqHz = ( UINT32 )( CurrentClockRangeDef.FrequencyMin );
  XTUInfoHobData->HostClockData.MaxSupportFreq = (UINT16)((FreqHz / 100000) * 10);
  FreqHz = ( UINT32 )( CurrentClockRangeDef.FrequencyMax );
  XTUInfoHobData->HostClockData.MinSupportFreq = (UINT16)((FreqHz / 100000) * 10);
  // current
  IccProtocol->GetCurrentClockSettings ( BCLKIndex, &CurrentClockSettings, &ICCStatus );
  ASSERT ( ICCStatus == ICC_LIB_STATUS_SUCCESS );

  FreqHz = ( UINT32 )( CurrentClockSettings.Frequency );
  XTUInfoHobData->HostClockData.CurrentFreq = (UINT16)((FreqHz / 100000) * 10);

  if ( SetupVariable->HostClockFreq == XTUInfoHobData->HostClockData.CurrentFreq
    && SetupVariable->HostClockFreq == SetupVariable->HostClockFreqBackup) {
    //
    // 0. HostClockFreq == HostClockFreqBackup == current
    //     -> do nothing
    //
    SaveSetupVariable = FALSE;
  } else if ( SetupVariable->HostClockFreq == XTUInfoHobData->HostClockData.CurrentFreq
    || SetupVariable->HostClockFreq == SetupVariable->HostClockFreqBackup) {
    //
    // 1. HostClockFreq == current (mean not change via AP or SCU)
    // 2. HostClockFreq == HostClockFreqBackup (mean BCLK change via SCU ICC SDK or first boot (both ==0))
    //     ->  set HostClockFreq = HostClockFreqBackup = current
    //
    SetupVariable->HostClockFreq = SetupVariable->HostClockFreqBackup = XTUInfoHobData->HostClockData.CurrentFreq;
  } else {
    //
    // 3. BCLK change via AP
    //     -> Apply new setting to ICC SDK, save current value to backup 
    //
    SetupVariable->HostClockFreqBackup = SetupVariable->HostClockFreq;  // if apply to icc fail, it will set to safe value in next boot
    ApplyToIcc = TRUE;
  }

  if (SaveSetupVariable) {
    Status = gRT->SetVariable (
                    L"Setup",
                    &GuidId,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//[-start-130709-IB05160465-modify]//
                    PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                    (VOID *) SetupVariable
                    );
  }

  if (ApplyToIcc) {
    CurrentClockSettings.Frequency = SetupVariable->HostClockFreq * 10000;

    IccProtocol->SetCurrentClockSettings ( BCLKIndex, CurrentClockSettings, &ICCStatus );
    ASSERT ( ICCStatus == ICC_LIB_STATUS_SUCCESS );

    IccProtocol->SetBootClockSettings ( BCLKIndex, CurrentClockSettings, &ICCStatus );
    ASSERT ( ICCStatus == ICC_LIB_STATUS_SUCCESS );
  }

  PassXTUDataToAsl(XTUInfoHobData);

//[-start-121130-IB03780468-add]//
  Status = gBS->CloseEvent (Event);
  ASSERT_EFI_ERROR (Status);
//[-end-121130-IB03780468-add]//
  
  return EFI_SUCCESS;
}

//[-start-121130-IB03780468-remove]//
//EFI_STATUS
//GtOverclocking (
//  IN EFI_HANDLE                        ImageHandle,
//  IN CHIPSET_CONFIGURATION              *SetupVariable
//  )
//{
//  EFI_STATUS            Status;
//  EFI_PHYSICAL_ADDRESS  MemBaseAddress;
//  UINT32                LoGTBaseAddress;
//  UINT32                HiGTBaseAddress;
//  UINT64                GTTMMADR;
//
//  if (McD2PciCfg16 (IGD_R_VID) == 0xFFFF) {
//    SetupVariable->GtOverclockCap = 0;
//  } else {
//    GTTMMADR = (UINT64) (McD2PciCfg32 (IGD_R_GTTMMADR));
//    GTTMMADR =  LShiftU64 ((UINT64)McD2PciCfg32 (IGD_R_GTTMMADR+4), 32) |(GTTMMADR);
//    GTTMMADR = GTTMMADR &~(BIT2 | BIT1);
//
//    if (GTTMMADR) {
//      SetupVariable->GtOverclockCap = DetectGtOverClockCap(GTTMMADR);
//
//      if (SetupVariable->GtOverclockCap && SetupVariable->GtOverclockSupport) {
//        ProcessGtOverclocking(GTTMMADR, SetupVariable->GtOverclockVoltage, SetupVariable->GtOverclockFreq);
//      }
//    } else {
//      gDS     = NULL;
//      Status  = EfiLibGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID **) &gDS);
//      ASSERT_EFI_ERROR (Status);
//
//      //
//      // Enable Bus Master, I/O and Memory access on 0:2:0
//      //
//      McD2PciCfg8Or (IGD_R_CMD, (BIT2 | BIT1));
//      
//      //
//      // Means Allocate 4MB for GTTMADDR
//      //
//      MemBaseAddress = 0x0ffffffffffffffff;
//
//      Status = gDS->AllocateMemorySpace (
//                      EfiGcdAllocateAnySearchBottomUp,
//                      EfiGcdMemoryTypeMemoryMappedIo,
//                      22,
//                      0x400000,
//                      &MemBaseAddress,
//                      ImageHandle,
//                      NULL
//                      );
//      ASSERT_EFI_ERROR (Status);
//
//      //
//      // Program GT PM Settings if GTTMMADR allocation is Successful
//      //
//      GTTMMADR                          = (UINTN)MemBaseAddress;
//      LoGTBaseAddress                   = (UINT32) (MemBaseAddress & 0xFFFFFFFF);
//      HiGTBaseAddress                   = (UINT32) RShiftU64 ((MemBaseAddress & 0xFFFFFFFF00000000), 32);
//      McD2PciCfg32 (IGD_R_GTTMMADR)     = LoGTBaseAddress | BIT2;
//      McD2PciCfg32 (IGD_R_GTTMMADR + 4) = HiGTBaseAddress;
//
//      SetupVariable->GtOverclockCap = DetectGtOverClockCap(GTTMMADR);
//
//      if (SetupVariable->GtOverclockCap && SetupVariable->GtOverclockSupport) {
//        UINT8                ByteData;
//        UINT32               DwordData;
//        
//        DwordData = McD2PciCfg32 (IGD_R_GTTMMADR);
//        SCRIPT_MEM_WRITE (
//          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
//          EfiBootScriptWidthUint32,
//          (UINTN) (MmPciAddress (0,
//                  0,
//                  2,
//                  0,
//                  IGD_R_GTTMMADR)),
//          1,
//          &DwordData
//          );
//
//        DwordData = McD2PciCfg32 (IGD_R_GTTMMADR + 4);
//        SCRIPT_MEM_WRITE (
//          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
//          EfiBootScriptWidthUint32,
//          (UINTN) (MmPciAddress (0,
//                  0,
//                  2,
//                  0,
//                  IGD_R_GTTMMADR + 4)),
//          1,
//          &DwordData
//          );
//
//        ByteData = McD2PciCfg8 (IGD_R_CMD);
//        SCRIPT_MEM_WRITE (
//          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
//          EfiBootScriptWidthUint8,
//          (UINTN) (MmPciAddress (0,
//                  0,
//                  2,
//                  0,
//                  IGD_R_CMD)),
//          1,
//          &ByteData
//          );
//
//        ProcessGtOverclocking(GTTMMADR, SetupVariable->GtOverclockVoltage, SetupVariable->GtOverclockFreq);
//      }
//
//      McD2PciCfg32And (IGD_R_CMD, ~(BIT2 | BIT1));
//      McD2PciCfg64 (IGD_R_GTTMMADR) = BIT2;
//
//      //
//      // Free allocated resources
//      //
//      gDS->FreeMemorySpace (MemBaseAddress, 0x400000);
//    } // GTTMMADR detect
//  } // 0-2-0 detect
//
//  return EFI_SUCCESS;
//}
//
//BOOLEAN
//DetectGtOverClockCap (
//  IN UINT64                            GTTMMADR 
//  )
//{
//  UINT32            Data32;
//  
//  //
//  // Wait for Mailbox ready
//  //
//  while ((Mmio32 (GTTMMADR, 0x138124)) & BIT31);
//
//  //
//  // Mailbox Command
//  //
//  Data32 = 0x8000000C;
//  Mmio32 (GTTMMADR, 0x138124) = Data32;
//
//  //
//  // Wait for Mailbox ready
//  //
//  while ((Mmio32 (GTTMMADR, 0x138124)) & BIT31);
//
//  //
//  // MailBox Data , if bit[31] == 1, this cpu support Gt Overclocking
//  //
//  Data32 = Mmio32 (GTTMMADR, 0x138128);
//
//  return ((Data32 & BIT31) ? 1 : 0);
//}
//
//EFI_STATUS
//ProcessGtOverclocking (
//  IN UINT64                            GTTMMADR,
//  IN UINT8                             GtVoltage,
//  IN UINT8                             GtFreq
//  )
//{
//  UINT32            Data32, Data32Mask, Result;
//  
//  //
//  // Wait for Mailbox ready
//  //
//  while ((PchMmio32 (GTTMMADR, 0x138124)) & BIT31);
//
//  Data32Mask  = BIT31;
//  Result      = 0;
//  SetPollStatusS3Item (
//    (UINTN) (GTTMMADR + 0x138124),
//    EfiBootScriptWidthUint32,
//    Data32Mask,
//    Result
//    );
//
//  //
//  // Mailbox Data  - [15:8] Extra Voltage (1/256 volt), [7:0] Frequency (50MHz steps)
//  //
//  Data32 = GtFreq;
//  Data32 |= (GtVoltage << 8);
//  Mmio32 (GTTMMADR, 0x138128) = Data32;
//
//  SCRIPT_MEM_WRITE (
//    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
//    EfiBootScriptWidthUint32,
//    (UINTN) (GTTMMADR + 0x138128),
//    1,
//    &Data32
//    );
//
//  //
//  // Mailbox Command
//  //
//  Data32 = 0x8000000B;
//  Mmio32 (GTTMMADR, 0x138124) = Data32;
//
//  SCRIPT_MEM_WRITE (
//    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
//    EfiBootScriptWidthUint32,
//    (UINTN) (GTTMMADR + 0x138124),
//    1,
//    &Data32
//    );
//
//  //
//  // Wait for Mailbox ready
//  //
//  while ((Mmio32 (GTTMMADR, 0x138124)) & BIT31);
//        
//  Data32Mask  = BIT31;
//  Result      = 0;
//  SetPollStatusS3Item (
//    (UINTN) (GTTMMADR + 0x138124),
//    EfiBootScriptWidthUint32,
//    Data32Mask,
//    Result
//    );
//
//  return EFI_SUCCESS;
//}
//
//
///**
// Set a "Poll Status" S3 dispatch item
//
// @param [in]   MmioAddress      Address
// @param [in]   Width            Operation Width
// @param [in]   Mask             Mask
// @param [in]   Value            Value to wait for
//
// @retval EFI_STATUS
//
//**/
//EFI_STATUS
//SetPollStatusS3Item (
//  IN   UINT64                          MmioAddress, 
//  IN   EFI_BOOT_SCRIPT_WIDTH           Width,
//  IN   UINT64                          Mask,
//  IN   UINT64                          Value
//  )
//{
//  EFI_STATUS                              Status;
//  STATIC EFI_PCH_S3_SUPPORT_PROTOCOL      *PchS3Support;
//  STATIC EFI_PCH_S3_PARAMETER_POLL_STATUS S3ParameterPollStatus;
//  STATIC EFI_PCH_S3_DISPATCH_ITEM         S3DispatchItem = {
//    PchS3ItemTypePollStatus,
//    &S3ParameterPollStatus
//  };
//  EFI_PHYSICAL_ADDRESS                    S3DispatchEntryPoint;
//
//  if (!PchS3Support) {
//    //
//    // Get the PCH S3 Support Protocol
//    //
//    Status = gBS->LocateProtocol (
//                    &gEfiPchS3SupportProtocolGuid,
//                    NULL,
//                    &PchS3Support
//                    );
//    ASSERT_EFI_ERROR (Status);
//    if (EFI_ERROR (Status)) {
//      return Status;
//    }
//  }
//
//  S3ParameterPollStatus.MmioAddress = MmioAddress;
//  S3ParameterPollStatus.Width       = Width;
//  S3ParameterPollStatus.Mask        = Mask;
//  S3ParameterPollStatus.Value       = Value;
//  S3ParameterPollStatus.Timeout     = 10000000;
//  //
//  // 10s
//  //
//  Status = PchS3Support->SetDispatchItem (
//                          PchS3Support,
//                          &S3DispatchItem,
//                          &S3DispatchEntryPoint
//                          );
//  ASSERT_EFI_ERROR (Status);
//  //
//  // Save the script dispatch item in the Boot Script
//  //
//  SCRIPT_DISPATCH (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, S3DispatchEntryPoint);
//
//  return Status;
//}
//[-end-121130-IB03780468-remove]//

/**
 pass XTU data to XTU.asl(GlobalNvsArea XTUB)

 @param [in]   XTUInfoHobData       Address

 @retval EFI_STATUS

**/
EFI_STATUS
PassXTUDataToAsl (
  IN XTU_INFO_DATA                    *XTUInfoHobData
)
{
  EFI_STATUS                                Status;
  CONTROLID_BUFF                            *CtlBuf = NULL;
  XMP_DISPLAY_VALUE                         *XmpDataBuffer;
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsArea;
  UINT8                                     MemoryFrequency;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsArea);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  sizeof(CONTROLID_BUFF),
                  (VOID **)&CtlBuf
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  EfiZeroMem (CtlBuf, sizeof(CONTROLID_BUFF));

//[-start-120521-IB03780442-modify]//
//[-start-120210-IB03780422-modify]//
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  186,
                  (VOID **)&XmpDataBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiZeroMem (XmpDataBuffer, 186);
//[-end-120210-IB03780422-modify]//
//[-end-120521-IB03780442-modify]//

  switch (XTUInfoHobData->XmpProfile.Default.DdrMultiplier) {
    case 1067:
      MemoryFrequency = 4;
      break;

    case 1333:
      MemoryFrequency = 5;
      break;

    case 1600:
      MemoryFrequency = 6;
      break;

    case 1867:
      MemoryFrequency = 7;
      break;

    case 2133:
      MemoryFrequency = 8;
      break;

    default:
      MemoryFrequency = 5;
      break;
  }

//[-start-121130-IB03780468-modify]//
  //
  // MaxNonTurboRatio
  //
  CtlBuf->CtrlID[0].ControlId = 0x00;
  CtlBuf->CtrlID[0].NumberOfValues = XTUInfoHobData->CpuNonTurboRatio.MaxRatio - XTUInfoHobData->CpuNonTurboRatio.MinRatio +1;
  CtlBuf->CtrlID[0].Precision = 0x00;
  CtlBuf->CtrlID[0].Flags = 0x00;
  CtlBuf->CtrlID[0].DefaultDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[0].MinDataValue = XTUInfoHobData->CpuNonTurboRatio.MinRatio;
  CtlBuf->CtrlID[0].MaxDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[0].MinDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MinRatio;
  CtlBuf->CtrlID[0].MaxDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  //
  // HostClockFrequency
  //  
  CtlBuf->CtrlID[1].ControlId = 0x01;
  CtlBuf->CtrlID[1].NumberOfValues = 0x01;
  CtlBuf->CtrlID[1].Precision = 0x02;
  CtlBuf->CtrlID[1].Flags = 0x00;
  CtlBuf->CtrlID[1].DefaultDataValue = 0x2710;
  CtlBuf->CtrlID[1].MinDataValue = 0x2710;
  CtlBuf->CtrlID[1].MaxDataValue = 0x2710;
  CtlBuf->CtrlID[1].MinDisplayValue = 0x2710;
  CtlBuf->CtrlID[1].MaxDisplayValue = 0x2710;
  //
  // CpuVoltageOverride
  //  
  CtlBuf->CtrlID[2].ControlId = 0x02;
  CtlBuf->CtrlID[2].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[2].Precision = 0x03;
  CtlBuf->CtrlID[2].Flags = 0x00;
  CtlBuf->CtrlID[2].DefaultDataValue = 0x4B0;
  CtlBuf->CtrlID[2].MinDataValue = 0x00;
  CtlBuf->CtrlID[2].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[2].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[2].MaxDisplayValue = 0x7D0;
  //
  // MemoryVoltage
  //  
  CtlBuf->CtrlID[3].ControlId = 0x05;
  CtlBuf->CtrlID[3].NumberOfValues = 0x04;
  CtlBuf->CtrlID[3].Precision = 0x02;
  CtlBuf->CtrlID[3].Flags = 0x00;
  CtlBuf->CtrlID[3].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[3].MinDataValue = 0x00;
  CtlBuf->CtrlID[3].MaxDataValue = 0x03;
  CtlBuf->CtrlID[3].MinDisplayValue = 0x96;
  CtlBuf->CtrlID[3].MaxDisplayValue = 0xA5;
  //
  // CasLatency
  //  
  CtlBuf->CtrlID[4].ControlId = 0x07;
  CtlBuf->CtrlID[4].NumberOfValues = 0x0F;
  CtlBuf->CtrlID[4].Precision = 0x00;
  CtlBuf->CtrlID[4].Flags = 0x00;
  CtlBuf->CtrlID[4].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tCL;
  CtlBuf->CtrlID[4].MinDataValue = 0x04;
  CtlBuf->CtrlID[4].MaxDataValue = 0x12;
  CtlBuf->CtrlID[4].MinDisplayValue = 0x04;
  CtlBuf->CtrlID[4].MaxDisplayValue = 0x12;
  //
  // TRCD
  //  
  CtlBuf->CtrlID[5].ControlId = 0x08;
  CtlBuf->CtrlID[5].NumberOfValues = 0x26;
  CtlBuf->CtrlID[5].Precision = 0x00;
  CtlBuf->CtrlID[5].Flags = 0x00;
  CtlBuf->CtrlID[5].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRCD;
  CtlBuf->CtrlID[5].MinDataValue = 0x01;
  CtlBuf->CtrlID[5].MaxDataValue = 0x26;
  CtlBuf->CtrlID[5].MinDisplayValue = 0x01;
  CtlBuf->CtrlID[5].MaxDisplayValue = 0x26;
  //
  // TRP
  //  
  CtlBuf->CtrlID[6].ControlId = 0x09;
  CtlBuf->CtrlID[6].NumberOfValues = 0x26;
  CtlBuf->CtrlID[6].Precision = 0x00;
  CtlBuf->CtrlID[6].Flags = 0x00;
  CtlBuf->CtrlID[6].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRP;
  CtlBuf->CtrlID[6].MinDataValue = 0x01;
  CtlBuf->CtrlID[6].MaxDataValue = 0x26;
  CtlBuf->CtrlID[6].MinDisplayValue = 0x01;
  CtlBuf->CtrlID[6].MaxDisplayValue = 0x26;
  //
  // TRAS
  //  
  CtlBuf->CtrlID[7].ControlId = 0x0A;
  CtlBuf->CtrlID[7].NumberOfValues = 0x24B;
  CtlBuf->CtrlID[7].Precision = 0x00;
  CtlBuf->CtrlID[7].Flags = 0x00;
  CtlBuf->CtrlID[7].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRAS;
  CtlBuf->CtrlID[7].MinDataValue = 0x00;
  CtlBuf->CtrlID[7].MaxDataValue = 0x24A;
  CtlBuf->CtrlID[7].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[7].MaxDisplayValue = 0x24A;
  //
  // TWR
  //
  CtlBuf->CtrlID[8].ControlId = 0x0B;
  CtlBuf->CtrlID[8].NumberOfValues = 0x27;
  CtlBuf->CtrlID[8].Precision = 0x00;
  CtlBuf->CtrlID[8].Flags = 0x00;
  CtlBuf->CtrlID[8].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tWR;
  CtlBuf->CtrlID[8].MinDataValue = 0x00;
  CtlBuf->CtrlID[8].MaxDataValue = 0x26;
  CtlBuf->CtrlID[8].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[8].MaxDisplayValue = 0x26;
  //
  // MemoryMultiplier
  //
  CtlBuf->CtrlID[9].ControlId = 0x13;
  CtlBuf->CtrlID[9].NumberOfValues = 0x07;
  CtlBuf->CtrlID[9].Precision = 0x02;
  CtlBuf->CtrlID[9].Flags = 0x00;
  CtlBuf->CtrlID[9].DefaultDataValue = MemoryFrequency;
  CtlBuf->CtrlID[9].MinDataValue = 0x04;
  CtlBuf->CtrlID[9].MaxDataValue = 0x0A;
  CtlBuf->CtrlID[9].MinDisplayValue = 0x320;
  CtlBuf->CtrlID[9].MaxDisplayValue = 0x7D0;
  //
  // TRFC
  //
  CtlBuf->CtrlID[10].ControlId = 0x15;
  CtlBuf->CtrlID[10].NumberOfValues = 0x2493;
  CtlBuf->CtrlID[10].Precision = 0x00;
  CtlBuf->CtrlID[10].Flags = 0x00;
  CtlBuf->CtrlID[10].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRFC;
  CtlBuf->CtrlID[10].MinDataValue = 0x01;
  CtlBuf->CtrlID[10].MaxDataValue = 0x2493;
  CtlBuf->CtrlID[10].MinDisplayValue = 0x01;
  CtlBuf->CtrlID[10].MaxDisplayValue = 0x2493;
  //
  // TRRD
  //
  CtlBuf->CtrlID[11].ControlId = 0x16;
  CtlBuf->CtrlID[11].NumberOfValues = 0x23;
  CtlBuf->CtrlID[11].Precision = 0x00;
  CtlBuf->CtrlID[11].Flags = 0x00;
  CtlBuf->CtrlID[11].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRRD;
  CtlBuf->CtrlID[11].MinDataValue = 0x04;
  CtlBuf->CtrlID[11].MaxDataValue = 0x26;
  CtlBuf->CtrlID[11].MinDisplayValue = 0x04;
  CtlBuf->CtrlID[11].MaxDisplayValue = 0x26;
  //
  // TWTR
  //
  CtlBuf->CtrlID[12].ControlId = 0x17;
  CtlBuf->CtrlID[12].NumberOfValues = 0x23;
  CtlBuf->CtrlID[12].Precision = 0x00;
  CtlBuf->CtrlID[12].Flags = 0x00;
  CtlBuf->CtrlID[12].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tWTR;
  CtlBuf->CtrlID[12].MinDataValue = 0x04;
  CtlBuf->CtrlID[12].MaxDataValue = 0x26;
  CtlBuf->CtrlID[12].MinDisplayValue = 0x04;
  CtlBuf->CtrlID[12].MaxDisplayValue = 0x26;
  //
  // CommandRate
  //  
  CtlBuf->CtrlID[13].ControlId = 0x18;
  CtlBuf->CtrlID[13].NumberOfValues = 0x03;
  CtlBuf->CtrlID[13].Precision = 0x00;
  CtlBuf->CtrlID[13].Flags = 0x00;
  CtlBuf->CtrlID[13].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[13].MinDataValue = 0x00;
  CtlBuf->CtrlID[13].MaxDataValue = 0x02;
  CtlBuf->CtrlID[13].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[13].MaxDisplayValue = 0x02;
  //
  // TRTP
  //  
  CtlBuf->CtrlID[14].ControlId = 0x19;
  CtlBuf->CtrlID[14].NumberOfValues = 0x23;
  CtlBuf->CtrlID[14].Precision = 0x00;
  CtlBuf->CtrlID[14].Flags = 0x00;
  CtlBuf->CtrlID[14].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tRTP;
  CtlBuf->CtrlID[14].MinDataValue = 0x04;
  CtlBuf->CtrlID[14].MaxDataValue = 0x26;
  CtlBuf->CtrlID[14].MinDisplayValue = 0x04;
  CtlBuf->CtrlID[14].MaxDisplayValue = 0x26;
  //
  // TurboModeEnable
  //  
  CtlBuf->CtrlID[15].ControlId = 0x1A;
  CtlBuf->CtrlID[15].NumberOfValues = 0x02;
  CtlBuf->CtrlID[15].Precision = 0x00;
  CtlBuf->CtrlID[15].Flags = 0x00;
  CtlBuf->CtrlID[15].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[15].MinDataValue = 0x00;
  CtlBuf->CtrlID[15].MaxDataValue = 0x01;
  CtlBuf->CtrlID[15].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[15].MaxDisplayValue = 0x01;
  //
  // TurboOneCoreMaxTurboRatio
  //  
  CtlBuf->CtrlID[16].ControlId = 0x1D;
  CtlBuf->CtrlID[16].NumberOfValues = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore1 - XTUInfoHobData->CpuNonTurboRatio.MaxRatio + 1;
  CtlBuf->CtrlID[16].Precision = 0x00;
  CtlBuf->CtrlID[16].Flags = 0x00;
  CtlBuf->CtrlID[16].DefaultDataValue = XTUInfoHobData->TurboRatioLimitDefault.LimitCore1;
  CtlBuf->CtrlID[16].MinDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[16].MaxDataValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore1;
  CtlBuf->CtrlID[16].MinDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[16].MaxDisplayValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore1;
  //
  // TurboTwoCoreMaxTurboRatio
  //  
  CtlBuf->CtrlID[17].ControlId = 0x1E;
  CtlBuf->CtrlID[17].NumberOfValues = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore2 - XTUInfoHobData->CpuNonTurboRatio.MaxRatio + 1;
  CtlBuf->CtrlID[17].Precision = 0x00;
  CtlBuf->CtrlID[17].Flags = 0x00;
  CtlBuf->CtrlID[17].DefaultDataValue = XTUInfoHobData->TurboRatioLimitDefault.LimitCore2;
  CtlBuf->CtrlID[17].MinDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[17].MaxDataValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore2;
  CtlBuf->CtrlID[17].MinDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[17].MaxDisplayValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore2;
  //
  // TurboThreeCoreMaxTurboRatio
  //
  CtlBuf->CtrlID[18].ControlId = 0x1F;
  CtlBuf->CtrlID[18].NumberOfValues = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore3 - XTUInfoHobData->CpuNonTurboRatio.MaxRatio + 1;
  CtlBuf->CtrlID[18].Precision = 0x00;
  CtlBuf->CtrlID[18].Flags = 0x00;
  CtlBuf->CtrlID[18].DefaultDataValue = XTUInfoHobData->TurboRatioLimitDefault.LimitCore3;
  CtlBuf->CtrlID[18].MinDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[18].MaxDataValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore3;
  CtlBuf->CtrlID[18].MinDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[18].MaxDisplayValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore3;
  //
  // TurboFourCoreMaxTurboRatio
  //
  CtlBuf->CtrlID[19].ControlId = 0x20;
  CtlBuf->CtrlID[19].NumberOfValues = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore4 - XTUInfoHobData->CpuNonTurboRatio.MaxRatio + 1;
  CtlBuf->CtrlID[19].Precision = 0x00;
  CtlBuf->CtrlID[19].Flags = 0x00;
  CtlBuf->CtrlID[19].DefaultDataValue = XTUInfoHobData->TurboRatioLimitDefault.LimitCore4;
  CtlBuf->CtrlID[19].MinDataValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[19].MaxDataValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore4;
  CtlBuf->CtrlID[19].MinDisplayValue = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
  CtlBuf->CtrlID[19].MaxDisplayValue = XTUInfoHobData->TurboRatioLimitMaximum.LimitCore4;
  //
  // CPU Voltage Offset
  //
  CtlBuf->CtrlID[20].ControlId = 0x22;
  CtlBuf->CtrlID[20].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[20].Precision = 0x00;
  CtlBuf->CtrlID[20].Flags = 0x00;
  CtlBuf->CtrlID[20].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[20].MinDataValue = 0x00;
  CtlBuf->CtrlID[20].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[20].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[20].MaxDisplayValue = 0x3E8;
  //
  // TFAW
  //
  CtlBuf->CtrlID[21].ControlId = 0x28;
  CtlBuf->CtrlID[21].NumberOfValues = 0x24B;
  CtlBuf->CtrlID[21].Precision = 0x00;
  CtlBuf->CtrlID[21].Flags = 0x00;
  CtlBuf->CtrlID[21].DefaultDataValue = XTUInfoHobData->XmpProfile.Default.tFAW;
  CtlBuf->CtrlID[21].MinDataValue = 0x00;
  CtlBuf->CtrlID[21].MaxDataValue = 0x24A;
  CtlBuf->CtrlID[21].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[21].MaxDisplayValue = 0x24A;
  //
  // EistEnable
  //
  CtlBuf->CtrlID[22].ControlId = 0x29;
  CtlBuf->CtrlID[22].NumberOfValues = 0x02;
  CtlBuf->CtrlID[22].Precision = 0x00;
  CtlBuf->CtrlID[22].Flags = 0x00;
  CtlBuf->CtrlID[22].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[22].MinDataValue = 0x00;
  CtlBuf->CtrlID[22].MaxDataValue = 0x01;
  CtlBuf->CtrlID[22].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[22].MaxDisplayValue = 0x01;
  
  //
  // TurboPackageTdpShort
  //  
  CtlBuf->CtrlID[23].ControlId = 0x2F;
  CtlBuf->CtrlID[23].NumberOfValues = 0x1000;
  CtlBuf->CtrlID[23].Precision = 0x00;
  CtlBuf->CtrlID[23].Flags = 0x00;
  CtlBuf->CtrlID[23].DefaultDataValue = 0x44;
  CtlBuf->CtrlID[23].MinDataValue = 0x00;
  CtlBuf->CtrlID[23].MaxDataValue = 0xFFF;
  CtlBuf->CtrlID[23].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[23].MaxDisplayValue = 0xFFF;
  //
  // TurboPackageTdpExtended
  //  
  CtlBuf->CtrlID[24].ControlId = 0x30;
  CtlBuf->CtrlID[24].NumberOfValues = 0x1000;
  CtlBuf->CtrlID[24].Precision = 0x00;
  CtlBuf->CtrlID[24].Flags = 0x00;
  CtlBuf->CtrlID[24].DefaultDataValue = 0x37;
  CtlBuf->CtrlID[24].MinDataValue = 0x00;
  CtlBuf->CtrlID[24].MaxDataValue = 0xFFF;
  CtlBuf->CtrlID[24].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[24].MaxDisplayValue = 0xFFF;
  //
  // TurboPackageTdpShortEnable
  //  
  CtlBuf->CtrlID[25].ControlId = 0x31;
  CtlBuf->CtrlID[25].NumberOfValues = 0x02;
  CtlBuf->CtrlID[25].Precision = 0x00;
  CtlBuf->CtrlID[25].Flags = 0x00;
  CtlBuf->CtrlID[25].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[25].MinDataValue = 0x00;
  CtlBuf->CtrlID[25].MaxDataValue = 0x01;
  CtlBuf->CtrlID[25].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[25].MaxDisplayValue = 0x01;
  //
  // TurboPackageTdpLock
  //  
  CtlBuf->CtrlID[26].ControlId = 0x32;
  CtlBuf->CtrlID[26].NumberOfValues = 0x02;
  CtlBuf->CtrlID[26].Precision = 0x00;
  CtlBuf->CtrlID[26].Flags = 0x00;
  CtlBuf->CtrlID[26].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[26].MinDataValue = 0x00;
  CtlBuf->CtrlID[26].MaxDataValue = 0x01;
  CtlBuf->CtrlID[26].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[26].MaxDisplayValue = 0x01;
  //
  // TurbolaCoreCurrentMaximum
  //  
  CtlBuf->CtrlID[27].ControlId = 0x39;
  CtlBuf->CtrlID[27].NumberOfValues = 0x2000;
  CtlBuf->CtrlID[27].Precision = 0x03;
  CtlBuf->CtrlID[27].Flags = 0x00;
  CtlBuf->CtrlID[27].DefaultDataValue = 0x380;
  CtlBuf->CtrlID[27].MinDataValue = 0x00;
  CtlBuf->CtrlID[27].MaxDataValue = 0x1FFF;
  CtlBuf->CtrlID[27].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[27].MaxDisplayValue = 0xF9F83;
  //
  // TurboGfxCoreCurrentMaximum
  //
  CtlBuf->CtrlID[28].ControlId = 0x3A;
  CtlBuf->CtrlID[28].NumberOfValues = 0x2000;
  CtlBuf->CtrlID[28].Precision = 0x03;
  CtlBuf->CtrlID[28].Flags = 0x00;
  CtlBuf->CtrlID[28].DefaultDataValue = 0x170;
  CtlBuf->CtrlID[28].MinDataValue = 0x00;
  CtlBuf->CtrlID[28].MaxDataValue = 0x1FFF;
  CtlBuf->CtrlID[28].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[28].MaxDisplayValue = 0xF9F83;
  //
  // TurboGfxCoreRatioLimit
  //
  CtlBuf->CtrlID[29].ControlId = 0x3B;
  CtlBuf->CtrlID[29].NumberOfValues = 0x100;
  CtlBuf->CtrlID[29].Precision = 0x01;
  CtlBuf->CtrlID[29].Flags = 0x00;
  CtlBuf->CtrlID[29].DefaultDataValue = 0x15;
  CtlBuf->CtrlID[29].MinDataValue = 0x00;
  CtlBuf->CtrlID[29].MaxDataValue = 0xFF;
  CtlBuf->CtrlID[29].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[29].MaxDisplayValue = 0x4FB;
  //
  // XMPProfileSelection
  //
  CtlBuf->CtrlID[30].ControlId = 0x40;
  CtlBuf->CtrlID[30].NumberOfValues = 0x04;
  CtlBuf->CtrlID[30].Precision = 0x00;
  CtlBuf->CtrlID[30].Flags = 0x00;
  CtlBuf->CtrlID[30].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[30].MinDataValue = 0x00;
  CtlBuf->CtrlID[30].MaxDataValue = 0x03;
  CtlBuf->CtrlID[30].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[30].MaxDisplayValue = 0x03;
  //
  // Memory Clock Multiplier
  //
  CtlBuf->CtrlID[31].ControlId = 0x49;
  CtlBuf->CtrlID[31].NumberOfValues = 0x02;
  CtlBuf->CtrlID[31].Precision = 0x02;
  CtlBuf->CtrlID[31].Flags = 0x00;
  CtlBuf->CtrlID[31].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[31].MinDataValue = 0x00;
  CtlBuf->CtrlID[31].MaxDataValue = 0x01;
  CtlBuf->CtrlID[31].MinDisplayValue = 0x64;
  CtlBuf->CtrlID[31].MaxDisplayValue = 0x85;
  //
  // Dynamic SVID Control
  //
  CtlBuf->CtrlID[32].ControlId = 0x4B;
  CtlBuf->CtrlID[32].NumberOfValues = 0x02;
  CtlBuf->CtrlID[32].Precision = 0x00;
  CtlBuf->CtrlID[32].Flags = 0x00;
  CtlBuf->CtrlID[32].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[32].MinDataValue = 0x00;
  CtlBuf->CtrlID[32].MaxDataValue = 0x01;
  CtlBuf->CtrlID[32].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[32].MaxDisplayValue = 0x01;
  //
  // Ring Ratio
  //
  CtlBuf->CtrlID[33].ControlId = 0x4C;
  CtlBuf->CtrlID[33].NumberOfValues = 0x51;
  CtlBuf->CtrlID[33].Precision = 0x00;
  CtlBuf->CtrlID[33].Flags = 0x00;
  CtlBuf->CtrlID[33].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[33].MinDataValue = 0x00;
  CtlBuf->CtrlID[33].MaxDataValue = 0x50;
  CtlBuf->CtrlID[33].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[33].MaxDisplayValue = 0x50;
  //
  // Ring Voltage Override
  //
  CtlBuf->CtrlID[34].ControlId = 0x4D;
  CtlBuf->CtrlID[34].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[34].Precision = 0x03;
  CtlBuf->CtrlID[34].Flags = 0x00;
  CtlBuf->CtrlID[34].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[34].MinDataValue = 0x00;
  CtlBuf->CtrlID[34].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[34].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[34].MaxDisplayValue = 0x7D0;
  //
  // Ring Voltage Mode
  //
  CtlBuf->CtrlID[35].ControlId = 0x4E;
  CtlBuf->CtrlID[35].NumberOfValues = 0x02;
  CtlBuf->CtrlID[35].Precision = 0x00;
  CtlBuf->CtrlID[35].Flags = 0x00;
  CtlBuf->CtrlID[35].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[35].MinDataValue = 0x00;
  CtlBuf->CtrlID[35].MaxDataValue = 0x01;
  CtlBuf->CtrlID[35].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[35].MaxDisplayValue = 0x01;
  //
  // Ring Voltage Offset
  //
  CtlBuf->CtrlID[36].ControlId = 0x4F;
  CtlBuf->CtrlID[36].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[36].Precision = 0x00;
  CtlBuf->CtrlID[36].Flags = 0x00;
  CtlBuf->CtrlID[36].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[36].MinDataValue = 0x00;
  CtlBuf->CtrlID[36].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[36].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[36].MaxDisplayValue = 0x3E8;
  //
  // Overclocking Enable
  //
  CtlBuf->CtrlID[37].ControlId = 0x50;
  CtlBuf->CtrlID[37].NumberOfValues = 0x02;
  CtlBuf->CtrlID[37].Precision = 0x00;
  CtlBuf->CtrlID[37].Flags = 0x00;
  CtlBuf->CtrlID[37].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[37].MinDataValue = 0x00;
  CtlBuf->CtrlID[37].MaxDataValue = 0x01;
  CtlBuf->CtrlID[37].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[37].MaxDisplayValue = 0x01;
  //
  // Graphics Core Voltage Override
  //
  CtlBuf->CtrlID[38].ControlId = 0x51;
  CtlBuf->CtrlID[38].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[38].Precision = 0x03;
  CtlBuf->CtrlID[38].Flags = 0x00;
  CtlBuf->CtrlID[38].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[38].MinDataValue = 0x00;
  CtlBuf->CtrlID[38].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[38].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[38].MaxDisplayValue = 0x7D0;
  //
  // Graphics Core Voltage Mode
  //
  CtlBuf->CtrlID[39].ControlId = 0x52;
  CtlBuf->CtrlID[39].NumberOfValues = 0x02;
  CtlBuf->CtrlID[39].Precision = 0x00;
  CtlBuf->CtrlID[39].Flags = 0x00;
  CtlBuf->CtrlID[39].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[39].MinDataValue = 0x00;
  CtlBuf->CtrlID[39].MaxDataValue = 0x01;
  CtlBuf->CtrlID[39].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[39].MaxDisplayValue = 0x01;
  //
  // Graphics Core Voltage Offset
  //
  CtlBuf->CtrlID[40].ControlId = 0x53;
  CtlBuf->CtrlID[40].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[40].Precision = 0x00;
  CtlBuf->CtrlID[40].Flags = 0x00;
  CtlBuf->CtrlID[40].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[40].MinDataValue = 0x00;
  CtlBuf->CtrlID[40].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[40].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[40].MaxDisplayValue = 0x3E8;
  //
  // System Agent Voltage Offset
  //
  CtlBuf->CtrlID[41].ControlId = 0x55;
  CtlBuf->CtrlID[41].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[41].Precision = 0x00;
  CtlBuf->CtrlID[41].Flags = 0x00;
  CtlBuf->CtrlID[41].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[41].MinDataValue = 0x00;
  CtlBuf->CtrlID[41].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[41].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[41].MaxDisplayValue = 0x3E8;
  //
  // FIVR Faults
  //
  CtlBuf->CtrlID[42].ControlId = 0x56;
  CtlBuf->CtrlID[42].NumberOfValues = 0x02;
  CtlBuf->CtrlID[42].Precision = 0x00;
  CtlBuf->CtrlID[42].Flags = 0x00;
  CtlBuf->CtrlID[42].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[42].MinDataValue = 0x00;
  CtlBuf->CtrlID[42].MaxDataValue = 0x01;
  CtlBuf->CtrlID[42].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[42].MaxDisplayValue = 0x01;
  //
  // FIVR Efficiency Management
  //
  CtlBuf->CtrlID[43].ControlId = 0x57;
  CtlBuf->CtrlID[43].NumberOfValues = 0x02;
  CtlBuf->CtrlID[43].Precision = 0x00;
  CtlBuf->CtrlID[43].Flags = 0x00;
  CtlBuf->CtrlID[43].DefaultDataValue = 0x01;
  CtlBuf->CtrlID[43].MinDataValue = 0x00;
  CtlBuf->CtrlID[43].MaxDataValue = 0x01;
  CtlBuf->CtrlID[43].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[43].MaxDisplayValue = 0x01;
  //
  // IA Core Voltage Mode
  //
  CtlBuf->CtrlID[44].ControlId = 0x58;
  CtlBuf->CtrlID[44].NumberOfValues = 0x02;
  CtlBuf->CtrlID[44].Precision = 0x00;
  CtlBuf->CtrlID[44].Flags = 0x00;
  CtlBuf->CtrlID[44].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[44].MinDataValue = 0x00;
  CtlBuf->CtrlID[44].MaxDataValue = 0x01;
  CtlBuf->CtrlID[44].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[44].MaxDisplayValue = 0x01;
  //
  // SVID Voltage Override
  //
  CtlBuf->CtrlID[45].ControlId = 0x59;
  CtlBuf->CtrlID[45].NumberOfValues = 0x9C5;
  CtlBuf->CtrlID[45].Precision = 0x00;
  CtlBuf->CtrlID[45].Flags = 0x00;
  CtlBuf->CtrlID[45].DefaultDataValue = 0x00;
  CtlBuf->CtrlID[45].MinDataValue = 0x00;
  CtlBuf->CtrlID[45].MaxDataValue = 0x9C4;
  CtlBuf->CtrlID[45].MinDisplayValue = 0x00;
  CtlBuf->CtrlID[45].MaxDisplayValue = 0x9C4;
  //
  // I/O Analog Voltage Offset
  //
  CtlBuf->CtrlID[46].ControlId = 0x5B;
  CtlBuf->CtrlID[46].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[46].Precision = 0x00;
  CtlBuf->CtrlID[46].Flags = 0x00;
  CtlBuf->CtrlID[46].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[46].MinDataValue = 0x00;
  CtlBuf->CtrlID[46].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[46].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[46].MaxDisplayValue = 0x3E8;
  //
  // I/O Digital Voltage Offset
  //
  CtlBuf->CtrlID[47].ControlId = 0x5C;
  CtlBuf->CtrlID[47].NumberOfValues = 0x7D1;
  CtlBuf->CtrlID[47].Precision = 0x00;
  CtlBuf->CtrlID[47].Flags = 0x00;
  CtlBuf->CtrlID[47].DefaultDataValue = 0x3E8;
  CtlBuf->CtrlID[47].MinDataValue = 0x00;
  CtlBuf->CtrlID[47].MaxDataValue = 0x7D0;
  CtlBuf->CtrlID[47].MinDisplayValue = 0xFFFFFC18;
  CtlBuf->CtrlID[47].MaxDisplayValue = 0x3E8;
//[-end-121130-IB03780468-modify]//

//[-start-120521-IB03780442-modify]//
//[-start-120210-IB03780422-modify]//
  //
  // XMP Profile 1 tCL
  //
  XmpDataBuffer[0].ControlID     = 0x07;
  XmpDataBuffer[0].Reserved      = 0;
  XmpDataBuffer[0].Precision     = 0;
  XmpDataBuffer[0].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tCL;

  //
  // XMP Profile 1 tRCD
  //
  XmpDataBuffer[1].ControlID     = 0x08;
  XmpDataBuffer[1].Reserved      = 0;
  XmpDataBuffer[1].Precision     = 0;
  XmpDataBuffer[1].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRCD;

  //
  // XMP Profile 1 tRP
  //
  XmpDataBuffer[2].ControlID     = 0x09;
  XmpDataBuffer[2].Reserved      = 0;
  XmpDataBuffer[2].Precision     = 0;
  XmpDataBuffer[2].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRP;

  //
  // XMP Profile 1 tRAS
  //
  XmpDataBuffer[3].ControlID     = 0x0A;
  XmpDataBuffer[3].Reserved      = 0;
  XmpDataBuffer[3].Precision     = 0;
  XmpDataBuffer[3].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRAS;

  //
  // XMP Profile 1 tWR
  //
  XmpDataBuffer[4].ControlID     = 0x0B;
  XmpDataBuffer[4].Reserved      = 0;
  XmpDataBuffer[4].Precision     = 0;
  XmpDataBuffer[4].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tWR;

  //
  // XMP Profile 1 tRFC
  //
  XmpDataBuffer[5].ControlID     = 0x15;
  XmpDataBuffer[5].Reserved      = 0;
  XmpDataBuffer[5].Precision     = 0;
  XmpDataBuffer[5].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRFC;

  //
  // XMP Profile 1 tRRD
  //
  XmpDataBuffer[6].ControlID     = 0x16;
  XmpDataBuffer[6].Reserved      = 0;
  XmpDataBuffer[6].Precision     = 0;
  XmpDataBuffer[6].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRRD;

  //
  // XMP Profile 1 tWTR
  //
  XmpDataBuffer[7].ControlID     = 0x17;
  XmpDataBuffer[7].Reserved      = 0;
  XmpDataBuffer[7].Precision     = 0;
  XmpDataBuffer[7].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tWTR;

  //
  // XMP Profile 1 tRTP
  //
  XmpDataBuffer[8].ControlID     = 0x19;
  XmpDataBuffer[8].Reserved      = 0;
  XmpDataBuffer[8].Precision     = 0;
  XmpDataBuffer[8].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tRTP;

  //
  // XMP Profile 1 tFAW
  //
  XmpDataBuffer[9].ControlID     = 0x28;
  XmpDataBuffer[9].Reserved      = 0;
  XmpDataBuffer[9].Precision     = 0;
  XmpDataBuffer[9].DisplayValue  = XTUInfoHobData->XmpProfile.Profile1.tFAW;   

  //
  // XMP Profile 1 Frequency
  //
  XmpDataBuffer[10].ControlID    = 0x13;
  XmpDataBuffer[10].Reserved     = 0;
//[-start-120521-IB03780442-modify]//
  XmpDataBuffer[10].Precision    = 0;
  XmpDataBuffer[10].DisplayValue = XTUInfoHobData->XmpProfile.Profile1.DdrMultiplier / MEMORY_CLOCK_MULTIPLIER;
//[-end-120521-IB03780442-modify]//

//[-start-120521-IB03780442-add]//
  //
  // XMP Profile 1 Memory Clock Multiplier
  //
  XmpDataBuffer[11].ControlID    = 0x49;
  XmpDataBuffer[11].Reserved     = 0;
  XmpDataBuffer[11].Precision    = 2;
  XmpDataBuffer[11].DisplayValue = MEMORY_CLOCK_MULTIPLIER;
//[-end-120521-IB03780442-add]//

  //
  // XMP Profile 2 tCL
  //
  XmpDataBuffer[12].ControlID    = 0x07;
  XmpDataBuffer[12].Reserved     = 0;
  XmpDataBuffer[12].Precision    = 0;
  XmpDataBuffer[12].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tCL;

  //
  // XMP Profile 2 tRCD
  //
  XmpDataBuffer[13].ControlID    = 0x08;
  XmpDataBuffer[13].Reserved     = 0;
  XmpDataBuffer[13].Precision    = 0;
  XmpDataBuffer[13].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRCD;

  //
  // XMP Profile 2 tRP
  //
  XmpDataBuffer[14].ControlID    = 0x09;
  XmpDataBuffer[14].Reserved     = 0;
  XmpDataBuffer[14].Precision    = 0;
  XmpDataBuffer[14].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRP;

  //
  // XMP Profile 2 tRAS
  //
  XmpDataBuffer[15].ControlID    = 0x0A;
  XmpDataBuffer[15].Reserved     = 0;
  XmpDataBuffer[15].Precision    = 0;
  XmpDataBuffer[15].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRAS;

  //
  // XMP Profile 2 tWR
  //
  XmpDataBuffer[16].ControlID    = 0x0B;
  XmpDataBuffer[16].Reserved     = 0;
  XmpDataBuffer[16].Precision    = 0;
  XmpDataBuffer[16].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tWR;

  //
  // XMP Profile 2 tRFC
  //
  XmpDataBuffer[17].ControlID    = 0x15;
  XmpDataBuffer[17].Reserved     = 0;
  XmpDataBuffer[17].Precision    = 0;
  XmpDataBuffer[17].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRFC;

  //
  // XMP Profile 2 tRRD
  //
  XmpDataBuffer[18].ControlID    = 0x16;
  XmpDataBuffer[18].Reserved     = 0;
  XmpDataBuffer[18].Precision    = 0;
  XmpDataBuffer[18].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRRD;

  //
  // XMP Profile 2 tWTR
  //
  XmpDataBuffer[19].ControlID    = 0x17;
  XmpDataBuffer[19].Reserved     = 0;
  XmpDataBuffer[19].Precision    = 0;
  XmpDataBuffer[19].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tWTR;

  //
  // XMP Profile 2 tRTP
  //
  XmpDataBuffer[20].ControlID    = 0x19;
  XmpDataBuffer[20].Reserved     = 0;
  XmpDataBuffer[20].Precision    = 0;
  XmpDataBuffer[20].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tRTP;

  //
  // XMP Profile 2 tFAW
  //
  XmpDataBuffer[21].ControlID    = 0x28;
  XmpDataBuffer[21].Reserved     = 0;
  XmpDataBuffer[21].Precision    = 0;
  XmpDataBuffer[21].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.tFAW;

  //
  // XMP Profile 2 Frequency
  //
  XmpDataBuffer[22].ControlID    = 0x13;
  XmpDataBuffer[22].Reserved     = 0;
//[-start-120521-IB03780442-modify]//
  XmpDataBuffer[22].Precision    = 0;
  XmpDataBuffer[22].DisplayValue = XTUInfoHobData->XmpProfile.Profile2.DdrMultiplier / MEMORY_CLOCK_MULTIPLIER;
//[-end-120521-IB03780442-modify]//
//[-end-120210-IB03780422-modify]//
//[-end-120521-IB03780442-modify]//
//[-start-120521-IB03780442-add]//
  //
  // XMP Profile 2 Memory Clock Multiplier
  //
  XmpDataBuffer[23].ControlID    = 0x49;
  XmpDataBuffer[23].Reserved     = 0;
  XmpDataBuffer[23].Precision    = 2;
  XmpDataBuffer[23].DisplayValue = MEMORY_CLOCK_MULTIPLIER;
//[-end-120521-IB03780442-add]//

  GlobalNvsArea->Area->XTUBaseAddress = *(UINT32 *)&CtlBuf;
//[-start-121130-IB03780468-modify]//
  GlobalNvsArea->Area->XTUSize        = sizeof(CONTROLID_BUFF);
//[-end-121130-IB03780468-modify]//
  GlobalNvsArea->Area->XMPBaseAddress = *(UINT32 *) &XmpDataBuffer;

  return EFI_SUCCESS;
}
