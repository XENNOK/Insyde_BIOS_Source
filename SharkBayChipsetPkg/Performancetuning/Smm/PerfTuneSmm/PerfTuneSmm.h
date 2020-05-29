#ifndef _PERTUNE_H_
#define _PERTUNE_H_


#include <ChipsetSetupConfig.h>
#include <Framework/SmmCis.h>
#include <ChipsetCmos.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <Library/IoLib.h>
#include <Library/CmosLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <Guid/DebugMask.h>
#include <Protocol/PerfTune.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmSwDispatch.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/SmmCpuSaveState.h>
#include <Protocol/XtuPolicy/XtuPolicy.h>

VOID
ReadBiosSetting (
  );

VOID
WriteBiosSetting (
  );

VOID
WriteBiosVariableData(
  BIOS_SETTINGS_DATA_STRUCT *Data,
  BOOLEAN                   WriteSysVar
  );

VOID
EnableWatchdogAfterPost(
  );

VOID
PerfTuneSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT   *DispatchContext
  );

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

VOID
SetCmosRatio(
  UINT8 Data
  );

VOID
ClearOsBootStatusFlag(
  );

VOID
SmmPerfTuneReadyToBootCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT   *DispatchContext
  );


#endif
