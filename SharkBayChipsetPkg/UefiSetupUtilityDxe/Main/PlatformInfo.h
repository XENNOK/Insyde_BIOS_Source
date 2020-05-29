/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_PLATFORM_INFO_H_
#define _SETUP_PLATFORM_INFO_H_

#include <Uefi.h>
#include <SetupUtility.h>
#include <SaAccess.h>
#include <CpuRegs.h>
#include <CpuDataStruct.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
#include <OemPlatformInforDisplayOptionDef.h>
#include <OemDataHubSubClassProcessor.h>
#include <Protocol/DataHub.h>

#include <Protocol/MeBiosPayloadData.h>

//[-start-121025-IB10370026-modify]//
//[-start-120404-IB05300309-modify]//
#define HSW_STEP_A0                    1
#define HSW_STEP_B0                    2
#define HSW_STEP_C0                    3
#define HSW_STEP_D0                    4

#define HSW_ULT_STEP_B0                0
#define HSW_ULT_STEP_C0                1
//[-end-120404-IB05300309-modify]//
//[-end-121025-IB10370026-modify]//

#define SA_BUS_NUMBER                  0x0
#define SA_DEVICE_NUMBER               0x0
#define SA_FUNCTION_NUMBER             0x0

#define LEGACY_BIOS_INT_10             0x10

#define VBE_OEM_EXTENSION_SERVICES     0x4F14
#define GET_VBIOS_VERSION_SUBFUNCTION  0x0087

//[-start-120524-IB10540007-add]//
#define MAXIMUN_EFFICIENCY_RATIO       100
//[-end-120524-IB10540007-add]//
//[-start-120620-IB10540011-add]//
#define CRB_BOARD_ID_1                 0x01
#define CRB_BOARD_ID_FAB_ID_FAB2       0x01
#define CRB_BOARD_ID_FAB_ID_FAB3       0x02
#define CRB_BOARD_ID_FAB_ID_FAB4       0x03
#define FAB2_ID                        0x02
#define FAB3_ID                        0x03
#define FAB4_ID                        0x04
//[-end-120620-IB10540011-add]//

//[-start-130422-IB05400398-remove]//
////[-start-130220-IB10930022-add]//
////
////  DT board detection
////       Bit3    Bit2    Bit1   Bit0    Board SKU
////        0       0       0      1      Flathead Creek EV
////        0       0       1      0
////        0       0       1      1      Flathead Creek PPV
////        0       1       0      0      DT reserved CPV
////        0       1       0      1      PI
////        0       1       1      0
////        0       1       1      1      Flathead Creek CRB
////        1       0       0      0      Haddock Creek (AIO)
////        1       0       1      0      Flathead Creek DOE 1
////        1       0       1      1      Flathead Creek DOE 2
////        1       1       0      0      Haddock Creek2  
//// --------------------------------------------------------
////
//#define FlatheadCreekEV     0x01
//#define FlatheadCreekPpv    0x03
//#define DtReservedCpv       0x04
//#define Pi                  0x05
//#define FlatheadCreekCrb    0x07
//#define HaddockCreek        0x08
//#define CrbOc               0x09
//#define FlatheadCreekDoe1   0x0A
//#define FlatheadCreekDoe2   0x0B
//#define HaddockCreek2       0x0C
//
//#define B_ID_BIT3_GPIO                      17
//#define B_ID_BIT2_GPIO                      34
//#define B_ID_BIT1_GPIO                      1
//#define B_ID_BIT0_GPIO                      6
////[-end-130220-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

EFI_STATUS
typedef
( *DISPLAY_PLATFORM_INFO_FUNCTION ) (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
typedef
( *DISPLAY_CPU_INFO_FUNCTION ) (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
typedef
( *DISPLAY_CPU_CAPABILITY_FUNCTION ) (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  );

//[-start-120531-IB10540007-add]//
EFI_STATUS
typedef
( *DISPLAY_SA_INFO_FUNCTION ) (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );
//[-end-120531-IB10540007-add]//

//[-start-120620-IB10540011-add]//
EFI_STATUS
typedef
( *DISPLAY_EC_INFO_FUNCTION ) (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );
//[-end-120620-IB10540011-add]//

//[-start-130422-IB05400398-remove]//
////[-start-130220-IB10930022-add]//
//EFI_STATUS
//typedef
//( EFIAPI *DISPLAY_BOARD_INFO_FUNCTION ) (
//  IN	VOID					   *OpCodeHandle,
//  IN     EFI_HII_HANDLE            MainHiiHandle,
//  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN     CHAR16                    *StringBuffer
//  );
////[-end-130220-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

typedef struct {
  UINTN                            Option;
  DISPLAY_PLATFORM_INFO_FUNCTION   DisplayPlatformInfoFunction;
} PLATFORM_INFO_DISPLAY_TABLE;

typedef struct {
  UINTN                            Option;
  DISPLAY_CPU_INFO_FUNCTION        DisplayCpuInfoFunction;
} CPU_INFO_DISPLAY_TABLE;

//[-start-120525-IB10540007-add]//
typedef struct {
  UINTN                            Option;
  DISPLAY_SA_INFO_FUNCTION         DisplaySaFunction;
} SA_INFO_DISPLAY_TABLE;
//[-end-120525-IB10540007-add]//

//[-start-120620-IB10540011-add]//
typedef struct {
  UINTN                            Option;
  DISPLAY_EC_INFO_FUNCTION         DisplayEcFunction;
} EC_INFO_DISPLAY_TABLE;
//[-end-120620-IB10540011-add]//

//[-start-130422-IB05400398-remove]//
////[-start-130220-IB10930022-add]//
//typedef struct {
//  UINTN                            Option;
//  DISPLAY_BOARD_INFO_FUNCTION     DisplayBoardFunction;
//} BOARD_INFO_DISPLAY_TABLE;
////[-end-130220-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

//[-start-120529-IB10540007-add]//
typedef struct {
  UINTN                            Level;
  UINTN                            Type;
  UINTN                            CacheSize;
} CACHE_INFO_SIZE_TABLE;
//[-end-120529-IB10540007-add]//

typedef struct {
  UINTN                            Option;
  DISPLAY_CPU_CAPABILITY_FUNCTION  DisplayCpuCapabilityFunction;
} CPU_CAPABILITY_DISPLAY_TABLE;

typedef struct {
  UINT8                            ReversionValue;
  CHAR16                           *SteppingString;
} STEPPING_STRING_DEFINITION;

//[-start-120619-IB06150226-modify]//
typedef struct {
  UINT16                            ReversionValue;
  CHAR16                           *SteppingString;
} SKU_STRING_DEFINITION;
//[-end-120619-IB06150226-modify]//

//
// Function Definition
//
EFI_STATUS
PlatformInfoInit (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle

  );

EFI_STATUS
GetVbiosVersion (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
GetEcVersion (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
GetPchReversionId (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
GetSaReversionId (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

//[-start-120620-IB10540011-add]//
EFI_STATUS
GetEcVerIdFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
GetBoardFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

//[-start-130422-IB05400398-remove]//
////[-start-130221-IB10930022-add]//
//EFI_STATUS
//GetDesktopBoardInfo (
//  IN	   VOID 					          *OpCodeHandle,
//  IN     EFI_HII_HANDLE            MainHiiHandle,
//  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN     CHAR16                    *StringBuffer
//);
////[-end-130221-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

EFI_STATUS
GetFabFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );
//[-end-120620-IB10540011-add]//
//[-start-120525-IB10540007-add]//
EFI_STATUS
GetSaReversionIdFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

EFI_STATUS
GetIgdFreqFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );
//[-end-120525-IB10540007-add]//

EFI_STATUS
GetMeVersion (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

//[-start-130107-IB10310017-add]//
EFI_STATUS
GetSpsMeVersion (
  IN VOID                         *OpCodeHandle,
  IN EFI_HII_HANDLE               MainHiiHandle,
  IN EFI_HII_HANDLE               AdvanceHiiHandle,
  IN CHAR16                       *StringBuffer
  );
//[-end-130107-IB10310017-add]//

EFI_STATUS
GetCpuMiscInfo (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  );

//[-start-120528-IB10540007-add]//
EFI_STATUS
GetCpuSpeedFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
GetCacheInfo (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
GetL1DataCache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
GetL1Insruction (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
GetL2Cache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );

EFI_STATUS
GetL3Cache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );
//[-end-120528-IB10540007-add]//

EFI_STATUS
GetCpuIdFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );
  
//[-start-121024-IB10370026-add]//
EFI_STATUS
GetCpuSteppingFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );
//[-end-121024-IB10370026-add]//

//[-start-121022-IB10540014-modify]//
//[-start-120620-IB10540011-add]//
EFI_STATUS
GetCoreNumFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );
//[-end-120620-IB10540011-add]//
//[-end-121022-IB10540014-modify]//

//[-start-121022-IB10540014-add]//
EFI_STATUS
GetThreadNumFunc (
  IN    VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  );
//[-end-121022-IB10540014-add]//

//[-start-120620-IB10540011-remove]//
//EFI_STATUS
//GetCoreNumFunc (
//  IN     EFI_HII_HANDLE            MainHiiHandle,
//  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN     CHAR16                    *StringBuffer,
//  IN     EFI_SUBCLASS_TYPE1_HEADER *DataHeader,
//  IN OUT EFI_HII_UPDATE_DATA       *UpdateData
//  );
//
//EFI_STATUS
//GetAllThreadFunc (
//  IN     EFI_HII_HANDLE            MainHiiHandle,
//  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN     CHAR16                    *StringBuffer,
//  IN     EFI_SUBCLASS_TYPE1_HEADER *DataHeader,
//  IN OUT EFI_HII_UPDATE_DATA       *UpdateData
//  );
//[-end-120620-IB10540011-remove]//

EFI_STATUS
GetMicrocodeVersion (
  IN     VOID                       *OpCodeHandle,
  IN     EFI_HII_HANDLE             MainHiiHandle,
  IN     EFI_HII_HANDLE             AdvanceHiiHandle,
  IN     CHAR16                     *StringBuffer,
  IN     EFI_SUBCLASS_TYPE1_HEADER  *DataHeader
  );

EFI_STATUS
GetTxtCapability (
  IN     VOID                       *OpCodeHandle,
  IN     EFI_HII_HANDLE             MainHiiHandle,
  IN     EFI_HII_HANDLE             AdvanceHiiHandle,
  IN     CHAR16                     *StringBuffer,
  IN     EFI_CPUID_REGISTER         CpuIdRegisters
  );

EFI_STATUS
GetVtdCapability (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  );

EFI_STATUS
GetVtxCapability (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  );

//UINT8 *
//EFIAPI
//HiiCreateSuppressIfOpCode (
//  IN VOID       *OpCodeHandle
//  );

//UINT8 *
//EFIAPI
//HiiCreateFalseOpCode (
//  IN VOID       *OpCodeHandle
//  );

//[-start-120911-IB06460449-add]//
EFI_STATUS
NewStringToHandle (
  IN  EFI_HII_HANDLE           SrcHiiHandle,
  IN  EFI_STRING_ID            SrcStringId,
  IN  EFI_HII_HANDLE           DstHiiHandle,
  OUT EFI_STRING_ID            *DstStringId
  );
//[-end-120911-IB06460449-add]//

#endif
