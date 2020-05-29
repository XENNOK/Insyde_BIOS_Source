//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PnP_DMI_H_
#define _PnP_DMI_H_

#include "Tiano.h"
#include "SmmPnp.h"
#include "Pnp.h"
#include EFI_GUID_DEFINITION (Smbios)
#include EFI_GUID_DEFINITION (DataHubRecords)

#define SMBIOS_SIGNATURE                EFI_SIGNATURE_32 ('_', 'S', 'M', '_')

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

typedef enum {
  DMI_BYTE_CHANGE,
  DMI_WORD_CHANGE,
  DMI_DWORD_CHANGE,
  DMI_STRING_CHANGE = 0x05,
  DMI_BLOCK_CHANGE
  
} DMI_0X52_SERVICES__SUPPORT;

#pragma pack (1)

//
// PnP SMBIOS function 0x50, Get SMBIOS Information
//

//short FAR (*entryPoint)(
//  short Function,                     /* PnP BIOS Function 50h */
//  unsigned char FAR *dmiBIOSRevision, /* Revision of the SMBIOS Extensions */
//  unsigned short FAR *NumStructures,  /* Max. Number of Structures the BIOS will */
//                                      /* return */
//  unsigned short FAR *StructureSize,  /* Size of largest SMBIOS Structure */
//  unsigned long FAR *dmiStorageBase,  /* 32-bit physical base address for memory- */
//                                      /* mapped SMBIOS data */
//  unsigned short FAR *dmiStorageSize, /* Size of the memory-mapped SMBIOS data */
//  unsigned short BiosSelector );      /* PnP BIOS readable/writable selector */

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

//short FAR (*entryPoint)(
//  short Function,                    /* PnP BIOS Function 51h */
//  unsigned short FAR *Structure,     /* Structure number/handle to retrieve*/
//  unsigned char FAR *dmiStrucBuffer, /* Pointer to buffer to copy structure data */
//  unsigned short dmiSelector,        /* SMBIOS data read/write selector */
//  unsigned short BiosSelector );     /* PnP BIOS readable/writable selector */

typedef struct _PNP_FUNCTION_0x51_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           Structure;
  PNP_FAR_PTR                           DmiStrucBuffer;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x51_FRAME;

//
//Function 52h ¡V Set SMBIOS Structure
//
//short FAR (*entryPoint)(
//short          Function,             /* PnP BIOS Function 52h */
//unsigned char  FAR *dmiDataBuffer,   /* Pointer to buffer with new/change data */
//unsigned char  FAR *dmiWorkBuffer,   /* Pointer to work buffer area for the BIOS */
//unsigned char  Control,              /* Conditions for performing operation */
//unsigned short dmiSelector,          /* SMBIOS data read/write selector */
//unsigned short BiosSelector );       /* PnP BIOS readable/writeable selector */

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
//Function 54h ¡V SMBIOS Control
//

//short FAR (*entryPoint)(
//short          Function,             /* PnP BIOS Function 54h */
//short          SubFunction,          /* Defines the specific control operation */
//void           FAR *Data,            /* Input/output data buffer, SubFunction specific */
//unsigned char  Control,              /* Conditions for setting the structure */
//unsigned short dmiSelector,          /* SMBIOS data read/write selector */
//unsigned short BiosSelector );       /* PnP BIOS readable/writeable selector */
typedef struct _PNP_FUNCTION_0x54_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  INT16                                 SubFunction;
  PNP_FAR_PTR                           Data;
  UINT8                                 Control;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x54_FRAME;

//
//Function 55H ¡V Get General-Purpose NonVolatile Information
//

//short FAR (*entryPoint)(
//short          Function,             /* PnP BIOS Function 55h */
//unsigned short FAR *Handle,          /* Identifies which GPNV to access */
//unsigned short FAR *MinGPNVRWSize,   /* Minimum buffer size in bytes for GPNV access */
//unsigned short FAR *GPNVSize,        /* Size allocated for GPNV within the R/W Block */
//unsigned long  FAR *NVStorageBase,   /* 32-bit physical base address for... */
//                                     /* ... mem. mapped nonvolatile storage media */
//unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */

typedef struct _PNP_FUNCTION_0x55_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  PNP_FAR_PTR                           Handle;
  PNP_FAR_PTR                           MinGPNVRWSize;
  PNP_FAR_PTR                           GPNVSize;
  PNP_FAR_PTR                           NVStorageBase;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x55_FRAME;

//
//Function 56H ¡V Read General-Purpose NonVolatile Data
//

//short FAR (*entryPoint)(
//short          Function,             /* PnP BIOS Function 56h */
//unsigned short Handle,               /* Identifies which GPNV is to be read */
//unsigned char  FAR *GPNVBuffer,      /* Address of buffer in which to return GPNV */
//short          FAR *GPNVLock,        /* Lock value */
//unsigned short GPNVSelector,         /* Selector for GPNV Storage */
//unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */

typedef struct _PNP_FUNCTION_0x56_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  UINT16                                Handle;
  PNP_FAR_PTR                           GPNVBuffer;
  PNP_FAR_PTR                           GPNVLock;
  UINT16                                GPNVSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x56_FRAME;

//
//Function 57H ¡V Write General-Purpose NonVolatile Data
//

//short FAR (*entryPoint)(
//short          Function,             /* PnP BIOS Function 57h */
//unsigned short Handle,               /* Identifies which GPNV is to be written */
//unsigned char  FAR *GPNVBuffer,      /* Address of buffer containing complete GPNV to write*/
//short          GPNVLock,             /* Lock value */
//unsigned short GPNVSelector,         /* Selector for GPNV Storage */
//unsigned short BiosSelector );       /* PnP BIOS readable/writable selector */

typedef struct _PNP_FUNCTION_0x57_FRAME {
  PNP_GENERIC_ENTRY_FRAME               GenericEntryFrame;
  UINT16                                Handle;
  PNP_FAR_PTR                           GPNVBuffer;
  INT16                                 GPNVLock;
  UINT16                                GPNVSelector;
  UINT16                                BiosSelector;
} PNP_FUNCTION_0x57_FRAME;

#pragma pack ()

INT16
Pnp0x50 (
  IN PNP_FUNCTION_0x50_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x50, Get SMBIOS Information.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

INT16
Pnp0x51 (
  IN PNP_FUNCTION_0x51_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x51, Get SMBIOS Structure.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

INT16
Pnp0x52 (
  IN PNP_FUNCTION_0x52_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x52, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

INT16
Pnp0x52ChangeString (
  FUNC_0x52_DATA_BUFFER                 *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
/*++

Routine Description:

  Function 0x52, command = change a single byte of information

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  INT16 to return from Pnp function call

--*/
;

INT16
Pnp0x52ChangeBlock (
  FUNC_0x52_DATA_BUFFER                 *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
/*++

Routine Description:

  Function 0x52, command = change a block of information

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  INT16 to return from Pnp function call

--*/
;

INT16
Pnp0x52ChangeFixedLength (
  FUNC_0x52_DATA_BUFFER                  *Data,
  SMBIOS_STRUCTURE                       *Structure
  );

INT16
Pnp0x54 (
  IN PNP_FUNCTION_0x54_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x54

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

INT16
Pnp0x55 (
  IN PNP_FUNCTION_0x55_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x55, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

INT16
Pnp0x56 (
  IN PNP_FUNCTION_0x56_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x56, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;
BOOLEAN
VaildGPNVCheck (
  IN OUT UINT16                     *Index,
  IN UINT16                         Handle
  );

INT16
Pnp0x57 (
  IN PNP_FUNCTION_0x57_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x57, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
;

EFI_STATUS
WriteGPNV (
  IN UINT16                             Handle,
  IN UINT8                              *GPNVBuffer
);

VOID
Memcpy (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  );
#endif
