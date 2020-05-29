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

/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

    IsaAcpi.h
    
Abstract:

    EFI ISA Acpi Protocol

Revision History

--*/

#ifndef _MSIO_ISA_ACPI_H_
#define _MSIO_ISA_ACPI_H_

#include <Protocol/IsaAcpi.h>

#define EFI_MSIO_ISA_ACPI_PROTOCOL_GUID \
        {0xec75ce8, 0xbcb8, 0x447c, 0x83, 0x90, 0xf, 0x3d, 0x88, 0xa3, 0xfe, 0x4d}

typedef struct _EFI_MSIO_ISA_ACPI EFI_MSIO_ISA_ACPI;

//
// Resource List definition: 
// at first, the resource was defined as below
// but in the future, it will be defined again that follow ACPI spec: ACPI resource type
// so that, in this driver, we can interpret the ACPI table and get the ISA device information. 
//
 
typedef struct {
  UINT32                      HID;
  UINT32                      UID;  
  UINT8                       WhichSIO;
} EFI_MSIO_ISA_ACPI_DEVICE_ID;  
  
typedef struct {
  EFI_MSIO_ISA_ACPI_DEVICE_ID      Device;
  EFI_ISA_ACPI_RESOURCE       *ResourceItem;
} EFI_MSIO_ISA_ACPI_RESOURCE_LIST;

//
// Prototypes for the ISA ACPI Protocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_DEVICE_ENUMERATE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  OUT EFI_MSIO_ISA_ACPI_DEVICE_ID         **Device
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_SET_DEVICE_POWER) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device,
  IN BOOLEAN                         OnOff
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_GET_CUR_RESOURCE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device,
  OUT EFI_MSIO_ISA_ACPI_RESOURCE_LIST     **ResourceList
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_GET_POS_RESOURCE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device,
  OUT EFI_MSIO_ISA_ACPI_RESOURCE_LIST     **ResourceList
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_SET_RESOURCE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device,
  IN EFI_MSIO_ISA_ACPI_RESOURCE_LIST      *ResourceList
  );    

typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_ENABLE_DEVICE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device,
  IN BOOLEAN                         Enable
  );    

typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_INIT_DEVICE) (
  IN EFI_MSIO_ISA_ACPI            *This,
  IN EFI_MSIO_ISA_ACPI_DEVICE_ID          *Device
  );  

typedef
EFI_STATUS
(EFIAPI *EFI_MSIO_ISA_ACPI_INTERFACE_INIT) (
  IN EFI_MSIO_ISA_ACPI            *This
  );

//
// Interface structure for the ISA ACPI Protocol
//
struct _EFI_MSIO_ISA_ACPI {
  EFI_MSIO_ISA_ACPI_DEVICE_ENUMERATE     DeviceEnumerate;
  EFI_MSIO_ISA_ACPI_SET_DEVICE_POWER     SetPower;
  EFI_MSIO_ISA_ACPI_GET_CUR_RESOURCE     GetCurResource;
  EFI_MSIO_ISA_ACPI_GET_POS_RESOURCE     GetPosResource;
  EFI_MSIO_ISA_ACPI_SET_RESOURCE         SetResource;
  EFI_MSIO_ISA_ACPI_ENABLE_DEVICE        EnableDevice;
  EFI_MSIO_ISA_ACPI_INIT_DEVICE          InitDevice;
  EFI_MSIO_ISA_ACPI_INTERFACE_INIT       InterfaceInit;
};

extern EFI_GUID gEfiMsioIsaAcpiProtocolGuid;
  
#endif
