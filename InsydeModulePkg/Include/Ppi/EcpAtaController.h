/** @file

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

/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AtaController.h

Abstract:

  ATA Controller PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide ATA services.

--*/

#ifndef _ECP_PEI_ATA_CONTROLLER_PPI_H_
#define _ECP_PEI_ATA_CONTROLLER_PPI_H_

#define ECP_PEI_ATA_CONTROLLER_PPI_GUID \
  { 0xedd95ea9, 0xb6b4, 0x4104, 0x9a, 0x29, 0x56, 0x45, 0x39, 0xd1, 0xde, 0x4f \
  }

#define PEI_IDE_NONE        0x00
#define PEI_IDE_PRIMARY     0x01
#define PEI_IDE_SECONDARY   0x02
#define PEI_SATA_NONE       0x04
#define PEI_SATA_PRIMARY    0x08
#define PEI_SATA_SECONDARY  0x010

typedef enum {
   IdePrimary    = 0,
   IdeSecondary  = 1,
   AtaChannel3   = 2,
   AtaChannel4   = 3,
   AtaChannel5   = 4,
   IdeMaxChannel = 5
} EFI_IDE_CHANNEL ;

typedef enum {
   IdeMaster    = 0,
   IdeSlave     = 1,
   IdeMaxDevice = 2
} EFI_IDE_DEVICE ;

//
// IDE Registers
//
typedef union
{
  UINT16  Command;    /* when write */
  UINT16  Status;     /* when read */
} IDE_CMD_OR_STATUS;

typedef union {
  UINT16  Error;      /* when read */
  UINT16  Feature;    /* when write */
} IDE_ERROR_OR_FEATURE;

typedef union {
  UINT16 AltStatus;       /* when read */
  UINT16 DeviceControl;   /* when write */
} IDE_AltStatus_OR_DeviceControl;

//
// IDE registers set
//
typedef struct {
  UINT16                              Data;
  IDE_ERROR_OR_FEATURE                Reg1;
  UINT16                              SectorCount;
  UINT16                              SectorNumber;
  UINT16                              CylinderLsb;
  UINT16                              CylinderMsb;
  UINT16                              Head;
  IDE_CMD_OR_STATUS                   Reg;
 
  IDE_AltStatus_OR_DeviceControl      Alt;
  UINT16                              DriveAddress;
} IDE_BASE_REGISTERS;

typedef struct _PEI_ATA_CONTROLLER_PPI PEI_ATA_CONTROLLER_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_ENABLE_ATA) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI         *This,
  IN UINT8                          ChannelMask
  );

typedef
EFI_STATUS
(EFIAPI *PEI_GET_ATA_CHANNEL_IO) (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI *This,
  IN  UINT8                  ChannelIndex,
  OUT IDE_BASE_REGISTERS     *ChannelIoSpace
  );

typedef
EFI_STATUS
(EFIAPI *PEI_GET_ATA_BASE_ADDR) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI         *This,
  IN EFI_PHYSICAL_ADDRESS           *ControllerAddr
  );

struct _PEI_ATA_CONTROLLER_PPI {
  PEI_ENABLE_ATA                    EnableAtaChannel;
  PEI_GET_ATA_CHANNEL_IO            GetAtaChannelIoSpace;
  PEI_GET_ATA_BASE_ADDR             GetAtaControllerBase;
};

extern EFI_GUID gEcpPeiAtaControllerPpiGuid;

#endif
