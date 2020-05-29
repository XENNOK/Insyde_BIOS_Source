//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  HddPasswordService.h

Abstract:

  Protocol used for HDD Password.

--*/

#ifndef _EFI_HDD_PASSWORD_SERVICE_PROTOCOL_H
#define _EFI_HDD_PASSWORD_SERVICE_PROTOCOL_H

#define EFI_HDD_PASSWORD_SERVICE_PROTOCOL_GUID \
  { \
    0x9c28be0c, 0xee32, 0x43d8, 0xa2, 0x23, 0xe7, 0xc1, 0x61, 0x4e, 0xf7, 0xca \
  }


#define HDD_PASSWORD_MAX_NUMBER            32
#define DEFAULT_RETRY_COUNT                10

EFI_FORWARD_DECLARATION (EFI_HDD_PASSWORD_SERVICE_PROTOCOL);

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
    UINT16  reserved_160_255[96];

} IDENTIFY_TAB;


//
// IDE Registers
//
typedef union {
  UINT16  Command;        // when write
  UINT16  Status;         // when read
} IDE_CMD_OR_STATUS;

typedef union {
  UINT16  Error;          // when read
  UINT16  Feature;        // when write
} IDE_ERROR_OR_FEATURE;

typedef union {
  UINT16  AltStatus;      // when read
  UINT16  DeviceControl;  // when write
} IDE_AltStatus_OR_DeviceControl;

//
// IDE registers set
//
typedef struct {
  UINT16                          Data;
  IDE_ERROR_OR_FEATURE            Reg1;
  UINT16                          SectorCount;
  UINT16                          SectorNumber;
  UINT16                          CylinderLsb;
  UINT16                          CylinderMsb;
  UINT16                          Head;
  IDE_CMD_OR_STATUS               Reg;

  IDE_AltStatus_OR_DeviceControl  Alt;
  UINT16                          DriveAddress;

  UINT16                          MasterSlave;
  UINT16                          BusMasterBaseAddr;
} IDE_BASE_REGISTERS;

typedef struct {
  IDENTIFY_TAB        *IdentifyTable;
  UINT8               HddSecurityStatus;
  CHAR16              HddModelString[20];
  UINT8               ControllerMode;
  UINT8               ControllerNumber;
  UINT16              PortNumber;
  UINT16              PortMulNumber;
  UINT16              MappedPort;
  UINTN               PciDevice;
  UINTN               PciFunction;
  UINTN               TotalNumOfHddInfo;
} HDD_SECURITY_INFO_DATA;

typedef struct HDD_PASSWORD_INFO_DATA{
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
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray,
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
    IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  HDD_SECURITY_INFO_DATA               *HddSecurityInfoArray,
    IN OUT UINT16                            *IdentifyTable
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_HDD_INFO) (
    IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This,
    IN OUT UINTN                             **HddInfoTable
);

typedef
EFI_STATUS
(EFIAPI *EFI_HDD_RESET) (
    IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FEATURES_CMD) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL  *This,
  IN  HDD_SECURITY_INFO_DATA             *HddSecurityInfoArray,
  IN  UINT8                              AtaFeatures,
  IN  UINT8                              AtaSectorCount
);

typedef
EFI_STATUS
(EFIAPI *EFI_UNLOCK_ALL_HDD) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddInfoPtr,
  IN  UINTN                             NumOfHarddisk
);

typedef
EFI_STATUS
(EFIAPI *EFI_HDD_FREEZE) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_SECURITY_INFO_DATA            *HddSecurityInfoArray
);

typedef
EFI_STATUS
(EFIAPI* EFI_HDD_GET_SKIP_DIALOG_KEY) (
  IN EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN EFI_INPUT_KEY                     *SkipDialogKey
);

typedef
EFI_STATUS
(EFIAPI* EFI_HDD_GET_SKIP_HOT_KEY_HINT) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  OUT CHAR16                            **SkipMsg
);

typedef
EFI_STATUS
(EFIAPI* EFI_CHECK_SKIP_DIALOG_KEY) (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  EFI_INPUT_KEY                     *SkipDialogKey
);

typedef struct _EFI_HDD_PASSWORD_SERVICE_PROTOCOL {
  EFI_SET_HDD_PASSWROD                  SetHddPassword;
  EFI_UNLOCK_HDD_PASSWORD               UnlockHddPassword;
  EFI_DISABLE_SERVICE                   DisableService;
  EFI_HDD_IDENTIFY                      HddIdentify;
  EFI_GET_HDD_INFO                      GetHddSecurityInfo;
  EFI_SET_FEATURES_CMD                  SetFeaturesCmd;
  EFI_HDD_RESET                         HddReset;
  EFI_UNLOCK_ALL_HDD                    UnlockAllHdd;
  EFI_HDD_FREEZE                        HddFreeze;
  EFI_HDD_GET_SKIP_DIALOG_KEY           HddGetSkipDialogKey;
  EFI_HDD_GET_SKIP_HOT_KEY_HINT         HddGetSkipHotKeyHint;
  EFI_CHECK_SKIP_DIALOG_KEY             CheckSkipDialogKey;
} EFI_HDD_PASSWORD_SERVICE_PROTOCOL;



extern EFI_GUID gEfiHddPasswordServiceProtocolGuid;
#endif
