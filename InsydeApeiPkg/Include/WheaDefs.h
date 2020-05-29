/** @file

  WHEA driver define headfile.

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

#ifndef _WHEA_DEFS_H_
#define _WHEA_DEFS_H_

//
// Statements that include other files
//
//=================================================
//  MdePkg\Include\
//=================================================
#include <IndustryStandard/Acpi.h>
#include <Guid/Cper.h>

#pragma pack(1)
//
// WHEA GPIO definition, here and whea.asl need to be modify if different GPIO pin 
//
#define WHEA_GPIO                      0

//
// WHEA macro defineitions
//
#define WHEA_GPIO_SCI                  (1 << WHEA_GPIO)
#define WHEA_GPIO_GPE                  (1 << (WHEA_GPIO + 16))
#define WHEA_GPIO_ROUTE0               (1 << (WHEA_GPIO * 2))
#define WHEA_GPIO_ROUTE1               (1 << ((WHEA_GPIO * 2) + 1))

//
// WHEA type definitions
//
//
// Maximum buffer size for WHEA ACPI tables
//
#define MAX_HEST_SIZE               0x8000
#define MAX_BERT_SIZE               0x8000
#define MAX_ERST_SIZE               0x2000
#define MAX_EINJ_SIZE               0x2000

#define MAX_ERROR_STATUS_SIZE       0x4000
#define MAX_ERROR_LOG_RANGE_SIZE    0x2000
#define MAX_BOOT_ERROR_LOG_SIZE     0x8000

// Error Flag defintions
#define FIRMWARE_FIRST                 BIT0
#define GLOBAL_ERROR                   BIT1
#define OS_SETTABLE                    BIT2

// MCE format type 
#define IA32_MCA                       1
#define EM64T_MCA                      2

// HW Error notification methods
#define ERR_NOTIFY_POLLING             0
#define ERR_NOTIFY_EXT_INT             1
#define ERR_NOTIFY_LOCAL_INT           2
#define ERR_NOTIFY_SCI                 3
#define ERR_NOTIFY_NMI                 4

typedef EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE EFI_ACPI_GAS;


// Generic error severity in data entry
#define GENERIC_ERROR_RECOVERABLE      0
#define GENERIC_ERROR_FATAL            1
#define GENERIC_ERROR_CORRECTED        2
#define GENERIC_ERROR_NONE             3
#define GENERIC_ERROR_INFO             3

//Generic error section revision (UEFI 2.1)
#define GENERIC_ERROR_SECTION_REVISION 0x0201
#define GENERIC_ERROR_SECTION_REVISION_UEFI231  0x0203

//
// System Error (Source) types based on type and sevirity
//
#define GEN_ERR_SEV_CPU_GENERIC_RECOVERABLE      0x0000      /// General Processor error servirity types
#define GEN_ERR_SEV_CPU_GENERIC_FATAL            0x0001
#define GEN_ERR_SEV_CPU_GENERIC_CORRECTED        0x0002
#define GEN_ERR_SEV_CPU_GENERIC_INFORMATION      0x0003
#define GEN_ERR_SEV_CPU_SPECIFIC_RECOVERABLE     0x0008      /// Processor specific error servirity types
#define GEN_ERR_SEV_CPU_SPECIFIC_FATAL           0x0009
#define GEN_ERR_SEV_CPU_SPECIFIC_CORRECTED       0x000A
#define GEN_ERR_SEV_CPU_SPECIFIC_INFORMATION     0x000B
#define GEN_ERR_SEV_PLATFORM_MEMORY_RECOVERABLE  0x0010      /// Platform memory error servirity types
#define GEN_ERR_SEV_PLATFORM_MEMORY_FATAL        0x0011
#define GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED    0x0012
#define GEN_ERR_SEV_PLATFORM_MEMORY_INFORMATION  0x0013
#define GEN_ERR_SEV_PCIE_RECOVERABLE             0x0018      /// PCIe (root port, device & bridge) error servirity types
#define GEN_ERR_SEV_PCIE_FATAL                   0x0019
#define GEN_ERR_SEV_PCIE_CORRECTED               0x001A
#define GEN_ERR_SEV_PCIE_INFORMATION             0x001B
#define GEN_ERR_SEV_PCI_BUS_RECOVERABLE          0x0020      /// PCI Bus error servirity types
#define GEN_ERR_SEV_PCI_BUS_FATAL                0x0021
#define GEN_ERR_SEV_PCI_BUS_CORRECTED            0x0022
#define GEN_ERR_SEV_PCI_BUS_INFORMATION          0x0023
#define GEN_ERR_SEV_PCI_DEV_RECOVERABLE          0x0028      /// PCI device error servirity types
#define GEN_ERR_SEV_PCI_DEV_FATAL                0x0029
#define GEN_ERR_SEV_PCI_DEV_CORRECTED            0x002A
#define GEN_ERR_SEV_PCI_DEV_INFORMATION          0x002B

/// Platform Memory error definitions
#define PLATFORM_MEM_ERROR_STATUS_VALID          BIT0
#define PLATFORM_MEM_PHY_ADDRESS_VALID           BIT1
#define PLATFORM_MEM_PHY_ADDRESS_MASK_VALID      BIT2
#define PLATFORM_MEM_NODE_VALID                  BIT3
#define PLATFORM_MEM_CARD_VALID                  BIT4
#define PLATFORM_MEM_MODULE_VALID                BIT5
#define PLATFORM_MEM_BANK_VALID                  BIT6
#define PLATFORM_MEM_DEVICE_VALID                BIT7
#define PLATFORM_MEM_ROW_VALID                   BIT8
#define PLATFORM_MEM_COLUMN_VALID                BIT9
#define PLATFORM_MEM_BIT_POS_VALID               BIT10
#define PLATFORM_MEM_REQUESTOR_ID_VALID          BIT11
#define PLATFORM_MEM_RESPONDER_ID_VALID          BIT12
#define PLATFORM_MEM_TARGET_ID_VALID             BIT13
#define PLATFORM_MEM_ERROR_TYPE_VALID            BIT14
#define PLATFORM_RANK_NUMBER_VALID                  0x000008000	// UEFI 2.3 specific
#define PLATFORM_CARD_HANDLE_VALID                  0x000010000 // UEFI 2.3 specific
#define PLATFORM_Module_HANDLE_VALID                0x000020000 // UEFI 2.3 specific


#define PLATFORM_MEM_ERROR_UNKNOWN               0x00
#define PLATFORM_MEM_ERROR_NONE                  0x01
#define PLATFORM_MEM_ERROR_SINGLEBIT_ECC         0x02
#define PLATFORM_MEM_ERROR_MLTIBIT_ECC           0x03
#define PLATFORM_MEM_ERROR_SINGLESYMBOLS_CHIPKIL 0x04
#define PLATFORM_MEM_ERROR_MULTISYMBOL_CHIPKILL  0x05
#define PLATFORM_MEM_ERROR_MATER_ABORT           0x06
#define PLATFORM_MEM_ERROR_TARGET_ABORT          0x07
#define PLATFORM_MEM_ERROR_PARITY                0x08
#define PLATFORM_MEM_ERROR_WDT                   0x09
#define PLATFORM_MEM_ERROR_INVALID_ADDRESS       0x0A
#define PLATFORM_MEM_ERROR_MIRROR_FAILED         0x0B
#define PLATFORM_MEM_ERROR_SPARING               0x0C
#define PLATFORM_MEM_ERROR_SCRUB_COR                0x0D	// UEFI 2.3 specific
#define PLATFORM_MEM_ERROR_SCRUB_UNCOR              0x0E	// UEFI 2.3 specific
#define PLATFORM_MEM_ERROR_PHY_MEM_MAPOUT           0x0F	// UEFI 2.3 specific

/// PCI/PCIx bus error definitions
#define PCI_PCIX_BUS_ERROR_STATUS_VALID          BIT0
#define PCI_PCIX_BUS_ERROR_TYPE_VALID            BIT1
#define PCI_PCIX_BUS_ERROR_BUS_ID_VALID          BIT2
#define PCI_PCIX_BUS_ERROR_BUS_ADDRESS_VALID     BIT3
#define PCI_PCIX_BUS_ERROR_BUS_DATA_VALID        BIT4
#define PCI_PCIX_BUS_ERROR_COMMAND_VALID         BIT5
#define PCI_PCIX_BUS_ERROR_REQUESTOR_ID_VALID    BIT6
#define PCI_PCIX_BUS_ERROR_COMPLETER_ID_VALID    BIT7
#define PCI_PCIX_BUS_ERROR_TARGET_ID_VALID       BIT8

#define PCI_PCIX_BUS_ERROR_UNKNOWN               0x0000
#define PCI_PCIX_BUS_ERROR_DATA_PARITY           0x0001
#define PCI_PCIX_BUS_ERROR_SYSTEM                0x0002
#define PCI_PCIX_BUS_ERROR_MASTER_ABORT          0x0003
#define PCI_PCIX_BUS_ERROR_BUS_TIMEOUT           0x0004
#define PCI_PCIX_BUS_ERROR_MASTER_DATA_PARITY    0x0005
#define PCI_PCIX_BUS_ERROR_ADDRESS_PARITY        0x0006
#define PCI_PCIX_BUS_ERROR_COMMAND_PARITY        0x0007

/// PCIe error definitions
#define PCIE_ERROR_PORT_TYPE_VALID               BIT0
#define PCIE_ERROR_VERSION_VALID                 BIT1
#define PCIE_ERROR_COMMAND_STATUS_VALID          BIT2
#define PCIE_ERROR_DEVICE_ID_VALID               BIT3
#define PCIE_ERROR_SERIAL_NO_VALID               BIT4
#define PCIE_ERROR_BRIDGE_CRL_STS_VALID          BIT5
#define PCIE_ERROR_CAPABILITY_INFO_VALID         BIT6
#define PCIE_ERROR_AER_INFO_VALID                BIT7

#define PCIE_ERROR_PORT_PCIE_ENDPOINT            0x00000000
#define PCIE_ERROR_PORT_PCI_ENDPOINT             0x00000001
#define PCIE_ERROR_PORT_ROOT_PORT                0x00000002
#define PCIE_ERROR_PORT_UPSWITCH_PORT            0x00000004
#define PCIE_ERROR_PORT_DOWNSWITCH_PORT          0x00000005
#define PCIE_ERROR_PORT_PCIE_TO_PCI_BRIDGE       0x00000006
#define PCIE_ERROR_PORT_PCI_TO_PCIE_BRIDGE       0x00000007
#define PCIE_ERROR_PORT_ROOT_INT_ENDPOINT        0x00000008
#define PCIE_ERROR_PORT_ROOT_EVENT_COLLECTOR     0x00000009

//
// PCI/PCIx Component error definitions
//
#define PCI_PCIX_DEV_ERROR_STATUS_VALID             BIT0
#define PCI_PCIX_DEV_ERROR_ID_INFO_VALID            BIT1
#define PCI_PCIX_DEV_ERROR_MEMORY_NUMBER_VALID      BIT2
#define PCI_PCIX_DEV_ERROR_IO_NUMBER_VALID          BIT3
#define PCI_PCIX_DEV_ERROR_REGISTER_DATA_PAIR_VALID BIT4

//
// Error Injection definition & structures
//
// Error type definition that can be injected
#define INJECT_ERROR_PROCESSOR_CE                BIT0
#define INJECT_ERROR_PROCESSOR_UE_NON_FATAL      BIT1
#define INJECT_ERROR_PROCESSOR_UE_FATAL          BIT2
#define INJECT_ERROR_MEMORY_CE                   BIT3
#define INJECT_ERROR_MEMORY_UE_NON_FATAL         BIT4
#define INJECT_ERROR_MEMORY_UE_FATAL             BIT5
#define INJECT_ERROR_PCIE_CE                     BIT6
#define INJECT_ERROR_PCIE_UE_NON_FATAL           BIT7
#define INJECT_ERROR_PCIE_UE_FATAL               BIT8
#define INJECT_ERROR_PLATFORM_CE                 BIT9
#define INJECT_ERROR_PLATFORM_UE_NON_FATAL       BIT10
#define INJECT_ERROR_PLATFORM_UE_FATAL           BIT11
#define INJECT_ERROR_VENDOR_DEF_TYPE             BIT31

//
// WHEA structure definitions
//
// Error type definition
typedef enum {
  XpfMce        = 0,
  XpfCmc        = 1,
  WheaErrResv1  = 2,
  XpfNmi        = 3,
  WheaErrResv2  = 4,
  WheaErrResv3  = 5,
  PcieRootAer   = 6,
  PcieDeviceAer = 7,
  PcieBridgeAer = 8,
  GenericHw     = 9
} WHEA_ERROR_TYPE;

// HW Error notification structure
typedef struct {
  UINT8                 Type;
  UINT8                 Size;
  UINT16                Pads;
  UINT32                PollInterval;
  UINT32                Vector;
  UINT32                PollingThreshold;
  UINT32                PollingThresholdInterval;
  UINT32                ErrorThreshold;
  UINT32                ErrorThresholdInterval;
} HW_NOTIFY_METHOD;

// Error source structures
typedef struct {
  UINT8                 BankNum;
  UINT8                 IsCleared;
  UINT8                 FormatType;
  UINT8                 Resv1;
  UINT32                ControlMsr;
  UINT64                ControlInit;
  UINT32                StatusMsr;
  UINT32                AddressMsr;
  UINT32                MiscMsr;
} XPF_MC_BANK;

// XPF MCE Error source defintion
typedef struct {
  UINT64                GlobalCapability;
  UINT64                GlobalControl;
    UINT8           NoOfMcBanks;
//
// SMSD_OVERRIDE_START
//
    UINT8           Resv1[7];
    XPF_MC_BANK     McBanks[22];
//
// SMSD_OVERRIDE_END
//
} XPF_MCE_SOURCE;

// XPF CMC Error source defintion
typedef struct {
    HW_NOTIFY_METHOD    NotifyMethod;
    UINT8           NoOfMcBanks;
//
// SMSD_OVERRIDE_START
//
    UINT8           Resv1[3];
    XPF_MC_BANK     McBanks[22]; 
//
// SMSD_OVERRIDE_END
//
} XPF_CMC_SOURCE;

// XPF NMI source definition
typedef struct {
  UINT32                ErrorDataSize;
} XPF_NMI_SOURCE;

//
// PCIe Root port error source structure definition
//
typedef struct {
  UINT32                Bus;
  UINT16                Dev;
  UINT16                Func;
  UINT16                DevControl;
  UINT16                Resv1;
  UINT32                UEMask;
  UINT32                UESeverity;
  UINT32                CEMask;
  UINT32                AerCapControl;
  UINT32                AerRootCmd;  
} PCIE_ROOT_AER_SOURCE;

//
// PCIe Device error source structure definition
//
typedef struct {
  UINT32                Bus;
  UINT16                Dev;
  UINT16                Func;
  UINT16                DevControl;
  UINT16                Resv1;
  UINT32                UEMask;
  UINT32                UESeverity;
  UINT32                CEMask;
  UINT32                AerCapControl;
} PCIE_DEVICE_AER_SOURCE;

//
// PCIe/PCI-x bridge error source structure definition
//
typedef struct {
  UINT32                Bus;
  UINT16                Dev;
  UINT16                Func;
  UINT16                DevControl;
  UINT16                Resv1;
  UINT32                UEMask;
  UINT32                UESeverity;
  UINT32                CEMask;
  UINT32                AerCapControl;
   UINT32               SecUEMask;
   UINT32               SecUESeverity;
  UINT32                SecAerCapControl;
} PCIE_BRIDGE_AER_SOURCE;

//
// Generic HW Error source definition
//
typedef struct {
  UINT16                RelatedSourceId;
  UINT32                MaxErrorDataSize;
  EFI_ACPI_GAS          ErrorStatusAddress;
  HW_NOTIFY_METHOD      NotifyMethod;
  UINT32                ErrorStatusSize;  
} GENERIC_HW_ERROR_SOURCE;

//
// Generic HW error data structure definitions
//

// ErrorStatus block record definitions for various errors
typedef struct {
  UINT32                UeValid:1;
  UINT32                CeValid:1;
  UINT32                MultipleUeValid:1;
  UINT32                MultipleCeValid:1;
  UINT32                NumErrorDataEntry:10;
  UINT32                Resv1:18;
} ERROR_BLOCK_STATUS;

typedef struct {
  UINT8                 FruIdValid:1;
  UINT8                 FruStringValid:1;
  UINT8                 Resv1:6;
} SECTION_DATA_VALID;

typedef struct {
  UINT8                 Primary:1;
  UINT8                 NotContained:1;
  UINT8                 ResetComponent:1;
  UINT8                 ThresholdExeeded:1;
  UINT8                 ResourceNotAvailable:1;
  UINT8                 LatentError:1;
  UINT8                 Resv1:2;
} SECTION_DATA_FLAG;

typedef struct {
    EFI_GUID        SectionType;
    UINT32          Severity;
    UINT16          Revision;
    SECTION_DATA_VALID  SecValidMask;
    SECTION_DATA_FLAG   SectionFlags;
    UINT32          DataSize;
    EFI_GUID        FruId;
    CHAR8           FruString[20];

//    UINT8           *ErrorData; //ErrorData[];    // [DataSize]    
} GENERIC_ERROR_DATA_ENTRY;

// Error Status block structure definition for Generic HW error
typedef struct {
// Error Status block
    ERROR_BLOCK_STATUS  BlockStatus;
    UINT32          RawDataOffset;
    UINT32          RawDataSize;
    UINT32          ErrDataSize;
    UINT32          Severity;

// Error Status data
//    GENERIC_ERROR_DATA_ENTRY
//                    *DataEntry; //DataEntry[];    // [BlockStatus.NumErrorDataEntry]
} GENERIC_ERROR_STATUS;

// Processor genric error data format
typedef struct {
  UINT64                ValidFields;
  UINT8                 Type;
  UINT8                 Isa;
  UINT8                 ErrorType;
  UINT8                 Operation;
  UINT8                 Flags;
  UINT8                 Level;
  UINT16                Resv1;
  UINT64                VersionInfo;
  CHAR8                 BrandString[128];
  UINT64                ApicId;
  UINT64                TargetAddr;
  UINT64                RequestorId;
  UINT64                ResponderId;
  UINT64                InstructionIP;
} PROCESSOR_GENERIC_ERROR_DATA;

// Will defnine processor specific error data format if required for our platform.

//
// Platform Memory error data format
//

//typedef enum {
//// General Internal errors
//    ErrorInternal       = 1,
//    ErrorBus            = 16,
//// Component Internal errors
//    ErrorMemStorage     = 4,        // Error in memory device
//    ErrorTlbStorage     = 5,        // TLB error in cache
//    ErrorCacheStorage   = 6,
//    ErrorFuntionalUnit  = 7,
//    ErrorSelftest       = 8,
//    ErrorOverflow       = 9,
//// Bus internal errors
//    ErrorVirtualMap     = 17,
//    ErrorAccessInvalid  = 18,       // improper access
//    ErrorUnimpAccess    = 19,       // Unimplemented memory access
//    ErrorLossOfLockstep = 20,
//    ErrorResponseInvalid= 21,       // Response not associated with request
//    ErrorParity         = 22,
//    ErrorProtocol       = 23,
//    ErrorPath           = 24,       // detected path error
//    ErrorTimeout        = 25,       // Bus timeout
//    ErrorPoisoned       = 26        // Read data poisoned
//} GEN_ERROR_STATUS_ERR_TYPE;

typedef struct {
  UINT64                Resv1:8;
  UINT64                Type:8;
  UINT64                AddressSignal:1;        /// Error in Address signals or in Address portion of transaction
  UINT64                ControlSignal:1;        /// Error in Control signals or in Control portion of transaction
  UINT64                DataSignal:1;           /// Error in Data signals or in Data portion of transaction
  UINT64                DetectedByResponder:1;  /// Error detected by responder
  UINT64                DetectedByRequester:1;  /// Error detected by requestor
  UINT64                FirstError:1;           /// First Error in the sequence - option field
  UINT64                OverflowNotLogged:1;    /// Additional errors were not logged due to lack of resources
  UINT64                Resv2:41;
} MEM_ERROR_STATUS;

typedef struct {
    UINT64          ValidFields;
    MEM_ERROR_STATUS    ErrorStatus;
    UINT64          PhysicalAddress;      // Error physical address
    UINT64          PhysicalAddressMask;  // Grnaularity
    UINT16          Node;                 // Node #
    UINT16          Card;
    UINT16          ModuleRank;           // Module or Rank#
    UINT16          Bank;
    UINT16          Device;
    UINT16          Row;
    UINT16          Column;
    UINT16          BitPosition;
    UINT64          RequestorId;
    UINT64          ResponderId;
    UINT64          TargetId;
    UINT8           ErrorType;
} PLATFORM_MEMORY_ERROR_DATA;

typedef struct {
    UINT64          ValidFields;
    MEM_ERROR_STATUS    ErrorStatus;
    UINT64          PhysicalAddress;      // Error physical address
    UINT64          PhysicalAddressMask;  // Grnaularity
    UINT16          Node;                 // Node #
    UINT16          Card;
    UINT16          ModuleRank;           // Module or Rank#
    UINT16          Bank;
    UINT16          Device;
    UINT16          Row;
    UINT16          Column;
    UINT16          BitPosition;
    UINT64          RequestorId;
    UINT64          ResponderId;
    UINT64          TargetId;
    UINT8           ErrorType;
    UINT8           Rsvd;
    UINT16          RankNumber;
    UINT16          SmBiosCardHandle;    // Type 16 handle;
    UINT16          SmBiosModuleHandle;	 // Type 17 handle;
} PLATFORM_MEMORY_ERROR_DATA_UEFI_231;

// PCIe error data format & structures
typedef struct {
    UINT16          Resv1:3;
    UINT16          Number:13;
} GEN_ERR_PCI_SLOT;

typedef struct {
    UINT16          VendorId;
    UINT16          DeviceId;
    UINT16          ClassCode;
    UINT8           ProgInterface;
    UINT8           Function;
    UINT8           Device;
    UINT16          Segment;
    UINT8           PrimaryOrDeviceBus;
    UINT8           SecondaryBus;
    GEN_ERR_PCI_SLOT    Slot;
    UINT8           Resv1;
} GEN_ERR_PCIE_DEV_BRIDGE_ID;

typedef struct {
  UINT8                 PcieCap[60];
} PCIE_CAPABILITY;

typedef struct {
  UINT8                 PcieAer[96];
} PCIE_AER;

typedef struct {
  UINT64                     ValidFields;
  UINT32                     PortType;
  UINT32                     Version;
  UINT32                     CommandStatus;
  UINT32                     Resv2;
  GEN_ERR_PCIE_DEV_BRIDGE_ID DevBridge;
  UINT64                     SerialNo;
  UINT32                     BridgeControlStatus;
  PCIE_CAPABILITY            Capability;
  PCIE_AER                   AerInfo;
} PCIE_ERROR_DATA;

// PCI/PCIX BUS error data format & structures
typedef struct {
    UINT64          ValidFields;
    MEM_ERROR_STATUS ErrorStatus;
    UINT16          Type;
    UINT16          BusId;
    UINT32          Resv2;
    UINT64          BusAddress;
    UINT64          BusData;
    UINT64          BusCommand;
    UINT64          RequestorId;
    UINT64          ResponderId;
    UINT64          TargetId;
} PCI_PCIX_BUS_ERROR_DATA;

// PCI/PCIX Device error data format & structures
typedef struct {
  UINT16                VendorId;
  UINT16                DeviceId;
  UINT16                ClassCode;
  UINT8                 Function;
  UINT8                 Device;
  UINT8                 Bus;
  UINT8                 Segment;
  UINT8                 Resv1;
  UINT32                Resv2;
} GEN_ERR_PCI_DEVICE_ID;

typedef struct {
    UINT64          ValidFields;
    MEM_ERROR_STATUS ErrorStatus;
    GEN_ERR_PCI_DEVICE_ID   DeviceId;
    UINT32          MemoryRegisters;
    UINT32          IOResgisters;
    UINT64          *RegisterData; //RegisterData[];     // [2*8*(MemoryRegisters+IORegisters)]
} PCI_PCIX_DEVICE_ERROR_DATA;

typedef struct {
    UINT16      RequesterId;
    UINT16      SegmentNumber;
    UINT8       FaultReason;
    UINT8       AccessType;
    UINT8       AddressType;
    UINT8       ArchitectureType;
    UINT64      DeviceAddress;
    UINT8       Rsvd[16];
} DMAR_GENERAIC_ERROR_DATA;

typedef struct {
    UINT8       Version;
    UINT8       Revision;
    UINT8       OemId[6];
    UINT64      Capability;
    UINT64      ExtendedCapability; 
    UINT32      GlobalCommand;
    UINT32      GlobalStatus;
    UINT32      FaultStatus;   
    UINT8       Rsvd[12];
    UINT64      FaultRecord1;
    UINT64      FaultRecord2;
    UINT64      RootEntry1;
    UINT64      RootEntry2;
    UINT64      ContextEntry1;
    UINT64      ContextEntry2;
    UINT64      Level6PTE;
    UINT64      Level5PTE;
    UINT64      Level4PTE;
    UINT64      Level3PTE;
    UINT64      Level2PTE;
    UINT64      Level1PTE;
} INTEL_VTD_DMAR_ERROR_DATA;
//
// SMSD_OVERRIDE_START
//
//
// IOH error data definition
//
typedef struct {
    UINT64          ValidFields;
    UINT32          PortType;
    UINT32          Version;
    UINT32          CommandStatus;
    UINT32          ErrorSource;
    GEN_ERR_PCIE_DEV_BRIDGE_ID  DevBridge;
    UINT64          SerialNo;
    UINT32          BridgeControlStatus;
    PCIE_CAPABILITY Capability;
    PCIE_AER        AerInfo;
} IOH_ERROR_DATA;
//
// SMSD_OVERRIDE_END
//

// FW Error data - will add it later since it's used for IPF.

#pragma pack()

#endif   
