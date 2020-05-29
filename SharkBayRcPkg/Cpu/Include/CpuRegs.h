/** @file
  Conventions:
  - Prefixes:
    Definitions beginning with "MSR_" are MSRs
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2004 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#include "CommonIncludes.h"

//
// Local APIC defines
//
#define APIC_REGISTER_LOCAL_ID_OFFSET         0x00000020
#define APIC_REGISTER_APIC_VERSION_OFFSET     0x00000030
#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET  0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET          0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET         0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET     0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET     0x00000360

#define BROADCAST_MODE_SPECIFY_CPU            0x00
#define BROADCAST_MODE_ALL_INCLUDING_SELF     0x01
#define BROADCAST_MODE_ALL_EXCLUDING_SELF     0x02

#ifndef DELIVERY_MODE_REMOTE_READ

//#define DELIVERY_MODE_FIXED           0x00
//#define DELIVERY_MODE_LOWEST_PRIORITY 0x01
//#define DELIVERY_MODE_SMI             0x02
//#define DELIVERY_MODE_REMOTE_READ     0x03
//#define DELIVERY_MODE_NMI             0x04
//#define DELIVERY_MODE_INIT            0x05
//#define DELIVERY_MODE_SIPI            0x06
//#define DELIVERY_MODE_MAX             0x07

#define TRIGGER_MODE_EDGE             0x00
#define TRIGGER_MODE_LEVEL            0x01

#endif

#define CACHE_UNCACHEABLE               0
#define CACHE_WRITECOMBINING            1
#define CACHE_WRITETHROUGH              4
#define CACHE_WRITEPROTECTED            5
#define CACHE_WRITEBACK                 6

#define CPUID_SIGNATURE                 0x0
#define CPUID_VERSION_INFO              0x1
#define CPUID_FUNCTION_4                0x4
#define CPU_CACHE_TYPE_MASK             0x1F
#define CPU_CACHE_LEVEL_MASK            0x07
#define CPU_CACHE_ASSOCIATIVITY_MASK    0x03FF
#define CPU_CACHE_PARTITION_MASK        0x03FF
#define CPU_CACHE_LINE_SIZE_MASK        0x0FFF
#define B_CPUID_VERSION_INFO_ECX_MWAIT  BIT3
#define B_CPUID_VERSION_INFO_ECX_VME    BIT5
#define B_CPUID_VERSION_INFO_ECX_SME    BIT6
#define B_CPUID_VERSION_INFO_ECX_EIST   BIT7
#define B_CPUID_VERSION_INFO_ECX_TM2    BIT8
#define B_CPUID_VERSION_INFO_ECX_DCA    BIT18
#define B_CPUID_VERSION_INFO_ECX_AES    BIT25
#define B_CPUID_VERSION_INFO_ECX_XAPIC  BIT21
#define B_CPUID_VERSION_INFO_EDX_XD     BIT20
#define B_CPUID_VERSION_INFO_EDX_HT     BIT28
#define B_CPUID_VERSION_INFO_EDX_TM1    BIT29

#define CPUID_CACHE_INFO                0x2
#define CPUID_SERIAL_NUMBER             0x3
#define CPUID_CACHE_PARAMS              0x4

//
// CPU ID Instruction defines
//
#define V_CPUID_CACHE_TYPE_MASK         0x1F
#define B_CPUID_CACHE_TYPE_DATA         0x1
#define B_CPUID_CACHE_TYPE_INSTRUCTION  0x2
#define B_CPUID_CACHE_TYPE_UNIFIED      0x3
#define V_CPUID_CACHE_LEVEL_MASK        0xE0
#define B_CPUID_CACHE_LEVEL_SHIFT       5
#define B_CPUID_CACHE_PARAMS_WAYS_SHIFT                               22
#define B_CPUID_CACHE_PARAMS_PARTITIONS_SHIFT                         12
#define CPUID_MONITOR_MWAIT_PARAMS                                    0x5
#define B_CPUID_MONITOR_MWAIT_ECX_EXTENSIONS                          BIT0
#define B_CPUID_MONITOR_MWAIT_EDX_CSTATE                              BIT0
#define V_CPUID_MONITOR_MWAIT_EDX_ENHANCED_CSTATE                     0x2
#define CPUID_POWER_MANAGEMENT_PARAMS                                 0x6
#define B_CPUID_POWER_MANAGEMENT_EAX_TURBO                            BIT1
#define B_CPUID_POWER_MANAGEMENT_EAX_FINE_GRAINED_CLOCK_MODULATION    BIT5
#define B_CPUID_POWER_MANAGEMENT_ECX_ENERGY_EFFICIENT_POLICY_SUPPORT  BIT3
#define CPUID_REV7                                                    0x7
#define CPUID_REV8                                                    0x8
#define CPUID_DCA_PARAMS                                              0x9
#define CPUID_REVA                                                    0xA
#define CPUID_CORE_TOPOLOGY                                           0xB
#define CPUID_EXTENDED_FUNCTION                                       0x80000000
#define CPUID_EXTENDED_CPU_SIG                                        0x80000001
#define CPUID_BRAND_STRING1                                           0x80000002
#define CPUID_BRAND_STRING2                                           0x80000003
#define CPUID_BRAND_STRING3                                           0x80000004
#define CPUID_L2_CACHE_FEATURE                                        0x80000006
#define CPUID_VIR_PHY_ADDRESS_SIZE                                    0x80000008

//
// MSR defines
//
#define MSR_IA32_PLATFORM_ID                                          0x00000017
#define MSR_IA32_APIC_BASE                                            0x0000001B
#define B_MSR_IA32_APIC_BASE_G_XAPIC                                  BIT11
#define B_MSR_IA32_APIC_BASE_M_XAPIC                                  BIT10
#define B_MSR_IA32_APIC_BASE_BSP                                      BIT8
#define PIC_THREAD_CONTROL                                            0x0000002E
#define B_PIC_THREAD_CONTROL_TPR_DIS                                  BIT10
#define MSR_CORE_THREAD_COUNT                                         0x00000035
#define N_CORE_COUNT_OFFSET                                           16
#define B_THREAD_COUNT_MASK                                           0xFFFF
#define MSR_SOCKET_ID_MSR                                             0x00000039
#define MSR_IA32_FEATURE_CONTROL                                      0x0000003A
#define B_MSR_IA32_FEATURE_CONTROL_LOCK                               BIT0
#define B_MSR_IA32_FEATURE_CONTROL_ELT                                BIT1
#define B_MSR_IA32_FEATURE_CONTROL_EVT                                BIT2
#define B_MSR_IA32_FEATURE_CONTROL_SLFE                               (BIT8 | BIT9 | BIT10 | BIT11 | BIT12 | BIT13 | BIT14)
#define B_MSR_IA32_FEATURE_CONTROL_SGE                                BIT15
#define MSR_IA32_SMM_SAVE_CONTROL                                     0x0000003E
#define B_MSR_IA32_SMM_SAVE_CONTROL_SFPPE                             BIT0
#define MSR_IA32_BIOS_UPDT_TRIG                                       0x00000079
#define MSR_IA32_BIOS_SIGN_ID                                         0x0000008B
#define MSR_IA32_SMM_MONITOR_CONTROL                                  0x0000009B
#define MSR_IA32_PMC0_MSR                                             0x000000C1
#define MSR_IA32_PMC1_MSR                                             0x000000C2
#define MSR_IA32_PMC2_MSR                                             0x000000C3
#define MSR_IA32_PMC3_MSR                                             0x000000C4
#define MSR_IA32_PMC4_MSR                                             0x000000C5
#define MSR_IA32_PMC5_MSR                                             0x000000C6
#define MSR_IA32_PMC6_MSR                                             0x000000C7
#define MSR_IA32_PMC7_MSR                                             0x000000C8
#define MSR_PLATFORM_INFO                                             0x000000CE
#define N_PLATFORM_INFO_MIN_RATIO                                     40
#define B_PLATFORM_INFO_RATIO_MASK                                    0xFF
#define N_PLATFORM_INFO_MAX_RATIO                                     8
#define B_MSR_PLATFORM_INFO_PFAT_AVAIL                                BIT35
#define N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET              33
#define V_CONFIG_TDP_NUM_LEVELS_MASK                                  (BIT34 | BIT33)
#define B_PLATFORM_INFO_TDC_TDP_LIMIT                                 BIT29
#define N_PLATFORM_INFO_RATIO_LIMIT                                   28
#define B_PLATFORM_INFO_RATIO_LIMIT                                   BIT28
#define B_FIVR_RFI_TUNING_AVAIL                                       BIT25
#define B_PLATFORM_INFO_SMM_SAVE_CONTROL                              BIT16
#define N_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    30
#define B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    BIT30
#define B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                         BIT37
#define MSR_PMG_CST_CONFIG                                            0x000000E2
#define B_CST_CONTROL_LOCK                                            BIT15
#define B_IO_MWAIT_REDIRECTION_ENABLE                                 BIT10
#define B_TIMED_MWAIT_ENABLE                                          BIT31
#define B_PACKAGE_C_STATE_LIMIT                                       (BIT3 | BIT2 | BIT1 | BIT0)
#define V_CSTATE_LIMIT_C1                                             0x01
#define V_CSTATE_LIMIT_C3                                             0x02
#define V_CSTATE_LIMIT_C6                                             0x03
#define V_CSTATE_LIMIT_C7                                             0x04
#define V_CSTATE_LIMIT_C7S                                            0x05
#define V_CSTATE_LIMIT_C8                                             0x06
#define V_CSTATE_LIMIT_C9                                             0x07
#define V_CSTATE_LIMIT_C10                                            0x08
#define B_C3_AUTO_DEMOTION_ENABLE                                     BIT25
#define B_C1_AUTO_DEMOTION_ENABLE                                     BIT26
#define B_C3_AUTO_UNDEMOTION_ENABLE                                   BIT27
#define B_C1_AUTO_UNDEMOTION_ENABLE                                   BIT28
#define B_PKG_CSTATE_DEMOTION_ENABLE                                  BIT29
#define B_PKG_CSTATE_UNDEMOTION_ENABLE                                BIT30
#define MSR_RFI_TUNNING                                               0x000000E3
#define V_FREQ_TUNNING_MASK                                           0xFFFF
#define MSR_PMG_IO_CAPTURE_BASE                                       0x000000E4
#define B_MSR_PMG_CST_RANGE                                           (BIT18 | BIT17 | BIT16)
#define V_IO_CAPT_LVL2                                                (0x0 << 16)   ///< C3
#define V_IO_CAPT_LVL3                                                (0x1 << 16)   ///< C6
#define V_IO_CAPT_LVL4                                                (0x2 << 16)   ///< C7
#define V_IO_CAPT_LVL5                                                (0x3 << 16)   ///< C8
#define V_IO_CAPT_LVL6                                                (0x4 << 16)   ///< C9
#define V_IO_CAPT_LVL7                                                (0x5 << 16)   ///< C10
#define V_IO_CAPT_LVL2_BASE_ADDR_MASK                                 0xFFFF
#define IA32_MTRR_CAP                                                 0x000000FE
#define B_IA32_MTRR_VARIABLE_SUPPORT                                  0xFF
#define B_IA32_MTRR_CAP_FIXED_SUPPORT                                 BIT8
#define B_IA32_MTRR_CAP_SMRR_SUPPORT                                  BIT11
#define B_IA32_MTRR_CAP_EMRR_SUPPORT                                  BIT12
#define MSR_PLAT_FRMW_PROT_CTRL                                       0x00000110
#define B_MSR_PLAT_FRMW_PROT_CTRL_LK                                  BIT0
#define B_MSR_PLAT_FRMW_PROT_CTRL_EN                                  BIT1
#define B_MSR_PLAT_FRMW_PROT_CTRL_S1                                  BIT2
#define MSR_PLAT_FRMW_PROT_HASH_0                                     0x00000111
#define MSR_PLAT_FRMW_PROT_HASH_1                                     0x00000112
#define MSR_PLAT_FRMW_PROT_HASH_2                                     0x00000113
#define MSR_PLAT_FRMW_PROT_HASH_3                                     0x00000114
#define MSR_PLAT_FRMW_PROT_TRIG_PARAM                                 0x00000115
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET                 0
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_MASK                   0x000000000000FFFF
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET                   16
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_MASK                     0x000000000000FFFF
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET               32
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_MASK                 0x000000000000FFFF
#define B_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE                            BIT62
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE_OFFSET                     62
#define MSR_PLAT_FRMW_PROT_TRIGGER                                    0x00000116
#define MSR_PLAT_FRMW_PROT_PASSWD                                     0x00000117
#define MSR_SPCL_CHIPSET_USAGE_ADDR                                   0x000001FE
#define MSR_IA32_FEATURE_CONFIG                                       0x0000013C
#define B_IA32_FEATURE_CONFIG_AES_DIS                                 BIT1
#define B_IA32_FEATURE_CONFIG_LOCK                                    BIT0
#define IA32_MCG_CAP                                                  0x00000179
#define IA32_MCG_STATUS                                               0x0000017A
#define MSR_FLEX_RATIO                                                0x00000194
#define N_FLEX_RATIO                                                  8
#define B_FLEX_RATIO                                                  (0xFF << 8)
#define B_FLEX_EN                                                     BIT16
#define B_MAX_EXTRA_VOLTAGE                                           0xFF
#define N_OVERCLOCKING_BINS                                           17
#define B_OVERCLOCKING_BINS                                           (0x7 << 17)
#define B_OVERCLOCKING_LOCK                                           BIT20
#define RATIO_FLEX_CLEAR_MASK                                         0xFFFFFFFFFFFF00FFULL
#define MSR_IA32_PERF_STS                                             0x00000198
//[-start-121225-IB10920006-modify]//
//[-start-110630-IB03600406-add]//
#define B_EFI_PLATFORM_P_STATE_RATIO_MASK                             0xFF
#define N_EFI_PLATFORM_P_STATE_RATIO                                  8
#define INTERL_BASE_CLOCK                                             100
//[-end-110630-IB03600406-add]//
//[-end-121225-IB10920006-modify]//
#define MSR_IA32_PERF_CTRL                                            0x00000199
#define N_IA32_PERF_CTRLP_STATE_TARGET                                8
#define B_IA32_PERF_CTRLP_STATE_TARGET                                (0x7F << 8)
#define B_IA32_PERF_CTRL_TURBO_DIS                                    BIT32
#define MSR_IA32_CLOCK_MODULATION                                     0x0000019A
#define IA32_THERM_INTERRUPT                                          0x0000019B
#define B_IA32_THERM_INTERRUPT_VIE                                    BIT4
#define MSR_IA32_THERM_STATUS                                         0x0000019C
#define MSR_IA32_MISC_ENABLE                                          0x000001A0
#define B_MSR_IA32_MISC_ENABLE_FSE                                    BIT0
#define B_MSR_IA32_MISC_ENABLE_TME                                    BIT3
#define N_MSR_IA32_MISC_ENABLE_EIST_OFFSET                            16
#define B_MSR_IA32_MISC_ENABLE_EIST                                   BIT16
#define B_MSR_IA32_MISC_ENABLE_MONITOR                                BIT18
#define B_MSR_IA32_MISC_ENABLE_CPUID_MAX                              BIT22
#define B_MSR_IA32_MISC_ENABLE_TPR_DIS                                BIT23
#define B_MSR_IA32_MISC_ENABLE_XD                                     BIT34
#define B_MSR_IA32_MISC_DISABLE_TURBO                                 BIT38
#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT                     24
#define N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET    (16)
#define B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK      (0xFF << 16)
#define N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               8
#define B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               (0xFF << 8)
#define MISC_FEATURE_CONTROL                                          0x000001A4
#define B_MISC_FEATURE_CONTROL_MLC_STRP                               BIT0
#define B_MISC_FEATURE_CONTROL_MLC_SPAP                               BIT1
#define B_MISC_FEATURE_CONTROL_DCU_STRP                               BIT2
#define B_MISC_FEATURE_CONTROL_DCU_IPP                                BIT3
#define MSR_MISC_PWR_MGMT                                             0x000001AA
#define B_MSR_MISC_PWR_MGMT_EIST_HW                                   BIT0
#define B_MSR_MISC_PWR_MGMT_LTMI                                      BIT22
#define MSR_TURBO_POWER_CURRENT_LIMIT                                 0x000001AC
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_EN                        BIT31
#define N_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_LIMIT                     16
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_LIMIT                     (0x7F << 16)
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_EN                        BIT15
#define N_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_LIMIT                     0
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_LIMIT                     (0x7F << 0)
#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define N_MSR_TURBO_RATIO_LIMIT_1C                                    0
#define B_MSR_TURBO_RATIO_LIMIT_1C                                    (0xFF << 0)
#define N_MSR_TURBO_RATIO_LIMIT_2C                                    8
#define B_MSR_TURBO_RATIO_LIMIT_2C                                    (0xFF << 8)
#define N_MSR_TURBO_RATIO_LIMIT_3C                                    16
#define B_MSR_TURBO_RATIO_LIMIT_3C                                    (0xFF << 16)
#define N_MSR_TURBO_RATIO_LIMIT_4C                                    24
#define B_MSR_TURBO_RATIO_LIMIT_4C                                    (0xFF << 24)
#define MSR_IA32_ENERGY_PERFORMANCE_BIAS                              0x1B0
#define B_ENERGY_POLICY_MASK                                          0xF
#define MSR_IA32_PLATFORM_DCA_CAP                                     0x000001F8
#define B_MSR_IA32_PLATFORM_DCA_CAP_TYPE0_EN                          BIT0
#define MSR_IA32_CPU_DCA_CAP                                          0x000001F9
#define B_MSR_IA32_CPU_DCA_CAP_TYPE0_SUP                              BIT0
#define MSR_IA32_DCA_0_CAP                                            0x000001FA
#define B_MSR_IA32_CPU_DCA_CAP_ENDID                                  BIT11
#define N_MSR_IA32_CPU_DCA_CAP_DELAY                                  13
#define B_MSR_IA32_CPU_DCA_CAP_DELAY                                  (BIT13 | BIT14 | BIT15 | BIT16)
#define B_MSR_IA32_CPU_DCA_CAP_SW_LOCK                                BIT24
#define B_MSR_IA32_CPU_DCA_CAP_SW_FLUSH                               BIT25
#define B_MSR_IA32_CPU_DCA_CAP_HW_LOCK                                BIT26
#define MSR_POWER_CTL                                                 0x000001FC
#define B_MSR_POWER_CTL_BROCHOT                                       BIT0
#define B_MSR_POWER_CTL_C1E                                           BIT1
#define B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE                     BIT18
#define B_MSR_POWER_CTL_DISABLE_PHOT_OUT                              BIT21
#define B_MSR_POWER_CTL_PROC_HOT_RESPONSE                             BIT22
#define B_MSR_POWER_CTL_PROC_HOT_LOCK                                 BIT23
#define B_MSR_POWER_CTL_DISABLE_VR_THERMAL_ALERT                      BIT24
#define B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE                       BIT30
#define MSR_FERR_CAPABILITIES                                         0x000001F1
#define B_MSR_FERR_ENABLE                                             BIT0
#define MSR_VR_CURRENT_CONFIG                                         0x00000601
#define B_CURRENT_LIMIT_LOCK                                          BIT31
#define B_CURRENT_LIMIT_MASK                                          0x1FFF
#define MSR_VR_MISC_CONFIG                                            0x603
#define N_MSR_VR_MISC_CONFIG_MIN_VID_OFFSET                           24
#define B_MSR_VR_MISC_CONFIG_MIN_VID_MASK                             0xFF
#define V_MSR_VR_MISC_CONFIG_MIN_VID_DEFAULT                          0
#define N_MSR_VR_MISC_CONFIG_IDLE_EXIT_RAMP_RATE_OFFSET               50
#define B_MSR_VR_MISC_CONFIG_IDLE_EXIT_RAMP_RATE                      BIT50
#define N_MSR_VR_MISC_CONFIG_IDLE_ENTRY_RAMP_RATE_OFFSET              51
#define B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_RAMP_RATE                     BIT51
#define N_MSR_VR_MISC_CONFIG_IDLE_ENTRY_DECAY_ENABLE_OFFSET           52
#define B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_DECAY_ENABLE                  BIT52
#define N_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_OFFSET             53
#define B_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_MASK               (BIT54 | BIT53)
#define V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_DEFAULT            1
#define V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_2             0
#define V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_4             BIT53
#define V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_8             BIT54
#define V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_16            (BIT54 | BIT53)
#define MSR_VR_MISC_CONFIG2                                           0x636
#define N_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_OFFSET                0
#define B_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_MASK                  0xFF
#define V_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_DEFAULT               0x6F
#define N_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_OFFSET                   8
#define B_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_MASK                     0xFF
#define V_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_DEFAULT                  0
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x606
#define PACKAGE_POWER_UNIT_MASK                                       0xF
#define PACKAGE_TIME_UNIT_MASK                                        0xF0000
#define MSR_C_STATE_LATENCY_CONTROL_0                                 0x60A
#define MSR_C_STATE_LATENCY_CONTROL_1                                 0x60B
#define MSR_C_STATE_LATENCY_CONTROL_2                                 0x60C
#define MSR_C_STATE_LATENCY_CONTROL_3                                 0x633
#define MSR_C_STATE_LATENCY_CONTROL_4                                 0x634
#define MSR_C_STATE_LATENCY_CONTROL_5                                 0x635
#define B_PKG_IRTL_VALID                                              BIT15
#define B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK                          0x3FF
#define B_TIME_UNIT_MASK                                              (0x7 << 10)
#define N_TIME_UNIT_OFFSET                                            10
#define MSR_PACKAGE_POWER_LIMIT                                       0x610
#define MSR_PACKAGE_POWER_SKU                                         0x614
#define B_POWER_LIMIT_ENABLE                                          BIT15
#define B_CRITICAL_POWER_CLAMP_ENABLE                                 BIT16
#define B_POWER_LIMIT_LOCK                                            BIT31
#define POWER_LIMIT_MASK                                              (0x7FFF)
#define POWER_LIMIT_1_TIME_MASK                                       (0xFE0000)
#define PACKAGE_TDP_POWER_MASK                                        (0x7FFF)
#define PACKAGE_MIN_POWER_MASK                                        (0x7FFF0000)
#define PACKAGE_MAX_POWER_MASK                                        (0x7FFF)
#define MSR_PLATFORM_POWER_LIMIT                                      0x615
#define POWER_LIMIT_3_TIME_MASK                                       (0xFE0000)
#define POWER_LIMIT_3_DUTY_CYCLE_MASK                                 (0x7F000000)
#define MSR_DDR_RAPL_LIMIT                                            0x618
#define MSR_RING_RATIO_LIMIT                                          0x620
#define MSR_MAX_RING_RATIO_LIMIT_MASK                                 0x7F
#define MSR_CONFIG_TDP_NOMINAL                                        0x648
#define CONFIG_TDP_NOMINAL_RATIO_MASK                                 0xFF
#define MSR_CONFIG_TDP_LVL1                                           0x649
#define CONFIG_TDP_LVL1_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL1_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL1_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_LVL2                                           0x64A
#define CONFIG_TDP_LVL2_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL2_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL2_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_CONTROL                                        0x64B
#define CONFIG_TDP_CONTROL_LOCK                                       (1 << 31)
#define CONFIG_TDP_CONTROL_LVL_MASK                                   0x3
#define CONFIG_TDP_NOMINAL                                            0
#define CONFIG_TDP_LEVEL1                                             1
#define CONFIG_TDP_LEVEL2                                             2
#define MSR_TURBO_ACTIVATION_RATIO                                    0x64C
#define MSR_TURBO_ACTIVATION_RATIO_LOCK                               (1 << 31)
#define MSR_TURBO_ACTIVATION_RATIO_MASK                               0xFF
#define SMRR_PHYS_BASE                                                0x000001F2
#define SMRR_PHYS_MASK                                                0x000001F3
#define EMRR_PHYS_BASE                                                0x000001F4
#define EMRR_PHYS_MASK                                                0x000001F5
#define B_MSR_EMRR_PHYS_MASK_EN                                       BIT11
#define B_MSR_EMRR_PHYS_MASK_LOCK                                     BIT10
#define V_MAXIMUM_VARIABLE_MTRR_NUMBER                                10
#define CACHE_VARIABLE_MTRR_BASE                                      0x00000200
#define V_FIXED_MTRR_NUMBER                                           11
#define IA32_MTRR_FIX64K_00000                                        0x00000250
#define IA32_MTRR_FIX16K_80000                                        0x00000258
#define IA32_MTRR_FIX16K_A0000                                        0x00000259
#define IA32_MTRR_FIX4K_C0000                                         0x00000268
#define IA32_MTRR_FIX4K_C8000                                         0x00000269
#define IA32_MTRR_FIX4K_D0000                                         0x0000026A
#define IA32_MTRR_FIX4K_D8000                                         0x0000026B
#define IA32_MTRR_FIX4K_E0000                                         0x0000026C
#define IA32_MTRR_FIX4K_E8000                                         0x0000026D
#define IA32_MTRR_FIX4K_F0000                                         0x0000026E
#define IA32_MTRR_FIX4K_F8000                                         0x0000026F
#define MSR_IA32_CR_PAT                                               0x00000277
#define CACHE_IA32_MTRR_DEF_TYPE                                      0x000002FF
#define B_CACHE_MTRR_VALID                                            BIT11
#define B_CACHE_FIXED_MTRR_VALID                                      BIT10
#define NO_EVICT_MODE                                                 0x000002E0
#define B_NO_EVICT_MODE_SETUP                                         BIT0
#define B_NO_EVICT_MODE_RUN                                           BIT1
#define UNCORE_CR_MEMLOCK_COMMANDS                                    0x000002E2
#define B_LOCK_MEM_CFG                                                BIT1
#define EFI_PCIEXBAR                                                  0x00000300
#define B_PCIEXBAR_EN                                                 BIT0
#define B_PCIEXBAR_SIZE                                               (BIT1 | BIT2 | BIT3)
#define N_PCIEXBAR_SIZE                                               1
#define IA32_MC0_CTL                                                  0x00000400
#define IA32_MC0_STATUS                                               0x00000401
#define IA32_MC0_ADDR                                                 0x00000402
#define IA32_MC0_MISC                                                 0x00000403
#define IA32_MC8_CTL                                                  (IA32_MC0_CTL + (8 * 4))
#define IA32_MC5_STATUS                                               (IA32_MC0_STATUS + (5 * 4))
#define IA32_MC6_STATUS                                               (IA32_MC0_STATUS + (6 * 4))
#define IA32_MC7_STATUS                                               (IA32_MC0_STATUS + (7 * 4))
#define IA32_MC8_STATUS                                               (IA32_MC0_STATUS + (8 * 4))
#define MSR_IA32_VMX_BASIC                                            0x00000480
#define MSR_IA32_VMX_MISC                                             0x00000485
#define APIC_GLOBAL_ENABLE                                            0x00000800
#define EXT_XAPIC_LOGICAL_APIC_ID                                     0x00000802
#define EXT_XAPIC_VERSION                                             0x00000803
#define EXT_XAPIC_SVR                                                 0x0000080F
#define EXT_XAPIC_ICR                                                 0x00000830
#define MSR_EXT_XAPIC_LVT_THERM                                       0x00000833
#define EXT_XAPIC_LVT_LINT0                                           0x00000835
#define EXT_XAPIC_LVT_LINT1                                           0x00000836
#define MSR_IA32_DEBUG_INTERFACE                                      0x00000C80
#define B_DEBUG_INTERFACE_ENABLE                                      BIT0
#define B_DEBUG_INTERFACE_LOCK                                        BIT30
#define B_DEBUG_INTERFACE_DEBUG_STATUS                                BIT31
#define NUM_TENTHS_TO_PERCENTAGE                                      1000
#define FIVR_SSC_LOCK_BIT                                             BIT31
#define MAX_FIVR_SSC_PERCENT                                          60
//
// MSRs for SMM State Save Register
//
#define MSR_SMM_MCA_CAP                                               0x17D
#define B_TARGETED_SMI                                                BIT56
#define N_TARGETED_SMI                                                56
#define B_SMM_CPU_SVRSTR                                              BIT57
#define N_SMM_CPU_SVRSTR                                              57
#define B_SMM_CODE_ACCESS_CHK                                         BIT58
#define N_SMM_CODE_ACCESS_CHK                                         58
#define B_LONG_FLOW_INDICATION                                        BIT59
#define N_LONG_FLOW_INDICATION                                        59
#define MSR_SMM_FEATURE_CONTROL                                       0x4E0
#define B_SMM_FEATURE_CONTROL_LOCK                                    BIT0
#define B_SMM_CPU_SAVE_EN                                             BIT1
#define B_SMM_CODE_CHK_EN                                             BIT2
#define MSR_SMM_ENABLE                                                0x4E1
#define MSR_SMM_DELAYED                                               0x4E2
#define MSR_SMM_BLOCKED                                               0x4E3
#define MSR_CR0                                                       0xC00
#define MSR_CR3                                                       0xC01
#define MSR_EFLAGS                                                    0xC02
#define MSR_RIP                                                       0xC04
#define MSR_DR6                                                       0xC05
#define MSR_DR7                                                       0xC06
#define MSR_TR_LDTR                                                   0xC07
#define MSR_GS_FS                                                     0xC08
#define MSR_DS_SS                                                     0xC09
#define MSR_CS_ES                                                     0xC0A
#define MSR_IO_MISC_INFO                                              0xC0B
#define MSR_IO_MEM_ADDR                                               0xC0C
#define MSR_RDI                                                       0xC0D
#define MSR_RSI                                                       0xC0E
#define MSR_RBP                                                       0xC0F
#define MSR_RSP                                                       0xC10
#define MSR_RBX                                                       0xC11
#define MSR_RDX                                                       0xC12
#define MSR_RCX                                                       0xC13
#define MSR_RAX                                                       0xC14
#define MSR_R8                                                        0xC15
#define MSR_R9                                                        0xC16
#define MSR_R10                                                       0xC17
#define MSR_R11                                                       0xC18
#define MSR_R12                                                       0xC19
#define MSR_R13                                                       0xC1A
#define MSR_R14                                                       0xC1B
#define MSR_R15                                                       0xC1C
#define MSR_EVENT_CTL_HLT_IO                                          0xC1F
#define MSR_SMBASE                                                    0xC20
#define MSR_SMM_REVID                                                 0xC21
#define MSR_IEDBASE                                                   0xC22
#define MSR_EPTP_ENABLE                                               0xC23
#define MSR_EPTP                                                      0xC24
#define MSR_LDTR_BASE                                                 0xC2C
#define MSR_IDTR_BASE                                                 0xC2D
#define MSR_GDTR_BASE                                                 0xC2E
#define MSR_CR4                                                       0xC37
#define MSR_IO_RSI                                                    0xC40
#define MSR_IO_RCX                                                    0xC41
#define MSR_IO_RIP                                                    0xC42
#define MSR_IO_RDI                                                    0xC43
#define MSR_BC_PBEC                                                   0x139
#define B_STOP_PBET                                                   BIT0

#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_NEM_INIT                                                    BIT0
#define V_TPM_PRESENT_MASK                                            0x06
#define V_TPM_PRESENT_NO_TPM                                          0
#define V_TPM_PRESENT_DTPM_12                                         1
#define V_TPM_PRESENT_DTPM_20                                         2
#define V_TPM_PRESENT_PTT                                             3
#define B_TPM_SUCCESS                                                 BIT3
#define B_MEASURED                                                    BIT5
#define B_VERIFIED                                                    BIT6
#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                   0x0000000F
#define CPUID_FULL_FAMILY_MODEL               0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_STEPPING      0x0FFF0FFF
#define CPUID_FULL_FAMILY_MODEL_HASWELL       0x000306C0
#define CPUID_FULL_FAMILY_MODEL_HASWELL_ULT   0x00040650
#define CPUID_FULL_FAMILY_MODEL_CRYSTALWELL   0x00040660

#define CPUID_PROCESSOR_TOPOLOGY              0xB

typedef enum {
  EnumCpuHsw     = CPUID_FULL_FAMILY_MODEL_HASWELL,
  EnumCpuHswUlt  = CPUID_FULL_FAMILY_MODEL_HASWELL_ULT,
  EnumCpuCrw     = CPUID_FULL_FAMILY_MODEL_CRYSTALWELL,
  EnumCpuMax     = CPUID_FULL_FAMILY_MODEL
} CPU_FAMILY;

typedef enum {
  ///
  /// Haswell Family Stepping
  ///
  EnumHswA0         = 1,
  EnumHswB0,
  EnumHswC0,
  EnumHswD0,
  ///
  /// Haswell ULT Family Stepping
  ///
  EnumHswUltB0      = 0,
  EnumHswUltC0,
  ///
  /// Crystalwell Family Stepping
  ///
  EnumCrwB0         = 0,
  EnumCrwC0,
  EnumCrwD0,
  ///
  /// Max Stepping
  ///
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;

typedef enum {
  EnumCpuUlt        = 0,
  EnumCpuTrad,
  EnumCpuUnknown
} CPU_SKU;

#endif
