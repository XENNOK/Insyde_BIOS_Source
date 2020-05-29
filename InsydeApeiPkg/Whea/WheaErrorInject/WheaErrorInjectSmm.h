/** @file

  Definitions for WheaErrorInject driver.

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

#ifndef _WHEA_CORE_EINJECT_SMM_H_
#define _WHEA_CORE_EINJECT_SMM_H_

//=================================================
//  MdePkg\Include\
//=================================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <WheaAcpi.h>
#include <Library/WheaLib.h>
#include <Protocol/WheaSupport.h>
#include <Protocol/WheaPlatform.h>

//
// Data & structure definition
//
//
// WHEA error injection action definitions
//
#define WHEA_EINJ_BEGIN_INJECT_OP                               0x00
#define WHEA_EINJ_GET_TRIGGER_ACTION_TABLE                      0x01
#define WHEA_EINJ_SET_ERROR_TYPE                                0x02
#define WHEA_EINJ_GET_ERROR_TYPE                                0x03
#define WHEA_EINJ_END_INJECT_OP                                 0x04
#define WHEA_EINJ_EXECUTE_INJECT_OP                             0x05
#define WHEA_EINJ_CHECK_BUSY_STATUS                             0x06
#define WHEA_EINJ_GET_CMD_STATUS                                0x07
#define WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRES                    0x08
#define WHEA_EINJ_TRIGGER_ERROR_ACTION                          0xFF

//
// Error injection operation definitions
//
#define EINJ_BEGIN_OPERATION                                    0x55AA55AA
#define EINJ_END_OPERATION                                      0x00000000

//
// Error injection operation status
//
#define WHEA_EINJ_CMD_SUCCESS                                   0x00
#define WHEA_EINJ_CMD_UNKNOWN_FAILURE                           0x01
#define WHEA_EINJ_CMD_INVALID_ACCESS                            0x02

//
// Port for SMI
//
#define SMI_CMD_IO_PORT                                         0xB2

//
// Error injection SW SMI defintions
//
#define EFI_WHEA_EINJ_EXECUTE_SWSMI                             0x9A
#define EFI_WHEA_EINJ_TRIGGER_SWSMI                             0x9B

//
// Vendor Error Type Extension
// Now, it is dummy data.
//
#define EFI_WHEA_EINJ_VENDOR_ERROR_VENDOR_ID                    0x5656
#define EFI_WHEA_EINJ_VENDOR_ERROR_DEVICE_ID                    0x4444
#define EFI_WHEA_EINJ_VENDOR_ERROR_REV_ID                       0x52

#pragma pack (1)

#define EinjRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  16                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}

  #define EinjRegisterFiller  { \
    EFI_ACPI_3_0_SYSTEM_MEMORY, \
    64,                         \
    0,                          \
    EFI_ACPI_3_0_QWORD,         \
    0}

typedef struct {
  UINT32                HeaderSize;
  UINT32                Revision;
  UINT32                TableSize;
  UINT32                EntryCount;
} WHEA_ERROR_TRIGGER_ACTION_HEADER;

typedef struct {
  WHEA_ERROR_TRIGGER_ACTION_HEADER          Header;
  APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION  Trigger0;
  APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION  Trigger1;
  APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION  Trigger2;
  APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION  Trigger3;
} WHEA_EINJ_TRIGGER_ACTION_TABLE;

typedef union {
  struct {
    UINT32                ProcessorValid:1;
    UINT32                MemoryAddressValid:1;
    UINT32                PcieSBDFValid:1;
    UINT32                Resvd:29;
  } Bits;
  UINT32 Data;
} WHEA_ERROR_VENDOR_DEFINED_TYPE_FLAGS;

typedef union _WHEA_EINT_VENDOR_DEFINED_TYPE_PCIE_SBDF {
  struct {
    UINT32                Resrved:8;
    UINT32                FuncNum:3;
    UINT32                DeviceNum:5;
    UINT32                BusNum:8;
    UINT32                PcieSegment:8;
  } Fields;
  UINT32 Data;
} WHEA_EINT_VENDOR_DEFINED_TYPE_PCIE_SBDF;

typedef struct {
  UINT32                ErrorTpye;
  UINT32                VendorErrorTypeExtenOffset;
  WHEA_ERROR_VENDOR_DEFINED_TYPE_FLAGS Flags;
  UINT32                ApicId;                     //optional,phaysical APIC ID or X2APIC ID.              
  UINT64                MemoryAddress;              //optional,phaysical address of the memory which is the target for the injection.
  UINT64                MemoryAddressRange;         //optional,range mask for the adress field.
  WHEA_EINT_VENDOR_DEFINED_TYPE_PCIE_SBDF PcieSBDF; //Byte 0:reserced; Byte 1:[7:3]Device Number, [2:0]Function number; Byte 2 Bus Number; Byte 3 PCIe Segment;
} WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS;

typedef struct {
  UINT32                Length;
  UINT32                SBDF;
  UINT16                VendorId;
  UINT16                DeviceId;
  UINT8                 RevId;
  UINT8                 Resvd[3];
  UINT64                OemDef;
} WHEA_EINJ_VENDOR_EXT;

typedef struct {
  UINT64                OpState;            // opeartion state, begin/end/etc
  UINT64                ErrorToInject;
  UINT64                TriggerActionTable;
  UINT64                ErrorInjectCap;
  UINT64                OpStatus;
  UINT64                CmdStatus;
  WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS SetErrorAddress;
  WHEA_EINJ_VENDOR_EXT  VendorExt;
} WHEA_EINJ_PARAM_BUFFER;

#pragma pack ()

#if 0  // Internal Uesd
STATIC
EFI_STATUS
InsydeWheaUpdateEinject (
  VOID
);
#endif

#if 0  // Internal Uesd
/**
  Simulate the error injection flow

  @param [in]      DispatchHandle      The handle of this callback, obtained when registering.
  @param [in]      DispatchContext     The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer          Common Buffer.
  @param [in, out] CommBufferSize      Common Buffer Size.

  @retval EFI_SUCCESS             Error Injection Log successfully.

**/
STATIC
EFI_STATUS
InsydeWheaEinjCreateElog (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
);
#endif

#if 0  // Internal Uesd
/**
  According the inject error type to trigger NMI or SCI

  @param [in]      DispatchHandle      The handle of this callback, obtained when registering.
  @param [in]      DispatchContext     The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer          Common Buffer.
  @param [in, out] CommBufferSize      Common Buffer Size.
  
  @retval EFI_SUCCESS             Error Injection trigger NMI or SCI successfully.

**/
STATIC
EFI_STATUS
InsydeWheaEinjTriggerNmiSci (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
);
#endif

#endif  
