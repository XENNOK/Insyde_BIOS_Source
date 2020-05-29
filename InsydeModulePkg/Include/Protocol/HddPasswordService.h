/** @file
  Definitions of hard disk password variable

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_HDD_PASSWORD_SERVICE_PROTOCOL_H
#define _EFI_HDD_PASSWORD_SERVICE_PROTOCOL_H

#define EFI_HDD_PASSWORD_SERVICE_PROTOCOL_GUID \
  { \
    0x9c28be0c, 0xee32, 0x43d8, 0xa2, 0x23, 0xe7, 0xc1, 0x61, 0x4e, 0xf7, 0xca \
  }

typedef struct _EFI_HDD_PASSWORD_SERVICE_PROTOCOL EFI_HDD_PASSWORD_SERVICE_PROTOCOL;


#define HDD_PASSWORD_MAX_NUMBER            32
#define DEFAULT_RETRY_COUNT                10
#define ATA_IDE_MODE                        0
#define ATA_AHCI_MODE                       1
#define ATA_RAID_MODE                       2
#define ATA_IDER_MODE                       3

typedef struct {
    UINT16  config;             /* General Configuration */

    UINT16  cylinders;          /* Number of Cylinders */

    UINT16  reserved_2;

    UINT16  heads;              /* Number of logical heads */
    UINT16  vendor_data1;
    UINT16  vendoe_data2;

    UINT16  sectors_per_track;
    UINT16  vendor_specific_7_9[3];

    CHAR8   SerialNo[20];       /* ASCII */

    UINT16  vendor_specific_20_21[2];

    UINT16  ecc_bytes_available;

    CHAR8   FirmwareVer[8];     /* ASCII */

    CHAR8   ModelName[40];      /* ASCII */

    UINT16  multi_sector_cmd_max_sct_cnt;
    UINT16  reserved_48;

    UINT16  capabilities;

    UINT16  reserved_50;

    UINT16  pio_cycle_timing;

    UINT16  reserved_52;

    UINT16  field_validity;

    UINT16  current_cylinders;
    UINT16  current_heads;
    UINT16  current_sectors;

    UINT16  CurrentCapacityLsb;
    UINT16  CurrentCapacityMsb;

    UINT16  reserved_59;

    UINT16  user_addressable_sectors_lo;
    UINT16  user_addressable_sectors_hi;

    UINT16  reserved_62;

    UINT16  multi_word_dma_mode;

    UINT16  advanced_pio_modes;

    UINT16  min_multi_word_dma_cycle_time;
    UINT16  rec_multi_word_dma_cycle_time;
    UINT16  min_pio_cycle_time_without_flow_control;
    UINT16  min_pio_cycle_time_with_flow_control;

    UINT16  reserved_69_79[11];

    UINT16  major_version_no;
    UINT16  minor_version_no;

    //UINT16  reserved_82_127[46];
    UINT16  command_set_supported_82; // word 82
    UINT16  command_set_supported_83; // word 83
    UINT16  command_set_feature_extn; // word 84
    UINT16  command_set_feature_enb_85; // word 85
    UINT16  command_set_feature_enb_86; // word 86
    UINT16  command_set_feature_default; // word 87
    UINT16  ultra_dma_mode; // word 88

    UINT16  reserved_89_127[39];
    UINT16  security_status; // word 128
    UINT16  vendor_data_129_159[31];
    UINT16  reserved_160_216[57];
    UINT16  nominal_media_rotation_rate;
    UINT16  reserved_218_255[38];

} IDENTIFY_TAB;

typedef struct {
  IDENTIFY_TAB                         *IdentifyTable;
  UINT8                                HddSecurityStatus;
  CHAR16                               HddModelString[20];
  UINT8                                ControllerMode;
  UINT8                                ControllerNumber;
  UINT16                               PortNumber;
  UINT16                               PortMulNumber;
  UINT16                               MappedPort;
  UINTN                                PciDevice;
  UINTN                                PciFunction;
  UINTN                                TotalNumOfHddInfo;
} HDD_SECURITY_INFO_DATA;

typedef struct {
  CHAR16                 DisableInputString[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                  DisableStringLength;
  UINT8                  DisableAllType;
  CHAR16                 DisableAllInputString[HDD_PASSWORD_MAX_NUMBER + 1];
  CHAR16                 MasterInputString[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                  MasterFlag;
  UINTN                  Flag;
  CHAR16                 InputString[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                  StringLength;
  HDD_SECURITY_INFO_DATA *HddDataInfo;
  UINTN                  NumOfEntry;
  UINTN                  LabelIndex;
} HDD_PASSWORD_INFO_DATA;

//
//  HDD Password Setup Information
//
#ifndef BITS
#define BITS(a)                        (1 << (a))
#endif
#define MAX_HDD_PASSWORD_LENGTH_BIT    BITS(0)
#define SKIP_HOT_KEY_BIT               BITS(1)
#define SKIP_HOT_KEY_BIT_MSG           BITS(2)
#define ENCODE_FUNCTION_BIT            BITS(3)
#define RETRY_COUNT_BIT                BITS(4)

#define SETUP_INFO_DATA_MASK           (BITS(5) - 1)

#define NOT_IN_SMM                     0
#define IN_SMM                         1

typedef
EFI_STATUS
(EFIAPI *EFI_ENCODE) (
  IN     VOID                          *PasswordPtr,
  IN     UINTN                         PasswordLength,
  IN OUT VOID                          **EncodePasswordPtr,
  IN OUT UINTN                         *EncodePasswordLength
  );

typedef struct {
  UINT32                                SetupDataValidBits;
  UINTN                                 MaxHddPasswordLength;
  EFI_INPUT_KEY                         SkipHotKey;
  CHAR16                                *SkipHotKeyMsg;
  EFI_ENCODE                            EncodeFunction;
  UINTN                                 RetryCount;
} HDD_PASSWORD_SETUP_INFO_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_SET_HDD_PASSWROD) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINT8                             *PasswordPtr,
  IN  UINTN                             PassLength
);

typedef
EFI_STATUS
(EFIAPI *EFI_UNLOCK_HDD_PASSWORD) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN HDD_SECURITY_INFO_DATA             *HddSecurityInfoArray,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINT8                             *PasswordPtr,
  IN  UINTN                             PassLength
);

typedef
EFI_STATUS
(EFIAPI *EFI_DISABLE_SERVICE) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINT8                             *PasswordPtr,
  IN  UINTN                             PassLength
);

typedef
EFI_STATUS
(EFIAPI *EFI_HDD_IDENTIFY) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray,
  IN OUT UINT16                         *IdentifyTable
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_HDD_INFO) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  OUT VOID                          **HddInfoTable
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FEATURES_CMD) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray,
  IN  UINT8                             AtaFeatures,
  IN  UINT8                             AtaSectorCount
);

typedef
EFI_STATUS
(EFIAPI *EFI_UNLOCK_ALL_HDD) (
//  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
//  IN  HDD_SECURITY_INFO_DATA            *HddInfoPtr,
//  IN  UINTN                             NumOfHarddisk
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This
);

typedef
EFI_STATUS
(EFIAPI *EFI_HDD_FREEZE) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray
);

typedef
EFI_STATUS
(EFIAPI* EFI_CHECK_SKIP_DIALOG_KEY) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  EFI_INPUT_KEY                     *SkipDialogKey
);

typedef
EFI_STATUS
(EFIAPI* EFI_GET_HDD_PSWD_SETUP_INFO_DATA) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  OUT HDD_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer
);

typedef
EFI_STATUS
(EFIAPI* EFI_SET_HDD_PSWD_SETUP_INFO_DATA) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer,
  IN  UINT32                            SetupBits
);

typedef
EFI_STATUS
(EFIAPI *EFI_HDD_RESET) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This
);

struct _EFI_HDD_PASSWORD_SERVICE_PROTOCOL {
  EFI_SET_HDD_PASSWROD                  SetHddPassword;
  EFI_UNLOCK_HDD_PASSWORD               UnlockHddPassword;
  EFI_DISABLE_SERVICE                   DisableService;
  EFI_HDD_IDENTIFY                      HddIdentify;
  EFI_GET_HDD_INFO                      GetHddSecurityInfo;
  EFI_SET_FEATURES_CMD                  SetFeaturesCmd;
  EFI_HDD_RESET                         HddReset;
  EFI_UNLOCK_ALL_HDD                    UnlockAllHdd;
  EFI_HDD_FREEZE                        HddFreeze;
  EFI_CHECK_SKIP_DIALOG_KEY             CheckSkipDialogKey;
  EFI_GET_HDD_PSWD_SETUP_INFO_DATA      GetHddPswdSetupInfoData;
  EFI_SET_HDD_PSWD_SETUP_INFO_DATA      SetHddPswdSetupInfoData;
};



extern EFI_GUID gEfiHddPasswordServiceProtocolGuid;

#endif
