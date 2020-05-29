/** @file
  Useful definitions for PnpSmm driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PNP_SMM_H_
#define _PNP_SMM_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Guid/HobList.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/SmBios.h>


#include <Protocol/SmmBase2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmFwBlockService.h>
#include <SmiTable.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>

#define GET_ERASE_SECTOR_NUM(Base, Len, EraseBase, SectorSize) \
                            (((Base) + (Len) - (EraseBase) + (SectorSize) - 1) / (SectorSize)) 

#define FLASH_BLOCK_SIZE                0x10000

#define SMBIOS_SIGNATURE                SIGNATURE_32 ('_', 'S', 'M', '_')
#define GPNV_SIGNATURE                  SIGNATURE_32 ('G', 'P', 'N', 'V')

#define DMI_SUCCESS                     0x00    // Function Completed Successfully
#define DMI_UNKNOWN_FUNCTION            0x81    // Unknown, or invalid, function number passed
#define DMI_FUNCTION_NOT_SUPPORTED      0x82    // The function is not supported on this system
#define DMI_INVALID_HANDLE              0x83    // SMBIOS Structure number/handle passed is invalid or out of
                                                // range.
#define DMI_BAD_PARAMETER               0x84    // The function detected invalid parameter or, in the case of a "Set
                                                // SMBIOS Structure" request, detected an invalid value for a
                                                // to-bechanged structure field.
#define DMI_INVALID_SUBFUNCTION         0x85    // The SubFunction parameter supplied on a SMBIOS Control
                                                // function is not supported by the system BIOS.
#define DMI_NO_CHANGE                   0x86    // There are no changed SMBIOS structures pending notification.
#define DMI_ADD_STRUCTURE_FAILED        0x87    // Returned when there was insufficient storage space to add the
                                                // desired structure.
#define DMI_READ_ONLY                   0x8D    // A "Set SMBIOS Structure" request failed because one or more of
                                                // the to-be-changed structure fields are read-only.
#define DMI_LOCK_NOT_SUPPORTED          0x90    // The GPNV functions do not support locking for the specified
                                                // GPNV handle.
#define DMI_CURRENTLY_LOCKED            0x91    // The GPNV lock request failed - the GPNV is already locked.
#define DMI_INVALID_LOCK                0x92    // The caller has failed to present the predefined GPNVLock value
                                                // which is expected by the BIOS for access of the GPNV area.
//
// PnP return codes
//
#define PNP_SUCCESS                                 0x00   // Function completed successfully

// Warning Codes 01h through 7Fh
#define PNP_NOT_SET_STATICALLY                      0x7F   // Warning that indicates a device could not be configured
                                                           // statically, but was successfully configured dynamically.
                                                           // This return code is used only when function 02h is
                                                           // requested to set a device both statically and dynamically.

// Error Codes 81h through FFh
#define PNP_UNKNOWN_FUNCTION                        0x81   // Unknown, or invalid, function number passed
#define PNP_FUNCTION_NOT_SUPPORTED                  0x82   // The function is not supported on this system.
#define PNP_INVALID_HANDLE                          0x83   // Device node number/handle passed is invalid or out of range.
#define PNP_BAD_PARAMETER                           0x84   // Function detected invalid resource descriptors or resource
                                                           // descriptors were specified out of order.
#define PNP_SET_FAILED                              0x85   // Set Device Node function failed.
#define PNP_EVENTS_NOT_PENDING                      0x86   // There are no events pending.
#define PNP_SYSTEM_NOT_DOCKED                       0x87   // The system is currently not docked.
#define PNP_NO_ISA_PNP_CARDS                        0x88   // Indicates that no ISA Plug and Play cards are installed in the system.
#define PNP_UNABLE_TO_DETERMINE_DOCK_CAPABILITIES   0x89   // Indicates that the system was not able to determine the
                                                           // capabilities of the docking station.
#define PNP_CONFIG_CHANGE_FAILED_NO_BATTERY         0x8A   // The system failed the undocking sequence because it
                                                           // detected that the system unit did not have a battery.
#define PNP_CONFIG_CHANGE_FAILED_RESOURCE_CONFLICT  0x8B   // The system failed to successfully dock because it
                                                           // detected a resource conflict with one of the primary
                                                           // boot devices; such as Input, Output, or the IPL device.
#define PNP_BUFFER_TOO_SMALL                        0x8C   // The memory buffer passed in by the caller was not
                                                           // large enough to hold the data to be returned by the system BIOS.
#define PNP_USE_ESCD_SUPPORT                        0x8D   // This return code is used by functions 09h and 0Ah to instruct
                                                           // the caller that reporting resources explicitly assigned to
                                                           // devices in the system to the system BIOS must be handled
                                                           // through the interfaces defined by the ESCD Specification.
#define PNP_MESSAGE_NOT_SUPPORTED                   0x8E   // This return code indicates the message passed to the system
                                                           // BIOS through function 04h, Send Message, is not supported on the system.
#define PNP_HARDWARE_ERROR                          0x8F   // This return code indicates that the system BIOS detected a
                                                           // hardware failure.


typedef enum {
  DMI_BYTE_CHANGE,
  DMI_WORD_CHANGE,
  DMI_DWORD_CHANGE,
  DMI_STRING_CHANGE = 0x05,
  DMI_BLOCK_CHANGE

} DMI_0X52_SERVICES__SUPPORT;

#pragma pack (1)
typedef struct _PNP_FAR_PTR {
  UINT16                                Offset;
  UINT16                                Segment;
} PNP_FAR_PTR;

typedef struct {
  UINT16                                PushedBp;
  PNP_FAR_PTR                           ReturnAddress;
  INT16                                 Function;
} PNP_GENERIC_ENTRY_FRAME;

typedef struct {
  UINT16                                Handle;
  UINTN                                 MinGPNVSize;
  UINTN                                 GPNVSize;
  UINTN                                 BaseAddress;
} PLATFORM_GPNV_MAP;

typedef struct {
  PLATFORM_GPNV_MAP                 PlatFormGPNVMap[FixedPcdGet16 (PcdDefaultGpnvMapBufferSize)];
} PLATFORM_GPNV_MAP_BUFFER;


//
// PnP SMBIOS function 0x50, Get SMBIOS Information
//
// short FAR (*entryPoint)(
// short Function,                     /* PnP BIOS Function 50h */
// unsigned char FAR *dmiBIOSRevision, /* Revision of the SMBIOS Extensions */
// unsigned short FAR *NumStructures,  /* Max. Number of Structures the BIOS will */
//                                     /* return */
// unsigned short FAR *StructureSize,  /* Size of largest SMBIOS Structure */
// unsigned long FAR *dmiStorageBase,  /* 32-bit physical base address for memory- */
//                                     /* mapped SMBIOS data */
// unsigned short FAR *dmiStorageSize, /* Size of the memory-mapped SMBIOS data */
// unsigned short BiosSelector );      /* PnP BIOS readable/writable selector */
//
typedef struct _PNP_FUNCTION_0x50_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           DmiBIOSRevision;
  PNP_FAR_PTR                           NumStructures;
  PNP_FAR_PTR                           StructureSize;
  PNP_FAR_PTR                           DmiStorageBase;
  PNP_FAR_PTR                           DmiStorageSize;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x50_FRAME;


//
// PnP SMBIOS function 0x51, Get SMBIOS Structure
//
// short FAR (*entryPoint)(
// short Function,                    /* PnP BIOS Function 51h */
// unsigned short FAR *Structure,     /* Structure number/handle to retrieve*/
// unsigned char FAR *dmiStrucBuffer, /* Pointer to buffer to copy structure data */
// unsigned short dmiSelector,        /* SMBIOS data read/write selector */
// unsigned short BiosSelector );     /* PnP BIOS readable/writable selector */
//
typedef struct _PNP_FUNCTION_0x51_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           Structure;
  PNP_FAR_PTR                           DmiStrucBuffer;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x51_FRAME;

//
// Function 52h ¡V Set SMBIOS Structure
//
// short FAR (*entryPoint)(
// short          Function,             /* PnP BIOS Function 52h */
// unsigned char  FAR *dmiDataBuffer,   /* Pointer to buffer with new/change data */
// unsigned char  FAR *dmiWorkBuffer,   /* Pointer to work buffer area for the BIOS */
// unsigned char  Control,              /* Conditions for performing operation */
// unsigned short dmiSelector,          /* SMBIOS data read/write selector */
// unsigned short BiosSelector );       /* PnP BIOS readable/writeable selector */
//
typedef struct _PNP_FUNCTION_0x52_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           DmiDataBuffer;
  PNP_FAR_PTR                           DmiWorkBuffer;
  UINT8                                 Control;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x52_FRAME;

typedef struct _FUNC_0x52_DATA_BUFFER {
  UINT8                                 Command;
  UINT8                                 FieldOffset;
  UINT32                                ChangeMask;
  UINT32                                ChangeValue;
  UINT16                                DataLength;
  SMBIOS_STRUCTURE                      StructureHeader;
  UINT8                                 StructureData[1]; // Variable length field
} FUNC_0x52_DATA_BUFFER;

//
// Function 54h ¡V SMBIOS Control
//
// short FAR (*entryPoint)(
// short          Function,             /* PnP BIOS Function 54h */
// short          SubFunction,          /* Defines the specific control operation */
// void           FAR *Data,            /* Input/output data buffer, SubFunction specific */
// unsigned char  Control,              /* Conditions for setting the structure */
// unsigned short dmiSelector,          /* SMBIOS data read/write selector */
// unsigned short BiosSelector );       /* PnP BIOS readable/writeable selector */
//
typedef struct _PNP_FUNCTION_0x54_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  INT16                                 SubFunction;
  PNP_FAR_PTR                           Data;
  UINT8                                 Control;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x54_FRAME;

//
// Function 55H ¡V Get General-Purpose NonVolatile Information
//
// short FAR (*entryPoint)(
// short          Function,             /* PnP BIOS Function 55h */
// unsigned short FAR *Handle,          /* Identifies which GPNV to access */
// unsigned short FAR *MinGPNVRWSize,   /* Minimum buffer size in bytes for GPNV access */
// unsigned short FAR *GPNVSize,        /* Size allocated for GPNV within the R/W Block */
// unsigned long  FAR *NVStorageBase,   /* 32-bit physical base address for... */
//                                      /* ... mem. mapped nonvolatile storage media */
// unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */
//
typedef struct _PNP_FUNCTION_0x55_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           Handle;
  PNP_FAR_PTR                           MinGPNVRWSize;
  PNP_FAR_PTR                           GPNVSize;
  PNP_FAR_PTR                           NVStorageBase;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x55_FRAME;

//
// Function 56H ¡V Read General-Purpose NonVolatile Data
//
// short FAR (*entryPoint)(
// short          Function,             /* PnP BIOS Function 56h */
// unsigned short Handle,               /* Identifies which GPNV is to be read */
// unsigned char  FAR *GPNVBuffer,      /* Address of buffer in which to return GPNV */
// short          FAR *GPNVLock,        /* Lock value */
// unsigned short GPNVSelector,         /* Selector for GPNV Storage */
// unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */
//
typedef struct _PNP_FUNCTION_0x56_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  UINT16                                Handle;
  PNP_FAR_PTR                           GPNVBuffer;
  PNP_FAR_PTR                           GPNVLock;
  UINT16                                GPNVSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x56_FRAME;

//
// Function 57H ¡V Write General-Purpose NonVolatile Data
//
// short FAR (*entryPoint)(
// short          Function,             /* PnP BIOS Function 57h */
// unsigned short Handle,               /* Identifies which GPNV is to be written */
// unsigned char  FAR *GPNVBuffer,      /* Address of buffer containing complete GPNV to write*/
// short          GPNVLock,             /* Lock value */
// unsigned short GPNVSelector,         /* Selector for GPNV Storage */
// unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */
//
typedef struct _PNP_FUNCTION_0x57_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  UINT16                                Handle;
  PNP_FAR_PTR                           GPNVBuffer;
  INT16                                 GPNVLock;
  UINT16                                GPNVSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x57_FRAME;

typedef
INT16
(*PNP_REDIRECTED_ENTRY_POINT) (
  IN VOID                               *Frame
  );

//
// GPNV Header
//
typedef struct {
  UINT32                            Signature;//"GPNV"
  UINT32                            TotalLength;
  UINT8                             Reserved[8];
} PLATFORM_GPNV_HEADER;

#pragma pack ()


EFI_STATUS
EFIAPI
PnPBiosCallback (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  );

INT16
Pnp0x50 (
  IN VOID            *Frame
  );

INT16
Pnp0x51 (
  IN VOID            *Frame
  );

INT16
Pnp0x52 (
  IN VOID            *Frame
  );

INT16
UnsupportedPnpFunction (
  VOID               *Frame
  );

INT16
Pnp0x54 (
  IN VOID            *Frame
  );

INT16
Pnp0x55 (
  IN VOID            *Frame
  );

INT16
Pnp0x56 (
  IN VOID            *Frame
  );

INT16
Pnp0x57 (
  IN VOID            *Frame
  );


extern EFI_SMM_SYSTEM_TABLE2              *mSmst;
extern EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *mSmmFwBlockService;

extern UINTN                              UpdateableStringCount;
extern DMI_UPDATABLE_STRING               *mUpdatableStrings;
extern UINTN                              OemGPNVHandleCount;
extern PLATFORM_GPNV_MAP_BUFFER           GPNVMapBuffer;
extern BOOLEAN                            mIsOemGPNVMap;


#endif
