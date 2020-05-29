/** @file
  Header file for the ICC Over Clocking

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

/*++

   Any software source code reprinted in this document is furnished under 
   a software license and may only be used or copied in accordance with 
   the terms of that license.
 

   Copyright c 2010-2011, Intel Corporation. All rights reserved.

--*/

#ifndef _ICC_OVERCLOCKING_H_
#define _ICC_OVERCLOCKING_H_

/* Types used by ICC LIB */

typedef enum _ICC_LIB_STATUS
{
        ICC_LIB_STATUS_SUCCESS,
        ICC_LIB_STATUS_INVALID_PARAMS,   
        ICC_LIB_STATUS_INVALID_CLOCK_NUMBER,
        ICC_LIB_STATUS_CLOCK_NOT_FOUND,
        ICC_LIB_STATUS_INVALID_FREQ_VALUE,
        ICC_LIB_STATUS_FREQ_TOO_LOW,
        ICC_LIB_STATUS_FREQ_TOO_HIGH,
        ICC_LIB_STATUS_FREQ_MUST_HAVE_ZERO_SSC,   
        ICC_LIB_STATUS_SSC_CHANGE_NOT_ALLOWED,
        ICC_LIB_STATUS_INVALID_SSC_MODE,
        ICC_LIB_STATUS_SSC_MODE_NOT_SUPPORTED,
        ICC_LIB_STATUS_SSC_OUT_OF_RANGE,
        ICC_LIB_STATUS_SSC_TOO_HIGH,
        ICC_LIB_STATUS_SSC_TOO_LOW,
        ICC_LIB_STATUS_SSC_CHANGE_NOT_ALLOWED_SSC_DISABLED,   
        ICC_LIB_STATUS_DYNAMIC_CHANGE_NOT_ALLOWED,
        ICC_LIB_STATUS_INVALID_DMI_PEG_RATIO,
        ICC_LIB_STATUS_REGISTER_IS_LOCKED,
        ICC_LIB_STATUS_MEI_INITIALIZATION_FAILED,   
        ICC_LIB_STATUS_MEI_CONNECTION_FAILED,   
        ICC_LIB_STATUS_UNEXPECTED_FW_ERROR,   
        ICC_LIB_STATUS_UNSUPPORTED_HW,   
        ICC_LIB_STATUS_CLOCK_DISABLED_FAILED_PROGRAMMING,   
        ICC_LIB_STATUS_FREQ_AND_SSC_NOT_MATCH,
        ICC_LIB_STATUS_WAITING_FOR_WARM_RESET,
        ICC_LIB_STATUS_NOT_ALLOWED_IN_USER_MODE,
        ICC_LIB_STATUS_TOO_MANY_CONNECTIONS,
        ICC_LIB_STATUS_INVALID_COOKIE,        
        ICC_LIB_STATUS_DMI_PEG_RATIO_CHANGE_NOT_ALLOWED,        
        ICC_LIB_STATUS_NO_USAGE_DEFINED_FOR_THE_CLOCK,        
        ICC_LIB_STATUS_DATA_NOT_AVAILABLE,                
 
        ICC_LIB_STATUS_UNSPECIFIED_ERROR                 = 0xFFFF
} ICC_LIB_STATUS;

    typedef enum _ICC_CLOCK_USAGE
    {
        ICC_CLOCK_USAGE_BCLK = 0,
        ICC_CLOCK_USAGE_DMI,
        ICC_CLOCK_USAGE_PEG,
        ICC_CLOCK_USAGE_PCIE,
        ICC_CLOCK_USAGE_PCI33,
        ICC_CLOCK_USAGE_RESERVED,
        ICC_CLOCK_USAGE_SATA,
        ICC_CLOCK_USAGE_USB3,
        ICC_CLOCK_USAGE_DISPLAY,
        ICC_CLOCK_USAGE_DISPLAY_NON_SPREAD,
        ICC_CLOCK_USAGE_27MHZ_DOWN_GFX,
        ICC_CLOCK_USAGE_DISPLAY_VGA,
        ICC_CLOCK_USAGE_USB_LEGACY,
        ICC_CLOCK_USAGE_14_31818_MHZ        
    } ICC_CLOCK_USAGE;

// @brief Defines a generic version structure used in the software build process.
typedef struct _ICC_LIB_VERSION
{
    UINT16      Major;
    UINT16      Minor;
    UINT16      Hotfix;
    UINT16      Build;
} ICC_LIB_VERSION;

// ssc mode 
typedef enum _ICC_SSC_MODE
    {
    ICC_SSC_NONE    = 0,    // if used in SET, it means DONT CARE and current percent 
                            // value will be used regardless of percent value
    ICC_SSC_UP      = 1,
    ICC_SSC_CENTER  = 2,
    ICC_SSC_DOWN    = 4
} ICC_SSC_MODE;

typedef UINT32 ICC_CLOCK_FREQUENCY;

// Ratio used to modify the BCLK value and to produce 
// the PEG Slot Frequency (~100MHz).  BCLK / ratio = PEG
// Used in HW where the BCLK, DMI, & PEG use a common clock
typedef enum _ICC_DMI_PEG_RATIO
{    
    ICC_DMI_PEG_RATIO_5_TO_5 = 0, //< 5/5 = 1    - 1:1 ratio used when BCLK at 100MHz
    ICC_DMI_PEG_RATIO_5_TO_4,       //< 5/4 = 1.25 - Used when BCLK around 125MHz
    ICC_DMI_PEG_RATIO_5_TO_3,       //< 5/3 = 1.66 - Used when BCLK around 166MHz
    ICC_DMI_PEG_RATIO_5_TO_2        //< 5/2 = 2.5  - Used when BCLK around 250MHz
} ICC_DMI_PEG_RATIO;

typedef struct _ICC_CLOCK_SETTINGS
{
    ICC_CLOCK_FREQUENCY Frequency;
    ICC_SSC_MODE SscMode;
    UINT8 SscPercent; // encoding example: 1.28% -> SSC_SPREAD value is 128
    ICC_DMI_PEG_RATIO DmiPegRatio;	
} ICC_CLOCK_SETTINGS;

typedef UINT16 ICC_CLOCK_USAGE_MASK;

typedef struct _ICC_CLOCK_RANGES
{
    ICC_CLOCK_FREQUENCY FrequencyMin;
    ICC_CLOCK_FREQUENCY FrequencyMax;
    UINT8 SscChangeAllowed;
    UINT8 SscUpAllowed;
    UINT8 SscCenterAllowed;
    UINT8 SscDownAllowed;
    UINT8 SscPercentMax;
    // that field is actually still a bit mask, but usually applications will never use it.
    ICC_CLOCK_USAGE_MASK UsageMask; 
    UINT8 SscHaltAllowed;
} ICC_CLOCK_RANGES;

typedef UINT8 ICC_CLOCK_ID;

typedef UINT32 ICC_CLOCK_ID_MASK;

typedef enum _ICC_RECORD_INVALIDATION_REASON
{
    ICC_LIB_RIR_RECORD_VALID            = 0,
    ICC_LIB_RIR_AC_POWER_LOSS,
    ICC_LIB_RIR_CMOS_BATTERY_REMOVED,
    ICC_LIB_RIR_PLATFORM_BOOT_TIMEOUT,
    ICC_LIB_RIR_WDT_EXPIRED,
    ICC_LIB_RIR_RESERVED,
    ICC_LIB_RIR_INVALIDATE_BY_REQUEST,
    ICC_LIB_RIR_FW_BUG
} ICC_RECORD_INVALIDATION_REASON;

//
// GUID for the ICC OverClocking Protocol
//
#define ICC_OVERCLOCKING_PROTOCOL_GUID \
  {0x8e8cbc58, 0x834c, 0x41e3, { 0xb8, 0xca, 0xf0, 0x0c, 0xcf, 0x5a, 0x71, 0x7c } }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID    gIccOverClockingProtocolGuid;

typedef VOID (EFIAPI *ICC_LIB_GET_INFO)              (ICC_LIB_VERSION*, UINT8*, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_CLOCK_ID)      (ICC_CLOCK_USAGE, ICC_CLOCK_ID_MASK*, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_CLOCK_RANGES)      (ICC_CLOCK_ID, ICC_CLOCK_RANGES*, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_CURRENT_CLOCK_SETTINGS)    (ICC_CLOCK_ID, ICC_CLOCK_SETTINGS*, ICC_LIB_STATUS*);                         
typedef VOID (EFIAPI *ICC_LIB_SET_CURRENT_CLOCK_SETTINGS)    (ICC_CLOCK_ID, ICC_CLOCK_SETTINGS, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_BOOT_CLOCK_SETTINGS)    (ICC_CLOCK_ID, ICC_CLOCK_SETTINGS*, ICC_LIB_STATUS*);                         
typedef VOID (EFIAPI *ICC_LIB_SET_BOOT_CLOCK_SETTINGS)    (ICC_CLOCK_ID, ICC_CLOCK_SETTINGS, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_DEFAULT_BOOT_SETTINGS)    (ICC_CLOCK_ID, ICC_CLOCK_SETTINGS*, ICC_LIB_STATUS*);                         
typedef VOID (EFIAPI *ICC_LIB_RESET_CLOCK_BOOT_SETTINGS_TO_DEFAULTS)    (ICC_CLOCK_ID, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_RESET_ALL_CLOCKS_BOOT_SETTINGS_TO_DEFAULTS)    (ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_GET_NEXT_FREQUENCY) (ICC_CLOCK_ID, ICC_CLOCK_FREQUENCY, ICC_CLOCK_FREQUENCY*, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_GET_PREVIOUS_FREQUENCY) (ICC_CLOCK_ID, ICC_CLOCK_FREQUENCY, ICC_CLOCK_FREQUENCY*, ICC_LIB_STATUS*);
typedef VOID (EFIAPI *ICC_LIB_GET_BOOT_CLOCK_SETTINGS_INVALIDATION_REASON)          (ICC_RECORD_INVALIDATION_REASON*, ICC_LIB_STATUS*);


typedef struct _ICC_OVERCLOCKING_PROTOCOL {
  ICC_LIB_GET_INFO                                                                     GetInfo;
  ICC_LIB_GET_CLOCK_ID                                                             GetClockId;
  ICC_LIB_GET_CLOCK_RANGES                                                     GetClockRanges;
  ICC_LIB_GET_CURRENT_CLOCK_SETTINGS                                   GetCurrentClockSettings;                         
  ICC_LIB_SET_CURRENT_CLOCK_SETTINGS                                   SetCurrentClockSettings;
  ICC_LIB_GET_BOOT_CLOCK_SETTINGS                                        GetBootClockSettings;                         
  ICC_LIB_SET_BOOT_CLOCK_SETTINGS                                        SetBootClockSettings;
  ICC_LIB_GET_DEFAULT_BOOT_SETTINGS                                      GetDefaultClockSettings; 
  ICC_LIB_RESET_CLOCK_BOOT_SETTINGS_TO_DEFAULTS              ResetClockBootSettingsToDefaults;
  ICC_LIB_RESET_ALL_CLOCKS_BOOT_SETTINGS_TO_DEFAULTS     ResetAllClocksBootSettingsToDefaults;
  ICC_GET_NEXT_FREQUENCY                                                        GetNextFrequency;
  ICC_GET_PREVIOUS_FREQUENCY                                                 GetPreviousFrequency;
  ICC_LIB_GET_BOOT_CLOCK_SETTINGS_INVALIDATION_REASON   GetBootClockSettingsInvalidationReason;
} ICC_OVERCLOCKING_PROTOCOL;

#endif /* _ICC_OVERCLOCKING_H_ */
