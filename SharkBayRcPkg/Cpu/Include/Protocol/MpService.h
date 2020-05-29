/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MpService.h

@brief
  This protocol produces MP Protocol
**/
#ifndef _MP_H_
#define _MP_H_
//[-start-120605-IB10820064-modify]//
//#include "LinkedList.h"
//#include EFI_PROTOCOL_CONSUMER (CpuIo)
//[-end-120605-IB10820064-modify]//
#define EFI_MP_SERVICES_PROTOCOL_GUID \
  { \
    0xf33261e7, 0x23cb, 0x11d5, 0xbd, 0x5c, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
  }
//[-start-120605-IB10820064-modify]//
typedef struct _EFI_MP_SERVICES_PROTOCOL EFI_MP_SERVICES_PROTOCOL;
//[-end-120605-IB10820064-modify]//
#ifndef DELIVERY_MODE_FIXED
#define DELIVERY_MODE_FIXED           0x0
#define DELIVERY_MODE_LOWEST_PRIORITY 0x1
#define DELIVERY_MODE_SMI             0x2
#define DELIVERY_MODE_REMOTE_READ     0x3
#define DELIVERY_MODE_NMI             0x4
#define DELIVERY_MODE_INIT            0x5
#define DELIVERY_MODE_SIPI            0x6
#define DELIVERY_MODE_MAX             0x7

#define TRIGGER_MODE_EDGE             0x0
#define TRIGGER_MODE_LEVEL            0x1
#endif

typedef union {
  struct {
    UINT32 Status                   : 2;
    UINT32 Tested                   : 1;
    UINT32 Reserved1                : 13;
    UINT32 VirtualMemoryUnavailable : 1;
    UINT32 Ia32ExecutionUnavailable : 1;
    UINT32 FloatingPointUnavailable : 1;
    UINT32 MiscFeaturesUnavailable  : 1;
    UINT32 Reserved2                : 12;
  } Bits;
  UINT32 Uint32;
} EFI_MP_HEALTH_FLAGS;

#define EFI_MP_HEALTH_FLAGS_STATUS_HEALTHY                  0x0
#define EFI_MP_HEALTH_FLAGS_STATUS_PERFORMANCE_RESTRICTED   0x1
#define EFI_MP_HEALTH_FLAGS_STATUS_FUNCTIONALLY_RESTRICTED  0x2

typedef struct {
  EFI_MP_HEALTH_FLAGS Flags;
  UINT32              TestStatus;
} EFI_MP_HEALTH;

typedef enum {
  EfiCpuAP                = 0,
  EfiCpuBSP,
  EfiCpuDesignationMaximum
} EFI_CPU_DESIGNATION;

typedef struct {
  UINT32 Package;
  UINT32 Die;
  UINT32 Core;
  UINT32 Thread;
} PHYSICAL_LOCATION;

typedef struct {
  UINT32              ApicID;
  BOOLEAN             Enabled;
  EFI_CPU_DESIGNATION Designation;
  EFI_MP_HEALTH       Health;
  UINTN               PackageNumber;
  UINTN               NumberOfCores;
  UINTN               NumberOfThreads;
  UINT64              ProcessorPALCompatibilityFlags;
  UINT64              ProcessorTestMask;
} EFI_MP_PROC_CONTEXT;

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_GET_GENERAL_MP_INFO)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  OUT UINTN                   *NumberOfCPUs,
  OUT UINTN                   *MaximumNumberOfCPUs,
  OUT UINTN                   *NumberOfEnabledCPUs,
  OUT UINTN                   *RendezvousIntNumber,
  OUT UINTN                   *RendezvousProcLength
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_GET_PROCESSOR_CONTEXT)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                    ProcessorNumber,
  IN OUT UINTN                *BufferLength,
  IN OUT EFI_MP_PROC_CONTEXT  *ProcessorContextBuffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_STARTUP_ALL_APS)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN BOOLEAN                  SingleThread,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN TimeoutInMicroSecs OPTIONAL,
  IN OUT VOID *ProcArguments  OPTIONAL,
  OUT UINTN *FailedCPUList    OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_STARTUP_THIS_AP)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN UINTN                    ProcessorNumber,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN TimeoutInMicroSecs OPTIONAL,
  IN OUT VOID *ProcArguments  OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_SWITCH_BSP)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                    ProcessorNumber,
  IN BOOLEAN                  EnableOldBSP
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_SEND_IPI)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                    ProcessorNumber,
  IN UINTN                    VectorNumber,
  IN UINTN                    DeliveryMode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_ENABLEDISABLEAP)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                    ProcessorNumber,
  IN BOOLEAN                  NewAPState,
  IN EFI_MP_HEALTH            *HealthState
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MP_SERVICES_WHOAMI)(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  OUT UINTN                   *ProcessorNumber
  );

///
/// This information is basically from Intel Platform Innovation Framework for EFI Multiprocessor Services Protocol
/// Specification. When installed, the MP Services Protocol produces a collection of services that are needed for MP
/// management, such as initialization and management of application processors.
///
struct _EFI_MP_SERVICES_PROTOCOL {
  ///
  /// This service retrieves general information of multiprocessors in the system.
  ///
  EFI_MP_SERVICES_GET_GENERAL_MP_INFO   GetGeneralMPInfo;
  ///
  /// This service gets detailed MP-related information of the requested processor.
  ///
  EFI_MP_SERVICES_GET_PROCESSOR_CONTEXT GetProcessorContext;
  ///
  /// This function is used to dispatch all enabled APs to the function specified by Procedure.
  ///
  EFI_MP_SERVICES_STARTUP_ALL_APS       StartupAllAPs;
  ///
  /// This function is used to dispatch one enabled AP to the function provided by the caller.
  ///
  EFI_MP_SERVICES_STARTUP_THIS_AP       StartupThisAP;
  ///
  /// This service switches the requested AP to be the BSP from that point onward.
  ///
  EFI_MP_SERVICES_SWITCH_BSP            SwitchBSP;
  ///
  /// This service sends an IPI to a specified AP.
  ///
  EFI_MP_SERVICES_SEND_IPI              SendIPI;
  ///
  /// This service lets the caller enable or disable an AP.
  ///
  EFI_MP_SERVICES_ENABLEDISABLEAP       EnableDisableAP;
  ///
  /// This service lets the caller processor get its handle number, with which any processor in the system can be
  /// uniquely identified.
  ///
  EFI_MP_SERVICES_WHOAMI                WhoAmI;
};

extern EFI_GUID gEfiMpServiceProtocolGuid;

#endif
