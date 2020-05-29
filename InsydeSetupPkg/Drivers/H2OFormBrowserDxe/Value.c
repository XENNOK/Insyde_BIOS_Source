/** @file
 Define function of value operation .  
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "InternalH2OFormBrowser.h"

/**
 Create value in boolean type.

 @param [in] TargetHiiValue     HII value which set type as BOOLEAN
 @param [in] ValueBoolean       Value for boolean type

 @retval EFI_SUCCESS            The function complete successfully.
**/
EFI_STATUS  
CreateValueAsBoolean (
  IN EFI_HII_VALUE *TargetHiiValue, 
  IN BOOLEAN        ValueBoolean
  )
{
  if (TargetHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  TargetHiiValue->Type      = EFI_IFR_TYPE_BOOLEAN;
  TargetHiiValue->BufferLen = sizeof (BOOLEAN);
  TargetHiiValue->Value.b   = ValueBoolean;

  return EFI_SUCCESS;
}

/**
 Create value in UINT64 type.

 @param [in] TargetHiiValue     HII value which set type as UINT64
 @param [in] ValueUint64        Value for UINT64 type

 @retval EFI_SUCCESS            The function complete successfully.
**/
EFI_STATUS  
CreateValueAsUint64 (
  IN EFI_HII_VALUE *TargetHiiValue, 
  IN UINT64         ValueUint64
  )
{
  if (TargetHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  TargetHiiValue->Type      = EFI_IFR_TYPE_NUM_SIZE_64;
  TargetHiiValue->BufferLen = sizeof (UINT64);
  TargetHiiValue->Value.u64 = ValueUint64;

  return EFI_SUCCESS;
}

/**
 Create value in string type.

 @param [in] TargetHiiValue     HII value which set type as string
 @param [in] BufferLen          String buffer length        
 @param [in] Buffer             String buffer

 @retval EFI_SUCCESS            The function complete successfully.
**/
EFI_STATUS  
CreateValueAsString (
  IN EFI_HII_VALUE  *TargetHiiValue, 
  IN UINT16          BufferLen,
  IN UINT8          *Buffer
  )
{
  if (TargetHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  TargetHiiValue->Type      = EFI_IFR_TYPE_STRING;
  TargetHiiValue->BufferLen = BufferLen;
  TargetHiiValue->Buffer    = Buffer;

  return EFI_SUCCESS;  
}

/**
 Copy HII value.
 
 @param [in] DestinationHiiValue   Destination HII value 
 @param [in] SourceHiiValue        Source HII value
      
**/
VOID
CopyHiiValue (
  IN EFI_HII_VALUE         *DestinationHiiValue,
  IN EFI_HII_VALUE         *SourceHiiValue
  )
{
  CopyMem (DestinationHiiValue, SourceHiiValue, sizeof (EFI_HII_VALUE));  
}

/**
 Copy value.
 
 @param [in] DestinationValue   Destination value 
 @param [in] SourceValue        Source value
      
**/
VOID
CopyValue (
  IN EFI_HII_VALUE              *DestinationValue,
  IN EFI_HII_VALUE              *SourceValue
  )
{
  CopyMem (&DestinationValue->Value, &SourceValue->Value, sizeof (EFI_IFR_TYPE_VALUE));
}

/**
 Set value as boolean.
 
 @param [in] SourceHiiValue      Source HII value
 @param [in] ValueBoolean        Set boolaean value

 @retval EFI_SUCCESS             The function complete successfully.      
**/
EFI_STATUS
SetValueBool(
  IN EFI_HII_VALUE   *SourceHiiValue,
  IN BOOLEAN          ValueBoolean    
)
{
  if (SourceHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  SourceHiiValue->Value.b = ValueBoolean;

  return EFI_SUCCESS;   
}

/**
 Dump HII value.
 
 @param [in] DumpHiiValue      HII value which dump

 @retval EFI_SUCCESS            The function complete successfully.      
 
**/
EFI_STATUS 
DumpValue(
IN EFI_HII_VALUE     *DumpHiiValue      
)
{
  CHAR16         *String;

  String = NULL;

  if (DumpHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  switch (DumpHiiValue->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8: 
    DEBUG ((EFI_D_INFO, "Number is %d (8-bit unsigned integer) \n", DumpHiiValue->Value.u8)); 
    break;
    
  case EFI_IFR_TYPE_NUM_SIZE_16:
    DEBUG ((EFI_D_INFO, "Number is %d (16-bit unsigned integer) \n", DumpHiiValue->Value.u16));
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    DEBUG ((EFI_D_INFO, "Number is %d (32-bit unsigned integer) \n", DumpHiiValue->Value.u32));
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    DEBUG ((EFI_D_INFO, "Number is %d (32-bit unsigned integer) \n", DumpHiiValue->Value.u64));
    break;

  case EFI_IFR_TYPE_BOOLEAN:
    if (DumpHiiValue->Value.b == TRUE) {
      DEBUG ((EFI_D_INFO, "Boolean is TRUE\n"));
    } else {
      DEBUG ((EFI_D_INFO, "Boolean is FALSE\n"));   
    }
    break;    

  case EFI_IFR_TYPE_TIME:
    DEBUG ((EFI_D_INFO, "Hour is %d.\n",  DumpHiiValue->Value.time.Hour));
    DEBUG ((EFI_D_INFO, "Minute is %d.\n",  DumpHiiValue->Value.time.Minute));
    DEBUG ((EFI_D_INFO, "Second is %d.\n",  DumpHiiValue->Value.time.Second));
    break;

  case EFI_IFR_TYPE_DATE:
    DEBUG ((EFI_D_INFO, "Year is %d\n", DumpHiiValue->Value.date.Year));
    DEBUG ((EFI_D_INFO, "Month is %d\n", DumpHiiValue->Value.date.Month));
    DEBUG ((EFI_D_INFO, "Day is %d\n", DumpHiiValue->Value.date.Day));    
    break;   

  case EFI_IFR_TYPE_STRING:
    String = GetString (DumpHiiValue->Value.string, mHiiHandle);
    DEBUG ((EFI_D_INFO, "String is %s.\n", String));
    break;  
    
  case EFI_IFR_TYPE_REF:
    DEBUG ((EFI_D_INFO, "Type  is EFI_IFR_TYPE_REF.\n"));
    break;  

  default:
    break;
  }
  return EFI_SUCCESS;   
}

