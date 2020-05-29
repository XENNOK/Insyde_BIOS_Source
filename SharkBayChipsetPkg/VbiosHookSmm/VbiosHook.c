/** @file
  This driver is for providing the Int15 callback service routines. It is a
  SMM driver. Operations (HOOKS) are assigned (divided) by different Function
  Numbers. Meanings and definitions of those Function Numbers can be found in
  the VBIOS Spec. Purpose of this driver is trying to provide Programmers the
  template to conduct the INT15 Hook services as they wish.

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

//[-start-120712-IB10820084-modify]//
#include <Uefi.h>
#include <SaAccess.h>
#include <ChipsetSetupConfig.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
//#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SetupUtility.h>
#include <SmmInt15Service.h>
//[-start-120531-IB09420077-modify]//
#include <Protocol/LegacyBios.h>
//[-end-120531-IB09420077-modify]//
//[-start-121113-IB10820163-add]//
#include <Library/SmmOemSvcChipsetLib.h>
#include <Library/DebugLib.h>
//[-end-121113-IB10820163-add]//
//[-start-130110-IB11410040-add]//
//[-start-130812-IB06720232-modify]//
//#include <Library/EcLib.h>
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-modify]//
//[-end-130110-IB11410040-add]//

//
// INT15 HOOK NUMBER
//
#define INT15_GET_UPDATE_SYSTEM_BIOS_STATE                           0x5F14
#define INT15_SET_PANEL_FITTING_HOOK                                 0x5F34
#define INT15_BOOT_DISPLAY_DEVICE_HOOK                               0x5F35
#define INT15_BOOT_PANEL_TYPE_HOOK                                   0x5F40
#define INT15_HOOK_TO_GET_INVERTER_TYPE_AND_POLARITY_FOR_BACKLIGHT   0x5F49
#define INT15_HOOK_TO_ENABLE_SQUELCH_REGISTER_FOR_EDP                0x5F50
#define INT15_HOOK_TO_SELECT_ACTIVE_LFP_CONFIGURATION                0x5F51
#define INT15_HOOK_TO_GET_PANEL_COLOR_DEPTH_FORM_SETUP               0x5F52

#define GET_MISCELLANEOUS_STATUS_HOOK               0x078F

#define NO_LVDS                                     0x00
#define INT_LVDS                                    0x01
#define SDVO_LVDS                                   0x02
#define EDP                                         0x03
#define NO_HOOK                                     0xFF

//
// INT15 Service Function HOOK 
// This array stores all the requested INT15 Hook Function Numbers that will
// be installed (registered) later. Detain meanings of such Function Numbers
// can be found in VBIOS Spec. Ex: 0x5F14 is "Get/Update System BIOS State"
//
#define INT15_VBIOS_FUNCTION_HOOK_LIST \
  INT15_GET_UPDATE_SYSTEM_BIOS_STATE, \
  INT15_SET_PANEL_FITTING_HOOK, \
  INT15_BOOT_DISPLAY_DEVICE_HOOK, \
  INT15_BOOT_PANEL_TYPE_HOOK, \
  INT15_HOOK_TO_GET_INVERTER_TYPE_AND_POLARITY_FOR_BACKLIGHT, \
  INT15_HOOK_TO_ENABLE_SQUELCH_REGISTER_FOR_EDP, \
  INT15_HOOK_TO_SELECT_ACTIVE_LFP_CONFIGURATION, \
  INT15_HOOK_TO_GET_PANEL_COLOR_DEPTH_FORM_SETUP

VOID
VbiosHookCallBack (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs, 
  IN VOID                               *Context
  );
//[-start-120712-IB10820084-modify]//


EFI_SMM_SYSTEM_TABLE2                   *mSmst;
CHIPSET_CONFIGURATION                    *mSetupVariable;

STATIC UINT16 mInt15VbiosFunctionHook[] = {INT15_VBIOS_FUNCTION_HOOK_LIST};

/**
  Initializes the SMM Dispatcher for VBIOS INT15 HOOK Services.

  @param  ImageHandle       Pointer to the loaded image protocol for this driver.
  @param  SystemTable       Pointer to the EFI System Table.

  @return Status            EFI_SUCCESS
  
**/
EFI_STATUS
EFIAPI
VbiosHookEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  BOOLEAN                               InSmm;
  EFI_SMM_INT15_SERVICE_PROTOCOL        *SmmInt15Service = NULL;
  EFI_SETUP_UTILITY_PROTOCOL            *EfiSetupUtility;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase2;
//[-start-121113-IB10820163-add]//
  UINT16                                *OemInt15VbiosFunctionHook;
  UINT16                                Size;

  Size=0;
//[-end-121113-IB10820163-add]//
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &SmmBase2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmBase2->InSmm (SmmBase2, &InSmm);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!InSmm) {
    return EFI_SUCCESS;
  }

  //
  // Great!  We're now in SMM!
  //
  // Initialize global variables
  //
  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);
  if (EFI_ERROR(Status)) { 
    return Status;
  }

  mSetupVariable = (CHIPSET_CONFIGURATION *)EfiSetupUtility->SetupNvData;

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmInt15ServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmInt15Service
                    );
  if (EFI_ERROR(Status)) { 
    return Status;
  }


  for (Index = 0; Index < sizeof (mInt15VbiosFunctionHook) / sizeof (UINT16); Index++) {
    Status = SmmInt15Service->InstallInt15ProtocolInterface (
                                SmmInt15Service,
                                mInt15VbiosFunctionHook[Index],
                                VbiosHookCallBack,
                                NULL
                                );
    if (Status == EFI_ALREADY_STARTED) { 
      //
      // use new callback function to replace original one
      //
      Status = SmmInt15Service->ReinstallInt15ProtocolInterface (
                                  SmmInt15Service,
                                  mInt15VbiosFunctionHook[Index],
                                  VbiosHookCallBack,
                                  NULL
                                  );
      return Status;
    }    
  }
//[-start-121113-IB10820163-add]//
  Status = GetOemInt15VbiosFunctionlist (&OemInt15VbiosFunctionHook, &Size);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib GetOemInt15VbiosFunctionlist, Status : %r\n", Status));
  if (Status == EFI_MEDIA_CHANGED){
    for (Index = 0; Index < Size; Index++) {
      Status = SmmInt15Service->InstallInt15ProtocolInterface (
                                  SmmInt15Service,
                                  OemInt15VbiosFunctionHook[Index],
                                  VbiosHookCallBack,
                                  NULL
                                  );
      if (Status == EFI_ALREADY_STARTED) {
        //
        // use new callback function to replace original one
        //
        Status = SmmInt15Service->ReinstallInt15ProtocolInterface (
                                    SmmInt15Service,
                                    OemInt15VbiosFunctionHook[Index],
                                    VbiosHookCallBack,
                                    NULL
                                    );
        return Status;
      }
    }
  }
//[-end-121113-IB10820163-add]//
  return EFI_SUCCESS;
}

/**
  INT15 Callback Routine. It contains several INT15 Services corresponding
  to specific Function Number such as 0x5F49 for Backlight Brightness. 

  @param  CpuRegs       The structure containing CPU Registers (AX, BX, CX, DX etc.).
  @param  Context       Context.

  @return None

  Components of CpuRegs
  CX - For most Settings (Configurations) described in the VBIOS Spec
  BX - In most casess, it is a calling register. It also can be used 
       as a return (for Settings/Configurations) register like CX
  AX - Indicate the Status of Function Supported Fail/Success
  
**/
VOID
VbiosHookCallBack (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs, 
  IN VOID                               *Context
  )
{
//[-start-130809-IB06720232-modify]//
//[-start-130110-IB11410040-remove]//
//[-start-121113-IB10820163-add]//
//  EFI_STATUS                            Status;
//[-end-121113-IB10820163-add]//
//[-end-130110-IB11410040-remove]//
  UINT32                                Int15FunNum;
  UINT32                                EdpRegister;
//[-start-121120-IB08050186-add]//
  BOOLEAN                               LidIsOpen;
  EFI_STATUS                            Status;
//[-end-121120-IB08050186-add]//
  EFI_STATUS                            EcGetLidState;

//[-start-121120-IB08050186-add]//
  LidIsOpen = TRUE;
  Status   = EFI_SUCCESS;
//[-end-121120-IB08050186-add]//
  EcGetLidState = EFI_SUCCESS;
//[-end-130809-IB06720232-modify]//

  //
  // GET THE INT15 FUNCTION NUMBER
  //
  Int15FunNum = (CpuRegs->X.AX & 0xFFFF);
  if (!Int15FunNum) {
    return;
  }

  //
  // SWITCH CASE Used For Applying Different HOOK
  // corresponding to the Function Number
  //
//[-start-121113-IB10820163-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcVbiosHookCallBack(Int15FunNum, CpuRegs, Context);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcVbiosHookCallBack, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return;
  }
//[-end-121113-IB10820163-add]//
  switch (Int15FunNum) {
    case INT15_GET_UPDATE_SYSTEM_BIOS_STATE :
      //
      // VBIOS : Get Miscellaneous Status Hook
      //
      //  Calling Registers: 
      //
      //    AX = 5F14h, System BIOS State Hook 
      //    BX = 078Fh, Get Miscellaneous Status
      //
      //  Return Registers: 
      //
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //       = 015Fh, Function supported but failed 
      //       = 005Fh, Function supported and successful 
      //    CL = Bit map of dock, lid and AC status: 
      //         Bits 7 - 3 = Reserved 
      //         Bit  2     = 0, no AC power 
      //                    = 1, AC power active 
      //         Bit  1     = 0, lid open 
      //                    = 1, lid closed 
      //         Bit  0     = 0, not docked 
      //                    = 1, docked 
      //
      if (CpuRegs->X.BX == GET_MISCELLANEOUS_STATUS_HOOK) {
        CpuRegs->H.CL &= ~(0xff);
//[-start-121120-IB08050186-modify]//
//[-start-130809-IB06720232-modify]//
        OemSvcEcGetLidState (&EcGetLidState, &LidIsOpen);
        ASSERT (!EFI_ERROR (EcGetLidState));
        
        CpuRegs->H.CL |= 0x04;
        if (!EFI_ERROR (EcGetLidState)) {
          if(!LidIsOpen) {
            //
            // If get lid state form EC successfully and lid is closed.
            //
            CpuRegs->H.CL |= BIT1;
          }
        }
//[-end-130809-IB06720232-modify]//
//[-end-121120-IB08050186-modify]//
        CpuRegs->X.AX &= ~(0xffff);
        CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      }
      break;

    case INT15_SET_PANEL_FITTING_HOOK :
      //
      // VBIOS : Set Panel Fitting Hook
      //
      //  Return Registers:
      //
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //       = 005Fh, Function supported and successful 
      //       = 015Fh, Function supported but failed 
      //    CL = Panel fitting flags (1 = Enable, 0 = Disable) 
      //       = 00h, use video BIOS default 
      //         Bits 7 - 3 = Reserved 
      //         Bit 2      = Graphics Stretching 
      //         Bit 1      = Text Stretching 
      //         Bit 0      = Centering (Can not be set when Bit1 or Bit2 is set) 
      //                  
      CpuRegs->H.CL &= ~(0xff);
      CpuRegs->H.CL |= 0x0;     
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      break;

    case INT15_BOOT_DISPLAY_DEVICE_HOOK :
      //
      // VBIOS : Boot Display Device Hook
      //
      //  Return Registers:
      //
      //     AX  = Return Status (function not supported if AL != 5Fh); 
      //         = 005Fh, Function supported and successful  
      //         = 015Fh, Function supported but failed 
      //     CX  = Display Device Combination to boot (1 = Enable, 0 = Disable): 
      //         = 00h, video BIOS Default 
      //           Bit 15 = Pipe B - LFP2  
      //           Bit 14 = Pipe B - EFP2  
      //           Bit 13 = Pipe B - EFP3  
      //           Bit 12 = Pipe B - CRT2  
      //           Bit 11 = Pipe B - LFP  
      //           Bit 10 = Pipe B - EFP  
      //           Bit 9  = Pipe B - TV  
      //           Bit 8  = Pipe B - CRT 
      //           Bit 7  = Pipe A - LFP2  
      //           Bit 6  = Pipe A - EFP2 
      //           Bit 5  = Pipe A - EFP3 
      //           Bit 4  = Pipe A - CRT2 
      //           Bit 3  = Pipe A - LFP  
      //           Bit 2  = Pipe A - EFP 
      //           Bit 1  = Pipe A - TV 
      //           Bit 0  = Pipe A - CRT 
      //
      CpuRegs->X.CX &= ~(0xffff);
      CpuRegs->X.CX |= (UINT16)mSetupVariable->IGDBootType;
      if ((mSetupVariable->IGDBootType != 0) &&
          (mSetupVariable->IGDBootType & mSetupVariable->IGDBootTypeSecondary) == 0) {
        CpuRegs->X.CX |= (UINT16)mSetupVariable->IGDBootTypeSecondary << 8;
      }
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      break;

    case INT15_BOOT_PANEL_TYPE_HOOK :
      //
      // VBIOS : Boot Panel Type Hook
      //
      //  Calling Registers:
      //
      //    AX = 5F40h, Boot Panel Type Hook 
      //    BL = Panel index  
      //       = 00h, LFP 
      //       = 01h, LFP2 
      //
      //  Return Registers:
      //
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //       = 005Fh, Function supported and successful 
      //       = 015Fh, Function supported but failed 
      //    CL = 1 - 16, Panel type (define in VBIOS Panel_#1 ~ Panel_#16)
      //             
      CpuRegs->X.CX &= ~(0xffff);
      CpuRegs->X.CX |= mSetupVariable->PanelType;
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported but failed
      break;

    case INT15_HOOK_TO_GET_INVERTER_TYPE_AND_POLARITY_FOR_BACKLIGHT :
      //
      //  VBIOS : Hook to get Inverter Type and Polarity for Backlight
      //
      //  Return Registers: 
      //
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //       = 005Fh, Function supported and successful 
      //       = 015Fh, Function supported but failed 
      //    CX = Return Parameter 
      //       = 00h, Enable PWM -- Inverted 
      //       = 01h, Enable I2C -- Inverted 
      //       = 02h, Enable PWM -- Normal 
      //       = 03h, Enable I2C -- Normal 
      //
      CpuRegs->X.CX &= ~(0xffff);
      CpuRegs->X.CX |= mSetupVariable->BacklightControl;
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      break;


    case INT15_HOOK_TO_ENABLE_SQUELCH_REGISTER_FOR_EDP :
      // 
      // Hook to Enable Squelch Register for eDP
      // 
      // Calling Registers:
      // 
      //   AX = Hook to enable Squelch Register for eDP
      //
      // Return Registers:
      //
      //   AX = Return Status (function not supported if AL != 5Fh):
      //      = 005Fh, Function supported and successful
      //      = 015Fh, Function supported but failed
      //
      // - In the 5F50h INT15h hook the SBIOS should program the registers as below
      // - PCIEXBAR + 0x8dfc[1] = 1b
      //
      EdpRegister = McD1PciCfg32 (0xDFC);
      EdpRegister |= BIT1;
      McD1PciCfg32 (0xDFC) = EdpRegister;
      //
      // - PCIEXBAR + 0x8f88[31] = 1b
      // - PCIEXBAR + 0x8f88[26] = 0b
      //
      EdpRegister = McD1PciCfg32 (0xF88);
      EdpRegister |= BIT31;
      EdpRegister &= ~BIT26;
      McD1PciCfg32 (0xF88) = EdpRegister;
      //
      // - PCIEXBAR + 0x8fa8[31] = 1b
      // - PCIEXBAR + 0x8fa8[26] = 0b
      //
      EdpRegister = McD1PciCfg32 (0xFA8);
      EdpRegister |= BIT31;
      EdpRegister &= ~BIT26;
      McD1PciCfg32 (0xFA8) = EdpRegister;
      //
      // - PCIEXBAR + 0x8fc8[31] = 1b
      // - PCIEXBAR + 0x8fc8[26] = 0b
      //
      EdpRegister = McD1PciCfg32 (0xFC8);
      EdpRegister |= BIT31;
      EdpRegister &= ~BIT26;
      McD1PciCfg32 (0xFC8) = EdpRegister;
      //
      // - PCIEXBAR + 0x8fe8[31] = 1b
      // - PCIEXBAR + 0x8fe8[26] = 0b
      //
      EdpRegister = McD1PciCfg32 (0xFE8);
      EdpRegister |= BIT31;
      EdpRegister &= ~BIT26;
      McD1PciCfg32 (0xFE8) = EdpRegister;
      //
      // Return Success
      //
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F;
      break;

    case INT15_HOOK_TO_SELECT_ACTIVE_LFP_CONFIGURATION :
      //
      // VBIOS : Hook to select Active LFP configuration
      //
      //  Return Registers: 
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //       = 005Fh, Function supported and successful 
      //       = 015Fh, Function supported but failed 
      //    CX = Active LFP configuration selected in BIOS setup. 
      //       = 00h, No LVDS, VBIOS does not enable LVDS. 
      //       = 01h, Int-LVDS, LFP driven by Integrated LVDS encoder. 
      //       = 02h, SDVO-LVDS, LFP driven by SDVO encoder. 
      //       = 03h, eDP, LFP Driven by Int-DisplayPort encoder. 
      //
      switch (mSetupVariable->LFPConfiguration) {
        case NO_LVDS :
          CpuRegs->X.CX &= ~(0xffff);
          CpuRegs->X.CX |= 0x0000;
          CpuRegs->X.AX &= ~(0xffff);
          CpuRegs->X.AX |= 0x005F;
          break;
        case INT_LVDS :
          CpuRegs->X.CX &= ~(0xffff);
          CpuRegs->X.CX |= 0x0001;
          CpuRegs->X.AX &= ~(0xffff);
          CpuRegs->X.AX |= 0x005F;
          break;
        case SDVO_LVDS :
          CpuRegs->X.CX &= ~(0xffff);
          CpuRegs->X.CX |= 0x0002;
          CpuRegs->X.AX &= ~(0xffff);
          CpuRegs->X.AX |= 0x005F;
          break;
        case EDP :
          CpuRegs->X.CX &= ~(0xffff);
          CpuRegs->X.CX |= 0x0003;
          CpuRegs->X.AX &= ~(0xffff);
          CpuRegs->X.AX |= 0x005F;
          break;
        case NO_HOOK :
          CpuRegs->X.AX &= ~(0xffff);
          CpuRegs->X.AX |= 0x015F;
          break;
      }
      break;

    case INT15_HOOK_TO_GET_PANEL_COLOR_DEPTH_FORM_SETUP :
      //
      // VBIOS : Hook to get Panel Color Depth from Setup
      //
      //  Return Registers: 
      //    AX = Return Status (function not supported if AL != 5Fh): 
      //      = 005Fh, Function supported and successful 
      //      = 015Fh, Function supported but failed 
      //    CX = Return Parameter 
      //      = 00h, 18 bit
      //      = 01h, 24 bit
      //
      CpuRegs->X.CX &= ~(0xffff);
      CpuRegs->X.CX |= mSetupVariable->PanelColorDepth;
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      break;

  default:
    break;
  }
}
