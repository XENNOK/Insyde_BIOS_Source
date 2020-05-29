//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   IhisiLib.h
//;

#ifndef _IHISI_LIB_H_
#define _IHISI_LIB_H_

#include "Tiano.h"
#include "EfiFlashMap.h"
#include "EfiSmmDriverLib.h"
#include "EfiCommonLib.h"
#include "FdSupportLib.h"
#include "ChipsetLib.h"
#include "BvdtLib.h"
#include "OemPnpStrings.h"
#include "SmiTable.h"

#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)
#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmVariable)
#include EFI_PROTOCOL_CONSUMER   (SmmSwDispatch)
#include EFI_PROTOCOL_CONSUMER   (SmmAccess)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmChipsetLibServices)

extern EFI_SMM_SYSTEM_TABLE      *mSmst;
extern EFI_SMM_RUNTIME_PROTOCOL  *mSmmRT;
extern EFI_SMM_CPU_SAVE_STATE    *SmmCpuSaveLocalState;
extern EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL *mSmmChipsetLibServices;

//
// SMI register value
//
#define SW_SMI_INPUT_VALUE       IHISI_SW_SMI


#define MODEL_NAME_SIZE          0x20
#define MODEL_VERSION_SIZE       0x80

//
// IHISI input register EBX, for security check
//
#define IHISI_EBX_SIGNATURE      0x2448324F      //$H2O

//
// Output table elements, for VATS security check.
//
#define IHISI_H2O_SIGNATURE      0x4F324824      //$H2O 0x4F324824
#define IHISI_VAR_SIGNATURE      0x72615624      //$Var 0x72615624
#define IHISI_TBL_SIGNATURE      0x6C625424      //$Tbl 0x6C625424
#define IHISI_TABLE_REVISION     0x10            //Version 1.0

//
// Ihisi Status Code
//
typedef enum {
  IhisiSuccess                = 0x00,
  IhisiAccessProhibited       = 0x01,
  IhisiObLenTooSmall          = 0x02,
  IhisiInvalidParamFatalError = 0x03,
  IhisiFunNotSupported        = 0xEF,
  //
  //  VATS status code.
  //
  VatsVarNotFound           = 0x10,
  VatsWrongObFormat         = 0x11,
  VatsObChecksumFailed      = 0x12,
  VataObTableRevUnsupported = 0x13,
  VatsVarPartAccessErr      = 0x14,
  //
  // FBTS status code.
  //
  FbtsPermissionDeny        = 0x20,
  FbtsUnknowPlatformInfo    = 0x21,
  FbtsUnknowPlatformRomMap  = 0x22,
  FbtsUnknowFlashPartInfo   = 0x23,
  FbtsReadFail              = 0x24,
  FbtsWriteFail             = 0x25,
  FbtsEraseFail             = 0x26,
  FbtsCanNotSkipModuleCheck = 0x27,
  FbtsNextBlock             = 0x28,
  FbtsSkipThisWriteBlock    = 0x29
} IHISI_STATUS_CODE;

#pragma pack(1)
//
// IHISI typedef struct
//
typedef struct {
  UINT8                                 SubFun;
  EFI_STATUS                            (*SubFunction) ();
} SMI_SUB_FUNCTION;

//
// VATS typedef struct
//
typedef struct {
  EFI_GUID                              VarGuid;
} IHISI_INPUT_BUFFER;

typedef struct {
  UINT32                                TableId1;
  UINT32                                TableId2;
  UINT32                                TableId3;
  UINT8                                 Reserved1[4];
  UINT32                                VarLength;
  UINT32                                Attribute;
  UINT8                                 Reserved2[5];
  UINT8                                 TableRev;
  UINT16                                VarChecksum;
} IHISI_OUTPUT_BUFFER;


//
// FBTS typedef struct
//
typedef struct {
  CHAR16                                ModelName [MODEL_NAME_SIZE];
  CHAR16                                ModelVersion [MODEL_VERSION_SIZE];
} FBTS_PLATFORM_INFO_BUFFER;

typedef struct {
  UINT8                                 Type;
  UINT32                                Address;
  UINT32                                Length;
} FBTS_PLATFORM_ROM_MAP;

typedef struct {
  FBTS_PLATFORM_ROM_MAP                 PlatFormRomMap [40];
} FBTS_PLATFORM_ROM_MAP_BUFFER;

typedef struct {
  UINT32                                LinearAddress;
  UINT32                                Size;
} FBTS_PLATFORM_PRIVATE_ROM;

typedef struct {
  FBTS_PLATFORM_PRIVATE_ROM             PlatFormRomMap [40];
} FBTS_PLATFORM_PRIVATE_ROM_BUFFER;

typedef struct {
  UINT8                                 AcStatus;
  UINT8                                 Battery;
  UINT8                                 Bound;
  UINT32                                OemHelp1;
  UINT32                                OemHelp2;
  UINT16                                Customer;
} FBTS_PLATFORM_STATUS_BUFFER;

typedef struct {
  UINT8                                 StringInfo [0x16];
  UINT32                                String1;
  UINT32                                String2;
  UINT32                                String3;
  UINT32                                String4;
} STRING_ARRAY;

typedef struct {
  UINT8                                 Size;
  CHAR8                                 VendorName [31];
  CHAR8                                 DeviceName [32];
  UINT32                                Id;
  UINT32                                SpecifiedSize;
  UINT16                                *BlockMap;
} FBTS_FLASH_DEVICE;

#pragma pack()
//
// IHISI prototype
//
VOID
IhisiLibErrorCodeHandler(
  IN     UINT32                                   IhisiStatus
  );

EFI_STATUS
IhisiLibInit (
  IN  SMI_SUB_FUNCTION                            *FunctionTableBuffer,
  IN  UINTN                                       FunctionTableCount
  );

EFI_STATUS
VatsLibInit (
  VOID
  );

EFI_STATUS
VatsLibRead (
  VOID
  );

EFI_STATUS
VatsLibWrite (
  VOID
  );

//
// VATS Support Functions
//
#define VATS_FUNCTIONS \
        { 0x00,  VatsLibRead       }, \
        { 0x01,  VatsLibWrite      }


#ifdef SECURE_BOOT_SUPPORT
//
// Factory default Support Function
//
#define COMMON_REGION_FUNCTIONS   { 0x49, CommonRegionDataCommunication }, \
                                  { 0x4B, CommonRegionDataRead }

EFI_STATUS
CommonRegionDataCommunication (
  VOID
  );

EFI_STATUS
CommonRegionDataRead (
  VOID
  );
#endif

#ifdef SECURE_FLASH_SUPPORT
#define SECURE_FLASH_SIGNATURE      EFI_SIGNATURE_32 ('$', 'S', 'E', 'C')

//
// Secure Flash Support Function
//
#define SECURE_FLASH_FUNCTION    { 0x48, SecureFlashFunction }
//
// Secure Flash Function Prototype
//
EFI_STATUS
SecureFlashFunction (
  VOID
  );
#endif


//
// FBTS prototype
//
EFI_STATUS
FbtsLibGetPlatformInfo (
  OUT    UINT32                                   *IhisiStatus
  );

EFI_STATUS
FbtsLibGetPlatformRomMap (
  IN     FBTS_PLATFORM_ROM_MAP_BUFFER             *RomMapBufferPtr,
  IN     FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *PrivateRomMapBufferPtr,
  OUT    UINT32                                   *IhisiStatus
  );

EFI_STATUS
FbtsLibGetFlashPartInfo (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL        *SmmFwBlockService,
  IN     EFI_SMM_SYSTEM_TABLE                     *mSmst,
  OUT    UINT32                                   *IhisiStatus
  );

EFI_STATUS
FbtsLibRead (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL        *SmmFwBlockService,
  OUT    UINT32                                   *IhisiStatus
  );

EFI_STATUS
FbtsLibWrite (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL        *SmmFwBlockService,
  OUT    UINT32                                   *IhisiStatus
  );

UINT32
FbtsLibStatusTranslation (
  IN EFI_STATUS                                   Status
  );

VOID (*IhisiOemHook)(UINTN);

EFI_STATUS
FbtsLibComplete (
  VOID
  );

EFI_STATUS
FbtsLibGetPreservedTable (
  VOID
  );

BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID   *Buffer,
  IN UINTN  BufferSize
  );

  
EFI_STATUS
VerifySignedImage (
  IN UINT8                      *ImageBase
  );

#endif //End of #ifndef _IHISI_LIB_H_
