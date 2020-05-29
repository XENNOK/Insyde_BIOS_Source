/** @file
  Header file for function definitions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#ifndef RAPID_START_AHCI_H_
#define RAPID_START_AHCI_H_

#include <RapidStartAhciReg.h>

#pragma pack(1)

typedef struct {
  UINT32  Abar;
  UINT32  PortBase;
  UINT32  PortSize;
  UINTN   Port;
  BOOLEAN PollCancellation;

  // Internal fields
  UINT16  *Identify;
  enum {
    AHCI_STATE_UNKNOWN,
    AHCI_STATE_INIT,
    AHCI_STATE_INUSE,
    AHCI_STATE_MAX,
  } State;
  struct {
    UINT8  HybridPriority : 4;
    UINT8  Reserved : 3;
    UINT8  HybridInfoValid : 1;
  } HybridInfo;
  UINT32   TotalRemainingCacheCapacityInSector;
} AHCI_CONTEXT;

typedef union {
  UINT32 Data;
  struct {
    UINT32  ByteReserved1 : 8;
    UINT32  ByteReserved2 : 8;
    UINT32  HybridPriority : 4;
    UINT32  Reserved : 1;
    UINT32  HybridInfoValid : 1;
    UINT32  Reserved1 : 2;
    UINT32  ByteReserved4 : 8;
  } r;
} ATA_AUX;

typedef struct {
  UINT64  MemAddr;
  UINT64  Lba;
  UINT32  SectorCount;
  UINT16  Feature;
  UINT8   Command;
  UINT8   Direction;
  ATA_AUX Auxiliary;
} ATA_COMMAND;

#pragma pack()

#define AhciSpinUp(Ahci) \
    AhciSpinUpPort (Ahci, Ahci->Port)

#define AhciPostCommand(Ahci, AtaCmd, CmdIdx) \
    AhciPostCommandWithZeroFilter (Ahci, AtaCmd, CmdIdx, NULL, NULL)

/**
  Initialize SATA controller and enable decode

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS - SATA controller has been initialized successfully
**/
EFI_STATUS
AhciInit (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Returns SATA enabled port bitmap basing on PCS value.

  @retval Enabled ports map.
**/
UINT32
AhciGetEnabledPorts (
  VOID
  );

/**
  Returns SATA present port bitmap basing on PCS value.

  @retval Present ports map.
**/
UINT32
AhciGetPresentPorts (
  VOID
  );

/**
  Check whether device is password locked.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS            - Device not locked
  @retval EFI_ACCESS_DENIED      - Drive is in LOCKED state and need to execute UNLOCK command before accessing
  @retval EFI_SECURITY_VIOLATION - Drive is already in SECURITY FREEZE state and will not accept UNLOCK command
  @retval EFI_TIMEOUT            - Timeout occured
  @retval EFI_DEVICE_ERROR       - Error occurred on device side.
**/
EFI_STATUS
AhciGetLockStatus (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Disable SATA controller after all AHCI commands have completed

  @param[in] Ahci  - SATA controller information structure
**/
VOID
AhciDone (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Spin up Ahci port

  @param[in] Ahci  - SATA controller information structure
  @param[in] Port  - SATA port number

  @retval EFI_SUCCESS            - AHCI Port has been spun up successfully
**/
EFI_STATUS
AhciSpinUpPort (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINTN                 Port
  );

/**
  Initialize AHCI port for reading/writing RapidStart Store

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_DEVICE_ERROR - AHCI port initialization failed
  @retval EFI_SUCCESS      - AHCI port initialized successfully and RapidStart Store is ready for reading/writing
**/
EFI_STATUS
AhciPortInit (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Stops AHCI port.

  @param[in] Ahci  - SATA controller information structure
**/
VOID
AhciPortDone (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Executes AHCI command.

  @param[in] Ahci        - SATA controller information structure
  @param[in,out] AtaCmd  - ATA command structure

  @retval EFI_SUCCESS      - Command successfull
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciExecCommand (
  IN     AHCI_CONTEXT          *Ahci,
  IN OUT ATA_COMMAND           *AtaCmd
  );

/**
  Enqueues ahci command and for read/write it is optionally with Zero filter.

  @param[in] Ahci            - SATA controller information structure
  @param[in,out] AtaCmd      - ATA command structure
  @param[out] CmdMask        - If given a bit corresponding to allocated command slot is set
  @param[in] ZeroPageBitMap  - A pointer for ZeroPageTable
  @param[out] SmRamBufferLba - If given the drive LBA storing original SMRAM buffer content will be passed by it.

  @retval EFI_SUCCESS      - AHCI command successfully posted
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciPostCommandWithZeroFilter (
  IN     AHCI_CONTEXT          *Ahci,
  IN OUT ATA_COMMAND           *AtaCmd,
  OUT    OPTIONAL UINT32                *CmdMask,
  IN     OPTIONAL UINT32                *ZeroPageBitMap,
  OUT    OPTIONAL UINT64                *SmRamBufferLba
  );

/**
  Issues ATA command with no data transfer.

  @param[in] Ahci    - SATA controller information structure
  @param[in] Command - Command to be issued

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciSimpleCommand (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT8                 Command
  );

/**
  Check whether there are available command slots.

  @param[in] Ahci      - SATA controller information structure

  @retval TRUE if there is an available slot, FALSE otherwise.
**/
BOOLEAN
AhciHasFreeCmdSlot (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Waits untill given command(s) complete.

  @param[in] Ahci      - SATA controller information structure
  @param[in] CmdMask   - Command(s) mask

  @retval EFI_SUCCESS      - Command complete
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Error occurred on device side.
  @retval EFI_NOT_STARTED  - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
EFI_STATUS
AhciWaitComplete (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT32                CmdMask
  );

/**
  Waits for all AHCI commands completed.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS      - All AHCI commands have completed
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Error occurred on device side.
  @retval EFI_NOT_STARTED  - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
EFI_STATUS
AhciWaitAllComplete (
  IN     AHCI_CONTEXT          *Ahci
  );

/**
  Sends Identify Device ATA command to port

  @param[in] Ahci    - SATA controller information structure
  @param[out] Buffer  - Buffer to store device information

  @retval EFI_SUCCESS      - function executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciIdentifyDevice (
  IN     AHCI_CONTEXT          *Ahci,
  OUT    VOID                  *Buffer
  );

/**
  Performs TRIM command on given LBA range

  @param[in] Ahci  - SATA controller information structure
  @param[in] Lba   - First block to be trimmed
  @param[in] Count - Number of blocks to trim

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @exception EFI_UNSUPPORTED  - TRIM is not supported by the device
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciTrimRange (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT64                Lba,
  IN     UINT32                Count
  );

/**
  Unlocks ATA device.

  @param[in] Ahci     - SATA controller information structure
  @param[in] Password - security credential

  @retval EFI_SUCCESS            - Command executed successfully
  @retval EFI_SECURITY_VIOLATION - Device security lock freeze
  @exception EFI_UNSUPPORTED        - Security is not supported by the device
  @retval EFI_TIMEOUT            - Timeout occured
  @retval EFI_DEVICE_ERROR       - Command failed
**/
EFI_STATUS
AhciSecurityUnlock (
  IN     AHCI_CONTEXT          *Ahci,
//[-start-111024-IB04770172-modify]//
  IN     CHAR8                 *Password,
  IN     UINT16                PasswordType
//[-end-111024-IB04770172-modify]//
  );

/**
  Hybrid Hard Disk Support.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @exception EFI_UNSUPPORTED  - TRIM is not supported by the device
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciHybridHardDiskSupport (
  IN     AHCI_CONTEXT          *Ahci
  );

#endif
