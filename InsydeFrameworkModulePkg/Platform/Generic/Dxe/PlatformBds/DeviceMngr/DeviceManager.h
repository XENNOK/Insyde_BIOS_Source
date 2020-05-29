//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
#include "GenericBdsLib.h"
#include "String.h"

#include EFI_PROTOCOL_DEFINITION (Hii)
#include EFI_PROTOCOL_DEFINITION (FormBrowser)
#include EFI_PROTOCOL_DEFINITION (FormCallback)

#define EFI_DISK_DEVICE_CLASS           0x01
#define EFI_VIDEO_DEVICE_CLASS          0x02
#define EFI_NETWORK_DEVICE_CLASS        0x04
#define EFI_INPUT_DEVICE_CLASS          0x08
#define EFI_ON_BOARD_DEVICE_CLASS       0x10
#define EFI_OTHER_DEVICE_CLASS          0x20
#define EFI_VBIOS_CLASS                 0x40

#define SET_VIDEO_BIOS_TYPE_QUESTION_ID 0x00

#pragma pack(1)
typedef struct {
  UINT8 VideoBIOS;
} MyDevMgrIfrNVData;
#pragma pack()

#define EFI_FP_CALLBACK_DATA_SIGNATURE  EFI_SIGNATURE_32 ('F', 'P', 'C', 'B')
#define EFI_FP_CALLBACK_DATA_FROM_THIS(a) \
  CR (a, \
      EFI_FRONTPAGE_CALLBACK_INFO, \
      DevMgrCallback, \
      EFI_FP_CALLBACK_DATA_SIGNATURE \
      )

typedef struct {
  UINTN                       Signature;
  MyDevMgrIfrNVData           Data;
  EFI_HII_HANDLE              DevMgrHiiHandle;
  EFI_HANDLE                  CallbackHandle;
  EFI_FORM_CALLBACK_PROTOCOL  DevMgrCallback;
} EFI_FRONTPAGE_CALLBACK_INFO;


extern UINT8  DeviceManagerVfrBin[];
extern UINT8  DeviceManagerStringsStr[];

EFI_STATUS
EFIAPI
DeviceManagerCallbackRoutine (
  IN EFI_FORM_CALLBACK_PROTOCOL       *This,
  IN UINT16                           KeyValue,
  IN EFI_IFR_DATA_ARRAY               *DataArray,
  OUT EFI_HII_CALLBACK_PACKET         **Packet
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

#endif
