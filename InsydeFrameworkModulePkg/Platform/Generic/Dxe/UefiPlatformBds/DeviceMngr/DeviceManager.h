//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2005, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  DeviceManager.c

Abstract:

  The platform device manager reference implement

Revision History

--*/

#ifndef _DEVICE_MANAGER_H
#define _DEVICE_MANAGER_H

#include "Tiano.h"
#include "Bds.h"
#include "BdsStrDefs.h"
#include "GenericBdsLib.h"
#include "String.h"
#include EFI_GUID_DEFINITION (HiiDriverHealthFormset)


#define DEVICE_MANAGER_FORMSET_GUID  \
  { \
    0x3ebfa8e6, 0x511d, 0x4b5b, 0xa9, 0x5f, 0xfb, 0x38, 0x26, 0xf, 0x1c, 0x27 \
  }
#define DRIVER_HEALTH_FORMSET_GUID  \
  { \
    0xf76e0a70, 0xb5ed, 0x4c38, 0xac, 0x9a, 0xe5, 0xf5, 0x4b, 0xf1, 0x6e, 0x34 \
  }

#define LABEL_VBIOS                          0x0040

#define DEVICE_MANAGER_FORM_ID               0x1000

#define DEVICE_KEY_OFFSET                    0x1000
#define DEVICE_MANAGER_KEY_VBIOS             0x2000
#define FRONT_PAGE_KEY_DEVICE_MANAGER        0x8567

//
// For Device Manager utility
//
#define LABEL_DEVICES_LIST                   0x1100
#define DEVICE_MANAGER_KEY_DRIVER_HEALTH     0x1111

//
// For Driver Health utility
//
#define LABEL_DRIVER_HEALTH                  0x2000
#define LABEL_DRIVER_HEALTH_END              0x2001

#define LABEL_DRIVER_HEALTH_REAPIR_ALL       0x3000
#define LABEL_DRIVER_HEALTH_REAPIR_ALL_END   0x3001

#define DRIVER_HEALTH_FORM_ID                0x1003

#define DRIVER_HEALTH_KEY_OFFSET             0x2000
#define DRIVER_HEALTH_REPAIR_ALL_KEY         0x3000
#define DRIVER_HEALTH_RETURN_KEY             0x4000

//
// These are the VFR compiler generated data representing our VFR data.
//
extern UINT8  DeviceManagerVfrBin[];
extern UINT8  DriverHealthVfrBin[];

#define DEVICE_MANAGER_CALLBACK_DATA_SIGNATURE  EFI_SIGNATURE_32 ('D', 'M', 'C', 'B')
#define DEVICE_MANAGER_DRIVER_HEALTH_INFO_SIGNATURE  EFI_SIGNATURE_32 ('D', 'M', 'D', 'H')

typedef struct {
  UINTN                           Signature;

  //
  // HII relative handles
  //
  EFI_HII_HANDLE                  HiiHandle;
  //
  // Driver Health HII relative handles
  //
  EFI_HII_HANDLE                  DriverHealthHiiHandle;
  
  EFI_HANDLE                      DriverHandle;
  EFI_HANDLE                      DriverHealthHandle;

  //
  // Produced protocols
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL  ConfigAccess;
  //
  // Driver Health Produced protocols
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL  DriverHealthConfigAccess;
  
  //
  // Configuration data
  //
  UINT8                           VideoBios;
} DEVICE_MANAGER_CALLBACK_DATA;


typedef struct {
  UINTN                           Signature;
  EFI_LIST_ENTRY                  Link;

  ///
  /// HII relative handles
  ///
  EFI_HII_HANDLE                  HiiHandle;

  ///
  /// Driver relative handles
  ///
  EFI_HANDLE                      DriverHandle;
  EFI_HANDLE                      ControllerHandle;
  EFI_HANDLE                      ChildHandle;

  EFI_DRIVER_HEALTH_PROTOCOL      *DriverHealth;
  ///
  /// Driver health messages of the specify Driver 
  ///
  EFI_DRIVER_HEALTH_HII_MESSAGE   *MessageList;

  ///
  /// Driver Health status
  ///
  EFI_DRIVER_HEALTH_STATUS        HealthStatus;
} DRIVER_HEALTH_INFO;

#define DEVICE_MANAGER_HEALTH_INFO_FROM_LINK(a) \
  CR (a, \
      DRIVER_HEALTH_INFO, \
      Link, \
      DEVICE_MANAGER_DRIVER_HEALTH_INFO_SIGNATURE \
      )

#define DEVICE_MANAGER_CALLBACK_DATA_FROM_THIS(a) \
  CR (a, \
      DEVICE_MANAGER_CALLBACK_DATA, \
      ConfigAccess, \
      DEVICE_MANAGER_CALLBACK_DATA_SIGNATURE \
      )

typedef struct {
  EFI_STRING_ID  StringId;
  UINT16         Class;
} DEVICE_MANAGER_MENU_ITEM;

EFI_STATUS
EFIAPI
DeviceManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
;

EFI_STATUS
InitializeDeviceManager (
  VOID
  )
;

EFI_STATUS
CallDeviceManager (
  VOID
  )
;

EFI_STATUS
EFIAPI
DriverHealthCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
EFIAPI
GetSingleControllerHealthStatus (
  IN OUT EFI_LIST_ENTRY               *DriverHealthList,
  IN EFI_HANDLE                       DriverHandle,
  IN EFI_HANDLE                       ControllerHandle,  OPTIONAL
  IN EFI_HANDLE                       ChildHandle,       OPTIONAL
  IN EFI_DRIVER_HEALTH_PROTOCOL       *DriverHealth,
  IN EFI_DRIVER_HEALTH_STATUS         *HealthStatus
  );

EFI_STATUS
GetAllControllersHealthStatus (
  IN OUT EFI_LIST_ENTRY  *DriverHealthList
  );

BOOLEAN
PlaformHealthStatusCheck (
  VOID
  );

VOID
PlatformRepairAll (
  IN EFI_LIST_ENTRY  *DriverHealthList
  );

VOID
ProcessSingleControllerHealth (
  IN  EFI_DRIVER_HEALTH_PROTOCOL         *DriverHealth,
  IN  EFI_HANDLE                         ControllerHandle, OPTIONAL
  IN  EFI_HANDLE                         ChildHandle,      OPTIONAL
  IN  EFI_DRIVER_HEALTH_STATUS           HealthStatus,
  IN  EFI_DRIVER_HEALTH_HII_MESSAGE      **MessageList,    OPTIONAL
  IN  EFI_HII_HANDLE                     FormHiiHandle,
  IN OUT BOOLEAN                         *RebootRequired
  );

VOID
RepairNotify (
  IN  UINTN Value,
  IN  UINTN Limit
  );

VOID
ProcessMessages (
  IN  EFI_DRIVER_HEALTH_HII_MESSAGE      *MessageList
  );

VOID
CallDriverHealth (
  VOID
  );

CHAR8 *
DriverHealthSelectBestLanguage (
  IN CHAR8        *SupportedLanguages,
  IN BOOLEAN      Iso639Language
  );

EFI_STATUS
GetComponentNameWorker (
  IN  EFI_GUID                    *ProtocolGuid,
  IN  EFI_HANDLE                  DriverBindingHandle,
  OUT EFI_COMPONENT_NAME_PROTOCOL **ComponentName,
  OUT CHAR8                       **SupportedLanguage
  );

EFI_STATUS
GetDriverNameWorker (
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  DriverBindingHandle,
  OUT CHAR16      **DriverName
  );

EFI_STATUS
DriverHealthGetDriverName (
  IN  EFI_HANDLE  DriverBindingHandle,
  OUT CHAR16      **DriverName
  );

EFI_STATUS
GetControllerNameWorker (
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  ChildHandle,
  OUT CHAR16      **ControllerName
  );

EFI_STATUS
DriverHealthGetControllerName (
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  ChildHandle,
  OUT CHAR16      **ControllerName
  );

#endif
