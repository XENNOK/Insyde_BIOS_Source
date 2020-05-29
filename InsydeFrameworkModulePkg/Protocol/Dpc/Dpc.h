//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
//  Abstract:
//
//  EFI Deferred Procedure Call Protocol


#ifndef _EFIDPC_H
#define _EFIDPC_H

//
// DPC Protocol GUID value
//
#define EFI_DPC_PROTOCOL_GUID \
    { 0x480f8ae9, 0xc46, 0x4aa9, 0xbc, 0x89, 0xdb, 0x9f, 0xba, 0x61, 0x98, 0x6 }

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_DPC_PROTOCOL  EFI_DPC_PROTOCOL;


/**
  Invoke a Deferred Procedure Call.

  @param  DpcContext           Pointer to the Deferred Procedure Call's context,
                               which is implementation dependent.

**/
typedef
VOID
(EFIAPI *EFI_DPC_PROCEDURE) (
  IN VOID  *DpcContext
  );

/**
  Add a Deferred Procedure Call to the end of the DPC queue.

  @param  This          Protocol instance pointer.
  @param  DpcTpl        The EFI_TPL that the DPC should be invoked.
  @param  DpcProcedure  Pointer to the DPC's function.
  @param  DpcContext    Pointer to the DPC's context.  Passed to DpcProcedure
                        when DpcProcedure is invoked.

  @retval EFI_SUCCESS            The DPC was queued.
  @retval EFI_INVALID_PARAMETER  DpcTpl is not a valid EFI_TPL.
  @retval EFI_INVALID_PARAMETER  DpcProcedure is NULL.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to
                                 add the DPC to the queue.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_DPC_QUEUE_DPC) (
  IN EFI_DPC_PROTOCOL   *This,
  IN EFI_TPL            DpcTpl,
  IN EFI_DPC_PROCEDURE  DpcProcedure,
  IN VOID               *DpcContext    OPTIONAL
  );

/**
  Dispatch the queue of DPCs.  ALL DPCs that have been queued with a DpcTpl
  value greater than or equal to the current TPL are invoked in the order that
  they were queued.  DPCs with higher DpcTpl values are invoked before DPCs with
  lower DpcTpl values.

  @param  This  Protocol instance pointer.

  @retval EFI_SUCCESS    One or more DPCs were invoked.
  @retval EFI_NOT_FOUND  No DPCs were invoked.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_DPC_DISPATCH_DPC) (
  IN EFI_DPC_PROTOCOL  *This
  );

//
// DPC Protocol structure
//
typedef struct _EFI_DPC_PROTOCOL {
  EFI_DPC_QUEUE_DPC     QueueDpc;
  EFI_DPC_DISPATCH_DPC  DispatchDpc;
};

//
// DPC Protocol GUID variable
//
extern EFI_GUID gEfiDpcProtocolGuid;

#endif /* _EFIDPC_H */
