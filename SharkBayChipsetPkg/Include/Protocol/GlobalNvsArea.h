/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to  
  the additional terms of the license agreement               
**/
/**

Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  GlobalNvsArea.h

@brief:

  Definition of the global NVS area protocol.  This protocol 
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM code and ASL code.
  The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

**/
#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

#include "OemGlobalNvsArea.h"

#define PRIMARY_DISPLAY                 1

//
// Includes
//
#define GLOBAL_NVS_DEVICE_ENABLE  1
#define GLOBAL_NVS_DEVICE_DISABLE 0

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL EFI_GLOBAL_NVS_AREA_PROTOCOL;
//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
  { \
    0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc \
  }
//
// Revision id - Added TPM related fields
//
#define GLOBAL_NVS_AREA_REVISION_1  1
//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

//
// Global NVS Area definition
//
#pragma pack(1)
typedef struct {
  //
  // Miscellaneous Dynamic Values, the definitions below need to be matched 
  // GNVS definitions in Platform.ASL
  //
  UINT16      OperatingSystem;  // 000
  UINT8       SmiFunction;      // 002   SMI function call via IO Trap
  UINT8       SmiParameter0;    // 003
  UINT8       SmiParameter1;    // 004
  UINT8       SciFunction;      // 005   SCI function call via _L00
  UINT8       SciParameter0;    // 006
  UINT8       SciParameter1;    // 007
  UINT8       GlobalLock;       // 008   Global lock function call
  UINT8       LockParameter0;   // 009
  UINT8       LockParameter1;   // 010
  UINT32      Port80DebugValue; // 011
  UINT8       PowerState;       // 015   AC = 1
  UINT8       DebugState;       // 016
  //
  // Thermal Policy Values
  //
  UINT8       Reserved01;                         // 017 
  UINT8       Ac1TripPoint;                       // 018
  UINT8       Ac0TripPoint;                       // 019
  UINT8       PassiveThermalTripPoint;            // 020
  UINT8       PassiveTc1Value;                    // 021
  UINT8       PassiveTc2Value;                    // 022
  UINT8       PassiveTspValue;                    // 023
  UINT8       CriticalThermalTripPoint;           // 024
  UINT8       EnableDigitalThermalSensor;         // 025   DTS Function enable
  UINT8       BspDigitalThermalSensorTemperature; // 026   Temperature of BSP
  UINT8       ApDigitalThermalSensorTemperature;  // 027   Temperature of AP
  UINT8       DigitalThermalSensorSmiFunction;    // 028   SMI function call via DTS IO Trap  
  UINT8       Reserved02;                         // 029
  //
  // Battery Support Values
  //
  UINT8       NumberOfBatteries;                  // 030
  UINT8       BatteryCapacity0;                   // 031   Battery 0 Stored Capacity
  UINT8       BatteryCapacity1;                   // 032   Battery 1 Stored Capacity
  UINT8       BatteryCapacity2;                   // 033   Battery 2 Stored Capacity
  UINT8       BatteryStatus0;                     // 034   Battery 0 Stored Status
  UINT8       BatteryStatus1;                     // 035   Battery 1 Stored Status
  UINT8       BatteryStatus2;                     // 036   Battery 2 Stored Status
  //
  // NOTE: Do NOT Change the Offset of Revision Field
  //
  UINT8       Revision;                                   // 037   Revision of the structure EFI_GLOBAL_NVS_AREA
  UINT8       Reserved03[2];                              // 038:039
  //
  // Processor Configuration Values
  //
  UINT8       ApicEnable;                                 // 040   APIC Enabled by SBIOS (APIC Enabled = 1)
  UINT8       ThreadCount;                                // 041   Number of Enabled Threads
  UINT8       CurentPdcState0;                            // 042   PDC settings, Processor 0
  UINT8       CurentPdcState1;                            // 043   PDC settings, Processor 1
  UINT8       MaximumPpcState;                            // 044   Maximum PPC state
  UINT32      PpmFlags;                                   // 045:048 PPM configuration flags, same as CFGD
  UINT8       C6C7Latency;                                // 049   C6/C7 Entry/Exit latency
  //
  // SIO Configuration Values
  //
  UINT8       DockedSioPresent;                           // 050   Dock SIO Present
  UINT8       DockComA;                                   // 051   COM A Port
  UINT8       DockComB;                                   // 052   COM B Port
  UINT8       DockLpt;                                    // 053   LPT Port
  UINT8       DockFdc;                                    // 054   FDC Port
  UINT8       OnboardCom;                                 // 055   Onboard COM Port
  UINT8       OnboardComCir;                              // 056   Onboard COM CIR Port
  UINT8       SMSC1007;                                   // 057
  UINT8       WPCN381U;                                   // 058
  UINT8       SMSC1000;                                   // 059
  //
  // Extended Mobile Access Values
  //
  UINT8       EmaEnable;                                  // 060  EMA Enable
  UINT16      EmaPointer;                                 // 061  EMA Pointer
  UINT16      EmaLength;                                  // 063  EMA Length
  UINT8       Reserved04[1];                              // 065
  //
  // Mobile East Fork Values
  //
  UINT8       MefEnable;                                  // 066 Mobile East Fork Enable
  //
  // PCIe Dock Status
  //
  UINT8       PcieDockStatus;                             // 067 PCIe Dock Status
  UINT8       Reserved05[4];                              // 068:071
  //
  // TPM Registers
  //
  UINT8       MorData;                                    // 072 Memory Overwrite Request Data
  UINT8       TcgParamter;                                // 073 Used for save the Mor and/or physical presence paramter
  UINT32      PPResponse;                                 // 074 Physical Presence request operation response
  UINT8       PPRequest;                                  // 078 Physical Presence request operation
  UINT8       LastPPRequest;                              // 079 Last Physical Presence request operation
  //
  // SATA Values
  //
  UINT8       GtfTaskFileBufferPort0[7];                  // 080  GTF Task File Buffer for Port 0
  UINT8       GtfTaskFileBufferPort2[7];                  // 087  GTF Task File Buffer for Port 2
  UINT8       IdeMode;                                    // 094  IDE Mode (Compatible\Enhanced)
  UINT8       GtfTaskFileBufferPort1[7];                  // 095:101 GTF Task File Buffer for Port 1
  //
  // Board Id
  // This field is for the ASL code to know whether this board is Matanzas, or Oakmont, etc
  //
  UINT16      BoardId;                                    // 102  Board Id
  UINT8       PlatformId;                                 // 104  Platform Id
  UINT8       Reserved06[7];                              // 105:111
  UINT8       PcieOSCControl;                             // 112  PCIE OSC Control
  UINT8       NativePCIESupport;                          // 113  Native PCI Express Support
  //
  // USB Sideband Deferring Support
  //
  UINT8       HostAlertVector1;                           // 114  GPE vector used for HOST_ALERT#1
  UINT8       HostAlertVector2;                           // 115  GPE vector used for HOST_ALERT#2
  UINT8       Reserved07[6];                              // 116:121
  UINT8       DOSSupportFlag;                             // 122 DSEN  _DOS Display Support Flag.
  UINT8       EcAvailable_disuse;                         // 123 Embedded Controller Availability Flag.
  //
  // Use OemEmbeddedControllerFlag of OemGlobalNvsArea.h
  //
  UINT8       GlobalIOAPICFlag;                           // 124 GPIC  Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8       GlobalCoolingTypeFlag;                      // 125 CTYP  Global Cooling Type Flag.
  UINT8       GlobalL01Counter;                           // 126 L01C  Global L01 Counter.
  UINT8       VirtualFan0Status;                          // 127 VFN0  Virtual Fan0 Status.
  UINT8       VirtualFan1Status;                          // 128 VFN1  Virtual Fan1 Status.
  UINT8       VirtualFan2Status;                          // 129 VFN2  Virtual Fan2 Status.
  UINT8       VirtualFan3Status;                          // 130 VFN3  Virtual Fan3 Status.
  UINT8       VirtualFan4Status;                          // 131 VFN4  Virtual Fan4 Status.
  UINT8       VirtualFan5Status;                          // 132 VFN5  Virtual Fan5 Status.
  UINT8       VirtualFan6Status;                          // 133 VFN6  Virtual Fan6 Status.
  UINT8       VirtualFan7Status;                          // 134 VFN7  Virtual Fan7 Status.
  UINT8       VirtualFan8Status;                          // 135 VFN8  Virtual Fan8 Status.
  UINT8       VirtualFan9Status;                          // 136 VFN9  Virtual Fan9 Status.
  UINT8       Reserved08[6];                             // 137:142
  //
  // Thermal
  //
  UINT8       ActiveThermalTripPointSA;                   // 143 Active Trip Point for System Agent
  UINT8       PassiveThermalTripPointSA;                  // 144 Passive Trip Point for System Agent
  UINT8       ActiveThermalTripPointTMEM;                 // 145 Active Trip Point for TMEM
  UINT8       PassiveThermalTripPointTMEM;                // 146 Passive Trip Point for TMEM
  UINT32      PlatformCpuId;                              // 147   Device ID 8
  UINT32      TBARB;                                      // 151 Thermal BAR for BIOS, TBAB in GloblNvs.asl
  UINT32      TBARBH;                                     // 155 Thermal BAR for BIOS, TBAH in GloblNvs.asl
  UINT8       RunTimeInterface;                           // 159 Run Time Interface for Intelligent Power Savings
  UINT8       TsOnDimmEnabled;                            // 160 TS-on-DIMM is chosen in SETUP and present on the DIMM
  UINT8       ActiveThermalTripPointPCH;                  // 161 Active Trip Point for PCH
  UINT8       PassiveThermalTripPointPCH;                 // 162 Passive Trip Point for PCH
  //
  // Board info
  //
  UINT8       PlatformFlavor;                             // 163 Platform Flavor
  UINT8       BoardRev;                                   // 164 Board Rev
  //
  // Package temperature
  //
  UINT8       PackageDTSTemperature;                      // 165 Package temperature
  UINT8       IsPackageTempMSRAvailable;                  // 166 Package Temperature MSR available
  UINT8       PeciAccessMethod;                           // 167 PECI Access Method (Direct I/O or ACPI)
  UINT8       Ac0FanSpeed;                                // 168 _AC0 Fan Speed
  UINT8       Ac1FanSpeed;                                // 169 _AC1 Fan Speed 
  UINT8       Ap2DigitalThermalSensorTemperature;         // 170   Temperature of the second AP
  UINT8       Ap3DigitalThermalSensorTemperature;         // 171   Temperature of the third AP
  UINT8       Reserved09[4];                              // 172:175
  UINT8       LtrEnable[8];                               // 176:183 Latency Tolerance Reporting Control
  UINT8       ObffEnable[8];                              // 184:191 Optimized Buffer Flush and Fill
  UINT8       XhciMode;                                   // 192 xHCI mode
  //
  // XTU 3.0 Specification
  //
  UINT32      XTUBaseAddress;                             // 193 XTU Base Address
  UINT32      XTUSize;                                    // 197 XTU Entries Size
  UINT32      XMPBaseAddress;                             // 201 XTU Base Address
  UINT8       DDRReferenceFreq;                           // 205 DDR Reference Frequency
  UINT8       Rtd3Support;                                // 206 Runtime D3 support.
  UINT8       Rtd3P0dl;                                   // 207 User selctable Delay for Device D0 transition.
  UINT8       Rtd3P3dl;                                   // 208 User selctable Delay for Device D0 transition.
  //
  // DPTF Devices and trip points
  //
  UINT8       EnableDptf;                                 // 209
  UINT8       EnableSaDevice;                             // 210
  UINT8       CriticalThermalTripPointSa;                 // 211
  UINT8       HotThermalTripPointSa;                      // 212
  UINT8       EnablePchDevice;                            // 213
  UINT8       CriticalThermalTripPointPch;                // 214
  UINT8       HotThermalTripPointPch;                     // 215
  //
  // DPTF Policies
  //
  UINT8       EnableCtdpPolicy;                           // 216
  UINT8       EnableLpmPolicy;                            // 217
  UINT8       CurrentLowPowerMode;                        // 218 CurrentLowPowerMode for DPTF
  UINT8   EnableCurrentExecutionUnit;          // 219 EnableCurrentExecutionUnit
  UINT16  TargetGfxFreq;                       // 220 : 221 TargetGfxFreq for
  //
  // DPPM Devices and trip points
  //
  UINT8   EnableMemoryDevice;                  // 222 EnableMemoryDevice
  UINT8   CriticalThermalTripPointTMEM;        // 223 CriticalThermalTripPointTMEM
  UINT8   HotThermalTripPointTMEM;             // 224 HotThermalTripPointTMEM
  UINT8   EnableFan1Device;                    // 225 EnableFan1Device
  UINT8   EnableFan2Device;                    // 226 EnableFan2Device
  UINT8   EnableAmbientDevice;                 // 227 EnableAmbientDevice
  UINT8   ActiveThermalTripPointAmbient;       // 228 ActiveThermalTripPointAmbient
  UINT8   PassiveThermalTripPointAmbient;      // 229 PassiveThermalTripPointAmbient
  UINT8   CriticalThermalTripPointAmbient;     // 230 CriticalThermalTripPointAmbient
  UINT8   HotThermalTripPointAmbient;          // 231 HotThermalTripPointAmbient
  UINT8   EnableSkinDevice;                    // 232 EnableSkinDevice
  UINT8   ActiveThermalTripPointSkin;          // 233 ActiveThermalTripPointSkin
  UINT8   PassiveThermalTripPointSkin;         // 234 PassiveThermalTripPointSkin
  UINT8   CriticalThermalTripPointSkin;        // 235 CriticalThermalTripPointSkin
  UINT8   HotThermalTripPointSkin;             // 236 HotThermalTripPointSkin
  UINT8   EnableExhaustFanDevice;              // 237 EnableExhaustFanDevice
  UINT8   ActiveThermalTripPointExhaustFan;    // 238 ActiveThermalTripPointExhaustFan
  UINT8   PassiveThermalTripPointExhaustFan;   // 239 PassiveThermalTripPointExhaustFan
  UINT8   CriticalThermalTripPointExhaustFan;  // 240 CriticalThermalTripPointExhaustFan
  UINT8   HotThermalTripPointExhaustFan;       // 241 HotThermalTripPointExhaustFan
  UINT8   EnableVRDevice;                      // 242 EnableVRDevice
  UINT8   ActiveThermalTripPointVR;            // 243 ActiveThermalTripPointVR
  UINT8   PassiveThermalTripPointVR;           // 244 PassiveThermalTripPointVR
  UINT8   CriticalThermalTripPointVR;          // 245 CriticalThermalTripPointVR
  UINT8   HotThermalTripPointVR;               // 246 HotThermalTripPointVR
  //
  // DPPM Policies
  //
  UINT8   EnableActivePolicy;                  // 247 EnableActivePolicy
  UINT8   EnablePassivePolicy;                 // 248 EnablePassivePolicy
  UINT8   EnableCriticalPolicy;                // 249 EnableCriticalPolicy
  UINT8   EnableCoolingModePolicy;             // 250 EnableCoolingModePolicy
  UINT8   TrtRevision;                         // 251 TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  UINT8   LPOEnable;                           // 252 LPOEnable
  UINT8   LPOStartPState;                      // 253 LPOStartPState
  UINT8   LPOStepSize;                         // 254 LPOStepSize
  UINT8   LPOPowerControlSetting;              // 255 LPOPowerControlSetting
  UINT8   LPOPerformanceControlSetting;        // 256 LPOPerformanceControlSetting
  //
  // Miscellaneous
  //
  UINT32  PpccStepSize;                        // 257:260 PPCC Step Size                   
  UINT8   EnableDisplayParticipant;            // 261 EnableDisplayParticipant
  //
  // PFAT
  //
  UINT64  PfatMemAddress;                      // 262:269 PFAT Memory Address for Tool Interface
  UINT8   PfatMemSize;                         // 270 PFAT Memory Size for Tool Interface
  UINT16  PfatIoTrapAddress;                   // 271:269 IoTrap Address for Tool Interface
  //
  // Smart Connect Technology
  //
  UINT8   IsctCfg;                             // 273 Isct Configuration
  //
  // Audio DSP
  //
  UINT32  DspBar0;                             // 274:277 Audio DSP BAR0
  UINT32  DspBar1;                             // 278:281 Audio DSP BAR1
  //
  // NFC
  //
  UINT8   NFCEnable;                           // 282 NFC module selection   
  //
  // ADSP Codec Selection
  //
  UINT8   AudioDspCodec;                       // 283 Audio Codec selection
  //
  // Sensor Hub Enable
  //
  UINT8   SensorHubEnable;                     // 284 Sensor Hub Enable
  UINT8   LowPowerS0Idle;                      // 285 Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  UINT8   ConfigTdpBios;                       // 286 enable/disable BIOS only version of Config TDP

  UINT8   Reserved17[266];                     // 287:552 Reserve for Intel RC
  UINT8   EnablePowerDevice;                   // 553 EnablePowerDevice
  UINT8   EnablePowerPolicy;                   // 554 EnablePowerPolicy
  UINT8   Rtd3UsbPowerResourceConfig;          // 555 RTD3 USB Power Resource config
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  UINT8   I2c0SensorDeviceSelection;           // 556 I2C0 Sensor Device Selection
  UINT8   I2c1SensorDeviceSelection;           // 557 I2C1 Sensor Device Selection
  UINT8   Spi0SensorDeviceSelection;           // 558 SPI0 Sensor Device Selection
  UINT8   Spi1SensorDeviceSelection;           // 559 SPI1 Sensor Device Selection
  UINT8   Uart0SensorDeviceSelection;          // 560 UART0 Sensor Device Selection
  UINT8   Uart1SensorDeviceSelection;          // 561 UART1 Sensor Device Selection
  UINT8   Reserved10;                          // 562 no longer used
  UINT8   RTD3I2C0SensorHub;                   // 563 RD3 support for I2C SH
  UINT8   PepDevice;                           // 564 RTD3 PEP support list(BIT0 - GFx , BIT1 - Sata, BIT2 - UART,
                                               //     BIT3 - SDHC, Bit4 - I2C0, BIT5 - I2C1, Bit6 - XHCI, Bit7 - Audio)
  UINT8   Reserved19[35];                      // 565:599 Reserve for Intel RC
  // Common_Part_Start  
  UINT8   ComPortCMode;                        // 600
  UINT8   ComPortD;                            // 601
  UINT8   Lpt1;                                // 602
  UINT8   PSType;                              // 603
  UINT8   WakeOnModemRing;                     // 604
  // Common_Part_End
  // Chipset_Start
  UINT8   IdeREnable;                          // 605
  UINT8   PcieRootPortPmeInt0;                 // 606
  UINT8   PcieRootPortPmeInt1;                 // 607
  UINT8   CStates;                             // 608
  UINT8   WakeOnPME;                           // 609 PME_EN: Power Management enable allow system be wake up by LAN.  
  UINT8   WakeOnLan;                           // 610
  UINT32  NVBIOSAddr;                          // 611
  UINT32  PegVendorID;                         // 615
  UINT8   OptimusFlag;                         // 619
  UINT8   RapidStartEnable;                    // 620 RapidStart enable flag
  UINT8   ISCT;                                // 621 ISCT flag
  UINT16  SATA1Channel0PioTiming;              // 622 SATA Controller 1 Pio Timing Data
  UINT16  SATA1Channel1PioTiming;              // 624 SATA Controller 1 Pio Timing Data
  UINT16  SATA2Channel0PioTiming;              // 626 SATA Controller 2 Pio Timing Data
  UINT16  SATA2Channel1PioTiming;              // 628 SATA Controller 2 Pio Timing Data
  UINT8   SleepState;                          // 630 Sleep state
  UINT32  AlarmTime;                           // 631 Alarm Time
  UINT8   FullGenericParticipant;              // 635 Full featured Generic Participant
  UINT8   HRAM;                                // 636
  UINT8   EnableACPIDebug;                     // 637   MDEN
  UINT8   TbtEndPointNum;                      // 638  Thunderbolt End Point Number
  // Chipset_End
  UINT8   ProtDevSlpEnable0;                   // 639  Port0 DevSlp Enable
  UINT8   ProtDevSlpEnable1;                   // 640  Port1 DevSlp Enable
  UINT8   ProtDevSlpEnable2;                   // 641  Port2 DevSlp Enable
  UINT8   ProtDevSlpEnable3;                   // 642  Port3 DevSlp Enable  
  UINT8   GPIObuttonNotifySleepState;          // 643 Virtual GPIO button Notify Sleep State Change
  UINT8   IUERButtonEnable;                    // 644 IUER Button Enable
  UINT8   IUERConvertibleEnable;               // 645 IUER Convertible Enable
  UINT8   IUERDockEnable;                      // 646 IUER Dock Enable
  UINT8   ECNotificationofLowPowerS0IdleState; // 647 EC Notification of Low Power S0 Idle State
  UINT16  RTD3AudioCodecdevicedelay;           // 648 RTD3 Audio Codec device delay
  UINT16  RTD3ADSPdevicedelay;                 // 650 RTD3 ADSP device delay
  UINT16  I2c0DeviceDelayTiming;               // 652 RTD3 I2C0 controller device delay timing
  UINT16  I2c1DDeviceDelayTiming;              // 654 RTD3 I2C1 controller device delay timing in Power Resourc
  UINT16  I2c1SDeviceDelayTiming;              // 656 RTD3 I2C1 controller device delay timing in _PS0
  UINT16  VRRampUpDelay;                       // 658 VR Ramp up delay
  UINT8   PStateCapping;                       // 660 P-state Capping
  UINT8   Rtd3Gpio;                            // 661 Rtd3 W/A Gpio, allow W/A for port 1 and 6 to use GPIO from SDHC device
  UINT16  I2C0DelayInPS0;                      // 662 Delay in _PS0 after powering up I2C0 Controller
  UINT16  I2C1DelayInPS0;                      // 664 Delay in _PS0 after powering up I2C1 Controller
  UINT8   Reserved18[2];                       // 666 : 667
  
  UINT8   RTD3ConfigSetting;                   // 668 RTD3 Config Setting(BIT0:ZPODD)
  UINT8   ECDebugLight;                        // 669 EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  UINT8   Ps2MouseEnable;                      // 670 Ps2 Mouse Enable
  UINT16  I2C0SSCNLow;                         // 671 SSCN-LOW  for I2C0
  UINT16  I2C0SSCNHigh;                        // 673 SSCN-HIGH for I2C0
  UINT16  I2C0SSCNHold;                        // 675 SSCN-HOLD for I2C0
  UINT16  I2C0FMCNLow;                         // 677 FMCN-LOW  for I2C0
  UINT16  I2C0FMCNHigh;                        // 679 FMCN-HIGH for I2C0
  UINT16  I2C0FMCNHold;                        // 681 FMCN-HOLD for I2C0
  UINT16  I2C0FPCNLow;                         // 683 FPCN-LOW  for I2C0
  UINT16  I2C0FPCNHigh;                        // 685 FPCN-HIGH for I2C0
  UINT16  I2C0FPCNHold;                        // 687 FPCN-HOLD for I2C0
  UINT16  I2C1SSCNLow;                         // 689 SSCN-LOW  for I2C1
  UINT16  I2C1SSCNHigh;                        // 691 SSCN-HIGH for I2C1
  UINT16  I2C1SSCNHold;                        // 693 SSCN-HOLD for I2C1
  UINT16  I2C1FMCNLow;                         // 695 FMCN-LOW  for I2C1
  UINT16  I2C1FMCNHigh;                        // 697 FMCN-HIGH for I2C1
  UINT16  I2C1FMCNHold;                        // 699 FMCN-HOLD for I2C1
  UINT16  I2C1FPCNLow;                         // 701 FPCN-LOW  for I2C1
  UINT16  I2C1FPCNHiigh;                       // 703 FPCN-HIGH for I2C1
  UINT16  I2C1FPCNHold;                        // 705 FPCN-HOLD for I2C1
  UINT16  I2C0M0D3;                            // 707 M0D3 for I2C0
  UINT16  I2C0M1D3;                            // 709 M1D3 for I2C0
  UINT16  I2C0M0D0;                            // 711 M0D0 for I2C0
  UINT16  I2C1M0D3;                            // 713 M0D3 for I2C1
  UINT16  I2C1M1D3;                            // 715 M1D3 for I2C1
  UINT16  I2C1M0D0;                            // 717 M0D0 for I2C1
  UINT16  SPI0M0D3;                            // 719 M0D3 for SPI0
  UINT16  SPI0M1D3;                            // 721 M1D3 for SPI0
  UINT16  SPI1M0D3;                            // 723 M0D3 for SPI1
  UINT16  SPI1M1D3;                            // 725 M1D3 for SPI1
  UINT16  UA00M0D3;                            // 727 M0D3 for UA00
  UINT16  UA00M1D3;                            // 729 M1D3 for UA00
  UINT16  UA01M0D3;                            // 731 M0D3 for UA01
  UINT16  UA01M1D3;                            // 733 M1D3 for UA01
  UINT8   ThunderBoltSMIFunctionNumber;        // 735 ThunderBolt SMI Function Number
  UINT32  GPIOIRQ;                             // 736:739 GPIO IRQ
  UINT8   PIRQS;                               // 740 PIRQS 34,50(GPIO)
  UINT8   PIRQX;                               // 741 PIRQX 39,55(GPIO)
  UINT8   PIRQM;                               // 742 PIRQM 28,14(GPIO)
  UINT8   LanPHYStatus;                        // 743 LAN PHY Status 0 = Not Present, 1 = Present
  UINT8   Reserved16;                          // 744 
  UINT8   SensorStandbymode;                   // 745 Sensor Standby mode
  UINT8   PL1LimitCS;                          // 746 set PL1 limit when entering CS
  UINT16  PL1LimitCSValue;                     // 747 PL1 limit value
  UINT8   EnableGen1Participant;               // 749 EnableGen1Participant
  UINT8   ActiveThermalTripPointGen1;          // 750 ActiveThermalTripPointGen1
  UINT8   PassiveThermalTripPointGen1;         // 751 PassiveThermalTripPointGen1
  UINT8   CriticalThermalTripPointGen1;        // 752 CriticalThermalTripPointGen1
  UINT8   HotThermalTripPointGen1;             // 753 HotThermalTripPointGen1
  UINT8   EnableGen2Participant;               // 754 EnableGen2Participant
  UINT8   ActiveThermalTripPointGen2;          // 755 ActiveThermalTripPointGen2
  UINT8   PassiveThermalTripPointGen2;         // 756 PassiveThermalTripPointGen2
  UINT8   CriticalThermalTripPointGen2;        // 757 CriticalThermalTripPointGen2
  UINT8   HotThermalTripPointGen2;             // 758 HotThermalTripPointGen2
  UINT8   EnableWwanTempSensorDevice;          // 759 EnableWwanTempSensorDevice
  UINT8   EnableCpuVrTempSensorDevice;         // 760 EnableCpuVrTempSensorDevice
  UINT8   EnableSsdTempSensorDevice;           // 761 EnableSsdTempSensorDevice
  UINT8   EnableInletFanTempSensorDevice;      // 762 EnableInletFanTempSensorDevice
  UINT8   ActiveThermalTripPointInletFan;      // 763 ActiveThermalTripPointInletFan
  UINT8   PassiveThermalTripPointInletFan;     // 764 PassiveThermalTripPointInletFan
  UINT8   CriticalThermalTripPointInletFan;    // 765 CriticalThermalTripPointInletFan
  UINT8   HotThermalTripPointInletFan;         // 766 HotThermalTripPointInletFan
  UINT8   DmaOsDetection;                      // 767 DMA OS detection, 2 = check for OS version when enabling DMA, 0,1 = don't care about OS
  UINT8   USBSensorHub;                        // 768 USB Sensor Hub Enable/Disable
  UINT8   BroadcomBlueToothAdapterRev;         // 769 Broadcom's Bluetooth adapter's revision
  UINT8   WittVerI2C0;                         // 770 I2C0/WITT devices version
  UINT8   WittVerI2C1;                         // 771 I2C1/WITT devices version
  UINT8   AtmelPanelFwUpdate;                  // 772 Atmel panel FW update Enable/Disable
  UINT8   TbtSmiOnHotPlug;                     // 773 SMI on Hot Plug for TBT devices
  UINT8   TbtNotifyOnHotPlug;                  // 774 Notify on Hot Plug for TBT devices
  UINT8   TbtRootPortSelector;                 // 775 ThunderBolt Root port selector
  UINT8   WakFinished;                         // 776 WAK Finished
  UINT16  PEPConstraints;                      // 777 PEP Constraints
  UINT16  VRStaggeringDelay;                   // 779 VR Staggering delay
  UINT8   TenSPowerButtonSupport;              // 781 10sec Power button support Bit0: 10 sec P-button Enable/Disable
                                               //                                Bit1: Internal Flag
                                               //                                Bit2: Rotation Lock flag, 0:unlock, 1:lock
                                               //                                Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
                                               //                                Bit4: Undock / Dock Flag, 0: Undock, 1: Dock  
                                               //                                Bit5, 6: reserved for future use.
                                               //                                Bit7: EC 10sec PB Override state for S3/S4 wake up. 
//[-start-130222-IB03780481-add]//
  UINT8   TbtChipType;                         // 782 Thunderbolt Chip Type (CR or RR)
  UINT8   TbtRootPortNum;                      // 783 Thunderbolt Root Port Number
  UINT32  TbtHostBridgeMemoryBase;             // 784 Thunderbolt Host Bridge Memory Base
//[-end-130222-IB03780481-add]//
//[-start-130611-IB11120016-add]//
  UINT8   InterfaceType;                       // 788 System Interface Type of BMC
  UINT8   SpecificationRevision;               // 789 IPMI Spec. Version of BMC
  UINT16  BaseAddress;                         // 790 Base Address of BMC
  UINT8   BmcStatus;                           // 792 BMC Status
  UINT8   RegisterOffset;                      // 793 Command and Data port Offset of BMC
//[-end-130611-IB11120016-add]//
  UINT8  EnableWWANParticipant;                // 794 EnableWWANParticipant
  UINT8  ActiveThermalTripPointWWAN;           // 795 ActiveThermalTripPointWWAN
  UINT8  PassiveThermalTripPointWWAN;          // 796 PassiveThermalTripPointWWAN
  UINT8  CriticalThermalTripPointWWAN;         // 797 CriticalThermalTripPointWWAN
  UINT8  HotThermalTripPointWWAN;              // 798 HotThermalTripPointWWAN
  UINT8  UltPlatform;                          // 799 ULT Platform
  UINT8  Reserved11[5];
  UINT16 MiniPowerLimitForDptf;                // 805 Minimum Power Limit for DPTF use via PPCC Object
  UINT8  Gpio13ReworkForSawtoothPeak;          // 807 GPIO13 Rework for Sawtooth Peak
//[-start-130220-IB02950497-add]//
  UINT8  ManualMode;                           // 808 Manual Mode switch
  UINT32 ManualModeUsb20PerPinRoute;           // 809 ManualModeUsb20PerPinRoute
  UINT8  ManualModeUsb30PerPinEnable;          // 813 ManualModeUsb30PerPinEnable          
//[-end-130220-IB02950497-add]//  
  UINT8  SataPortState;                        // 814 SPST, SATAPort state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  UINT8  ECLowPowerMode;                       // 815 EC Low Power Mode: 1 - Enabled, 0 - Disabled
//[-start-140521-IB05400527-add]//
  UINT8  Mmio64Support;                        // 816 Enable/Disable 64bits MMIO support
  UINT64 MmioMem64Base;                        // 817 64 bits MMIO base address
  UINT64 MmioMem64Limit;                       // 825 64 bits MMIO limit
  UINT64 MmioMem64Length;                      // 833 64 bits MMIO length
//[-end-140521-IB05400527-add]//
} EFI_GLOBAL_NVS_AREA;
#pragma pack()
//
// Global NVS Area Protocol
//
struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA        *Area;
  EFI_OEM_GLOBAL_NVS_AREA    *OemArea;
};

#endif
