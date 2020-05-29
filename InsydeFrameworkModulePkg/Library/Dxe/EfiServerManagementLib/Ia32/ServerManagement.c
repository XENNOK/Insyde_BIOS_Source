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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++
Copyright (c) 1996 - 2005, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:

  Servermanagement.c

Abstract:

  Light weight lib to support EFI 2.0 Server Management drivers.

--*/

#include "Tiano.h"
#include "EfiServerManagement.h"
#include "EfiMgmtModeRuntimeLib.h"

#include EFI_PROTOCOL_DEFINITION (SmComLayer)
#include EFI_PROTOCOL_DEFINITION (GenericElog)
#include EFI_PROTOCOL_DEFINITION (GenericSensor)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)


//
// Module Globals
//
EFI_SM_COM_LAYER_PROTOCOL *mComProtocol             = NULL;
VOID                      *mComLayerRegistration    = NULL;
EFI_SM_ELOG_PROTOCOL      *mGenericElogProtocol     = NULL;
VOID                      *mGenericElogRegistration = NULL;
EFI_EVENT                 mComLayerEvent;
EFI_SM_SENSOR_PROTOCOL    *mGenericSensorProtocol     = NULL;
VOID                      *mGenericSensorRegistration = NULL;

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Calculate the space size of a device path.

Arguments:

  DevicePath  - A specified device path

Returns:

  The size.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!EfiIsDevicePathEnd (DevicePath)) {
    DevicePath = EfiNextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
/*++

Routine Description:

  Function is used to append a Src1 and Src2 together.

Arguments:

  Src1  - A pointer to a device path data structure.
  Src2  - A pointer to a device path data structure.

Returns:

  A pointer to the new device path is returned.
  NULL is returned if space for the new device path could not be allocated from pool.
  It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

--*/
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINTN                     Size1;
  UINTN                     Size2;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SecondDevicePath;

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL
  //
  Size1 = DevicePathSize (Src1);
  Size2 = DevicePathSize (Src2);
  Size  = Size1 + Size2;

  if (Size1 != 0 && Size2 != 0) {
    Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **) &NewDevicePath);

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  gBS->CopyMem (NewDevicePath, Src1, Size1);

  //
  // Over write Src1 EndNode and do the copy
  //
  if (Size1 != 0) {
    SecondDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) ((CHAR8 *) NewDevicePath + (Size1 - sizeof (EFI_DEVICE_PATH_PROTOCOL)));
  } else {
    SecondDevicePath = NewDevicePath;
  }

  gBS->CopyMem (SecondDevicePath, Src2, Size2);

  return NewDevicePath;
}

VOID
EFIAPI
EfiGetFullDriverPath (
  IN  EFI_HANDLE                  ImageHandle,
  IN  EFI_SYSTEM_TABLE            *SystemTable,
  OUT EFI_DEVICE_PATH_PROTOCOL    **CompleteFilePath
  )
/*++

Routine Description:

  Function is used to get the full device path for this driver.

Arguments:

  ImageHandle        - The loaded image handle of this driver.
  SystemTable        - The pointer of system table.
  CompleteFilePath   - The pointer of returned full file path

Returns:

  none

--*/
{
  EFI_STATUS                Status;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL  *ImageDevicePath;

  gST = SystemTable;
  gBS = gST->BootServices;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  &LoadedImage
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->HandleProtocol (
                  LoadedImage->DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID *) &ImageDevicePath
                  );
  ASSERT_EFI_ERROR (Status);

  *CompleteFilePath = EfiAppendDevicePath (
                        ImageDevicePath,
                        LoadedImage->FilePath
                        );

  return ;
}

VOID
EFIAPI
ComLayerNotificationFunction (
  EFI_EVENT       Event,  // GC_TODO: add IN/OUT modifier to Event
  VOID            *Context  // GC_TODO: add IN/OUT modifier to Context
  )
/*++

  Routine Description:
    This function is called whenever an instance of COM protocol is created.  When the function is notified
    it initializes the module global data.

  Arguments:
    EFI_EVENT - This is used only for EFI compatability
    VOID      - This is used only for EFI compatability

  Returns:
    EFI_SUCCESS - if the COM layer protocol is found and the module global data is initialized
    EFI_NOT_FOUND - if the COM layer could not be found

--*/
// GC_TODO:    Event - add argument and description to function comment
// GC_TODO:    Context - add argument and description to function comment
{
  EFI_STATUS  Status;

  Status = EfiLocateProtocolInterface (
            &gEfiSmComLayerProtocolGuid,
            NULL,
            &mComProtocol
            );

  if (EFI_ERROR (Status)) {
    mComProtocol = NULL;
  }
}

EFI_STATUS
EfiInitializeComLayer (
  VOID
  )
/*++

  Routine Description:
    The function will set up a notification on the COM layer protocol.  This function is required to be called prior
    to utilizing the COM layer from within this library.

  Arguments:

  Returns:
    EFI_SUCCESS - after the notificication has been setup.

--*/
{
  EFI_EVENT Event;

  EfiRegisterProtocolCallback (
    ComLayerNotificationFunction,
    NULL,
    &gEfiSmComLayerProtocolGuid,
    EFI_TPL_CALLBACK,
    &mComLayerRegistration,
    &Event
    );

  //
  // Signal an event in case the protocol has already been installed
  //
  EfiSignalProtocolEvent (Event);

  return EFI_SUCCESS;
}

EFI_STATUS
EfiSmLibGetComControllerInfo (
  IN  UINT16                             InstanceNumber,
  OUT UINT32                             *NumInterfaces,
  OUT EFI_SM_CTRL_INFO                   *ControllerInfo,
  OUT EFI_SM_COM_ADDRESS                 *ControllerAddress
  )
/*++

  Routine Description:
    This function returns the server management controller information of the InstanceNumber that
    is passed in.  It also returns the number of contollers that are currently in the system.

  Arguments:
    InstanceNumber - is the identifying number of the controller that the caller requires information.
    NumInterfaces - the number of controllers that are found in the system.
    ControllerInfo - information returned about the numbered controller
    ControllerAddress - address of the communication controller.

  Returns:
    EFI_SUCCESS - if the controller's information is passed back to the caller
    EFI_NOT_AVAILABLE_YET - if the COM layer has not been initialized.
    EFI_NOT_FOUND - if the number of the controller does not exist.
    EFI_NO_RESPONSE - if the controller does not respond to the call for information.
    EFI_NOT_STARTED - if the controller hasn't started yet.

--*/
{
  EFI_STATUS  Status;
  UINT16      Count;

  Status  = EFI_SUCCESS;
  Count   = 0;

  ASSERT (NumInterfaces);
  ASSERT (ControllerInfo);
  ASSERT (ControllerAddress);

  if (mComProtocol == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }
  //
  // Get the information on the InstanceNumber controller
  //
  Status = mComProtocol->SmGetComCtrlInfo (
                          mComProtocol,
                          InstanceNumber,
                          ControllerInfo,
                          ControllerAddress
                          );

  //
  // Count the number of controllers in the system
  //
  while (mComProtocol->SmGetComCtrlInfo (mComProtocol, Count, NULL, NULL) == EFI_SUCCESS) {
    Count++;
  }

  *NumInterfaces = Count;
  return Status;
}

EFI_STATUS
EfiSmLibSendComData (
  IN  UINT16                    Instance,
  IN  EFI_SM_PROTOCOL_TYPE      ComProtocol,
  IN  EFI_SM_COM_ADDRESS        *SourceAddress,
  IN  EFI_SM_COM_ADDRESS        *TargetAddress,
  IN  UINT8                     *Data,
  IN OUT UINT32                 *Size,
  IN  UINT32                    *ReceiveKey
  )
/*++

  Routine Description:
    This function will send data from a source controller to a target controller.

  Arguments:
    Instance - the instance number of the server management COM controller.
    ComProtocol - the communication protocol for sending the data either Ipmi, Tcp, Udp, or Icmp.
    SourceAddress - the source address of the data to be sent.
    TargetAddress - the target address of the data to be sent.
    Data - pointer of data to be sent to the target.
    Size - size of the data to be sent.  On output it contains the number of bytes actually sent.
    ReceiveKey - this is a key required to receive data.

  Returns:
    EFI_SUCCESS - if the controller was found
    EFI_INVALID_PARAMETER - if the ChannelTypes do not match and if the Size is zero.
    EFI_OUT_OF_RESOURCES - if the internal buffer for the EfiSmBmc MediaType is full.
    EFI_UNSUPPORTED - if the Media type entered is not EfiSmBmc.
    EFI_DEVICE_ERROR - if the controller does not respond or responds with an error.

--*/
// GC_TODO:    EFI_NOT_AVAILABLE_YET - add return value to function comment
{
  if (mComProtocol == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  return mComProtocol->SmSendComData (
                        mComProtocol,
                        Instance,
                        ComProtocol,
                        SourceAddress,
                        TargetAddress,
                        Data,
                        Size,
                        ReceiveKey
                        );
}

EFI_STATUS
EfiSmLibReceiveComData (
  IN UINT32                     SendKey,
  IN UINT8                      *Data,
  IN OUT UINT32                 *Size,
  IN EFI_SM_CALLBACK            *SignalData,
  IN BOOLEAN                    Blocking,
  IN OUT BOOLEAN                *ReleaseKey
  )
/*++

  Routine Description:
    This function receives data from a remote controller to the localcontroller.  This function
    should be called in the same mode as the send function for the same ReceiveKey (i.e. SMM, Physical, Virtual).

  Arguments:
    SendKey - this is required to receive data.
    Data - pointer to the data to be received by the target
    Size - size of the data to be received.  On output it contains the number of bytes actually received.
    SignalData - callback structure that indicates the receipt of the data
    Blocking - if set the the read is blocking
    ReleaseKey - if it is true then the key is released

  Returns:
    EFI_SUCCESS - if the controller was found and data was initially received.
    EFI_PROTOCOL_ERROR - if the COM controller discovered a protocol error.
    EFI_DEVICE_ERROR - if the COM controller had an device error.
    EFI_BUFFER_TOO_SMALL - if the buffer for the data to be received is too small. If this is returned then
                           the Size will contain the number of bytes required to complete the transaction.
    EFI_INVALID_PARAMETER - if the SendKey is invalid or if the release key is not valid.
    EFI_NOT_AVAILABLE_YET - if the COM layer is not loaded or the library was not initialized.
    EFI_UNSUPPORTED - if the DataType is SmIcmbData

--*/
{
  if (mComProtocol == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  return mComProtocol->SmReceiveComData (
                        mComProtocol,
                        SendKey,
                        Data,
                        Size,
                        SignalData,
                        Blocking,
                        ReleaseKey
                        );
}

VOID
EFIAPI
GenericElogNotificationFunction (
  EFI_EVENT       Event,  // GC_TODO: add IN/OUT modifier to Event
  VOID            *Context  // GC_TODO: add IN/OUT modifier to Context
  )
/*++

  Routine Description:
    This function is called whenever an instance of ELOG protocol is created.  When the function is notified
    it initializes the module global data.

  Arguments:
    EFI_EVENT - This is used only for EFI compatability
    VOID      - This is used only for EFI compatability

  Returns:
    EFI_SUCCESS - if the ELOG protocol is found and the module global data is initialized
    EFI_NOT_FOUND - if the ELOG protocol could not be found

--*/
// GC_TODO:    Event - add argument and description to function comment
// GC_TODO:    Context - add argument and description to function comment
{
  EFI_STATUS  Status;

  Status = EfiLocateProtocolInterface (&gEfiGenericElogProtocolGuid, NULL, &mGenericElogProtocol);
  ASSERT_EFI_ERROR (Status);
}

EFI_STATUS
EfiInitializeGenericElog (
  VOID
  )
/*++

  Routine Description:
    The function will set up a notification on the ELOG protocol.  This function is required to be called prior
    to utilizing the ELOG protocol from within this library.

  Arguments:

  Returns:
    EFI_SUCCESS - after the notificication has been setup.

--*/
{
  EFI_EVENT Event;

  EfiRegisterProtocolCallback (
    GenericElogNotificationFunction,
    NULL,
    &gEfiGenericElogProtocolGuid,
    EFI_TPL_CALLBACK,
    &mGenericElogRegistration,
    &Event
    );

  EfiSignalProtocolEvent (Event);

  return EFI_SUCCESS;
}

EFI_STATUS
EfiSmSetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  )
/*++

  Routine Description:
    This function sends event log data to the destination such as LAN, ICMB, BMC etc.

  Arguments:
    ElogData - is a pointer to the event log data that needs to be recorded
    DataType - type of elog data that is being recorded.  The elog is redirected based on this
               parameter.
    AlertEvent - is an indication that the input data type is an alert.  The underlying
                 drivers need to decide if they need to listen to the DataType and send it on
                 an appropriate channel as an alert use of the information.
    DataSize - is the size of the data to be logged
    RecordId -is the array of record IDs sent by the target.  This can be used to retieve the
              records or erase the records.

  Returns:
    EFI_SUCCESS - if the data was logged.
    EFI_INVALID_PARAMETER - if the DataType is >= EfiSmElogMax
    EFI_OUT_OF_RESOURCES - if the DataSize is larger than the elog temp buffer and we cannot log the record
    EFI_NOT_FOUND - the event log target was not found
    EFI_PROTOCOL_ERROR - there was a data formatting error

--*/
{
  return mGenericElogProtocol->SetEventLogData (
                                mGenericElogProtocol,
                                ElogData,
                                DataType,
                                AlertEvent,
                                DataSize,
                                RecordId
                                );
}

EFI_STATUS
EfiSmGetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId
  )
/*++

  Routine Description:
    This function gets event log data from the destination dependant on the DataType.  The destination
    can be a remote target such as LAN, ICMB, IPMI, or a FV.  The ELOG redir driver will resolve the
    destination.

  Arguments:
    ElogData - a pointer to the an event log data buffer to contain the data to be retrieved.
    DataType - this is the type of elog data to be gotten.  Elog is redirected based upon this
               information.
    DataSize - this is the size of the data to be retrieved.
    RecordId - the RecordId of the next record.  If ElogData is NULL, this gives the RecordId of the first
               record available in the database with the correct DataSize.  A value of 0 on return indicates
               that it was last record if the Status is EFI_SUCCESS.

  Returns:
    EFI_SUCCESS - if the event log was retrieved successfully.
    EFI_NOT_FOUND - if the event log target was not found.
    EFI_NO_RESPONSE - if the event log target is not responding.  This is done by the redir driver.
    EFI_INVALID_PARAMETER - DataType or another parameter was invalid.
    EFI_BUFFER_TOO_SMALL -the ElogData buffer is too small to be filled with the requested data.

--*/
{
  return mGenericElogProtocol->GetEventLogData (
                                mGenericElogProtocol,
                                ElogData,
                                DataType,
                                DataSize,
                                RecordId
                                );
}

EFI_STATUS
EfiSmEraseEventlogData (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  )
/*++

  Routine Description:
    This function erases the event log data defined by the DataType.  The redir driver associated with
    the DataType resolves the path to the record.

  Arguments:
    DataType - the type of elog data that is to be erased.
    RecordId - the RecordId of the data to be erased.  If RecordId is NULL,  all records in the
               database are erased if permitted by the target.  RecordId will contain the deleted
               RecordId on return.

  Returns:
    EFI_SUCCESS - the record or collection of records were erased.
    EFI_NOT_FOUND - the event log target was not found.
    EFI_NO_RESPONSE - the event log target was found but did not respond.
    EFI_INVALID_PARAMETER - one of the parameters was invalid.

--*/
{
  return mGenericElogProtocol->EraseEventlogData (
                                mGenericElogProtocol,
                                DataType,
                                RecordId
                                );
}

EFI_STATUS
EfiSmActivateEventLog (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  )
/*++

  Routine Description:
    This function enables or disables the event log defined by the DataType.

  Arguments:
    DataType - the type of elog data that is being activated.
    EnableElog - enables or disables the event log defined by the DataType.  If it is NULL
                 it returns the current status of the DataType log.
    ElogStatus - is the current status of the Event log defined by the DataType.  Enabled is
                 TRUE and Disabled is FALSE.

  Returns:
    EFI_SUCCESS - if the event log was successfully enabled or disabled.
    EFI_NOT_FOUND - the event log target was not found.
    EFI_NO_RESPONSE - the event log target was found but did not respond.
    EFI_INVALID_PARAMETER - one of the parameters was invalid.

--*/
{
  return mGenericElogProtocol->ActivateEventLog (
                                mGenericElogProtocol,
                                DataType,
                                EnableElog,
                                ElogStatus
                                );
}

VOID
EFIAPI
GenericSensorNotificationFunction (
  EFI_EVENT       Event,  // GC_TODO: add IN/OUT modifier to Event
  VOID            *Context  // GC_TODO: add IN/OUT modifier to Context
  )
/*++

  Routine Description:
    This function is called whenever an instance of SENSOR protocol is created.  When the function is notified
    it initializes the module global data.

  Arguments:
    EFI_EVENT - This is used only for EFI compatability
    VOID      - This is used only for EFI compatability

  Returns:
    EFI_SUCCESS - if the sensor protocol is found and the module global data is initialized
    EFI_NOT_FOUND - if the sensor protocol could not be found

--*/
// GC_TODO:    Event - add argument and description to function comment
// GC_TODO:    Context - add argument and description to function comment
{
  EFI_STATUS  Status;

  Status = EfiLocateProtocolInterface (&gEfiGenericSensorProtocolGuid, NULL, &mGenericSensorProtocol);
  ASSERT_EFI_ERROR (Status);
}

EFI_STATUS
EfiInitializeGenericSensor (
  VOID
  )
/*++

  Routine Description:
    The function will set up a notification on the sensor protocol.  This function is required to be called prior
    to utilizing the sensor protocol from within this library.

  Arguments:

  Returns:
    EFI_SUCCESS - after the notificication has been setup.

--*/
{
  EFI_EVENT Event;

  EfiRegisterProtocolCallback (
    GenericSensorNotificationFunction,
    NULL,
    &gEfiGenericSensorProtocolGuid,
    EFI_TPL_CALLBACK,
    &mGenericSensorRegistration,
    &Event
    );

  EfiSignalProtocolEvent (Event);

  return EFI_SUCCESS;
}

EFI_STATUS
EfiSmLibReadSensorInfo (
  IN EFI_SM_SENSOR_TYPE    SensorType,
  IN UINTN                 EventType,
  OUT UINTN                *SensorNumber,
  OUT UINTN                *SensorCount
  )
/*++

Routine Description:

    This routine retrieves the sensor information for a specific combination of
    Sensor Type and Event Type.

Arguments:
    SensorType   - ID that represent the type of the SENSOR. Eg CPU, Voltage etc. (Section D.3)
    EventType    - Type of event that is represented by this SENSOR. (Section D.1)
    SensorNumber - Sensor Number that is unique for this combination (SensorType + EventType)
    SensorCount  - Number of SENSORs that belong to the above combination.

Returns:
    EFI_NOT_FOUND - the sensor could not be found
    EFI_SUCCESS - the sensor information requested was retrieved

--*/
{
  return mGenericSensorProtocol->ReadSensorInfo (
                                  mGenericSensorProtocol,
                                  SensorType,
                                  EventType,
                                  SensorNumber,
                                  SensorCount
                                  );
}

EFI_STATUS
EfiSmLibReadSensorStatus (
  IN UINTN               SensorNumber,
  IN UINTN               SensorInstance,
  IN  UINT64             StateBitMask,
  OUT UINT64             *StateBitStatus
  )
/*++

Routine Description:

    This routine retrieves the Sensor Status for a specific sensor represented by
    Sensor Number. This sensor number can be retrieved using EfiReadSensorInfo().

Arguments:

    SensorNumber   - Sensor Number retrieved using EfiReadSensorInfo() and reprsents a
                    specific combination of SENSOR TYPE & EVENT TYPE.
    SensorInstance - Instance of sensor given by Sensor Number.
    StateBitMask   - BIT MASK for the Status Retrivel.
    StateBitStatus - Status corresponding to the BIT Mask.

Returns:
    EFI_NOT_FOUND - if the sensor could not be found in the system.
    EFI_NOT_READY - if the sensor is not ready or otherwise busy.
    EFI_SUCCESS - if the sensor status was found and returned

--*/
{
  return mGenericSensorProtocol->ReadSensorStatus (
                                  mGenericSensorProtocol,
                                  SensorNumber,
                                  SensorInstance,
                                  StateBitMask,
                                  StateBitStatus
                                  );
}

EFI_STATUS
EfiSmLibRearmSensor (
  IN UINTN                       SensorNumber,
  IN UINTN                       SensorInstance,
  IN  UINT64                     RearmBitMask,
  OUT UINT64                     *RearmBitStatus
  )
/*++

Routine Description:

    This routine Re-Arms the Sensor for a specific sensor represented by
    Sensor Number. This sensor number can be retrieved using EfiReadSensorInfo().

Arguments:

    SensorNumber    - Sensor Number retrieved using EfiReadSensorInfo() and reprsents a
                      specific combination of SENSOR TYPE & EVENT TYPE.
    SensorInstance  - Instance of sensor given by Sensor Number.
    RearmBitMask    - BIT MASK for the Re-Arming
    RearmBitStatus  - Status following the Re-Arm request for masked bits only.

Returns:
    EFI_NOT_FOUND - if the sensor that is to be rearmed cannot be found in the system.
    EFI_SUCCESS - if the sensor was rearmed.
    EFI_NOT_READY - if the sensor is not ready or otherwise busy.

--*/
{
  return mGenericSensorProtocol->RearmSensor (
                                  mGenericSensorProtocol,
                                  SensorNumber,
                                  SensorInstance,
                                  RearmBitMask,
                                  RearmBitStatus
                                  );
}

EFI_STATUS
EfiSmLibReadSensorData (
  IN UINTN                                SensorNumber,
  IN UINTN                                SensorInstance,
  OUT EFI_SENSOR_THRESHOLD_STRUCT         *SensorThreshold,
  OUT EFI_SENSOR_READING_STRUCT           *SensorReading,
  OUT UINT8                               *SensorDataUnit
  )
/*++

Routine Description:

    This routine retrieves the sensor Data for a specific combination of
    Sensor Type and Event Type.

Arguments:
    SensorNumber    - Sensor Number retrieved using EfiReadSensorInfo() and reprsents a
                      specific combination of SENSOR TYPE & EVENT TYPE.
    SensorInstance  - Instance of sensor given by Sensor Number.
    SensorThreshold - Sensor Threshold Structure that contains all the necessary thresholds.
    SensorReading   - Sensor Reading Structiure that gives the nominal reading and Sensor ID.
    SensorDataUnit  - Unit in which Sensor Data Threshold & Nominal reading is specified.

Returns:
    EFI_NOT_FOUND - if the sensor could not be found in the system.
    EFI_SUCCESS - if the sensor data was read and returned.

--*/
{
  return mGenericSensorProtocol->ReadSensorData (
                                  mGenericSensorProtocol,
                                  SensorNumber,
                                  SensorInstance,
                                  SensorThreshold,
                                  SensorReading,
                                  SensorDataUnit
                                  );
}
