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


#include <HardwareMonitor.h>

#include <HwMonitorEcDxe.h>

#define MAX_DATA_SIZE  0x100

CHAR16 *VoltageData [MAX_DATA_SIZE]     = {0};
CHAR16 *TemperatureData [MAX_DATA_SIZE] = {0};
CHAR16 *FanSpeedData [MAX_DATA_SIZE]    = {0};

HARDWARE_MONITOR_STRING VoltageStrings[] = 
  {
    {VOLT_1_5V_KEY, STRING_TOKEN (STR_1_05_VIN_STRING), STRING_TOKEN (STR_1_05_VIN_TEXT)},
    {VOLT_3_3V_KEY, STRING_TOKEN (STR_3_3_VIN_STRING),  STRING_TOKEN (STR_3_3_VIN_TEXT)},
    {VOLT_5V_KEY,   STRING_TOKEN (STR_5_VIN_STRING),    STRING_TOKEN (STR_5_VIN_TEXT)},
    {VOLT_12V_KEY,  STRING_TOKEN (STR_12_VIN_STRING),   STRING_TOKEN (STR_12_VIN_TEXT)}
  };

HARDWARE_MONITOR_STRING   FanSpeedStrings[] = 
  {
    {CPU_FAN_KEY, STRING_TOKEN (STR_CPU_FAN_SPEED_STRING), STRING_TOKEN (STR_CPU_FAN_SPEED_TEXT)}
  };

HARDWARE_MONITOR_STRING   TemperatureStrings[] = 
  {
    {CPU_VR_TEMP_KEY,             STRING_TOKEN (STR_CPU_CORE_VR_TEMP_STRING),        STRING_TOKEN (STR_CPU_CORE_VR_TEMP_TEXT)},
    {HEAT_EXCHANGER_FAN_TEMP_KEY, STRING_TOKEN (STR_HEAT_EXCHANGER_FAN_TEMP_STRING), STRING_TOKEN (STR_HEAT_EXCHANGER_FAN_TEMP_TEXT)},
    {SKIN_TEMP_KEY,               STRING_TOKEN (STR_SKIN_TEMP_STRING),               STRING_TOKEN (STR_SKIN_TEMP_TEXT)},
    {AMBIENT_TEMP_KEY,            STRING_TOKEN (STR_AMBIENT_TEMP_STRING),            STRING_TOKEN (STR_AMBIENT_TEMP_TEXT)},
    {DIMM0_TEMP_KEY,              STRING_TOKEN (STR_DIMM0_TEMP_STRING),              STRING_TOKEN (STR_DIMM0_TEMP_TEXT)},
    {DIMM1_TEMP_KEY,              STRING_TOKEN (STR_DIMM1_TEMP_STRING),              STRING_TOKEN (STR_DIMM1_TEMP_TEXT)},
    {PCH_DTS_TEMP_KEY,            STRING_TOKEN (STR_PCH_DTS_TEMPFROM_PCH_STRING),    STRING_TOKEN (STR_PCH_DTS_TEMPFROM_PCH_TEXT)},
    {TS_ON_DIMM0_TEMP_KEY,        STRING_TOKEN (STR_TS_ON_DIMM0_TEMP_STRING),        STRING_TOKEN (STR_TS_ON_DIMM0_TEMP_TEXT)},
    {TS_ON_DIMM1_TEMP_KEY,        STRING_TOKEN (STR_TS_ON_DIMM1_TEMP_STRING),        STRING_TOKEN (STR_TS_ON_DIMM1_TEMP_TEXT)},
    {CPU_TEMP_BY_PECI_KEY,        STRING_TOKEN (STR_CPU_TMEP_BY_PECI_STRING),        STRING_TOKEN (STR_CPU_TEMP_BY_PECI_TEXT)}
  };

/**
  Create EFI_IFR_REFRESH_OP opcode.

  @param[in]  StartOpCodeHandle  Handle to the buffer of opcodes.

  @retval NULL   There is not enough space left in Buffer to add the opcode.
  @retval Other  A pointer to the created opcode.

**/
EFI_STATUS
CreateRefreshOpCode (
  IN UINT8             RefreshInterval, 
  IN VOID             *StartOpCodeHandle
  )
{
  EFI_IFR_REFRESH            Refresh;
  HII_LIB_OPCODE_BUFFER      *OpCodeHandleBuffer;
  UINT8                      *LocalBuffer;

  if (StartOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Refresh.Header.Length   = sizeof (EFI_IFR_REFRESH);
  Refresh.Header.OpCode   = EFI_IFR_REFRESH_OP;
  Refresh.RefreshInterval = RefreshInterval;

  OpCodeHandleBuffer = (HII_LIB_OPCODE_BUFFER *)StartOpCodeHandle;
  LocalBuffer = (UINT8 *) OpCodeHandleBuffer->Buffer + OpCodeHandleBuffer->Position;
  CopyMem (LocalBuffer, &Refresh, sizeof (EFI_IFR_REFRESH));
  OpCodeHandleBuffer->Position+= sizeof (EFI_IFR_REFRESH);
  
  return EFI_SUCCESS;
}

STRING_REF
TokenStringTransfer (
  IN UINTN                          Table,
  IN UINTN                          StringID,
  IN UINTN                          Field
)
{
  UINT8   Index1;

  switch (Table) {
    case VOLTAGE_LABEL:
      for (Index1 = 0; Index1 < sizeof (VoltageStrings)/sizeof(HARDWARE_MONITOR_STRING); Index1++ ) {
        if (VoltageStrings[Index1].StringID == StringID) {
          return Field==1 ? VoltageStrings[Index1].TokenString1:VoltageStrings[Index1].TokenString2;
        }
      }
    break;

    case FAN_SPEED_LABEL:
      for (Index1 = 0; Index1 < sizeof (FanSpeedStrings)/sizeof(HARDWARE_MONITOR_STRING); Index1++ ) {
        if (FanSpeedStrings[Index1].StringID == StringID) {
          return Field==1 ? FanSpeedStrings[Index1].TokenString1:FanSpeedStrings[Index1].TokenString2;
        }
      }
    break;

    case TEMPERATURE_LABEL:
      for (Index1 = 0; Index1 < sizeof (TemperatureStrings)/sizeof(HARDWARE_MONITOR_STRING); Index1++ ) {
        if (TemperatureStrings[Index1].StringID == StringID) {
          return Field==1 ? TemperatureStrings[Index1].TokenString1:TemperatureStrings[Index1].TokenString2;
        }
      }
    break;
  }

  return EFI_SUCCESS;
}


VOID
UpdateToken (
  IN  EFI_HII_HANDLE       HiiHandle,
  IN  VOID                 *Data[],
  IN  UINTN                Index,
  IN  UINTN                ValueData,
  IN  EFI_STRING_ID        StringId
  )
{

  CHAR16                          *StringBuffer;
  UINTN                           StringLength;
  //
  //  Data is NULL and StringId is zero that indicate it doesn't update token
  //
  if ((Data == NULL) || (StringId == 0)) return;

  if (Data[Index]) {
    gBS->FreePool (Data[Index]);
  }

  StringBuffer  = NULL;
  StringLength  = 0;
  Data[Index]   = AllocateZeroPool (sizeof (CHAR16) * 0x100);

  if (Data == VoltageData) {
    UnicodeSPrint ((CHAR16 *)Data[Index], sizeof (CHAR16) * 0x100, L"<%d.%03d V>", ValueData / 1000, ValueData % 1000);
  } else if (Data == TemperatureData) {
    UnicodeSPrint ((CHAR16 *)Data[Index], sizeof (CHAR16) * 0x100, L"<%d ¢XC>", ValueData, ((ValueData * 9) / 5) + 32);
  } else if (Data == FanSpeedData) {
    UnicodeSPrint ((CHAR16 *)Data[Index], sizeof (CHAR16) * 0x100, L"<%d RPM>",ValueData);
  }

  HiiSetString (HiiHandle, StringId, Data[Index], NULL);  
}

EFI_STATUS
UpdateHardwareMonitorTemperature (
  IN EFI_HII_HANDLE   HiiHandle,
  IN UINTN            KeyValue
  )
{
  EFI_HARDWARE_MONITOR_PROTOCOL *Protocol;
  EFI_HARDWARE_MONITOR_INFO     *SysTempInfo;  
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         Counter;  
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;

  Counter = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIsaTemperatureDeviceProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaTemperatureDeviceProtocolGuid,
                    (VOID *)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->GetData (&SysTempInfo, &Counter);
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < Counter; Index++) {
        if (SysTempInfo[Index].KeyFlag == KeyValue) {
          UpdateToken (
            HiiHandle,
            TemperatureData,
            Index,
            SysTempInfo[Index].Data,
            TokenStringTransfer (TEMPERATURE_LABEL, SysTempInfo[Index].StringID, 2)
            );
          return EFI_SUCCESS;          
        }
      }
    }
  }
  return EFI_UNSUPPORTED;
}

EFI_STATUS
UpdateHardwareMonitorVoltage (
  IN EFI_HII_HANDLE   HiiHandle,
  IN UINTN            KeyValue
  )
{
  EFI_HARDWARE_MONITOR_PROTOCOL *Protocol;
  EFI_HARDWARE_MONITOR_INFO     *SysTempInfo;  
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         Counter;  
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;

  Counter = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIsaVoltagleDeviceProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaVoltagleDeviceProtocolGuid,
                    (VOID *)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->GetData (&SysTempInfo, &Counter);
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < Counter; Index++) {
        if (SysTempInfo[Index].KeyFlag == KeyValue) {
          UpdateToken (
            HiiHandle,
            VoltageData,
            Index,
            SysTempInfo[Index].Data,
            TokenStringTransfer (VOLTAGE_LABEL, SysTempInfo[Index].StringID, 2)
            );
          return EFI_SUCCESS;          
        }
      }
    }
  }
  return EFI_UNSUPPORTED;
}


EFI_STATUS
UpdateHardwareMonitorFanSpeed (
  IN  EFI_HII_HANDLE   HiiHandle,
  IN  UINTN            KeyValue
  )
{
  EFI_HARDWARE_MONITOR_PROTOCOL *Protocol;
  EFI_HARDWARE_MONITOR_INFO     *SysTempInfo;  
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         Counter;  
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;

  Counter = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIsaFanSpeedDeviceProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaFanSpeedDeviceProtocolGuid,
                    (VOID *)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->GetData (&SysTempInfo, &Counter);
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < Counter; Index++) {
        if (SysTempInfo[Index].KeyFlag == KeyValue) {
          UpdateToken (
            HiiHandle,
            FanSpeedData,
            Index,
            SysTempInfo[Index].Data,
            TokenStringTransfer (FAN_SPEED_LABEL, SysTempInfo[Index].StringID, 2)
            );
          return EFI_SUCCESS;          
        }
      }
    }
  }
  return EFI_UNSUPPORTED;
}

EFI_STATUS
InitHardwareMonitorMenu (
  IN EFI_HII_HANDLE   HiiHandle,
  IN EFI_GUID         *Protocol,     
  IN UINTN            FormLabel
  )
{
  UINTN                      Index;
  EFI_STATUS                 Status;
  VOID                       *StartOpCodeHandle;
  UINTN                      Counter;
  EFI_IFR_GUID_LABEL         *StartLabel;
  
  UINTN                          NumberOfHandles;  
  EFI_HANDLE                     *HandleBuffer;
  EFI_HARDWARE_MONITOR_PROTOCOL  *EcProtocol;
  EFI_HARDWARE_MONITOR_INFO      *HwInfo;

  Status = EFI_UNSUPPORTED;
  HwInfo = NULL;
  Counter = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    Protocol,
                    (VOID *) &EcProtocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = EcProtocol->GetData (&HwInfo, &Counter);
    if (!EFI_ERROR(Status)) {
      
      StartOpCodeHandle = HiiAllocateOpCodeHandle ();
      
      StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
      StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
      StartLabel->Number       = (UINT16)FormLabel;

      for (Index = 0; Index < Counter; Index++) {
        HiiCreateActionOpCodeEx (
          StartOpCodeHandle,
          HwInfo[Index].KeyFlag,
          TokenStringTransfer (FormLabel, HwInfo[Index].StringID, 1),
          STRING_TOKEN (STR_BLANK_STRING),
          TokenStringTransfer (FormLabel, HwInfo[Index].StringID, 2),
          EFI_IFR_FLAG_CALLBACK,
          0
          );

        CreateRefreshOpCode (1, StartOpCodeHandle);
      }
      HiiUpdateForm (HiiHandle, NULL, HARDWARE_MONITOR_FORM_ID, StartOpCodeHandle, NULL);
      HiiFreeOpCodeHandle (StartOpCodeHandle);
    } 
  } 
  return Status;
}
