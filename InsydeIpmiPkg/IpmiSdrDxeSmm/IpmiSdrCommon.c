/** @file
 H2O IPMI SDR module common implement code.

 This c file contains SDR related common functions for DXE/SMM phase.
 
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

#include "IpmiSdrCommon.h"



//
// Module variable
//
H2O_IPMI_SDR_PROTOCOL               mSdrProtocol;
BOOLEAN                             mInitialized = FALSE;
BOOLEAN                             mUseBuiltIn = FALSE;


//
// Full type Sensor Data Record table
//
H2O_IPMI_SDR_FULL_SENSOR_RECORD     *mFullSdrRecordTable = NULL;
BOOLEAN                             mHaveAllFullRecords = FALSE;


//
// Sensor type description table
//
CHAR16 *mSensorTypeDesc[] = {
  L"reserved",
  L"Temperature",
  L"Voltage",
  L"Current",
  L"Fan",
  L"Physical Security",
  L"Platform Security",
  L"Processor",
  L"Power Supply",
  L"Power Unit",
  L"Cooling Device",
  L"Other",
  L"Memory",
  L"Drive Slot / Bay",
  L"POST Memory Resize",
  L"System Firmwares",
  L"Event Logging Disabled",
  L"Watchdog",
  L"System Event",
  L"Critical Interrupt",
  L"Button",
  L"Module / Board",
  L"Microcontroller",
  L"Add-in Card",
  L"Chassis",
  L"Chip Set",
  L"Other FRU",
  L"Cable / Interconnect",
  L"Terminator",
  L"System Boot Initiated",
  L"Boot Error",
  L"OS Boot",
  L"OS Stop / Shutdown",
  L"Slot / Connector",
  L"System ACPI Power State",
  L"Watchdog",
  L"Platform Alert",
  L"Entity Presence",
  L"Monitor ASIC",
  L"LAN",
  L"Management Subsystem Health",
  L"Battery",
  L"Session Audit",
  L"Version Change",
  L"FRU State"
};

//
// Sensor unit description table
//
SDR_UNIT_STRING_STRUCT mSensorUnitDesc[] = {
  {L"unspecified", NULL},
  {L"degrees C", L" C"},
  {L"degrees F", L" F"},
  {L"degrees K", L" K"},
  {L"Volts", L"V"},
  {L"Amps", L"A"},
  {L"Watts", L"W"},
  {L"Joules", NULL},
  {L"Coulombs", NULL},
  {L"VA", NULL},
  {L"Nits", NULL},
  {L"lumen", NULL},
  {L"lux", NULL},
  {L"Candela", NULL},
  {L"kPa", NULL},
  {L"PSI", NULL},
  {L"Newton", NULL},
  {L"CFM", NULL},
  {L"RPM", NULL},
  {L"Hz", NULL},
  {L"microsecond", L"us"},
  {L"millisecond", L"ms"},
  {L"second", L"s"},
  {L"minute", L"m"},
  {L"hour", L"h"},
  {L"day", L"d"},
  {L"week", L"w"},
  {L"mil", NULL},
  {L"inches", NULL},
  {L"feet", NULL},
  {L"cu in", NULL},
  {L"cu feet", NULL},
  {L"mm", NULL},
  {L"cm", NULL},
  {L"m", NULL},
  {L"cu cm", NULL},
  {L"cu m", NULL},
  {L"liters", NULL},
  {L"fluid ounce", NULL},
  {L"radians", NULL},
  {L"steradians", NULL},
  {L"revolutions", NULL},
  {L"cycles", NULL},
  {L"gravities", NULL},
  {L"ounce", NULL},
  {L"pound", NULL},
  {L"ft-lb", NULL},
  {L"oz-in", NULL},
  {L"gauss", NULL},
  {L"gilberts", NULL},
  {L"henry", NULL},
  {L"millihenry", NULL},
  {L"farad", NULL},
  {L"microfarad", NULL},
  {L"ohms", NULL},
  {L"siemens", NULL},
  {L"mole", NULL},
  {L"becquerel", NULL},
  {L"PPM", NULL},
  {L"reserved", NULL},
  {L"Decibels", NULL},
  {L"DbA", NULL},
  {L"DbC", NULL},
  {L"gray", NULL},
  {L"sievert", NULL},
  {L"color temp deg K", NULL},
  {L"bit", NULL},
  {L"kilobit", NULL},
  {L"megabit", NULL},
  {L"gigabit", NULL},
  {L"byte", NULL},
  {L"kilobyte", NULL},
  {L"megabyte", NULL},
  {L"gigabyte", NULL},
  {L"word", NULL},
  {L"dword", NULL},
  {L"qword", NULL},
  {L"line", NULL},
  {L"hit", NULL},
  {L"miss", NULL},
  {L"retry", NULL},
  {L"reset", NULL},
  {L"overflow", NULL},
  {L"underrun", NULL},
  {L"collision", NULL},
  {L"packets", NULL},
  {L"messages", NULL},
  {L"characters", NULL},
  {L"error", NULL},
  {L"correctable error", NULL},
  {L"uncorrectable error", NULL},
  {L"fatal error", NULL},
  {L"grams", NULL}
};

/**
 Convert float to string.

 @param[in]         DoubleValue         Float to convert to string.
 @param[in]         Precision           Decimal number will be shown in string.
 @param[out]        Buffer              String buffer to store string.
*/
VOID
DoubleToStr (
  IN  UINT64  DoubleValue,
  IN  UINT8   Precision,
  OUT CHAR16  *Buffer
  )
{
#ifdef H2OIPMI_X64
  UINT64        Decimal;
  UINT64        Integer;
  DOUBLE        Temporarily;
  DOUBLE        Comparison;
  DOUBLE        Value;
  UINTN         IntegerDigits;
  UINTN         Position;


  if (Precision > 5) {
    Precision = 5;
  }

  Value = * (DOUBLE *) &DoubleValue;
  Temporarily = (Value < 0.0 ? -Value : Value);

  Comparison = MathPow10 (-1.0 * Precision);
  Temporarily += Comparison / 2.0;
  Integer = (UINT64) Temporarily;
  Decimal = (UINT64) ( (Temporarily - Integer) / Comparison);

  Buffer[0] = 0;
  Position = 0;
  if (Value < 0) {
    StrCat (Buffer, L"-");
    Position += 1;
  }

  IntegerDigits = 0;
  if (Integer > 0) {
    IntegerDigits = (UINTN) (MathLog10 (Integer + 0.0) + 1.0);
  }
  
  Position += UnicodeValueToString (&Buffer[Position],0, Integer, IntegerDigits);

  if (Decimal > 0) {
    Buffer[Position] = L'.';
    Position += 1;
    Position += UnicodeValueToString (&Buffer[Position], PREFIX_ZERO, Decimal, Precision);
  }
#endif

#ifdef H2OIPMI_IA32
  StrCpy (Buffer, L"0.0");
#endif  

}

/**
 Check if use built-in SDR from Get Device ID information.
 
 @retval EFI_SUCCESS                    Check success.
 @retval EFI_UNSUPPORTED                Check fail.
*/
EFI_STATUS
CheckUseBuiltIn (
  VOID
  )
{
  EFI_STATUS          Status;
  UINT8               RecvSize;
  H2O_IPMI_BMC_INFO   BmcInfo;



  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_APPLICATION,
             H2O_IPMI_CMD_GET_DEVICE_ID,
             NULL, 
             0, 
             &BmcInfo, 
             &RecvSize
             );
                    
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (BmcInfo.ProvideSDRsDevice) {
    if (!BmcInfo.SDRRepositoryDevice) {
      if (BmcInfo.SensorDevice) {
        mUseBuiltIn = TRUE;
      } else {
        Status = EFI_UNSUPPORTED;
      }
    }
  }

  return Status;
  
}

/**
 Convert specified number of 8 bit ASCII characters to unicode string.

 @param[in]         AsciiString         8 bit ASCII string.
 @param[in]         AsciiStrSize        8 bit ASCII string size.
 @param[out]        UnicodeString       Buffer to store converted unicode string. It is caller's
                                        responsibility to prepare a big enough buffer to store
                                        converted string.
*/
VOID
Ascii8BitToUnicode (
  IN  UINT8   *AsciiString,
  IN  UINT8   AsciiStrSize,
  OUT CHAR16  *UnicodeString
  )
{
  UINT8 Index;

  for (Index = 0; Index < AsciiStrSize; ++Index) {
    UnicodeString[Index] = AsciiString[Index];
  }
  UnicodeString[Index] = 0;
}


/**
 Convert raw sensor reading data to value.

 This is based on the following equation:
 Value = ((M * RawReading) + (b * 10^BExp)) * (10^RExp)

 @param[in]         Record              H2O_IPMI_SDR_FULL_SENSOR_RECORD sturcture pointer.
                                        This contains sensor record data.
 @param[in]         RawReading          Raw sensor data of this sensor.
 
 @return The value converted from raw sensor reading data.
*/
#ifdef H2OIPMI_X64
DOUBLE
ConvertSensorReadingStage1 (
  IN   H2O_IPMI_SDR_FULL_SENSOR_RECORD  *Record,
  IN   UINT8                            RawReading
  )
{
  INT32   M;
  INT32   B;
  INT32   BExp;
  INT32   RExp;
  INT32   TempInt; 
  DOUBLE  Result;

  TempInt = (INT32) (Record->Mls8bits | (Record->Mms2bits << 8));
  M = GetInt32 (TempInt, 10);

  TempInt = (INT32) (Record->Bls8bits | (Record->Bms2bits << 8));
  B = GetInt32 (TempInt, 10);

  TempInt = (INT32) Record->BExp;
  BExp = GetInt32 (TempInt, 4);

  TempInt = (INT32) Record->RExp;
  RExp = GetInt32 (TempInt, 4);

  Result = M * RawReading;
  Result += B * MathPow10 (BExp);
  Result *= MathPow10 (RExp);

  return Result;

#ifdef H2OIPMI_IA32
  return 0;
#endif
}
#endif


/**
 Convert raw sensor reading data to value.

 This is based on the result of ConvertSensorReadingStage1 () and convert it
 based-on LinearizationEnum of this sensor.

 @param[in]         Record              H2O_IPMI_SDR_FULL_SENSOR_RECORD sturcture pointer.
                                        This contains sensor record data.
 @param[in]         RawReading          Raw sensor data of this sensor.
 
 @return The value converted from raw sensor reading data.
*/
UINT64
ConvertSensorReading (
  IN   H2O_IPMI_SDR_FULL_SENSOR_RECORD  *Record,
  IN   UINT8                            RawReading
  )
{
#ifdef H2OIPMI_X64
  DOUBLE  Value1;
  DOUBLE  Value2;
  DOUBLE  Result;

  Value1 = 0;
  switch (Record->AnalogDataFormat) {
  case 1:
    if (RawReading & 0x80) {
      ++RawReading;
    }

  case 0:
  case 2:
    Value1 = ConvertSensorReadingStage1 (Record, RawReading);
    break;
  }
  
  Value2 = 0;
  Result = 0;
  switch (Record->LinearizationEnum) {
  case SDR_SENSOR_L_LN:
    Result = MathLn (Value1);
    break;    

  case SDR_SENSOR_L_LOG10:
    Result = MathLog10 (Value1);
    break;

  case SDR_SENSOR_L_LOG2:
    Result = MathLog2 (Value1);
    break;

  case SDR_SENSOR_L_E:
    Result = MathExp (Value1);
    break;

  case SDR_SENSOR_L_EXP10:
    Result = MathPow10 (Value1);
    break;

  case SDR_SENSOR_L_EXP2:
    Result = MathPow2 (Value1);
    break;      

  case SDR_SENSOR_L_1_X:
    if (Value1 > 0){
      Result = MathInverse (Value1);
    }
    break;

  case SDR_SENSOR_L_SQR:
    Result = MathSquare (Value1);
    break;

  case SDR_SENSOR_L_CUBE:
    Result = MathCube (Value1);
    break;

  case SDR_SENSOR_L_SQRT:
    Result = MathSquareRoot (Value1);
    break;

  case SDR_SENSOR_L_CUBERT:
    Result = MathCubeRoot (Value1);
    break;

  default:
    Result = Value1;
    Value2 = 0;
    break;
  }

  return *(UINT64 *)&Result;
#endif

#ifdef H2OIPMI_IA32
  return 0;
#endif
}


/**
 Get specified sensor reading.

 @param[in]         Sensor              Sensor ID.
 @param[out]        Reading             H2O_IPMI_GET_SENSOR_READING sturcture pointer. This 
                                        will store the data get from sensor.
 
 @retval EFI_SUCCESS                    Get sensor reading success.
 @retval !EFI_SUCCESS                   Get sensor reading fail.
*/
EFI_STATUS
GetSensorReading (
  IN  UINT8                             Sensor,
  OUT H2O_IPMI_GET_SENSOR_READING       *Reading
  )
{
  UINT8                 RecvSize;

  return IpmiLibExecuteIpmiCmd (
           H2O_IPMI_NETFN_SENSOR_EVENT,
           H2O_IPMI_CMD_GET_SENSOR_READING,
           &Sensor,
           1,
           Reading,
           &RecvSize
           );
  
}


/**
 Get specified sensor threshold.

 @param[in]         Sensor              Sensor ID.
 @param[out]        Buffer              Buffer to store sensor threshold data.
 
 @retval EFI_SUCCESS                    Get sensor threshold success.
 @retval !EFI_SUCCESS                   Get sensor threshold fail.
*/
EFI_STATUS
GetSensorThreshold (
  IN  UINT8     Sensor,
  OUT VOID      *Buffer        
  )
{
  UINT8                RecvSize; 

  RecvSize = 0;
  return IpmiLibExecuteIpmiCmd (
           H2O_IPMI_NETFN_SENSOR_EVENT,
           H2O_IPMI_CMD_GET_SENSOR_THRESHOLD,
           &Sensor,
           1,
           Buffer,
           &RecvSize
           );
  
}


/**
 Fill SDR_DATA_STRUCT structure from sensor record data. This mainly process full sensor type.

 @param[in]         Record              H2O_IPMI_SDR_FULL_SENSOR_RECORD structure pointer. This
                                        contains sensor record data.
 @param[out]        SdrBuffer           SDR_DATA_STURCT structure pointer. This will return to caller.
 @param[in]         Flag                Low byte: If low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                                  return full unit string in SDR_DATA_STURCT. Otherwise
                                                  it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string.
                                                   Maximum is 10 digital.
 
 @retval EFI_SUCCESS                    Fill SdrBuffer success.
 @retval EFI_UNSUPPORTED                Fill SdrBuffer fail.
*/
EFI_STATUS
FillFullSensor (
  IN   H2O_IPMI_SDR_FULL_SENSOR_RECORD   *Record,
  OUT  SDR_DATA_STURCT                   *SdrBuffer,
  IN   UINT16                            Flag
  )
{
  EFI_STATUS                    Status;
  UINT8                         ThresholdPointer[7];
  H2O_IPMI_GET_SENSOR_READING   Reading;
  BOOLEAN                       IsAnalog;
  CHAR16                        UnitBase[0x20];
  CHAR16                        UnitModifier[0x20];



  //
  // Check if this is a analog sensor
  //
  IsAnalog = (Record->AnalogDataFormat != 3) ? TRUE : FALSE;



  //
  // Fill Sensor Name
  //
  switch (Record->IdStringCode.Type) {

  case TYPE_LENGTH_BYTE_8BIT_ASCII:
    Ascii8BitToUnicode (
      Record->IdString,
      Record->IdStringCode.Length,
      SdrBuffer->Name
      );
    break;

  default:
    StrCpy (SdrBuffer->Name, DEFAULT_SENSOR_NAME);
    break;

  }

  //
  // Fill Sensor number
  //
  SdrBuffer->SensorNumber = Record->SensorNumber;


  //
  // Only support liner sensor now
  //
  if (IsAnalog && Record->LinearizationEnum >= 0x70) {
    return EFI_UNSUPPORTED;
  }



  //
  // Get Sensor Reading
  //
  Status = GetSensorReading (Record->SensorNumber, &Reading);
  if (!EFI_ERROR (Status)) {

    //
    // Need to check ReadingUnavailable bit and ScanDisable bit.
    // According to IPMI Specification,
    // if ReadingUnavailable = 1, the sensor reading is unavailable.
    // if ScanDisable = 0, the sensor reading is unavailable.
    //
    if (!Reading.ReadingUnavailable && Reading.ScanDisable) {
      SdrBuffer->DataStatus |= SDR_VALUE_AVAILABLE;
    }

  }



  //
  // Analog sensor needs to check more
  //
  if (IsAnalog) {
    SdrBuffer->DataStatus |= SDR_SENSOR_TYPE_ANALOG;
    
    if (SdrBuffer->DataStatus & SDR_VALUE_AVAILABLE) {
      if (Reading.SensorReading != 0) {
        SdrBuffer->Value = ConvertSensorReading(Record, Reading.SensorReading);
      } else {
        SdrBuffer->Value = 0;
      }
      DoubleToStr (SdrBuffer->Value, (UINT8)(Flag >> 8), SdrBuffer->ValueStr);
      SdrBuffer->DataStatus |= (Reading.Data[0] << 8);
      
    } else {
      StrCpy (SdrBuffer->ValueStr, DEFAULT_NO_VALUE);
    }



    //
    // Get Sensor Reading Unit
    // Check DISPLAY_LONG_UNIT flag. Default we use short unit string,
    // if DISPLAY_LONG_UNIT flag is set, we use long unit string.
    //
    if (!(Flag & DISPLAY_LONG_UNIT) && 
        mSensorUnitDesc[Record->SensorUnitBase].Short != NULL ) {
      StrCpy (UnitBase, mSensorUnitDesc[Record->SensorUnitBase].Short);

    } else {
      StrCpy (UnitBase, mSensorUnitDesc[Record->SensorUnitBase].Long);

    } 

    if (!(Flag & DISPLAY_LONG_UNIT) && 
        mSensorUnitDesc[Record->SensorUnitModifier].Short != NULL ) {
      StrCpy (UnitModifier, mSensorUnitDesc[Record->SensorUnitModifier].Short);

    } else {
      StrCpy (UnitModifier, mSensorUnitDesc[Record->SensorUnitModifier].Long);

    } 
    
    switch (Record->ModifierUnit) {
    case 2:
      StrCpy (SdrBuffer->Unit, UnitBase);
      StrCat (SdrBuffer->Unit, L"*");
      StrCat (SdrBuffer->Unit, UnitModifier);

      break;

    case 1:
      StrCpy (SdrBuffer->Unit, UnitBase);
      StrCat (SdrBuffer->Unit, L"/");
      StrCat (SdrBuffer->Unit, UnitModifier);

      break;

    case 0:
    default:
      StrCpy (SdrBuffer->Unit, UnitBase);
      if (!(Flag & DISPLAY_LONG_UNIT)) {
        //
        // Check for degrees symbol
        //
        switch (Record->SensorUnitBase) {
        case 1:
        case 2:
        case 3:
          SdrBuffer->Unit[0] = 0x00B0;
          break;
        }
        
      }

      break;
    }
    
  } else {
    //
    // We still use SdrBuffer->Value to store discrete sensor value.
    // But we need to keep in mind we use SdrBuffer->Value as UINT32 when
    // it is a discrete sensor
    //
    if (SdrBuffer->DataStatus & SDR_VALUE_AVAILABLE) {
      SdrBuffer->Value = (UINT64)Reading.SensorReading;
      UnicodeValueToString (SdrBuffer->ValueStr, LEFT_JUSTIFY, (UINT64)Reading.SensorReading, 0);
      
    } else {
      StrCpy (SdrBuffer->ValueStr, DEFAULT_NO_VALUE);
    }
    
    StrCpy (SdrBuffer->Unit, DISCRETE_UNIT);
    
  }


  //
  // Get Sensor Thresholds
  //
  ZeroMem (ThresholdPointer, (sizeof (ThresholdPointer)));
  Status = GetSensorThreshold (Record->SensorNumber, ThresholdPointer);
  if (EFI_ERROR (Status)) {
    ThresholdPointer[0] = (UINT8)(Record->DiscreateReadingMask & 0x003F);
    ThresholdPointer[1] = Record->LowNonCriticalThreshold;
    ThresholdPointer[2] = Record->LowCriticalThreshold;
    ThresholdPointer[3] = Record->LowNonRecoverThreshold;
    ThresholdPointer[4] = Record->UpNonCriticalThreshold;
    ThresholdPointer[5] = Record->UprCriticalThreshold;
    ThresholdPointer[6] = Record->UpNonRecoverThreshold;
  }



  //
  // Parse Threshold Data
  // 1. Lower Non Recoverable Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_LOWER_NON_RECOV_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_LOWER_NON_RECOVER_AVAILABLE;
    SdrBuffer->LowNonRecoverThres = ConvertSensorReading (Record, ThresholdPointer[3]);
    DoubleToStr (SdrBuffer->LowNonRecoverThres, (UINT8)(Flag >> 8), SdrBuffer->LowNonRecoverThresStr);

  } else {
    StrCpy (SdrBuffer->LowNonRecoverThresStr, DEFAULT_NO_VALUE);
  }

  //
  // 2. Lower Critical Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_LOWER_CRIT_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_LOWER_CRITICAL_AVAILABLE;
    SdrBuffer->LowCriticalThres = ConvertSensorReading (Record, ThresholdPointer[2]);
    DoubleToStr (SdrBuffer->LowCriticalThres, (UINT8)(Flag >> 8), SdrBuffer->LowCriticalThresStr);

  } else {
    StrCpy (SdrBuffer->LowCriticalThresStr, DEFAULT_NO_VALUE);
  }

  //
  // 3. Lower Non Critical Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_LOWER_NON_CRIT_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_LOWER_NON_CRITICAL_AVAILABLE;
    SdrBuffer->LowNonCriticalThres = ConvertSensorReading (Record, ThresholdPointer[1]);
    DoubleToStr (SdrBuffer->LowNonCriticalThres, (UINT8)(Flag >> 8), SdrBuffer->LowNonCriticalThresStr);

  } else {
    StrCpy (SdrBuffer->LowNonCriticalThresStr, DEFAULT_NO_VALUE);
  }

  //
  // 4. Upper Non Critical Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_UPPER_NON_CRIT_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_UPPER_NON_CRITICAL_AVAILABLE;
    SdrBuffer->UpNonCriticalThres = ConvertSensorReading (Record, ThresholdPointer[4]);
    DoubleToStr (SdrBuffer->UpNonCriticalThres, (UINT8)(Flag >> 8), SdrBuffer->UpNonCriticalThresStr);

  } else {
    StrCpy (SdrBuffer->UpNonCriticalThresStr, DEFAULT_NO_VALUE);
  }

  //
  // 5. Upper Critical Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_UPPER_CRIT_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_UPPER_CRITICAL_AVAILABLE;
    SdrBuffer->UpCriticalThres = ConvertSensorReading (Record, ThresholdPointer[5]);
    DoubleToStr (SdrBuffer->UpCriticalThres, (UINT8)(Flag >> 8), SdrBuffer->UpCriticalThresStr);

  } else {
    StrCpy (SdrBuffer->UpCriticalThresStr, DEFAULT_NO_VALUE);
  }

  //
  // 6. Upper Critical Threshold
  //
  if (ThresholdPointer[0] & SDR_SENSOR_T_UPPER_NON_RECOV_SPECIFIED) {
    SdrBuffer->DataStatus |= SDR_UPPER_NON_RECOVER_AVAILABLE;
    SdrBuffer->UpNonRecoverThres = ConvertSensorReading (Record, ThresholdPointer[6]);
    DoubleToStr (SdrBuffer->UpNonRecoverThres, (UINT8)(Flag >> 8), SdrBuffer->UpNonRecoverThresStr);

  } else {
    StrCpy (SdrBuffer->UpNonRecoverThresStr, DEFAULT_NO_VALUE);
  }


  return EFI_SUCCESS;
  
}


/**
 Fill SDR_DATA_STRUCT structure from sensor record data. This mainly process compact sensor type.

 @param[in]         Record              H2O_IPMI_SDR_FULL_SENSOR_RECORD structure pointer. This
                                        contains sensor record data.
 @param[out]        SdrBuffer           SDR_DATA_STURCT structure pointer. This will return to caller.
 
 @retval EFI_SUCCESS                    Fill SdrBuffer success.
 @retval EFI_UNSUPPORTED                Fill SdrBuffer fail.
*/
EFI_STATUS
FillCompactSensor (
  IN  H2O_IPMI_SDR_COMPACT_SENSOR_RECORD *Record,
  IN  SDR_DATA_STURCT                    *SdrBuffer
  )
{
  EFI_STATUS                   Status;
  H2O_IPMI_GET_SENSOR_READING  Reading;



  //
  // Get Sensor Name
  //
  switch (Record->IdStringCode.Type) {
  case TYPE_LENGTH_BYTE_8BIT_ASCII:
    Ascii8BitToUnicode (Record->IdString, Record->IdStringCode.Length, SdrBuffer->Name);
    break;
    
  default:
    StrCpy (SdrBuffer->Name, DEFAULT_SENSOR_NAME);
    break;
  }

  //
  // Fill Sensor number
  //
  SdrBuffer->SensorNumber = Record->SensorNumber;


  //
  // Get Sensor Reading
  //
  Status = GetSensorReading (Record->SensorNumber, &Reading);
  if (!EFI_ERROR (Status)) {

    //
    // Need to check ReadingUnavailable bit and ScanDisable bit.
    // According to IPMI Specification,
    // if ReadingUnavailable = 1, the sensor reading is unavailable.
    // if ScanDisable = 0, the sensor reading is unavailable.
    //
    if (!Reading.ReadingUnavailable && Reading.ScanDisable) {
      SdrBuffer->DataStatus |= SDR_VALUE_AVAILABLE;
    }

  }
  


  //
  // We still use SdrBuffer->Value to store compact sensor value.
  // But we need to keep in mind we use SdrBuffer->Value as UINT32 when
  // it is a compact sensor
  //
  if (SdrBuffer->DataStatus & SDR_VALUE_AVAILABLE) {
    SdrBuffer->Value = (UINT64)Reading.SensorReading;
    UnicodeValueToString (SdrBuffer->ValueStr, LEFT_JUSTIFY, (UINT64)Reading.SensorReading, 0);
    
  } else {
    StrCpy (SdrBuffer->ValueStr, DEFAULT_NO_VALUE);
  }
  
  StrCpy (SdrBuffer->Unit, DISCRETE_UNIT);

  return EFI_SUCCESS;
  
}


/**
 Get Reserved SDR Repository ID.

 @param[out]        ReserveId           Reserved SDR Repository ID.
 
 @retval EFI_SUCCESS                    Get Reserved SDR Repository ID success.
 @retval !EFI_SUCCESS                   Get Reserved SDR Repository ID fail.
*/
EFI_STATUS
ReserveSdrRepository (
  OUT UINT16   *ReserveId
  )
{
  EFI_STATUS            Status;
  UINT16                Id;
  UINT8                 RecvSize;
  UINT8                 NetFn;
  UINT8                 Cmd;

  if (mUseBuiltIn) {
    NetFn = H2O_IPMI_NETFN_SENSOR_EVENT;
    Cmd = H2O_IPMI_CMD_RESERVE_DEVICE_SDR_REPOSITORY;
  } else {
    NetFn = H2O_IPMI_NETFN_STORAGE;
    Cmd = H2O_IPMI_CMD_RESERVE_SDR_REPOSITORY;
  }

  Status = IpmiLibExecuteIpmiCmd (NetFn, Cmd, NULL, 0, &Id, &RecvSize);

  if (!EFI_ERROR (Status)) {
    *ReserveId = Id;
  }

  return Status;
  
}


/**
 Get sensor record header.

 @param[in]         CurrId              Record ID.
 @param[in, out]    ReserveId           IN:  Current reserved SDR Repository ID.
                                        OUT: If lost reservation, return new reservation ID. 
 @param[out]        HeaderBuffer        Buffer to store record data.
 @param[out]        NextId              Pointer to store next record ID.
 
 @retval EFI_SUCCESS                    Get header success.
 @retval !EFI_SUCCESS                   Get header fail.
*/
EFI_STATUS
GetSdrHeader (
  IN     UINT16  CurrId,
  IN OUT UINT16  *ReserveId,
  OUT    VOID    *HeaderBuffer,
  OUT    UINT16  *NextId
  )
{
  EFI_STATUS                      Status;
  UINT8                           RecvSize;           
  UINT8                           Buffer[0x10];
  UINT8                           Retry;
  UINT8                           *Header;
  UINT16                          TempReserveId;
  H2O_IPMI_GET_SDR_REQUEST_DATA   Req;
  UINT8                           NetFn;
  UINT8                           Cmd;

  if (mUseBuiltIn) {
    NetFn = H2O_IPMI_NETFN_SENSOR_EVENT;
    Cmd = H2O_IPMI_CMD_GET_DEVICE_SDR;

  } else {
    NetFn = H2O_IPMI_NETFN_STORAGE;
    Cmd = H2O_IPMI_CMD_GET_SDR;
  }

  Req.ReservationId = *ReserveId;
  Req.RecordId = CurrId;
  Req.Offset = 0;
  Req.ByteToRead = sizeof (H2O_IPMI_SDR_RECORD_HEADER);
  Header = (UINT8*)HeaderBuffer;

  Status = EFI_SUCCESS;
  for (Retry = 0; Retry < 5; ++Retry) {
    Status = IpmiLibExecuteIpmiCmd (
                         NetFn,
                         Cmd,
                         &Req,
                         sizeof(H2O_IPMI_GET_SDR_REQUEST_DATA),
                         Buffer,
                         &RecvSize
                         );

    if (!EFI_ERROR (Status)) {
      //
      // Get SDR header successfully
      //
      CopyMem (Header, &Buffer[2], sizeof (H2O_IPMI_SDR_RECORD_HEADER));
      *NextId = *(UINT16*)Buffer;
      break;
      
    } else {
      //
      // Lost Reservation 
      //
      if (Buffer[0] == 0xC5) {
        MicroSecondDelay (1000);

        Status = ReserveSdrRepository (&TempReserveId);
        if (EFI_ERROR (Status)) {
          break;
        }
        Req.ReservationId = TempReserveId;
        *ReserveId = TempReserveId;
      }
    }
  }

  return Status;

  
}


/**
 Get sensor record data.
 
 @param[in]         Header              Sensor record header.
 @param[in]         CurrId              Record ID.
 @param[in, out]    ReserveId           IN:  Current reserved SDR Repository ID.
                                        OUT: If lost reservation, return new reservation ID.
 @param[out]        RecordBuffer        Buffer to store record data.
 
 @retval EFI_SUCCESS                    Get record data success.
 @retval !EFI_SUCCESS                   Get record data fail.
*/
EFI_STATUS
GetSdrRecord (
  IN     H2O_IPMI_SDR_RECORD_HEADER   *Header,
  IN     UINT16                       CurrId,
  IN OUT UINT16                       *ReserveId,
  OUT    VOID                         *RecordBuffer
  )
{
  EFI_STATUS                      Status;
  UINT8                           RecvSize;           
  UINT8                           Buffer[0x50];
  UINT16                          TempReserveId;
  STATIC UINT8                    MaxReadLength;
  UINT8                           Index;
  UINT8                           Length;
  UINT8                           NetFn;
  UINT8                           Cmd;
  H2O_IPMI_GET_SDR_REQUEST_DATA   Req;

  Length = Header->RecordLength;
  Index = 0;

  if (mUseBuiltIn) {
    NetFn = H2O_IPMI_NETFN_SENSOR_EVENT;
    Cmd = H2O_IPMI_CMD_GET_DEVICE_SDR;

  } else {
    NetFn = H2O_IPMI_NETFN_STORAGE;
    Cmd = H2O_IPMI_CMD_GET_SDR;
    
  }

  Req.ReservationId = *ReserveId;
  Req.RecordId = CurrId;

  Status = EFI_SUCCESS;
  MaxReadLength = 0xFF;
  while (Index < Length) {
    Req.ByteToRead = (Length - Index < MaxReadLength) ? Length - Index : MaxReadLength;
    Req.Offset = Index + 5;

    Status = IpmiLibExecuteIpmiCmd (
               NetFn,
               Cmd,
               &Req, 
               sizeof (H2O_IPMI_GET_SDR_REQUEST_DATA),
               Buffer, 
               &RecvSize
               );

    if (!EFI_ERROR (Status)) {
      //
      // Get all data successfully
      //
      CopyMem (((UINT8*)RecordBuffer + Index), &Buffer[2], Req.ByteToRead);
      Index += MaxReadLength;

    } else {
      if (Buffer[0] == 0xCA) {
        //
        // Read too mamy bytes at once
        //
        MaxReadLength = Req.ByteToRead - 1;

      } else if (Buffer[0] == 0xC5) {
        //
        // Lost Reservation 
        //
        MicroSecondDelay (1000);

        Status = ReserveSdrRepository (&TempReserveId);
        if (EFI_ERROR (Status)) {
          break;
        }
        Req.ReservationId = TempReserveId;
        *ReserveId = TempReserveId;

      } else {
        break;
      }
        
    }
      
  }

  return Status;
  
}


/**
 Get SDR information using Get SDR Repository Info command.

 @param[out]        RepoInfo            H2O_IPMI_GET_SDR_REPOSITORY_INFO structure pointer. This will
                                        store SDR repository information.
 
 @retval EFI_SUCCESS                    Get SDR information success.
 @retval !EFI_SUCCESS                   Get SDR information fail.
*/
EFI_STATUS
GetSdrRepositoryInfo (
  OUT H2O_IPMI_GET_SDR_REPOSITORY_INFO *RepoInfo
  )
{
  UINT8                RecvSize; 

  return IpmiLibExecuteIpmiCmd (
           H2O_IPMI_NETFN_STORAGE,
           H2O_IPMI_CMD_GET_SDR_REPOSITORY_INFO,
           NULL,
           0,
           RepoInfo, 
           &RecvSize
           );

}


/**
 Get SDR information using Get Device SDR Info command.

 @param[out]        SdrInfo             H2O_IPMI_GET_DEVICE_SDR_INFO structure pointer. This will
                                        store SDR repository information.
 
 @retval EFI_SUCCESS                    Get SDR information success.
 @retval !EFI_SUCCESS                   Get SDR information fail.
*/
EFI_STATUS
GetDeviceSdrInfo (
  OUT H2O_IPMI_GET_DEVICE_SDR_INFO *SdrInfo
  )
{
  UINT8                RecvSize; 

  return IpmiLibExecuteIpmiCmd (
           H2O_IPMI_NETFN_SENSOR_EVENT,
           H2O_IPMI_CMD_GET_DEVICE_SDR_INFO,
           NULL,
           0,
           SdrInfo, 
           &RecvSize
           );

}


/**
 Get SDR record.

 @param[in]         CurrId              Record ID.
 @param[in]         ReserveId           Reserved SDR Repository ID.
 @param[in]         Buffer              Buffer to store SDR data.
 @param[out]        NextId              Pointer to store next record ID.
 
 @retval EFI_SUCCESS                    Get SDR success.
 @retval !EFI_SUCCESS                   Get SDR fail.
*/
EFI_STATUS
GetSdr (
  IN  UINT16      CurrId,
  IN  UINT16      *ReserveId,
  IN  UINT8       *Buffer,
  OUT UINT16      *NextId
  )
{
  EFI_STATUS                          Status;
  H2O_IPMI_SDR_RECORD_HEADER          *Header;
  H2O_IPMI_SDR_FULL_SENSOR_RECORD     *FullSdr;
  
  Status = GetSdrHeader (CurrId, ReserveId, Buffer, NextId);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Header = (H2O_IPMI_SDR_RECORD_HEADER*)Buffer;
  
  switch (Header->RecordType) {
    
  case SDR_RECORD_TYPE_FULL_SENSOR:
  case SDR_RECORD_TYPE_COMPACT_SENSOR:
    Status = GetSdrRecord (Header, CurrId, ReserveId, &Buffer[5]);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    break;

  default:
    return EFI_UNSUPPORTED;
    break;
    
  }
  
  //
  // Create full sdr record table.
  //
  if ((mFullSdrRecordTable == NULL)) {
    mFullSdrRecordTable = AllocateZeroPool ((UINTN)((sizeof (H2O_IPMI_SDR_FULL_SENSOR_RECORD)) * MAX_SENSOR_NUMBER));
  }
  
  //
  // Check the table was built and did not have all full type records already.
  //
  if ((mFullSdrRecordTable != NULL) && (!mHaveAllFullRecords) && 
      (Header->RecordType == SDR_RECORD_TYPE_FULL_SENSOR)) {
    FullSdr = (H2O_IPMI_SDR_FULL_SENSOR_RECORD*)Buffer;
    
    //
    // Use SensorNumber and SDR Version to check the record already exist or not,
    // if did not fill this record before, then fill record by sensor number.
    //
    if ((mFullSdrRecordTable[FullSdr->SensorNumber].Header.SdrVersion != DEFAULT_SDR_VERSION) ||
        (mFullSdrRecordTable[FullSdr->SensorNumber].SensorNumber != FullSdr->SensorNumber)) {
      CopyMem (&mFullSdrRecordTable[FullSdr->SensorNumber], FullSdr, sizeof (H2O_IPMI_SDR_FULL_SENSOR_RECORD));
    }
  }

  return EFI_SUCCESS;


}


/**
 Fill SDR_DATA_STRUCT structure from sensor record data. This mainly process full sensor type.

 @param[out]        SdrBuffer           SDR_DATA_STURCT structure pointer. This will return to caller.
 @param[in]         CurrId              Specified record ID.
 @param[in]         Buffer              Buffer that contains sensor record.
 @param[in]         Flag                Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                                  return full unit string in SDR_DATA_STURCT. Otherwise
                                                  it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string.
                                                   Maximum is 10 digital.
 
 @retval EFI_SUCCESS                    Fill SdrBuffer success.
 @retval !EFI_SUCCESS                   Fill SdrBuffer fail.
*/
EFI_STATUS
FillSdrBuffer (
  OUT SDR_DATA_STURCT    *SdrBuffer,
  IN  UINT16             CurrId,
  IN  UINT8              *Buffer,
  IN  UINT16             Flag
  )
{
  EFI_STATUS                         Status;
  H2O_IPMI_SDR_RECORD_HEADER         *Header;
  H2O_IPMI_SDR_FULL_SENSOR_RECORD    *Type01Record;
  H2O_IPMI_SDR_COMPACT_SENSOR_RECORD *CompactRecord;



  //
  // Fill SdrBuffer according to different record type
  //
  Status = EFI_SUCCESS;
  Header = (H2O_IPMI_SDR_RECORD_HEADER*)Buffer;
  switch (Header->RecordType) {
    
  case SDR_RECORD_TYPE_FULL_SENSOR:
    Type01Record = (H2O_IPMI_SDR_FULL_SENSOR_RECORD*)Buffer;
    Status = FillFullSensor (Type01Record, SdrBuffer, Flag);

    SdrBuffer->RecordId = CurrId;

    break;

  case SDR_RECORD_TYPE_COMPACT_SENSOR:
    CompactRecord = (H2O_IPMI_SDR_COMPACT_SENSOR_RECORD*)Buffer;
    Status = FillCompactSensor (CompactRecord, SdrBuffer);

    SdrBuffer->RecordId = CurrId;
    
    break;

  }

  return Status;

  
}


/**
 Get sensor data from "SDR Repository Device" or "Device SDR".

 @param[in]         This                Pointer to H2O_IPMI_SDR_PROTOCOL instance.
 @param[in]         RecordId            Speficied ID of SDR that want to get. 0xFFFF means getting
                                        all recoreds.
 @param[in]         Flag                Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                                  return full unit string in SDR_DATA_STURCT. Otherwise
                                                  it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string.
                                                   Maximum is 10 digital.
 @param[out]        Count               Number of records if user passes 0xFFFF in RecordId.
 @param[out]        Record              A buffer to SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.
 
 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Speificed Id does not exist or cannot get SDR data.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. Count is NULL
                                        3. Record is NULL
*/
EFI_STATUS
EFIAPI
GetSdrData (
  IN  H2O_IPMI_SDR_PROTOCOL       *This,
  IN  UINT16                      RecordId,
  IN  UINT16                      Flag,
  OUT UINT16                      *Count,
  OUT SDR_DATA_STURCT             **Record
  )
{
  EFI_STATUS                         Status;
  UINT16                             SdrCount;
  UINT16                             CurrId;
  UINT16                             NextId;
  UINT8                              Buffer[0x50];
  UINT16                             ReserveId;
  H2O_IPMI_GET_SDR_REPOSITORY_INFO   RepoInfo;
  H2O_IPMI_GET_DEVICE_SDR_INFO       SdrInfo;
  SDR_DATA_STURCT                    *SdrBuffer;
  UINT16                             CurrSdrCount;
  UINT16                             SdrBufferNumber;
  UINT16                             Index;



  //
  // Check parameters first
  //
  if (This == NULL || Count == NULL || Record == NULL) {
    return EFI_INVALID_PARAMETER;
  }



  //
  // Check we use Device SDR or SDR Repository
  //
  Status = CheckUseBuiltIn ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  

  //
  // Get RecordCount, it may come from Device SDR or SDR Repository
  //
  if (mUseBuiltIn) {
    Status = GetDeviceSdrInfo (&SdrInfo);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    DEBUG ((EFI_D_ERROR, "[H2OIpmiSdrDriver] Use Get Device SDR Info\n"));

    SdrCount = SdrInfo.Count;
    
  } else {
    Status = GetSdrRepositoryInfo (&RepoInfo);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    DEBUG ((EFI_D_ERROR, "[H2OIpmiSdrDriver] Use Get SDR Repository Info\n"));

    SdrCount = RepoInfo.RecordCount;
  }

  DEBUG ((EFI_D_ERROR, "[H2OIpmiSdrDriver] SDR Count: 0x%02x\n", SdrCount));



  //
  // Check speficied ID
  //
  if (RecordId != SDR_ID_ALL) {
    return EFI_UNSUPPORTED;
  }

  

  //
  // Get Reservation ID
  //
  Status = ReserveSdrRepository (&ReserveId);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }



  //
  // Allocate buffer to store SDR record
  //
  SdrBuffer = NULL;
  SdrBufferNumber = (RecordId == SDR_ID_ALL) ? SdrCount : 1;
  SdrBuffer = AllocateZeroPool (sizeof(SDR_DATA_STURCT) * SdrBufferNumber);

  if (SdrBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  

  //
  // Start to get all SDR
  //
  CurrSdrCount = 0;
  if (RecordId == SDR_ID_ALL) {
    //
    // Get All SDR
    //
    CurrId = 0;
    for (Index = 0; Index < SdrCount; ++Index) {
      Status = GetSdr (CurrId, &ReserveId, Buffer, &NextId);
      if (EFI_ERROR (Status)) {
        CurrId = NextId;
        continue;
      }

      Status = FillSdrBuffer (&SdrBuffer[CurrSdrCount], CurrId, Buffer, Flag);
      if (EFI_ERROR (Status)) {
        CurrId = NextId;
        continue;
      }

      ++CurrSdrCount;
      CurrId = NextId;

    }
    
    //
    // After get all SDR, also have all full record type records in FullSdrRecordTable.
    //
    mHaveAllFullRecords = TRUE;
    
  } else {
    //
    // Get One Specified SDR
    //
    Status = GetSdr (RecordId, &ReserveId, Buffer, &NextId);
    if (EFI_ERROR (Status)) {
      FreePool (SdrBuffer);
      return EFI_UNSUPPORTED;
    }

    Status = FillSdrBuffer (SdrBuffer, RecordId, Buffer, Flag);
    if (EFI_ERROR (Status)) {
      FreePool (SdrBuffer);
      return EFI_UNSUPPORTED;
    }

    ++CurrSdrCount;
  }


  *Count = CurrSdrCount;
  *Record = SdrBuffer;


  return EFI_SUCCESS;
  
}


/**
 Get sensor raw data from "SDR Repository Device" or "Device SDR".

 @param[in]         This                Pointer to H2O_IPMI_SDR_PROTOCOL instance.
 @param[in]         RecordId            Speficied ID of SDR that want to get.
 @param[out]        Record              A buffer to SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.
                                       
 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Speificed Id does not exist or cannot get SDR data.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. Record is NULL
*/
EFI_STATUS
EFIAPI
GetSdrRaw (
  IN  H2O_IPMI_SDR_PROTOCOL       *This,
  IN  UINT16                      RecordId,
  OUT H2O_IPMI_SDR_RAW            **Record
  )
{
  EFI_STATUS                         Status;
  UINT16                             ReserveId;
  UINT16                             NextId;
  UINT8                              *Buffer;



  //
  // Check parameters first
  //
  if (This == NULL ||  Record == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check we use Device SDR or SDR Repository
  //
  Status = CheckUseBuiltIn ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Get Reservation ID
  //
  Status = ReserveSdrRepository (&ReserveId);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  
  Buffer = NULL;
  Buffer = AllocateZeroPool (MAX_SDR_DATA_LEN + sizeof (NextId));
  if (Buffer == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  
  Status = GetSdr (RecordId, &ReserveId, &Buffer[2], &NextId);
  CopyMem (Buffer, &NextId, sizeof (NextId));
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return EFI_UNSUPPORTED;
  }
  
  *Record = (H2O_IPMI_SDR_RAW *)Buffer;
  
  return EFI_SUCCESS;
  
}


/**
 Convert analog sensor reading or threshold event data (trigger reading or trigger threshold) value to string.

 @param[in]         This                A pointer to H2O_IPMI_SDR_PROTOCOL structure.
 @param[in]         SensorNumber        Unique number identifying the sensor.
 @param[in]         SensorReading       A byte sensor reading or event data (contain trigger reading or 
                                        trigger threshold) value need to convert.
 @param[in]         Precision           The precision of value to string.
 @param[out]        ConvertValueStr     The point of CHAR16 string space to carry converted value string.
 
 @retval EFI_SUCCESS                    Convert sensor reading value success.
 @retval EFI_NOT_READY                  Get sensor data record fail.
 @retval EFI_UNSUPPORTED                Unique sensor number can't find the corresponding SDR or 
                                        the record type of sensor number is not full type.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. ConvertValueStr is NULL
*/
EFI_STATUS
EFIAPI
SensorReadingToStr (
  IN  H2O_IPMI_SDR_PROTOCOL             *This,
  IN  UINT8                             SensorNumber,
  IN  UINT8                             SensorReading,
  IN  UINT8                             Precision,
  OUT CHAR16                            *ConvertValueStr
  )
{
  EFI_STATUS                            Status;
  UINT64                                TempValue;
  H2O_IPMI_SDR_PROTOCOL                 *Sdr;
  SDR_DATA_STURCT                       *SdrData;
  UINT16                                SdrCount;  
  H2O_IPMI_SDR_FULL_SENSOR_RECORD       *Record;

  if (This == NULL || ConvertValueStr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If mFullSdrRecordTable without the corresponding full SDR and did not have all full type records already, 
  // then create table and get all full type records by using GetSdrData function.
  //
  Sdr = NULL;
  if ((mFullSdrRecordTable == NULL) || 
      ((mFullSdrRecordTable[SensorNumber].SensorNumber != SensorNumber) && (mHaveAllFullRecords != TRUE))) {
    Status = GetSdrData (Sdr, SDR_ID_ALL, 0x0300, &SdrCount, &SdrData);
    if (!EFI_ERROR (Status)) { 
      FreePool (SdrData);
    } else {
      return EFI_NOT_READY;
    }
  }

  //
  // If record type is not full sensor record or SDR version is not 0x51 or
  // can't find the corresponding SDR, without linearization field to convert.
  //
  if ((mFullSdrRecordTable[SensorNumber].Header.RecordType != SDR_RECORD_TYPE_FULL_SENSOR) ||
      (mFullSdrRecordTable[SensorNumber].Header.SdrVersion != DEFAULT_SDR_VERSION) ||
      (mFullSdrRecordTable[SensorNumber].SensorNumber != SensorNumber)) {
    return EFI_UNSUPPORTED;
  }

  Record = &mFullSdrRecordTable[SensorNumber];

  TempValue = ConvertSensorReading (Record, SensorReading);
  
  DoubleToStr (TempValue, Precision, ConvertValueStr);

  return EFI_SUCCESS;

}


/**
 Fill private data content.

*/
VOID
InitialIpmiSdrContent (
  VOID
  )
{
  mSdrProtocol.GetSdrData = GetSdrData;
  mSdrProtocol.GetSdrRaw = GetSdrRaw;
  mSdrProtocol.SensorReadingToStr = SensorReadingToStr;
  
}

