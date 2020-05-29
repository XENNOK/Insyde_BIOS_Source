/** @file

  Server Platform Services (SPS) Policy for SPS ME.

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

#ifndef _SPS_POLICY_H_
#define _SPS_POLICY_H_

#pragma pack(1)

///
/// Revision definition
///  0x01 : Initial version
///
#define SPS_POLICY_REVISION            0x01

///
/// ME-BIOS Interface Version
///
typedef struct {
  UINT8                 Major;
  UINT8                 Minor;
} SPS_POLICY_ME_BIOS_INTERFACE_VERSION;


///
/// SPS SKU switch
///
typedef struct {
  BOOLEAN               SiliconEnabled;
  BOOLEAN               NodeManagerEnabled;
} SPS_POLICY_SKU_SWITCH;

///
/// Enable/Disable HECI Message
///   Set to TRUE this message can be send.
///   Set to FALSE this message can not send.
///
typedef struct {
  BOOLEAN  mPhySurvivability;
  BOOLEAN  DramInitDone;
  BOOLEAN  MeBiosInterfaceVersion;
  BOOLEAN  DynamicFusing;
  BOOLEAN  IccSetClockEnables;
  BOOLEAN  IccSetSpreadSpectrum;
  BOOLEAN  HostConfiguration;
  BOOLEAN  HmrfpoGetStatus;
  BOOLEAN  HmrfpoEnable;
  BOOLEAN  HmrfpoLock;
  BOOLEAN  EndOfPost;
} SPS_POLICY_MSG_SWITCH;

///
/// SPS ICC Clocl enables policy
/// Same as SPS_ICC_SET_CLOCK_ENABLES_BITS in SpsMe.h
///
#define SPS_POLICY_ICC_CLOCK_ENABLES_FLEX0     BIT0
#define SPS_POLICY_ICC_CLOCK_ENABLES_FLEX1     BIT1
#define SPS_POLICY_ICC_CLOCK_ENABLES_FLEX2     BIT2
#define SPS_POLICY_ICC_CLOCK_ENABLES_FLEX3     BIT3
#define SPS_POLICY_ICC_CLOCK_ENABLES_Reserved1 (BIT4 | BIT5 | BIT6)
#define SPS_POLICY_ICC_CLOCK_ENABLES_PCICLK0   BIT7
#define SPS_POLICY_ICC_CLOCK_ENABLES_PCICLK1   BIT8
#define SPS_POLICY_ICC_CLOCK_ENABLES_PCICLK2   BIT9
#define SPS_POLICY_ICC_CLOCK_ENABLES_PCICLK3   BIT10
#define SPS_POLICY_ICC_CLOCK_ENABLES_PCICLK4   BIT11
#define SPS_POLICY_ICC_CLOCK_ENABLES_Reserved2 (BIT12 | BIT13| BIT14| BIT15)
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC0      BIT16
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC1      BIT17
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC2      BIT18
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC3      BIT19
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC4      BIT20
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC5      BIT21
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC6      BIT22
#define SPS_POLICY_ICC_CLOCK_ENABLES_SRC7      BIT23
#define SPS_POLICY_ICC_CLOCK_ENABLES_ITPXDP    BIT24
#define SPS_POLICY_ICC_CLOCK_ENABLES_Reserved3 BIT25
#define SPS_POLICY_ICC_CLOCK_ENABLES_PEG_A     BIT26
#define SPS_POLICY_ICC_CLOCK_ENABLES_PEG_B     BIT27
#define SPS_POLICY_ICC_CLOCK_ENABLES_DMI       BIT28
#define SPS_POLICY_ICC_CLOCK_ENABLES_DMI2      BIT29  ///< For WellsBurg
#define SPS_POLICY_ICC_CLOCK_ENABLES_DP        BIT29  ///< For LynxPoint
#define SPS_POLICY_ICC_CLOCK_ENABLES_PEG_C     BIT30  ///< For WellsBurg
#define SPS_POLICY_ICC_CLOCK_ENABLES_DPNS      BIT30  ///< For LynxPoint
#define SPS_POLICY_ICC_CLOCK_ENABLES_Modulator4Enable BIT31

typedef union _SPS_POLICY_ICC_CLOCK_ENABLES {
  UINT32 UInt32;
  struct {
    UINT32 FLEX0     :1;
    UINT32 FLEX1     :1;
    UINT32 FLEX2     :1;
    UINT32 FLEX3     :1;
    UINT32 Reserved1 :3;
    UINT32 PCICLK0   :1;
    UINT32 PCICLK1   :1;
    UINT32 PCICLK2   :1;
    UINT32 PCICLK3   :1;
    UINT32 PCICLK4   :1;
    UINT32 Reserved2 :4;
    UINT32 SRC0      :1;
    UINT32 SRC1      :1;
    UINT32 SRC2      :1;
    UINT32 SRC3      :1;
    UINT32 SRC4      :1;
    UINT32 SRC5      :1;
    UINT32 SRC6      :1;
    UINT32 SRC7      :1;
    UINT32 ITPXDP    :1;
    UINT32 Reserved3 :1;
    UINT32 PEG_A     :1;
    UINT32 PEG_B     :1;
    UINT32 DMI       :1;
    UINT32 DP        :1;
    UINT32 DPNS      :1;
    UINT32 Modulator4Enable :1;
  } LynxPoint;
  struct {
    UINT32 FLEX0     :1;
    UINT32 FLEX1     :1;
    UINT32 FLEX2     :1;
    UINT32 FLEX3     :1;
    UINT32 Reserved1 :3;
    UINT32 PCICLK0   :1;
    UINT32 PCICLK1   :1;
    UINT32 PCICLK2   :1;
    UINT32 PCICLK3   :1;
    UINT32 PCICLK4   :1;
    UINT32 Reserved2 :4;
    UINT32 SRC0      :1;
    UINT32 SRC1      :1;
    UINT32 SRC2      :1;
    UINT32 SRC3      :1;
    UINT32 SRC4      :1;
    UINT32 SRC5      :1;
    UINT32 SRC6      :1;
    UINT32 SRC7      :1;
    UINT32 ITPXDP    :1;
    UINT32 Reserved3 :1;
    UINT32 PEG_A     :1;
    UINT32 PEG_B     :1;
    UINT32 DMI       :1;
    UINT32 DMI2      :1;
    UINT32 PEG_C     :1;
    UINT32 Modulator4Enable :1;
  } WellsBurg;
} SPS_POLICY_ICC_CLOCK_ENABLES;

typedef SPS_POLICY_ICC_CLOCK_ENABLES   SPS_POLICY_ICC_CLOCK_ENABLES_MASK;

///
/// SPS ICC spread spectrum mode select policy
/// Same as SPS_ICC_SPREAD_SPECTRUM_SELECT in SpsMe.h
///
typedef union _SPS_POLICY_ICC_SPREAD_SPECTRUM_SELECT {
  UINT32 UInt32;
  struct {
    UINT32 SSC1ModeSelect : 4;
    UINT32 SSC2ModeSelect : 4;
    UINT32 SSC3ModeSelect : 4;
    UINT32 SSC4ModeSelect : 4;
    UINT32 SSC5ModeSelect : 4;
    UINT32 SSC6ModeSelect : 4;
    UINT32 SSC7ModeSelect : 4;
    UINT32 SSC8ModeSelect : 4;
  } Field;
} SPS_POLICY_ICC_SPREAD_SPECTRUM_SELECT;

///
/// SPS_POLICY_ICC_SPREAD_SPECTRUM_SELECT::IccSSMSelect Values
/// Selects spread spectrum mode for 8 SSC outputs.
///   0000b - Down Spread
///   0001b - Reserved
///   0010b - Reserved
///   0011b - Disable - Power off SSC# and select bypass path to SSC# output
///   0100b - Do Not Change - SSC# settings from Intel/OEM Record will be applied
///
#define SPS_POLICY_ICC_SSC_MODE_DOWN_SPREAD 0x0
#define SPS_POLICY_ICC_SSC_MODE_DISABLE     0x3
#define SPS_POLICY_ICC_SSC_MODE_NOT_CHANGE  0x4

///
/// Response Mode
///   0 - Response is expected
///       If BIOS sets this bit to 0 then it must wait for response.
///   1 - No response is expected
///       In this mode, Intel(R) ME would not respond to the command and
///       BIOS would not wait for a response, and cannot know if the command failed.
///       BIOS should use this mode in production platforms
///       only after it was tested with full response for this command.
///
#define SPS_POLICY_ICC_RESPONSE_MODE_WAIT    0
#define SPS_POLICY_ICC_RESPONSE_MODE_SKIP    1

typedef struct _SPS_POLICY_ICC_SETTING {
  SPS_POLICY_ICC_CLOCK_ENABLES              IccClockEnables;
  SPS_POLICY_ICC_CLOCK_ENABLES_MASK         IccClockEnablesMask;
  BOOLEAN                                   IccClockEnablesSkipResponse;
  SPS_POLICY_ICC_SPREAD_SPECTRUM_SELECT     IccSpreadSpectrum;
  BOOLEAN                                   IccSpreadSpectrumSkipResponse;
} SPS_POLICY_ICC_SETTING;

///
/// Used for Host Configuration Message.
/// Same as SPS_NM_HOST_CONFIGURATION_DATA in SpsMe.h
///
typedef struct {
  UINT16 Capabilities;       ///< Bit coded platform capabilities
  UINT8  PStatesNumber;      ///< Number of supported P-states.
  UINT8  TStatesNumber;      ///< Number of supported T-states.
  UINT16 MaxPower;           ///< Maximum power in watts that can be consumed by the platform, zero if unknown.
  UINT16 MinPower;           ///< Minimum power in watts that can be consumed by the platform when idle, zero if unknown.
  UINT8  ProcNumber;         ///< The number of processors (packages)installed in the system during platform boot.
  UINT8  ProcCoresNumber;    ///< Number of all processor cores in all installed processor packages.
  UINT8  ProcCoresEnabled;   ///< Number of all currently enabled processor cores in all installed processor packages.
  UINT8  ProcThreadsEnabled; ///< Number of all currently enabled hardware threads, logical processors, in all installed processor packages.
  UINT64 TurboRatioLimit;    ///< Turbo Ratio Limit MSR, MSR 1ADh for BSP processor.
  UINT64 PlatformInfo;       ///< Platform Info MSR, MSR CEh for BSP processor.
  UINT32 Altitude;           ///< The altitude of the platform location above the see level, expressed in meters.
  UINT8  PStatesRatio[16];   ///< Each byte contains a ratio for a P-state with the corresponding index, starting from P0.
} SPS_POLICY_HOST_INFO;

///
/// SPS_POLICY_HOST_INFO::Capabilities Values
/// Bit coded platform capabilities
///
#define SPS_POLICY_HOST_INFO_CAP_TURBO                BIT0
#define SPS_POLICY_HOST_INFO_CAP_SMI_OPTIMIZED        BIT1
#define SPS_POLICY_HOST_INFO_CAP_POWER_MEASUREMENT    BIT2
#define SPS_POLICY_HOST_INFO_CAP_HADRWARE_CHANGE      BIT3

///
/// Used for ME Firmware Upgrade Configure.
///
#define SPS_ME_FW_UPGRADED_CMOS_SIG    0xF1   ///< V_ME_GLOBAL_RESET

typedef struct {
  ///
  ///  SPS ME Firmware Upgrade support switch
  ///    TRUE  : Firmware Upgrade  Supported
  ///    FALSE : Firmware Upgrade  Unsupported
  ///
  BOOLEAN     UpgradeSupported;

  ///
  ///  Protect descriptor region through SPI protect register with SPS ME firmware production mode
  ///    TRUE  : Descriptor region Protected by SPS.
  ///    FALSE : Descriptor region not Protected by SPS.
  ///
  BOOLEAN     ProtectDescriptorRegion;

  ///
  ///  Protect ME region through SPI protect register, except operational partitions.
  ///  If BIOS needs supporting "safe update entire ME", please disable this.
  ///    TRUE  : ME region Protected by SPS.
  ///    FALSE : ME region not Protected by SPS.
  ///
  BOOLEAN     ProtectMeRegion;

  ///
  ///  Protect GBE region through SPI protect register with SPS ME firmware production mode
  ///    TRUE  : GBE region Protected by SPS.
  ///    FALSE : GBE region not Protected by SPS.
  ///
  BOOLEAN     ProtectGbeRegion;

  ///
  ///  Protect Platform Data Regione(PDR) region through SPI protect register with SPS ME firmware production mode
  ///    TRUE  : PDR region Protected by SPS.
  ///    FALSE : PDR region not Protected by SPS.
  ///
  BOOLEAN     ProtectPDRRegion;

  ///
  ///  Protect Device Expansion Region(DER) region through SPI protect register with SPS ME firmware production mode
  ///    TRUE  : DER region Protected by SPS.
  ///    FALSE : DER region not Protected by SPS.
  ///
  BOOLEAN     ProtectDERRegion;

  ///
  ///  Lock SPI registers for security
  ///    TRUE  : SPI registers will Lock
  ///    FALSE : SPI registers not Lock
  ///
  BOOLEAN     SpiLock;

  ///
  ///  SPS ME Firmware Upgrade flag in CMOS.
  ///  CmosIndexPort : Index port of CMOS where storing the ME upgraded flag.
  ///  CmosDataPort  : Data  port of CMOS where storing the ME upgraded flag.
  ///  CmosOffset    : Offset     of CMOS where storing the ME upgraded flag.
  ///
  UINT8       CmosIndexPort;
  UINT8       CmosDataPort;
  UINT8       CmosOffset;

  ///
  ///  SPS ME Firmware Factory Default size.
  ///  Include FPT, MFSB, FTPR, MFS and SCA.
  ///  For recovery mode.
  ///
  UINT32      FactoryDefaultSize;
  ///
  ///  SPS ME Firmware Operation Image size.
  ///  For recovery mode.
  ///
  UINT32      OperationImageSize;

  ///
  ///  SPS ME Firmware Upgrade Block mode.
  ///    TURE  : Mulit SMI
  ///    FALSE : One SMI.
  ///
  BOOLEAN     BlockMode;

  ///
  ///  SPS ME Firmware Downgrade suppoer.
  ///    TURE  : Firmware Downgrade support.
  ///    FALSE : Firmware Downgrade Unsupport.
  ///
  BOOLEAN     Downgrade;
} SPS_POLICY_UPGRADE_CONFIG;

typedef struct _SPS_POLICY {
  UINT8                                Revision;
  SPS_POLICY_ME_BIOS_INTERFACE_VERSION MeBiosIntfVer;

  SPS_POLICY_SKU_SWITCH                SkuSwitch;
  SPS_POLICY_MSG_SWITCH                MsgSwitch;

  SPS_POLICY_ICC_SETTING               IccSetting;
  SPS_POLICY_HOST_INFO                 HostInfo;

  SPS_POLICY_UPGRADE_CONFIG            MeFwUpgradeConfig;

  UINT16                               mPhyTableVer;
  UINT16                               mPhyTableLen;
  UINT8                                mPhyTableData[512];
} SPS_POLICY;

#pragma pack()

#endif
