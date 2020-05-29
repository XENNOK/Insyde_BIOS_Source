/** @file
  Console Redirection Setup Configuration Definitions

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
#ifndef _CR_SETUP_CONFIG_H_
#define _CR_SETUP_CONFIG_H_

//#include <Protocol/CRPolicy.h>
#include <KernelSetupConfig.h>


#define CR_CONFIGURATION_GUID   { \
    0x7EC07B9F, 0x66E3, 0x43d4, 0x9B, 0x52, 0x38, 0xFB, 0xB4, 0x63, 0x90, 0xCC \
  }

extern EFI_GUID gCrConfigurationGuid;

#define CR_CONFIGURATION_VARSTORE_ID        0x7777

#define CR_SERIAL_PORT_LABEL                0x1090
#define CR_SERIAL_DEV_FORM_SUBTITTLE_LABEL  0x1091

#define CONSOLE_REDIRECTION_FORM_ID_0       0xC00
#define CONSOLE_REDIRECTION_FORM_ID_1       0xC01
#define CONSOLE_REDIRECTION_FORM_ID_2       0xC02
#define CONSOLE_REDIRECTION_FORM_ID_3       0xC03
#define CONSOLE_REDIRECTION_FORM_ID_4       0xC04
#define CONSOLE_REDIRECTION_FORM_ID_5       0xC05
#define CONSOLE_REDIRECTION_FORM_ID_6       0xC06
#define CONSOLE_REDIRECTION_FORM_ID_7       0xC07
#define CONSOLE_REDIRECTION_FORM_ID         0xC20

#define KEY_CONSOLE_REDIRECTION_1           0xC08
#define KEY_CONSOLE_REDIRECTION_2           0xC09
#define KEY_CONSOLE_REDIRECTION_3           0xC0A
#define KEY_CONSOLE_REDIRECTION_4           0xC0B
#define KEY_CONSOLE_REDIRECTION_5           0xC0C
#define KEY_CONSOLE_REDIRECTION_6           0xC0D
#define KEY_CONSOLE_REDIRECTION_7           0xC0E
#define KEY_CONSOLE_REDIRECTION_8           0xC0F
#define KEY_CONSOLE_REDIRECTION_9           0xC10
#define KEY_CONSOLE_REDIRECTION_10          0xC11
#define KEY_CONSOLE_REDIRECTION_11          0xC12
#define KEY_CONSOLE_REDIRECTION_12          0xC13
#define KEY_CONSOLE_REDIRECTION_13          0xC14
#define KEY_CONSOLE_REDIRECTION_14          0xC15
#define KEY_CONSOLE_REDIRECTION_15          0xC16
#define KEY_CONSOLE_REDIRECTION_16          0xC17


#pragma pack(1)

typedef struct {
  UINT8         PortEnable; 
  UINT8         UseGlobalSetting;
  UINT8         TerminalType;                         
  UINT8         BaudRate;                             
  UINT8         Parity;                               
  UINT8         DataBits;                             
  UINT8         StopBits;                             
  UINT8         FlowControl;                          
} CR_SERIAL_PORT_CONFIG;

typedef struct {
  UINT8                  GlobalTerminalType;
  UINT8                  GlobalBaudRate;
  UINT8                  GlobalParity;
  UINT8                  GlobalDataBits;
  UINT8                  GlobalStopBits;
  UINT8                  GlobalFlowControl;
  UINT8                  AutoRefresh;                             
  UINT8                  FailSaveBaudRate; 
  CR_SERIAL_PORT_CONFIG  CrPortConfig;                         
} CR_CONFIGURATION;

#pragma pack()

#endif
