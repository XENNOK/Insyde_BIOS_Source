//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   DebugIo.h
//; 

#ifndef _DEBUG_IO_H_
#define _DEBUG_IO_H_

#define PEI_DEBUG_IO_PPI_GUID \
  { 0x148AEFEB, 0xF0AE, 0x462a, 0x8B, 0x05, 0xE7, 0xA7, 0x9B, 0x3D, 0xE1, 0x3B }

EFI_FORWARD_DECLARATION (PEI_DEBUG_IO_PROTOCOL);

typedef struct _DEBUGIO_CONFIG {
  UINT32 RPR           :  1;
  UINT32 Bypass        :  1;
  UINT32 DriverType    :  2;
  UINT32 EhciPort      :  4;
  UINT32 UsbDebugPort  :  8;
  UINT32 DebugPort     : 16;
  UINT32 BypassTimeout;
} DEBUGIO_CONFIG;

#define DEBUGIO_CONFIG_RPR_LPC  0
#define DEBUGIO_CONFIG_RPR_PCI  1

//
// Forward declaration of PEI_DEBUG_SUPPORT_PROTOCOL
//
typedef struct _PEI_DEBUG_IO_PROTOCOL PEI_DEBUG_IO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PEI_DEBUG_IO_WRITE) (
  IN     PEI_DEBUG_IO_PROTOCOL          *This,
  IN OUT UINTN                          *BufferSize,
  IN     VOID                           *Buffer
  )
/*++

  Routine Description:
    Writes data to a debug device

  Arguments:
    This       - Protocol instance pointer
    BufferSize - On input, the size of the Buffer.
                 On output, the amount of data actually written
    Buffer     - The buffer of data to write

  Returns:
    EFI_SUCCES       - The data was written
    EFI_DEVICE_ERROR - The device reported an error
    EFI_TIMEOUT      - The data write was stopped due to a timeout

--*/
;

typedef
EFI_STATUS
(EFIAPI *PEI_DEBUG_IO_READ) (
  IN     PEI_DEBUG_IO_PROTOCOL          *This,
  IN OUT UINTN                          *BufferSize,
  OUT    VOID                           *Buffer
  )
/*++

  Routine Description:
    Read data from a debug device

  Arguments:
    This       - Protocol instance pointer
    BufferSize - On input, the size of the Buffer
                 On output, the amount of data returned in Buffer
    Buffer     - The buffer to return the data into

  Returns:
    EFI_SUCCESS      - The data was read
    EFI_DEVICE_ERROR - The device reported an error
    EFI_TIMEOUT      - The data write was stopped due to a timeout

--*/
;

typedef
EFI_STATUS
(EFIAPI *PEI_DEBUG_IO_GET_INFO) (
  IN     PEI_DEBUG_IO_PROTOCOL          *This,
  IN OUT DEBUGIO_CONFIG                 *Info
  )
/*++

  Routine Description:
    Get debug device info

  Arguments:
    This    - Protocol instance pointer
    Info    - The buffer to return the info

  Returns:
    EFI_SUCCESS      - The data was read

--*/
;

typedef struct _PEI_DEBUG_IO_PROTOCOL {
  PEI_DEBUG_IO_WRITE                   Write;
  PEI_DEBUG_IO_READ                    Read;
  PEI_DEBUG_IO_GET_INFO                GetInfo;
} PEI_DEBUG_IO_PROTOCOL;

extern EFI_GUID  gPeiDebugIoPpiGuid;

#endif

