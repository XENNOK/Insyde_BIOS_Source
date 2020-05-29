/** @file
  Prototypes and defines for the PCH SMM Dispatcher.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmm.h

@brief
  Prototypes and defines for the PCH SMM Dispatcher.

**/
#ifndef PCH_SMM_H
#define PCH_SMM_H

//
// Driver Consumed Protocol Prototypes
//
//
// Used during initialization
//
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LoadedImage.h>
//
// Used during SMI dispatch
//
#include <Protocol/SmmBase.h>
#include <Protocol/SmmControl.h>
//
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUPPORTED PROTOCOLS
//
#include <Protocol/SmmUsbDispatch.h>
#include <Protocol/SmmSxDispatch.h>
#include <Protocol/SmmSwDispatch.h>
#include <Protocol/SmmGpiDispatch.h>
#include <Protocol/SmmIchnDispatch.h>
#include <Protocol/SmmIchnDispatchEx.h>
#include <Protocol/SmmPowerButtonDispatch.h>
#include <Protocol/SmmPeriodicTimerDispatch.h>
#include <PchAccess.h>
#include <PchPlatformLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PerformanceLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DxeServicesTableLib.h>


#define EFI_BAD_POINTER          0xAFAFAFAF

///
/// Define an enumeration for all the supported protocols
///
typedef enum {
  UsbType,
  SxType,
  SwType,
  GpiType,
  IchnType,
  IchnExType,
  PowerButtonType,
  PeriodicTimerType,
  PchSmmProtocolTypeMax
} PCH_SMM_PROTOCOL_TYPE;

///
/// SPECIFYING A REGISTER
/// We want a general way of referring to addresses.  For this case, we'll only
/// need addresses in the ACPI table (and the TCO entries within the ACPI table).
/// However, it's interesting to consider what it would take to support other types
/// of addresses.  To address Will's concern, I think it prudent to accommodate it
/// early on in the design.
///
/// Addresses we need to consider:
///
///  Type:                           Required:
///  I/O                             Yes
///    ACPI (special case of I/O)    Only if we want to
///    TCO  (special case of ACPI)   Only if we want to
///    GPIO  (special case of I/O)    Only if we want to
///  Memory (or Memory Mapped I/O)   Only if we want to
///  PCIE                            Yes, for BiosWp
///
typedef enum {
  ///
  ///  IO_ADDR_TYPE, /// unimplemented
  ///
  ACPI_ADDR_TYPE,
  GPIO_ADDR_TYPE,
  ///
  ///  MEMORY_ADDR_TYPE, /// unimplemented
  ///
  MEMORY_MAPPED_IO_ADDRESS_TYPE,
  PCIE_ADDR_TYPE,
  NUM_ADDR_TYPES,                     ///< count of items in this enum
  PCH_SMM_ADDR_TYPE_NULL        = -1  ///< sentinel to indicate NULL or to signal end of arrays
} ADDR_TYPE;

//
// Assumption: 32-bits -- enum's evaluate to integer
// Assumption: This code will only run on IA-32.  Justification: IA-64 doesn't have SMIs.
// We don't have to worry about 64-bit addresses.
// Typedef the size of addresses in case the numbers I'm using are wrong or in case
// this changes.  This is a good idea because PCI_ADDR will change, for example, when
// we add support for PciExpress.
//
typedef UINT16 IO_ADDR;
typedef IO_ADDR GPIO_ADDR;
typedef IO_ADDR ACPI_ADDR;  ///< can omit
typedef IO_ADDR TCO_ADDR;   ///< can omit
typedef UINTN MEM_ADDR;
typedef MEM_ADDR *MEMORY_MAPPED_IO_ADDRESS;
typedef union {
  UINT32  Raw;
  struct {
    UINT8 Reg;
    UINT8 Fnc;
    UINT8 Dev;
    UINT8 Bus;
  } Fields;
} PCIE_ADDR;

typedef struct {
  ADDR_TYPE Type;
  union {
    ///
    /// used to initialize during declaration/definition
    ///
    UINT32                    raw;

    ///
    /// used to access useful data
    ///
    IO_ADDR                   io;
    ACPI_ADDR                 acpi;
    TCO_ADDR                  tco;
    GPIO_ADDR                 gpio;
    MEM_ADDR                  mem;
    MEMORY_MAPPED_IO_ADDRESS  Mmio;
    PCIE_ADDR                 pcie;

  } Data;

} PCH_SMM_ADDRESS;

///
/// SPECIFYING BITS WITHIN A REGISTER
/// Here's a struct that helps us specify a source or enable bit.
///
typedef struct {
  PCH_SMM_ADDRESS Reg;
  UINT8           SizeInBytes;  ///< of the register
  UINT8           Bit;
} PCH_SMM_BIT_DESC;

//
// Sometimes, we'll have bit descriptions that are unused.  It'd be great to have a
// way to easily identify them:
//
#define IS_BIT_DESC_NULL(BitDesc)   ((BitDesc).Reg.Type == PCH_SMM_ADDR_TYPE_NULL)  ///< "returns" true when BitDesc is NULL
#define NULL_THIS_BIT_DESC(BitDesc) ((BitDesc).Reg.Type = PCH_SMM_ADDR_TYPE_NULL)   ///< will "return" an integer w/ value of 0
#define NULL_BIT_DESC_INITIALIZER \
  { \
    { \
      PCH_SMM_ADDR_TYPE_NULL, \
      { \
        0 \
      } \
    }, \
    0, 0 \
  }
//
// I'd like a type to specify the callback's Sts & En bits because they'll
// be commonly used together:
//
#define NUM_EN_BITS   2
#define NUM_STS_BITS  1

//
// Flags
//
typedef UINT8 PCH_SMM_SOURCE_FLAGS;

//
// Flags required today
//
#define PCH_SMM_NO_FLAGS          0
#define PCH_SMM_SCI_EN_DEPENDENT  1

//
// Flags that might be required tomorrow
//
///
/// #define PCH_SMM_CLEAR_WITH_ONE 2 /// may need to support bits that clear by writing 0
/// #define PCH_SMM_MULTIBIT_FIELD 3 /// may need to support status/enable fields 2 bits wide
///
typedef struct {
  PCH_SMM_SOURCE_FLAGS  Flags;
  PCH_SMM_BIT_DESC      En[NUM_EN_BITS];
  PCH_SMM_BIT_DESC      Sts[NUM_STS_BITS];
} PCH_SMM_SOURCE_DESC;
//
// 31 bytes, I think
//
#define NULL_SOURCE_DESC_INITIALIZER \
  { \
    PCH_SMM_NO_FLAGS, \
    { \
      NULL_BIT_DESC_INITIALIZER, NULL_BIT_DESC_INITIALIZER \
    }, \
    { \
      NULL_BIT_DESC_INITIALIZER \
    } \
  }

///
/// CHILD CONTEXTS
/// To keep consistent w/ the architecture, we'll need to provide the context
/// to the child when we call its callback function.  After talking with Will,
/// we agreed that we'll need functions to "dig" the context out of the hardware
/// in many cases (Sx, Trap, Gpi, etc), and we'll need a function to compare those
/// contexts to prevent unnecessary dispatches.  I'd like a general type for these
/// "GetContext" functions, so I'll need a union of all the protocol contexts for
/// our internal use:
///
typedef union {
  //
  // (in no particular order)
  //
  EFI_SMM_ICHN_DISPATCH_CONTEXT           Ichn;
  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        IchnEx;
  EFI_SMM_SX_DISPATCH_CONTEXT             Sx;
  EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT PeriodicTimer;
  EFI_SMM_SW_DISPATCH_CONTEXT             Sw;
  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   PowerButton;
  EFI_SMM_USB_DISPATCH_CONTEXT            Usb;
  EFI_SMM_GPI_DISPATCH_CONTEXT            Gpi;
} PCH_SMM_CONTEXT;
//
// Assumption: PeriodicTimer largest at 3x64-bits or 24 bytes
//
typedef struct _DATABASE_RECORD DATABASE_RECORD;

///
/// Assumption: the GET_CONTEXT function will be as small and simple as possible.
/// Assumption: We don't need to pass in an enumeration for the protocol because each
///    GET_CONTEXT function is written for only one protocol.
/// We also need a function to compare contexts to see if the child should be dispatched
///
typedef
VOID
(EFIAPI *GET_CONTEXT) (
  IN  DATABASE_RECORD    * Record,
  OUT PCH_SMM_CONTEXT    * Context
  );

typedef
BOOLEAN
(EFIAPI *CMP_CONTEXT) (
  IN PCH_SMM_CONTEXT     * Context1,
  IN PCH_SMM_CONTEXT     * Context2
  );

///
/// Finally, every protocol will require a "Get Context" and "Compare Context" call, so
/// we may as well wrap that up in a table, too.
///
typedef struct {
  GET_CONTEXT GetContext;
  CMP_CONTEXT CmpContext;
} CONTEXT_FUNCTIONS;

extern CONTEXT_FUNCTIONS          ContextFunctions[PchSmmProtocolTypeMax];

///
/// MAPPING CONTEXT TO BIT DESCRIPTIONS
/// I'd like to have a general approach to mapping contexts to bit descriptions.
/// Sometimes, we'll find that we can use table lookups or constant assignments;
/// other times, we'll find that we'll need to use a function to perform the mapping.
/// If we define a macro to mask that process, we'll never have to change the code.
/// I don't know if this is desirable or not -- if it isn't, then we can get rid
/// of the macros and just use function calls or variable assignments.  Doesn't matter
/// to me.
/// Mapping complex contexts requires a function
///


/**
  Maps a USB context to a source description.

  @param[in] Context              The context we need to map.  Type must be USB.
  @param[out] SrcDesc             The source description that corresponds to the given context.

  @retval None
**/
VOID
MapUsbToSrcDesc (
  IN  PCH_SMM_CONTEXT         *Context,
  OUT PCH_SMM_SOURCE_DESC     *SrcDesc
  );

/**
  Figure out which timer the child is requesting and
  send back the source description

  @param[in] DispatchContext      The pointer to the Dispatch Context instances
  @param[out] SrcDesc             The pointer to the source description

  @retval None
**/
VOID
MapPeriodicTimerToSrcDesc (
  IN  PCH_SMM_CONTEXT                                         *DispatchContext,
  OUT PCH_SMM_SOURCE_DESC                                     *SrcDesc
  );

//
// Mapping simple contexts can be done by assignment or lookup table
//
extern const PCH_SMM_SOURCE_DESC  SW_SOURCE_DESC;
extern const PCH_SMM_SOURCE_DESC  SX_SOURCE_DESC;
extern const PCH_SMM_SOURCE_DESC  POWER_BUTTON_SOURCE_DESC;

//
// With the changes we've made to the protocols, we can now use table
// lookups for the following protocols:
//
#define NUM_SUPPORTED_GPIS  16
extern const PCH_SMM_SOURCE_DESC  LPTH_GPI_SOURCE_DESC[NUM_SUPPORTED_GPIS];
extern const PCH_SMM_SOURCE_DESC  LPTLP_GPI_SOURCE_DESC[NUM_SUPPORTED_GPIS];

extern PCH_SMM_SOURCE_DESC        ICHN_H_SOURCE_DESCS[NUM_ICHN_TYPES];
extern PCH_SMM_SOURCE_DESC        ICHN_LP_SOURCE_DESCS[NUM_ICHN_TYPES];
extern PCH_SMM_SOURCE_DESC        ICHN_EX_SOURCE_DESCS[IchnExTypeMAX - IchnExPciExpress];

///
/// For PCHx, APMC is UINT8 port, so the MAX SWI Value is 0xFF.
///
#define MAXIMUM_SWI_VALUE 0xFF

///
/// GENERALIZING THE CALLBACK
/// All SmmXxxDispatch callbacks have the same form:
///
/// VOID Callback( EFI_HANDLE, EFI_SMM_Xxx_DISPATCH_CONTEXT )
/// We need a typedef that'll allow us to call any callback
///
typedef
VOID
(EFIAPI *PCH_SMM_CALLBACK) (
  IN  EFI_HANDLE                      Handle,
  IN  PCH_SMM_CONTEXT                 * Context
  );

///
/// Open: Need to make sure this kind of type cast will actually work.
///   May need an intermediate form w/ two VOID* arguments.  I'll figure
///   that out when I start compiling.
///
typedef
VOID
(EFIAPI *PCH_SMM_CLEAR_SOURCE) (
  PCH_SMM_SOURCE_DESC * SrcDesc
  );

///
/// "DATABASE" RECORD
/// Linked list data structures
///
#define DATABASE_RECORD_SIGNATURE SIGNATURE_32 ('D', 'B', 'R', 'C')

struct _DATABASE_RECORD {
  UINT32                Signature;
  LIST_ENTRY            Link;

  ///
  /// Status and Enable bit description
  ///
  PCH_SMM_SOURCE_DESC   SrcDesc;

  ///
  /// Callback function
  ///
  PCH_SMM_CALLBACK      Callback;
  PCH_SMM_CONTEXT       ChildContext;

  ///
  /// Special handling hooks -- init them to NULL if unused/unneeded
  ///
  PCH_SMM_CLEAR_SOURCE  ClearSource;  ///< needed for SWSMI timer

  ///
  /// Functions required to make callback code general
  ///
  CONTEXT_FUNCTIONS     ContextFunctions;

  ///
  /// The protocol that this record dispatches
  ///
  PCH_SMM_PROTOCOL_TYPE ProtocolType;

};

#define DATABASE_RECORD_FROM_LINK(_record)  CR (_record, DATABASE_RECORD, Link, DATABASE_RECORD_SIGNATURE)

///
/// HOOKING INTO THE ARCHITECTURE
///
typedef
EFI_STATUS
(EFIAPI *PCH_SMM_GENERIC_REGISTER) (
  IN  VOID                                    **This,
  IN  VOID                                    *DispatchFunction,
  IN  VOID                                    *DispatchContext,
  OUT EFI_HANDLE                              * DispatchHandle
  );
typedef
EFI_STATUS
(EFIAPI *PCH_SMM_GENERIC_UNREGISTER) (
  IN  VOID                                    **This,
  IN  EFI_HANDLE                              DispatchHandle
  );

///
/// Define a memory "stamp" equivalent in size and function to most of the protocols
///
typedef struct {
  PCH_SMM_GENERIC_REGISTER    Register;
  PCH_SMM_GENERIC_UNREGISTER  Unregister;
  UINTN                       Extra1;
  UINTN                       Extra2; ///< may not need this one
} PCH_SMM_GENERIC_PROTOCOL;

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the PCH_SMM_GENERIC_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to dispatch function to be invoked for this SMI source.
  @param[in] DispatchContext      Pointer to the dispatch function's context.
  @param[out] DispatchHandle      Handle of dispatch function, for when interfacing
                                  with the parent SMM driver, will be the address of linked
                                  list link in the call back record.

  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  registered and the SMI source has been enabled.
**/
EFI_STATUS
PchSmmCoreRegister (
  IN  PCH_SMM_GENERIC_PROTOCOL                          *This,
  IN  PCH_SMM_CALLBACK                                  DispatchFunction,
  IN  PCH_SMM_CONTEXT                                   *DispatchContext,
  OUT EFI_HANDLE                                        *DispatchHandle
  );

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of dispatch function to deregister.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered and the SMI source has been disabled
                                  if there are no other registered child dispatch
                                  functions for this SMI source.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
EFI_STATUS
PchSmmCoreUnRegister (
  IN  PCH_SMM_GENERIC_PROTOCOL                         *This,
  IN  EFI_HANDLE                                       *DispatchHandle
  );

typedef union {
  PCH_SMM_GENERIC_PROTOCOL                  Generic;

  EFI_SMM_USB_DISPATCH_PROTOCOL             Usb;
  EFI_SMM_SX_DISPATCH_PROTOCOL              Sx;
  EFI_SMM_SW_DISPATCH_PROTOCOL              Sw;
  EFI_SMM_GPI_DISPATCH_PROTOCOL             Gpi;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL            Ichn;
  EFI_SMM_ICHN_DISPATCH_EX_PROTOCOL         IchnEx;
  EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL    PowerButton;
  EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL  PeriodicTimer;
} PCH_SMM_PROTOCOL;

///
/// Define a structure to help us identify the generic protocol
///
#define PROTOCOL_SIGNATURE  SIGNATURE_32 ('P', 'R', 'O', 'T')

typedef struct {
  UINTN                 Signature;

  PCH_SMM_PROTOCOL_TYPE Type;
  PCH_SMM_PROTOCOL      Protocols;
} PCH_SMM_QUALIFIED_PROTOCOL;

#define QUALIFIED_PROTOCOL_FROM_GENERIC(_generic) \
  CR ( \
  _generic, \
  PCH_SMM_QUALIFIED_PROTOCOL, \
  Protocols, \
  PROTOCOL_SIGNATURE \
  )

///
/// Create private data for the protocols that we'll publish
///
typedef struct {
  LIST_ENTRY                  CallbackDataBase;
  EFI_HANDLE                  InstallMultProtHandle;
  PCH_SMM_QUALIFIED_PROTOCOL  Protocols[PchSmmProtocolTypeMax];
} PRIVATE_DATA;

extern PRIVATE_DATA           mPrivateData;
extern EFI_SMM_SYSTEM_TABLE   *mSmst;
extern UINT32                 mAcpiBaseAddr;
extern UINT32                 mGpioBaseAddr;
extern EFI_SMM_BASE_PROTOCOL  *mSmmBase;

/**
  Get the Software Smi value

  @param[in] Record               No use
  @param[out] Context             The context that includes Software Smi value to be filled

  @retval None
**/
VOID
EFIAPI
SwGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  );

/**
  Check whether software SMI value of two contexts match

  @param[in] Context1             Context 1 that includes software SMI value 1
  @param[in] Context2             Context 2 that includes software SMI value 2

  @retval FALSE                   Software SMI value match
  @retval TRUE                    Software SMI value don't match
**/
BOOLEAN
EFIAPI
SwCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  );

/**
  Get the Sleep type

  @param[in] Record               No use
  @param[out] Context             The context that includes SLP_TYP bits to be filled

  @retval None
**/
VOID
EFIAPI
SxGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  );

/**
  Check whether sleep type of two contexts match

  @param[in] Context1             Context 1 that includes sleep type 1
  @param[in] Context2             Context 2 that includes sleep type 2

  @retval FALSE                   Sleep types match
  @retval TRUE                    Sleep types don't match
**/
BOOLEAN
EFIAPI
SxCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  );

/**
  Update the elapsed time from the Interval data of DATABASE_RECORD

  @param[in] Record               The pointer to the DATABASE_RECORD.
  @param[out] HwContext           The Context to be updated.

  @retval None

**/
VOID
EFIAPI
PeriodicTimerGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  );

/**
  Check whether Periodic Timer of two contexts match

  @param[in] Context1             Context 1 that includes Periodic Timer  1
  @param[in] Context2             Context 2 that includes Periodic Timer  2

  @retval FALSE                   Periodic Timer match
  @retval TRUE                    Periodic Timer don't match
**/
BOOLEAN
EFIAPI
PeriodicTimerCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  );

/**
  Get the power button status.

  @param[in] Record               The pointer to the DATABASE_RECORD.
  @param[out] Context             Calling context from the hardware, will be updated with the current power button status.

  @retval None
**/
VOID
EFIAPI
PowerButtonGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  );

/**
  Check whether Power Button status of two contexts match

  @param[in] Context1             Context 1 that includes Power Button status 1
  @param[in] Context2             Context 2 that includes Power Button status 2

  @retval FALSE                   Power Button status match
  @retval TRUE                    Power Button status don't match
**/
BOOLEAN
EFIAPI
PowerButtonCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  );

/**
  This function is responsible for calculating and enabling any timers that are required
  to dispatch messages to children. The SrcDesc argument isn't acutally used.

  @param[in] SrcDesc              Pointer to the PCH_SMM_SOURCE_DESC instance.

  @retval None.
**/
VOID
EFIAPI
PchSmmPeriodicTimerClearSource (
  IN PCH_SMM_SOURCE_DESC *SrcDesc
  );

/**
  This services returns the next SMI tick period that is supported by the chipset.
  The order returned is from longest to shortest interval period.

  @param[in] This                 Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param[in, out] SmiTickInterval Pointer to pointer of the next shorter SMI interval period that is supported by the child.

  @retval EFI_SUCCESS             The service returned successfully.
  @retval EFI_INVALID_PARAMETER   The parameter SmiTickInterval is invalid.
**/
EFI_STATUS
PchSmmPeriodicTimerDispatchGetNextShorterInterval (
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL           *This,
  IN OUT UINT64                                         **SmiTickInterval
  );

/**
  When we get an SMI that indicates that we are transitioning to a sleep state,
  we need to actually transition to that state.  We do this by disabling the
  "SMI on sleep enable" feature, which generates an SMI when the operating system
  tries to put the system to sleep, and then physically putting the system to sleep.

  @param[in] None

  @retval None.
**/
VOID
PchSmmSxGoToSleep (
  VOID
  );

/**
  Clear the SMI status bit after the SMI handling is done

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
EFIAPI
PchSmmIchnClearSource (
  IN PCH_SMM_SOURCE_DESC *SrcDesc
  );

/**
  Fix the base address of the source regs and status regs.
  Since Base should get from register filled by platform modules already.

  @param[in] None.

  @retval None.
**/
VOID
PchSmmIchnFixSourceBase (
  VOID
  );

#endif
