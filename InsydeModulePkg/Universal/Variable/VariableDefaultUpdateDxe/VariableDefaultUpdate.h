/** @file
  This is a simple variable default update driver. This driver checks policy to update
  default setting to NV_VARIABLE_STORE and provide interface for update default setting to
  NV_VARIABLE_STORE.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _EFI_VARIABLE_DEFAULT_UPDATE_H_
#define _EFI_VARIABLE_DEFAULT_UPDATE_H_

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableSupportLib.h>
//
// Protocols definition
//
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/VariableDefaultUpdate.h>
#include <Protocol/DevicePath.h>

//
// GUID definitons
//
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>

//
// Driver realtive defitions
//
#define VARIABLE_DEFAULT_UPDATE_SIGNATURE   SIGNATURE_32 ('_', 'V', 'D', 'U')
#define FACTORY_DEFAULT_COPY_SIGNATURE      SIGNATURE_32 ('_', 'F', 'D', 'C')

#define VARIABLE_DEFAULT_INSTANCE_FROM_THIS(a) CR (a, VARIABLE_DEFAULT_UPDATE_DEVICE, DefaultUpdateInstance, VARIABLE_DEFAULT_UPDATE_SIGNATURE)

//
// Store Index relative definitions
//
#define  VARIABLE_STORE           0x00
#define  SPARE_STORE              0x01
#define  FACTORY_COPY_STORE       0x02
#define  MAX_NUM_STORE            0x03

//
// Process state relative definitions
//
#define  FACTORY_DEFAULT_STATE    0x01
#define  BACKUP_DEFAULT_STATE     0x02


#define MAX_TRY_ACCESS_FVB_TIMES  0x02


//
// Driver realtive structure definitions
//
#pragma pack(push, 1)
typedef struct {
  UINT32              Signature;
  UINT32              Length;
} FACTORY_DEFAULT_HEADER;
#pragma pack(pop)

typedef struct {
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *Fvb;
  EFI_PHYSICAL_ADDRESS                    StoreAddress;
  EFI_LBA                                 StoreLba;
  UINTN                                   StoreLength;
  UINTN                                   SizeofBlock;
  UINTN                                   NumberOfBlock;
 } STORE_DEVICE_INSTANCE;

typedef struct {
  UINT32                                  Signature;
  EFI_HANDLE                              Handle;
  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL    DefaultUpdateInstance;
  STORE_DEVICE_INSTANCE                   StoreDev[MAX_NUM_STORE];
} VARIABLE_DEFAULT_UPDATE_DEVICE;

/**
  Check the data in factory copy sub-region header  is whether valid.

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   The data in facotry copy sub-region header is valid.
  @retval FALSE                  The data in facotry copy sub-region header is invalid.

**/
BOOLEAN
IsFacotryCopyHeaderValid (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  );


/**
  Check the data in factory copy sub-region is whether valid.

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   The data in facotry copy region is valid.
  @retval FALSE                  The data in facotry copy region is invalid.

**/
BOOLEAN
IsFactoryCopyValid (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  );

/**
  Check system state is whether in the restore factory default process

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   System is in resotre factory default process.
  @retval FALSE                  System isn't in resotre factory default process.

**/
BOOLEAN
InRestoreFactoryDefaultProcess (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  );

/**
  Read fixed length data from specific variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  Length                  Number of bytes want to read.
  @param  DevIndex                Index for variable default update device.
  @param  Offset                  Start offset in this store
  @param  Buffer                  Buffer to put read data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ABORTED             The function could not complete successfully
  @retval  EFI_INVALID_PARAMETER  Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateReadBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 Length,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 Offset,
  OUT UINT8                                 *Buffer
  );


/**
  Write fixed length data to specific variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  Length                  Number of bytes want to write.
  @param  DevIndex                Index for variable default update device.
  @param  Offset                  Start offset in this store
  @param  Buffer                  Buffer saved data want to write.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ABORTED             The function could not complete successfully
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateWriteBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 Length,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 Offset,
  IN  UINT8                                 *Buffer
  );


/**
  This function uses to erase specifc variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  DevIndex                Index for variable default update device.
  @param  StartLba                Start Lba which want to erase.
  @param  NumOfBlock              Number of blocks want to erase.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateEraseBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 StartLba,
  IN  UINTN                                 NumOfBlock
  );


/**
  This function uses to erase specifc variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  State                   Variable default update state.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
SetDefaultUpdateState (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 State
  );


/**
  This function uses to restore data in factory default sub-region to variable
  store region.

  @param  VariableDefaultDevice   Point to private data.
  @param  DataLength              The size in bytes of the Data buffer.
  @param  Data                    The contents for the variable store.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
RestoreFactoryDefaultToVariableStore (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 DataSize,
  IN  UINT8                                 *Data
  );

/**
  This function uses to recoery factory default sub-region.

  @param  VariableDefaultDevice   Point to private data.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
RecoveryFactoryDefault (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice
  );

/**
  According to restore type to collect factory default settings and restore these settings
  in output buffer.

  @param  VariableDefaultDevice   Point to private data.
  @param  RestoreType             Restore type to update for the variable store.
  @param  VariableStoreLength     On input, the size in bytes of the return VariableBuffer.
                                  On output the size of data returned in VariableBuffer.
  @param  VariableBuffer          The buffer to return the contents of the variable.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.
  @retval EFI_BUFFER_TOO_SMALL    The VariableStoreLength is too small for the result.  VariableStoreLength has
                                  been updated with the size needed to complete the request.
**/
EFI_STATUS
CollectFactoryDefaultSetting (
  IN     VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice,
  IN     UINT32                               RestoreType,
  IN OUT UINTN                                *VariableStoreLength,
  OUT    UINT8                                *VariableBuffer
  );

/**
  This function uses backup data to update system setting to factory default.

  @param  This                    Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.

  @retval EFI_INVALID_PARAMETER   Input parameter is invalid.
  @retval EFI_SUCCESS             Update system setting to factory default successful.
  @return Other                   Other error cause update system to factory default failed.

**/
EFI_STATUS
UpdateFactorySettingFromBackup (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This
  );
#endif
