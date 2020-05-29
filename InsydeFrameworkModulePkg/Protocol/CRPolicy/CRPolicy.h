//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/*++

Module Name:

  CRPolicy.h
    
Abstract:

  Protocol used for Terminal Policy definition.

--*/

#ifndef _CR_POLICY_PROTOCOL_H_
#define _CR_POLICY_PROTOCOL_H_

EFI_FORWARD_DECLARATION (EFI_CR_POLICY_PROTOCOL);

#define BIT_0              (1<<0)
#define BIT_1              (1<<1)
#define BIT_2              (1<<2)
#define BIT_3              (1<<3)
#define BIT_4              (1<<4)
#define BIT_5              (1<<5)
#define BIT_6              (1<<6)
#define BIT_7              (1<<7)
//[-start-130710-IB09740033-add]//
#define BIT_8              (1<<8)
#define BIT_9              (1<<9)
#define BIT_10             (1<<10)
#define BIT_11             (1<<11)
#define BIT_12             (1<<12)
#define BIT_13             (1<<13)
#define BIT_14             (1<<14)
#define BIT_15             (1<<15)
//[-end-130710-IB09740033-add]//
#define CR_DISABLE        0
#define CR_ENABLE         1

#define CR_TERMINAL_VT100   0
#define CR_TERMINAL_VT100P  1
#define CR_TERMINAL_UTF8    2
#define CR_TERMINAL_ANSI    3

#define CR_TERMINAL_VIDEO_FLAG     BIT_0
#define CR_TERMINAL_VIDEO_COLOR    0
#define CR_TERMINAL_VIDEO_MONO     BIT_0

#define CR_TERMINAL_KEY_FLAG       BIT_1
#define CR_TERMINAL_KEY_SENSELESS  0
#define CR_TERMINAL_KEY_SENSITIVE  BIT_1

#define CR_TERMINAL_COMBO_KEY_FLAG      BIT_2
#define CR_TERMINAL_COMBO_KEY_ENABLE    0
#define CR_TERMINAL_COMBO_KEY_DISABLE   BIT_2

#define CR_TERMINAL_ROW_FLAG            BIT_3
#define CR_TERMINAL_ROW_25              0
#define CR_TERMINAL_ROW_24              BIT_3

#define CR_24ROWS_POLICY_FLAG           BIT_4
#define CR_24ROWS_POLICY_DEL_FIRST      0
#define CR_24ROWS_POLICY_DEL_LAST       BIT_4

#define CR_UNKBC_SUPPORT_FLAG           BIT_5
#define CR_UNKBC_SUPPORT_ENABLE         0
#define CR_UNKBC_SUPPORT_DISABLE        BIT_5

#define CR_AUTO_REFRESH_FLAG           BIT_6
#define CR_AUTO_REFRESH_ENABLE         0
#define CR_AUTO_REFRESH_DISABLE        BIT_6
#define CR_MANUAL_REFRESH_FLAG         BIT_7
#define CR_MANUAL_REFRESH_ENABLE       0
#define CR_MANUAL_REFRESH_DISABLE      BIT_7

//[-start-130710-IB09740033-add]//
#define CR_TERMINAL_CHARSET_FLAG       BIT_8
#define CR_TERMINAL_CHARSET_GRAPHIC        0
#define CR_TERMINAL_CHARSET_ASCII      BIT_8

//[-start-130902-IB09740044-remove]//
//#define CR_LAST_CHAR_DELETE_FLAG       BIT_9
//#define CR_LAST_CHAR_DELETE_ENABLE     0
//#define CR_LAST_CHAR_DELETE_DISABLE    BIT_9
//[-end-130902-IB09740044-remove]//
//[-end-130710-IB09740033-add]//

#define CR_ISA_SERIAL_PORTA       0x00
#define CR_ISA_SERIAL_PORTB       0x01
#define CR_ISA_SERIAL_PORTC       0x02
#define CR_ISA_SERIAL_PORTD       0x03
#define CR_PCI_SERIAL_ONLY        0x80
#define CR_ALL_PORTS              0xFF

#define CR_BAUD_RATE_115200 0
#define CR_BAUD_RATE_57600  1
#define CR_BAUD_RATE_38400  2
#define CR_BAUD_RATE_19200  3
#define CR_BAUD_RATE_9600   4
#define CR_BAUD_RATE_4800   5
#define CR_BAUD_RATE_2400   6
#define CR_BAUD_RATE_1200   7
#define CR_BAUD_RATE_300    8

#define CR_DATA_7BIT      7
#define CR_DATA_8BIT      8

#define CR_PARITY_NONE    1
#define CR_PARITY_EVEN    2
#define CR_PARITY_ODD     3

#define CR_STOP_1BIT      1
#define CR_STOP_2BIT      3

#define CR_FLOW_CONTROL_NONE      0
#define CR_FLOW_CONTROL_HARDWARE  1
#define CR_FLOW_CONTROL_SOFTWARE  2

#define CR_HEADLESS_USE_VBUFFER   0
#define CR_HEADLESS_USE_EBDA      1

#define EFI_CR_POLICY_PROTOCOL_GUID \
  { \
    0x1ae6fb95, 0xc2c7, 0x4e8e, 0xae, 0x09, 0xb9, 0xcc, 0x8e, 0xc9, 0x3c, 0xe2 \
  }

//[-start-130708-IB09740033-modify]//
typedef struct _EFI_CR_POLICY_PROTOCOL {
  BOOLEAN                           CREnable;
  UINT8                             CRSerialPort;
  UINT8                             CRBaudRate;
  UINT8                             CRParity;
  UINT8                             CRDataBits;
  UINT8                             CRStopBits;
  UINT8                             CRFlowControl;
  UINT8                             CRTerminalType;
  UINT16                            CRTerminalKey;
  UINT16                            CRComboKey;
  UINT16                            CRTerminalRows;
  UINT16                             CR24RowsPolicy;
  UINT8                             CRInfoWaitTime;
  BOOLEAN                           CRAfterPost;
  UINT16                            CRVideoType;
  BOOLEAN                           CRShowHelp;
  BOOLEAN                           CRHeadlessVBuffer;
  UINT8                             CRFifoLength;
  BOOLEAN                           AcpiSpcr;
  BOOLEAN                           CRAsyncTerm;          // Only for EFI Terminal
  BOOLEAN                           CRForce80x25;         // Force terminal be 80x25 or 80x24 resolution
  BOOLEAN                           CRFailSaveBaudrate;
  UINT16                            CRUnKbcSupport;
  UINT16                            CRAutoRefresh;
  UINT16                            CRManualRefresh;
  UINT16                            CRTerminalCharSet;
//[-start-130902-IB09740044-modify]//
  UINT8                             CRWriteCharInterval;
//  UINT16                            CRLastCharDelete;
//[-end-130902-IB09740044-modify]//
} EFI_CR_POLICY_PROTOCOL;
//[-end-130708-IB09740033-modify]//

extern EFI_GUID gEfiCRPolicyProtocolGuid;

#endif // _CR_POLICY_PROTOCOL_H_

