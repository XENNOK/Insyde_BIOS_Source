/** @file
  SPI Access Init routines

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

#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/DxeServicesTableLib.h>
//[-start-120419-IB10820037-modify]//
#include <PchAccess.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
//[-end-120419-IB10820037-modify]//
//[-start-120925-IB10820124-add]//
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Pfat.h>
//[-start-121002-IB10820131-add]//
#include <Protocol/SmmBase2.h>

//[-start-130416-IB10820284-add]//
extern BOOLEAN                        mPfatProtocolInitialed;
extern PFAT_PROTOCOL                  *mPfatProtocol;
extern EFI_GUID                       mSmmPfatProtocolGuid;
extern EFI_SMM_SYSTEM_TABLE2          *mSmst2;
//[-end-130416-IB10820284-add]//

BOOLEAN                               mAtRuntime = FALSE;
EFI_EVENT                             mExitBootServicesEvent;


/**
  Set AtRuntime flag as TRUE after ExitBootServices.

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.
**/
STATIC
VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  mAtRuntime = TRUE;
}

/**
  Detect whether the system is at EFI runtime or not

  @param  None

  @retval TRUE                  System is at EFI runtime
  @retval FALSE                 System is not at EFI runtime

**/
BOOLEAN
EFIAPI
AtRuntime (
  VOID
  )
{
  return mAtRuntime;
}

/**
  Detect whether the system is at SMM mode.

  @retval TRUE                  System is at SMM mode.
  @retval FALSE                 System is not at SMM mode.
**/
STATIC
BOOLEAN
IsInSmm (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);
  return InSmm;
}

//[-start-130416-IB10820284-add]//
EFI_STATUS
LocatePfatProtocol (
  IN CONST EFI_GUID                     *Protocol,
  IN VOID                               *Interface,
  IN EFI_HANDLE                         Handle
  )
{
  EFI_STATUS  Status;

  if (mPfatProtocol != NULL) {
    return EFI_SUCCESS;
  }

  Status = mSmst2->SmmLocateProtocol (
                     &gSmmPfatProtocolGuid,
                     NULL,
                     (VOID **)&mPfatProtocol
                     );
  if (EFI_ERROR (Status)) {
    mPfatProtocol = NULL;
  } else {
    mPfatProtocolInitialed = TRUE;
  }

  return EFI_SUCCESS;
}
//[-end-130416-IB10820284-add]//

/**
  Initialization routine for SpiAccessLib

  @param  None

  @retval EFI_SUCCESS           SpiAccessLib successfully initialized
  @return Others                SpiAccessLib initialization failed

**/
EFI_STATUS
EFIAPI
SpiAccessInit (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS BaseAddress;
  UINTN                Length;
  EFI_STATUS           Status;
//[-start-130416-IB10820284-add]//
  EFI_SMM_BASE2_PROTOCOL      *SmmBase;
  VOID                        *Registration;
//[-end-130416-IB10820284-add]//


  if (!IsInSmm ()) {
    //
    // Only create exit boot services event in protected mode.
    // Otherwise, system will trigger exception if event is signaled and callback to function
    // which is located in SMM RAM.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ExitBootServicesEvent,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &mExitBootServicesEvent
                    );
    ASSERT_EFI_ERROR (Status);
  }


  //
  // Reserve RCBA space in GCD, which will be reported to OS by E820
  // It will assert if RCBA Memory Space is not allocated
  // The caller is responsible for the existence and allocation of the RCBA Memory Spaces
  //
  BaseAddress = (EFI_PHYSICAL_ADDRESS)PCH_RCRB_BASE;
  Length      = 0x4000;
  Status = gDS->SetMemorySpaceAttributes (
                    BaseAddress,
                    Length,
                    EFI_MEMORY_RUNTIME
                    );
////[-start-120817-IB05300312-add]//
////[-start-120907-IB05300326-modify]//
//  //
//  // Locate the PFAT protocol.
//  //
//  if ( FeaturePcdGet ( PcdPfatSupport ) ) {
//    if (!mPfatProtocolInitialed) {
//      Status = gBS->LocateProtocol (
//                      &gSmmPfatProtocolGuid,
//                      NULL,
//                      (VOID **)&mPfatProtocol
//                      );
//      if (EFI_ERROR (Status)) {
//          mPfatProtocol = NULL;
//          Status = EFI_SUCCESS;
//      }
//      mPfatProtocolInitialed = TRUE;
//    }
//  }
////[-end-120907-IB05300326-modify]//
////[-end-120817-IB05300312-add]//

//[-start-130714-IB10930041-modify]//
//[-start-130416-IB10820284-add]//
  if (FeaturePcdGet (PcdPfatSupport)) {
    if (IsInSmm ()) {
      Status = gBS->LocateProtocol (
                &gEfiSmmBase2ProtocolGuid,
                NULL,
                (VOID **)&SmmBase
                );
      ASSERT_EFI_ERROR (Status);
      //
      // Get Smm Syatem Table
      //
      Status = SmmBase->GetSmstLocation(
                          SmmBase,
                          &mSmst2
                          );
      if (!EFI_ERROR (Status)) {
        //
        // Try to locate Smm Pfat protocol, if locate fail -> register notify for it
        //
        Status = mSmst2->SmmLocateProtocol (
                           &gSmmPfatProtocolGuid,
                           NULL,
                           (VOID **)&mPfatProtocol
                           );
        if (EFI_ERROR (Status)) {
          mPfatProtocol = NULL;
          //
          // Register Notify Event for LocatePfatProtocol
          //
          Status = mSmst2->SmmRegisterProtocolNotify (
                             &gSmmPfatProtocolGuid,
                             LocatePfatProtocol,
                             &Registration
                             );
        } else {
          mPfatProtocolInitialed = TRUE;
        }
      }
    }
  }
//[-end-130416-IB10820284-add]//
//[-end-130714-IB10930041-modify]//

  return Status;
}

/**
  This routine uses to free the allocated resource by SpiAccessInit ().

  @retval EFI_SUCCESS    Free allocated resource successful.
  @return Others         Free allocated resource failed.
**/
EFI_STATUS
EFIAPI
SpiAccessDestroy (
  VOID
  )
{
  EFI_STATUS      Status;

  if (mExitBootServicesEvent != NULL) {
    Status = gBS->CloseEvent (mExitBootServicesEvent);
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}