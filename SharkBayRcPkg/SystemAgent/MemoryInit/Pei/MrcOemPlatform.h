/** @file
  This file contains platform related functions.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _MrcOemPlatform_h_
#define _MrcOemPlatform_h_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcOem.h>
#include <MrcOemIo.h>
#include <MrcOemMmio.h>
#include <MrcWriteDqDqs.h>

#ifdef MRC_MINIBIOS_BUILD

#define GPIO_BASE_ADDRESS (0x800)

#define PLATFORM_MB             (0x04)
#define PLATFORM_SRVER          (0x01)
#define PLATFORM_DT             (0x01)

#define SB_BUS                  (0)
#define SB_PCI2ISA_DEVICE       (31)
#define SB_PCI2ISA_FUNC         (0)
#define SB_PCI2ISA_BUS_DEV_FUNC ((SB_BUS << 8) + ((SB_PCI2ISA_DEVICE << 3) + SB_PCI2ISA_FUNC))
#define PCI_LPC_BASE            (0x80000000 + (SB_PCI2ISA_BUS_DEV_FUNC << 8))

///
/// CPU Mobile SA Device IDs B0:D0:F0
///
#define MRC_SA_DEVICE_ID_MB_0        0x0C04   ///< Haswell Mobile SA DID
#define MRC_SA_DEVICE_ID_MB_1        0x0A04   ///< Haswell Ult Mobile SA DID
#define MRC_SA_DEVICE_ID_MB_2        0x0D04   ///< Crystalwell Mobile SA DID

///
/// CPU Desktop SA Device IDs B0:D0:F0
///
#define MRC_SA_DEVICE_ID_DT_0        0x0C00   ///< Haswell Desktop SA DID
#define MRC_SA_DEVICE_ID_DT_1        0x0D00   ///< Crystalwell Desktop SA DID
#define MRC_SA_DEVICE_ID_DT_2        0x0C0C   ///< Haswell Marketing SpareAffect SA DID

///
/// CPU Server SA Device IDs B0:D0:F0
///
#define MRC_SA_DEVICE_ID_SVR_0       0x0C08   ///< Haswell Server SA DID
#define MRC_SA_DEVICE_ID_SVR_1       0x0D08   ///< Crystalwell Server SA DID

#endif // MRC_MINIBIOS_BUILD
#define MRC_EXIT_VALUE       (0xFF)
#define PCU_CR_PLATFORM_INFO (0xCE)
#define isprint(a)           (((a) >= ' ') && ((a) <= '~') ? (a) : 0)

//[-start-130314-IB06720210-add]//
typedef enum {
  MEMORY_ABSENT,    ///< No memory down and no physical memory slot.
  MEMORY_SLOT_ONLY, ///< No memory down and a physical memory slot.
  MEMORY_DOWN_ONLY, ///< Memory down and not a physical memory slot.
} MemorySlotStatus;
//[-end-130314-IB06720210-add]//

typedef enum {
  RefRateLowTempOOS,  // Not safe
  RefRateFourth,      // 4x tREFI - Not safe
  RefRateHalf,        // 2x tREFI
  RefRate1x,          // tREFI
  RefRate2x,          // 1/2 tREFI
  RefRate4x,          // 1/4 tREFI
  RefRate4xDeRateAc,  // 1/4 tREFI de-rate AC timing - Not safe
  RefRateHighTempOOS  // Not safe
} LpddrRefreshRates;

#pragma pack (push, 1)
typedef union {
  struct {
    U32                         : 8;
    U32 MAX_NON_TURBO_LIM_RATIO : 8;
    U32                         : 16;
    U32                         : 32;
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_PLATFORM_INFO_STRUCT;

typedef union {
  struct {
    U32 Enable_2x_Refresh                       :  1;  // Bits 0:0
    U32 LPDDR_Min_MR4                           :  3;  // Bits 1:3
    U32                                         :  27; // Bits 4:30
    U32 Lock_Bit                                :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MrcMailbox2xRefresh;
#pragma pack (pop)
/**
  This function directs pCode to force 2x Refresh through the mailbox.

  @param[in]  MrcData - Pointer to the MRC global data structure

  @retval - Nothing.
**/
void
MrcOemEnable2xRefresh (
  IN MrcParameters *const MrcData
  );

/**
  This function changes the DIMM Voltage to the closest desired voltage without
  going higher. Default wait time is the minimum value of 200us, if more time
  is needed before deassertion of DIMM Reset#, then change the parameter.

  @param[in, out] MrcData            - The MRC "global data" area.
  @param[in]      VddVoltage         - Selects the DDR voltage to use, in mV.
  @param[in, out] VddSettleWaitTime  - Time needed for Vdd to settle after the update

  @retval TRUE if a voltage change occurred, otherwise FALSE.
**/
extern
BOOL
MrcOemVDDVoltageCheckAndSwitch (
  IN OUT MrcParameters      *MrcData,
  IN     const MrcVddSelect VddVoltage,
  IN OUT U32 * const        VddSettleWaitTime
  );

/**
@brief
  Gets CPU ratio - P-State ratio. for get the real time we need to multiply it in B CLK.

  @param[in] Nothing

  @retval U32 - PERF STATUS.
**/
extern
U32
MrcGetCpuRatio (
  void
  );

/**
@brief
  Gets CPU current time - rdtsc value.
  return the result in millisec.

  @param[in] Nothing

  @retval U64 - rdtsc value.
**/
extern
U64
MrcGetCpuTime (
  void
  );

/**
@brief
  Sets CpuModel and CpuStepping in MrcData based on CpuModelStep.

  @param[out] MrcData     - The Mrc Host data structure
  @param[in]  CpuModel    - The CPU Family Model.
  @param[in]  CpuStepping - The CPU Stepping.

  @retval - mrcSuccess if the model and stepping is found.  Otherwise mrcFail
**/
MrcStatus
MrcSetCpuInformation (
  OUT MrcParameters  *MrcData,
  IN  MrcCpuModel    CpuModel,
  IN  MrcCpuStepping CpuStepping
  );

/**
@brief
  Gets CPU's random number generator.
  return the GeneratedSeed result.

  @param[in] Nothing

  @retval U32 -           GeneratedSeed value.
**/
extern
U32
MrcGetRandomNumber (
  void
  );

/**
@brief
  Hook before normal mode is enabled.

  @param[in, out] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
extern
void
MrcOemBeforeNormalModeTestMenu (
  IN OUT MrcParameters *const MrcData
  );

/**
@brief
  Hook after normal mode is enabled

  @param[in] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
extern
void
MrcOemAfterNormalModeTestMenu (
  IN MrcParameters *MrcData
  );

/**
@brief
  Overwrite Thermal settings

  @param[in] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
extern
void
MrcThermalOverwrites (
  IN MrcParameters *MrcData
  );


/**
@brief
  this function use by the OEM to do dedicated task during the MRC.

  @param[in] MrcData           - include all the MRC data
  @param[in] OemStatusCommand  - A command that indicates the task to perform.
  @param[in] ptr               - general ptr for general use.

  @retval The status of the task.
**/
extern
MrcStatus
MrcOemCheckPoint (
  IN MrcParameters        *MrcData,
  IN MRC_OemStatusCommand OemStatusCommand,
  IN void                 *ptr
  );

/**
@brief
  This function display on port 80 number.
  It can be different debug interface.
  This function can be use for any debug ability according to OEM requirements.

  @param[in] MrcData            - Mrc Global Data
  @param[in] DisplayDebugNumber - the number to display on port 80.

  @retval Nothing
**/
extern
void
MrcOemDebugHook (
  IN MrcParameters *MrcData,
  IN U16           DisplayDebugNumber
  );

#ifdef MRC_DEBUG_PRINT
/**
@brief
  Print the input parameters to the debug message output port.

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess
**/
extern
MrcStatus
MrcPrintInputParameters (
  MrcParameters * const MrcData
  );

/**
@brief
  Print the specified memory to the serial message debug port.

  @param[in] Debug - Serial message debug structure.
  @param[in] Start - The starting address to dump.
  @param[in] Size  - The amount of data in bytes to dump.

  @retval Nothing.
**/
extern
void
MrcPrintMemory (
  IN const MrcDebug *const Debug,
  IN const U8 *const       Start,
  IN const U32              Size
  );
#endif

#if (defined MEMORY_DOWN_SUPPORT && (MEMORY_DOWN_SUPPORT > 0))
/**
@brief
  Enables Memory Down support and sets SPD data for all DIMMs needing support.

  @param[in, out] Inputs - MRC Input data structure.
  @param[in]      BoardId - The ID of the board.

  @retval Nothing
**/
extern
void
EnableMemoryDown (
  IN OUT MrcInput *const Inputs,
  IN     U16             BoardId
  );
#endif  // MEMORY_DOWN_SUPPORT

/**
@brief
  Initialize the board-specific CMD/CTL/CLK and DQ/DQS mapping for LPDDR3.

  @param[in, out] Inputs  - MRC Input data structure.
  @param[in]      BoardId - The ID of the board.

  @retval none
**/
void
MrcOemLpddrBoardMapping (
  IN OUT  MrcInput *Inputs,
  IN      U16      BoardId
  );

#endif // _MrcOemPlatform_h_
