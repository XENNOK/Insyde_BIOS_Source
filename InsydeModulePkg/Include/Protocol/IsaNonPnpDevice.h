/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ISA_NON_PNP_DEVICE_
#define _ISA_NON_PNP_DEVICE_

typedef UINT16               EFI_STRING_ID;
typedef EFI_STRING_ID        STRING_REF;

typedef struct {
  UINT8  *Buffer;
  UINTN  BufferSize;
  UINTN  Position;
} HII_LIB_OPCODE_BUFFER;

typedef struct {
  UINT8                             MSB;
  UINT8                             LSB;
  UINTN                             Data;
  UINT16                            KeyFlag;
  UINT16                            StringID;
} EFI_HARDWARE_MONITOR_INFO;

typedef struct {
  UINT8                             MSB;
  UINT8                             LSB;
  UINTN                             Data;
  UINT16                            Label;
  STRING_REF                        Promp;
  STRING_REF                        Help;
  UINT16                            Offset;
} EFI_HARDWARE_MONITOR_INFO2;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_HARDWARE_MONITOR_DATA) (
  OUT EFI_HARDWARE_MONITOR_INFO   **SioInfo,
  OUT INTN                        *Counter
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_HARDWARE_MONITOR_DATA2) (
  OUT EFI_HARDWARE_MONITOR_INFO2  **SioInfo,
  OUT INTN                        *Counter
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_HARDWARE_MONITOR_STATUS) (
  IN  UINTN       Register,
  OUT UINTN       *Status
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_HARDWARE_MONITOR_CTRL) (
  IN UINTN        Register,
  IN UINTN        Data
  );

typedef struct _EFI_SIO_HARDWARE_MONITOR_PROTOCOL {
  EFI_GET_HARDWARE_MONITOR_DATA              GetData;
  EFI_GET_HARDWARE_MONITOR_STATUS            GetStatus;
  EFI_SET_HARDWARE_MONITOR_CTRL              SetCtrl;
} EFI_HARDWARE_MONITOR_PROTOCOL;

typedef struct _EFI_SIO_HARDWARE_MONITOR_PROTOCOL2 {
  EFI_GET_HARDWARE_MONITOR_DATA2             GetData;
  EFI_GET_HARDWARE_MONITOR_STATUS            GetStatus;
  EFI_SET_HARDWARE_MONITOR_CTRL              SetCtrl;
} EFI_HARDWARE_MONITOR_PROTOCOL2;

typedef struct {
  BOOLEAN                OnOff;
  UINT8                  Mode;  
  UINT8                  MSB;  
  UINT8                  LSB;
} EFI_WDT_INFO;

typedef
EFI_STATUS
(EFIAPI *EFI_WRITE_ONOFF) (
  BOOLEAN           OnOff
  );

typedef
EFI_STATUS
(EFIAPI *EFI_WRITE_MODE) (
  UINT8             Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_WRITE_COUNTER) (
  UINT16            Counter
  );

typedef
EFI_STATUS
(EFIAPI *EFI_WRITE_REG) (
  UINT8             Offset,
  UINT8             Reg  
  );

typedef
EFI_STATUS
(EFIAPI *EFI_READ_DATA) (
  EFI_WDT_INFO      **Info
  );

typedef struct _EFI_SIO_WDT_PROTOCOL {
  EFI_WRITE_ONOFF        WriteOnOff;
  EFI_WRITE_MODE         WriteMode;  
  EFI_WRITE_COUNTER      WriteCounter;  
  EFI_WRITE_REG          WriteReg;  
  EFI_READ_DATA          ReadData;
} EFI_SIO_WDT_PROTOCOL;

#define EFI_ISA_VOLTAGE_DEVICE_PROTOCOL_GUID \
  { \
    0x17C57DB5, 0x813E, 0x482C, 0xA0, 0xA8, 0x06, 0x4F, 0x8F, 0xBD, 0xE4, 0x15  \
  }
extern EFI_GUID gEfiIsaVoltagleDeviceProtocolGuid;

#define EFI_ISA_FAN_DEVICE_PROTOCOL_GUID \
  { \
    0xCCA40B3C, 0x8020, 0x4EBF, 0x80, 0x1E, 0x93, 0x19, 0x65, 0x0F, 0xAF, 0x86  \
  }
extern EFI_GUID gEfiIsaFanSpeedDeviceProtocolGuid;

#define EFI_ISA_TEMPERATURE_DEVICE_PROTOCOL_GUID \
  { \
    0x1756F9B8, 0x921E, 0x4A85, 0x81, 0xF7, 0x19, 0xFF, 0xC5, 0xAE, 0xAF, 0xE1  \
  }
extern EFI_GUID gEfiIsaTemperatureDeviceProtocolGuid;

#define EFI_SIO_WDT_PROTOCOL_GUID \
  { \
    0x43f87b04, 0xd9c1, 0x4ee8, { 0x96, 0x8f, 0x71, 0x57, 0xbb, 0xb5, 0x23, 0x98  \
  }
extern EFI_GUID gEfiSioWdtProtocolGuid;

#endif
