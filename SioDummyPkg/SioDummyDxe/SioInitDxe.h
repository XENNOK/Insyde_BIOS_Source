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

#ifndef _SIO_DUMMY_DXE_H_
#define _SIO_DUMMY_DXE_H_

#include <Include/SioDummyReg.h>
#include <Sio/SioResource.h>
#include <Protocol/IsaPnpDevice.h>
#include <Protocol/CpuIo2.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiLib/UefiLibInternal.h>

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
  Convert a single number to character.
  It assumes the input Char is in the scope of L'0' ~ L'9' and L'A' ~ L'F'.
  
  @param[in] Num    The input char which need to change to a hex number.
**/
UINT16
UintToChar (
  IN UINT8                           Num
  );

/**
  Word to chage place for high byte, low byte
   
  @param[in] Value    The number need to chage.

  @retval    Value    Function complete successfully. 
**/
UINT16
UpsideDown (
  IN UINT16                           Value
  );

#endif
