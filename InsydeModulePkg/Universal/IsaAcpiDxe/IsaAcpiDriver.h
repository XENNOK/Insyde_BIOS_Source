/** @file

Declaration for IsaAcpiDriver.c

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

#ifndef _LPC_DRIVER_H_
#define _LPC_DRIVER_H_

#include <IsaAcpi.h>
#include <PostCode.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/PciIo.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>  
#include <Library/PostCodeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define PCI_CLASS_ISA                   0x01  // obsolete 
#define PCI_CLASS_ISA_POSITIVE_DECODE   0x80  // obsolete

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

//
// LPC device private data structure
//
#define LPC_DEV_SIGNATURE  SIGNATURE_32('L', 'P', 'C', 'D')

typedef struct {
  UINTN                         Signature;
  EFI_HANDLE                    Handle;
  EFI_ISA_ACPI_PROTOCOL         IsaAcpi;
  EFI_PCI_IO_PROTOCOL           *PciIo;
} LPC_DEV;

#define LPC_ISA_ACPI_FROM_THIS(a) CR(a, LPC_DEV, IsaAcpi, LPC_DEV_SIGNATURE)

#pragma pack(1)
typedef struct {
  UINT8                         Func;
  UINT8                         Dev;
} ISA_BRIDGE_INFO;
#pragma pack()

/**
  The entry point of the Lpc driver.  

  @param[in]  ImageHandle       Pointer to the loaded image protocol for this driver.
  @param[in]  SystemTable       Pointer to the EFI System Table.

  @retval     EFI_SUCCESS       Function complete successfully.  
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverEntryPoint(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

/**
  Test to see if this driver supports ControllerHandle. 

  @param[in]  This                  Protocol instance pointer.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval EFI_SUCCESS               Success.
  @retval EFI_UNSUPPORTED           This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Start this driver on ControllerHandle. 

  @param[in]  This                  Protocol instance pointer.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval     EFI_SUCCESS           Success.
  @retval     EFI_UNSUPPORTED       This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Start this driver on ControllerHandle. 

  @param[in]  This                  Stop this driver on ControllerHandle.
  @param[in]  Controller            Handle of device to test.
  @param[in]  RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval     EFI_SUCCESS           Success.
  @retval     EFI_UNSUPPORTED       This driver does not support this device. 
**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

#endif
