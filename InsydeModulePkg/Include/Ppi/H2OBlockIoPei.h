/** @file

  Provides the services to write a block I/O device.
  This PPI is based on EFI_PEI_RECOVERY_BLOCK_IO_PPI.
  
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _H2O_PEI_BLOCK_IO_H_
#define _H2O_PEI_BLOCK_IO_H_


#define H2O_PEI_BLOCK_IO_PPI_GUID \
  { \
    0x84f93e9c, 0xea83, 0x409a, { 0xb3, 0x60, 0x36, 0xcb, 0xdd, 0x68, 0xa8, 0x8f } \
  }


///
/// The forward declaration for H2O_PEI_BLOCK_IO_PPI.
///
typedef struct _H2O_PEI_BLOCK_IO_PPI  H2O_PEI_BLOCK_IO_PPI;


/**
  Write the requested number of blocks to the specified block device.

  The function write the requested number of blocks to the device. All the 
  blocks are write, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  PeiServices   General-purpose services that are available to 
                            every PEIM.
  @param[in]  This          Indicates the H2O_PEI_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants 
                            to talk. Because the driver that implements Block I/O 
                            PPIs will manage multiple block devices, PPIs that 
                            want to talk to a single device must specify the device 
                            index that was assigned during the enumeration process. 
                            This index is a number from one to NumberBlockDevices.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the 
                            buffer.
                         
  @retval EFI_SUCCESS             The data was write correctly to the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting 
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The write request contains LBAs that are not 
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_PEI_WRITE_BLOCKS)(
  IN  EFI_PEI_SERVICES               **PeiServices,
  IN  H2O_PEI_BLOCK_IO_PPI           *This,
  IN  UINTN                          DeviceIndex,
  IN  EFI_PEI_LBA                    StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  );

///
///  H2O_PEI_BLOCK_IO_PPI provides the services that are required
///  to write a block I/O device during PEI Phase.
///
struct _H2O_PEI_BLOCK_IO_PPI {
  ///
  /// Write the requested number of blocks from the specified block device.
  ///
  H2O_PEI_WRITE_BLOCKS                  WriteBlocks;
};

extern EFI_GUID gH2OPeiVirtualBlockIoPpiGuid;

#endif
