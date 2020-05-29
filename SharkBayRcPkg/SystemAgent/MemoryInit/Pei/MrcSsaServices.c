/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MrcSsaServices.c

@brief
  This file contains the SSA BIOS services PPI.
**/

#include "MrcGlobal.h"
#include "MrcDdr3.h"
#include "MrcOemDebugPrint.h"
#include "MrcOemIo.h"
#include "MrcOemMmio.h"
#include "MrcOemPlatform.h"
#include "MrcOemSmbus.h"
#include "MrcSsaServices.h"

#define MAX_CADB_ENTRIES                (16)
#define REUT_CPGC_OFFSET                (0x400)
#define EXTRA_INDEX_OFFSET              (1)
#define SSA_REVISION_BIOS               (('0' << 24) | ('0' << 16) | ('7' << 8) | '5')
#define SSA_REVISION_COMMON             (('0' << 24) | ('0' << 16) | ('7' << 8) | '5')
#define SSA_REVISION_MEMORY             (('0' << 24) | ('0' << 16) | ('7' << 8) | '5')
#define SPD_SENSOR_BASE_ADDRESS         (0x30)
#define SPD_SENSOR_TEMPERATURE_OFFSET   (5)

extern EFI_GUID gSsaBiosServicesPpiGuid;

typedef union {
  struct {
    UINT32  Low;
    UINT32  High;
  } Data32;
  UINT64 Data;
} UINT64_STRUCT;

#ifdef SSA_FLAG

/**

@brief
  Verify that the indicated socket is present and enabled.

  @param[in] MrcData - Pointer to the MRC global data area.
  @param[in] Socket  - Zero based CPU socket number.

  @retval TRUE if present and enabled, otherwise FALSE.

**/
static
BOOLEAN
IsSocketPresent (
  const MrcParameters * const MrcData,
  const UINT8                 Socket
  )
{
  return ((Socket < MAX_CPU_SOCKETS) ? TRUE : FALSE);
}

/**

@brief
  Verify that the indicated controller is present and enabled.

  @param[in] MrcData    - Pointer to the MRC global data area.
  @param[in] Socket     - Zero based CPU socket number.
  @param[in] Controller - Zero based memory controller number.

  @retval TRUE if present and enabled, otherwise FALSE.

**/
static
BOOLEAN
IsControllerPresent (
  const MrcParameters * const MrcData,
  const UINT8                 Socket,
  const UINT8                 Controller
  )
{
  return (((IsSocketPresent (MrcData, Socket)) &&
    (Controller < MAX_CONTROLLERS) &&
    (MrcData->SysOut.Outputs.Controller[Controller].Status == CONTROLLER_PRESENT)) ?
    TRUE : FALSE);
}

/**

@brief
  Verify that the indicated channel is present and enabled.

  @param[in] MrcData    - Pointer to the MRC global data area.
  @param[in] Socket     - Zero based CPU socket number.
  @param[in] Controller - Zero based memory controller number.
  @param[in] Channel    - Zero based memory channel number.

  @retval TRUE if present and enabled, otherwise FALSE.

**/
static
BOOLEAN
IsChannelPresent (
  const MrcParameters * const MrcData,
  const UINT8                 Socket,
  const UINT8                 Controller,
  const UINT8                 Channel
  )
{
  return (((IsControllerPresent (MrcData, Socket, Controller)) &&
    (Channel < MAX_CHANNEL) &&
    (MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Status == CHANNEL_PRESENT)) ?
    TRUE : FALSE);
}

/**

@brief
  Verify that the indicated DIMM is present and enabled.

  @param[in] MrcData    - Pointer to the MRC global data area.
  @param[in] Socket     - Zero based CPU socket number.
  @param[in] Controller - Zero based memory controller number.
  @param[in] Channel    - Zero based memory channel number.
  @param[in] Dimm       - Zero based memory DIMM number.

  @retval TRUE if present and enabled, otherwise FALSE.

**/
static
BOOLEAN
IsDimmPresent (
  const MrcParameters * const MrcData,
  const UINT8                 Socket,
  const UINT8                 Controller,
  const UINT8                 Channel,
  const UINT8                 Dimm
  )
{
  return (((IsChannelPresent (MrcData, Socket, Controller, Channel)) &&
    (Dimm < MAX_DIMMS_IN_CHANNEL) &&
    (MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm].Status == DIMM_PRESENT)) ?
    TRUE : FALSE);
}

/**

@brief
  Verify that the indicated rank is present and enabled.

  @param[in] MrcData    - Pointer to the MRC global data area.
  @param[in] Socket     - Zero based CPU socket number.
  @param[in] Controller - Zero based memory controller number.
  @param[in] Channel    - Zero based memory channel number.
  @param[in] Dimm       - Zero based memory DIMM number.
  @param[in] Rank       - Zero based memory rank number in the DIMM.

  @retval TRUE if present and enabled, otherwise FALSE.

**/
static
BOOLEAN
IsRankPresent (
  const MrcParameters * const MrcData,
  const UINT8                 Socket,
  const UINT8                 Controller,
  const UINT8                 Channel,
  const UINT8                 Dimm,
  const UINT8                 Rank
  )
{
  return (((IsDimmPresent (MrcData, Socket, Controller, Channel, Dimm)) &&
    (Rank < MAX_RANK_IN_DIMM) &&
    ((MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm].RankInDIMM - 1) >= Rank)) ?
    TRUE : FALSE);
}

/**

@brief
  Initialize the heap so that malloc and free can be used.

  @param[in] MrcData    - Pointer to the MRC global data area.

  @retval TRUE if present and enabled, otherwise FALSE.

**/

static
BOOLEAN
InitHeap (
  const MrcParameters * const MrcData
  )
{
  const MrcInput *Inputs;
  UINT8          *HeapLimitPtr;
  HeapBufHeader  *HeapBase;
  UINT8          *BaseAddr;

  Inputs = &MrcData->SysIn.Inputs;

  //
  // If heap is provided
  //
  if (Inputs->SsaHeapSize) {
    BaseAddr     = (UINT8 *) Inputs->SsaHeapBase;
    HeapLimitPtr = BaseAddr + Inputs->SsaHeapSize;

    //
    // Initialize the start header
    //
    HeapBase                 = (HeapBufHeader *) BaseAddr;
    HeapBase->BufBase        = BaseAddr + sizeof (HeapBufHeader);
    HeapBase->BufLimit       = Inputs->SsaHeapSize - (2 * sizeof (HeapBufHeader));
    HeapBase->BufFlags.Data  = 0;

    //
    // Initialize the end header
    //
    HeapBase                        = (HeapBufHeader *) (HeapLimitPtr - sizeof (HeapBufHeader));
    HeapBase->BufBase               = HeapLimitPtr;
    HeapBase->BufLimit              = 0;
    HeapBase->BufFlags.Bits.HeapEnd = 1;
    return TRUE;
  }
  return FALSE;
}

/**

@brief
  Reads a variable-sized value from a memory mapped register using an absolute address.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the register to be accessed.
  @param[out]     Buffer      - Value storage location.

  @retval Nothing.

**/
static
VOID
ReadMem (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  )
{
  MMIO_BUFFER *MmioBuffer;

  MmioBuffer = (MMIO_BUFFER *) Buffer;
  switch (Width) {
    case RegWidth8:
      MrcOemMmioRead8 ((U32) Address, &MmioBuffer->Data8, 0);
      break;

    case RegWidth16:
      MrcOemMmioRead16 ((U32) Address, &MmioBuffer->Data16, 0);
      break;

    case RegWidth32:
      MrcOemMmioRead ((U32) Address, &MmioBuffer->Data32, 0);
      break;

    case RegWidth64:
      MrcOemMmioRead64 ((U32) Address, &MmioBuffer->Data64, 0);
      break;

    default:
      break;
  }
  return;
}

/**

@brief
  Writes a variable sized value to a memory mapped register using an absolute address.
  
  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the register to be accessed.
  @param[in]      Buffer      - Value to write.

  @retval Nothing.

**/
static
VOID
WriteMem (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  )
{
  MMIO_BUFFER         *MmioBuffer;

  MmioBuffer = (MMIO_BUFFER *) Buffer;
  switch (Width) {
    case RegWidth8:
      MrcOemMmioWrite8 ((U32) Address, MmioBuffer->Data8, 0);
      break;

    case RegWidth16:
      MrcOemMmioWrite16 ((U32) Address, MmioBuffer->Data16, 0);
      break;

    case RegWidth32:
      MrcOemMmioWrite ((U32) Address, MmioBuffer->Data32, 0);
      break;

    case RegWidth64:
      MrcOemMmioWrite64 ((U32) Address, MmioBuffer->Data64, 0);
      break;

    default:
      break;
  }
  return;
}

/**

@brief
  Reads a variable sized value from I/O.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the I/O to be accessed.
  @param[out]     Buffer      - Value storage location.

  @retval Nothing.

**/
static
VOID
ReadIo (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  )
{
  IO_BUFFER           *IoBuffer;

  IoBuffer = (IO_BUFFER *) Buffer;
  switch (Width) {
    case RegWidth8:
      IoBuffer->Data8 = MrcOemInPort8 ((UINT16) Address);
      break;

    case RegWidth16:
      IoBuffer->Data16 = MrcOemInPort16 ((UINT16) Address);
      break;

    case RegWidth32:
      IoBuffer->Data32 = MrcOemInPort32 ((UINT16) Address);
      break;

    default:
      break;
  }
  return;
}

/**

@brief
  Writes a variable sized value to I/O.
  
  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the I/O to be accessed.
  @param[in]      Buffer      - Value to write.

  @retval Nothing.

**/
static
VOID
WriteIo (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  )
{
  IO_BUFFER           *IoBuffer;

  IoBuffer = (IO_BUFFER *) Buffer;
  switch (Width) {
    case RegWidth8:
      MrcOemOutPort8 ((UINT16) Address, IoBuffer->Data8);
      break;

    case RegWidth16:
      MrcOemOutPort16 ((UINT16) Address, IoBuffer->Data16);
      break;

    case RegWidth32:
      MrcOemOutPort32 ((UINT16) Address, IoBuffer->Data32);
      break;

    default:
      break;
  }
  return;
}

/**

@brief
  Reads a variable sized value from the PCI config space register.
  This function takes advantage of any caching implemented by BIOS.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[out]     Buffer      - Value storage location.
  @param[in]      CachedData  - If set to TRUE, returns the Cached data (if applicable) for performance. If set to FALSE returns the data read from device. 

  @retval Nothing.

**/
static
VOID
ReadPci (
  EFI_PEI_SERVICES                **PeiServices,
  SSA_BIOS_SERVICES_PPI           *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer,
  BOOLEAN                         CachedData
  )
{
  PCI_BUFFER        *PciBuffer;
  PCI_CONFIG_SPACE  PciAddress;
  UINT32            Value;

  PciBuffer                 = (PCI_BUFFER *) Buffer;
  PciAddress.Value          = 0;
  PciAddress.Bits.Bus       = Address->Bus;
  PciAddress.Bits.Device    = Address->Device;
  PciAddress.Bits.Function  = Address->Function;
  PciAddress.Bits.Offset    = Address->Register;
  PciAddress.Bits.Enable    = 1;
  MrcOemOutPort32 (MrcOemPciIndex (), PciAddress.Value);
  Value = MrcOemInPort32 (MrcOemPciData ());

  switch (Width) {
    case RegWidth8:
      PciBuffer->Data8 = (UINT8) Value;
      break;

    case RegWidth16:
      PciBuffer->Data16 = (UINT16) Value;
      break;

    case RegWidth32:
      PciBuffer->Data32 = Value;
      break;

    default:
      break;
  }

  return;
}

/**

@brief
  Writes a variable sized value to the PCI config space register.
  
  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Width       - The size of the value to write.
  @param[in]      Address     - Address of the I/O to be accessed. Must be modulo 'Width'.
  @param[in]      Buffer      - Value to write.
  @param[in]      CachedData  - If set to TRUE, returns the Cached data (if applicable) for performance. If set to FALSE returns the data read from device. 

  @retval Nothing.

**/
static
VOID
WritePci (
  EFI_PEI_SERVICES                **PeiServices,
  SSA_BIOS_SERVICES_PPI           *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer,
  BOOLEAN                         CachedData
  )
{
  PCI_BUFFER        *PciBuffer;
  PCI_CONFIG_SPACE  PciAddress;
  BOOLEAN           DoIt;
  UINT32            Value;

  PciBuffer                 = (PCI_BUFFER *) Buffer;
  DoIt                      = TRUE;
  PciAddress.Value          = 0;
  PciAddress.Bits.Bus       = Address->Bus;
  PciAddress.Bits.Device    = Address->Device;
  PciAddress.Bits.Function  = Address->Function;
  PciAddress.Bits.Offset    = Address->Register;
  PciAddress.Bits.Enable    = 1;
  Value                     = 0;

  switch (Width) {
    case RegWidth8:
      ReadPci (PeiServices, This, RegWidth32, Address, (PCI_BUFFER *) &Value, FALSE);
      Value &= ~0xFF;
      Value |= PciBuffer->Data8;
      break;

    case RegWidth16:
      ReadPci (PeiServices, This, RegWidth32, Address, (PCI_BUFFER *) &Value, FALSE);
      Value &= ~0xFFFF;
      Value |= PciBuffer->Data16;
      break;

    case RegWidth32:
      Value = PciBuffer->Data32;
      break;

    default:
      Value = 0;
      DoIt  = FALSE;
      break;
  }

  if (DoIt) {
    MrcOemOutPort32 (MrcOemPciIndex (), PciAddress.Value);
    MrcOemOutPort32 (MrcOemPciData (), Value);
  }

  return;
}

/**

@brief
  Gets a base address to be used in the different memory map or MMIO register access functions.
  
  @param[in, out] PeiServices     - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Index           - Additional index to locate the register.
  @param[in]      BaseAddressType - Value that indicates the type of base address to be retrieved.
  @param[in]      BaseAddress     - Where to write the base address

  @retval Success or error code.

**/
static
SSA_STATUS
GetBaseAddress (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Index,
  BASE_ADDR_TYPE        BaseAddressType,
  EFI_PHYSICAL_ADDRESS  *BaseAddress
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  switch (BaseAddressType) {
    case MCH_BAR:
      *BaseAddress = IsControllerPresent (MrcData, Socket, Controller) ?
        MrcData->SysIn.Inputs.MchBarBaseAddress : 0;
      return (Success);
    default:
      break;
  }
  return (UnsupportedValue);
}

/**

@brief
  Function used to dynamically allocate memory.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Size        - Amount of memory in bytes to allocate.

  @retval Returns a pointer to an allocated memory block on success or NULL on failure. 

**/
static
VOID *
Malloc (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Size
  )
{
  MrcInput       *Inputs;
  MrcDebug       *Debug;
  HeapBufHeader  *HeaderPtr;
  HeapBufHeader  *NextHeaderPtr;
  MrcParameters  *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;

  if ((Size == 0) || (Inputs->SsaHeapSize == 0)) {
    return NULL;
  }

  if (Inputs->SsaHeapFlag.Bits.Init == 0) {
    Inputs->Debug.Current         = 0;
    Inputs->SsaHeapFlag.Bits.Init = 1;
    InitHeap (MrcData);
  }

  //
  // Round size up to a QWORD integral.
  //
  Size += sizeof (UINT64) - (Size % sizeof (UINT64));

  //
  // Check to see if request exceeds available heap size.
  //
  if (Size > (Inputs->SsaHeapSize - (3 * sizeof (HeapBufHeader)))) {
    return NULL;
  }

  HeaderPtr = (HeapBufHeader *) Inputs->SsaHeapBase;

  //
  // Walk the heap looking for an available buffer.
  //
  while ((HeaderPtr->BufFlags.Bits.Occupied > 0) || (HeaderPtr->BufLimit < Size)) {
    HeaderPtr = (HeapBufHeader *) (HeaderPtr->BufBase + HeaderPtr->BufLimit);
  }

  //
  // Check for the end of heap space.
  //
  if (HeaderPtr->BufFlags.Bits.HeapEnd > 0) {
    return NULL;
  }

  //
  // Lock memory for the buffer.
  //
  HeaderPtr->BufFlags.Bits.Occupied = 1;

  //
  // Initialize the current size and next header if required.
  //
  if ((HeaderPtr->BufLimit - Size) > sizeof (HeapBufHeader)) {
    NextHeaderPtr                = (HeapBufHeader *) (HeaderPtr->BufBase + Size);
    NextHeaderPtr->BufBase       = (UINT8 *) NextHeaderPtr + sizeof (HeapBufHeader);
    NextHeaderPtr->BufLimit      = HeaderPtr->BufLimit - Size - sizeof (HeapBufHeader);
    NextHeaderPtr->BufFlags.Data = 0;
    HeaderPtr->BufLimit          = Size;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SSA malloc. Base = %Xh, Size = %u\n", HeaderPtr->BufBase, Size);
  //
  // Return the current base.
  //
  return HeaderPtr->BufBase;
}

/**

@brief
  Function used to release memory allocated using Malloc.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Buffer      - The buffer to return to the free pool.

  @retval Nothing. 

**/
static
VOID
Free (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *Buffer
  )
{
  const MrcInput *Inputs;
  const MrcDebug *Debug;
  HeapBufHeader  *HeaderPtr;
  HeapBufHeader  *TempPtr;
  MrcParameters  *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  if (Inputs->SsaHeapSize > 0) {
    //
    // Initialize a pointer to the given buffer header.
    //
    HeaderPtr = (HeapBufHeader *) ((UINT8 *) Buffer - sizeof (HeapBufHeader));

    //
    // Validate the given pointer before proceeding.
    //
    if (HeaderPtr->BufBase == Buffer) {
      //
      // Free the given buffer.
      //
      HeaderPtr->BufFlags.Bits.Occupied = 0;

      //
      // Initialize the root header.
      //
      HeaderPtr = (HeapBufHeader *) Inputs->SsaHeapBase;

      //
      // Walk the heap looking for holes to merge.
      //
      do {
        //
        // Find the next hole.
        //
        while (HeaderPtr->BufFlags.Bits.Occupied > 0) {
          HeaderPtr = (HeapBufHeader *) (HeaderPtr->BufBase + HeaderPtr->BufLimit);
        }

        //
        // Check for the end of heap space.
        //
        if (HeaderPtr->BufFlags.Bits.HeapEnd > 0) {
          break;
        }

        //
        // Look for adjacent holes to merge.
        //
        TempPtr = (HeapBufHeader *) (HeaderPtr->BufBase + HeaderPtr->BufLimit);
        while ((TempPtr->BufFlags.Bits.Occupied == 0) && (TempPtr->BufFlags.Bits.HeapEnd == 0)) {
          //
          // Add this buffer to the current limit and move to the next buffer.
          //
          HeaderPtr->BufLimit += TempPtr->BufLimit + sizeof (HeapBufHeader);
          TempPtr = (HeapBufHeader *) (TempPtr->BufBase + TempPtr->BufLimit);
        }
        //
        // Move to the next buffer.
        //
        HeaderPtr = (HeapBufHeader *) (HeaderPtr->BufBase + HeaderPtr->BufLimit);

      } while (HeaderPtr->BufFlags.Bits.HeapEnd == 0);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SSA free. Base = %Xh\n", Buffer);
    }
  }
  return;
}

/**

@brief
  Function used to output debug messages to the output logging device.

  @param[in, out] PeiServices  - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      PrintLevel   - The severity level of the string.
  @param[in]      FormatString - The reduced set of printf style format specifiers.
                  %[flags][width]type
                  [flags] '-' left align
                  [flags] '+' prefix with sign (+ or -)
                  [flags] '0' zero pad numbers
                  [flags] ' ' prefix black in front of postive numbers
                  [width] non negative decimal integer that specifies the width to print a value.
                  [width] '*' get the width from a int argument on the stack.
                  type    'd'|'i' signed decimal integer
                  type    'u' unsigned integer
                  type    'x'|'X' hexidecimal using "ABCDEF"
                  type    'c' print character
                  type    'p' print a pointer to void
                  type    's' print a null terminated string
  @param[in]      ...          - Variable list of output values.

  @retval Nothing. 

**/
static
VOID
SsaDebugPrint (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  PRINT_LEVEL           PrintLevel,
  UINT8                 *FormatString,
  ...
  )
{
#ifdef MRC_DEBUG_PRINT
  MrcVaList Marker;
  char      Buffer[MAX_STRING_LENGTH];

  if (FormatString != NULL) {
    VA_START (Marker, FormatString);
    if (StringFormatter (FormatString, Marker, sizeof (Buffer), Buffer) > 0) {
      DEBUG ((PrintLevel, Buffer));
    }
  }
#endif

  return;
}

/**

@brief
  Returns the platform's memory voltage (VDD).

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Voltage     - Where the platform's memory voltage (in mV) will be written.

  @retval Success or failure code.

**/
static
SSA_STATUS
GetMemVoltage (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                *Voltage
  )
{
  MrcParameters *MrcData;

  MrcData  = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  *Voltage = MrcData->SysOut.Outputs.VddVoltage[MrcData->SysIn.Inputs.MemoryProfile];
  return (Success);
}

/**

@brief
  Sets the platform's memory voltage (VDD).

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Voltage     - The requested platform's memory voltage (in mV).

  @retval Success or failure code.

**/
static
SSA_STATUS
SetMemVoltage (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                *Voltage
  )
{
  UINT32 VddSettleWaitTime;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  VddSettleWaitTime = MIN (MrcData->SysIn.Inputs.VddSettleWaitTime, 200);
  MrcOemVDDVoltageCheckAndSwitch (MrcData, *Voltage, &VddSettleWaitTime);
  MrcWait (MrcData, VddSettleWaitTime * HPET_1US);
  MrcData->SysOut.Outputs.VddVoltage[MrcData->SysIn.Inputs.MemoryProfile] = *Voltage;
  return (Success);
}

/**

@brief
  Returns the temperature of the specified DIMM in whole degree Celsius.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[out]     Temperature - Where the DIMM's temperature in whole degree Celsius will be written.

  @retval Success or failure code.

**/
static
SSA_STATUS
GetMemTemp (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  INT32                 *Temperature
  )
{
  const MrcParameters *MrcData;
  const MrcInput      *Inputs;
  UINT16              Value;
  UINT8               Address;
  union {
    struct {
      UINT16 Fraction : 4;
      UINT16 Whole    : 8;
      UINT16 Sign     : 1;
      UINT16 Low      : 1;
      UINT16 High     : 1;
      UINT16 Tcrit    : 1;
    } Bit;
    UINT16            Data;
    UINT8             Data8[2];
  } TsRegisterSet;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Inputs  = &MrcData->SysIn.Inputs;
  if (IsDimmPresent (MrcData, Socket, Controller, Channel, Dimm)) {
    Address = SPD_SENSOR_BASE_ADDRESS | (Inputs->Controller[Controller].Channel[Channel].Dimm[Dimm].SpdAddress & 0xF);
    if (mrcSuccess == MrcOemSmbusRead16 (Inputs->SmbusBaseAddress, Address, SPD_SENSOR_TEMPERATURE_OFFSET, &Value)) {
      // Value read is in big endian format, convert it to little endian.
      TsRegisterSet.Data = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0xFF);
      *Temperature       = (TsRegisterSet.Bit.Sign) ? ((-1) * TsRegisterSet.Bit.Whole) : TsRegisterSet.Bit.Whole;
      // SsaDebugPrint (PeiServices, This, SSA_D_INFO, "SSA GetMemTemp %u/%u/%u/%u %04Xh %04Xh %d\n", Socket, Controller, Channel, Dimm, Address, TsRegisterSet.Data, *Temperature);
      return (Success);
    }
  }
  *Temperature = 0;
  return (NotAvailable);
}

/**

@brief
  Sets the rank's mode register.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based rank number in the DIMM.
  @param[in]      Address     - Zero based mode register number.
  @param[in]      Data        - Value to write to the register.

  @retval Success or failure code.

**/
static
SSA_STATUS
WriteMrs (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Address,
  UINT16                Data
  )
{
  MrcParameters *MrcData;
  U8            LogicalRank;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsRankPresent (MrcData, Socket, Controller, Channel, Dimm, Rank)) {
    LogicalRank = (Dimm * MAX_RANK_IN_DIMM) + Rank;
#ifdef ULT_FLAG
    if (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3) {
      MrcIssueMrw (MrcData, Channel, LogicalRank, Address, Data, FALSE, FALSE);
    } else
#endif
    {
      MrcWriteMRSAll (MrcData, Channel, MRC_BIT0 << LogicalRank, Address, &Data);
    }
    return (Success);
  }
  return (LogicalRankNotSupported);
}

/**

@brief
  Restores the rank's mode register using the default value that the MRC has stored away.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based rank number in the DIMM.
  @param[in]      Address     - Zero based mode register number.

  @retval Success or failure code.

**/
static
SSA_STATUS
RestoreMrs (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Address
  )
{
  MrcParameters *MrcData;
  UINT16        Data;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Data    = MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm].Rank[Rank].MR[Address];
  return (WriteMrs (PeiServices, This, Socket, Controller, Channel, Dimm, Rank, Address, Data));
}

/**

@brief
  Get the rank's mode register.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.
  @param[in]      Rank        - Zero based rank number in the DIMM.
  @param[in]      Address     - Zero based mode register number.
  @param[out]     Data        - Value read from the register.

  @retval Success or failure code.

**/
static
SSA_STATUS
ReadMrs (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Address,
  UINT16                *Data
  )
{
  MrcParameters *MrcData;
  SSA_STATUS    Status;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsRankPresent (MrcData, Socket, Controller, Channel, Dimm, Rank)) {
    *Data  = MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm].Rank[Rank].MR[Address];
    Status = Success;
  } else {
    *Data  = 0;
    Status = LogicalRankNotSupported;
  }
  return (Status);
}

/**

@brief
  Returns the DIMM number according to the rank number.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Rank        - Zero based rank number in the channel.

  @retval Returns the zero based DIMM index or FFh on error.

**/
static
UINT8
GetDimmFromLogicalRank (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Rank
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel) && (Rank < MAX_RANK_IN_CHANNEL)) {
    return (Rank / MAX_RANK_IN_DIMM);
  }
  return ((UINT8) ~0);
}

/**

@brief
  Gets DIMM information. 

  @param[in, out] PeiServices    - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Dimm           - Zero based DIMM number.
  @param[in, out] DimmInfoBuffer - Location to store DIMM information.

  @retval Returns DIMM information when a good status code is returned.

**/
static
SSA_STATUS
GetDimmInfo (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  MrcDimmInfo           *DimmInfoBuffer
  )
{
  MrcDimmOut  *DimmOut;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsDimmPresent (MrcData, Socket, Controller, Channel, Dimm)) {
    DimmOut = &MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm];
    DimmInfoBuffer->EccSupport   = MrcData->SysOut.Outputs.EccSupport;
    DimmInfoBuffer->DimmCapacity = DimmOut->DimmCapacity;
    DimmInfoBuffer->RowSize      = DimmOut->RowSize;
    DimmInfoBuffer->ColumnSize   = DimmOut->ColumnSize;
    CopyMem (&DimmInfoBuffer->SerialNumber,
      &MrcData->SysIn.Inputs.Controller[Controller].Channel[Channel].Dimm[Dimm].Spd.Ddr3.ModuleId,
      sizeof (SPD_UNIQUE_MODULE_ID));
    return (Success);
  } else {
    SetMem (DimmInfoBuffer, 0, sizeof (MrcDimmInfo));
    return (NotAvailable);
  }
}

/**

@brief
  Returns the number of ranks in a specific DIMM on a given socket/controller.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Dimm        - Zero based DIMM number.

  @retval Returns the number of ranks in a specific DIMM on a given socket/controller.

**/
static
UINT8
GetRankInDimm (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  return ((IsDimmPresent (MrcData, Socket, Controller, Channel, Dimm)) ?
    MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm].RankInDIMM : 0);
}

/**

@brief
  Returns the bitmask of valid ranks on a given channel.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.

  @retval Returns the bitmask of valid ranks on a given channel.

**/
static
UINT8
GetLogicalRankBitMask (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  return ((IsChannelPresent (MrcData, Socket, Controller, Channel)) ?
    MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].ValidRankBitMask : 0);
}

/**

@brief
  Returns the channel bit mask of the populated channels.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.

  @retval Returns the channel bit mask of the populated channels.

**/
static
UINT8
GetChannelBitMask (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  )
{
  UINT8 Channel;
  UINT8 ChannelMask;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  ChannelMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
      ChannelMask |= (MRC_BIT0 << Channel);
    }
  }
  return (ChannelMask);
}

/**

@brief
  Gets information about the system.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] SystemInfo  - Pointer to buffer to be filled with system information.

  @retval Returns information about the system. 

**/
static
SSA_STATUS
GetSystemInfo (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  MrcSystemInfo         *SystemInfoBuffer
  )
{
  SystemInfoBuffer->MaxNumberSockets      = MAX_CPU_SOCKETS;
  SystemInfoBuffer->MaxNumberControllers  = MAX_CONTROLLERS;
  SystemInfoBuffer->MaxNumberChannels     = MAX_CHANNEL;
  SystemInfoBuffer->MaxNumberLogicalRanks = MAX_RANK_IN_CHANNEL;
  SystemInfoBuffer->SocketsBitMask        = ((UINT32) (~0)) >> (32 - MAX_CPU_SOCKETS);
  return (Success);
}

/**

@brief
  Get a bit mask representing the present and enabled memory controllers in a CPU socket.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.

  @retval A bit mask representing the present and enabled memory controllers in a CPU socket.

**/
static
UINT8
GetControllerBitMask (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  return (IsSocketPresent (MrcData, Socket) ? (((UINT8) (~0)) >> (8 - MAX_CONTROLLERS)) : 0);
}

/**

@brief
  Function used to reset a DIMM.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.

  @retval Nothing.

**/
static
VOID
JedecReset (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsControllerPresent (MrcData, Socket, Controller)) {
    MrcResetSequence (MrcData);
  }
  return;
}

/**

@brief
  Function returns the low side range of a margin parameter.

  @param[in] IoLevel     - Id of the I/O level to access.
  @param[in] MarginGroup - Id of the margin group. Can be RxDq, TxDq, RxVref or TxVref.

  @retval Function returns the low side range of a margin parameter.

**/
static
INT16
GetMarginParamMin (
  GSM_LT IoLevel,
  GSM_GT MarginGroup
  )
{
  INT16 Value;

  switch (MarginGroup) {
      case RxDqsDelay:
      case TxDqsDelay:
        Value = (-31);
        break;

      case RxVref:
      case TxVref:
        Value = (-54);
        break;

      default:
        Value = INT16_MIN;
        break;
  }
  return (Value);
}

/**

@brief
  Function returns the high side range of a margin parameter.

  @param[in] IoLevel     - Id of the I/O level to access.
  @param[in] MarginGroup - Id of the margin group. Can be RxDq, TxDq, RxVref or TxVref.

  @retval Function returns the high side range of a margin parameter.

**/
static
INT16
GetMarginParamMax (
  GSM_LT IoLevel,
  GSM_GT MarginGroup
  )
{
  INT16 Value;

  switch (MarginGroup) {
    case RxDqsDelay:
    case TxDqsDelay:
      Value = 31;
      break;

    case RxVref:
    case TxVref:
      Value = 54;
      break;
      
    default:
      Value = INT16_MAX;
      break;
  }
  return (Value);
}

/**

@brief
  Function returns the minimum and maximum offsets that can be applied to the margin group
  and the time delay in micro seconds for the new value to take effect.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      LogicRank   - Zero based rank number in the channel.
  @param[in]      IoLevel     - Id of the I/O level to access.
  @param[in]      MarginGroup - Id of the margin group.
  @param[in]      MinOffset   - Minimum offset supported by the given margin group.
  @param[in]      MaxOffset   - Maximum offset supported by the given margin group.
  @param[out]     Delay       - Wait time in micro-seconds that is required for the new setting to take effect.

  @retval Success or error code.

**/
static
SSA_STATUS
GetMarginParamLimits (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 LogicRank,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 *MinOffset,
  INT16                 *MaxOffset,
  UINT16                *Delay
  )
{
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsRankPresent (MrcData, Socket, Controller, Channel, LogicRank % MAX_RANK_IN_DIMM, LogicRank)) {
    *MinOffset = GetMarginParamMin (IoLevel, MarginGroup);
    *MaxOffset = GetMarginParamMax (IoLevel, MarginGroup);
  *Delay = 0;
  }
  return (Success);
}

/**

@brief
  Function used to adjust a margin parameter.It will add an offset from the training value
  (if memory has been trained) or from the default value (if memory has not been trained yet).

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      LogicRank   - Zero based rank number in the channel.
  @param[in]      IoLevel     - Id of the I/O level to access.
  @param[in]      MarginGroup - Id of the margin group. Can be RcvEna(0), RdT(1), WrT(2), WrDqsT(3), RdV(4) or WrV(5).
  @param[in]      Offset      - Offset to be applied to the Margin parameter from the nominal.

  @retval Nothing.

**/
static
SSA_STATUS
OffsetMarginParam (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 LogicRank,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  UINT16                Offset
  )
{
  UINT8 Byte;
  UINT8 ByteEnd;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsRankPresent (MrcData, Socket, Controller, Channel, LogicRank % MAX_RANK_IN_DIMM, LogicRank)) {
    if (MarginGroup < WrLevel) {
      ByteEnd = MrcData->SysOut.Outputs.SdramCount;
      for (Byte = 0; Byte < ByteEnd; Byte++) {
        if ((MarginGroup != WrV) || (Byte == 0)) {
          ChangeMargin(
            MrcData,
            MarginGroup,          // param
            (S32) Offset,         // value0
            0,                    // value1
            0,                    // EnMultiCast
            Channel,              // ch
            LogicRank,            // rank
            Byte,                 // byte
            0,                    // bit
            0,                    // UpdateMrcData
            1,                    // SkipWait
            MrcRegFileStart
            );
        } // if
      } // for
    } else {
      return (UnsupportedValue);
    } // if
  } else {
    return (LogicalRankNotSupported);
  } // if
  return (Success);
}

/**

@brief
  Function used to write to the Write Data Buffer (WDB).

  @param[in, out] PeiServices         - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      Pattern             - Buffer containing the WDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in term of the count of cachelines.
  @param[in]      StartCachelineIndex - Start offset on the WDB.

  @retval Nothing.

**/
static
VOID
SetWdbPattern (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                *Pattern,
  UINT8                 CachelineCount,
  UINT8                 StartCachelineIndex
  )
{
  UINT64_STRUCT *Pointer;
  MCHBAR_CH0_CR_QCLK_LDAT_PDAT_STRUCT CrQclkLdatPdat;
  MCHBAR_CH0_CR_QCLK_LDAT_SDAT_STRUCT CrQclkLdatSdat;
  UINT32 CrQclkLdatDatain0Offset;
  UINT32 CrQclkLdatDatain1Offset;
  UINT32 CrQclkLdatSdatOffset;
  UINT32 CrQclkLdatPdatOffset;
  UINT8 PatternCachelineIdx;
  UINT8 Chunk;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    CrQclkLdatDatain0Offset = MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG +
      ((MCHBAR_CH1_CR_QCLK_LDAT_DATAIN_0_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG) * Channel);
    CrQclkLdatDatain1Offset = MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_1_REG +
      ((MCHBAR_CH1_CR_QCLK_LDAT_DATAIN_1_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_1_REG) * Channel);
    CrQclkLdatSdatOffset = MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG +
      ((MCHBAR_CH1_CR_QCLK_LDAT_SDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG) * Channel);
    CrQclkLdatPdatOffset = MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG +
      ((MCHBAR_CH1_CR_QCLK_LDAT_PDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG) * Channel);

    CrQclkLdatSdat.Data = 0;
    CrQclkLdatSdat.Bits.MODE = 1;

    CrQclkLdatPdat.Data = 0;
    CrQclkLdatPdat.Bits.CMDB = MRC_BIT3;

    for (PatternCachelineIdx = 0; PatternCachelineIdx < CachelineCount; PatternCachelineIdx++) {
      Pointer = (UINT64_STRUCT *) &Pattern[PatternCachelineIdx];
      for (Chunk = 0; Chunk < MAX_CHUNK_SIZE; Chunk++) {
        WriteMem (PeiServices, This, RegWidth32, CrQclkLdatDatain0Offset, &Pointer[Chunk].Data32.Low);
        WriteMem (PeiServices, This, RegWidth32, CrQclkLdatDatain1Offset, &Pointer[Chunk].Data32.High);

        // Set rep = 0, don't want to replicate the data.
        // Set banksel field to the value of the chunk you want to write the 64 bits to.
        // Set arraysel = 0 (indicating it is the MC WDB) and mode = 'b01 in the SDAT register.
        CrQclkLdatSdat.Bits.BANKSEL = Chunk;
        WriteMem (PeiServices, This, RegWidth32, CrQclkLdatSdatOffset, &CrQclkLdatSdat.Data);

        // Finally, write the PDAT register indicating which cacheline of the WDB you want to write to
        // by setting fastaddr field to one of the 64 cache lines. Also set cmdb in the pdat register to 4'b1000,
        // indicating that this is a LDAT write.
        CrQclkLdatPdat.Bits.FASTADDR = StartCachelineIndex + PatternCachelineIdx;
        WriteMem (PeiServices, This, RegWidth32, CrQclkLdatPdatOffset, &CrQclkLdatPdat.Data);
      } // Chunk

      // Turn off LDAT mode after writing to WDB is complete.
      CrQclkLdatSdat.Data = 0;
      WriteMem (PeiServices, This, RegWidth32, CrQclkLdatSdatOffset, &CrQclkLdatSdat.Data);
    } // PatternCachelineIdx
  }
  return;
}

/**

@brief
  Function used to write to the CADB.

  @param[in, out] PeiServices         - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      Pattern             - Buffer containing the WDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in term of the count of cachelines.
  @param[in]      StartCachelineIndex - Start offset on the WDB.

  @retval Nothing.

**/
static
VOID
SetCadbPattern (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                *Pattern,
  UINT8                 CachelineCount,
  UINT8                 StartCachelineIndex
  )
{
  UINT32 Offset;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG) * Channel);
    MrcWriteCR (MrcData, Offset, StartCachelineIndex % MAX_CADB_ENTRIES);

    CachelineCount %= MAX_CADB_ENTRIES;
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_PROG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG) * Channel);
    while (CachelineCount--) {
      // Write Row. CADB is auto incremented after every write
      MrcWriteCR64 (MrcData, Offset, *Pattern++);
    }
  }
  return;
}

/**

@brief
  Function used to clear the lane error status registers.

  @param[in, out] PeiServices  - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      ChannelMask  - Each bit represents a channel to be cleared.

  @retval Nothing.

**/
static
SSA_STATUS
ClearErrorStatus (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelMask
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_STRUCT CrReutChErrDataStatus;
  MrcParameters *MrcData;
  UINT8         Channel;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsControllerPresent (MrcData, Socket, Controller)) {
    if (ChannelMask > 0) {
      Channel = 0;
      while (ChannelMask) {
        if ((ChannelMask & 1) && IsChannelPresent (MrcData, Socket, Controller, Channel)) {
          CrReutChErrDataStatus.Data = 0;
          WriteMem (
            PeiServices,
            This,
            RegWidth64,
            MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG +
            (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG)),
            &CrReutChErrDataStatus.Data);
        }
        Channel++;
        ChannelMask >>= 1;
      }
    } else {
      return (UnsupportedValue);
    }
  } else {
    return (ControllerNotSupported);
  }
  return (Success);
}

/**

@brief
  Function used to clear the error counter register.

  @param[in, out] PeiServices  - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Counter      - Zero based counter number.

  @retval Nothing.

**/
static
SSA_STATUS
ClearErrorCounter (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_STRUCT CrReutChErrCounterStatus0;
  UINT32  Offset;
  UINT8   Byte;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG +
      (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG));
    CrReutChErrCounterStatus0.Data = 0;
    for (Byte = 0; Byte < MrcData->SysOut.Outputs.SdramCount; Byte++) {
      WriteMem (PeiServices, This, RegWidth32, Offset, &CrReutChErrCounterStatus0.Data);
      Offset += MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_1_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG;
    }
  } else {
    return (ChannelNotSupported);
  }
  return (Success);
}

/**

@brief
  Function used to get the DQ lane error status.

  @param[in, out] PeiServices  - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.

  @retval The DQ lane error status..

**/
static
UINT64
GetDqErrorStatus (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_STRUCT CrReutChErrDataStatus;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  CrReutChErrDataStatus.Data = 0;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    ReadMem (
      PeiServices,
      This,
      RegWidth64,
      MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG +
      (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG)),
      &CrReutChErrDataStatus.Data);
  }
  return (CrReutChErrDataStatus.Data);
}

/**

@brief
  Function used to get the ECC lane error status.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.

  @retval Nothing.

**/
static
UINT8
GetEccErrorStatus (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_STRUCT Status;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Status.Data = 0;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    ReadMem (
      PeiServices,
      This,
      RegWidth32,
      MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
      (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
      MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG)),
      &Status.Data);
  }
  return ((UINT8) Status.Bits.ECC_Error_Status);
}

/**

@brief
  Function used to get the ECC lane error status.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Counter     - Zero based counter number.
  @param[in]      CounterMode - Enum that indicates the counter mode to be used. Count on all lanes,
                                count on a particular lane, count on a byte group, count on a particular chunk.
  @param[in]      ModeIndex   - Extra index used to provide additional information if needed by the mode selected.
                                This indicates which lane, byte group or chunk has been selected.

  @retval Nothing.

**/
static
SSA_STATUS
SetErrorCounterMode (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter,
  COUNTER_MODE          CounterMode,
  UINT32                ModeIndex
  )
{
  UINT32                                         Offset;
  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_STRUCT ReutChErrCounterCtl;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    ReutChErrCounterCtl.Data = 0;
    switch (CounterMode) {
      case AllLanes:
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_CTL_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Channel);
        break;

      case ParticularLane:
      case ParticularByteGroup:
      case ParticularChunk:
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_CTL_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Channel) +
          ((MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_1_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Counter);
        ReutChErrCounterCtl.Bits.Counter_Pointer = ModeIndex;
        ReutChErrCounterCtl.Bits.Counter_Control = CounterMode;
        break;

      default:
        return (UnsupportedValue);
    }
    MrcWriteCR (MrcData, Offset, ReutChErrCounterCtl.Data);
  }
  return (Success);
}

/**

@brief
  Function used to get the error count value for a given channel on a given socket/controller.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Counter     - Zero based counter number.

  @retval Nothing.

**/
static
ERROR_COUNT_32BITS
GetErrorCount (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_STRUCT CrReutErrCounterStatus0;
  ERROR_COUNT_32BITS Count;
  UINT32             Offset;
  UINT8              Byte;
  MrcParameters      *MrcData;

  MrcData    = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  Count.Data = 0;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG +
      (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG));
    for (Byte = 0; Byte < MrcData->SysOut.Outputs.SdramCount; Byte++) {
      ReadMem (PeiServices, This, RegWidth32, Offset, &CrReutErrCounterStatus0.Data);
      Count.Data += CrReutErrCounterStatus0.Data;
      Offset += MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_1_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG;
    }
  }
  if (Count.Data > 0x7FFFFFFF) {
    Count.Bits.Count    = 0x7FFFFFFF;
    Count.Bits.Overflow = 1;
  }

  return (Count);
}

/**

@brief
  Function used to set the lane validation mask for a give channel on a given socket/controller.
  Only the lanes with the mask bit set will be checked for errors.

  @param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      DqMask      - DQ lanes bitmask.
  @param[in]      EccMask     - ECC lanes bitmask.

  @retval Nothing.

**/
static
VOID
SetValidationBitMask (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                DqMask,
  UINT8                 EccMask
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_STRUCT CrReutErrDataMask;
  MCHBAR_CH0_CR_REUT_CH_ERR_ECC_MASK_STRUCT CrReutChErrEccMask;
  UINT32 Offset;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_DATA_MASK_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG) * Channel);
    CrReutErrDataMask.Data = ~DqMask;
    WriteMem (PeiServices, This, RegWidth64, Offset, &CrReutErrDataMask.Data);

    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_ECC_MASK_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_ECC_MASK_REG - MCHBAR_CH0_CR_REUT_CH_ERR_ECC_MASK_REG) * Channel);
    CrReutChErrEccMask.Data = ~EccMask;
    WriteMem (PeiServices, This, RegWidth8, Offset, &CrReutChErrEccMask.Data);
  }
  return;
}

/**

@brief
  Function used to set the phase mask for a give channel on a given socket/controller.
  Only the phases with the mask bit set will be checked for errors.

  @param[in, out] PeiServices        - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This               - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket             - Zero based CPU socket number.
  @param[in]      Controller         - Zero based controller number.
  @param[in]      Channel            - Zero based channel number.
  @param[in]      CachelineMask      - Mask for the cacheline to be enabled.
  @param[in]      PhaseMask          - Mask for the Phase. One bit for each phase.

  @retval Nothing.

**/
static
VOID
SetValidationPhaseMask (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 CachelineMask,
  UINT8                 PhaseMask
  )
{
  MCHBAR_CH0_CR_REUT_CH_ERR_CTL_STRUCT CrReutChErrCtl;
  UINT32 Offset;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
    CrReutChErrCtl.Data = 0;
    CrReutChErrCtl.Bits.Selective_Error_Enable_Cacheline = CachelineMask;
    CrReutChErrCtl.Bits.Selective_Error_Enable_Chunk = PhaseMask;
    WriteMem (PeiServices, This, RegWidth32, Offset, &CrReutChErrCtl.Data);
  }
  return;
}

/**

@brief
  Function used to run a point test.

  @param[in, out] PeiServices     - An indirect pointer to the PEI Services Table published by the PEI Foundation.
  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      TestParameters  - Architecture-specific test parameters.
  @param[in]      SkipSetup       - Skip the test setup. It is OK to skip the setup after the first test.

  @retval Nothing.

**/
static
VOID
RunPointTest (
  EFI_PEI_SERVICES      **PeiServices,
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  VOID                  *TestParameters,
  BOOLEAN               SkipSetup
  )
{
  const MRC_REUTAddress ReutAddress = {
    {0, 0, 0, 0},    // Start
    {0, 0, 0, 1023}, // Stop
    {0, 0, 0, 0},    // Order
    {0, 0, 0, 0},    // IncRate
    {0, 0, 0, 1}     // IncValue
  };
  //                                  IncRate,  Start, Stop, DQPat
  const MRC_WDBPattern CWdbPattern = {   16,      0,     1, BasicVA};

  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_STRUCT CrReutChPatWdbClMuxCfg;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT CrReutChSeqCfgMcMain0;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT CrReutGlobalCtl;
  MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_STRUCT CrReutGlobalErr;
  MRC_WDBPattern WdbPattern;
  POINT_TEST_PARAMETERS *Params;
  UINT32 Offset;
  UINT16 BurstLength;
  UINT8 DumArr[7];
  UINT8 Channel;
  UINT8 Rank;
  UINT8 TargetRank;
  UINT8 LoopCount;
  MrcParameters *MrcData;

  MrcData = (MrcParameters *) This->SsaMemoryConfig.MrcData;
  if (IsControllerPresent (MrcData, Socket, Controller)) {
    CopyMem (&WdbPattern, &CWdbPattern, sizeof (MRC_WDBPattern));
    SetMem (DumArr, 1, sizeof (DumArr));
    Params = (POINT_TEST_PARAMETERS *) TestParameters;

    // Program the set up the test for each channel.
    if (!SkipSetup) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
          // Update the WDB pattern incRate, start and stop.
          WdbPattern.IncRate = Params->WdbIncRates[Channel];
          WdbPattern.Stop    = Params->WdbEnds[Channel];

          // if (aggressor) traffic is WR or RD, need double the burst length to make it overlap with
          // (victim) loopback traffic.
          switch (Params->TrafficModes[Channel].Bits.TrafficMode) {
            case TrafficModeWrite:              // PatWr (Write Only)
            case TrafficModeRead:               // PatRd (Read Only)
              BurstLength = Params->BurstLength * 2;
              LoopCount   = Params->LoopCount + 1;
              break;

            case TrafficModeWrRd:               // PatWrRd (Standard Write/Read Loopback)
            default:
              BurstLength = Params->BurstLength;
              LoopCount   = Params->LoopCount;
              break;
          }
          SetupIOTest (MrcData,
            (MRC_BIT0 << Channel),                          // ChbitMask,
            Params->TrafficModes[Channel].Bits.TrafficMode, // CmdPat,
            BurstLength,                                    // NumCL,
            LoopCount,                                      // LC,
            &ReutAddress,                                   // REUTAddress,
            Params->StopOnErr,                              // SOE,
            &WdbPattern,                                    // WDBPattern,
            Params->EnCadb[Channel],                        // EnCADB,
            0,                                              // EnCKE,
            0);                                             // SubSeqWait

          // Set up LFSR or fix pattern modes.
          if (Params->Modes[Channel].Bits.PatternMode == PatternModeFixed) {
            // Sequentially walk through the WDB.
            CrReutChPatWdbClMuxCfg.Data = 0;
            CrReutChPatWdbClMuxCfg.Bits.Mux2_Control = 1;
            CrReutChPatWdbClMuxCfg.Bits.Mux1_Control = 1;
            CrReutChPatWdbClMuxCfg.Bits.Mux0_Control = 1;
            CrReutChPatWdbClMuxCfg.Bits.ECC_Data_Source_Sel = 1;
            Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG +
              ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG) * Channel);
            WriteMem (PeiServices, This, RegWidth32, Offset, &CrReutChPatWdbClMuxCfg.Data);
          }

          // Update the target rank.
          TargetRank = 0;
          for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
            // support one rank now
            if (Params->Ranks[Channel] & (1 << Rank)) {
              TargetRank = Rank;
              break;
            }
          } // Rank
          SelectReutRanks (MrcData, Channel, (1 << TargetRank), 0);
        } // if
      } // Channel

      // The SetupIOTest() disables channel's global control, we need to enable them.
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (IsChannelPresent (MrcData, Socket, Controller, Channel)) {
          Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
            ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
          ReadMem (PeiServices, This, RegWidth64, Offset, &CrReutChSeqCfgMcMain0.Data);
          CrReutChSeqCfgMcMain0.Bits.Global_Control = 1;
          WriteMem (PeiServices, This, RegWidth64, Offset, &CrReutChSeqCfgMcMain0.Data);
        } // if
      } // Channel

      // Run test
      RunIOTest (MrcData, // MrcParameters *MrcData,
        3,                // U8 ChbitMask,
        BasicVA,          // U8 DQPat,
        DumArr,           // U8 *SeqLCs,
        0,                // U8 ClearErrors,
        0);
    } else {
      // bypassSetup. Only issue test start bit. The not !bypassSetup needed to be called
      // first to set up the system.
      CrReutGlobalCtl.Data = 0;
      CrReutGlobalCtl.Bits.Global_Start_Test = 1;
      WriteMem (PeiServices, This, RegWidth32, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, &CrReutGlobalCtl.Data);

      // Wait until channel test done.
      do {
        ReadMem (PeiServices, This, RegWidth32, MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_REG, &CrReutGlobalErr.Data);
      }
      while (!CrReutGlobalErr.Bits.Channel_Test_Done_Status_0 || !CrReutGlobalErr.Bits.Channel_Test_Done_Status_1);
    }
  }
  return;
}

const SSA_BIOS_SERVICES_PPI SsaBiosServicesConst = {
  {
    SSA_REVISION_BIOS,
    0,                                  // *SsaCommonConfig
    0,                                  // *SsaMemoryConfig
  },
  {
    SSA_REVISION_COMMON,
    0,                                  // MrcData
    ReadMem,
    WriteMem,
    ReadIo,
    WriteIo,
    ReadPci,
    WritePci,
    GetBaseAddress,
    Malloc,
    Free,
    SsaDebugPrint,
  },
  {
    SSA_REVISION_MEMORY,
    0,                                  // MrcData
    GetSystemInfo,
    GetMemVoltage,
    SetMemVoltage,
    GetMemTemp,                         // @todo: not implemented yet
    RestoreMrs,
    WriteMrs,
    ReadMrs,
    GetDimmFromLogicalRank,
    GetDimmInfo,
    GetRankInDimm,
    GetLogicalRankBitMask,
    GetChannelBitMask,
    GetControllerBitMask,
    JedecReset,
    GetMarginParamLimits,
    OffsetMarginParam,
    SetWdbPattern,
    SetCadbPattern,
    ClearErrorStatus,
    ClearErrorCounter,
    GetDqErrorStatus,
    GetEccErrorStatus,
    SetErrorCounterMode,
    GetErrorCount,
    SetValidationBitMask,
    SetValidationPhaseMask,
    RunPointTest
  }
};

/**

@brief
  Initialize the SsaBiosServices data structure.

  @param[in] MrcData  - The MRC global data area.

  @retval Nothing

**/
VOID
SsaBiosInitialize (
  IN MrcParameters       *MrcData
  )
{
  EFI_PEI_SERVICES       **PeiServices;
  SSA_BIOS_SERVICES_PPI  *SsaBiosServicesPpi;
  EFI_PEI_PPI_DESCRIPTOR *SsaBiosServicesPpiDesc;
  EFI_STATUS             Status;

  SsaBiosServicesPpi = (SSA_BIOS_SERVICES_PPI *) AllocatePool (sizeof (SSA_BIOS_SERVICES_PPI));
  ASSERT (SsaBiosServicesPpi != NULL);
  SsaBiosServicesPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocatePool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (SsaBiosServicesPpiDesc != NULL);

  CopyMem (SsaBiosServicesPpi, &SsaBiosServicesConst, sizeof (SSA_BIOS_SERVICES_PPI));
  SsaBiosServicesPpi->SsaHeader.SsaCommonConfig = &SsaBiosServicesPpi->SsaCommonConfig;
  SsaBiosServicesPpi->SsaHeader.SsaMemoryConfig = &SsaBiosServicesPpi->SsaMemoryConfig;
  SsaBiosServicesPpi->SsaCommonConfig.BiosData = MrcData;
  SsaBiosServicesPpi->SsaMemoryConfig.MrcData  = MrcData;

  EfiCommonLibZeroMem (SsaBiosServicesPpiDesc, sizeof (EFI_PEI_PPI_DESCRIPTOR));
  SsaBiosServicesPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SsaBiosServicesPpiDesc->Guid   = &gSsaBiosServicesPpiGuid;
  SsaBiosServicesPpiDesc->Ppi    = SsaBiosServicesPpi;

  PeiServices = (EFI_PEI_SERVICES **) MrcData->SysIn.Inputs.Debug.Stream;
  Status = (**PeiServices).InstallPpi (PeiServices, SsaBiosServicesPpiDesc);
  ASSERT_EFI_ERROR (Status);

  MRC_DEBUG_MSG (&MrcData->SysIn.Inputs.Debug, MSG_LEVEL_NOTE, "SSA Interface ready\n");

  return;
}

#endif // SSA_FLAG
