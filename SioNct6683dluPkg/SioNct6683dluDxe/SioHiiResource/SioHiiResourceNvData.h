/** @file

Declaration file for NV Data

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SIO_NCT6683DLU_HII_RESOURCE_NV_DATA_H_
#define _SIO_NCT6683DLU_HII_RESOURCE_NV_DATA_H_

#include <Guid/SioHiiResourceForm.h>
#include <Sio/SioCommon.h>

//
// Used by VFR for form or button identification
//
#define SIO_CONFIGURATION_FORM_ID       0x01
#define SIO_CONFIGURATION_VARSTORE_ID   0x1234
#define SIO_HARDWARE_MONITOR_FORM_ID      0x4002
#define SIO_HWM_LABEL                     0x4400
#define SIO_HWM_CONFIGURATION_VARSTORE_ID 0x4343
#define SIO_HWM_QUESTION_ID               0x4350
#define SIO_WDT_QUESTION_ID               0x4351
#define SIO_HWM_UPDATE_QUESTION_ID        0x4352
#define SIO_VOL_LABEL                     0x4400
#define SIO_TEMP_LABEL                    0x4500
#define SIO_FAN_LABEL                     0x4600                    
#define SIO_LABEL_END                     0x4700



//
// EFI Variable attributes
//
#define EFI_VARIABLE_NON_VOLATILE       0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS     0x00000004
#define EFI_VARIABLE_READ_ONLY          0x00000008

//
// VarOffset that will be used to create question
// all these values are computed from the structure
// defined below
//
#define VAR_OFFSET(Field)          ((UINT16) ((UINTN) &(((SIO_CONFIGURATION *) 0)->Field)))
#define SIO_HWM_OFFSET             VAR_OFFSET (HwmConfig)

#define HWM_OFFSET(Field)          ((UINT16) ((UINTN) &(((SIO_HWM_CONFIGURATION *) 0)->Field)))
#define SIO_VOL1_HWM_OFFSET        HWM_OFFSET (VolData1)
#define SIO_VOL2_HWM_OFFSET        HWM_OFFSET (VolData2)
#define SIO_VOL3_HWM_OFFSET        HWM_OFFSET (VolData3)
#define SIO_VOL4_HWM_OFFSET        HWM_OFFSET (VolData4)
#define SIO_VOL5_HWM_OFFSET        HWM_OFFSET (VolData5)
#define SIO_VOL6_HWM_OFFSET        HWM_OFFSET (VolData6)
#define SIO_VOL7_HWM_OFFSET        HWM_OFFSET (VolData7)
#define SIO_VOL8_HWM_OFFSET        HWM_OFFSET (VolData8)
#define SIO_TEMP1_HWM_OFFSET       HWM_OFFSET (TempData1)
#define SIO_TEMP2_HWM_OFFSET       HWM_OFFSET (TempData2)
#define SIO_TEMP3_HWM_OFFSET       HWM_OFFSET (TempData3)
#define SIO_FAN1_HWM_OFFSET        HWM_OFFSET (FanData1)
#define SIO_FAN2_HWM_OFFSET        HWM_OFFSET (FanData2)
#define SIO_FAN3_HWM_OFFSET        HWM_OFFSET (FanData3)

//
// Nv Data structure referenced by IFR
//
#pragma pack(1)
typedef struct {
  UINT16         VolData1[10];
  UINT16         VolData2[10];
  UINT16         VolData3[10];
  UINT16         VolData4[10];
  UINT16         VolData5[10];
  UINT16         VolData6[10];
  UINT16         VolData7[10];
  UINT16         VolData8[10];
  UINT16         TempData1[12];
  UINT16         TempData2[12];
  UINT16         TempData3[12];
  UINT16         FanData1[19];
  UINT16         FanData2[19];
  UINT16         FanData3[19];
  UINT8          Enable;  
} SIO_HWM_CONFIGURATION;

typedef struct {
  SIO_DEVICE_LIST_TABLE DeviceA; ///< ComA
  SIO_DEVICE_LIST_TABLE DeviceB; ///< ComB
  SIO_DEVICE_LIST_TABLE DeviceC; ///< LPT
  SIO_HWM_CONFIGURATION HwmConfig;
} SIO_CONFIGURATION;
#pragma pack()

// Hwm registers
#define HARDWARE_MONITOR_FAN_SPEED_COUNT 1500000

#endif
