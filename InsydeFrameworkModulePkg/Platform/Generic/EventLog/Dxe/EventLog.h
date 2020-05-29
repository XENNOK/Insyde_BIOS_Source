//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _LOG_EVENT_H_
#define _LOG_EVENT_H_

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FlashLayout.h"
#include "EfiHobLib.h"
#include "SetupConfig.h"

#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (DataHubRecords)
#include EFI_GUID_DEFINITION (EventLogHob)
#include EFI_PROTOCOL_DEFINITION (EventLog)
#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)
#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_CONSUMER (LoadedImage)
#include EFI_PROTOCOL_CONSUMER (NonVolatileVariable)
#include EFI_PROTOCOL_CONSUMER (ChipsetLibServices)
#include EFI_PROTOCOL_CONSUMER (SmmChipsetLibServices)
#ifdef EFI_DEBUG
#include EFI_PROTOCOL_DEFINITION (DebugMask)
#endif


typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  FLASH_DEVICE                          *DevicePtr;
  UINTN                                 GPNVBase;
  UINTN                                 GPNVLength;
  UINTN                                 SpareBase;
  UINTN                                 SpareSize;
  EFI_EVENT_LOG_PROTOCOL                EventLogService;
} EFI_EVENT_LOG_INSTANCE;

#define EVENT_LOG_SIGNATURE  EFI_SIGNATURE_32 ('E', 'L', 'O', 'G')

#define INSTANCE_FROM_EFI_EVENT_LOG_THIS(a)  CR (a, EFI_EVENT_LOG_INSTANCE, EventLogService, EVENT_LOG_SIGNATURE)

#define FLASH_BLOCK_SIZE                0x10000

typedef struct {
  UINT32                    Signature;
  EFI_LIST_ENTRY            Link;
  EVENT_LOG_ORGANIZATION    *Buffer;
} EFI_EVENT_LOG_DATA;

#define EVENT_LOG_DATA_SIGNATURE  EFI_SIGNATURE_32 ('E', 'E', 'L', 'D')
#define DATA_FROM_EFI_EVENT_LOG_THIS(a)  CR (a, EFI_EVENT_LOG_DATA, Link, EVENT_LOG_DATA_SIGNATURE)

#define EVENT_LOG_CLEAR_PROCESS_SIGNATURE EFI_SIGNATURE_32 ('E', 'L', 'C', 'P')
#pragma pack(push, 1)
typedef struct {
  UINT32              Signature;
  UINT32              Length;
} EVENT_LOG_HEADER;
#pragma pack(pop)


EFI_STATUS
EfiWriteEventLog (
IN  EFI_EVENT_LOG_PROTOCOL              *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );

EFI_STATUS
EfiClearEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This
  );

EFI_STATUS
EfiReadNextEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN OUT VOID                          **EventListAddress
  );

EFI_DEVICE_PATH_PROTOCOL *
AppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

VOID
EFIAPI
EventLogNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  );

VOID *
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

EVENT_LOG_ORGANIZATION *
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

#endif
