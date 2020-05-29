/** @file

Declaration file for SioMainDxe.c

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SIO_NCT6683DLU_DXE_H_
#define _SIO_NCT6683DLU_DXE_H_

#include <Include/SioNct6683dluReg.h>
#include <Sio/SioResource.h>
#include <SioHiiResource.h>
#include <SioHiiResourceNvData.h>
#include <Protocol/IsaPnpDevice.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/AcpiSupport.h>
#include <Library/S3BootScriptLib.h>
#include <Library/SioLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiLib/UefiLibInternal.h>

//[-start-131231-IB12970047-add]//
#include <Library/IoLib.h>
//[-end-131231-IB12970047-add]//

//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define EFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (EFI_SIGNATURE_32 (A, B, C, D) | ((UINT64) (EFI_SIGNATURE_32 (E, F, G, H)) << 32))

//
// EFI_FIELD_OFFSET - returns the byte offset to a field within a structure
//
#define EFI_FIELD_OFFSET(TYPE,Field) ((UINTN)(&(((TYPE *) 0)->Field)))

/**
  Write data to the specific register.  

  @param[in]  Index       SIO index port.
  @param[in]  Data        The 8 bits value write to index port.
**/
VOID
IDW8 (
  IN UINT8  Index,
  IN UINT8  Data
  );

/**
  Read data to the specific register.  

  @param[in]   Index                SIO index port.

  @retval      UINT8                Read value from SIO. 
**/
UINT8
IDR8 (
  IN UINT8  Index
  );

/**
  Writes an I/O port using the CPU I/O Protocol.  

  @param[in]  Port        I/O Port to write.
  @param[in]  Value       The 8 bits value write to port.
**/
VOID
SioWrite8 (
  IN UINT16  Port,
  IN UINT8   Value
  );

/**
  Writes an I/O port using the CPU I/O Protocol to get value.  

  @param[in]  Port                 I/O Port to write.

  @retval     Value                Get data from SIO. 
**/
UINT8
SioRead8 (
  IN UINT16  Port
  );

/**
  Writes an I/O port using the CPU I/O Protocol to get value.  

  @param[in]   Port                 I/O Port to write.

  @retval      Value                Get data from SIO. 
**/
UINT16
SioRead16 (
  IN UINT16  Port
  );

/**
  Transfer from SIO_DEVICE_LIST_TABLE to EFI_ISA_DEVICE_RESOURCE for using of device protocol

  @param[in]  *DeviceList            SIO_DEVICE_LIST_TABLE structure.
  @param[in]  *DeviceResource        EFI_ISA_DEVICE_RESOURCE structure.

  @retval EFI_SUCCESS                The function completed successfully.
**/
EFI_STATUS
DeviceListToDeviceResource (
  IN SIO_DEVICE_LIST_TABLE        *DeviceList,
  IN EFI_ISA_DEVICE_RESOURCE      *DeviceResource
  );

/**
  Install Device protocol from PCD structure.
**/
VOID
InstallEnabledDeviceProtocol (
  VOID
  );

/**
  To get correct SIO data. 

  @retval EFI_SUCCESS           Found SIO data. 
  @retval EFI_NOT_FOUND         Not found. 
**/
EFI_STATUS
CheckDevice (  
  VOID
  ); 

/**
  Check ID of SIO whether it's correct

  @param[in]  *DeviceTable       SIO_DEVICE_LIST_TABLE from PCD structure.

  @retval     IndexPort          return value as zero, it means "not match".
                                 return value is not zero, it means "match".
**/
UINT16
CheckConfig (
  IN SIO_DEVICE_LIST_TABLE      *DeviceTable
  ); 

/**
  Create NVs area first, then we'll get PCD data to update it and SSDT table.

  @param[in]   SioAcpiTable           SSDT table.
  
  @retval      EFI_SUCCESS            Function complete successfully. 
**/
EFI_STATUS
UpdateSioMbox (
  IN EFI_ACPI_DESCRIPTION_HEADER      *SioAcpiTable
  );

/**
  Find SSDT table from RAW section of DXE driver, then publish it to ACPI table.

  @param[in]  Event           Event whose notification function is being invoked.
  @param[in]  Context         Pointer to the notification function's context.
**/
VOID
UpdateSsdt (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  );

/**
  Convert a single number to character.
  It assumes the input Char is in the scope of L'0' ~ L'9' and L'A' ~ L'F'.
  
  @param[in] Num    The input char which need to change to a hex number.
**/
UINT16
UintToChar (
  IN UINT8                           Num
  );

/**
  Create SCU by SIO and update value to PCD data.

  @retval      EFI_SUCCESS            Function complete successfully. 
**/
EFI_STATUS
SioScu (
  VOID
  );

//[-start-131231-IB12970047-add]//
BOOLEAN
KbcExistence (
  );

BOOLEAN
MouseExistence (
  );

BOOLEAN
WriteCmd (
  UINT8  Cmd
  );

BOOLEAN
WriteData (
  UINT8  Data
  );

BOOLEAN
WriteCmd2Kbd (
  UINT8  Cmd
  );

BOOLEAN
WaitObf (
  );
 
BOOLEAN
WaitObe(
  );

BOOLEAN
WaitIbe(
  );
//[-end-131231-IB12970047-add]//

#endif
