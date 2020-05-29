/** @file

  Memory Initialization PEIM.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MEMORY_INIT_H_
#define _MEMORY_INIT_H_

#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/SaPlatformPolicy.h>
#include <MemInfoHob.h>
///
/// These header files are from MRC
///
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcGlobal.h>
#include <MrcGeneral.h>
#include <MrcStartMemoryConfiguration.h>

#include <Guid/SaDataHob.h>

#ifdef SSA_FLAG
#include "SsaCallbackPeim.h"
#endif // SSA_FLAG


///
/// SMRAM range definitions
///
#define MC_ABSEG_HSEG_PHYSICAL_START  0x000A0000
#define MC_ABSEG_HSEG_LENGTH          0x00020000
#define MC_ABSEG_CPU_START            0x000A0000
#define MC_HSEG_CPU_START             0xFEDA0000

///
/// See NonDistributed\ReferenceCode\Txt\BiosAcm\Txt.h
///
#define TXT_PUBLIC_BASE 0xFED30000

///
/// Maximum number of memory ranges supported by the memory controller
///
#define MAX_RANGES  (SA_MC_MAX_ROWS + 8)
#define MEM_EQU_4GB 0x100000000ULL

///
/// TPM Status and Time-out
///
#define TPM_STATUS_REG_ADDRESS  0xfed40000
#define TPM_TIME_OUT            750

#define PLATFORM_ID_MOBILE      1

//
//  Boot mode
//
#define BOOT_IN_RECOVERY_MODE_MASK                    0x40
#define BOOT_SPECIAL_MASK                             0x80


///
/// Memory range types
///
typedef enum {
  DualChannelDdrMainMemory,
  DualChannelDdrSmramCacheable,
  DualChannelDdrSmramNonCacheable,
  DualChannelDdrGraphicsMemoryCacheable,
  DualChannelDdrGraphicsMemoryNonCacheable,
  DualChannelDdrReservedMemory,
  DualChannelDdrMaxMemoryRangeType
} PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE;

///
/// Memory map range information
///
#pragma pack(push, 1)
typedef struct {
  UINT8                                   RowNumber;
  EFI_PHYSICAL_ADDRESS                    PhysicalAddress;
  EFI_PHYSICAL_ADDRESS                    CpuAddress;
  EFI_PHYSICAL_ADDRESS                    RangeLength;
  PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE  Type;
} PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE;
#pragma pack(pop)

/**
@brief
  Main starting point for system memory initialization.
    1. Get SysBootMode and MrcBootMode
    2. Locate SaPlatformPolicy PPI
    3. Locate S3DataPtr from SaPlatformPolicy.
    4. SaveDataValid := TRUE if S3DataPtr is not NULL.
    5. If SysBootMode is BOOT_ON_S3_RESUME and S3Data is not valid:
    -> ASSERT.
    6. If MrcBootMode is Warm boot, but S3 data is not valid :
    -> change MrcBootMode to Cold boot.
    7. If MrcBootMode is Cold boot:
    -> Run MRC code
    -> Save S3 Restore Data
    Else
    -> Run MRC_S3Resume
    8. Run MRC_Done().
    9. Install EFI memory HOBs.

  @param[in] FfsHeader        - Not used.
  @param[in] PeiServices      - General purpose services available to every PEIM.

  @retval EFI_SUCCESS      - Memory initialization completed successfully.
  @retval EFI_NOT_READY    - Cannot locate SA Platform Policy.
  @retval EFI_NOT_FOUND    - No S3 data in S3 Boot Mode.
  @retval EFI_DEVICE_ERROR - MemoryInit failed or IOSAV Memory test failed.
**/
extern
EFI_STATUS
PeimMemoryInit (
  IN EFI_PEI_FILE_HANDLE       FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );


/**
@brief
  This function installs memory for all paths except S3 resume.

  @param[in] Inputs      - MRC input structure.
  @param[in] PeiServices - PEI Services table.
  @param[in] SysBootMode - The specific boot path that is being followed.

  @retval EFI_SUCCESS          The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES Out of Resources.
**/
extern
EFI_STATUS
InstallEfiMemory (
  IN const MrcInput *const Inputs,
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_BOOT_MODE         SysBootMode
  );

/**
@brief
  This function installs memory for the S3 resume path.

  @param[in] Inputs           - Mrc input data structure
  @param[in] PeiServices      - PEI services table.
  @param[in] VariableServices - Pointer to EFI Variable PPI

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES - Out of Resources.
**/
extern
EFI_STATUS
InstallS3Memory (
  IN const MrcInput *const          Inputs,
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_READ_ONLY_VARIABLE_PPI *VariableServices
);

/**
@brief
  Determine the memory size desired based on HOB memory information.

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] Size        - The memory size to return.

  @retval Nothing.
**/
extern
void
RetrieveRequiredMemorySize (
  IN  EFI_PEI_SERVICES **PeiServices,
  OUT UINTN            *Size
  );

/**
@brief
  Determine the Total DPR memory size needed based on the DPR directory in the SA Data HOB.

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] Size        - The memory size in MB to return.

  @retval Nothing.
**/
extern
void
CalculateTotalDprMemorySize (
  IN     EFI_PEI_SERVICES  **PeiServices,
  IN OUT UINT8             *Size
  );

/**
@brief
  Calculates the bases for each technology consuming the DPR region
  and updates the SA Data HOB with the appropriate values in the Dpr
  directory

  @param[in] PeiServices    - PEI Services table.
  @param[in] Base           - The memory base to return.
  @param[in] TotalDprSizeMB - The total DPR size in MB

  @retval Nothing.
**/
extern
VOID
UpdateDprHobInfo (
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PHYSICAL_ADDRESS  Base,
  IN UINT8                 TotalDprSizeMB
  );

/**
@brief
  Determine the memory size desired by GDXC

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] MotSize     - The MOT memory size
  @param[in, out] GdxcSize    - The GDXC memory size

  @retval Nothing.
**/
extern
void
RetrieveGdxcMemorySize (
  IN  EFI_PEI_SERVICES **PeiServices,
  OUT UINT64            *MotSize,
  OUT UINT64            *GdxcSize
  );

/**
@brief
  This function returns the memory ranges to be enabled, along with information
  describing how the range should be used. The MemoryMap buffer will be filled in and
  NumRanges will contain the actual number of memory ranges that are to be enabled.

  @param[in]      PeiServices - PEI Services Table.
  @param[in, out] MemoryMap   - Buffer to record details of the memory ranges to be enabled.
  @param[in, out] NumRanges   - On input, this contains the maximum number of memory ranges that
                                can be described in the MemoryMap buffer.

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL - The specified number of ranges is too large.
**/
extern
EFI_STATUS
GetMemoryMap (
  IN     EFI_PEI_SERVICES                      **PeiServices,
  IN OUT PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *MemoryMap,
  IN OUT UINT8                                 *NumRanges
  );

/**
@brief
  This function returns a pointer to the allocated hand off buffer.

  @param[in]      PeiServices - A pointer to the EFI PEI services table
  @param[in, out] Hob         - A pointer to where to store the pointer to the allocated data buffer.
  @param[in]      Size        - The size of the buffer to get.

  @retval EFI_SUCCESS - Hob is successfully built.
  @retval Others      - Error occured while creating the Hob.
**/
extern
EFI_STATUS
MrcGetHobForDataStorage (
  IN     EFI_PEI_SERVICES       **PeiServices,
  IN OUT HOB_SAVE_MEMORY_DATA   **Hob,
  IN     UINT16                 BlockSize
  );

/**
@brief
  A small memory test to quickly point out severe memory issues.

  @param[in] Inputs - Pointer to the MRC Input data structure

  @retval mrcFail on failure, otherwise mrcSuccess.
**/
extern
MrcStatus
BasicMemoryTest (
  IN const MrcInput *const Inputs
  );

#ifndef TXT_SUPPORT
/**
@brief
  Determines whether or not the platform has executed a TXT launch by
  examining the TPM Establishment bit.

  @param[in] PeiServices - General purpose services available to every PEIM.

  @retval TRUE        - If the TPM establishment bit is asserted.
  @retval FALSE       - If the TPM establishment bit is unasserted.
**/
extern
BOOLEAN
IsEstablishmentBitAsserted (
  IN EFI_PEI_SERVICES **PeiServices
  );

/**
@brief
  Unlock memory when security is set and TxT is not enabled.

  @param[in] MrcData     - Mrc global data.
  @param[in] PeiServices - PEI Services Table.

  @retval Nothing
**/
extern
void
UnlockMemory (
  IN const MrcParameters *const MrcData,
  IN EFI_PEI_SERVICES           **PeiServices
  );
#endif /// TXT_SUPPORT
/**
@brief
  Determine whether a cold reset of the platform is required.
  Note that the memory configuration saved data must be valid.

  @param[in] MrcData             - The MRC "global data" area.
  @param[in] SaPlatformPolicyPpi - SA Platform Policy structure.

  @retval TRUE if cold reset is required, otherwise returns FALSE.
**/
extern
BOOLEAN
ColdBootRequired (
  IN const MrcParameters *const MrcData,
  IN SA_PLATFORM_POLICY_PPI     *SaPlatformPolicyPpi
  );

/**
@brief
  Set up the MRC input data structure.

  @param[in]  SysBootMode         - Boot mode of the system.
  @param[in]  BootMode            - Boot mode of the Mrc.
  @param[out] Inputs              - Pointer to the Mrc Input data structure.
  @param[in]  PeiServices         - PEI Services Table.
  @param[in]  SaPlatformPolicyPpi - SA Platform Policy structure.

  @retval Always returns mrcSuccess.
**/
extern
MrcBootMode
MrcSetupMrcData (
  IN  const EFI_BOOT_MODE           SysBootMode,
  IN  const MrcBootMode             BootMode,
  OUT MrcInput *const               Inputs,
  IN  EFI_PEI_SERVICES **const      PeiServices,
  IN  SA_PLATFORM_POLICY_PPI *const SaPlatformPolicyPpi
  );

/**
@brief
  Check to see if user defined profile is selected and if it is, then copy the
  timing settings for this profile to the timing override structure. If user
  defined profile is not selected, then set the timing override structure to 0.

  Note that even though we set timings on a DIMM by DIMM basis, the controller
  may force DIMM timings to be the same for all DIMMs in a channel.

  @param[in, out] Inputs              - The MRC Input data structure.
  @param[in]      SaPlatformPolicyPpi - The Peim to Peim interface of SaPlatformPolicy.

  @retval Nothing.
**/
extern
void
CheckForTimingOverride (
  IN OUT MrcInput *const               Inputs,
  IN     SA_PLATFORM_POLICY_PPI *const SaPlatformPolicyPpi
  );


#endif
