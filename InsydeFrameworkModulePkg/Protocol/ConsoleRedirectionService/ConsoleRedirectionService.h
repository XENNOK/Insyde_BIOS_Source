//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _CONSOLE_REDIRECTION_SERVICE_H_
#define _CONSOLE_REDIRECTION_SERVICE_H_


#define CONSOLE_REDIRECTION_SERVICE_PROTOCOL_GUID \
  { \
    0x60f904c4, 0x6ff, 0x4004, 0x8e, 0x2b, 0x95, 0x86, 0x90, 0xe3, 0xcc, 0x97 \
  }

typedef enum {
  UNKNOW_SERIAL_DEVICE = 0,
  ISA_SERIAL_DEVICE,
  PCI_SERIAL_DEVICE,
  MAX_SERIAL_DEVICE,TYPE
} CR_SERIAL_DEVICE_TYPE;

#pragma pack(1)

typedef struct {
  UINT16                    ComPortAddress;
  UINT8                     ComPortIrq;
} CR_ISA_SERIAL_DEVICE;

typedef struct {
  UINT8                   Bus;
  UINT8                   Device;
  UINT8                   Function;
} CR_PCI_SERIAL_DEVICE;

typedef union {
  CR_ISA_SERIAL_DEVICE   IsaSerial;
  CR_PCI_SERIAL_DEVICE   PciSerial;
} CONSOLE_REDIRECTION_DEVICE;

typedef struct {
  CR_SERIAL_DEVICE_TYPE       Type;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  CONSOLE_REDIRECTION_DEVICE  Device;
  UINT8                       BaudRateDivisor;
} EFI_CONSOLE_REDIRECTION_DEVICE;

typedef struct _EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL;

typedef struct _EFI_CONSOLE_REDIRECTION_INFO {
    //UINT8                             BaudRate;
    //UINT8                             Parity;
    //UINT8                             DataBits;
    //UINT8                             StopBits;
    //UINT8                             FlowControl;
    //UINT8                             TerminalType;
    //UINT8                             InfoWaitTime;
    //BOOLEAN                           CRAfterPost;
    //BOOLEAN                           CREnable;
    BOOLEAN                           Headless;
    //BOOLEAN                           Spcr;
    UINT8                             DeviceCount;
    EFI_CONSOLE_REDIRECTION_DEVICE    CRDevice[1];
} EFI_CONSOLE_REDIRECTION_INFO;

typedef 
EFI_STATUS
(EFIAPI *DISCONNECT_CRS_TERMINAL) (
  IN EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *This,
  IN UINTN                                     TerminalNumber
)
  /*++
  
  Routine Description:
  
    Disconnect console redirection terminal controller.
  
  Arguments:
  
    This             - A pointer to the EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL instance.
    TerminalNumber   - Specify the terminal index that will be disconnect. The first index number is 0.
    
  Returns:
  
    EFI_STATUS
  
  --*/
;

typedef struct _EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL{
  EFI_CONSOLE_REDIRECTION_INFO  *CRInfo;
  DISCONNECT_CRS_TERMINAL       DisconnectCrsTerminal;
} EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL;

#pragma pack()

extern EFI_GUID gConsoleRedirectionServiceProtocolGuid;

#endif
