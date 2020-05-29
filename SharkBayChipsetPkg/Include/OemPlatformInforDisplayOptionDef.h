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

#ifndef _OEM_PLATFORM_INFO_DISPLAY_OPTION_DEF_H_
#define _OEM_PLATFORM_INFO_DISPLAY_OPTION_DEF_H_
//[-start-120419-IB10820038-modify]//
#include <Uefi.h>
//[-end-120419-IB10820038-modify]//
//[-start-120711-IB10820079-removed]//
//#include "UefiIfrLibrary.h"
//[-end-120711-IB10820079-removed]//

#define DISPLAY_DISABLE      0
#define DISPLAY_ENABLE       1
#define DISPLAY_INFO_END     2

//
// Function Option
//
#define GET_VBIOS_VERSION_OPTION        DISPLAY_ENABLE , GetVbiosVersion
#define GET_EC_VERSION_OPTION           DISPLAY_ENABLE , GetEcVersion
#define GET_PCH_REVERSION_OPTION        DISPLAY_ENABLE , GetPchReversionId
//[-start-121026-IB10370026-remove]//
//#define GET_SA_REVERSION_OPTION         DISPLAY_ENABLE , GetSaReversionId
//[-end-121026-IB10370026-remove]//
#define GET_ME_VERSION_OPTION           DISPLAY_ENABLE , GetMeVersion
#define GET_CPU_MISC_INFOR_OPTION       DISPLAY_ENABLE , GetCpuMiscInfo
//[-start-130422-IB05400398-remove]//
////[-start-130220-IB10930022-add]//
//#define GET_BOARD_INFOR_OPTION          DISPLAY_ENABLE , GetDesktopBoardInfo
////[-end-130220-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

//[-start-120525-IB10540007-add]//
//
// Get SA Information Option
//
//[-start-121026-IB10370026-remove]//
//#define GET_SA_REVERSION_ID_OPTION      DISPLAY_ENABLE , GetSaReversionIdFunc
//[-end-121026-IB10370026-remove]//
#define GET_IGD_FREQUENCY_OPTION        DISPLAY_ENABLE , GetIgdFreqFunc
//[-end-120525-IB10540007-add]//

//
// Get CPU Informaiton Option
//
//[-start-120528-IB10540007-add]//
#define GET_CPU_SPEED_OPTION            DISPLAY_ENABLE , GetCpuSpeedFunc
#define GET_CACHE_INFO_OPTION           DISPLAY_ENABLE , GetCacheInfo
#define GET_L1_DATA_CACHE_OPTION        DISPLAY_ENABLE , GetL1DataCache
#define GET_L1_INSTRUCTION_CACHE_OPTION DISPLAY_ENABLE , GetL1Insruction
#define GET_L2_CACHE_OPTION             DISPLAY_ENABLE , GetL2Cache
#define GET_L3_CACHE_OPTION             DISPLAY_ENABLE , GetL3Cache
//[-end-120528-IB10540007-add]//
#define GET_CPU_ID_OPTION               DISPLAY_ENABLE , GetCpuIdFunc
//[-start-121022-IB10540014-modify]//
//[-start-120620-IB10540011-add]//
#define GET_CORE_NUMBER_OPTION          DISPLAY_ENABLE , GetCoreNumFunc
//[-end-120620-IB10540011-add]//
//[-end-121022-IB10540014-modify]//
//[-start-121022-IB10540014-add]//
#define GET_THREAD_NUMBER_OPTION        DISPLAY_ENABLE , GetThreadNumFunc
//[-end-121022-IB10540014-add]//
//[-start-120620-IB10540011-remove]//
//#define GET_CORE_NUMBER_OPTION          DISPLAY_ENABLE , GetCoreNumFunc
//#define GET_ALL_THREAD_PER_CORE_OPTION  DISPLAY_ENABLE , GetAllThreadFunc
//[-end-120620-IB10540011-remove]//
//[-start-121024-IB10370026-add]//
#define GET_CPU_STEPPING_OPTION         DISPLAY_ENABLE , GetCpuSteppingFunc
//[-end-121024-IB10370026-add]//
#define GET_MICROCODE_VERSION_OPTION    DISPLAY_ENABLE , GetMicrocodeVersion
#define GET_TXT_CAPABILITY_OPTION       DISPLAY_ENABLE , GetTxtCapability
#define GET_VTD_CAPABILITY_OPTION       DISPLAY_ENABLE , GetVtdCapability
#define GET_VTX_CAPABILITY_OPTION       DISPLAY_ENABLE , GetVtxCapability

//[-start-120620-IB10540011-add]//
//
// Get EC Information Option
//
#define GET_EC_VERSION_ID_OPTION        DISPLAY_ENABLE , GetEcVerIdFunc
//[-start-130422-IB05400398-modify]//
//[-start-130221-IB10930022-modify]//
#define GET_BOARD_ID_OPTION             DISPLAY_ENABLE , GetBoardFunc
//[-end-130221-IB10930022-modify]//
//[-end-130422-IB05400398-modify]//
#define GET_FAB_ID_OPTION               DISPLAY_ENABLE , GetFabFunc
//[-end-120620-IB10540011-add]//

//
// Platform Information Display Option Table
//
//[-start-130422-IB05400398-modify]//
//[-start-121026-IB10370026-modify]//
//[-start-130220-IB10930022-modify]//
#define PLATFORM_INFO_DISPLAY_OPTION_TABLE_LIST     {GET_CPU_MISC_INFOR_OPTION}, \
                                                    {GET_PCH_REVERSION_OPTION}, \
                                                    {GET_VBIOS_VERSION_OPTION}, \
                                                    {GET_EC_VERSION_OPTION}, \
                                                    {GET_ME_VERSION_OPTION}, \
                                                    {DISPLAY_INFO_END, NULL}
//[-end-130220-IB10930022-modify]//
//[-end-121026-IB10370026-modify]//
//[-end-130422-IB05400398-modify]//
//[-start-120525-IB10540007-add]//                                                    

//
// SA Information Display Option Table
//
//[-start-121026-IB10370026-modify]//
#define SA_INFO_DISPLAY_TABLE_LIST                  {GET_IGD_FREQUENCY_OPTION}, \
                                                    {DISPLAY_INFO_END, NULL} 
//[-end-121026-IB10370026-modify]//
//[-end-120525-IB10540007-add]//
//[-start-120601-IB10540008-add]//
//
// EC Information Display Option Table
//                                                  
#define EC_INFO_DISPLAY_TABLE_LIST                  {GET_EC_VERSION_ID_OPTION}, \
                                                    {GET_BOARD_ID_OPTION}, \
                                                    {GET_FAB_ID_OPTION}, \
                                                    {DISPLAY_INFO_END, NULL}
//[-end-120620-IB10540011-add]//
//[-start-121022-IB10540014-modify]//                                                 
//[-start-120620-IB10540011-modify]//
//[-start-120528-IB10540007-modify]//                                                    
//
// CPU Information Display Option Table
//
//[-start-121025-IB10370026-modify]//
#define CPU_INFO_DISPLAY_TABLE_LIST                 {GET_CPU_ID_OPTION}, \
                                                    {GET_CPU_SPEED_OPTION}, \
                                                    {GET_CPU_STEPPING_OPTION}, \
                                                    {GET_CACHE_INFO_OPTION}, \
                                                    {GET_L1_DATA_CACHE_OPTION}, \
                                                    {GET_L1_INSTRUCTION_CACHE_OPTION}, \
                                                    {GET_L2_CACHE_OPTION}, \
                                                    {GET_L3_CACHE_OPTION}, \
                                                    {GET_CORE_NUMBER_OPTION}, \
                                                    {GET_THREAD_NUMBER_OPTION}, \
                                                    {GET_MICROCODE_VERSION_OPTION}, \
                                                    {DISPLAY_INFO_END, NULL}
//[-end-121025-IB10370026-modify]//
//[-end-120528-IB10540007-modify]//
//[-end-120620-IB10540011-modify]//
//[-end-121022-IB10540014-modify]//

#define CPU_CAPABILITY_DISPLAY_TABLE_LIST           {GET_TXT_CAPABILITY_OPTION}, \
                                                    {GET_VTD_CAPABILITY_OPTION}, \
                                                    {GET_VTX_CAPABILITY_OPTION}, \
                                                    {DISPLAY_INFO_END, NULL}

//[-start-130422-IB05400398-remove]//
////[-start-130220-IB10930022-add]//
//#define BOARD_DISPLAY_TABLE_LIST                    {GET_BOARD_INFOR_OPTION}, \
//                                                    {DISPLAY_INFO_END, NULL}                                                    
////[-end-130220-IB10930022-add]//
//[-end-130422-IB05400398-remove]//

#endif
