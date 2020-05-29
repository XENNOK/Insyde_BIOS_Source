/** @file
  EFI Variable Default Update protocol

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

#ifndef _EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL_H_
#define _EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL_H_

#include <Protocol/DevicePath.h>
//
// Global ID for the Factory Default Update Protocol
//
#define EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL_GUID \
  {0xb701cfae, 0x4dd4, 0x42ff, 0xae, 0x96, 0xf2, 0x14, 0x67, 0xa9, 0x2b, 0x3c}


typedef struct _EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL;

#define RESTORE_WITH_CLEARING_ALL_SETTINGS          0
#define RESTORE_WITH_RESERVING_OTHER_SETTINGS       1


/**
 This function uses to update system setting to factory default.

 @param [in]   This             Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.
 @param [in]   RestoreType      Restore type to update for the variable store.

 @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
 @retval EFI_SUCCESS            Update system setting to factory default successful.
 @return Other                   Other error cause update system to factory default failed.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_VARIABLE_DEFAULT_UPDATE_FACTORY_SETTING) (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      UINT32                                   RestoreType
  );

/**
 This function uses to update system setting to backup default.

 @param [in]   This             Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.
 @param [in]   DevicePath       Device path pointer to the source of backup settings.
 @param [in]   RestoreType      Restore type to update for the variable store.

 @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
 @retval EFI_SUCCESS            Update system setting to backup default successful.
 @return Other                   Other error cause update system to backup default failed.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_VARIABLE_DEFAULT_UPDATE_BACKUP_SETTING) (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      EFI_DEVICE_PATH_PROTOCOL                 *DevicePath,
  IN      UINT32                                   RestoreType
  );


//
// Interface structure for the Variable Defult Update Protocol
//
struct _EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL {
   EFI_VARIABLE_DEFAULT_UPDATE_FACTORY_SETTING         UpdateFactorySetting;
   EFI_VARIABLE_DEFAULT_UPDATE_BACKUP_SETTING          UpdateBackupSetting;
 };


extern EFI_GUID gEfiVariableDefaultUpdateProtocolGuid;

#endif
