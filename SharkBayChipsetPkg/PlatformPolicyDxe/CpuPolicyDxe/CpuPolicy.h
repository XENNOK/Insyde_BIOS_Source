//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   CpuPolicy.h
//;

#ifndef _CPU_POLICY_DXE_H_
#define _CPU_POLICY_DXE_H_
typedef UINT16  STRING_REF;

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/GenericMemoryTest.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/HiiString.h>
#include <Protocol/NonCSMSupport.h>
#include <Protocol/CpuPlatformPolicy.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <SmiTable.h>
#include <ChipsetSmiTable.h>
#include <CpuAccess.h>
#include <PowerMgmtDefinitions.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>
//[-start-121114-IB11410022-add]//
#include <Protocol/SmmAccess.h>
#include <Guid/SmramCpuDataHeader.h>
//[-end-121114-IB11410022-add]//

//[-start-121003-IB10820132-modify]//
extern EFI_GUID gAcpiVariableSetGuid;
//[-end-121003-IB10820132-modify]//

#define ACPI_GLOBAL_VARIABLE  L"AcpiGlobalVariable"

#define MP_CPU_EXCHANGE_INFO_OFFSET   (0x1000 - 0x400)
#define MAXIMUM_CPU_NUMBER            0x40

typedef enum {
  WakeUpApCounterInit  = 0,
  WakeUpApPerHltLoop   = 1,
  WakeUpApPerMwaitLoop = 2,
  WakeUpApPerRunLoop   = 3
} WAKEUP_AP_MANNER;

//[-start-120413-IB06460389-modify]//
typedef struct {
  //
  // Acpi Related variables
  //
  EFI_PHYSICAL_ADDRESS    AcpiReservedMemoryBase;
  UINT32                  AcpiReservedMemorySize;
  EFI_PHYSICAL_ADDRESS    S3ReservedLowMemoryBase;
  EFI_PHYSICAL_ADDRESS    AcpiBootScriptTable;
  EFI_PHYSICAL_ADDRESS    RuntimeScriptTableBase;
  EFI_PHYSICAL_ADDRESS    AcpiFacsTable;
  UINT64                  SystemMemoryLengthBelow4GB;
  UINT64                  SystemMemoryLengthAbove4GB;
  ACPI_CPU_DATA           AcpiCpuData;
  //
  // VGA OPROM to support Video Re-POST for Linux S3
  //
  EFI_PHYSICAL_ADDRESS    VideoOpromAddress;
  UINT32                  VideoOpromSize;
  EFI_PHYSICAL_ADDRESS    MsrTableAddress; 
  BOOLEAN                 ResetRequired; 
  UINT32                  NumofThreading;
  UINT32                  CountThreading;
} ACPI_VARIABLE_SET;
//[-end-120413-IB06460389-modify]//


typedef struct {
  UINT32                 Number;
  UINT32                 BIST;
} BIST_INFO;

typedef struct {
  UINTN                  Lock;
  VOID                   *StackStart;
  UINTN                  StackSize;
  VOID                   *ApFunction;
  PSEUDO_DESCRIPTOR      GdtrProfile;
  PSEUDO_DESCRIPTOR      IdtrProfile;
  UINT32                 BufferStart;
  UINT32                 Cr3;
  UINT32                 InitFlag;
  WAKEUP_AP_MANNER       WakeUpApManner;
  BIST_INFO              BistBuffer[MAXIMUM_CPU_NUMBER];
} MP_CPU_EXCHANGE_INFO;
//[-start-121003-IB10820132-remove]//
//#define PLATFORM_CPU_MAX_CORE_FREQUENCY        4000
//#define PLATFORM_CPU_MAX_FSB_FREQUENCY         1066
//[-end-121003-IB10820132-remove]//

VOID
EFIAPI
DumpCPUPolicy (
  IN      DXE_CPU_PLATFORM_POLICY_PROTOCOL        *CPUPlatformPolicy
  );

//[-start-120924-IB03610461-add]//
VOID
InvalidateSmramCpuData (
  VOID
  );
//[-end-120924-IB03610461-add]//

EFI_STATUS
InitializeWakeUpBuffer (
  );

EFI_STATUS
InitialAcpiS3Variable (
  );

VOID
EFIAPI
MpCpuReAllocateEventCallback (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  );

EFI_STATUS
NullFunctionForApInit (
  );

#endif
