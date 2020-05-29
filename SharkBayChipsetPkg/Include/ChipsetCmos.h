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

/*--
Module Name:

  ChipsetCmos.h

Abstract:


--*/

#ifndef _CHIPSET_CMOS_H_
#define _CHIPSET_CMOS_H_

#define R_XCMOS_INDEX                     0x72
#define R_XCMOS_DATA                      0x73

//
// PlatformConfigFlag
//
#define B_CONFIG_IGD_PRESENT_FLAG         BIT0

//
// PlatformSettingFlag
//
#define B_SETTING_MEM_REFRESH_FLAG        BIT0

//
// VmxSmxFlag
//
#define B_SMX_SETUP_FLAG                  BIT0
#define B_SMX_CPU_FLAG                    BIT1
#define B_SMX_CHIPSET_FLAG                BIT2
#define B_SMX_FLAG                        ( B_SMX_SETUP_FLAG | B_SMX_CPU_FLAG | B_SMX_CHIPSET_FLAG )
#define B_VMX_SETUP_FLAG                  BIT4
#define B_VMX_CPU_FLAG                    BIT5
#define B_VMX_FLAG                        ( B_VMX_SETUP_FLAG | B_VMX_CPU_FLAG )

//
// TxtMemoryScrubFlag
//
#define B_MEMORY_SCRUB                    BIT0

#define RTC_BANK1_SIZE                    0x80

//
//MEFlashReset
//
#define V_ME_GLOBAL_RESET                    0xF1
#define V_ME_NO_RESET                        0x00
#define V_ME_NEED_BIOS_SYNC                  0xE1
#define NV_NOT_FOUND                         0xF0
#define NV_AVAILABLE                         0x00
//[-start-120605-IB10820064-removed]//
//#ifdef XTU_SUPPORT
//[-end-120605-IB10820064-removed]//
#define B_XTU_WATCH_DOG_TRIGGER           BIT0    // for tuning AP
#define B_XTU_WATCH_DOG_STATUS            BIT1    // for tuning AP
#define B_XTU_WATCH_DOG                   ( B_XTU_WATCH_DOG_TRIGGER | B_XTU_WATCH_DOG_STATUS )
#define SET_WATCH_DOG_STATUS(cmos)        ( (cmos) | B_XTU_WATCH_DOG_TRIGGER )
#define UPDATE_WATCH_DOG_STATUS(cmos)     ( ((cmos) & ~B_XTU_WATCH_DOG) | (((cmos) & B_XTU_WATCH_DOG_TRIGGER) << 1) )
#define B_XTU_WATCH_DOG_FLAG_PEI_1        BIT2
#define B_XTU_WATCH_DOG_FLAG_PEI_2        BIT3
#define B_XTU_WATCH_DOG_FLAG_XMP          BIT4
#define B_XTU_FLAG_FAIL                   BIT5
#define B_XTU_FLAG_POSTING                BIT6
#define B_XTU_FLAG_SAFE_RECOVERY          BIT7  // if clear (clear cmos), XTU will set disable!!!
// XtuRuntimeStatus
#define B_XTU_XMP_CHANGE                  BIT0
//[-start-120605-IB10820064-removed]//
//#endif   
//[-end-120605-IB10820064-removed]//
typedef enum {

  TcoWatchdogTimerStatus            = 0x45        // 0x45        : Chipset Reserve Watchdog Timer status
                                                  // 0x48 ~ 0x4F : OEM Reserve
                                                  // 0x6C ~ 0x7F : OEM Reserve
} OPTION_CMOS_TABLE1;
//} OPTION_COMS_TABLE1;

//[-start-120619-IB03530372-modify]//
typedef enum {

                                                  // 0x00 ~ 0x0F : OEM Reserve
  FFS_SMMRAM_HASH_KEY_Byte_0_28     = 0x10,       // 0x10 ~ 0x2C : RapidStart SMM Ram Hash key byte 0~28
  MEFlashReset                      = 0x2D,       // 0x2D        : ME Flash Reset
  FFS_SMMRAM_HASH_KEY_Byte_29_31    = 0x2E,       // 0x2E ~ 0x30 : RapidStart SMM Ram Hash key byte 29~31
//#ifdef RAPID_START_FLAG
  FFS_NV_CONFIG_REG   = (0x31 + RTC_BANK1_SIZE),  // 0x31        : RapidStart runtime configuration settings
//#endif 
  PlatformConfigFlag                = 0x32,       // 0x32        : Flag of Platform Configuration
  PlatformSettingFlag               = 0x33,       // 0x33        : Flag of Platform Settings
//[-start-120404-IB05300309-remove]//
//  ScramblerSeedOffset = (0x34 + RTC_BANK1_SIZE),  // 0x34 ~ 0x35 : 16bit CMOS location for scrambling seed storage
//[-end-120404-IB05300309-remove]//
//[-start-120605-IB10820064-removed]//
//#ifdef XTU_SUPPORT
//[-end-120605-IB10820064-removed]//
  XtuRuntimeStatus                  = 0x36,       // 0x36        : XTU Runtime Status
//[-start-120605-IB10820064-removed]//
//#endif
//[-end-120605-IB10820064-removed]//
  CritticalBatWakeThres             = 0x37,       // 0x37        : RapidStart CRITICAL BATTERY WAKE THRESHOLD
//#ifdef RAPID_START_FLAG
  FFS_NV_FLAG_REG     = (0x38 + RTC_BANK1_SIZE),  // 0x38        : Non-volatile RapidStart flag
//#endif 
//#ifdef XTU_SUPPORT
//[-end-120605-IB10820064-removed]//
  // bit0 : set when watchdog trigger, this bit will "<< 1" in next POST time (AcpiWatchDog.c)
  // bit1 : watchdog status, 1=latest reboot/shoudown by watchdog, 0=normal
  XtuWdtStatus                      = 0x39,       // 0x39        : XTU check watch dog status
  XtuCrashCounter                   = 0x3B,       // 0x3B
//[-start-120605-IB10820064-removed]//  
//#endif    
//[-end-120605-IB10820064-removed]//                  
  IccWatchdogFlag                   = 0x3A,       // 0x3A        : ICC set profile WDT flag
  SetupNVFlag                       = 0x3C,       // 0x3C        : NV varible availible flag
                                                  // 0x3D        : Chipset Reserve
//[-start-130124-IB10130023-modify]//
  VmxSmxFlag                        = 0x3E,       // 0x3E        : Flag of VT / TXT
                                                  // 0x3F        : Chipset Reserve
  RecoveryFlag                      = 0x40        // 0x40        : RecoveryFlag
                                                  // 0x41 ~ 0x7F : OEM Reserve
//[-end-130124-IB10130023-modify]//

} OPTION_CMOS_TABLE2;
//} OPTION_COMS_TABLE2;
//[-end-120619-IB03530372-modify]//

#endif
