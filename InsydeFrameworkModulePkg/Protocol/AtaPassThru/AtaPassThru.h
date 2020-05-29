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
//;   AtaPassThru.h
//; 
//; Abstract:
//; 
//;   EFI_ATA_PASS_THRU_PROTOCOL as defined in UEFI 2.2
//; 

#ifndef _EFI_ATA_PASS_THRU_PROTOCOL_H_   
#define _EFI_ATA_PASS_THRU_PROTOCOL_H_

#include <Tiano.h>

EFI_FORWARD_DECLARATION (EFI_ATA_PASS_THRU);

//
//  EFI AtaPassThru protocol
//
#define EFI_ATA_PASS_THRU_PROTOCOL_GUID \
  {\
    0x1d3de7f0, 0x0807, 0x424f, 0xaa, 0x69, 0x11, 0xa5, 0x4e, 0x19, 0xa4, 0x6f \
  }


typedef struct _EFI_ATA_PASS_THRU_PROTOCOL   EFI_ATA_PASS_THRU_PROTOCOL;

//
// Related Definitions
//

typedef struct {
UINT32 Attributes;
UINT32 IoAlign;
} EFI_ATA_PASS_THRU_MODE;
                 
//
// EFI_ATA_PASS_THRU_MODE Definitions
//
#define EFI_ATA_PASS_THRU_ATTRIBUTES_PHYSICAL   0x0001
#define EFI_ATA_PASS_THRU_ATTRIBUTES_LOGICAL    0x0002
#define EFI_ATA_PASS_THRU_ATTRIBUTES_NONBLOCKIO 0x0004

//
//  EFI_ATA_PASS_THRU_CMD_PROTOCOL Definitions
//
typedef UINT8 EFI_ATA_PASS_THRU_CMD_PROTOCOL;
#define EFI_ATA_PASS_THRU_PROTOCOL_ATA_HARDWARE_RESET 0x00
#define EFI_ATA_PASS_THRU_PROTOCOL_ATA_SOFTWARE_RESET 0x01
#define EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA       0x02
#define EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN        0x04
#define EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT       0x05
#define EFI_ATA_PASS_THRU_PROTOCOL_DMA                0x06
#define EFI_ATA_PASS_THRU_PROTOCOL_DMA_QUEUED         0x07
#define EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_DIAGNOSTIC  0x08
#define EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_RESET       0x09
#define EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_IN       0x0A
#define EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_OUT      0x0B
#define EFI_ATA_PASS_THRU_PROTOCOL_FPDMA              0x0C
#define EFI_ATA_PASS_THRU_PROTOCOL_RETURN_RESPONSE    0xFF

//
//  EFI_ATA_PASS_THRU_LENGTH Definitions
//
typedef UINT8 EFI_ATA_PASS_THRU_LENGTH;
#define EFI_ATA_PASS_THRU_LENGTH_BYTES                0x80
#define EFI_ATA_PASS_THRU_LENGTH_MASK                 0x70
#define EFI_ATA_PASS_THRU_LENGTH_NO_DATA_TRANSFER     0x00
#define EFI_ATA_PASS_THRU_LENGTH_FEATURES             0x10
#define EFI_ATA_PASS_THRU_LENGTH_SECTOR_COUNT         0x20
#define EFI_ATA_PASS_THRU_LENGTH_TPSIU                0x30    //Transport Protocol-Specific Information Unit (TPSIU)
#define EFI_ATA_PASS_THRU_LENGTH_COUNT                0x0F

typedef struct _EFI_ATA_COMMAND_BLOCK {
  UINT8   Reserved1[2];
  UINT8   AtaCommand;
  UINT8   AtaFeatures;
  UINT8   AtaSectorNumber;
  UINT8   AtaCylinderLow;
  UINT8   AtaCylinderHigh;
  UINT8   AtaDeviceHead;
  UINT8   AtaSectorNumberExp;
  UINT8   AtaCylinderLowExp;
  UINT8   AtaCylinderHighExp;
  UINT8   AtaFeaturesExp;
  UINT8   AtaSectorCount;
  UINT8   AtaSectorCountExp;
  UINT8   Reserved2[6];
} EFI_ATA_COMMAND_BLOCK;

typedef struct _EFI_ATA_STATUS_BLOCK {
  UINT8   Reserved1[2];
  UINT8   AtaStatus;
  UINT8   AtaError;
  UINT8   AtaSectorNumber;
  UINT8   AtaCylinderLow;
  UINT8   AtaCylinderHigh;
  UINT8   AtaDeviceHead;
  UINT8   AtaSectorNumberExp;
  UINT8   AtaCylinderLowExp;
  UINT8   AtaCylinderHighExp;
  UINT8   Reserved2;
  UINT8   AtaSectorCount;
  UINT8   AtaSectorCountExp;
  UINT8   Reserved3[6];
} EFI_ATA_STATUS_BLOCK;

typedef struct {
  EFI_ATA_STATUS_BLOCK            *Asb;
  EFI_ATA_COMMAND_BLOCK           *Acb;
  UINT64                          Timeout;
  VOID                            *InDataBuffer;
  VOID                            *OutDataBuffer;
  UINT32                          InTransferLength;
  UINT32                          OutTransferLength;
  EFI_ATA_PASS_THRU_CMD_PROTOCOL  Protocol;
  EFI_ATA_PASS_THRU_LENGTH        Length;
} EFI_ATA_PASS_THRU_COMMAND_PACKET;

typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_PASSTHRU) (
  IN      EFI_ATA_PASS_THRU_PROTOCOL        *This,
  IN      UINT16                            Port,
  IN      UINT16                            PortMultiplierPort,
  IN OUT  EFI_ATA_PASS_THRU_COMMAND_PACKET  *Packet,
  IN      EFI_EVENT                         Event  OPTIONAL
)
/*++

Routine Description:

  Sends an ATA command to an ATA device that is attached to the ATA controller. 

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - The port number of the ATA device to send the command.
  PortMultiplierPort   - The port multiplier port number of the ATA device to send the command. 
                         If there is no port multiplier, then specify 0.
  Packet               - A pointer to the ATA command to send to the ATA device specified by Port and
                         PortMultiplierPort.
  Event                - If non-blocking I/O is not supported then Event is ignored, and blocking I/O is
                         performed. If Event is NULL, then blocking I/O is performed. If Event is not
                         NULL and non blocking I/O is supported, then non-blocking I/O is performed, and
                         Event will be signaled when the ATA command completes.
Returns:

  EFI_STATUS

--*/
;


typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_GET_NEXT_PORT) (
  IN     EFI_ATA_PASS_THRU_PROTOCOL  *This,
  IN OUT UINT16                      *Port
)
/*++

Routine Description:

  Used to retrieve the lis t of legal port numbers for ATA devices on an ATA controller.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - On input, a pointer to the port number on the ATA controller. On output, a pointer to
                         the next port number on the ATA controller. An input value of 0xFFFF retrieves the
                         first port number on the ATA controller.
Returns:

  EFI_STATUS

--*/
;


typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_GET_NEXT_DEVICE) (
  IN     EFI_ATA_PASS_THRU_PROTOCOL   *This,
  IN     UINT16                       Port,
  IN OUT UINT16                       *PortMultiplierPort
)
/*++

Routine Description:

  Used to retrieve the list of legal port multiplier port numbers for ATA devices on a port of an ATA controller.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - The port number present on the ATA controller.
  PortMultiplierPort   - On input, a pointer to the port multiplier port number of an ATA device present on the
                         ATA controller. If on input a PortMultiplierPort of 0xFFFF is specified,
                         then the port multiplier port number of the first ATA device is returned. On output, a
                         pointer to the port multiplier port number of the next ATA device present on an ATA
                         controller.
Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_BUILD_DEVICE_PATH) (
  IN      EFI_ATA_PASS_THRU_PROTOCOL  *This,
  IN      UINT16                      Port,
  IN      UINT16                      PortMultiplierPort,
  IN OUT  EFI_DEVICE_PATH_PROTOCOL    **DevicePath
)
/*++

Routine Description:

  Used to allocate and build a device path node for an ATA device on an ATA controller.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - Port specifies the port number of the ATA device for which a device path node is to be allocated and built.
  PortMultiplierPort   - The port multiplier port number of the ATA device for which a device path node is to be allocated and built. If there is no port multiplier, then specify 0.
  DevicePath           - A pointer to a single device path node that describes the ATA device specified by Port and PortMultiplierPort. This function is responsible for allocating the
                         buffer DevicePath with the boot service AllocatePool(). It is the caller¡¦s responsibility to free DevicePath when the caller is finished with DevicePath.
Returns:

  EFI_STATUS

--*/
;


typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_GET_DEVICE) (
  IN  EFI_ATA_PASS_THRU_PROTOCOL *This,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT UINT16                     *Port,
  OUT UINT16                     *PortMultiplierPort
)
/*++

Routine Description:

  Used to translate a device path node to a port number and port multiplier port number.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  DevicePath           - A pointer to the device path node that describes an ATA device on the ATA controller.
  Port                 - On return, points to the port number of an ATA device on the ATA controller.
  PortMultiplierPort   - On return, points to the port multiplier port number of an ATA device on the ATA controller.
  
Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_RESET_PORT) (
  IN EFI_ATA_PASS_THRU_PROTOCOL  *This,
  IN UINT16                      Port
)
/*++

Routine Description:

  Resets a specific port on the ATA controller. This operation also resets all the ATA devices connected to the port.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - The port number on the ATA controller.
  
Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_ATA_PASS_THRU_RESET_DEVICE) (
  IN EFI_ATA_PASS_THRU_PROTOCOL  *This,
  IN UINT16                      Port,
  IN UINT16                      PortMultiplierPort
)
/*++

Routine Description:

  Resets an ATA device that is connected to an ATA controller.

Arguments:

  This                 - A pointer to the EFI_ATA_PASS_THRU_PROTOCOL instance.
  Port                 - Port represents the port number of the ATA device to be reset.
  PortMultiplierPort   - The port multiplier port number of the ATA device to reset. If there is no port multiplier, then specify 0.
  
Returns:

  EFI_STATUS

--*/
;

typedef struct _EFI_ATA_PASS_THRU_PROTOCOL {
  EFI_ATA_PASS_THRU_MODE              *Mode;
  EFI_ATA_PASS_THRU_PASSTHRU          PassThru;
  EFI_ATA_PASS_THRU_GET_NEXT_PORT     GetNextPort;
  EFI_ATA_PASS_THRU_GET_NEXT_DEVICE   GetNextDevice;
  EFI_ATA_PASS_THRU_BUILD_DEVICE_PATH BuildDevicePath;
  EFI_ATA_PASS_THRU_GET_DEVICE        GetDevice;
  EFI_ATA_PASS_THRU_RESET_PORT        ResetPort;
  EFI_ATA_PASS_THRU_RESET_DEVICE      ResetDevice;
} EFI_ATA_PASS_THRU_PROTOCOL;


extern EFI_GUID gEfiAtaPassThruProtocolGuid;

#endif 
