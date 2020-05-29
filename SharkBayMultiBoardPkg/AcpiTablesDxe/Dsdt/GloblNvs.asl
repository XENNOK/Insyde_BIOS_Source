/** @file
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/



  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.

  OperationRegion(GNVS,SystemMemory,0x55AA55AA, 0x55AA55AA)
  Field(GNVS,AnyAcc,Lock,Preserve)
  {
  Offset(0),    // Miscellaneous Dynamic Registers:
  OSYS, 16,     //   (000) Operating System
  SMIF, 8,      //   (002) SMI Function Call (ASL to SMI via I/O Trap)
  PRM0, 8,      //   (003) SMIF - Parameter 0
  PRM1, 8,      //   (004) SMIF - Parameter 1
  SCIF, 8,      //   (005) SCI Function Call (SMI to ASL via _L00)
  PRM2, 8,      //   (006) SCIF - Parameter 0
  PRM3, 8,      //   (007) SCIF - Parameter 1
  LCKF, 8,      //   (008) Global Lock Function Call (EC Communication)
  PRM4, 8,      //   (009) LCKF - Parameter 0
  PRM5, 8,      //   (010) LCKF - Parameter 1
  P80D, 32,     //   (011) Port 80 Debug Port Value
  PWRS, 8,      //   (015) Power State (AC Mode = 1)
  DBGS, 8,      //   (016) Debug State
  //
  // Thermal Policy Registers:
  //
  THOF, 8,      //   (017) Enable Thermal Offset for KSC
  ACT1, 8,      //   (018) Active Trip Point 1
  ACTT, 8,      //   (019) Active Trip Point
  PSVT, 8,      //   (020) Passive Trip Point
  TC1V, 8,      //   (021) Passive Trip Point TC1 Value
  TC2V, 8,      //   (022) Passive Trip Point TC2 Value
  TSPV, 8,      //   (023) Passive Trip Point TSP Value
  CRTT, 8,      //   (024) Critical Trip Point
  DTSE, 8,      //   (025) Digital Thermal Sensor Enable
  DTS1, 8,      //   (026) Digital Thermal Sensor 1 Reading
  DTS2, 8,      //   (027) Digital Thermal Sensor 2 Reading
  DTSF, 8,      //   (028) DTS SMI Function Call 
  Offset(30),   // Battery Support Registers: (Moved outside this ASL code but still in used)
// ! ACPI RC code will define below 7 fields in this offset!
//  BNUM, 8,    //   (030) Battery Number Present
//  B0SC, 8,    //   (031) Battery 0 Stored Capacity
//  B1SC, 8,    //   (032) Battery 1 Stored Capacity
//  B2SC, 8,    //   (033) Battery 2 Stored Capacity
//  B0SS, 8,    //   (034) Battery 0 Stored Status
//  B1SS, 8,    //   (035) Battery 1 Stored Status
//  B2SS, 8,    //   (036) Battery 2 Stored Status
  Offset(37),   // Revision Field:
  REVN, 8,      //   (037) Revison of GlobalNvsArea
  Offset(40),   // CPU Identification Registers:
  //
  // Processor Configuration Values
  //
  APIC, 8,      //   (040) APIC Enabled by SBIOS (APIC Enabled = 1)
  TCNT, 8,      //   (041) Number of Enabled Threads
  PCP0, 8,      //   (042) PDC Settings, Processor 0
  PCP1, 8,      //   (043) PDC Settings, Processor 1
  PPCM, 8,      //   (044) Maximum PPC state
  PPMF, 32,     //   (045) PPM Flags (Same as CFGD)
  C67L, 8,      //   (049) C6/C7 Entry/Exit latency
  //
  // SIO Configuration Values
  //
  NATP, 8,      //   (050) National SIO Present
  CMAP, 8,      //   (051)   COM A Port
  CMBP, 8,      //   (052)   COM B Port
  LPTP, 8,      //   (053)   LPT Port
  FDCP, 8,      //   (054)   FDC Port
  CMCP, 8,      //   (055) SMSC Com Port
  CIRP, 8,      //   (056) SMSC Com CIR Port
  SMSC, 8,      //   (057) SMSC1007 SIO Present
  W381, 8,      //   (058) WPCN381U SIO Present
  SMC1, 8,      //   (059) SMSC1000 SIO Present  
  //
  // Extended Mobile Access Values
  //
  EMAE, 8,      //   (060) EMA Enable
  EMAP, 16,     //   (061) EMA Pointer   
  EMAL, 16,     //   (063) EMA Length
  Offset(66),   // 
  //
  // Mobile East Fork Values
  //
  MEFE, 8,      //   (066) MEF Enable
  //
  // PCIe Dock Status
  //
  DSTS, 8,      //   (067) PCIe Dock Status
  Offset(72),   // 
  //
  // TPM Registers
  //
  MORD, 8,      //   (072) Memory Overwrite Request Data
  TCGP, 8,      //   (073) Used for save the Mor and/or physical presence paramter
  PPRP, 32,     //   (074) Physical Presence request operation response
  PPRQ, 8,      //   (078) Physical Presence request operation
  LPPR, 8,      //   (079) Last Physical Presence request operation
  //
  // SATA Values
  //
  GTF0, 56,     //   (080) GTF Task File Buffer for Port 0
  GTF2, 56,     //   (087) GTF Task File Buffer for Port 2
  IDEM, 8,      //   (094) IDE Mode (Compatible\Enhanced)
  GTF1, 56,     //   (095) GTF Task File Buffer for Port 1
  //
  // Board Id
  // This field is for the ASL code to know whether this board is Matanzas, or Oakmont, etc
  //
  BID,  16,     //   (102) Platform board id
  PLID, 8,      //   (104) Platform id 
  Offset(112),
  OSCC, 8,      //   (112) PCIE OSC Control
  NEXP, 8,      //   (113) Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  SBV1, 8,      //   (114) USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  SBV2, 8,      //   (115) USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  Offset(122),  // Global Variables
  DSEN, 8,      //   (122) _DOS Display Support Flag.
  Offset(124),  //   (123) Embedded Controller Availability Flag.
  //
  // Use ECON of OemGlobalNvs.asl
  //
  GPIC, 8,      //   (124) Global IOAPIC/8259 Interrupt Mode Flag.
  CTYP, 8,      //   (125) Global Cooling Type Flag.
  L01C, 8,      //   (126) Global L01 Counter.
  VFN0, 8,      //   (127) Virtual Fan0 Status.
  VFN1, 8,      //   (128) Virtual Fan1 Status.
  VFN2, 8,      //   (129) Virtual Fan2 Status.
  VFN3, 8,      //   (130) Virtual Fan3 Status.
  VFN4, 8,      //   (131) Virtual Fan4 Status. 
  VFN5, 8,      //   (132) Virtual Fan5 Status.
  VFN6, 8,      //   (133) Virtual Fan6 Status.
  VFN7, 8,      //   (134) Virtual Fan7 Status.
  VFN8, 8,      //   (135) Virtual Fan8 Status.
  VFN9, 8,      //   (136) Virtual Fan9 Status.
  Offset(143),
  //
  // Thermal
  //
  ATMC, 8,      //   (143) Active Trip Point for MCH
  PTMC, 8,      //   (144) Passive Trip Point for MCH
  ATRA, 8,      //   (145) Active Trip Point for TMEM
  PTRA, 8,      //   (146) Passive Trip Point for TMEM
  PNHM, 32,     //   (147) CPUID Feature Information [EAX]
  TBAB, 32,     //   (151) Thermal Base Low Address for BIOS
  TBAH, 32,     //   (155) Thermal Base High Address for BIOS
  RTIP, 8,      //   (159) Run Time Interface for Intelligent Power Savings
  TSOD, 8,      //   (160) TS-on-DIMM is chosen in SETUP and present on the DIMM
  ATPC, 8,      //   (161) Active Trip Point for PCH
  PTPC, 8,      //   (162) Passive Trip Point for PCH
  //
  // Board info
  //
  PFLV, 8,      //   (163) Platform Flavor
  BREV, 8,      //   (164) Board Rev
  //
  // Package temperature
  //
  PDTS, 8,      //   (165) Package Temperature
  PKGA, 8,      //   (166) Package Temperature MSR available
  PAMT, 8,      //   (167) Peci Access Method
  AC0F, 8,      //   (168) _AC0 Fan Speed
  AC1F, 8,      //   (169) _AC1 Fan Speed 
  DTS3, 8,      //   (170) Digital Thermal Sensor 3 Reading
  DTS4, 8,      //   (171) Digital Thermal Sensor 4 Reading
  Offset(176),  //   (172):(175) are reserved for future use
  // LtrEnable[8]
  LTR1, 8,      //   (176) Latency Tolerance Reporting Enable
  LTR2, 8,      //   (177) Latency Tolerance Reporting Enable
  LTR3, 8,      //   (178) Latency Tolerance Reporting Enable
  LTR4, 8,      //   (179) Latency Tolerance Reporting Enable
  LTR5, 8,      //   (180) Latency Tolerance Reporting Enable
  LTR6, 8,      //   (181) Latency Tolerance Reporting Enable
  LTR7, 8,      //   (182) Latency Tolerance Reporting Enable
  LTR8, 8,      //   (183) Latency Tolerance Reporting Enable
  // ObffEnable[8]
  OBF1, 8,      //   (184) Optimized Buffer Flush and Fill
  OBF2, 8,      //   (185) Optimized Buffer Flush and Fill
  OBF3, 8,      //   (186) Optimized Buffer Flush and Fill
  OBF4, 8,      //   (187) Optimized Buffer Flush and Fill
  OBF5, 8,      //   (188) Optimized Buffer Flush and Fill
  OBF6, 8,      //   (189) Optimized Buffer Flush and Fill
  OBF7, 8,      //   (190) Optimized Buffer Flush and Fill
  OBF8, 8,      //   (191) Optimized Buffer Flush and Fill
  XHCI, 8,      //   (192) xHCI controller mode
  //
  // XTU 3.0 Specification
  //
  XTUB, 32,     //   (193) XTU Continous structure Base Address 
  XTUS, 32,     //   (197) XMP Size
  XMPB, 32,     //   (201) XMP Base Address
  DDRF, 8,      //   (205) DDR Reference Frequency
  RTD3, 8,      //   (206) Runtime D3 support.
  PEP0, 8,      //   (207) User selctable Delay for Device D0 transition.
  PEP3, 8,      //   (208) User selctable Delay for Device D3 transition.
  //
  // DPTF Devices and trip points
  //
  DPTF, 8,      //   (209) EnableDptf

  SADE, 8,      //   (210) EnableSaDevice
  SACR, 8,      //   (211) CriticalThermalTripPointSa
  SAHT, 8,      //   (212) HotThermalTripPointSa

  PCHD, 8,      //   (213) EnablePchDevice
  PCHC, 8,      //   (214) CriticalThermalTripPointPch
  PCHH, 8,      //   (215) HotThermalTripPointPch
  //
  // DPTF Policies
  //
  CTDP, 8,      //   (216) EnableCtdpPolicy
  LPMP, 8,      //   (217) EnableLpmPolicy
  LPMV, 8,      //   (218) CurrentLowPowerMode for LPM
  ECEU, 8,      //   (219) EnableCurrentExecutionUnit
  TGFG, 16,     //   (220) TargetGfxFreq
  //
  // DPPM Devices and trip points
  //
  MEMD, 8,      //   (222) EnableMemoryDevice
  MEMC, 8,      //   (223) CriticalThermalTripPointTMEM
  MEMH, 8,      //   (224) HotThermalTripPointTMEM

  FND1, 8,      //   (225) EnableFan1Device
  FND2, 8,      //   (226) EnableFan2Device

  AMBD, 8,      //   (227) EnableAmbientDevice
  AMAT, 8,      //   (228) ActiveThermalTripPointAmbient
  AMPT, 8,      //   (229) PassiveThermalTripPointAmbient
  AMCT, 8,      //   (230) CriticalThermalTripPointAmbient
  AMHT, 8,      //   (231) HotThermalTripPointAmbient

  SKDE, 8,      //   (232) EnableSkinDevice
  SKAT, 8,      //   (233) ActiveThermalTripPointSkin
  SKPT, 8,      //   (234) PassiveThermalTripPointSkin
  SKCT, 8,      //   (235) CriticalThermalTripPointSkin
  SKHT, 8,      //   (236) HotThermalTripPointSkin

  EFDE, 8,      //   (237) EnableExhaustFanDevice
  EFAT, 8,      //   (238) ActiveThermalTripPointExhaustFan
  EFPT, 8,      //   (239) PassiveThermalTripPointExhaustFan
  EFCT, 8,      //   (240) CriticalThermalTripPointExhaustFan
  EFHT, 8,      //   (241) HotThermalTripPointExhaustFan

  VRDE, 8,      //   (242) EnableVRDevice
  VRAT, 8,      //   (243) ActiveThermalTripPointVR
  VRPT, 8,      //   (244) PassiveThermalTripPointVR
  VRCT, 8,      //   (245) CriticalThermalTripPointVR
  VRHT, 8,      //   (246) HotThermalTripPointVR
  //
  // DPPM Policies
  //
  DPAP, 8,      //   (247) EnableActivePolicy
  DPPP, 8,      //   (248) EnablePassivePolicy
  DPCP, 8,      //   (249) EnableCriticalPolicy
  DCMP, 8,      //   (250) EnableCoolingModePolicy
  TRTV, 8,      //   (251) TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  LPOE, 8,      //   (252) LPOEnable
  LPOP, 8,      //   (253) LPOStartPState
  LPOS, 8,      //   (254) LPOStepSize
  LPOW, 8,      //   (255) LPOPowerControlSetting
  LPER, 8,      //   (256) LPOPerformanceControlSetting
  //
  // Miscellaneous DPTF
  //
  PPSZ, 32,     //   (257) PPCC Step Size
  DISE, 8,      //   (261) EnableDisplayParticipant
  //
  // PFAT
  //
  PFMA,  64,    //   (262) PFAT Memory Address for Tool Interface
  PFMS,  8,     //   (270) PFAT Memory Size for Tool Interface
  PFIA,  16,    //   (271) PFAT IoTrap Address for Tool Interface
  //
  // Smart Connect Technology
  //
  ICNF, 8,      //   (273) Isct Configuration
  //
  // Audio DSP
  //
  DSP0, 32,     //   (274) Audio DSP BAR0
  DSP1, 32,     //   (278) Audio DSP BAR1
  //
  // NFC
  //
  NFCE, 8,      //   (282) NFC module selection 	
  //
  // ADSP Codec Selection
  //
  CODS, 8,      //   (283) Audio Codec selection
  //
  // Sensor Hub Enable
  //
  SNHE, 8,      //   (284) Sensor Hub Enable
  S0ID, 8,      //   (285) Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  CTDB, 8,      //   (286) enable/disable BIOS only version of Config TDP

  Offset (553),
  PWRE, 8,      //   (553) EnablePowerDevice
  PWRP, 8,      //   (554) EnablePowerPolicy
  XHPR, 8,      //   (555) RTD3 USB Power Resource config
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  SDS0, 8,      //   (556) I2C0 Sensor Device Selection
  SDS1, 8,      //   (557) I2C1 Sensor Device Selection
  SDS2, 8,      //   (558) SPI0 Sensor Device Selection
  SDS3, 8,      //   (559) SPI1 Sensor Device Selection
  SDS4, 8,      //   (560) UART0 Sensor Device Selection
  SDS5, 8,      //   (561) UART1 Sensor Device Selection
  Offset(563),  //   (562) no longer used
  RIC0, 8,      //   (563)
  PEPY, 8,      //   (564) RTD3 PEP support list(BIT0 - GFx , BIT1 - Sata, BIT2 - UART, BIT3 - SDHC, Bit4 - I2C0, BIT5 - I2C1, Bit6 - XHCI, Bit7 - Audio)
  Offset(600),
  // Common_Part_Start
  CCMD, 8,      // (600) ComPortCMode;
  COMD, 8,      // (601) ComPortD;
  LPT1, 8,      // (602) Lpt1;
  PSTP, 8,      // (603) PSType;
  WKMD, 8,      // (604) WakeOnModemRing;  
  // Common_Part_End  
  // Chipset_Start  
  IDER, 8,      // (605) IdeREnable
  PIE0, 8,      // (606) PcieRootPortPmeInt0;
  PIE1, 8,      // (607) PcieRootPortPmeInt1;
  CSTS, 8,      // (608) CStates;
  PMEE, 8,      // (609) PME_EN: Power Management Enable
  WOLE, 8,      // (610) Wake On Lan Eanble
  NVAD, 32,     // (611:614) NVBIOSAddr;
  NVSZ, 32,     // (615:618) NVBIOSSize;
  OPTF, 8,      // (619) OptimusFlag
  IFSE, 8,      // (620) RapidStart enable
  ISCT, 8,      // (621) ISCT flag 
  S1C0, 16,     // (622:623) SATA1Channel0PioTiming
  S1C1, 16,     // (624:625) SATA1Channel1PioTiming
  S2C0, 16,     // (626:627) SATA2Channel0PioTiming
  S2C1, 16,     // (628:629) SATA2Channel1PioTiming        
  SLPS, 8,      // (630) SleepState
  ALAT, 32,     // (631) Alarm Time
  FGEN, 8,      // (635) Full Generic Participant
  HRAM, 8,      // (636) 
  MDEN, 8,      // (637)  EnableACPIDebug
  TBEP, 8,      // (638) Thunderbolt End Point Number
  // Chipset_End
  DVS0, 8,      // (639) Port0 DevSlp Enable
  DVS1, 8,      // (640) Port1 DevSlp Enable
  DVS2, 8,      // (641) Port2 DevSlp Enable
  DVS3, 8,      // (642) Port3 DevSlp Enable
  GBSX, 8,      // (643) Virtual GPIO button Notify Sleep State Change
  IUBE, 8,      // (644) IUER Button Enable
  IUCE, 8,      // (645) IUER Convertible Enable
  IUDE, 8,      // (646) IUER Dock Enable
  ECNO, 8,      // (647) EC Notification of Low Power S0 Idle State
  AUDD, 16,     // (648) RTD3 Audio Codec device delay
  DSPD, 16,     // (650) RTD3 ADSP device delay
  IC0D, 16,     // (652) RTD3 SensorHub delay time after applying power to device
  IC1D, 16,     // (654) RTD3 TouchPanel delay time after applying power to device
  IC1S, 16,     // (656) RTD3 TouchPad delay time after applying power to device
  VRRD, 16,     // (658) VR Ramp up delay
  PSCP, 8,      // (660) P-state Capping
  RWAG, 8,      // (661) Rtd3 W/A Gpio, allow W/A for port 1 and 6 to use GPIO from SDHC device
  I20D, 16,     // (662) Delay in _PS0 after powering up I2C0 Controller
  I21D, 16,     // (664) Delay in _PS0 after powering up I2C1 Controller
  Offset(668),

  RCG0, 8,      // (668) RTD3 Config Setting(BIT0:ZPODD,BIT1:USB Camera Port4, BIT2/3:SATA Port3)
  ECDB, 8,      // (669) EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  P2ME, 8,      // (670) Ps2 Mouse Enable
  SSH0, 16,     // (671) SSCN-LOW  for I2C0
  SSL0, 16,     // (673) SSCN-HIGH for I2C0
  SSD0, 16,     // (675) SSCN-HOLD for I2C0
  FMH0, 16,     // (677) FMCN-LOW  for I2C0
  FML0, 16,     // (679) FMCN-HIGH for I2C0
  FMD0, 16,     // (681) FMCN-HOLD for I2C0
  FPH0, 16,     // (683) FPCN-LOW  for I2C0
  FPL0, 16,     // (685) FPCN-HIGH for I2C0
  FPD0, 16,     // (687) FPCN-HOLD for I2C0
  SSH1, 16,     // (689) SSCN-LOW  for I2C1
  SSL1, 16,     // (691) SSCN-HIGH for I2C1
  SSD1, 16,     // (693) SSCN-HOLD for I2C1
  FMH1, 16,     // (695) FMCN-LOW  for I2C1
  FML1, 16,     // (697) FMCN-HIGH for I2C1
  FMD1, 16,     // (699) FMCN-HOLD for I2C1
  FPH1, 16,     // (701) FPCN-LOW  for I2C1
  FPL1, 16,     // (703) FPCN-HIGH for I2C1
  FPD1, 16,     // (705) FPCN-HOLD for I2C1
  M0C0, 16,     // (707) M0D3 for I2C0
  M1C0, 16,     // (709) M1D3 for I2C0
  M2C0, 16,     // (711) M0D0 for I2C0
  M0C1, 16,     // (713) M0D3 for I2C1
  M1C1, 16,     // (715) M1D3 for I2C1
  M2C1, 16,     // (717) M0D0 for I2C1
  M0C2, 16,     // (719) M0D3 for SPI0
  M1C2, 16,     // (721) M1D3 for SPI0
  M0C3, 16,     // (723) M0D3 for SPI1
  M1C3, 16,     // (725) M1D3 for SPI1
  M0C4, 16,     // (727) M0D3 for UA00
  M1C4, 16,     // (729) M1D3 for UA00
  M0C5, 16,     // (731) M0D3 for UA01
  M1C5, 16,     // (733) M1D3 for UA01
  TBSF, 8,      // (735) ThunderBolt SMI Function Number
  GIRQ, 32,     // (736:739) GPIO IRQ
  DMTP, 8,      // (740) PIRQS 34,50(GPIO)
  DMTD, 8,      // (741) PIRQX 39,55(GPIO)
  DMSH, 8,      // (742) PIRQM 28,14(GPIO)
  LANP, 8,      // (743) LAN PHY Status 0 = Not Present, 1 = Present
  Offset(745),  // 744 
  SHSB, 8,      // (745) Sensor Standby mode
  PLCS, 8,      // (746) set PL1 limit when entering CS
  PLVL, 16,     // (747) PL1 limit value
  GN1E, 8,      // (749) EnableGen1Participant
  G1AT, 8,      // (750) ActiveThermalTripPointGen1
  G1PT, 8,      // (751) PassiveThermalTripPointGen1
  G1CT, 8,      // (752) CriticalThermalTripPointGen1
  G1HT, 8,      // (753) HotThermalTripPointGen1
  GN2E, 8 ,     // (754) EnableGen2Participant
  G2AT, 8,      // (755) ActiveThermalTripPointGen2
  G2PT, 8,      // (756) PassiveThermalTripPointGen2
  G2CT, 8,      // (757) CriticalThermalTripPointGen2
  G2HT, 8,      // (758) HotThermalTripPointGen2
  WWSD, 8,      // (759) EnableWwanTempSensorDevice
  CVSD, 8,      // (760) EnableCpuVrTempSensorDevice
  SSDD, 8,      // (761) EnableSsdTempSensorDevice
  INLD, 8,      // (762) EnableInletFanTempSensorDevice
  IFAT, 8,      // (763) ActiveThermalTripPointInletFan
  IFPT, 8,      // (764) PassiveThermalTripPointInletFan
  IFCT, 8,      // (765) CriticalThermalTripPointInletFan
  IFHT, 8,      // (766) HotThermalTripPointInletFan
  DOSD, 8,      // (767) DMA OS detection, 1 = check for OS version when enabling DMA, 0 = don't care about OS
  USBH, 8,      // (768) USB Sensor Hub Enable/Disable
  BCV4, 8,      // (769) Broadcom's Bluetooth adapter's revision
  WTV0, 8,      // (770) I2C0/WITT devices version
  WTV1, 8,      // (771) I2C1/WITT devices version
  APFU, 8,      // (772) Atmel panel FW update Enable/Disable
  SOHP, 8,      // (773) SMI on Hot Plug for TBT devices
  NOHP, 8,      // (774) Notify on Hot Plug for TBT devices
  TBSE, 8,      // (775) ThunderBolt Root port selector
  WKFN, 8,      // (776) WAK Finished
  PEPC, 16,     // (777) PEP Constraints
  VRSD, 16,     // (779) VR Staggering delay
  PB1E, 8,      // (781) 10sec Power button support Bit0: 10 sec P-button Enable/Disable
                //                                  Bit1: Internal Flag
                //                                  Bit2: Rotation Lock flag, 0:unlock, 1:lock
                //                                  Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
                //                                  Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
                //                                  Bit5, 6: reserved for future use.
                //                                  Bit7: EC 10sec PB Override state for S3/S4 wake up. 
//[-start-130222-IB03780481-add]//
  TBTC, 8,      // (782) Thunderbolt Chip Type (CR or RR)
  TRPN, 8,      // (783) Thunderbolt Root Port Number
  TBTM, 32,     // (784) Thunderbolt Host Bridge Memory Base
//[-end-130222-IB03780481-add]//
//[-start-130611-IB11120016-add]//
  IMIT, 8,      // (788) System Interface Type of BMC
  IMSV, 8,      // (789) IPMI Spec. Version of BMC
  IMBA, 16,     // (790) Base Address of BMC
  BMCS, 8,      // (792) BMC Status
  REOT, 8,      // (793) Command and Data port Offset of BMC
//[-end-130611-IB11120016-add]//
  WAND, 8,      // (794) EnableWWANParticipant
  WWAT, 8,      // (795) ActiveThermalTripPointWWAN
  WWPT, 8,      // (796) PassiveThermalTripPointWWAN
  WWCT, 8,      // (797) CriticalThermalTripPointWWAN
  WWHT, 8,      // (798) HotThermalTripPointWWAN
  ULTP, 8,      // (799) ULT Platform
  Offset(805),
  MPLT, 16,     // (805) Minimum Power Limit for DPTF use via PPCC Object
  GR13, 8,      // (807) GPIO13 Rework for Sawtooth Peak
//[-start-130205-IB02950497-modify]//  
  MAUL, 8,      // (808) Manual mode switch
  U2PR, 32,     // (809) Manual Mode Usb20 Per Pin Route
  U3SS, 8,      // (813) Manual Mode Usb20 Per Pin Route
//[-end-130205-IB02950497-modify]//    
  SPST, 8,      // (814) SATA port state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  ECLP, 8,      // (815) EC Low Power Mode: 1 - Enabled, 0 - Disabled
//[-start-140521-IB05400527-add]//
  MM64, 8,      // (816) Enable/Disable 64bits MMIO support
  HMBE, 64,     // (817) 64 bits MMIO base address
  HMLM, 64,     // (825) 64 bits MMIO limit
  HMLE, 64      // (833) 64 bits MMIO length
//[-end-140521-IB05400527-add]//
  }
