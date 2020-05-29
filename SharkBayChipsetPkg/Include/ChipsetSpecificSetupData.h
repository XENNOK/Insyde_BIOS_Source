/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifdef _IMPORT_CHIPSET_SPECIFIC_SETUP_

//==========================================================================================================
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// Start area for Platform. The following area is used by Chipset team to modify.
// The total size of variable in this part are fixed (500bytes). That means if you need to add or remove
// variables, please modify the PlatformRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------
//[-start-130709-IB05160465-modify]//
//Platform_Start
//Offset(530);

  //
  // SATA Configuration
  //
  UINT8         AhciOptionRomSupport;                 //Offset 530
  UINT8         AggressLinkPower;
  UINT8         RaidAlternateId;
  UINT8         HdcP0HotPlug;
  UINT8         HdcP1HotPlug;
  UINT8         HdcP2HotPlug;
  UINT8         HdcP3HotPlug;
  UINT8         HdcP4HotPlug;
  UINT8         HdcP5HotPlug;
  UINT8         HdcP0SpinUp;
  UINT8         HdcP1SpinUp;                          //Offset 540
  UINT8         HdcP2SpinUp;
  UINT8         HdcP3SpinUp;
  UINT8         HdcP4SpinUp;
  UINT8         HdcP5SpinUp;
  UINT8         HddUnlock;
  UINT8         LedLocate;
  UINT8         Raid0;
  UINT8         Raid1;
  UINT8         Raid10;
  UINT8         Raid5;                                //Offset 550
  UINT8         Irrt;
  UINT8         OromUiBanner;
  UINT8         IrrtOnly;
  UINT8         SataP0DeviceType;
  UINT8         SataP1DeviceType;
  UINT8         SataP2DeviceType;
  UINT8         SataP3DeviceType;
  UINT8         SataP4DeviceType;
  UINT8         SataP5DeviceType;

  //
  // RapidStart
  //
  UINT8         EnableRapidStart;                     //Offset 560
  UINT8         EntryOnS3RtcWake;
  UINT16        S3WakeTimerMin;
  UINT8         EntryOnS3CritBattWake;
  UINT8         CritticalBatWakeThres;

  //
  // iSCT
  //
  UINT8         ISCTEnable;
  UINT8         ISCTNotify;
  UINT8         ISCTWLanPower;
  UINT8         ISCTWWLanPower;

  //
  // Overclocking Configuration
  //
  UINT8         Overclocking;                         //Offset 570
  UINT8         OcReserved01;
  UINT8         FlexRatioOverride;
  UINT8         FlexRatio;
  UINT16        HostClockFreq;
  UINT8         MemoryTcwl;
  UINT8         MemoryTcl;
  UINT8         MemoryTrcd;
  UINT8         MemoryTrp;
  UINT8         MemoryTras;                           //Offset 580
  UINT8         MemoryTwr;
  UINT16        MemoryTrfc;
  UINT8         MemoryTrrd;
  UINT8         MemoryTwtr;
  UINT8         MemoryTrtp;
  UINT8         MemoryTrc;
  UINT8         MemoryTfaw;
  UINT8         MemoryVoltage;
  UINT8         XmpProfileSetting;                    //Offset 590
  UINT8         MemoryTrpab;
  UINT16        MemoryTrefi;
  UINT8         OcReserved02;
  UINT8         GtOverclockFreq;

  //
  // ME / AT
  //
  UINT8         AtConfig;
  UINT8         AtState;
  UINT8         AtEnterSuspendState;
  UINT8         IFRUpdate;
  
  //
  // ICC
  //
  UINT8         IccCapabilities;                      //Offset 600
  UINT8         IccWatchDog;
  UINT8         IccUnusedPci;
  UINT8         IccLockRegisters;
  UINT8         IccProfile;
  UINT8         IccOperable;
  UINT8         IccClockPage;
  UINT16        IccNewDivisor;
  UINT8         IccNewSccMode;
  UINT8         IccNewSccValue;                       //Offset 610
  UINT8         IccApply;
  UINT8         IccBackupProfile;
  UINT8         IccSupportedProfilesNumber;

  //
  // GPIO
  //
  UINT8         GpioLockdown;

  //
  // DPTF / cTDP
  //
  UINT8         EnableDptf;
  UINT8         DptfProcessorThermalDevice;
  UINT8         DptfPchThermalDevice;
  UINT8         LPM;
  UINT8         CurrentLowPowerMode;
  UINT8         cTDP;                                 //Offset 620
  UINT8         ConfigTDPCapability;
  UINT8         ConfigTDPLevel;
  UINT8         ActivePageThresholdEnable;
  UINT32        ActivePageThresholdSize;
  UINT8         ConfigTDPLock;
  UINT8         ConfigTdpCustom;
  UINT8         CustomTdpCount;                       //Offset 630
  UINT8         CustomBootModeIndex;
  UINT16        CustomPowerLimit10;
  UINT16        CustomPowerLimit20;
  UINT8         CustomPowerLimit1Time0;
  UINT8         CustomTurboActivationRatio0;
  UINT8         CustomConfigTdpControl0;
  UINT16        CustomPowerLimit11;                   //Offset 639 - 640
  UINT16        CustomPowerLimit21;
  UINT8         CustomPowerLimit1Time1;
  UINT8         CustomTurboActivationRatio1;
  UINT8         CustomConfigTdpControl1;
  UINT16        CustomPowerLimit12;
  UINT16        CustomPowerLimit22;
  UINT8         CustomPowerLimit1Time2;               //Offset 650
  UINT8         CustomTurboActivationRatio2;
  UINT8         CustomConfigTdpControl2;

  //
  // Misc
  // 
  UINT8         RtcLock;
 
  // 
  // CPPC 
  // 
  UINT8         EnableCppc;
  UINT8         EnableCppcPlatformSCI;
  UINT16        MaxTOLUD;
  UINT8         S5LongRunTest;
  UINT8         BistOnReset;
  UINT8         Pfat;                                 //Offset 660
  UINT8         HardwarePrefetcher;
  UINT8         BootInLfm;
  UINT8         EnergyPolicy;
  UINT8         DdrPowerLimitLock;
  UINT8         DdrLongTermPowerLimitOverride;
  UINT8         DdrShortTermPowerLimitOverride;
  UINT8         DdrLongTermPowerLimit;
  UINT8         DdrLongTermTimeWindow;
  UINT8         DdrShortTermPowerLimit;
  UINT8         ActiveThermalTripPointMCH;            //Offset 670
  UINT8         PassiveThermalTripPointMCH;
  UINT8         CriticalThermalTripPointSa;
  UINT8         HotThermalTripPointSa;
  UINT32        PPCCStepSize;
  UINT8         LPOEnable;
  UINT8         LPOStartPState;
  UINT8         LPOStepSize;                          //Offset 680
  UINT8         LPOPowerControlSetting;
  UINT8         LPOPerformanceControlSetting;
  UINT8         ActiveThermalTripPointPCH;
  UINT8         PassiveThermalTripPointPCH;
  UINT8         CriticalThermalTripPointPch;
  UINT8         HotThermalTripPointPch;
  UINT8         EnableMemoryDevice;
  UINT8         ActiveThermalTripPointTMEM;
  UINT8         PassiveThermalTripPointTMEM;
  UINT8         CriticalThermalTripPointTMEM;         //Offset 690
  UINT8         HotThermalTripPointTMEM;
  UINT8         EnableFan1Device;
  UINT8         EnableFan2Device;
  UINT8         EnableAmbientDevice;
  UINT8         ActiveThermalTripPointAmbient;
  UINT8         PassiveThermalTripPointAmbient;
  UINT8         CriticalThermalTripPointAmbient;
  UINT8         HotThermalTripPointAmbient;
  UINT8         EnableSkinDevice;
  UINT8         ActiveThermalTripPointSkin;           //Offset 700
  UINT8         PassiveThermalTripPointSkin;
  UINT8         CriticalThermalTripPointSkin;
  UINT8         HotThermalTripPointSkin;
  UINT8         EnableExhaustFanDevice;
  UINT8         ActiveThermalTripPointExhaustFan;
  UINT8         PassiveThermalTripPointExhaustFan;
  UINT8         CriticalThermalTripPointExhaustFan;
  UINT8         HotThermalTripPointExhaustFan;
  UINT8         EnableVRDevice;
  UINT8         ActiveThermalTripPointVR;             //Offset 710
  UINT8         PassiveThermalTripPointVR;
  UINT8         CriticalThermalTripPointVR;
  UINT8         HotThermalTripPointVR;
  UINT8         EnableActivePolicy;
  UINT8         EnablePassivePolicy;
  UINT8         EnableCriticalPolicy;
  UINT8         EnableCoolingModePolicy;
  UINT8         TrtRevision;
  UINT16        SataP0TimeOut;                        //Offset 719 - 720
  UINT8         FullGenericParticipant;
  UINT8         EnableLowPowerS0Cap;
  UINT8         EnableSerialIoDma;
  UINT8         EnableSerialIoI2c0;
  UINT8         EnableSerialIoI2c1;
  UINT8         EnableSerialIoSpi0;
  UINT8         EnableSerialIoSpi1;
  UINT8         EnableSerialIoUart0;
  UINT8         EnableSerialIoUart1;
  UINT8         EnableSerialIoSdio;                   //Offset 730
  UINT8         EnableSerialIoAudioDsp;
  UINT8         SerialIoMode;
  UINT8         RuntimeDevice3;
  UINT8         DisPlayMode;
  UINT8         PEGWorkAround;
  UINT8         EnableC8;
  UINT8         EnableC9;
  UINT8         EnableC10;
  UINT8         CstateLatencyControl3TimeUnit;
  UINT8         CstateLatencyControl4TimeUnit;        //Offset 740
  UINT8         CstateLatencyControl5TimeUnit;
  UINT16        CstateLatencyControl3Irtl;
  UINT16        CstateLatencyControl4Irtl;
  UINT16        CstateLatencyControl5Irtl;
  UINT8         EnableACPIDebug;
  UINT32        ACPIDebugAddr;                        //Offset 749 -752
  UINT8         TbtWorkaround;
  UINT8         TbtSxWakeTimer;
  UINT16        ReserveMemoryPerSlot;
  UINT16        ReservePMemoryPerSlot;
  UINT8         ReserveIOPerSlot;
  UINT8         IsctTimerChoice;                      //Offset 760
  UINT8         NfcSelection;
  UINT8         AudioDspD3PowerGating;
  UINT8         AudioDspBluetoothSupport;
  UINT8         AudioDspAcpiMode;
  UINT8         AudioCodecSelect;
  UINT8         HybridHardDisk;
  UINT8         DisplaySaveRestore;
  UINT8         DisplayType;
  UINT8         FTpmSwitch;
  UINT8         Rtd3UsbSpeed1;                        //Offset 770
  UINT8         Rtd3UsbSpeed2;
  UINT8         PepGfx;
  UINT8         PepSata;
  UINT8         MemThermalManage;
  UINT8         PeciInjectedTemp;
  UINT8         ExttsViaTsOnBoard;
  UINT8         ExttsViaTsOnDimm;
  UINT8         VirtualTempSensor;
  
  UINT8         LockThermalManageRegs;
  UINT8         ExternThermalStatus;                  //Offset 780
  
  UINT8         ClosedLoopThermalManage;
  UINT8         OpenLoopThermalManage;
  
  UINT8         WarmThresholdCh0Dimm0;
  UINT8         WarmThresholdCh0Dimm1;
  UINT8         HotThresholdCh0Dimm0;
  UINT8         HotThresholdCh0Dimm1;
  
  UINT8         WarmThresholdCh1Dimm0;
  UINT8         WarmThresholdCh1Dimm1;
  UINT8         HotThresholdCh1Dimm0;
  UINT8         HotThresholdCh1Dimm1;                 //Offset 790
  
  UINT8         WarmBudgetCh0Dimm0;
  UINT8         WarmBudgetCh0Dimm1;
  UINT8         HotBudgetCh0Dimm0;
  UINT8         HotBudgetCh0Dimm1;
  
  UINT8         WarmBudgetCh1Dimm0;
  UINT8         WarmBudgetCh1Dimm1;
  UINT8         HotBudgetCh1Dimm0;
  UINT8         HotBudgetCh1Dimm1;

  UINT8         RaplPLLock;
  UINT8         RaplPL1Enable;                        //Offset 800
  UINT16        RaplPL1Power;
  UINT8         RaplPL1WindowX;
  UINT8         RaplPL1WindowY;
                       
  UINT8         RaplPL2Enable;
  UINT16        RaplPL2Power;
  UINT8         RaplPL2WindowX;
  UINT8         RaplPL2WindowY;
  UINT8         EnableWifiDevice;                     //Offset 810
  UINT8         ActiveThermalTripPointWifi;
  UINT8         PassiveThermalTripPointWifi;
  UINT8         CriticalThermalTripPointWifi;
  UINT8         HotThermalTripPointWifi;
  UINT8         EnablePowerDevice;
  UINT8         EnablePowerPolicy;
  UINT8         EnableDisplayParticipant;
  //
  // Overclocking Configuration
  //
  UINT8         OcCapIaCore;
  UINT8         OcCapGt;
  UINT8         OcCapClr;                             //Offset 820
  UINT8         OcCapUncore;
  UINT8         OcCapIoa;
  UINT8         OcCapIod;
  UINT16        CoreMaxOcRatio;
  UINT8         CoreVoltageMode;
  UINT16        CoreExtraTurboVoltage;
  UINT16        CoreVoltageOverride;                  //Offset 829 - 830
  UINT16        CoreVoltageOffset;
  UINT8         CoreVoltageOffsetPrefix;
  UINT16        ClrMaxOcRatio;
  UINT8         ClrVoltageMode;
  UINT16        ClrExtraTurboVoltage;
  UINT16        ClrVoltageOverride;                   //Offset 839 - 840
  UINT16        ClrVoltageOffset;
  UINT8         ClrVoltageOffsetPrefix;
  UINT8         SvidSupport;
  UINT16        SvidVoltageOverride;
  UINT8         FivrFaults;
  UINT8         FivrEfficiencyManagement;
  UINT8         GtVoltageMode;
  UINT16        GtExtraTurboVoltage;                  //Offset 849 - 850
  UINT16        GtVoltageOverride;
  UINT16        GtVoltageOffset;
  UINT8         GtVoltageOffsetPrefix;
  UINT16        UncoreVoltageOffset;
  UINT8         UncoreVoltageOffsetPrefix;
  UINT16        IoaVoltageOffset;                     //Offset 850 - 861
  UINT8         IoaVoltageOffsetPrefix;
  UINT16        IodVoltageOffset;
  UINT8         IodVoltageOffsetPrefix;
  UINT8         MemoryRefClk;
  UINT8         MemoryRatio;
  UINT8         SaAudioEnable;
  UINT8         SataP0DeviceSleep;
  UINT8         SataP1DeviceSleep;                    //Offset 870
  UINT8         SataP2DeviceSleep;
  UINT8         SataP3DeviceSleep;
  UINT8         SataP4DeviceSleep;
  UINT8         SataP5DeviceSleep;
  UINT8         SataP0EnableDito;
  UINT8         SataP1EnableDito;
  UINT8         SataP2EnableDito;
  UINT8         SataP3EnableDito;
  UINT8         SataP4EnableDito;
  UINT8         SataP5EnableDito;                     //Offset 880
  UINT16        SataP0DitoVal;
  UINT16        SataP1DitoVal;
  UINT16        SataP2DitoVal;
  UINT16        SataP3DitoVal;
  UINT16        SataP4DitoVal;                        //Offset 889 - 890
  UINT16        SataP5DitoVal;
  UINT8         SataP0DmVal;
  UINT8         SataP1DmVal;
  UINT8         SataP2DmVal;
  UINT8         SataP3DmVal;
  UINT8         SataP4DmVal;
  UINT8         SataP5DmVal;

  //
  // I2c
  //
  UINT8         I2c0IntelSensorHub;
  UINT8         I2c0WITTDevice;                       //Offset 900
  UINT16        I2c1AtmelTouchPanel;
  UINT16        I2c1ElantechTouchPanel;
  UINT16        I2c1ElantechTouchPad;
  UINT16        I2c1SynapticsTouchPad;
  UINT16        I2c1WITTDevice;                       //Offset 909 - 910
  UINT16        I2c1NTrigDigitizer;
  UINT16        I2c1EETITouchPanel;
  UINT16        I2c1AlpsTouchPad;
  UINT16        I2c1CypressTouchPad;

  UINT8         ECLowPowerS0IdleEnable;
  
  //
  // RTD3
  //
  UINT16        Rtd3AudioDeviceDelay;                 //Offset 920 - 921
  UINT16        Rtd3AdspDeviceDelay;
  UINT16        I2c0DeviceDelayTiming;
  UINT16        I2c1DeviceDelayTiming;
  UINT8         PStateCapping;
  UINT8         Rtd3CongigSetting;

  //
  // Intel Ultrabook Event Support
  //
  UINT8         IuerDockEnable;                       //Offset 930
  UINT8         IuerButtonEnable;
  UINT8         NativePCIESupport;
  UINT8         AudioDisableBitmap;
  UINT8         Tpm2Flag;
//[-start-130123-IB04770265-add]//
  UINT8         TbtWakeFromDevice;
//[-end-130123-IB04770265-add]//
//[-start-130307-IB03780481-add]//
  UINT8         TbtRsvdBus;
  UINT16        TbtRsvdIo;
  UINT16        TbtRsvdMem;                           //Offset 939 - 940
  UINT16        TbtRsvdPmem;
  UINT8         TbtRsvdMemAlign;
  UINT8         TbtRsvdPmemAlign;
//[-end-130307-IB03780481-add]//
//[-start-130611-IB11120016-add]//
  //
  // BMC/IPMI
  //
  UINT8         IpmiBootOption;
  UINT8         IpmiEnable;
  UINT8         IpmiSetBiosVersion;
  UINT8         IpmiSdrListEnable;
  UINT8         IpmiSpmiEnable;
  UINT8         BmcWdtEnable;                         //Offset 950
  UINT8         BmcWdtTimeout;
  UINT8         BmcWdtAction;
  UINT8         BmcWarmupTime;
  UINT8         BmcLanPortConfig;
  UINT8         BmcLanChannel;
  UINT8         BmcIPv4Source;
  UINT8         BmcIPv4IpAddress[4];                  //Offset 957 - 960
  UINT8         BmcIPv4SubnetMask[4];
  UINT8         BmcIPv4GatewayAddress[4];
  UINT8         BmcFrontPanelPwrBtn;
  UINT8         BmcFrontPanelRstBtn;                  //Offset 970
  UINT8         BmcFrontPanelNmiBtn;
  UINT8         BmcStatus;
  UINT8         BmcIPv6Mode;
  UINT8         BmcIPv6AutoConfig;
  UINT8         BmcIPv6PrefixLength;
  UINT8         BmcIPv6IpAddress[16];                 //Offset 976 - 991
  UINT8         BmcIPv6GatewayAddress[16];            //Offset 992 - 1007
  UINT8         BmcPowerCycleTimeEnable;
  UINT8         BmcPowerCycleTime;
//[-end-130611-IB11120016-add]//
//[-start-130617-IB04560405-add]//
  //
  // Event Log
  //
  UINT8         EventLogFullOption;                   //Offset 1010
  UINT8         LogEventTo;
//[-start-140318-IB08400253-modify]//
  UINT8         PostMessageEn;
  UINT8         ProgressCode;
  UINT8         ErrorCode;
  UINT8         DebugCode;
  UINT8         LogPostMsg;                   
  UINT8         ShowPostMsg;
  UINT8         BeepPostMsg;
//[-end-140318-IB08400253-modify]//
//[-end-130617-IB04560405-add]//
//[-start-130709-IB05400426-add]//
  UINT8         TbtDevice;
  UINT8         TbtChip;                              //Offset 1020
  UINT8         TbtHotPlug;
  UINT8         PcieRpStrap0;
  UINT8         PcieRpStrap1;
//[-end-130709-IB05400426-add]//
//[-start-130205-IB02950497-add]//
  UINT8         ManualMode;
  UINT8         USBRouteSwitch;
  UINT8         USB20Pin0;
  UINT8         USB20Pin1;
  UINT8         USB20Pin2;
  UINT8         USB20Pin3;
  UINT8         USB20Pin4;                            //offset 1030
  UINT8         USB20Pin5;
  UINT8         USB20Pin6;
  UINT8         USB20Pin7;
  UINT8         USB20Pin8;
  UINT8         USB20Pin9;
  UINT8         USB20Pin10;
  UINT8         USB20Pin11;
  UINT8         USB20Pin12;
  UINT8         USB20Pin13;
  UINT8         USB30SuperSpeed;                      //offset 1040
  UINT8         USB30Pin1;
  UINT8         USB30Pin2;
  UINT8         USB30Pin3;
  UINT8         USB30Pin4;
  UINT8         USB30Pin5;
  UINT8         USB30Pin6;
//[-end-130205-IB02950497-add]//  
//[-start-130714-IB10930040-add]//
  UINT8         UsbPrecondition;                      //offset 1047
//[-end-130714-IB10930040-add]//
//[-start-130403-IB05330413-add]//
  UINT8         EnablePwrDn;     
  UINT8         EnablePwrDnLpddr;
  UINT8         Refresh2X;                            //Offset 1050
  UINT8         LpddrThermalSensor; 
  UINT8         UserPowerWeightsEn; 
  UINT8         EnergyScaleFact;    
  UINT8         RaplPwrFlCh0;       
  UINT8         RaplPwrFlCh1;       
  UINT8         IdleEnergyCh0Dimm0; 
  UINT8         IdleEnergyCh0Dimm1; 
  UINT8         IdleEnergyCh1Dimm0; 
  UINT8         IdleEnergyCh1Dimm1; 
  UINT8         PdEnergyCh0Dimm0;                     //Offset 1060
  UINT8         PdEnergyCh0Dimm1;                     //Offset 
  UINT8         PdEnergyCh1Dimm0;                     //Offset 
  UINT8         PdEnergyCh1Dimm1;                     //Offset 
  UINT8         ActEnergyCh0Dimm0;                    //Offset 
  UINT8         ActEnergyCh0Dimm1;                    //Offset 
  UINT8         ActEnergyCh1Dimm0;                    //Offset 
  UINT8         ActEnergyCh1Dimm1;                    //Offset 
  UINT8         RdEnergyCh0Dimm0;                     //Offset 
  UINT8         RdEnergyCh0Dimm1;                     //Offset 
  UINT8         RdEnergyCh1Dimm0;                     //Offset 1070
  UINT8         RdEnergyCh1Dimm1;                     //Offset 
  UINT8         WrEnergyCh0Dimm0;                     //Offset 
  UINT8         WrEnergyCh0Dimm1;                     //Offset 
  UINT8         WrEnergyCh1Dimm0;                     //Offset 
  UINT8         WrEnergyCh1Dimm1;                     //Offset 
  UINT8         SrefCfgEna;                           //Offset 
  UINT16        SrefCfgIdleTmr;                       //Offset 
  UINT8         ThrtCkeMinDefeat;                     //Offset 
  UINT8         ThrtCkeMinTmr;                        //Offset 1080
  UINT8         ThrtCkeMinDefeatLpddr;                //Offset 
  UINT8         ThrtCkeMinTmrLpddr;                   //Offset 1082
//[-end-130403-IB05330413-add]//

//[-start-130911-IB08620310-add]//
  //
  // SR-IOV
  //
  UINT8         PCIeSRIOVSupport;                     //Offset 1083
  UINT8         PCIeAriSupport;                       //Offset 1084
  UINT16        PCIeSupportPageSize;                  //Offset 1085
//[-end-130911-IB08620310-add]//

//[-start-131225-IB08620313-add]//
  UINT8         WheaSupport;                          //Offset 1087 // Enable/Disable WHEA
  UINT16        WheaEinjType;                         //Offset 1088 // WHEA Error Injection Types
  UINT8         WheaDefrag;                           //Offset 1090 // WHEA Defrag time
//[-end-131225-IB08620313-add]//
//[-start-140425-IB10310054-modify]//
  UINT8         WheaErrorRecordFormat;                //Offset 1091
//[-start-140428-IB10110147-modify]//
//[-start-140512-IB10300110-add]//
  UINT8         BiosWDT;                             //Offset 1092
  UINT8         WDTOpromScan;                        //Offset 1093
  UINT8         WDTOsLoad;                           //Offset 1094
//[-end-140512-IB10300110-add]//

//[-start-140512-IB10300110-modify]//
//[-start-131225-IB08620313-modify]//
//[-start-130911-IB08620310-modify]//
//[-start-130205-IB02950497-modify]//
//[-start-140425-IB10310054-modify]//
  UINT8         PlatformRSV[5];                      //Offset 1095 - 1099
//[-start-140428-IB10110147-modify]//
//[-end-130205-IB02950497-modify]//
//[-end-130911-IB08620310-modify]//
//[-end-131225-IB08620313-modify]//
//[-end-140512-IB10300110-modify]//

//Platform_End
//[-end-130709-IB05160465-modify]//
//----------------------------------------------------------------------------------------------------------
// End of area for Chipset team use.
//----------------------------------------------------------------------------------------------------------

#endif
