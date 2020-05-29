/** @file

  Header file of BIOS Storage Kernel Dxe implementation.

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

#ifndef _BIOS_STORAGE_KERNEL_DXE_H_
#define _BIOS_STORAGE_KERNEL_DXE_H_

#include <EfiFlashMap.h>
#include <ChipsetSmiTable.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/FdSupportLib.h>
#include <Library/HobLib.h>
#include <Library/CmosLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/SpiAccessLib.h>

#include <Guid/BiosEventLogHob.h>
#include <Guid/EventLogVariable.h>
//[-start-140306-IB08400249-add]//
#include <Guid/EventStorageVariable.h>
//[-end-140306-IB08400249-add]//
#include <Guid/HobList.h>
#include <Guid/FlashMapHob.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/BiosEventLog.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SmmBase2.h>
//#include <Protocol/SmmRuntime.h>
#include <Protocol/NonVolatileVariable.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/VariableWrite.h>

#define FLASH_BLOCK_SIZE                0x10000
#define SW_SMI_IO_PORT                  0xB2

#define GET_ERASE_SECTOR_NUM(Base, Len, EraseBase, SectorSize) \
                            (((Base) + (Len) - (EraseBase) + (SectorSize) - 1) / (SectorSize)) 

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  FLASH_DEVICE                          *DevicePtr;
  UINTN                                 GPNVBase;
  UINTN                                 GPNVLength;
  UINTN                                 SpareBase;
  UINTN                                 SpareSize;
  EFI_BIOS_EVENT_LOG_PROTOCOL           EventLogService;
} EFI_EVENT_LOG_INSTANCE;

#define EVENT_LOG_SIGNATURE  SIGNATURE_32 ('E', 'L', 'O', 'G')

#define INSTANCE_FROM_EFI_EVENT_LOG_THIS(a)  CR (a, EFI_EVENT_LOG_INSTANCE, EventLogService, EVENT_LOG_SIGNATURE)

#define FLASH_BLOCK_SIZE                0x10000

typedef struct {
  UINT32                        Signature;
  LIST_ENTRY                    Link;
  BIOS_EVENT_LOG_ORGANIZATION   *Buffer;
} EFI_EVENT_LOG_DATA;

#define EVENT_LOG_DATA_SIGNATURE  SIGNATURE_32 ('E', 'E', 'L', 'D')
#define DATA_FROM_EFI_EVENT_LOG_THIS(a)  CR (a, EFI_EVENT_LOG_DATA, Link, EVENT_LOG_DATA_SIGNATURE)

#define EVENT_LOG_CLEAR_PROCESS_SIGNATURE SIGNATURE_32 ('E', 'L', 'C', 'P')
#pragma pack(push, 1)
typedef struct {
  UINT32              Signature;
  UINT32              Length;
} EVENT_LOG_HEADER;
#pragma pack(pop)


EFI_STATUS
EFIAPI
EfiWriteEventLog (
IN  EFI_BIOS_EVENT_LOG_PROTOCOL         *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );

EFI_STATUS
EFIAPI
EfiClearEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This
  );

EFI_STATUS
EFIAPI
EfiReadNextEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN OUT VOID                           **EventListAddress
  );

VOID *
EFIAPI
EventLogAllocatePool (
  IN UINTN            Size
  );
/*++

Routine Description:

  Event log driver allocate pool function.

Arguments:

  Size        - Request memory size.

Returns:

  The allocate pool, or NULL

--*/

EFI_STATUS
EFIAPI
EventLogFreePool (
  IN VOID            *Buffer
  );
/*++

Routine Description:

  Event log driver free pool function.

Arguments:

  Buffer                  - The allocated pool entry to free.

Returns:

  EFI_SUCCESS             - Free pool successful.
  EFI_INVALID_PARAMETER   - Inputer parameter is NULL.
  Other                   - System free pool function fail.

--*/

BOOLEAN
EFIAPI
InClearProcess (
  IN EFI_EVENT_LOG_INSTANCE     *ELPrivate
  );
/*++

Routine Description:

  Check system state is whether in the clear event log process

Arguments:

  ELPrivate - Point to event log private data.

Returns:

  TRUE      - System is in the clear event log process
  FALSE     - System isn't in the clear event log process

--*/

EFI_STATUS
EFIAPI
UpdateEventLogFromBackup (
  IN EFI_EVENT_LOG_INSTANCE     *ELPrivate
  );
/*++

Routine Description:

  This function uses backup data to update event log in GPNV store.

Arguments:

  ELPrivate              - Point to event log private data.

Returns:

  EFI_SUCCESS            - Update system setting to factory default successful.
  EFI_ABORTED            - It is not in the clear event log process or backup data size is not correct.
  EFI_OUT_OF_RESOURCES   - Allocate memory fail.
  Other                  - Erase or program flash device fail.

--*/

BIOS_EVENT_LOG_ORGANIZATION *
EFIAPI
GenernateEventLog (
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );
/*++

Routine Description:

  Genernate event log.

Arguments:

  EventLogType      - Event log type.
  PostBitmap1       - Post bitmap 1 which will be stored in data area of POST error type log.
  PostBitmap2       - Post bitmap 2 which will be stored in data area of POST error type log.
  OptionDataSize    - Option data size.
  OptionLogData     - Poiunter to option data.

Returns:

  Pointer to allocated memory which contain event log data or NULL.

--*/

EFI_STATUS
EFIAPI
EventLogFlashFdErase (
  IN FLASH_DEVICE              *DevicePtr,
  IN UINTN                     LbaWriteAddress,
  IN UINTN                     EraseBlockSize
  );
/*++

Routine Description:

  Event log driver flash erase function.

Arguments:

  DevicePtr         - Pointer to flash device instance.
  LbaWriteAddress   - LBA write address.
  EraseBlockSize    - Erase size.

Returns:

  EFI_SUCCESS           - Flash erase successful.
  EFI_INVALID_PARAMETER - Flash device pointer is NULL.
  Other                 - Flash erase function return fail.

--*/

EFI_STATUS
EFIAPI
EventLogFlashFdProgram (
  IN FLASH_DEVICE              *DevicePtr,
  IN UINT8                     *Dest,
  IN UINT8                     *Src,
  IN UINTN                     *NumBytes,
  IN UINTN                     LbaWriteAddress
  );
/*++

Routine Description:

  Event log driver flash program function.

Arguments:

  DevicePtr         - Pointer to flash device instance.
  Dest              - Destination Offset.
  Src               - Pointer to source buffer.
  NumBytes          - Number of bytes which will be write into.
  LbaWriteAddress   - LBA write address.

Returns:

  EFI_SUCCESS           - Flash program successful.
  EFI_INVALID_PARAMETER - Flash device pointer is NULL.
  Other                 - Flash program function return fail.

--*/

EFI_STATUS
EFIAPI
EfiOverWriteEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN  BIOS_EVENT_LOG_ORGANIZATION            *InputBuffer
  );


VOID
EFIAPI
BiosProtectEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

VOID *
EFIAPI
EventLogAllocateZeroBuffer (
  IN UINTN     Size
  );
  
/*++

Routine Description:

  Event Log Smm function call.
  Let the EventLogDxe driver to modify the BIOS NV region via EventLogSmm driver.

Arguments:

  InPutBuff         - The data structure will pass to EventLogSmm driver.
  DataSize          - Size.
  SubFunNum         - Sub function number to decide the behaviors.
  SmiPort           - Io port that can issue a SW SMI.

Returns:

--*/
UINT8
EFIAPI
SmmEventLogCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );

BOOLEAN
EFIAPI
SwitchToSmm (
  VOID
  );

VOID
EFIAPI
InstalledVarWriteNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  );

#endif
