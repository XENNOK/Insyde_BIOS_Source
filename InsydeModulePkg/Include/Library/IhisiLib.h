/** @file

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

#ifndef _IHISI_LIB_H_
#define _IHISI_LIB_H_

#include <PiSmm.h>
#include <SmiTable.h>


#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmAccess2.h>


#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/BvdtLib.h>
#include <Library/FdSupportLib.h>
#include <Library/VariableLib.h>
#include <Library/UefiBootServicesTableLib.h>

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
  FbtsSkipThisWriteBlock    = 0x29,
  FbtsMeLockReadFail        = 0x2A
} IHISI_STATUS_CODE;

//
// FbtsGetPlatformRomMap type code.
//
typedef enum {
  FbtsRomMapPei = 0,       //EFI_FLASH_AREA_RECOVERY_BIOS
  FbtsRomMapCpuMicrocode,  //EFI_FLASH_AREA_CPU_MICROCODE
  FbtsRomMapNVRam,         //EFI_FLASH_AREA_EFI_VARIABLES
  FbtsRomMapDxe,           //DXE EFI_FLASH_AREA_MAIN_BIOS
  FbtsRomMapEc,            //EFI_FLASH_AREA_FV_EC
  FbtsLogo,                //
  FbtsRomMapNvStorage,     //EFI_FLASH_AREA_GUID_DEFINED
  FbtsRomMapFtwBackup,     //EFI_FLASH_AREA_FTW_BACKUP
  FbtsRomMapFtwState,      //EFI_FLASH_AREA_FTW_STATE
  FbtsRomMapSmbiosLog,     //EFI_FLASH_AREA_SMBIOS_LOG
  FbtsRomMapOemData,       //EFI_FLASH_AREA_OEM_BINARY
  FbtsRomMapGpnv,          //EFI_FLASH_AREA_GPNV
  FbtsRomMapDmiFru,        //EFI_FLASH_AREA_DMI_FRU
  FbtsRomMapPalB,          //EFI_FLASH_AREA_PAL_B
  FbtsRomMapMcaLog,        //EFI_FLASH_AREA_MCA_LOG
  FbtsRomMapPassword,      //EFI_FLASH_AREA_RESERVED_03
  FbtsRomMapOemNvs,        //EFI_FLASH_AREA_RESERVED_04
  FbtsRomMapReserved07,    //EFI_FLASH_AREA_RESERVED_07
  FbtsRomMapReserved08,    //EFI_FLASH_AREA_RESERVED_08
  FbtsRomMapReserved09,    //
  FbtsRomMapReserved0A,    //EFI_FLASH_AREA_RESERVED_0A
  FbtsRomMapUnused,        //EFI_FLASH_AREA_UNUSED
  FbtsRomMapFactoryCopy,   //EFI_FLASH_AREA_RESERVED_09
  FbtsRomMapUndefined,     //
  FbtsRomMapEos = 0xff,    //

} FBST_ROM_MAP_CODE;

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
  UINT8                                 Type;
  UINT32                                Address;
  UINT32                                Size;
  UINT32                                Attribute;
} FBTS_INTERNAL_BIOS_ROM_MAP;

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

#define SECURE_FLASH_SIGNATURE      SIGNATURE_32 ('$', 'S', 'E', 'C')

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
FbtsLibGetWholeBiosRomMap (
  OUT    UINT32                                   *IhisiStatus
  );


EFI_STATUS
FbtsLibGetFlashPartInfo (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
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


/**
  This function uses to read saved CPU double word register from current executing CPU.

  @param[in] RegisterNum  Register number which want to get

  @return UINT32          Specific register UINT32 value.
**/
UINT32
IhisiLibGetDwordRegister (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum
  );

/**
  This function uses to write specific double word value to current executing CPU specific register.

  @param[in] RegisterNum  Register number which want to get

  @return UINT32          Specific register UINT32 value.
**/
EFI_STATUS
IhisiLibSetDwordRegister (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINT32                            RegisterData
  );

UINTN
IhisiLibGetCpuIndex (
  VOID
  );

VOID (*IhisiOemHook)(UINTN);

EFI_STATUS
FbtsLibComplete (
  VOID
  );

/**
  Check whether the start address of buffer is within any of the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval     TURE        The buffer is in SMRAM ranges.
  @retval     FALSE       The buffer is out of SMRAM ranges.
**/
BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID   *Buffer,
  IN UINTN  BufferSize
  );
  
/**
  Verify if the image is signed.

  @param[in] ImageBase       The address of the image.

  @retval EFI_SUCCESS        if the image is not signed
  @retval EFI_ACCESS_DENIED  if the image is signed
**/
EFI_STATUS
VerifySignedImage (
  IN UINT8                      *ImageBase
  );  


extern EFI_SMM_CPU_PROTOCOL       *mSmmCpu;
extern SMI_SUB_FUNCTION           *mFunctionTableBuffer;
extern UINTN                      mFunctionTableCount;
#endif
