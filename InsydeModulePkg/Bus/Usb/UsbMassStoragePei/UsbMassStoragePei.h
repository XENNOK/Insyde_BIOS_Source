/** @file
  Usb Mass Storage Peim definition

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_USB_MASS_STORAGE_PEIM_H
#define _PEI_USB_MASS_STORAGE_PEIM_H

#include <PiPei.h>
#include <Ppi/Stall.h>
#include <Ppi/Usb3Io.h>
#include <Ppi/BlockIo.h>
#include <Ppi/H2OBlockIoPei.h>

#include <IndustryStandard/Atapi.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#define MAXSENSEKEY             5

#define USBFLOPPY               1
#define USBFLOPPY2              2 // for those that use ReadCapacity(0x25) command to retrieve media capacity
#define USBCDROM                3

#define MAX_INIT_MEDIA_RETRY    5

#define MASS_STORAGE_CLASS      0x08
#define BOT_INTERFACE_PROTOCOL  0x50
#define CBI0_INTERFACE_PROTOCOL 0x00

#define USB_BOT_DATA_TIMEOUT    (12 * 1000)
#define USB_BOT_CSW_TIMEOUT     (10 * 1000)

typedef struct _PEI_BLKIO_DEVICE PEI_BLKIO_DEVICE;

typedef
EFI_STATUS
(EFIAPI *PEI_ATAPI_COMMAND)(
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BLKIO_DEVICE          *PeiBlkIoDev,
  IN  VOID                      *Command,
  IN  UINT8                     CommandSize,
  IN  VOID                      *DataBuffer,
  IN  UINT32                    BufferLength,
  IN  EFI_USB_DATA_DIRECTION    Direction,
  IN  UINT16                    TimeOutInMilliSeconds
  );

//
// Bot device structure
//
#define PEI_BLKIO_DEVICE_SIGNATURE      SIGNATURE_32 ('P', 'B', 'L', 'K')

struct _PEI_BLKIO_DEVICE {
  UINTN                         Signature;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI BlkIoPpi;
  EFI_PEI_PPI_DESCRIPTOR        BlkIoPpiList;  
  H2O_PEI_BLOCK_IO_PPI          H2OBlkIoPpi;
  EFI_PEI_PPI_DESCRIPTOR        H2OBlkIoPpiList;  
  
  EFI_PEI_BLOCK_IO_MEDIA        Media;
  PEI_USB3_IO_PPI               *UsbIoPpi;
  EFI_PEI_STALL_PPI             *StallPpi;
  PEI_ATAPI_COMMAND             AtapiCommand;
  EFI_USB_ENDPOINT_DESCRIPTOR   *BulkInEndpoint;
  EFI_USB_ENDPOINT_DESCRIPTOR   *BulkOutEndpoint;
  EFI_USB_ENDPOINT_DESCRIPTOR   *InterruptEndpoint;
  UINTN                         DeviceType;
  ATAPI_REQUEST_SENSE_DATA      *SenseData;
  UINT32                        CbwTag;
  UINT32                        BypassCswTimeout;
  BOOLEAN                       FirstReadPassed;
  BOOLEAN                       FirstReadCapPassed;
  BOOLEAN                       FirstRequestSensePassed;
  BOOLEAN                       FirstInquiryPassed;
};

#define PEI_BLKIO_DEVICE_FROM_THIS(a)     CR (a, PEI_BLKIO_DEVICE, BlkIoPpi, PEI_BLKIO_DEVICE_SIGNATURE)
#define H2O_PEI_BLKIO_DEVICE_FROM_THIS(a) CR (a, PEI_BLKIO_DEVICE, H2OBlkIoPpi, PEI_BLKIO_DEVICE_SIGNATURE)

/**

  Initialize the usb bot device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  NotifyDesc            Address of the notification descriptor data structure.
  @param  InvokePpi             Address of the PPI that was invoked.

  @retval EFI_SUCCESS           The function completes successfully.

**/
EFI_STATUS
EFIAPI
NotifyOnUsbIoPpi (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDesc,
  IN  VOID                              *InvokePpi
  );

/**

  Gets the count of block I/O devices that one specific block driver detects

  @param  PeiServices           General purpose services that are available to every PEIM
  @param  This                  Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance
  @param  NumberBlockDevices    The number of block I/O devices discovered.

  @retval EFI_SUCCESS           Operation performed successfully.

**/
EFI_STATUS
EFIAPI
GetNumberOfBlockDevices (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI     *This,
  OUT UINTN                             *NumberBlockDevices
  );

/**

  Gets a block device's media information.

  @param  PeiServices           General purpose services that are available to every PEIM
  @param  This                  Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param  DeviceIndex           The number from one to NumberBlockDevices.
  @param  MediaInfo             The media information of the specified block media. 

  @retval EFI_SUCCESS           Media information about the specified block device was obtained successfully.
  @retval EFI_DEVICE_ERROR      Cannot get the media information due to a hardware error.

**/
EFI_STATUS
EFIAPI
GetMediaInfo (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI     *This,
  IN  UINTN                             DeviceIndex,
  OUT EFI_PEI_BLOCK_IO_MEDIA            *MediaInfo
  );

/**

  Reads the requested number of blocks from the specified block device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  This                  Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param  DeviceIndex           The number from one to NumberBlockDevices.
  @param  StartLBA              The starting logical block address (LBA) to read from on the device
  @param  BufferSize            The size of the Buffer in bytes. This number must be 
                                a multiple of the intrinsic block size of the device.
  @param  Buffer                A pointer to the destination buffer for the data.
                                The caller is responsible for the ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while attempting to perform the read operation.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE   The BufferSize parameter is not a multiple of the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
ReadBlocks (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI     *This,
  IN  UINTN                             DeviceIndex,
  IN  EFI_PEI_LBA                       StartLBA,
  IN  UINTN                             BufferSize,
  OUT VOID                              *Buffer
  );



/**

  Writes the requested number of blocks from the specified block device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  This                  Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param  DeviceIndex           The number from one to NumberBlockDevices.
  @param  StartLBA              The starting logical block address (LBA) to read from on the device
  @param  BufferSize            The size of the Buffer in bytes. This number must be 
                                a multiple of the intrinsic block size of the device.
  @param  Buffer                A pointer to the destination buffer for the data.
                                The caller is responsible for the ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while attempting to perform the read operation.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE   The BufferSize parameter is not a multiple of the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
WriteBlocks (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  H2O_PEI_BLOCK_IO_PPI              *This,
  IN  UINTN                             DeviceIndex,
  IN  EFI_PEI_LBA                       StartLBA,
  IN  UINTN                             BufferSize,
  OUT VOID                              *Buffer
  );

/**

  Detect whether the removable media is present and whether it has changed.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiUsbDetectMedia (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev
  );

/**

  Sends out ATAPI Inquiry Packet Command to the specified device.
  This command will return INQUIRY data of the device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.

  @retval EFI_SUCCESS           Inquiry command completes successfully.
  @retval EFI_DEVICE_ERROR      Inquiry command failed.

**/
EFI_STATUS
PeiUsbInquiry (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev
  );

/**

  Sends out ATAPI Test Unit Ready Packet Command to the specified device
  to find out whether device is accessible.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.

  @retval EFI_SUCCESS           TestUnit command executed successfully.
  @retval EFI_DEVICE_ERROR      Device cannot be executed TestUnit command successfully.

**/
EFI_STATUS
PeiUsbTestUnitReady (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev
  );

/**

  Sends out ATAPI Request Sense Packet Command to the specified device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  SenseCounts            Length of sense buffer.
  @param  SenseKeyBuffer        Pointer to sense buffer.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiUsbRequestSense (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev,
  OUT UINTN             *SenseCounts,
  IN  UINT8             *SenseKeyBuffer
  );

/**

  Sends out ATAPI Read Capacity Packet Command to the specified device.
  This command will return the information regarding the capacity of the
  media in the device.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiUsbReadCapacity (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev
  );

/**

  PEI UFI Read10 comand

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Buffer                The pointer to data buffer.
  @param  Lba                   The start logic block address of reading.
  @param  NumberOfBlocks        The block number of reading.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiUsbRead10 (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev,
  IN  VOID              *Buffer,
  IN  EFI_PEI_LBA       Lba,
  IN  UINTN             NumberOfBlocks
  );

EFI_STATUS
PeiUsbWrite10 (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BLKIO_DEVICE  *PeiBlkIoDev,
  IN  VOID              *Buffer,
  IN  EFI_PEI_LBA       Lba,
  IN  UINTN             NumberOfBlocks
  );

/**

  Check if there is media according to sense data.

  @param  SenseData             Pointer to sense data.
  @param  SenseCounts           SenseCounts Count of sense data.

  @retval TRUE                  No media
  @retval FALSE                 Media exists

**/
BOOLEAN
IsNoMedia (
  IN  ATAPI_REQUEST_SENSE_DATA  *SenseData,
  IN  UINTN                     SenseCounts
  );

/**

  Check if there is media error according to sense data.

  @param  SenseData             Pointer to sense data.
  @param  SenseCounts           Count of sense data.

  @retval TRUE                  Media error
  @retval FALSE                 No media error

**/
BOOLEAN
IsMediaError (
  IN  ATAPI_REQUEST_SENSE_DATA  *SenseData,
  IN  UINTN                     SenseCounts
  );

/**

  Check if there is media error according to sense data.

  @param  SenseData             Pointer to sense data.
  @param  SenseCounts           Count of sense data.

  @retval TRUE                  Media error
  @retval FALSE                 No media error

**/
BOOLEAN
IsMediaChange (
  IN  ATAPI_REQUEST_SENSE_DATA  *SenseData,
  IN  UINTN                     SenseCounts
  );

/**

  Check is the device busy according to sense data.

  @param  SenseData             Pointer to sense data.
  @param  SenseCounts           SenseCounts Count of sense data.

  @retval TRUE                  Device is busy
  @retval FALSE                 Device is not busy

**/
BOOLEAN
IsDeviceBusy (
  IN  ATAPI_REQUEST_SENSE_DATA  *SenseData,
  IN  UINTN                     SenseCounts
  );

/**

  Send ATAPI command using BOT protocol.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Command               The command to be sent to ATAPI device.
  @param  CommandSize           The length of the data to be sent.
  @param  DataBuffer            The pointer to the data.
  @param  BufferLength          The length of the data.
  @param  Direction             The direction of the data.
  @param  TimeOutInMilliSeconds Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiBotAtapiCommand (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  PEI_BLKIO_DEVICE            *PeiBlkIoDev,
  IN  VOID                        *Command,
  IN  UINT8                       CommandSize,
  IN  VOID                        *DataBuffer,
  IN  UINT32                      BufferLength,
  IN  EFI_USB_DATA_DIRECTION      Direction,
  IN  UINT16                      TimeOutInMilliSeconds
  );

/**

  Send ATAPI command using CBI protocol.

  @param  PeiServices           General purpose services that are available to every PEIM.
  @param  PeiBlkIoDev           Indicates the PEI_BLKIO_DEVICE instance.
  @param  Command               The command to be sent to ATAPI device.
  @param  CommandSize           The length of the data to be sent.
  @param  DataBuffer            The pointer to the data.
  @param  BufferLength          The length of the data.
  @param  Direction             The direction of the data.
  @param  TimeOutInMilliSeconds Indicates the maximum time, in millisecond, which the transfer is allowed to complete.

  @retval EFI_SUCCESS           Command executed successfully.
  @retval EFI_DEVICE_ERROR      Some device errors happen.

**/
EFI_STATUS
PeiCbiAtapiCommand (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  PEI_BLKIO_DEVICE            *PeiBlkIoDev,
  IN  VOID                        *Command,
  IN  UINT8                       CommandSize,
  IN  VOID                        *DataBuffer,
  IN  UINT32                      BufferLength,
  IN  EFI_USB_DATA_DIRECTION      Direction,
  IN  UINT16                      TimeOutInMilliSeconds
  );

#endif
