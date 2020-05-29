/** @file
  PeiSmbus2OnPeiSmbusThunk.c

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

#include <PiPei.h>
#include <Ppi/Smbus.h>
#include <Ppi/Smbus2.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
PeiSmbus2Execute (
  IN CONST  EFI_PEI_SMBUS2_PPI        *This,
  IN        EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN        EFI_SMBUS_DEVICE_COMMAND  Command,
  IN        EFI_SMBUS_OPERATION       Operation,
  IN        BOOLEAN                   PecCheck,
  IN OUT    UINTN                     *Length,
  IN OUT    VOID                      *Buffer
  );

EFI_STATUS
EFIAPI
PeiSmbus2ArpDevice (
  IN CONST  EFI_PEI_SMBUS2_PPI        *This,
  IN        BOOLEAN                   ArpAll,
  IN        EFI_SMBUS_UDID            *SmbusUdid,
  IN OUT    EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  );

EFI_STATUS
EFIAPI
PeiSmbus2GetArpMap (
  IN CONST  EFI_PEI_SMBUS2_PPI    *This,
  IN OUT    UINTN                 *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP  **SmbusDeviceMap
  );

EFI_STATUS
EFIAPI
PeiSmbus2Notify (
  IN CONST EFI_PEI_SMBUS2_PPI              *This,
  IN       EFI_SMBUS_DEVICE_ADDRESS        SlaveAddress,
  IN       UINTN                           Data,
  IN       EFI_PEI_SMBUS_NOTIFY2_FUNCTION  NotifyFunction
  );

//
// Module globals
//
EFI_PEI_SMBUS2_PPI mPeiSmbus2Ppi = {
  PeiSmbus2Execute,
  PeiSmbus2ArpDevice,
  PeiSmbus2GetArpMap,
  PeiSmbus2Notify,
  EFI_PEI_SMBUS2_PPI_GUID
};

EFI_PEI_PPI_DESCRIPTOR     mPeiSmbus2PpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiSmbus2PpiGuid,
  &mPeiSmbus2Ppi
};

/**
  User entry for this PEIM driver.
  
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS PeiSmbus2 PPI is successfully installed.
  @return Others      PeiSmbus2 PPI is not successfully installed.

**/
EFI_STATUS
EFIAPI
PeimInitializePeiSmbus2 (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  //
  // This thunk module can only be used together with a PI PEI core, as we 
  // assume PeiServices Pointer Table can be located in a standard way defined
  // in PI spec.
  //
  ASSERT ((*PeiServices)->Hdr.Revision >= 0x00010000);

  //
  // Developer should make sure PeiSmbus2ToPeiSmbus module is not present. or else, the call chain will form a
  // infinite loop: PeiSmbus2 -> PeiSmbus -> PeiSmbus2 -> .....
  //
  //
  // Publish the variable capability to other modules
  //
  return PeiServicesInstallPpi (&mPeiSmbus2PpiList);

}

/**
  Executes an SMBus operation to an SMBus controller. Returns when either 
  the command has been executed or an error is encountered in doing the operation.

  @param  This          A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  SlaveAddress
  @param  Command,
  @param  Operation,
  @param  PecCheck,
  @param  Length,
  @param  Buffer

  @retval EFI_SUCCESS  
  @retval Others         

**/
EFI_STATUS
PeiSmbus2Execute (
  IN CONST  EFI_PEI_SMBUS2_PPI        *This,
  IN        EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN        EFI_SMBUS_DEVICE_COMMAND  Command,
  IN        EFI_SMBUS_OPERATION       Operation,
  IN        BOOLEAN                   PecCheck,
  IN OUT    UINTN                     *Length,
  IN OUT    VOID                      *Buffer
  )
{
  EFI_STATUS        Status;
  EFI_PEI_SMBUS_PPI *SmbusPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbusPpiGuid,
             0,
             NULL,
             (VOID **)&SmbusPpi
             );
  ASSERT_EFI_ERROR (Status);
  
  return SmbusPpi->Execute (
                         (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                         SmbusPpi,
                         SlaveAddress,
                         Command,
                         Operation,
                         PecCheck,
                         Length,
                         Buffer
                         );
}

/**
  The ArpDevice() function enumerates the entire bus or enumerates a specific 
  device that is identified by SmbusUdid. 

  @param  This          A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  ArpAll,
  @param  SmbusUdid,
  @param  SlaveAddress

  @retval EFI_SUCCESS  
  @retval Others         

**/
EFI_STATUS
EFIAPI
PeiSmbus2ArpDevice (
  IN CONST  EFI_PEI_SMBUS2_PPI        *This,
  IN        BOOLEAN                   ArpAll,
  IN        EFI_SMBUS_UDID            *SmbusUdid,
  IN OUT    EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  )
{
  EFI_STATUS        Status;
  EFI_PEI_SMBUS_PPI *SmbusPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbusPpiGuid,
             0,
             NULL,
             (VOID **)&SmbusPpi
             );
  ASSERT_EFI_ERROR (Status);
  
  return SmbusPpi->ArpDevice (
                     (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                     SmbusPpi,
                     ArpAll,
                     SmbusUdid,
                     SlaveAddress
                     );
}

/**
  The GetArpMap() function returns the mapping of all the SMBus devices 
  that are enumerated by the SMBus host driver. 

  @param  This           
  @param  Length         
  @param  SmbusDeviceMap 
                         
  @retval EFI_SUCCESS  
  @retval Others         

**/
EFI_STATUS
EFIAPI
PeiSmbus2GetArpMap (
  IN CONST  EFI_PEI_SMBUS2_PPI    *This,
  IN OUT    UINTN                 *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP  **SmbusDeviceMap
  )
{
  EFI_STATUS        Status;
  EFI_PEI_SMBUS_PPI *SmbusPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbusPpiGuid,
             0,
             NULL,
             (VOID **)&SmbusPpi
             );
  ASSERT_EFI_ERROR (Status);

  return SmbusPpi->GetArpMap (
                     (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                     SmbusPpi,
                     Length,
                     SmbusDeviceMap
                     );
}
/**
  The Notify() function registers all the callback functions to allow the 
  bus driver to call these functions when the SlaveAddress/Data pair happens.

  @param  This          
  @param  SlaveAddress  
  @param  Data          
  @param  NotifyFunction

  @retval EFI_SUCCESS
  @retval Others
  
**/
EFI_STATUS
EFIAPI
PeiSmbus2Notify (
  IN CONST EFI_PEI_SMBUS2_PPI              *This,
  IN       EFI_SMBUS_DEVICE_ADDRESS        SlaveAddress,
  IN       UINTN                           Data,
  IN       EFI_PEI_SMBUS_NOTIFY2_FUNCTION  NotifyFunction
  )
{
  EFI_STATUS        Status;
  EFI_PEI_SMBUS_PPI *SmbusPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbusPpiGuid,
             0,
             NULL,
             (VOID **)&SmbusPpi
             );
  ASSERT_EFI_ERROR (Status);

  return SmbusPpi->Notify (
                     (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                     SmbusPpi,
                     SlaveAddress,
                     Data,
                     (EFI_PEI_SMBUS_NOTIFY_FUNCTION) NotifyFunction
                     );
}

