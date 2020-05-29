/** @file
  Definition for Oem Services Default Lib.

;******************************************************************************
;* Copyright (c) 2012- 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_OEM_SVC_KERNEL_LIB_H_
#define _SMM_OEM_SVC_KERNEL_LIB_H_

#include <Uefi.h>
#include <DmiStringInformation.h>

#define PCIE_ASPM_DEV_END_OF_TABLE  0xFFFF

#pragma pack (1)
//
// Device List Structure
//
typedef struct _PCIE_ASPM_DEV_INFO {
  UINT16    VendorId;           // offset 0
  UINT16    DeviceId;           // offset 2
  UINT8     RevId;              // offset 8; 0xFF means all steppings
  UINT8     RootAspm;
  UINT8     EndpointAspm;
} PCIE_ASPM_DEV_INFO;

#pragma pack ()


//
// OemSvc function prototypes
//
EFI_STATUS
OemSvcDisableAcpiCallback (
  VOID
  );

EFI_STATUS
OemSvcEnableAcpiCallback (
  VOID
  );

EFI_STATUS
OemSvcGetAspmOverrideInfo (
  OUT PCIE_ASPM_DEV_INFO                    **PcieAspmDevs
  );

EFI_STATUS
OemSvcSmmInstallPnpGpnvTable (
  OUT UINTN                                 *UpdateableGpnvCount,
  OUT OEM_GPNV_MAP                          **GetOemGPNVMap
  );

EFI_STATUS
OemSvcSmmInstallPnpStringTable (
  OUT UINTN                                 *UpdateableStringCount,
  OUT DMI_UPDATABLE_STRING                  **mUpdatableStrings
  );

EFI_STATUS
OemSvcPowerButtonCallback (
  VOID
  );

EFI_STATUS
OemSvcRestoreAcpiCallback (
  VOID
  );

EFI_STATUS
OemSvcS1Callback (
  VOID
  );

EFI_STATUS
OemSvcS3Callback (
  VOID
  );

EFI_STATUS
OemSvcS4Callback (
  VOID
  );

EFI_STATUS
OemSvcS5AcLossCallback (
  VOID
  );

EFI_STATUS
OemSvcS5Callback (
  VOID
  );

EFI_STATUS
OemSvcOsResetCallback (
  VOID
  );

EFI_STATUS
OemSvcVariablePreservedTable (
  IN OUT PRESERVED_VARIABLE_TABLE              **VariablePreservedTable,
  IN OUT BOOLEAN                               *IsKeepVariableInList
  );

EFI_STATUS
OemSvcIhisiGetWholeBiosRomMap (
  OUT VOID                                     **BiosRomMap,
  OUT UINTN                                    *NumberOfRegions
  );

#endif
