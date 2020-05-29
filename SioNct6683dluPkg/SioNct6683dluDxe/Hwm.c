/** @file

Device Protocol

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/
#include "SioInitDxe.h"
#include <Protocol/IsaNonPnpDevice.h>

extern VOID
EnterConfigMode (
  VOID
  );

extern VOID
ExitConfigMode (
  VOID
  );

UINT8 Ldn;
UINT16 mHwmPort;
UINT16 mHwmIndexPort;
UINT16 mHwmDataPort;

//  
//EFI_HARDWARE_MONITOR_INFO2[] => Msb, Lsb, Data, Label, Promp, Help, Offset
//
EFI_HARDWARE_MONITOR_INFO2 mVoltageData[] = {
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VCORE_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL1_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VDIMM_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL2_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_12V_STRING),   STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL3_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_5V_STRING),    STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL4_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VLDT_STRING),  STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL5_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VIN5_STRING),  STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL6_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VIN6_STRING),  STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL7_HWM_OFFSET},  
  { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_VBAT_STRING),  STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_VOL8_HWM_OFFSET},   
};
          
EFI_HARDWARE_MONITOR_INFO2 mTemperatureData[] = {
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_SYS_TEMP1_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_TEMP1_HWM_OFFSET},
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_DDR_TEMP_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_TEMP2_HWM_OFFSET},
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_AUX_TEMP_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_TEMP3_HWM_OFFSET},
 };

EFI_HARDWARE_MONITOR_INFO2 mFanSpeedData[] = {
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_SYS_FAN_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_FAN1_HWM_OFFSET},
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_CPU_FAN_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_FAN2_HWM_OFFSET},
   { 0, 0, 0, 0, STRING_TOKEN (STR_HARDWARE_MONITOR_AUX_FAN_STRING), STRING_TOKEN (STR_SIO_EMPTY), SIO_HWM_OFFSET + SIO_FAN3_HWM_OFFSET},
 };

VOID
HwmInitFunction (
  VOID
  );

UINT8
HMIndexReading (
  IN UINT8  Index
  );

VOID
HMIndexWriting (
  IN UINT8  Index,
  IN UINT8  Data8
  );

EFI_STATUS
GetVoltageData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2 **VoltageInfo,
  OUT INTN                       *Counter
  );

EFI_STATUS
GetVoltageStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  );

EFI_STATUS
SetVoltageCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  );

EFI_STATUS
GetFanSpeedData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2 **VoltageInfo,
  OUT INTN                       *Counter
  );

EFI_STATUS
GetFanSpeedStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  );

EFI_STATUS
SetFanSpeedCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  );


EFI_STATUS
GetTemperatureData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2 **VoltageInfo,
  OUT INTN                       *Counter
  );

EFI_STATUS
GetTemperatureStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  );

EFI_STATUS
SetTemperatureCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  );

STATIC EFI_HARDWARE_MONITOR_PROTOCOL2 mSioVoltageProtocol = {
  GetVoltageData2,
  GetVoltageStatus,
  SetVoltageCtrl
};

STATIC EFI_HARDWARE_MONITOR_PROTOCOL2 mSioFanSpeedProtocol = {
  GetFanSpeedData2,
  GetFanSpeedStatus,
  SetFanSpeedCtrl
};

STATIC EFI_HARDWARE_MONITOR_PROTOCOL2 mSioTemperatureProtocol = {
  GetTemperatureData2,
  GetTemperatureStatus,
  SetTemperatureCtrl
};

/**
  brief-description of function. 

  extended description of function.  
  
**/
EFI_STATUS
InstallHwmProtocol (
  IN SIO_DEVICE_LIST_TABLE* DeviceList
  )
{
  EFI_STATUS       Status;
  EFI_HANDLE       DeviceHandle;

  //
  // Get resources from PCD data
  //
  Ldn = DeviceList->DeviceLdn;
  mHwmPort = UpsideDown (DeviceList->DeviceBaseAdr);

  DeviceHandle=NULL;
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaVoltagleDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSioVoltageProtocol
                );

  DeviceHandle=NULL;
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaFanSpeedDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSioFanSpeedProtocol
                );

  DeviceHandle=NULL;
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaTemperatureDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSioTemperatureProtocol
                );

  HwmInitFunction();

  return Status;
}

VOID
HMIndexWriting (
  IN UINT8    Index,
  IN UINT8    Data8
  )
{
  SioWrite8 (mHwmIndexPort, Index);
  SioWrite8 (mHwmDataPort, Data8);
}

UINT8
HMIndexReading (
  IN UINT8 Index
  )
{
  UINT8      Data;

  SioWrite8 (mHwmIndexPort, Index);
  Data = SioRead8 (mHwmDataPort);

  return Data;
}

VOID
HwmInitFunction (
  VOID
  )
{
  IDW8(SIO_LDN, Ldn);

  IDW8(SIO_BASE_IO_ADDR1_MSB, (UINT8) (mHwmPort >> 8));
  IDW8(SIO_BASE_IO_ADDR1_LSB, (UINT8) (mHwmPort & 0xFF));
  IDW8(SIO_DEV_ACTIVE, 1);

  mHwmIndexPort = mHwmPort + 5;
  mHwmDataPort  = mHwmIndexPort + 1;
}

EFI_STATUS
GetVoltageData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2 **Info,
  OUT INTN                       *Counter
  )
{
  UINT8     Index;
  UINT8     Size;

  Size = sizeof (mVoltageData) / sizeof (EFI_HARDWARE_MONITOR_INFO2);

  for ( Index = 0; Index < Size; Index++)
  {
    mVoltageData[Index].Data  = (UINTN) HMIndexReading ( mVoltageData[Index].LSB) * 2 * 12; // The unit of reading is 8mV.
    mVoltageData[Index].Label = SIO_VOL_LABEL + Index;    
  }
  *Info         = mVoltageData;
  *Counter      = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
GetVoltageStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
SetVoltageCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
GetFanSpeedData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2 **Info,
  OUT INTN                       *Counter
  )
{
  UINT8     Index;
  UINT8     Size;
  UINTN     Data;

  Size = sizeof (mFanSpeedData) / sizeof (EFI_HARDWARE_MONITOR_INFO2);

  for ( Index = 0; Index < Size ; Index++)
  {
    Data  = HMIndexReading ( mFanSpeedData[Index].MSB );// MSB
    Data  = Data << 8;
    Data |= HMIndexReading ( mFanSpeedData[Index].LSB); // LSB

    if ((Data == 0) || (Data == 0xFFFF)) {
      mFanSpeedData[Index].Data = 0;
    } else {
      mFanSpeedData[Index].Data  = HARDWARE_MONITOR_FAN_SPEED_COUNT / (Data * 2);
    }
    mFanSpeedData[Index].Label = SIO_FAN_LABEL + Index; 
  }
  *Info         = mFanSpeedData;
  *Counter      = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
GetFanSpeedStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
SetFanSpeedCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
GetTemperatureData2 (
  OUT EFI_HARDWARE_MONITOR_INFO2  **Info,
  OUT INTN                        *Counter
  )
{
  UINT8     Index;
  UINT8     Size;

  Size = sizeof (mTemperatureData) / sizeof (EFI_HARDWARE_MONITOR_INFO2);

  for ( Index = 0; Index < Size; Index++)
  {
    mTemperatureData[Index].Data  = (UINTN) HMIndexReading ( mTemperatureData[Index].LSB);
    mTemperatureData[Index].Label = SIO_TEMP_LABEL + Index; 
  }

  *Info         = mTemperatureData;
  *Counter      = Size;

  return EFI_SUCCESS;
}


EFI_STATUS
GetTemperatureStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
SetTemperatureCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_SUCCESS;
}

