/** @file
  MMC 5.0 spec definition.

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

#ifndef _MMC_H
#define _MMC_H

#pragma pack(1)

#define  CMD0                   0
#define  CMD1                   1
#define  CMD2                   2
#define  CMD3                   3
#define  CMD4                   4
#define  CMD6                   6
#define  CMD7                   7
#define  CMD8                   8
#define  CMD9                   9
#define  CMD10                  10
#define  CMD11                  11
#define  CMD12                  12
#define  CMD13                  13
#define  CMD14                  14
#define  CMD15                  15
#define  CMD16                  16
#define  CMD17                  17
#define  CMD18                  18
#define  CMD19                  19
#define  CMD20                  20
#define  CMD23                  23
#define  CMD24                  24
#define  CMD25                  25
#define  CMD26                  26
#define  CMD27                  27
#define  CMD28                  28
#define  CMD29                  29
#define  CMD30                  30
#define  CMD35                  35
#define  CMD36                  36
#define  CMD38                  38
#define  CMD39                  39
#define  CMD40                  40
#define  CMD42                  42
#define  CMD55                  55
#define  CMD56                  56

//
// MMC commands definition
//
#define CMD6_ACS_CMD_SET          0
#define CMD6_ACS_SET_BIT          1
#define CMD6_ACS_CLR_BIT          2
#define CMD6_ACS_WR_BYTE          3

#define FREQUENCY_OD            (400 * 1000)
#define FREQUENCY_MMC_PP        (26 * 1000 * 1000)
#define FREQUENCY_MMC_PP_HIGH   (52 * 1000 * 1000)

#define DEFAULT_DSR_VALUE       0x404

#define IDLE_STATE              0 
#define READY_STATE             1 
#define IDENT_STATE             2 
#define STBY_STATE              3 
#define TRAN_STATE              4 
#define DATA_STATE              5 
#define RCV_STATE               6 
#define PRG_STATE               7 
#define DIS_STATE               8 
#define BTST_STATE              9 

#define BYTE_MODE               0 
#define SECTOR_MODE             2 

//
// 5.0 FFU
//
#define NORMAL_MODE             0
#define FFU_MODE                1
#define VENDOR_SPECIFIC_MODE    0x10
#define FFU_INSTALL             1
#define FFU_ABORT               2

typedef struct {
  UINT32  Reserved1         :7; // 0 [6:0]
  UINT32  V1p65             :1; // 1.65V-1.95V [7:7]     
  UINT32  V2p0              :7; // 2.00V-2.60V [14:8]  
  UINT32  V2p7              :9; // 2.70V-3.60V [23:15]      
  UINT32  Reserved2         :5; // Reserved [30:24]
  UINT32  AccessMode        :2; // Access mode [30:29]
  UINT32  CardBusy          :1; // This bit is set to LOW if the card has not finished the power up routine [31:31]
} MMC_OCR;

typedef struct {
  UINT8   NotUsed           :1; // 1 [0:0]
  UINT8   Checksum          :7; // CRC7 checksum [7:1]
  UINT8   ManufacturingDate;    // Manufacturing date [15:8]
  UINT32  ProductSerialNumber;  // Product serial number [47:16]
  UINT8   ProductRevision;      // Product revision [55:48]
  UINT8   ProductName[6];       // Product name [103:56]
  UINT8   OemApplicationId;     // OEM/Application ID [111:104]
  UINT8   CardBGA           :2; // [113:112]
  UINT8   Reserved1         :6; // Reserved [119:114]
  UINT8   ManufacturerId;       // Manufacturer ID [127:120]
} MMC_CID;

typedef struct {
  UINT8   NotUsed                 :1; // Not used, always 1 [0:0]
  UINT8   CRC                     :7; // CRC [7:1]
  UINT8   ECC                     :2; // ECC Code [9:8] 
  UINT8   FileFormat              :2; // File format [11:10]
  UINT8   TemporaryWriteProtect   :1; // Temporary write protection [12:12]
  UINT8   PermanentWriteProtect   :1; // Permanent write protection [13:13]
  UINT8   CopyFlag                :1; // Copy flag (OTP) [14:14]
  UINT8   FileFormatGroup         :1; // File format group [15:15]
  UINT16  ContentProtectApp       :1; // Content Protection Application [16:16]
  UINT16  Reserved                :4; // Reserved [20:17]
  UINT16  WriteBlkPartial         :1; // Partial blocks for write allowed [21:21]
  UINT16  MaxWriteBlkLen          :4; // Max. write data block length [25:22]
  UINT16  WriteSpeedFactor        :3; // Write speed factor [28:26]
  UINT16  DefaultECC              :2; // Manufacturer default ECC [30:29]
  UINT16  WriteProtectGroupEnable :1; // Write protect group enable [31:31]
  UINT32  WriteProtectGroupSize   :5; // Write protect group size [36:32]
  UINT32  EraseGroupSizeMult      :5; // Erase group size multiplier [41:37]
  UINT32  EraseGroupSize          :5; // Erase group size [46:42]
  UINT32  DeviceSizeMult          :3; // Device size multiplier [49:47]
  UINT32  VddMaxWriteCurrent      :3; // Max. write current @ VDD max [52:50]
  UINT32  VddMinWriteCurrent      :3; // Max. write current @ VDD min [55:53]
  UINT32  VddMaxReadCurrent       :3; // Max. read current @ VDD max [58:56]
  UINT32  VddMinReadCurrent       :3; // Max. read current @ VDD min [61:59]
  UINT32  DeviceSizeLow2          :2; // Device size [63:62]
  UINT32  DeviceSizeHigh10        :10;// Device size [73:64]
  UINT32  Reserved3               :2; // Reserved [75:74]
  UINT32  DSRImplemented          :1; // DSR implemented [76:76]
  UINT32  ReadBlkMisalign         :1; // Read block misalignment [77:77]
  UINT32  WriteBlkMisalign        :1; // Write block misalignment [78:78]
  UINT32  ReadBlkPartial          :1; // Partial blocks for read allowed [79:79]
  UINT32  MaxReadBlkLen           :4; // Max. read data block length [83:80]
  UINT32  CardCommandClasses      :12;// Card command classes [95:84]
  UINT8   MaxTransferRate;            // Max. bus clock frequency [103:96]
  UINT8   Nsac;                       // Data read access-time 2 in CLK cycles (NSAC*100) [111:104]
  UINT8   Taac;                       // Data read access-time 1 [119:112]
  UINT8   Reserved4               :2; // Reserved [121:120]
  UINT8   SpecVersion             :4; // Specification version [125:122]
  UINT8   CsdStructure            :2; // CSD structure [127:126]
} MMC_CSD;

typedef struct {
  //
  // Modes Segment
  //
  UINT8   Reserved133_0[16];          // Reserved [15:0]
  UINT8   SecureRemovalType;          // Secure Removal Type [16:16]
  UINT8   ProdStateAwarenessEnablement;   // Product state awareness enablement [17:17]
  UINT8   MaxPreLoadingDataSize[4];   // Max pre loading data size [21:18]
  UINT8   PreLoadingDataSize[4];      // Pre loading data size [25:22]
  UINT8   FfuStatus;                  // FFU status [26:26]
  UINT8   Reserved28_27[2];           // Reserved [28:27]
  UINT8   ModeOperationCodes;         // Mode operation codes [29:29]
  UINT8   ModeConfig;                 // Mode config [30:30]
  UINT8   Reserved31;                 // Reserved [31:31]
  UINT8   FlushCache;                 // Flushing of the cache [32:32]
  UINT8   CacheCtrl;                  // Control to turn the Cache ON/OFF [33:33]
  UINT8   PowerOffNotification;       // Power Off Notification [34:34]
  UINT8   PackedFailureIndex;         // Packed command failure index [35:35]
  UINT8   PackedCommandStatus;        // Packed command status [36:36] 
  UINT8   ContextConf[15];            // Context configuration [51:37] 
  UINT8   ExtPartitionsAttribute[2];  // Extended Partitions Attribute [53:52] 
  UINT8   ExceptionEventsStatus[2];   // Exception events status [55:54] 
  UINT8   ExceptionEventsCtrl[2];     // Exception events control [57:56] 
  UINT8   DyncapNeeded;               // Number of addressed group to be Released [58:58] 
  UINT8   Class6Ctrl;                 // Class 6 commands control [59:59] 
  UINT8   IniTimeoutEmu;              // 1st initialization after disabling sector size emulation [60:60] 
  UINT8   DataSectorSize;             // Sector size [61:61] 
  UINT8   UseNativeSector;            // Sector size emulation [62:62] 
  UINT8   NativeSectorSize;           // Native sector size [63:63] 
  UINT8   VendorSpecificField[64];    // Vendor Specific Fields [127:64]
  UINT8   Reserved129_128[2];         // Reserved [129:128]
  UINT8   ProgCidCsdDdrSupport;       // Program CID/CSD in DDR mode support [130:130] 
  UINT8   PeriodicWakeup;             // Periodic Wake-up [131:131] 
  UINT8   TcaseSupport;               // Package Case Temperature is controlled [132:132] 
  UINT8   ProdStateAwareness;         // Production state awareness [133:133] 
  UINT8   SecBadBlkMgmnt;             // Bad Block Management mode [134] 
  UINT8   Reserved135;                // Reserved [135]
  UINT8   EnhStartAddr[4];            // Enhanced User Data Start Address [139:136] 
  UINT8   EnhSizeMul[3];              // Enhanced User Data Start Size [142:140] 
  UINT8   GpSizeMul1[3];              // GPP1 Size [145:143]  
  UINT8   GpSizeMul2[3];              // GPP2 Size [148:146] 
  UINT8   GpSizeMul3[3];              // GPP3 Size [151:149] 
  UINT8   GpSizeMul4[3];              // GPP4 Size [154:152] 
  UINT8   PartitionSetCompleted;      // Partitioning Setting [155] 
  UINT8   PartitionAttributes;        // Partitions attributes [156] 
  UINT8   MaxEnhSizeMul[3];           // GPP4 Start Size [159:157] 
  UINT8   PartitionSupport;           // Partitioning Support [160] 
  UINT8   HpiMgmt;                    // HPI management [161] 
  UINT8   RstFun;                     // H/W reset function [162] 
  UINT8   BkOpEn;                     // Enable background operations handshake [163] 
  UINT8   BkOpStart;                  // Manually start background operations [164] 
  UINT8   SanitizeStart;              // Start Sanitize operation [165:165]
  UINT8   WrRelParam;                 // Write reliability parameter register [166] 
  UINT8   WrRelSet;                   // Write reliability setting register [167] 
  UINT8   RpmbSizeMul;                // RPMB Size [168] 
  UINT8   FwCfg;                      // FW configuration [169] 
  UINT8   Reserved170;                // Reserved [170] 
  UINT8   UserWp;                     // User area write protection [171] 
  UINT8   Reserved172;                // Reserved [172]
  UINT8   BootWp;                     // Boot area write protection [173] 
  UINT8   Reserved174;                // Reserved [174]
  UINT8   EraGroupDef;                // High density erase group definition [175] 
  UINT8   Reserved176;                // Reserved [176]
  UINT8   BootBusWidth;               // Boot bus width [177] 
  UINT8   BootCfgProtect;             // Boot config protection [178] 
  UINT8   PartitionCfg;               // Partition config [179] 
  UINT8   Reserved180;                // Reserved [180]
  UINT8   EraMemContent;              // Erased Memory Content [181]
  UINT8   Reserved182;                // Reserved [182]
  UINT8   BusWidth;                   // Bus Width Mode [183:183]
  UINT8   Reserved2;                  // Reserved [184:184]
  UINT8   HighSpeedTiming;            // High Speed Interface Timing [185:185]
  UINT8   Reserved3;                  // Reserved [186:186]
  UINT8   PowerClass;                 // PowerClass [187:187]
  UINT8   Reserved4;                  // Reserved [188:188]
  UINT8   CmdSetRev;                  // Command Set Revision [189:189]
  UINT8   Reserved5;                  // Reserved [190:190]
  UINT8   CmdSet;                     // Command Set [191:191]
  //
  // Properties Segment
  //
  UINT8   ExtCsdRev;                  // Extended CSD Revision [192:192]
  UINT8   Reserved6;                  // Reserved [193:193]
  UINT8   CsdVer;                     // CSD Structure Version [194:194]
  UINT8   Reserved7;                  // Reserved [195:195]
  UINT8   DeviceType;                 // Device Type [196:196]    
  UINT8   DriverStrength;             // I/O Driver Strength [197:197]    
  UINT8   OutOfInterruptTime;         // Out-of-interrupt busy timing [198:198]    
  UINT8   PartitionSwitchTime;        // Partition switching timing [199:199]    
  UINT8   PwrClass52Mhz195;           // Power Class for 52MHz @ 1.95V [200:200]
  UINT8   PwrClass26Mhz195V;          // Power Class for 26MHz @ 1.95V [201:201]
  UINT8   PwrClass52Mhz360V;          // Power Class for 52MHz @ 3.60V [202:202]
  UINT8   PwrClass26Mhz360V;          // Power Class for 26MHz @ 3.60V [203:203]
  UINT8   Reserved8;                  // Reserved [204:204]
  UINT8   MinPerfR4B26Mhz;            // Minimum Read Performance for 4bit @ 26 MHz [205:205]
  UINT8   MinPerfW4B26Mhz;            // Minimum Write Performance for 4bit @ 26 MHz [206:206]    
  UINT8   MinPerfR8B26Mhz4B52Mhz;     // Minimum Read Performance for 8bit @ 26 MHz / 4bit @52MHz [207:207]    
  UINT8   MinPerfW8B26Mhz4B52Mhz;     // Minimum Write Performance for 8bit @ 26 MHz / 4bit @52MHz [208:208]
  UINT8   MinPerfR8B52Mhz;            // Minimum Read Performance for 8bit @ 52 MHz [209:209]
  UINT8   MinPerfW8B52Mhz;            // Minimum Write Performance for 8bit @ 52 MHz [210:210]
  UINT8   Reserved211;                // Reserved [211] 
  UINT8   SecCnt[4];                  // Sector Count [215:212] 
  UINT8   SlpNotificationTime;        // Sleep Notification Timout [216] 
  UINT8   SlpAwkTimeout;              // Sleep/awake timeout [217] 
  UINT8   ProdStateAwarenessTimeout;  // Production state awareness timeout [218] 
  UINT8   SlpCurrentVccq;             // Sleep current (VCCQ) [219] 
  UINT8   SlpCurrentVcq;              // Sleep current (VCC) [220] 
  UINT8   HcWpGrpSize;                // High-capacity write protect group size [221] 
  UINT8   RelWrSecCnt;                // Reliable write sector count [222] 
  UINT8   EraTimeoutMul;              // High-capacity erase timeout [223] 
  UINT8   HcEraGrpSize;               // High-capacity erase unit size [224] 
  UINT8   AccSize;                    // Access size [225] 
  UINT8   BootSizeMul;                // Boot partition size [226] 
  UINT8   Reserved227;                // Reserved [227] 
  UINT8   BootInfo;                   // Boot information [228] 
  UINT8   SecTrimMul;                 // Secure TRIM Multiplier [229] 
  UINT8   SecEraMul;                  // Secure Erase Multiplier [230] 
  UINT8   SecFeatureSupport;          // Secure Feature support [231] 
  UINT8   TrimMul;                    // TRIM Multiplier [232] 
  UINT8   Reserved233;                // Reserved [233] 
  UINT8   MinPerfWDdrR8B52MHz;        // [Min Read Performance for 8-bit @ 52MHz 234] 
  UINT8   MinPerfDdrW8B52MHz;         // Min Write Performance for 8-bit @ 52MHz [235] 
  UINT8   PwrCl200_130;               // Power class for 200MHz at VCCQ =1.3V, VCC = 3.6V [236] 
  UINT8   PwrCl200_195;               // Power class for 200MHz at VCCQ =1.95V, VCC = 3.6V [237] 
  UINT8   PwrClDdr52Mhz195V;          // Power class for 52MHz, DDR at 1.95V [238] 
  UINT8   PwrClDdr52Mhz360V;          // Power class for 52MHz, DDR at 3.6V [239] 
  UINT8   Reserved240;                // Reserved [240]
  UINT8   IniTimeOutAp;               // 1st initialization time after partitioning [241] 
  UINT8   CorrectPrgSecNum[4];        // Number of correctly programmed sectors [245:242] 
  UINT8   BkOpStatus;                 // Background operations status [246:246] 
  UINT8   PowerOffLongTime;           // Power off notification(long) timeout [247:247] 
  UINT8   GenericCmd6Time;            // Generic CMD6 timeout [248:248] 
  UINT8   CacheSize[4];               // Cache size [252:249] 
  UINT8   PwrClDdr200_360;            // Power class for 200MHz, DDR at VCC= 3.6V [253:253] 
  UINT8   FirmwareVersion[8];         // Firmware version [261:254] 
  UINT8   DeviceVersion[2];           // Device version [263:262] 
  UINT8   OptimalTrimUnitSize;        // Optimal trim unit size [264:264] 
  UINT8   OptimalWriteSize;           // Optimal write size [265:265] 
  UINT8   OptimalReadSize;            // Optimal read size [266:266] 
  UINT8   PreEolInfo;                 // Pre EOL information [267:267] 
  UINT8   DeviceLifeTimeEstTypA;      // Device life time estimation type A [268:268] 
  UINT8   DeviceLifeTimeEstTypB;      // Device life time estimation type B [269:269] 
  UINT8   VendorPtyHealthReport[32];  // Vendor proprietary health report [301:270]
  UINT8   NumOfFwSectorCorrectlyProgrammed[4];    // Number of FW sectors correctly programmed [305:302]
  UINT8   Reserved9[181];             // Reserved [486:306]
  UINT8   FfuArg[4];                  // FFU Argument [490:487]
  UINT8   OperationCodeTimeout;       // Operation codes timeout [491:491]
  UINT8   FfuFeatures;                // FFU features [492:492]
  UINT8   SupportedModes;             // Supported modes [493:493]
  UINT8   ExtSupport;                 // Extended partitions attribute support [494:494]
  UINT8   LargeUnitSizeM1;            // Large Unit size [495:495]
  UINT8   ContextCapabilities;        // Context management capabilities [496:496]
  UINT8   TagResSize;                 // Tag Resources Size [497:497]
  UINT8   TagUnitSize;                // Tag Unit Size [498:498]
  UINT8   DataTagSupport;             // Data Tag Support [499:499]
  UINT8   MaxPackedWrites;            // Max packed write commands [500:500]
  UINT8   MaxPackedReads;             // Max packed read commands [501:501]
  UINT8   BkOpSupport;                // Background operations support [502:502]
  UINT8   HpiFeatures;                // HPI features [503:503]
  UINT8   SupportCmdSet;              // Supported Command Sets [504:504]
  UINT8   ExtSecCmdErr;               // Extended Security Commands Error [505:505]
  UINT8   Reserved10[6];              // Reserved [511:506]
} MMC_EXT_CSD;

//
// MMC Card Status definition
//
typedef struct {
  UINT32  Reserved0:           2; //Reserved for Manufacturer Test Mode
  UINT32  Reserved1:           2; //Reserved for Application Specific commands
  UINT32  Reserved2:           1; //
  UINT32  SappCmd:             1; //
  UINT32  Reserved3:           1; //Reserved
  UINT32  SwitchError:         1; //
  UINT32  ReadyForData:        1; //
  UINT32  CurrentState:        4; //
  UINT32  EraRest:             1; //
  UINT32  Reserved4:           1; //Reserved
  UINT32  WpEraSkip:           1; //
  UINT32  CidCsdOverWr:        1; //
  UINT32  OverRun:             1; //
  UINT32  UnderRun:            1; //
  UINT32  Error:               1; //
  UINT32  CcError:             1; //
  UINT32  CardEccFailed:       1; //
  UINT32  IllCmd:              1; //
  UINT32  ComCrcError:         1; //
  UINT32  LockUnLockFailed:    1; //
  UINT32  CardIsLocked:        1; //
  UINT32  WpViolation:         1; //
  UINT32  EraParam:            1; //
  UINT32  EraSeqError:         1; //
  UINT32  BlkLenError:         1; //
  UINT32  AddrMisalign:        1; //
  UINT32  AddrOutOfRange:      1; //
}CARD_STATUS;

//
// Command Arguments
//
typedef union {  
  struct {
    UINT8   CmdSet            :3; // Command Set [2:0]
    UINT8   Reserved1         :5; // Reserved [7:3]     
    UINT8   Value;                // Value [15:8]  
    UINT8   Index;                // Index [23:16]      
    UINT8   Access            :2; // Access mode [25:24]
    UINT8   Reserved2         :6; // Reserved [31:26]
  } Bit;
  UINT32  Value;
} CMD6_ARG;

#pragma pack()
#endif
