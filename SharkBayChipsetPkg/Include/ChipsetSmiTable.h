/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SMI_TABLE_DEFINE_H_
#define _CHIPSET_SMI_TABLE_DEFINE_H_

typedef enum { 
  INT15_HOOK             = 0x15,
  //
  // Write PDR Command
  //
  Write_PDR_SW_SMI                                = 0x51,
  //
  // Initial Cipher for S3
  //
  Re_Init_Cipher                                  = 0x52,
  Return_PDR0_Data                                = 0x53,

  SW_SMI_FROM_SMMBASE                             = 0x55,

//[-start-120314-IB07360178-remove]//
//  SMM_SET_SSID_SVID_CALLBACK                      = 0x60,
//[-end-120314-IB07360178-remove]//
//[-start-120315-IB04770219-modify]//
  ISCT_RTC_SW_SMI                                 = 0x70,
//[-end-120315-IB04770219-modify]//
//[-start-120705-IB05300312-add]//
  PFAT_FV_SW_SMI                                  = 0x71,
//[-end-120705-IB05300312-add]//
  //
  //  XTU SMI
  //
//#ifdef XTU_SUPPORT
  EFI_PERF_TUNE_SW_SMI                            = 0x72,
//#endif  

//#ifdef NVIDIA_SG_SUPPORT
  //
  // Nvidia Optimus _ON/_OFF save/restore SMI
  //
  OPTIMUS_SAVE_PEG_REGISTER                       = 0x7A,
  OPTIMUS_RESTORE_PEG_REGISTER                    = 0x7B,
//#endif
  //
  // OS_COMMAND
  //
  PPM_OS_COMMAND                                  = 0x80,
  //
  // INIT_COMMAND
  //
  PPM_INIT_COMMAND                                = 0x81, //no function
  //
  // APP_COMMAND
  //
  PPM_APP_COMMAND                                 = 0x82,
  //
  // OS_REQUEST
  //
  PPM_OS_REQUEST                                  = 0x83, //no function
  //
  // CSTATE_COMMAND
  //
  PPM_CSTATE_COMMAND                              = 0x85, //no function
  //
  // IST_INIT_OS_TRANSITION
  //
  PPM_IST_INIT_OS_TRANSITION                      = 0x86,  
//[-start-130613-IB10930032-add]//
  //
  //  For Variable Editor Tool
  //
  IVE_SW_SMI                                      = 0x97,  
//[-end-130613-IB10930032-add]//
//[-start-130617-IB04560405-add]//
  SMM_EVENT_LOG_SMI                               = 0x99,
//[-end-130617-IB04560405-add]//
  PPM_INTERNAL_SW_SMI                             = 0xE4,
  S3_RESTORE_MSR_SW_SMI                           = 0xE5,
  POWER_STATE_SWITCH_SMI                          = 0xE6,
  ENABLE_C_STATE_IO_REDIRECTION_SMI               = 0xE7,
  DISABLE_C_STATE_IO_REDIRECTION_SMI              = 0xE8,
  ENABLE_P_STATE_HARDWARE_COORDINATION_SMI        = 0xE9,
  DISABLE_P_STATE_HARDWARE_COORDINATION_SMI       = 0xEA,
  BIOS_LOCK_SW_SMI                                = 0xEB,
//[-start-120315-IB04770219-remove]//
//   ISCT_RTC_SW_SMI            =  0xEC,
//[-end-120315-IB04770219-remove]//
//#ifdef THUNDERBOLT_SUPPORT
  //
  // Thunderbolt SMI Handler
  //
  THUNDERBOLT_SW_SMI                              = 0xF1,
//#endif
  AMT_FLASH_SW_SMI                                = 0xF2,
  AMT_FLASH_WRITE_PROTECT_POST_COMPLETE_SW_SMI    = 0xF3,

  //
  // Smm platform
  //
  PCIE_HOT_PLUG_SMI                               = 0xF4,
  ME_LOCK_SW_SMI                                  = 0xF5,
  EFI_OEM_FUNCTION_SW_SMI                         = 0xB2
}CHIPSET_SW_SMI_PORT_TABLE;

#endif

