/** @file

SIO main code, it include create SCU, install device protocol, init in DXE stage

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

#include "SioInitDxe.h"

EFI_CPU_IO2_PROTOCOL  *mCpuIo;
SIO_DEVICE_LIST_TABLE *mTablePtr;

extern EFI_SIO_TABLE mSioTable[];

//[-start-140116-IB12970054-modify]//
extern UINT16 mSioCfgPortListSize;
//[-end-140116-IB12970054-modify]//

extern EFI_INSTALL_DEVICE_FUNCTION mDeviceFunction[];
extern EFI_SIO_RESOURCE_FUNCTION mSioResourceFunction[];

/**
  The entry point of the SIO driver.  

  @param [in] ImageHandle            A handle for the image that is initializing this driver
  @param [in] SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Function complete successfully. 
**/
EFI_STATUS
EFIAPI
SioDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                        Status;

  mTablePtr = (SIO_DEVICE_LIST_TABLE *)PcdGetPtr (PcdSioDummyCfg);
  
  //
  // must locate CPUIO in here for test cfg port
  //
  Status = gBS->LocateProtocol (
                  &gEfiCpuIo2ProtocolGuid, 
                  NULL, 
                  (VOID **) &mCpuIo
                  );
  
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Get SIO from PCD, if it fail, we don't install SIO DXE driver
  //
  Status = CheckDevice ();  
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  InstallEnabledDeviceProtocol ();

  return Status;
}

/**
  To get correct SIO data. 

  @retval EFI_SUCCESS           Found SIO data. 
  @retval EFI_NOT_FOUND         Not found. 
**/
EFI_STATUS
CheckDevice (
  VOID
  ) 
{
  SIO_DEVICE_LIST_TABLE             *PcdPointer;

  PcdPointer = mTablePtr;

  //
  // Calculate the number of non-zero entries in the table
  //
  while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
    if (PcdPointer->TypeH == SIO_ID1) {
      return EFI_SUCCESS;          
    } 
    PcdPointer++;
  }
  
  return EFI_NOT_FOUND;
}

/**
  Write data to the specific register.  

  @param[in]  Index       SIO index port.
  @param[in]  Data        The 8 bits value write to index port.
**/
VOID
IDW8 (
  IN UINT8  Index,
  IN UINT8  Data
  )
{
  SioWrite8 (
    mSioResourceFunction->SioCfgPort, 
    Index
    );
  SioWrite8 (
    mSioResourceFunction->SioCfgPort+1, 
    Data
    );
}

/**
  Read data to the specific register.  

  @param[in]   Index                SIO index port.

  @retval      UINT8                Read value from SIO. 
**/
UINT8
IDR8 (
  IN UINT8  Index
  )  
{
  BOOLEAN FlagTemp;

  FlagTemp = mSioResourceFunction->ScriptFlag;
  mSioResourceFunction->ScriptFlag = FALSE;
  
  SioWrite8 (
    mSioResourceFunction->SioCfgPort, 
    Index
    );
  mSioResourceFunction->ScriptFlag = FlagTemp;

  return SioRead8((mSioResourceFunction->SioCfgPort)+1);
}

/**
  Writes an I/O port using the CPU I/O Protocol.  

  @param[in]  Port        I/O Port to write.
  @param[in]  Value       The 8 bits value write to port.
**/
VOID
SioWrite8 (
  IN UINT16  Port,
  IN UINT8   Value
  )
{
  mCpuIo->Io.Write (
           mCpuIo, 
           EfiCpuIoWidthUint8, 
           Port, 
           1, 
           &Value
           ); 
}

/**
  Writes an I/O port using the CPU I/O Protocol to get value.   

  @param[in]  Port                 I/O Port to write.

  @retval     Value                Get data from SIO. 
**/
UINT8
SioRead8 (
  IN UINT16  Port
  )
{
  UINT8   Value;

  mCpuIo->Io.Read (
           mCpuIo, 
           EfiCpuIoWidthUint8, 
           Port, 
           1, 
           &Value
           );
  return Value;
}

/**
    Writes an I/O port using the CPU I/O Protocol to get value.    

  @param[in]   Port                 I/O Port to write.

  @retval      Value                Get data from SIO. 
**/
UINT16
SioRead16 (
  IN UINT16  Port
  )
{
  UINT8 Value;

  mCpuIo->Io.Read (
            mCpuIo, 
            EfiCpuIoWidthUint16, 
            Port, 
            1, 
            &Value
            );
  return Value;
}

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
  )
{
  DeviceResource->IoPort = UpsideDown(DeviceList->DeviceBaseAdr);
  DeviceResource->IrqNumber = DeviceList->DeviceIrq;
  DeviceResource->DmaChannel = DeviceList->DeviceDma;

  return EFI_SUCCESS;
}

/**
  Install Device protocol from PCD structure.
**/
VOID
InstallEnabledDeviceProtocol (
  VOID
  )
{
  EFI_STATUS                         Status;
  UINT8                              Index;
  SIO_DEVICE_LIST_TABLE              *PcdPointer;

  Status       = EFI_NOT_FOUND;
  
  Index = 0;
  while (mDeviceFunction[Index].Device != NULL_ID) {
    PcdPointer   = mTablePtr;
    while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
      if ((PcdPointer->TypeH == SIO_ID1) && (PcdPointer->Device == mDeviceFunction[Index].Device) 
         && (PcdPointer->DeviceInstance == mDeviceFunction[Index].Instance)) {   
        //
        // if the device is enable, then install it
        //
        if (PcdPointer->DeviceEnable != FALSE) {
          if (mDeviceFunction[Index].InstallDeviceProtocol != NULL) {
            Status = mDeviceFunction[Index].InstallDeviceProtocol (
                       PcdPointer
                       ); 
          }
        }
        break;
      } 
      PcdPointer++;
    }
    Index++;
  }  
}

/**
  Convert a single number to character.
  It assumes the input Char is in the scope of L'0' ~ L'9' and L'A' ~ L'F'.
  
  @param[in] Num    The input char which need to change to a hex number.
**/
UINT16
UintToChar (
  IN UINT8                           Num
  )
{
  UINT8      Index;
  UINT16     Value;
  UINT8      TempValue;

  Value = 0;

  for (Index = 0; Index < (sizeof(UINT16)/sizeof(UINT8)); Index++) {
    TempValue = Num & 0xF;
    if ((TempValue >= 0) && (TempValue <= 9)) {
      Value = (Value << 8) + ((UINT16) (TempValue + L'0'));
    }

    if ((TempValue >= 0xA) && (TempValue <= 0xF)) {
      Value = (Value << 8) + ((UINT16) (TempValue + L'A' - 0xA ));
    }
    Num = Num >> 4;
  }
  
  return Value;
}

/**
  Word to chage place for high byte, low byte
   
  @param[in] Value    The number need to chage.

  @retval    Value    Function complete successfully. 
**/
UINT16
UpsideDown (
  IN UINT16                           Value
  )
{
  UINT16     High;
  UINT8      Low;

  High = Value & 0xFF;
  Low = Value >> 8;

  Value = (High << 8) + Low;

  return Value;
}
