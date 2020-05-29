/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <PiSmm.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Variable.h>
#include <Protocol/VariableLock.h>
#include <Protocol/Hash.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/FaultTolerantWriteLite.h>
#include <Protocol/SmmFtw.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmVariable.h>

#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/ImageRelocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MultiConfigBaseLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>
#include <Guid/BootOrderHook.h>
#include <Guid/ReturnFromImage.h>

#include "SecureBoot.h"
#include <MultiConfigStructDef.h>


//
// SMI related definitions
//
#define SW_SMI_PORT                          PcdGet16(PcdSoftwareSmiPort)
#define SECURE_BOOT_SW_SMI                        0xEC
#define SMM_CONTROL_SENSITIVE_CAPABILITY_FUN_NUM  0xF9
#define SMM_VARIABLE_LOCK_FUN_NUM                 0xFA
#define LEGACY_BOOT_SMI_FUN_NUM                   0xFB
#define SMM_SET_VARIABLE_SMI_FUN_NUM              0xFC
#define DISABLE_VARIABLE_CACHE_SMI_FUN_NUM        0xFE
#define DISABLE_SECURE_BOOT_SMI_FUN_NUM           0xFF

//
// Signature related definitions
//
#define SMM_VARIABLE_LOCK_SIGNATURE             SIGNATURE_32 ('s', 'V', 'L', 'L')
#define SMM_LEGACY_BOOT_SIGNATURE               SIGNATURE_32 ('s', 'S', 'L', 'B')
#define SMM_VARIABLE_SIGNATURE                  SIGNATURE_32 ('s', 'm', 'v', 'a')
#define DISABLE_VARIABLE_CACHE_SIGNATURE        SIGNATURE_32 ('s', 'D', 'V', 'C')
#define DISABLE_SET_SENSITIVE_SIGNATURE         SIGNATURE_32 ('s', 'D', 'S', 'S')
#define ENABLE_SET_SENSITIVE_SIGNATURE          SIGNATURE_32 ('s', 'E', 'S', 'S')
#define INSTANCE_FROM_EFI_SMM_VARIABLE_THIS(a)  CR (a, SMM_VARIABLE_INSTANCE, SmmVariable, SMM_VARIABLE_SIGNATURE)

//
// Size related definitions
//
#define VARIABLE_STORE_SIZE               (64 * 1024)
//
// According to Logo requirement, the maximum variable size is 56K,
// so increase scratch size to 60K
//
#define SCRATCH_SIZE                       (60 * 1024)
#define MAX_HARDWARE_ERROR_VARIABLE_SIZE   MAX_VARIABLE_SIZE
#define VARIABLE_RECLAIM_THRESHOLD         (1024)
#define APPEND_BUFF_SIZE                   MAX_VARIABLE_SIZE

//
// Attributes related definitions
//
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x00000008
#define EFI_VARIABLE_ATTRIBUTES_MASK       (EFI_VARIABLE_NON_VOLATILE | \
                                            EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                            EFI_VARIABLE_RUNTIME_ACCESS | \
                                            EFI_VARIABLE_HARDWARE_ERROR_RECORD | \
                                            EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | \
                                            EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | \
                                            EFI_VARIABLE_APPEND_WRITE)

//
// Misc definitions
//
#define MAX_TRY_SET_VARIABLE_TIMES         2
#define EFI_FLASH_AREA_FACTORY_COPY        EFI_FLASH_AREA_RESERVED_09
#define DEFAULT_VARIALBE_NUM               0x04

#define NEED_DO_RECLAIM_NAME               L"NeedDoReclaim"

#define VARIABLE_ATTRIBUTE_BS_RT        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define VARIABLE_ATTRIBUTE_NV_BS_RT     (VARIABLE_ATTRIBUTE_BS_RT | EFI_VARIABLE_NON_VOLATILE)
#define VARIABLE_ATTRIBUTE_NV_BS_RT_AT  (VARIABLE_ATTRIBUTE_NV_BS_RT | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

typedef struct {
  CHAR16      *Name;
  UINT32      Attributes;
} GLOBAL_VARIABLE_ENTRY;

typedef struct {
  VARIABLE_HEADER *CurrPtr;
  VARIABLE_HEADER *EndPtr;
  VARIABLE_HEADER *StartPtr;
  BOOLEAN         Volatile;
} VARIABLE_POINTER_TRACK;

typedef struct {
  EFI_PHYSICAL_ADDRESS  VolatileVariableBase;
  EFI_PHYSICAL_ADDRESS  NonVolatileVariableBase;
  EFI_LOCK              VariableServicesLock;
  UINT32                ReentrantState;
} VARIABLE_GLOBAL;


typedef struct {
  VARIABLE_GLOBAL                    VariableBase;
  UINTN                              VolatileLastVariableOffset;
  UINTN                              NonVolatileLastVariableOffset;
  UINTN                              NonVolatileVariableCacheSize;
  EFI_PHYSICAL_ADDRESS               FactoryDefaultBase;
  UINTN                              FactoryDefaultSize;
  UINT8                              *NonVolatileVariableCache;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *FvbInstance;
  BOOLEAN                            SmmCodeReady;
  BOOLEAN                            SecureBootCallbackEnabled;
  BOOLEAN                            BootOrderVariableHook;
  BOOLEAN                            AtRuntime;
  BOOLEAN                            EndOfDxe;
  BOOLEAN                            VolataileVariableValid;
  BOOLEAN                            InsydeSecureVariableLocked;
  PRESERVED_VARIABLE_TABLE           *PreservedVariableTable;
  UINT32                             PreservedTableSize;
  EFI_SET_VARIABLE                   SmmSetVariable;
} ESAL_VARIABLE_GLOBAL;


typedef struct {
  EFI_PHYSICAL_ADDRESS   FvbBaseAddress;
  EFI_LBA                Lba;
  UINTN                  Offset;
} VARIABLE_STORE_INFO;

typedef struct {
  VOID                  *BackupBuffer;
  EFI_LBA               VariableLba;
  UINTN                 VariableOffset;
  EFI_FTW_LITE_PROTOCOL *FtwLiteProtocol;
} VARIALBE_RECLAIM_INFO;

typedef struct {
  UINT32                                Signature;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;
  EFI_SMM_CPU_PROTOCOL                  *SmmCpu;
  EFI_HANDLE                            SwHandle;
  UINTN                                 CurrentlyExecutingCpu;
  VARIABLE_STORE_INFO                   VariableStoreInfo;
  ESAL_VARIABLE_GLOBAL                  *ProtectedModeVariableModuleGlobal;
} SMM_VARIABLE_INSTANCE;

typedef struct {
  EFI_STATUS                            Status;
  UINT32                                Signature;
  EFI_GUID                              VarGuid;
  UINT32                                Attributes;
  UINTN                                 VariableNameSize;
  UINTN                                 DataSize;
  UINT16                                VarChecksum;
//
//CHAR16      VarName[]; //Input buffer Variable's Name.
//                       //This is a Unicode string and end with Unicode NULL.
//
//UINT8       *VarData;
//
} SMM_VAR_BUFFER;

#define EFI_FREE_POOL(Addr) \
{  mSmst ? mSmst->SmmFreePool((VOID*) (Addr)) : gBS->FreePool ((VOID *) (Addr)); \
   Addr = NULL;}
/**
  Initializes variable store area for non-volatile and volatile variable.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.

**/
EFI_STATUS
VariableCommonInitialize (
  VOID
  );

/**
  Initialize SMM mode NV data

  @param  SmmBase        Pointer to EFI_SMM_BASE_PROTOCOL

  @retval EFI_SUCCESS    Initialize SMM mode NV data successful.
  @retval Other          Any error occured during initialize SMM NV data.

**/
EFI_STATUS
EFIAPI
SmmNvsInitialize (
  IN  EFI_SMM_BASE2_PROTOCOL               *SmmBase
  );

/**
  Callback function for service administer secure boot

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
SecureBootCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  );


/**
  Callback function only for variable service

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
NonSecureBootCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  );

/**
  Setup Variable driver for SMM

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.

  None

**/
VOID
EFIAPI
SetupSmmVariable (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/**
  Callback for SMM runtime

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.

  None

**/
VOID
EFIAPI
SmmRuntimeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );


/**
  Callback for Cryto services

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.

  None

**/
VOID
EFIAPI
CryptoCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/**
  Callback routine to enable hook BootOrder variable mechanic.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
BootOrderHookEnableCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
;

/**
  Callback routine to disable hook BootOrder variable mechanic.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
BootOrderHookDisableCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
;

/**
  The notification of gEfiCryptoServiceProtocolGuid protocol is installed

  @param  Protocol              Points to the protocol's unique identifier.
  @param  Interface             Points to the interface instance.
  @param  Handle                The handle on which the interface was installed.

**/
EFI_STATUS
EFIAPI
CryptoServicesInstalled (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  );

EFI_STATUS
GetVariableStoreInfo (
  IN  EFI_PHYSICAL_ADDRESS   VariableBase,
  OUT EFI_PHYSICAL_ADDRESS   *FvbBaseAddress,
  OUT EFI_LBA                *Lba,
  OUT UINTN                  *Offset
  );

EFI_STATUS
EFIAPI
VariableServiceInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
VariableServicesGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          * VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  );

EFI_STATUS
EFIAPI
VariableServicesGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  );

EFI_STATUS
EFIAPI
VariableServicesSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  );

EFI_STATUS
EFIAPI
VariableServicesQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  );

/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  Otherwise, VariableName and VendorGuid are compared.

  @param  VariableName                Name of the variable to be found.
  @param  VendorGuid                  Vendor GUID to be found.
  @param  PtrTrack                    VARIABLE_POINTER_TRACK structure for output,
                                      including the range searched and the target position.
  @param  PtrTrack                    Variable count  for output.
  @param  Global                      Pointer to VARIABLE_GLOBAL structure, including
                                      base of volatile variable storage area, base of
                                      NV variable storage area, and a lock.

  @retval EFI_INVALID_PARAMETER       If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval EFI_SUCCESS                 Variable successfully found.
  @retval EFI_INVALID_PARAMETER       Variable not found.

**/
EFI_STATUS
FindVariableByLifetime (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  );


/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Guid of variable.
  @param[in] Data               Variable data.
  @param[in] DataSize           Size of data. 0 means delete.
  @param[in] Attributes         Attributes of the variable.
  @param[in] KeyIndex           Index of associated public key.
  @param[in] MonotonicCount     Value of associated monotonic count.
  @param[in] Variable           The variable information that is used to keep track of variable usage.

  @param[in] TimeStamp          Value of associated TimeStamp.
  @param[in] Global             Pointer to VARIABLE_GLOBAL

  @retval EFI_SUCCESS           The update operation is success.
  @retval EFI_OUT_OF_RESOURCES  Variable region is full, cannot write other data into this region.

**/
EFI_STATUS
UpdateVariable (
  IN CONST CHAR16                 *VariableName,
  IN CONST EFI_GUID               *VendorGuid,
  IN CONST VOID                   *Data,
  IN       UINTN                  DataSize,
  IN       UINT32                 Attributes,
  IN       UINT32                 KeyIndex        OPTIONAL,
  IN       UINT64                 MonotonicCount  OPTIONAL,
  IN       VARIABLE_POINTER_TRACK *Variable,
  IN       EFI_TIME               *TimeStamp      OPTIONAL,
  IN       VARIABLE_GLOBAL        *Global
  );


/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if Runtime is FALSE.
  Allocate EfiRuntimeServicesData pool in protect mode if Runtime is TRUE.
  Allocate memory from SMM ram if system in SMM mode.

  @param[in] Size    The size of buffer to allocate
  @param[in] Runtime Runtime Data or not.

  @return NULL       Buffer unsuccessfully allocated.
  @return Other      Buffer successfully allocated.
**/
VOID *
VariableAllocateZeroBuffer (
  IN UINTN     Size,
  IN BOOLEAN   Runtime
  );


INT8
EFIAPI
IhisiVatsCall (
  IN     UINT8            *InPutBuff,
  IN     UINT8            *OutPutBuff,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  );


UINT8
EFIAPI
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,
  IN     UINTN            DataSize,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  );


/**
  Check is whether support administer secure boot or not.

  @return TRUE  Administer Secure boot is enabled.
  @return FALSE Administer Secure boot is disabled.

**/
BOOLEAN
IsAdministerSecureBootSupport (
  VOID
  );

/**
  This function uses to do specific action when legacy boot event is signaled.

  @retval EFI_SUCCESS      All of action for legacy boot event in SMM mode is successful.
  @retval Other            Any error occurred.
--*/
EFI_STATUS
SmmLegacyBootEvent (
  VOID
  );

/**
  This fucnitons uses to invoke SMM mode SetVariable ()

  @retval EFI_SUCCESS     Disable all secure boot SMI functions successful.
  @return Other           Any erro occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SmmInternalSetVariable (
  VOID
  );

/**
  This funciotn is used to disable variable whole cache mechanism in protected mode or SMM mode.

**/
VOID
DisableVariableCache (
  VOID
  );

/**
  This fucnitons uses to disable variable cache address

  @return EFI_SUCCESS    Disable all secure boot SMI functions successful.
  @return Other          Any erro occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SmmDisableVariableCache (
  VOID
  );

/**
  This fucnitons uses to flush current non-volatile data to variable cache

  @param  CacheBaseAdress         pointer to variable cache base address.
  @param  NonVolatileBaseAddress  pointer to non-volatile base address.
  @param  CacheSize               Variable cache size.
  @param  LastVariableOffset      Pointer to save last variable offset.

  @return EFI_SUCCESS             Flush non-volatile data to variable cache successful.
  @return EFI_INVALID_PARAMETER   CacheBaseAdress or NonVolatileBaseAddress is NULL.
  @return EFI_BUFFER_TOO_SMALL    CacheSize is too small.

**/
EFI_STATUS
FlushVariableCache (
  IN  UINT8    *CacheBaseAdress,
  IN  UINT8    *NonVolatileBaseAddress,
  IN  UINTN    CacheSize,
  OUT UINTN    *LastVariableOffset
  );

/**
  Get the proper fvb handle and/or fvb protocol by the given Flash address.

  @param  Address        The Flash address.
  @param  FvbHandle      In output, if it is not NULL, it points to the proper FVB handle.
  @param  FvbProtocol    In output, if it is not NULL, it points to the proper FVB protocol.

**/
EFI_STATUS
GetFvbInfoByAddress (
  IN  EFI_PHYSICAL_ADDRESS                Address,
  OUT EFI_HANDLE                          *FvbHandle OPTIONAL,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvbProtocol OPTIONAL
  );

/**
  Function returns an array of handles that support the FVB protocol
  in a buffer allocated from pool.

  @param  NumberHandles         The number of handles returned in Buffer.
  @param  Buffer                A pointer to the buffer to return the requested
                                array of  handles that support FVB protocol.

  @retval EFI_SUCCESS           The array of handles was returned in Buffer, and the number of
                                handles in Buffer was returned in NumberHandles.
  @retval EFI_NOT_FOUND         No FVB handle was found.
  @retval EFI_OUT_OF_RESOURCES  There is not enough pool memory to store the matching results.
  @retval EFI_INVALID_PARAMETER NumberHandles is NULL or Buffer is NULL.

**/
EFI_STATUS
GetFvbCountAndBuffer (
  OUT UINTN                               *NumberHandles,
  OUT EFI_HANDLE                          **Buffer
  );

/**
  Retrive the FVB protocol interface by HANDLE.

  @param  FvBlockHandle         The handle of FVB protocol that provides services for
                                reading, writing, and erasing the target block.
  @param  FvBlock               The interface of FVB protocol

  @retval EFI_SUCCESS           The interface information for the specified protocol was returned.
  @retval EFI_UNSUPPORTED       The device does not support the FVB protocol.
  @retval EFI_INVALID_PARAMETER FvBlockHandle is not a valid EFI_HANDLE or FvBlock is NULL.

**/
EFI_STATUS
GetFvbByHandle (
  IN  EFI_HANDLE                          FvBlockHandle,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvBlock
  );

/**
  This fucnitons uses to read saved CPU double word register

  @param  RegisterNum   Register number which want to get
  @param  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Read double word register successful
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
GetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  OUT UINT32                            *RegisterData
  );

/**
  This fucnitons uses to write saved CPU double word register

  @param  RegisterNum   Register number which want to get
  @param  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Read double word register successful
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN UINT32                            RegisterData
  );

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It convers pointer to new virtual address.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
VariableAddressChangeEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  );


/**
  This function allows the caller to determine if UEFI ExitBootServices() has been called.

  This function returns TRUE after all the EVT_SIGNAL_EXIT_BOOT_SERVICES functions have
  executed as a result of the OS calling ExitBootServices().  Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access
  services that go away after ExitBootServices().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.

**/
BOOLEAN
EFIAPI
AtRuntime (
  VOID
  );

/**
  Notification function of gEfiEventExitBootServicesGuid.

  This is a notification function registered on gEfiEventExitBootServicesGuid event.
  It convers pointer to new virtual address.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  );

/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.

  This function causes the notification function to be executed for every protocol of type
  ProtocolGuid instance that exists in the system when this function is invoked. In addition,
  every time a protocol of type ProtocolGuid instance is installed or reinstalled, the notification
  function is also executed. This function returns the notification event that was created.
  If ProtocolGuid is NULL, then ASSERT().
  If NotifyTpl is not a legal TPL value, then ASSERT().
  If NotifyFunction is NULL, then ASSERT().
  If Registration is NULL, then ASSERT().


  @param  ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param  NotifyTpl       Supplies the task priority level of the event notifications.
  @param  NotifyFunction  Supplies the function to notify when the event is signaled.
  @param  NotifyContext   The context parameter to pass to NotifyFunction.
  @param  Registration    A pointer to a memory location to receive the registration value.
                          This value is passed to LocateHandle() to obtain new handles that
                          have been added that support the ProtocolGuid-specified protocol.

  @return The notification event that was created.

**/
EFI_EVENT
EFIAPI
VariableCreateProtocolNotifyEvent(
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  );

/**
  This code gets the size of non-volatile variable store.

  @return UINTN           The size of non-volatile variable store.

**/
UINTN
GetNonVolatileVariableStoreSize (
  VOID
  );

VOID
ElimateVariableWhenRegionFull (
  IN  UINT8    *ValidBuffer,
  IN  UINTN    TotalBufferSize,
  OUT UINTN    *ValidVariableSize
  );

VOID
PreservedTableAddressChange (
  VOID
  );

extern ESAL_VARIABLE_GLOBAL         *mVariableModuleGlobal;
extern SMM_VARIABLE_INSTANCE        *mSmmVariableGlobal;
extern EFI_SMM_SYSTEM_TABLE2        *mSmst;
extern VOID                         *mStorageArea;
extern VOID                         *mCertDbList;
extern VARIALBE_RECLAIM_INFO        *mVariableReclaimInfo;
extern SMM_VAR_BUFFER               *mSmmPhyVarBuf;

#endif
