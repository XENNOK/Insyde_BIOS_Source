//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _KERNEL_SETUP_CONFIG_H_
#define _KERNEL_SETUP_CONFIG_H_

#define SETUP_UTILITY_CLASS             1
#define SETUP_UTILITY_SUBCLASS          0
#define EFI_USER_ACCESS_TWO             0x04
#define EFI_USER_ACCESS_THREE           0x05
#define ROOT_FORM_ID                    1

#define SYSTEM_CONFIGURATION_GUID {0xA04A27f4, 0xDF00, 0x4D42, 0xB5, 0x52, 0x39, 0x51, 0x13, 0x02, 0x11, 0x3D}
#define PASSWORD_CONFIGURATION_GUID {0xf72deef6, 0x13ef, 0x4958, 0xb0, 0x27, 0xe, 0x45, 0xce, 0x7f, 0xa4, 0x5e}

#define VAR_EQ_GRAY_TEXT                    1

//
// Boot order relative label
//
#define BOOT_ORDER_LABEL                    0x3000
#define FDD_BOOT_DEVICE_LABEL               0x3001
#define HDD_BOOT_DEVICE_LABEL               0x3002
#define CD_BOOT_DEVICE_LABEL                0x3003
#define PCMCIA_BOOT_DEVICE_LABEL            0x3004
#define USB_BOOT_DEVICE_LABEL               0x3005
#define EMBED_NETWORK_BOOT_DEVICE_LABEL     0x3006
#define BEV_BOOT_DEVICE_LABEL               0x3007
#define OTHER_BOOT_DEVICE_LABEL             0x3008
#define EFI_BOOT_DEVICE_LABEL               0x3009
#define BOOT_LEGACY_ADV_BOOT_LABEL          0x300A

//
// Boot type relative definitions
//
#define DUAL_BOOT_TYPE     0x00
#define LEGACY_BOOT_TYPE   0x01
#define EFI_BOOT_TYPE      0x02
#define ADD_POSITION_FIRST     0x00
#define ADD_POSITION_LAST      0x01
#define ADD_POSITION_AUTO      0x02

#define EFI_TIMEOUT_VARIABLE_DEFAULT        0

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
//
// Setup varaiable offset definition
//
#define SETUP_VAR_OFFSET(Field)              ((UINT16)((UINTN)&(((SYSTEM_CONFIGURATION *)0)->Field)))

//
// definition for create specific callback question ID doesn't have relative for SYSTEM_CONFIGURATION
//
#define SETUP_MISC_VALUE_BASE                0x3200
#define SETUP_MISC_QUESTION_ID(a)            (a + SETUP_MISC_VALUE_BASE)

//
// definition for dynamic create form which need question ID
//
#define SETUP_DYNAMIC_CREATE_KEY_VALUE_BASE  0x5200
#define SETUP_DYNAMIC_QUESTION_ID(Field)     (SETUP_VAR_OFFSET(Field)+SETUP_DYNAMIC_CREATE_KEY_VALUE_BASE)
#endif

#define KEY_ABOUT_THIS_SOFTWARE                  0x1059
//
// Boot Menu relative key
//
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#define KEY_BOOT_TYPE_ORDER_BASE            SETUP_MISC_QUESTION_ID(0x50)
#define KEY_BOOT_MENU_TYPE                  0x105A
#define KEY_LEGACY_NORMAL_BOOT_MENU         0x105B
#define KEY_NORMAL_BOOT_PRIORITY            0x105C
#define KEY_NEW_POSITION_POLICY             0x105D
#define KEY_BOOT_MODE_TYPE                  0x105E
#define KEY_BOOT_DEVICE_TYPE_BASE           SETUP_MISC_QUESTION_ID(0x60)
#else
#define KEY_BOOT_TYPE_ORDER_BASE            0x50
#define KEY_BOOT_MENU_TYPE                  0x5A
#define KEY_LEGACY_NORMAL_BOOT_MENU         0x5B
#define KEY_NORMAL_BOOT_PRIORITY            0x5C
#define KEY_NEW_POSITION_POLICY             0x5D
#define KEY_BOOT_MODE_TYPE                  0x5E
#define KEY_BOOT_DEVICE_TYPE_BASE           0x60
#endif

#define BOOT_DEVICE_TYPE_BASE_NUMBER(a)     (KEY_BOOT_DEVICE_TYPE_BASE + ((a) * (MAX_BOOT_TYPE_DEVICES)))
#define KEY_FDD_BOOT_DEVICE_BASE            BOOT_DEVICE_TYPE_BASE_NUMBER(0)
#define KEY_HDD_BOOT_DEVICE_BASE            BOOT_DEVICE_TYPE_BASE_NUMBER(1)
#define KEY_CD_BOOT_DEVICE_BASE             BOOT_DEVICE_TYPE_BASE_NUMBER(2)
#define KEY_OTHER_BOOT_DEVICE_BASE          BOOT_DEVICE_TYPE_BASE_NUMBER(3)
#define KEY_PCMCIA_BOOT_DEVICE_BASE         BOOT_DEVICE_TYPE_BASE_NUMBER(4)
#define KEY_USB_BOOT_DEVICE_BASE            BOOT_DEVICE_TYPE_BASE_NUMBER(5)
#define KEY_EMBED_NETWORK_BASE              BOOT_DEVICE_TYPE_BASE_NUMBER(6)
#define KEY_BEV_BOOT_DEVICE_BASE            BOOT_DEVICE_TYPE_BASE_NUMBER(7)

#define KEY_BOOT_DEVICE_NAME_BASE           BOOT_DEVICE_TYPE_BASE_NUMBER(MAX_BOOT_ORDER_NUMBER)
#define BOOT_DEVICE_NAME_BASE_NUMBER(a)     (KEY_BOOT_DEVICE_NAME_BASE + ((a) * (MAX_BOOT_DEVICES_NUMBER)))
#define KEY_EFI_BOOT_DEVICE_BASE            BOOT_DEVICE_NAME_BASE_NUMBER(0)
#define KEY_ADV_LEGACY_BOOT_BASE            BOOT_DEVICE_NAME_BASE_NUMBER(1)
#define KEY_BOOT_DEVICE_BASE                BOOT_DEVICE_NAME_BASE_NUMBER(2)


//
// Boot menu form ID
//
#define BOOT_OPTION_FORM_ID                 0x530
#define BOOT_DEVICE_FDD_FORM_ID             0x531
#define BOOT_DEVICE_HDD_FORM_ID             0x532
#define BOOT_DEVICE_CD_FORM_ID              0x533
#define BOOT_DEVICE_PCMCIA_FORM_ID          0x534
#define BOOT_DEVICE_USB_FORM_ID             0x535
#define BOOT_EMBED_NETWORK_FORM_ID          0x536
#define BOOT_DEVICE_BEV_FORM_ID             0x537
#define BOOT_DEVICE_OTHER_FORM_ID           0x538
#define BOOT_DEVICE_ADVANCE_FORM_ID         0x539
#define BOOT_DEVICE_EFI_FORM_ID             0x53A
#define BOOT_DEVICE_LEG_NOR_BOOT_ID         0x53B
#define BOOT_DEVICE_LEG_ADV_BOOT_ID         0x53C

//
// Misc labels
//
#define SETUP_UTILITY_LANG_MENU             0x1001
#define BOOT_DEVICE_LABEL                   0x1002
#define USER_PASSWORD_LABEL                 0x1003
#define CLEAR_USER_PASSWORD_LABEL           0x1004
#define UPDATE_INFO_RAM_SLOT_LABEL          0x1007
#define MAIN_PAGE_PLATFORM_INFO_LABEL       0x1008
#define UPDATE_CPU_TYPE_LABEL               0x1009
#define UPDATE_SYSTEM_BUS_SPEED_LABEL       0x100A
#define UPDATE_CACHE_RAM_LABEL              0x100B
#define UPDATE_EFI_OPTION_LABEL             0x100C
#define OPROM_STORAGE_DEVICE_BOOT_LABEL     0x100D
#define COPYRIGHT_LABEL                     0x100E
#define BIOS_BUILD_TIME_LABEL               0x100F
#define UPDATE_SYSTEM_BUS_SPEED_END_LABEL   0x1801

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#define KEY_TPM_CLEAR                       0x10B5
#define KEY_TPM                             0x10B6
#else
#define KEY_TPM_CLEAR                       0xB5
#define KEY_TPM                             0xB6
#endif
#define KEY_TPM2_ENABLE                     0x11B7
#define KEY_TPM2_CLEAR                      0x11B8
#define KEY_TPM_SELECT                      0x11B9
#define KEY_TPM_HIDE                        0x11BA
#define KEY_TPM2_HIDE                       0x11BB
#define TPM_STATE_LABEL                     0x11BC

//
// harddisk relative label
//
#define HDD_PASSWORD_LABEL                  0x1014
#define HDD_PASSWORD_CH_1_MASTER_LABEL      0x1020
#define HDD_PASSWORD_CH_1_SLAVE_LABEL       0x1021
#define HDD_PASSWORD_CH_2_MASTER_LABEL      0x1022
#define HDD_PASSWORD_CH_2_SLAVE_LABEL       0x1023
#define HDD_PASSWORD_CH_3_MASTER_LABEL      0x1024
#define HDD_PASSWORD_CH_3_SLAVE_LABEL       0x1025
#define HDD_PASSWORD_CH_4_MASTER_LABEL      0x1026
#define HDD_PASSWORD_CH_4_SLAVE_LABEL       0x1027
#define HDD_PASSWORD_CH_5_MASTER_LABEL      0x1028
#define HDD_PASSWORD_CH_5_SLAVE_LABEL       0x1029
#define HDD_PASSWORD_CH_6_MASTER_LABEL      0x102a
#define HDD_PASSWORD_CH_6_SLAVE_LABEL       0x102b
#define HDD_PASSWORD_CH_7_MASTER_LABEL      0x102c
#define HDD_PASSWORD_CH_7_SLAVE_LABEL       0x102d
#define HDD_PASSWORD_CH_8_MASTER_LABEL      0x102e
#define HDD_PASSWORD_CH_8_SLAVE_LABEL       0x102f
#define HDD_PASSWORD_CH_1_MASTER_CBM_LABEL  0x1030
#define HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL   0x1031
#define HDD_PASSWORD_CH_2_MASTER_CBM_LABEL  0x1032
#define HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL   0x1033
#define HDD_PASSWORD_CH_3_MASTER_CBM_LABEL  0x1034
#define HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL   0x1035
#define HDD_PASSWORD_CH_4_MASTER_CBM_LABEL  0x1036
#define HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL   0x1037
#define HDD_PASSWORD_CH_5_MASTER_CBM_LABEL  0x1038
#define HDD_PASSWORD_CH_5_SLAVE_CBM_LABEL   0x1039
#define HDD_PASSWORD_CH_6_MASTER_CBM_LABEL  0x103a
#define HDD_PASSWORD_CH_6_SLAVE_CBM_LABEL   0x103b
#define HDD_PASSWORD_CH_7_MASTER_CBM_LABEL  0x103c
#define HDD_PASSWORD_CH_7_SLAVE_CBM_LABEL   0x103d
#define HDD_PASSWORD_CH_8_MASTER_CBM_LABEL  0x103e
#define HDD_PASSWORD_CH_8_SLAVE_CBM_LABEL   0x103f

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
//
// Misc Keys
//
#define KEY_SAVE_EXIT                       0x1031
#define KEY_EXIT_DISCARD                    0x1032
#define KEY_LOAD_OPTIMAL                    0x1033
#define KEY_LOAD_CUSTOM                     0x1034
#define KEY_SAVE_CUSTOM                     0x1035
#define KEY_DISCARD_CHANGE                  0x1036
#define KEY_SUPERVISOR_PASSWORD             0x1037
#define KEY_USER_PASSWORD                   0x1038
#define KEY_CLEAR_USER_PASSWORD             0x1039
#define KEY_UP_SHIFT                        0x103a
#define KEY_DOWN_SHIFT                      0x103b


#define KEY_LOCK_ALL_MASTER_HDD_SECURITY    0x1042
#define KEY_LOCK_ALL_HDD_SECURITY           0x1043
#define KEY_DISABLE_ALL_HDD_SECURITY        0x1044
#define KEY_LANGUAGE_CARRY_OFF              0x1046
#define KEY_SAVE_WITHOUT_EXIT               0x1047
#define KEY_LANGUAGE_UPDATE                 SETUP_DYNAMIC_QUESTION_ID(Language)
//
// Harddisk password relative key
//
#define KEY_SET_HDD_PSW_NUM_0               0x1070
#define KEY_SET_HDD_PSW_NUM_1               0x1071
#define KEY_SET_HDD_PSW_NUM_2               0x1072
#define KEY_SET_HDD_PSW_NUM_3               0x1073
#define KEY_SET_HDD_PSW_NUM_4               0x1074
#define KEY_SET_HDD_PSW_NUM_5               0x1075
#define KEY_SET_HDD_PSW_NUM_6               0x1076
#define KEY_SET_HDD_PSW_NUM_7               0x1077
#define KEY_SET_HDD_PSW_NUM_8               0x1078
#define KEY_SET_HDD_PSW_NUM_9               0x1079
#define KEY_SET_HDD_PSW_NUM_A               0x107a
#define KEY_SET_HDD_PSW_NUM_B               0x107b
#define KEY_SET_HDD_PSW_NUM_C               0x107c
#define KEY_SET_HDD_PSW_NUM_D               0x107d
#define KEY_SET_HDD_PSW_NUM_E               0x107e
#define KEY_SET_HDD_PSW_NUM_F               0x107f

#define KEY_CHECK_HDD_PSW_NUM_0             0x10b0
#define KEY_CHECK_HDD_PSW_NUM_1             0x10b1
#define KEY_CHECK_HDD_PSW_NUM_2             0x10b2
#define KEY_CHECK_HDD_PSW_NUM_3             0x10b3
#define KEY_CHECK_HDD_PSW_NUM_4             0x10b4
#define KEY_CHECK_HDD_PSW_NUM_5             0x10b5
#define KEY_CHECK_HDD_PSW_NUM_6             0x10b6
#define KEY_CHECK_HDD_PSW_NUM_7             0x10b7
#define KEY_CHECK_HDD_PSW_NUM_8             0x10b8
#define KEY_CHECK_HDD_PSW_NUM_9             0x10b9
#define KEY_CHECK_HDD_PSW_NUM_A             0x10ba
#define KEY_CHECK_HDD_PSW_NUM_B             0x10bb
#define KEY_CHECK_HDD_PSW_NUM_C             0x10bc
#define KEY_CHECK_HDD_PSW_NUM_D             0x10bd
#define KEY_CHECK_HDD_PSW_NUM_E             0x10be
#define KEY_CHECK_HDD_PSW_NUM_F             0x10bf

#define KEY_SET_HDD_PSW_CBM_NUM_0           0x1080
#define KEY_SET_HDD_PSW_CBM_NUM_1           0x1081
#define KEY_SET_HDD_PSW_CBM_NUM_2           0x1082
#define KEY_SET_HDD_PSW_CBM_NUM_3           0x1083
#define KEY_SET_HDD_PSW_CBM_NUM_4           0x1084
#define KEY_SET_HDD_PSW_CBM_NUM_5           0x1085
#define KEY_SET_HDD_PSW_CBM_NUM_6           0x1086
#define KEY_SET_HDD_PSW_CBM_NUM_7           0x1087
#define KEY_SET_HDD_PSW_CBM_NUM_8           0x1088
#define KEY_SET_HDD_PSW_CBM_NUM_9           0x1089
#define KEY_SET_HDD_PSW_CBM_NUM_A           0x108a
#define KEY_SET_HDD_PSW_CBM_NUM_B           0x108b
#define KEY_SET_HDD_PSW_CBM_NUM_C           0x108c
#define KEY_SET_HDD_PSW_CBM_NUM_D           0x108d
#define KEY_SET_HDD_PSW_CBM_NUM_E           0x108e
#define KEY_SET_HDD_PSW_CBM_NUM_F           0x108f

#define KEY_CHECK_HDD_PSW_CBM_NUM_0         0x10c0
#define KEY_CHECK_HDD_PSW_CBM_NUM_1         0x10c1
#define KEY_CHECK_HDD_PSW_CBM_NUM_2         0x10c2
#define KEY_CHECK_HDD_PSW_CBM_NUM_3         0x10c3
#define KEY_CHECK_HDD_PSW_CBM_NUM_4         0x10c4
#define KEY_CHECK_HDD_PSW_CBM_NUM_5         0x10c5
#define KEY_CHECK_HDD_PSW_CBM_NUM_6         0x10c6
#define KEY_CHECK_HDD_PSW_CBM_NUM_7         0x10c7
#define KEY_CHECK_HDD_PSW_CBM_NUM_8         0x10c8
#define KEY_CHECK_HDD_PSW_CBM_NUM_9         0x10c9
#define KEY_CHECK_HDD_PSW_CBM_NUM_A         0x10ca
#define KEY_CHECK_HDD_PSW_CBM_NUM_B         0x10cb
#define KEY_CHECK_HDD_PSW_CBM_NUM_C         0x10cc
#define KEY_CHECK_HDD_PSW_CBM_NUM_D         0x10cd
#define KEY_CHECK_HDD_PSW_CBM_NUM_E         0x10ce
#define KEY_CHECK_HDD_PSW_CBM_NUM_F         0x10cf

#define KEY_HDD_MASTER_PSW_KEY_NUM_0        0x1700
#define KEY_HDD_MASTER_PSW_KEY_NUM_1        0x1701
#define KEY_HDD_MASTER_PSW_KEY_NUM_2        0x1702
#define KEY_HDD_MASTER_PSW_KEY_NUM_3        0x1703
#define KEY_HDD_MASTER_PSW_KEY_NUM_4        0x1704
#define KEY_HDD_MASTER_PSW_KEY_NUM_5        0x1705
#define KEY_HDD_MASTER_PSW_KEY_NUM_6        0x1706
#define KEY_HDD_MASTER_PSW_KEY_NUM_7        0x1707
#define KEY_HDD_MASTER_PSW_KEY_NUM_8        0x1708
#define KEY_HDD_MASTER_PSW_KEY_NUM_9        0x1709
#define KEY_HDD_MASTER_PSW_KEY_NUM_A        0x170a
#define KEY_HDD_MASTER_PSW_KEY_NUM_B        0x170b
#define KEY_HDD_MASTER_PSW_KEY_NUM_C        0x170c
#define KEY_HDD_MASTER_PSW_KEY_NUM_D        0x170d
#define KEY_HDD_MASTER_PSW_KEY_NUM_E        0x170e
#define KEY_HDD_MASTER_PSW_KEY_NUM_F        0x170f

#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_0    0x1800
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_1    0x1801
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_2    0x1802
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_3    0x1803
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_4    0x1804
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_5    0x1805
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_6    0x1806
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_7    0x1807
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_8    0x1808
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_9    0x1809
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_A    0x180a
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_B    0x180b
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_C    0x180c
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_D    0x180d
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_E    0x180e
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_F    0x180f
#else
//
// Misc Keys
//
#define KEY_SAVE_EXIT                       0x31
#define KEY_EXIT_DISCARD                    0x32
#define KEY_LOAD_OPTIMAL                    0x33
#define KEY_LOAD_CUSTOM                     0x34
#define KEY_SAVE_CUSTOM                     0x35
#define KEY_DISCARD_CHANGE                  0x36
#define KEY_SUPERVISOR_PASSWORD             0x37
#define KEY_USER_PASSWORD                   0x38
#define KEY_CLEAR_USER_PASSWORD             0x39
#define KEY_UP_SHIFT                        0x3a
#define KEY_DOWN_SHIFT                      0x3b
#define KEY_LANGUAGE_UPDATE                 0x3c

#define KEY_LOCK_ALL_MASTER_HDD_SECURITY    0x42
#define KEY_LOCK_ALL_HDD_SECURITY           0x43
#define KEY_DISABLE_ALL_HDD_SECURITY        0x44
#define KEY_LANGUAGE_CARRY_OFF              0x46
#define KEY_SAVE_WITHOUT_EXIT               0x47
//
// Harddisk password relative key
//
#define KEY_HDD_PSW_DEVICE_BASE             0x70
#define KEY_HDD_PSW_KEY_NUM_0               0x70
#define KEY_HDD_PSW_KEY_NUM_1               0x71
#define KEY_HDD_PSW_KEY_NUM_2               0x72
#define KEY_HDD_PSW_KEY_NUM_3               0x73
#define KEY_HDD_PSW_KEY_NUM_4               0x74
#define KEY_HDD_PSW_KEY_NUM_5               0x75
#define KEY_HDD_PSW_KEY_NUM_6               0x76
#define KEY_HDD_PSW_KEY_NUM_7               0x77
#define KEY_HDD_PSW_KEY_NUM_8               0x78
#define KEY_HDD_PSW_KEY_NUM_9               0x79
#define KEY_HDD_PSW_KEY_NUM_A               0x7a
#define KEY_HDD_PSW_KEY_NUM_B               0x7b
#define KEY_HDD_PSW_KEY_NUM_C               0x7c
#define KEY_HDD_PSW_KEY_NUM_D               0x7d
#define KEY_HDD_PSW_KEY_NUM_E               0x7e
#define KEY_HDD_PSW_KEY_NUM_F               0x7f

#define KEY_HDD_PSW_DEVICE_CBM_BASE         0x80
#define KEY_HDD_PSW_KEY_CBM_NUM_0           0x80
#define KEY_HDD_PSW_KEY_CBM_NUM_1           0x81
#define KEY_HDD_PSW_KEY_CBM_NUM_2           0x82
#define KEY_HDD_PSW_KEY_CBM_NUM_3           0x83
#define KEY_HDD_PSW_KEY_CBM_NUM_4           0x84
#define KEY_HDD_PSW_KEY_CBM_NUM_5           0x85
#define KEY_HDD_PSW_KEY_CBM_NUM_6           0x86
#define KEY_HDD_PSW_KEY_CBM_NUM_7           0x87
#define KEY_HDD_PSW_KEY_CBM_NUM_8           0x88
#define KEY_HDD_PSW_KEY_CBM_NUM_9           0x89
#define KEY_HDD_PSW_KEY_CBM_NUM_A           0x8a
#define KEY_HDD_PSW_KEY_CBM_NUM_B           0x8b
#define KEY_HDD_PSW_KEY_CBM_NUM_C           0x8c
#define KEY_HDD_PSW_KEY_CBM_NUM_D           0x8d
#define KEY_HDD_PSW_KEY_CBM_NUM_E           0x8e
#define KEY_HDD_PSW_KEY_CBM_NUM_F           0x8f

#define KEY_HDD_MASTER_PSW_DEVICE_BASE      0x700
#define KEY_HDD_MASTER_PSW_KEY_NUM_0        0x700
#define KEY_HDD_MASTER_PSW_KEY_NUM_1        0x701
#define KEY_HDD_MASTER_PSW_KEY_NUM_2        0x702
#define KEY_HDD_MASTER_PSW_KEY_NUM_3        0x703
#define KEY_HDD_MASTER_PSW_KEY_NUM_4        0x704
#define KEY_HDD_MASTER_PSW_KEY_NUM_5        0x705
#define KEY_HDD_MASTER_PSW_KEY_NUM_6        0x706
#define KEY_HDD_MASTER_PSW_KEY_NUM_7        0x707
#define KEY_HDD_MASTER_PSW_KEY_NUM_8        0x708
#define KEY_HDD_MASTER_PSW_KEY_NUM_9        0x709
#define KEY_HDD_MASTER_PSW_KEY_NUM_A        0x70a
#define KEY_HDD_MASTER_PSW_KEY_NUM_B        0x70b
#define KEY_HDD_MASTER_PSW_KEY_NUM_C        0x70c
#define KEY_HDD_MASTER_PSW_KEY_NUM_D        0x70d
#define KEY_HDD_MASTER_PSW_KEY_NUM_E        0x70e
#define KEY_HDD_MASTER_PSW_KEY_NUM_F        0x70f
#define KEY_HDD_MASTER_PSW_DEVICE_CBM_BASE  0x800
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_0    0x800
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_1    0x801
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_2    0x802
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_3    0x803
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_4    0x804
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_5    0x805
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_6    0x806
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_7    0x807
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_8    0x808
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_9    0x809
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_A    0x80a
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_B    0x80b
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_C    0x80c
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_D    0x80d
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_E    0x80e
#define KEY_HDD_MASTER_PSW_KEY_CBM_NUM_F    0x80f
#endif

#pragma pack(1)
typedef struct {
  UINT8     HddPasswordSupport;

  UINT8     C1MsMasterHddPasswordFlag;
  UINT8     C1SlMasterHddPasswordFlag;
  UINT8     C2MsMasterHddPasswordFlag;
  UINT8     C2SlMasterHddPasswordFlag;
  UINT8     C3MsMasterHddPasswordFlag;
  UINT8     C3SlMasterHddPasswordFlag;
  UINT8     C4MsMasterHddPasswordFlag;
  UINT8     C4SlMasterHddPasswordFlag;

  UINT8     C1MsCBMMasterHddPasswordFlag;
  UINT8     C1SlCBMMasterHddPasswordFlag;
  UINT8     C2MsCBMMasterHddPasswordFlag;
  UINT8     C2SlCBMMasterHddPasswordFlag;
  UINT8     C3MsCBMMasterHddPasswordFlag;
  UINT8     C3SlCBMMasterHddPasswordFlag;
  UINT8     C4MsCBMMasterHddPasswordFlag;
  UINT8     C4SlCBMMasterHddPasswordFlag;
} PASSWORD_CONFIGURATION;
#pragma pack()

#endif
