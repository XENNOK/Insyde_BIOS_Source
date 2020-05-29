/** @file
  This file contains function definitions that can determine
  the TXT capabilities of a platform during PEI and perform
  certain specific platform tasks that are required for TXT
  during PEI.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#ifndef _TXT_PEI_LIB_H_
#define _TXT_PEI_LIB_H_

#include <Txt.h>
#include <CpuAccess.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootGuardLibrary.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Ppi/Stall.h>
#include <Ppi/PciCfg.h>
#include <Ppi/TpmInitialized.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/TxtMemoryUnlocked.h>
#include <Ppi/CpuPlatformPolicy.h>
#include <Guid/TxtInfoHob.h>

#define RESET_PORT                    0x0CF9
#define FULL_RESET_VALUE              0xE

#define PCI_BUS_NUMBER_PCH_LPC        0
#define PCI_DEVICE_NUMBER_PCH_LPC     31
#define PCI_FUNCTION_NUMBER_PCH_LPC   0
#define R_PCH_LPC_ACPI_BASE           0x40

#define APIC_SPURIOUS_VECTOR_REGISTER 0xF0
#define DEST_FIELD                    (0 << 18)
#define ALL_EXCLUDING_SELF            BIT19 + BIT18
#define SIPI                          BIT10 + BIT9
#define INIT                          BIT10 + BIT8
#define LEVEL_ASSERT                  BIT14
#define LEVEL_DEASSERT                (0 << 14)
#define DELIVERY_STATUS               BIT13
#define BASE_ADDR_MASK                0xFFFFF000
///
/// #define EFI_MSR_EXT_XAPIC_LVT_THERM         0x833
///
#define EFI_MSR_EXT_XAPIC_SVR 0x80F
#define AP_STARTUP_SIZE       0x1000
#define AP_STARTUP_ADDR       0x1000
#define AP_STARTUP_STKOFF     AP_STARTUP_ADDR + 0xFF0

#define BFV                   0       ///< Boot Firmware Voume
#define PCI_CMD               0x0004  ///< PCI Command Register
#define BM_BIT                0x4     ///< Bus Master bit
#define PCI_SCC               0x000A  ///< Sub Class Code Register
#define PCI_HDR               0x000E  ///< Header Type Register
#define MF_BIT                0x80    ///< Multi-function bit
///
///    Machne                        check architecture MSR registers
///
#define MCG_CAP     0x179
#define MCG_STATUS  0x17A
#define MCG_CTL     0x17B
#define MC0_CTL     0x400
#define MC0_STATUS  0x401
#define MC0_ADDR    0x402
#define MC0_MISC    0x403

#define EFI_DEADLOOP()    { volatile UINTN __iii; __iii = 1; while (__iii); }

typedef struct _ACM_HEADER {
  UINT32 ModuleType; ///< Module type
  UINT32 HeaderLen;  ///< 4 4 Header length (in multiples of four bytes)
  /// (161 for version 0.0)
  ///
  UINT32 HeaderVersion; ///< 8 4 Module format version
  UINT32 ModuleID;      ///< 12 4 Module release identifier
  UINT32 ModuleVendor;  ///< 16 4 Module vendor identifier
  UINT32 Date;          ///< 20 4 Creation date (BCD format:
  /// year.month.day)
  ///
  UINT32 Size;            ///< 24 4 Module size (in multiples of four bytes)
  UINT32 Reserved1;       ///< 28 4 Reserved for future extensions
  UINT32 CodeControl;     ///< 32 4 Authenticated code control flags
  UINT32 ErrorEntryPoint; ///< 36 4 Error response entry point offset (bytes)
  UINT32 GDTLimit;        ///< 40 4 GDT limit (defines last byte of GDT)
  UINT32 GDTBasePtr;      ///< 44 4 GDT base pointer offset (bytes)
  UINT32 SegSel;          ///< 48 4 Segment selector initializer
  UINT32 EntryPoint;      ///< 52 4 Authenticated code entry point offset (bytes)
  UINT32 Reserved2;       ///< 56 64 Reserved for future extensions
  UINT32 KeySize;         ///< 120 4 Module public key size less the exponent
  /// (in multiples of four bytes
  /// - 64 for version 0.0)
  ///
  UINT32 ScratchSize; ///< 124 4 Scratch field size (in multiples of four bytes)
  /// (2 * KeySize + 15 for version 0.0)
  ///
  UINT8 RSAPubKey[65 * 4]; ///< 128 KeySize * 4 + 4 Module public key
  UINT8 RSASig[256];       ///< 388 256 PKCS #1.5 RSA Signature.
} ACM_HEADER;

typedef struct _TXT_PEI_LIB_CONTEXT {
  CONST EFI_PEI_SERVICES  **PeiServices;
  EFI_PEI_CPU_IO_PPI      *CpuIoPpi;
  EFI_PEI_PCI_CFG_PPI     *PciCfgPpi;
  EFI_PEI_STALL_PPI       *PeiStall;
  ACM_HEADER              *BiosAcmBase;
  UINT32                  BiosAcmSize;
  VOID                    *ApStartup;
  UINT32                  *McuStart;
  UINT32                  Ia32ApicBase;
  TXT_INFO_HOB            *Hob;
} TXT_PEI_LIB_CONTEXT;

#pragma pack(1)
typedef union _MSR_REGISTER {
  UINT64 Qword;

  struct _DWORDS {
    UINT32 Low;
    UINT32 High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;


#pragma pack()

#define PLATFORM_ID_SHIFT 50
#define PLATFORM_ID_MASK  7   ///< Bits 52:50
typedef struct _MCU {
  UINT32 headerVer;     ///< MCU Header Version ( = 00000001h )
  UINT32 revision;      ///< MCU Revision
  UINT32 date;          ///< MCU Date
  UINT32 signature;     ///< MCU Processor Signature
  UINT32 checksum;      ///< MCU Main checksum
  UINT32 loaderRev;     ///< MCU Loader Revision
  UINT32 procFlags;     ///< MCU Processor Flags (Platform ID)
  UINT32 dataSize;      ///< MCU Data Size
  UINT32 totalSize;     ///< MCU Total Size
  UINT32 reserved[3];
} MCU;

typedef struct _EST {
  UINT32 count;         ///< EST Count
  UINT32 checksum;      ///< EST Checksum
  UINT32 reserved[3];
} EST;

typedef struct _PSS {
  UINT32 signature;     ///< PSS Processor Signature
  UINT32 procFlags;     ///< PSS Processor Flags (Platform ID)
  UINT32 checksum;      ///< PSS Checksum
} PSS;

/**
  Returns CPU count

  @retval Number of CPUs
**/
UINT32
GetCpuCount (
  VOID
  );

/**
  Execute SCLEAN through BIOS ACM
**/
VOID
LaunchBiosAcmSclean (
  VOID
  );

/**
  Issue a global reset through PCH and PORTCF9
**/
VOID
DoGlobalReset (
  VOID
  );

/**
  Issue a cpu-only reset through PCH and PORTCF9
**/
VOID
DoCpuReset (
  VOID
  );

/**
  Issue a HOST reset through PCH and PORTCF9
**/
VOID
DoHostReset (
  VOID
  );

/**
  Dispatch APs to execute *Function with parameter pointed by *Param

  @param[in] (*Function)  - Address of Function to be executed by APs
  @param[in] Param        - Function parameter to be passed to
**/
VOID
StartupAllAPs (
  VOID (*Function)(),
  UINT64 *Param
  );

/**
  Put All APs into Wait-for-SIPI state
**/
VOID
PutApsInWfs (
  VOID
  );

/**
  Restore MTRR registers

  @param[in] ApCfg - Point to the MTRR buffer
**/
VOID
RestoreMtrrProgramming (
  UINT64 *ApCfg
  );

/**
  Restore APs' registers

  @param[in] ApCfg - Point to APs' registers buffer
**/
VOID
RestoreApConfig (
  UINT64 *ApCfg
  );

/**
  Initializes values passed to AP

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure
**/
VOID
PrepareApParams (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  This routine initializes and collects all PPIs and data required
  by the routines in this file.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] mPS - A pointer to the PEI Service Table

  @exception EFI_UNSUPPORTED   - If any of the required PPIs or data are unavailable
  @retval EFI_SUCCESS       - In all cases not listed above
**/
EFI_STATUS
InitializeTxtPeiLib (
  TXT_PEI_LIB_CONTEXT *pctx,
  IN CONST EFI_PEI_SERVICES **mPS
  );

/**
  Determines whether or not the current processor is TXT Capable.

  @retval TRUE          - If the current processor supports TXT
  @retval FALSE         - If the current processor does not support TXT
**/
BOOLEAN
IsTxtProcessor (
  VOID
  );

/**
  Determines whether or not the current chipset is TXT Capable.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the current chipset supports TXT
  @retval FALSE         - If the current chipset doesn't supports TXT
**/
BOOLEAN
IsTxtChipset (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines whether or not POISON bit is set in status register

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TXT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the TXT_WAKE_ERROR bit is unasserted.
**/
BOOLEAN
IsTxtResetSet (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines whether or not SECRETS.STS bit is set in E2STS status register

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the LT.SECRETS.STS bit is asserted.
  @retval FALSE         - If the LT.SECRETS.STS bit is unasserted.
**/
BOOLEAN
IsTxtSecretsSet (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsEstablishmentBitAsserted (
  IN TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines whether or not the platform has encountered an error during
  a sleep or power-off state.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TXT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the TXT_WAKE_ERROR bit is unasserted.
**/
BOOLEAN
IsTxtWakeError (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines whether or not the platform memory has been locked

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If memroy is locked
  @retval FALSE         - If memory is unlocked
**/
BOOLEAN
IsMemoryLocked (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If TXT is enabled by platform setting
  @retval FALSE         - If TXT is disabled by platform setting
**/
BOOLEAN
IsTxtEnabled (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines ACM is matched to chipset or not

  @param[in] pctx         - Point to TXT_PEI_LIB_CONTEXT structure
  @param[in] BiosAcmBase  - A pointer to BIOS ACM location

  @retval TRUE          - BIOS ACM is matched to chipset
  @retval FALSE         - BIOS ACM is NOT matched to chipset
**/
BOOLEAN
CheckTxtAcmMatch (
  TXT_PEI_LIB_CONTEXT *pctx,
  ACM_HEADER          *BiosAcmBase
  );

/**
  Clear Sleep Type register.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - Always
**/
EFI_STATUS
ClearSlpTyp (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Invokes the SCLEAN function from the TXT BIOS ACM.
  1. Clearing of sleep type is necessary because SCLEAN destroys memory
  context, so S3 after it is run and system is reset is impossible. We
  do it here since there is no any indication that can sustain reset
  for any other module to do it on our behalf.
  2. APs are initialized before calling of SCLEAN
  3. SCLEAN function is invoked.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - Always.
**/

EFI_STATUS
DoSclean (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Determines presence of TPM in system

  @param[in] pctx         - Point to TXT_PEI_LIB_CONTEXT structure

  @retval EFI_SUCCESS          - If the TPM is present.
  @retval EFI_NOT_FOUND        - If the TPM is not present.
**/
EFI_STATUS
IsTpmPresent (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Searches PEI firemare volume (FV_BB) for file containig BIOS ACM.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] pBIOSAC_BASE  - A pointer to pointer to variable to hold found address

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindBiosAcmInVolume (
  TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32          **pBIOSAC_BASE
  );

/**
  Searches PEI firmware volume (FV_BB) for file containig AP Startup code

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] pAP_STARTUP  - A pointer to pointer to variable to hold address
  @param[in] address.

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindApStartupInVolume (
  TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32          **pAP_STARTUP
  );

/**
  Searches PEI firmware volume (FV_BB) for offset of currently loaded MCU patch

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] pMCU          - A pointer to pointer to variable to hold found offset
  @param[in] address.

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindMcuInVolume (
  TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32          **pMCU
  );

/**
  Searches PEI firmware volume (FV_BB) for the file with specified GUID through pGuid

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] pGuid         - A pointer GUID
  @param[in] pModule       - A pointer to pointer to variable to hold address

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI ERROR     - If address has not been found
**/
EFI_STATUS
FindModuleInFlash (
  TXT_PEI_LIB_CONTEXT *pctx,
  EFI_GUID            *pGuid,
  OUT UINT32          **pModule
  );

/**
  Parses Hob list for TXT Info HOB

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - If TXT Info Hob is found
  @retval EFI_NOT_FOUND - If TXT Info Hob is not found
**/
EFI_STATUS
CreateTxtInfoHob (
  TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Unlock memory when security is set ant TxT is not enabled

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS     - Complete memory unlock
  @exception EFI_UNSUPPORTED - CPU doesn't support TxT.
**/
EFI_STATUS
UnlockMemory (
  IN TXT_PEI_LIB_CONTEXT *pctx
  );

/**
  Fix up pointers since they are located in real memory now.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
DprUpdate (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *Ppi
  );

#endif
