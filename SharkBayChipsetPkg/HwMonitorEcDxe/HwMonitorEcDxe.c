/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <HwMonitorEcDxe.h>
#include <Library/UefiBootServicesTableLib.h>
//[-start-130813-IB06720232-add]//
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130813-IB06720232-add]//

STATIC EFI_HARDWARE_MONITOR_INFO  mVoltageInfo[] = 
  {
    {EC_REG_VOLTAGE_1_05_VH, EC_REG_VOLTAGE_1_05_VL, 0, 0, VOLT_1_5V_KEY },  
    {EC_REG_VOLTAGE_3_3_VH,  EC_REG_VOLTAGE_3_3_VL,  0, 0, VOLT_3_3V_KEY },  
    {EC_REG_VOLTAGE_5_VH,    EC_REG_VOLTAGE_5_VL,    0, 0, VOLT_5V_KEY   },  
    {EC_REG_VOLTAGE_12_VH,   EC_REG_VOLTAGE_12_VL,   0, 0, VOLT_12V_KEY  }
  };

STATIC EFI_HARDWARE_MONITOR_INFO  mFanSpeedInfo[] = 
  {
    {0,EC_REG_CPU_FAN_SPEED, 0, 0, CPU_FAN_KEY}
  };
          
STATIC EFI_HARDWARE_MONITOR_INFO  mTemperatureInfo[] = 
  {
   { 0, EC_REG_CPU_VR_TEMP,       0, 0, CPU_VR_TEMP_KEY},
   { 0, EC_REG_HEAT_EXG_FAN_TEMP, 0, 0, HEAT_EXCHANGER_FAN_TEMP_KEY},
   { 0, EC_REG_SKIN_TEMP,         0, 0, SKIN_TEMP_KEY},
   { 0, EC_REG_AMBIENT_TEMP,      0, 0, AMBIENT_TEMP_KEY},
   { 0, EC_REG_DIMM0_TEMP,        0, 0, DIMM0_TEMP_KEY},
   { 0, EC_REG_DIMM1_TEMP,        0, 0, DIMM1_TEMP_KEY},
   { 0, EC_REG_PCH_DTS_TEMP,      0, 0, PCH_DTS_TEMP_KEY},
   { 0, EC_REG_TS_ON_DIMM0_TEMP,  0, 0, TS_ON_DIMM0_TEMP_KEY},
   { 0, EC_REG_TS_ON_DIMM1_TEMP,  0, 0, TS_ON_DIMM1_TEMP_KEY},
   { EC_REG_CPU_FRACTIONAL_TEMP_PECI, EC_REG_CPU_INTRGER_TEMP_PECI, 0, 0, CPU_TEMP_BY_PECI_KEY}
 };

EFI_STATUS
ReadKSCRegister (
  IN  UINT8 Reg,
  OUT UINT8 *Value
  );

EFI_STATUS
GetHardwareMonitorInfo (
  IN  UINT16  KeyValue,
  OUT INTN    *Value          
  );

EFI_STATUS
GetVoltageData (
  OUT EFI_HARDWARE_MONITOR_INFO  **VoltageInfo,
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
GetFanSpeedData (
  OUT EFI_HARDWARE_MONITOR_INFO  **VoltageInfo,
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
GetTemperatureData (
  OUT EFI_HARDWARE_MONITOR_INFO  **VoltageInfo,
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

STATIC EFI_HARDWARE_MONITOR_PROTOCOL mEcVoltageProtocol = {
  GetVoltageData,
  GetVoltageStatus,
  SetVoltageCtrl
};

STATIC EFI_HARDWARE_MONITOR_PROTOCOL mEcFanSpeedProtocol = {
  GetFanSpeedData,
  GetFanSpeedStatus,
  SetFanSpeedCtrl
};

STATIC EFI_HARDWARE_MONITOR_PROTOCOL mEcTemperatureProtocol = {
  GetTemperatureData,
  GetTemperatureStatus,
  SetTemperatureCtrl
};

EFI_STATUS
EFIAPI
InstallHMProtocol (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  DeviceHandle;
  UINT8       MajorNum;
  UINT8       MinorNum;
//[-start-130809-IB06720232-add]//
  EFI_STATUS          ReadEcVersionStatus;
//[-end-130809-IB06720232-add]//

  DeviceHandle = NULL;
  MajorNum     = 0;
  MinorNum     = 0;
//[-start-130809-IB06720232-add]//
  ReadEcVersionStatus = EFI_UNSUPPORTED;
//[-end-130809-IB06720232-add]//

//[-start-130809-IB06720232-modify]//
  OemSvcEcVersion (&ReadEcVersionStatus, &MajorNum, &MinorNum);
  if (EFI_ERROR(ReadEcVersionStatus)) {
    return ReadEcVersionStatus;
  }
//[-start-121002-IB06460449-modify]//

  if (MajorNum == 0 && MinorNum == 0) {
    return EFI_UNSUPPORTED;
  }
  
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaVoltagleDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mEcVoltageProtocol
                );

  DeviceHandle=NULL;
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaFanSpeedDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mEcFanSpeedProtocol
                );

  DeviceHandle=NULL;
  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaTemperatureDeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mEcTemperatureProtocol
                );

  return Status;
}

/**
  Get Hardware Info from EC.

  @param[in]   KeyValue           - Hardware Monitor Key Index (String ID)
  @param[out] MonitorValues    - Hardware Monitor Table
  @param[out] Value                - Hardware Monitor data

  @retval EFI_SUCCESS      - The function completed successfully.
**/
EFI_STATUS
ReadKSCRegister (
  IN  UINT8 Reg,
  OUT UINT8 *Value
  )
{
  EFI_STATUS Status;

  Status = SendKscCommand (KSC_C_READ_MEM);
  if (Status == EFI_SUCCESS) {
    Status = SendKscData (Reg);
    if (Status == EFI_SUCCESS) {
      Status = ReceiveKscData (Value);
    }
  }
  return Status;
}

/**
  Get Hardware Info from EC.

  @param[in]   KeyValue           - Hardware Monitor Key Index (String ID)
  @param[out] MonitorValues    - Hardware Monitor Table
  @param[out] Value                - Hardware Monitor data

  @retval EFI_SUCCESS      - The function completed successfully.
**/
EFI_STATUS
GetHardwareMonitorInfo (
  IN  UINT16                    KeyValue,
  OUT INTN                      *Value          
  )
{
  //
  // Local Variables
  //
  UINT8                     Temp;
  UINT8                     Register;
  INTN                      Buffer;
  EFI_STATUS                Status;

  Status = InitializeKscLib();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (KeyValue) {

  case CPU_VR_TEMP_KEY : 
    //
    // Read CPU VR Temp from EC
    //
    Register = EC_REG_CPU_VR_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case CPU_FAN_KEY :
    //
    // Read CPU Fan Speed from EC
    //
    Register = EC_REG_CPU_FAN_SPEED + 1;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;

    Register = EC_REG_CPU_FAN_SPEED;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (*Value << 8) + (INTN)Temp;
  break;

  case HEAT_EXCHANGER_FAN_TEMP_KEY :
    //
    // Read Heat Exchanger Fan Temp from EC
    //
    Register = EC_REG_HEAT_EXG_FAN_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case SKIN_TEMP_KEY :
    //
    // Read Skin Temp from EC
    //
    Register = EC_REG_SKIN_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case AMBIENT_TEMP_KEY :
    //
    // Read Ambient Temp from EC
    //
    Register = EC_REG_AMBIENT_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case DIMM0_TEMP_KEY :
    //
    // Read DIMM 0 Temp from EC
    //
    Register = EC_REG_DIMM0_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case DIMM1_TEMP_KEY :
    //
    // Read DIMM 1 Temp from EC
    //
    Register = EC_REG_DIMM1_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case PCH_DTS_TEMP_KEY :
    //
    // Read PCH DTS Temp from EC
    //
    Register = EC_REG_PCH_DTS_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case TS_ON_DIMM0_TEMP_KEY :
    //
    // Read TS-on-DIMM 0 Temp from EC
    //
    Register = EC_REG_TS_ON_DIMM0_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case TS_ON_DIMM1_TEMP_KEY :
    //
    // Read TS-on-DIMM 1 Temp from EC
    //
    Register = EC_REG_TS_ON_DIMM1_TEMP;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    *Value = (INTN)Temp;
  break;

  case CPU_TEMP_BY_PECI_KEY :
    // 
    // CPU temprature reported by EC via PECI
    // Store current Integer part of reading in temporary variable	
    //
    Register = EC_REG_CPU_INTRGER_TEMP_PECI;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    Buffer = Temp; 
    
    *Value = Buffer;
  break;

  case VOLT_1_5V_KEY :
    //
    // get 1.5v High Byte 
    //
    Register = EC_REG_VOLTAGE_1_05_VH;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    Buffer = Temp * 0x100;
    //
    // get 1.5v Low Byte
    //
    Register = EC_REG_VOLTAGE_1_05_VL;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    //
    // Raw value = High Byte + Low Byte
    //
    Buffer = Buffer + Temp;
    //
    // 1.5V : 3.3/1024(Vref/1024) * 2(HW scaling factor)* Raw value
    //
    *Value = ((Vef * Scaling_Factor_1_5V * Buffer) / 100000);
  break;
  
  case VOLT_3_3V_KEY :
    //
    // get 3.3v High Byte
    //
    Register = EC_REG_VOLTAGE_3_3_VH;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    Buffer = Temp * 0x100;
    //
    // get 3.3v low Byte
    //
    Register = EC_REG_VOLTAGE_3_3_VL;
    Temp = 0;
    Status = ReadKSCRegister (Register, &Temp);
    //
    // Raw value = High Byte + Low Byte
    //
    Buffer = Buffer + Temp;
    //
    // 3.3V : 3.3/1024(Vref/1024) * 2(HW scaling factor)* Raw value
    //
    *Value = ((Vef * Scaling_Factor_3V * Buffer) / 100000);
  break;
  
  case VOLT_5V_KEY :
    //
    // get 5V High Byte 
    //
    Register = EC_REG_VOLTAGE_5_VH;
    Temp = 0;    
    Status = ReadKSCRegister (Register, &Temp);
    Buffer = Temp * 0x100;
    //
    // get 5V low Byte 
    //
    Register = EC_REG_VOLTAGE_5_VL;
    Temp = 0;    
    Status = ReadKSCRegister (Register, &Temp);
    //
    // Raw value = High Byte + Low Byte
    //
    Buffer = Buffer + Temp;
    //
    // 5V : 3.3/1024(Vref/1024) * 2(5V HW Scaling Factor)* Raw value  
    //
    *Value = ((Vef * Scaling_Factor_5V * Buffer) / 100000);    
  break;
  
  case VOLT_12V_KEY :
    //
    // get 12V High Byte
    //
    Register = EC_REG_VOLTAGE_12_VH;
    Temp = 0;          
    Status = ReadKSCRegister (Register, &Temp);
    Buffer = Temp * 0x100;
    //
    // get 12V low Byte
    //
    Register = EC_REG_VOLTAGE_12_VL;
    Temp = 0;          
    Status = ReadKSCRegister (Register, &Temp);
    //
    // Raw value = High Byte + Low Byte
    //
    Buffer = Buffer + Temp;
    //
    // 12V : 3.3/1024(Vref/1024) * 401(12V HW Scaling Factor)* Raw value
    //
    *Value = ((Vef * Scaling_Factor_12V * Buffer) / 100000);
  break;

  default:
    return EFI_INVALID_PARAMETER;
  break;
  }

  return Status;
}

/**
  Get Voltage from EC.

  @param[out] Info           - Hardware Monitor data structure
  @param[out] Counter      - Hardware Monitor data number

  @retval EFI_SUCCESS      - The function completed successfully.
**/
EFI_STATUS
GetVoltageData (
  OUT EFI_HARDWARE_MONITOR_INFO  **Info,
  OUT INTN                       *Counter
  )
{
  UINT8 Index;
  UINT8 Size;
  INTN  Data;

  Size = sizeof (mVoltageInfo) /  sizeof (EFI_HARDWARE_MONITOR_INFO);

  for (Index = 0; Index < Size; Index++) {
    GetHardwareMonitorInfo (mVoltageInfo[Index].StringID, &Data);
    mVoltageInfo[Index].Data    = Data;
    mVoltageInfo[Index].KeyFlag = KEY_HARDWARE_MONITOR_VOLTAGE + Index;
  }
  *Info    = mVoltageInfo;
  *Counter = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
GetVoltageStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_UNSUPPORTED;
}

EFI_STATUS
SetVoltageCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_UNSUPPORTED;
}


/**
  Get Fan Speed from EC.

  @param[out] Info         - Hardware Monitor data structure
  @param[out] Counter      - Hardware Monitor data number

  @retval EFI_SUCCESS      - The function completed successfully.
**/
EFI_STATUS
GetFanSpeedData (
  OUT EFI_HARDWARE_MONITOR_INFO  **Info,
  OUT INTN                       *Counter  
  )
{
  UINT8 Index;
  UINT8 Size;
  INTN  Data;

  Size = sizeof (mFanSpeedInfo) / sizeof (EFI_HARDWARE_MONITOR_INFO);

  for (Index = 0; Index < Size; Index++) {
    GetHardwareMonitorInfo (mFanSpeedInfo[Index].StringID, &Data);
    mFanSpeedInfo[Index].Data    = Data;
    mFanSpeedInfo[Index].KeyFlag = KEY_HARDWARE_MONITOR_FANSPEED + Index;
  }
  
  *Info         = mFanSpeedInfo;
  *Counter      = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
GetFanSpeedStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_UNSUPPORTED;
}

EFI_STATUS
SetFanSpeedCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_UNSUPPORTED;
}

/**
  Get Temperature from EC.

  @param[out] Info           - Hardware Monitor data structure
  @param[out] Counter      - Hardware Monitor data number

  @retval EFI_SUCCESS      - The function completed successfully.
**/
EFI_STATUS
GetTemperatureData (
  OUT EFI_HARDWARE_MONITOR_INFO  **Info,
  OUT INTN                       *Counter
  )
{
  UINT8 Index;
  UINT8 Size;
  INTN  Data;  

  Size = sizeof (mTemperatureInfo) /sizeof (EFI_HARDWARE_MONITOR_INFO);

  for (Index = 0; Index < Size; Index++) {
    GetHardwareMonitorInfo (mTemperatureInfo[Index].StringID, &Data);
    mTemperatureInfo[Index].Data    = Data;
    mTemperatureInfo[Index].KeyFlag = KEY_HARDWARE_MONITOR_TEMPERATURE + Index;
  }
  
  *Info         = mTemperatureInfo;
  *Counter      = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
GetTemperatureStatus (
  IN  UINTN  Register,
  OUT UINTN  *Status
  )
{

  return EFI_UNSUPPORTED;
}

EFI_STATUS
SetTemperatureCtrl (
  IN UINTN  Register,
  IN UINTN  Data
  )
{

  return EFI_UNSUPPORTED;
}

