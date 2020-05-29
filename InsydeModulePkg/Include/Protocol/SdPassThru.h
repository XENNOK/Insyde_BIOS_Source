/** @file
  EFI_SD_PASS_THRU_PROTOCOL

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

#ifndef _SD_PASS_THRU_H_
#define _SD_PASS_THRU_H_

#include <Mmc.h>
#include <SdCard.h>

//
//  EFI SdPassThru protocol
//
#define SD_PASS_THRU_PROTOCOL_GUID \
  {\
    0x593ea8f0, 0x201b, 0x4ade, 0xb4, 0xff, 0x44, 0x70, 0x78, 0x57, 0x57, 0x19 \
  }

typedef struct _SD_PASS_THRU_PROTOCOL   SD_PASS_THRU_PROTOCOL;


#ifdef SD_LEGACY_SMI
#define	SDHC_SW_SMI               SD_LEGACY_SMI
#else
#define	SDHC_SW_SMI               0xF8
#endif

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

typedef enum {
  UNKNOWN_CARD,
  MMC_CARD,              //MMC card
  SD_CARD,               //SD 1.1 card
  SD_CARD_2,             //SD 2.0 or above standard card
  SD_CARD_2_HIGH,        //SD 2.0 or above high capacity card
  SDIO                   //SDIO
} CARD_TYPE;

typedef struct  {
  UINT16      RCA;
  UINT32      BlockSize;
  UINTN       NumBlocks;
  UINT32      ClockFrequencySelect;
  CARD_TYPE   CardType;
  SD_OCR      SdOcrData;
  SD_CID      SdCidData;
  SD_CSD1     SdCsd1Data;
  SD_CSD2     SdCsd2Data;
  MMC_OCR     MmcOcrData;
  MMC_CID     MmcCidData;
  MMC_CSD     MmcCsdData;
  MMC_EXT_CSD MmcCsdExData;
  UINT8       SlotNum;
} CARD_INFO;

//
// SDIO definition (temporary)
//
typedef struct {
  UINT32  Reserved1         :7;// 0
  UINT32  V20_21            :1; // 2.0-2.1V
  UINT32  V21_22            :1; // 2.1-2.2V
  UINT32  V22_23            :1; // 2.2-2.3V
  UINT32  V23_24            :1; // 2.3-2.4V
  UINT32  V24_25            :1; // 2.4-2.5V
  UINT32  V25_26            :1; // 2.5-2.6V
  UINT32  V26_27            :1; // 2.6-2.7V
  UINT32  V27_28            :1; // 2.7-2.8V
  UINT32  V28_29            :1; // 2.8-2.9V
  UINT32  V29_30            :1; // 2.9-3.0V
  UINT32  V30_31            :1; // 3.0-3.1V
  UINT32  V31_32            :1; // 3.1-3.2V
  UINT32  V32_33            :1; // 3.2-3.3V
  UINT32  V33_34            :1; // 3.3-3.4V
  UINT32  V34_35            :1; // 3.4-3.5V
  UINT32  V35_36            :1; // 3.5-3.6V
} SDIO_OCR;

typedef struct {
  SDIO_OCR SdioOcr;
  UINT32   Stuff             :3; // Stuff Bits
  UINT32   MemPresent        :1; // Memory Present
  UINT32   NumOfFunc         :3; // Num
} SDIO_OP_COND;

#pragma pack()

//
// SdhcPassThru
//
/**
  Sends a SD command to a SD/MMC device that is attached to the SD controller.

  @param[in]     This                   Pointer to SD_PASS_THRU_PROTOCOL
  @param[in]     CmdParameters          Pointer to SD command paramters SD_CMD_PARAMETER
  @param[in]     SlotNum                The slot number of the SD/MMC device to send the command
  @param[in,out  DataBuffer             The destination buffer
  @param[in,out] SdResponse             Return the response value of the SD command

  @retval EFI_NOT_FOUND                 Can't find the private data of device
  @retval EFI_NO_MEDIA                  Can't find the device          
  @retval EFI_SUCCESS                   Command was executed successfully
  @retval Other                         Operation failed

**/
typedef
EFI_STATUS
(EFIAPI *SD_PASS_THRU_PASSTHRU) (
  IN SD_PASS_THRU_PROTOCOL     *This,
  IN SD_CMD_PARAMETER          *CmdParameters,
  IN UINT8                     SlotNum,
  IN OUT VOID                  *DataBuffer,
  IN OUT UINT32                *SdResponse
);

//
// SdPassThruGetCardInfo
//
/**
  Get specified card info.

  @param[in] This              Pointer to SD_PASS_THRU_PROTOCOL
  @param[in] CardInfo          Pointer to CARD_INFO structure

  @retval EFI_NO_MEDIA         Can't find the device          
  @retval EFI_SUCCESS          Operation succeeded

**/
typedef
EFI_STATUS
(EFIAPI *SD_PASS_THRU_GET_CARD_INFO) (
  IN  SD_PASS_THRU_PROTOCOL *This,
  OUT CARD_INFO                 *Card_INFO
);

struct _SD_PASS_THRU_PROTOCOL {
  SD_PASS_THRU_PASSTHRU          PassThru;
  SD_PASS_THRU_GET_CARD_INFO     GetCardInfo;
};

extern EFI_GUID gSdPassThruProtocolGuid;

#endif
