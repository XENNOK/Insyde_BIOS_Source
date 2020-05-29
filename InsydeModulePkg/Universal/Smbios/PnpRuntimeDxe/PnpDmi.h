/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PNP_DMI_H_
#define _PNP_DMI_H_

#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <Guid/SmBios.h>

#define SMBIOS_SIGNATURE                SIGNATURE_32 ('_', 'S', 'M', '_')

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

typedef UINT64 RS_PNP_PTR;

//												
// for PnpRuntime services registration
//
typedef enum {
  PNP_0X50_SERVICES = 0x50,
  PNP_0X51_SERVICES,
  PNP_0X52_SERVICES
  //
  // new services to be added
  //  
} PNP_COMMANDS;

//
// Pnp0x52 command
//												
typedef enum {
  DMI_BYTE_CHANGE,
  DMI_WORD_CHANGE,
  DMI_DWORD_CHANGE,
  DMI_STRING_CHANGE = 0x05,
  DMI_BLOCK_CHANGE
  
} DMI_0X52_SERVICES_SUPPORT;
												
#pragma pack (1)

//
// PnpRuntime service entry prototype
//
typedef
EFI_STATUS
(*PNP_REDIRECTED_ENTRY_POINT) (
  IN VOID                               *CommBuf
  );

//
// generic frame  
//
typedef struct {
  UINT16              ReturnCode;        // Ref. to ¡§Error Return Codes¡¨ defined in SMBIOS spec
  RS_PNP_PTR          Reserved;
  INT16               Function;
} RS_PNP_GENERIC_ENTRY_FRAME;

//
// PnP SMBIOS function 0x50 frame buffer, Get SMBIOS Information
//
typedef struct _RS_PNP_FUNCTION_0x50_FRAME {
  RS_PNP_GENERIC_ENTRY_FRAME            GenericEntryFrame;
  RS_PNP_PTR                            DmiBIOSRevisionAddress;
  RS_PNP_PTR                            NumStructuresAddress;
  RS_PNP_PTR                            StructureSizeAddress;
  RS_PNP_PTR                            DmiStorageBaseAddress;
  RS_PNP_PTR                            DmiStorageSizeAddress; 
  UINT16                                BiosSelector;
} RS_PNP_FUNCTION_0x50_FRAME;

//
// PnP SMBIOS function 0x51 frame buffer, Get SMBIOS Structure
//
typedef struct _RS_PNP_FUNCTION_0x51_FRAME {
  RS_PNP_GENERIC_ENTRY_FRAME            GenericEntryFrame;
  RS_PNP_PTR                            StructureAddress;
  RS_PNP_PTR                            DmiStrucBufferAddress;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} RS_PNP_FUNCTION_0x51_FRAME;

//
// PnP SMBIOS function 0x52 frame buffer, Set SMBIOS Structure
//
typedef struct _RS_PNP_FUNCTION_0x52_FRAME {
  RS_PNP_GENERIC_ENTRY_FRAME            GenericEntryFrame;
  RS_PNP_PTR                            DmiDataBufferAddress;
  RS_PNP_PTR                            DmiWorkBuffer;
  UINT8                                 Control;
  UINT16                                DmiSelector;
  UINT16                                BiosSelector;
} RS_PNP_FUNCTION_0x52_FRAME;

typedef struct _RS_FUNC_0x52_DATA_BUFFER {
  UINT8                                 Command;
  UINT8                                 FieldOffset;
  UINT32                                ChangeMask;
  UINT32                                ChangeValue;
  UINT16                                DataLength;
  SMBIOS_STRUCTURE                      StructureHeader;
  UINT8                                 StructureData[1];    // Variable length field
} RS_FUNC_0x52_DATA_BUFFER;


#pragma pack ()

//
// PnpRuntime services
//
EFI_STATUS
Pnp0x50 (
  IN  VOID        *CommBuf
  );

EFI_STATUS
Pnp0x51 (
  IN  VOID        *CommBuf
  );

EFI_STATUS
Pnp0x52 (
  IN  VOID        *CommBuf
  );
									
#endif