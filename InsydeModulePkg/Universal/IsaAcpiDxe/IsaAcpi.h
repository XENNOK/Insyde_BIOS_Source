/** @file

Declaration for IsaAcpi.c

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _LPC_ISA_ACPI_H_
#define _LPC_ISA_ACPI_H_

#include <Sio/SioResource.h>
#include <Protocol/IsaPnpDevice.h>
#include <Protocol/IsaIo.h>

/**
  Search resources of the IsaAcpi.  

  @param[in]       This            This IsaAcpi protocol.
  @param[out]      Device          Pointer IsaAcpi device.

  @retval          EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaDeviceEnumerate (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  OUT    EFI_ISA_ACPI_DEVICE_ID      **Device
  );

/**
  Connect to device protocol to control power on or off.  

  @param[in]   This            This IsaAcpi protocol.
  @param[in]   Device          Pointer IsaAcpi device.
  @param[in]   OnOff           Pointer IsaAcpi device.

  @retval      EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaDeviceSetPower (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  IN     BOOLEAN                     OnOff
  );

/**
  Connect to device protocol to get current resource.  

  @param[in]   This            This IsaAcpi protocol.
  @param[in]   Device          Device ID.
  @param[out]  ResourceList    Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaGetCurrentResource (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT    EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  );

/**
  Connect to device protocol to get porssible resource.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[out]  ResourceList     Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS      Function complete successfully. 
  @retval      EFI_UNSUPPORTED  Function not supported yet. 
**/
EFI_STATUS
EFIAPI
IsaGetPossibleResource (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT    EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  );

/**
  Connect to device protocol to set resources.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[in]   ResourceList     Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaSetResource (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  IN     EFI_ISA_ACPI_RESOURCE_LIST  *ResourceList
  );

/**
  Connect to device protocol to set enable or disable.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[in]   Enable           Enable or disable.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaEnableDevice (
  IN    EFI_ISA_ACPI_PROTOCOL        *This,
  IN    EFI_ISA_ACPI_DEVICE_ID       *Device,
  IN    BOOLEAN                      Enable
  );

/**
  Init DMA controller.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaInitDevice (
  IN    EFI_ISA_ACPI_PROTOCOL        *This,
  IN    EFI_ISA_ACPI_DEVICE_ID       *Device
  );

/**
  Get resource from SIO devices, then create table that IsaBus need  

  @param[in]   This             This IsaAcpi protocol.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
LpcInterfaceInit (
  IN    EFI_ISA_ACPI_PROTOCOL        *This
  );

#endif
