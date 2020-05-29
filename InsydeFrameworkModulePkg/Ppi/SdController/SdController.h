//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   SdController.h
//; 

#ifndef _PEI_SD_CONTROLLER_PPI_H_
#define _PEI_SD_CONTROLLER_PPI_H_

#define PEI_SD_CONTROLLER_PPI_GUID \
  { \
    0xc0e300dc, 0x1f04, 0x4f36, 0xa6, 0xb1, 0xea, 0xc6, 0x70, 0x90, 0x9a, 0x77 \
  }

#define PEI_SDHC_CONTROLLER 0x01

EFI_FORWARD_DECLARATION (PEI_SD_CONTROLLER_PPI);


#pragma pack(1)
typedef struct {
  UINT16           BlockCount;
  UINT32           Argument;
  UINT16           TransMode;
  UINT16           Command;
  UINT16           NormalIntEnable;
  UINT16           ErrorIntEnable;
} SD_CMD_PARAMETER;

typedef struct {
  UINT16   Response[8];
} SD_RESPONSE;

typedef struct {
  UINT32  Reserved1         :15;// 0
  UINT32  V27               :1; // 2.70V
  UINT32  V28               :1; // 2.80V
  UINT32  V29               :1; // 2.90V
  UINT32  V30               :1; // 3.00V
  UINT32  V31               :1; // 3.10V
  UINT32  V32               :1; // 3.20V
  UINT32  V33               :1; // 3.30V
  UINT32  V34               :1; // 3.40V
  UINT32  V35               :1; // 3.50V
  UINT32  S18A              :1; // Switching to 1.8V Accepted (S18A)
  UINT32  Reserved2         :5; // Reserved
  UINT32  CardCapacityStatus:1; // Card Capacity Status (CCS)
  UINT32  CardBusy          :1; // This bit is set to LOW if the card has not finished the power up routine
} SD_OCR;

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
  UINT16  ManufacturingDate :12;// Manufacturing date [19:8]
  UINT16  Reserved1         :4; // Reserved [23:20]
  UINT32  ProductSerialNumber;  // Product serial number [55:24]
  UINT8   ProductRevision;      // Product revision [63:56]
  UINT8   ProductName[5];       // Product name [64:103]
  UINT16  OemApplicationId;     // OEM/Application ID [119:104]
  UINT8   ManufacturerId;       // Manufacturer ID [127:120]
} SD_CID;

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
  UINT8   Reserved1               :2; // Reserved [9:8]
  UINT8   FileFormat              :2; // File format [11:10]
  UINT8   TemporaryWriteProtect   :1; // Temporary write protection [12:12]
  UINT8   PermanentWriteProtect   :1; // Permanent write protection [13:13]
  UINT8   CopyFlag                :1; // Copy flag (OTP) [14:14]
  UINT8   FileFormatGroup         :1; // File format group [15:15]
  UINT16  Reserved2               :5; // Reserved [20:16]
  UINT16  WriteBlkPartial         :1; // Partial blocks for write allowed [21:21]
  UINT16  MaxWriteBlkLen          :4; // Max. write data block length [25:22]
  UINT16  WriteSpeedFactor        :3; // Write speed factor [28:26]
  UINT16  Reserved3               :2; // Reserved [30:29]
  UINT16  WriteProtectGroupEnable :1; // Write protect group enable [31:31]
  UINT32  WriteProtectGroupSize   :7; // Write protect group size [38:32]
  UINT32  EraseSectorSize         :7; // Erase sector size [45:39]
  UINT32  EraseSingleBlkEnable    :1; // Erase single block enable [46:46]
  UINT32  DeviceSizeMult          :3; // Device size multiplier [49:47]
  UINT32  VddMaxWriteCurrent      :3; // Max. write current @ VDD max [52:50]
  UINT32  VddMinWriteCurrent      :3; // Max. write current @ VDD min [55:53]
  UINT32  VddMaxReadCurrent       :3; // Max. read current @ VDD max [58:56]
  UINT32  VddMinReadCurrent       :3; // Max. read current @ VDD min [61:59]
  UINT32  DeviceSizeLow2          :2; // Device size [63:62]
  UINT32  DeviceSizeHigh10        :10;// Device size [73:64]
  UINT32  Reserved4               :2; // Reserved [75:74]
  UINT32  DSRImplemented          :1; // DSR implemented [76:76]
  UINT32  ReadBlkMisalign         :1; // Read block misalignment [77:77]
  UINT32  WriteBlkMisalign        :1; // Write block misalignment [78:78]
  UINT32  ReadBlkPartial          :1; // Partial blocks for read allowed [79:79]
  UINT32  MaxReadBlkLen           :4; // Max. read data block length [83:80]
  UINT32  CardCommandClasses      :12;// Card command classes [95:84]
  UINT8   MaxTransferRate;            // Max. bus clock frequency [103:96]
  UINT8   Nsac;                       // Data read access-time 2 in CLK cycles (NSAC*100) [111:104]
  UINT8   Taac;                       // Data read access-time 1 [119:112]
  UINT8   Reserved5               :6; // Reserved [125:120]
  UINT8   CsdStructure            :2; // CSD structure [127:126]
} SD_CSD1;

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
  UINT8   Reserved1[183];             // Reserved [182:0]
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
  UINT8   CardType;                   // Card Type [196:196]    
  UINT8   Reserved[3];                // Reserved [199:197]    
  UINT8   PwrClass52Hz195V;           // Power Class for 52MHz @ 1.95V [200:200]
  UINT8   PwrClass26Hz195V;           // Power Class for 26MHz @ 1.95V [201:201]
  UINT8   PwrClass52Hz360V;           // Power Class for 52MHz @ 3.60V [202:202]
  UINT8   PwrClass26Hz360V;           // Power Class for 26MHz @ 3.60V [203:203]
  UINT8   Reserved8;                  // Reserved [204:204]
  UINT8   MinPerfR4B26Hz;             // Minimum Read Performance for 4bit @ 26 MHz [205:205]
  UINT8   MinPerfW4B26Hz;             // Minimum Write Performance for 4bit @ 26 MHz [206:206]    
  UINT8   MinPerfR8B26Hz4B52Hz;       // Minimum Read Performance for 8bit @ 26 MHz / 4bit @52MHz [207:207]    
  UINT8   MinPerfW8B26Hz4B52Hz;       // Minimum Write Performance for 8bit @ 26 MHz / 4bit @52MHz [208:208]
  UINT8   MinPerfR8B52Hz;             // Minimum Read Performance for 8bit @ 52 MHz [209:209]
  UINT8   MinPerfW8B52Hz;             // Minimum Write Performance for 8bit @ 52 MHz [210:210]
  UINT8   Reserved9[293];             // Reserved [503:211]
  UINT8   SupportCmdSet;              // Supported Command Sets [504:504]
  UINT8   Reserved10[7];              // Reserved [511:505]
} MMC_EXT_CSD;

typedef struct {
  UINT8   NotUsed                 :1; // Not used, always 1 [0:0]
  UINT8   CRC                     :7; // CRC [7:1]
  UINT8   Reserved1               :2; // Reserved [9:8]
  UINT8   FileFormat              :2; // File format [11:10]
  UINT8   TemporaryWriteProtect   :1; // Temporary write protection [12:12]
  UINT8   PermanentWriteProtect   :1; // Permanent write protection [13:13]
  UINT8   CopyFlag                :1; // Copy flag (OTP) [14:14]
  UINT8   FileFormatGroup         :1; // File format group [15:15]
  UINT16  Reserved2               :5; // Reserved [20:16]
  UINT16  WriteBlkPartial         :1; // Partial blocks for write allowed [21:21]
  UINT16  MaxWriteBlkLen          :4; // Max. write data block length [25:22]
  UINT16  WriteSpeedFactor        :3; // Write speed factor [28:26]
  UINT16  Reserved3               :2; // Reserved [30:29]
  UINT16  WriteProtectGroupEnable :1; // Write protect group enable [31:31]
  UINT16  WriteProtectGroupSize   :7; // Write protect group size [38:32]
  UINT16  EraseSectorSize         :7; // Erase sector size [45:39]
  UINT16  EraseSingleBlkEnable    :1; // Erase single block enable [46:46]
  UINT16  Reserved4               :1; // Reserved [47:47]
  UINT32  DeviceSizeLow16         :16;// Device size [69:48]
  UINT32  DeviceSizeHigh6         :6; // Device size [69:48]
  UINT32  Reserved5               :6; // Reserved [75:70]
  UINT32  DSRImplemented          :1; // DSR implemented [76:76]
  UINT32  ReadBlkMisalign         :1; // Read block misalignment [77:77]
  UINT32  WriteBlkMisalign        :1; // Write block misalignment [78:78]
  UINT32  ReadBlkPartial          :1; // Partial blocks for read allowed [79:79]
  UINT16  MaxReadBlkLen           :4; // Max. read data block length [83:80]
  UINT16  CardCommandClasses      :12;// Card command classes [95:84]
  UINT8   MaxTransferRate;            // Max. bus clock frequency [103:96]
  UINT8   Nsac;                       // Data read access-time 2 in CLK cycles (NSAC*100) [111:104]
  UINT8   Taac;                       // Data read access-time 1 [119:112]
  UINT8   Reserved6               :6; // 0 [125:120]
  UINT8   CsdStructure            :2; // CSD structure [127:126]
} SD_CSD2;

typedef enum {
  UNKNOWN_CARD,
  MMC_CARD,              //MMC card
  SD_CARD,               //SD 1.1 card
  SD_CARD_2,             //SD 2.0 or above standard card
  SD_CARD_2_HIGH         //SD 2.0 or above high capacity card
} CARD_TYPE;

typedef struct  {
  UINT16    RCA;
  UINT32    BlockSize;
  UINTN     NumBlocks;
  UINTN     ClockFrequencySelect;
  CARD_TYPE CardType;
  SD_OCR      SdOcrData;
  SD_CID      SdCidData;
  SD_CSD1     SdCsd1Data;
  SD_CSD2     SdCsd2Data;
  MMC_OCR     MmcOcrData;
  MMC_CID     MmcCidData;
  MMC_CSD     MmcCsdData;
  MMC_EXT_CSD MmcCsdExData;
} CARD_INFO;

#pragma pack()

typedef
EFI_STATUS
(EFIAPI *PEI_GET_SD_CONTROLLER) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_SD_CONTROLLER_PPI          * This,
  IN UINT8                          SdControllerId,
  OUT UINTN                         *ControllerType,
  OUT UINT64                        *PciCfgAddress
  );

typedef struct _PEI_SD_CONTROLLER_PPI {
  PEI_GET_SD_CONTROLLER  GetSdController;
} PEI_SD_CONTROLLER_PPI;

extern EFI_GUID gPeiSdControllerPpiGuid;

#endif
