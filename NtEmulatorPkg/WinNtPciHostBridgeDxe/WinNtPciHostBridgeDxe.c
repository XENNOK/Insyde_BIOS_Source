/** @file
  WinNtPciHostBridgeDxe driver provides fake protcols.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    WinNtPciHostBridgeDxe.c

Abstract:

Revision History

--*/

#include "WinNtPciHostBridgeDxe.h"
#include <Library/MemoryAllocationLib.h>

//
// Hard code the device path and apperatures
//
#define RES_IO_BASE  0x1000
#define RES_MEM_BASE 0x20000000

static UINTN RootBridgeNumber[1] = {
  1
};

static UINT64 RootBridgeAttribute[1][1] = {
  EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM
};

static EFI_PCI_ROOT_BRIDGE_DEVICE_PATH mEfiPciRootBridgeDevicePath[1][1] = {
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0A03),
    0,
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

static PCI_ROOT_BRIDGE_RESOURCE_APPETURE  mResAppeture[1][1] = {
  {0, 255, 0, 0xffffffff, 0, 1 << 16}
};

static EFI_HANDLE mDriverImageHandle;

/**

  Entry point of this driver

  @param[in] ImageHandle -
  @param[in] SystemTable -

  @retval EFI_SUCCESS       - Driver Start OK
  @retval EFI_DEVICE_ERROR  - Fail to install PCI_ROOT_BRIDGE_IO protocol.

**/
EFI_STATUS
InitializeWinNtPciHostBridge (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  UINTN       i, j;
  WINNT_PCI_HOST_BRIDGE_INSTANCE *HostBridge;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE *PrivateData;

  mDriverImageHandle = ImageHandle;

  //
  // Create Host Bridge Device Handle
  //
  for (i = 0; i < HOST_BRIDGE_NUMBER; i++) {
    Status = gBS->AllocatePool(
                    EfiBootServicesData,
                    sizeof(WINNT_PCI_HOST_BRIDGE_INSTANCE),
                    &HostBridge
                    );
    ASSERT(!EFI_ERROR(Status));

    HostBridge->Signature = WINNT_PCI_HOST_BRIDGE_SIGNATURE;
    HostBridge->RootBridgeNumber = RootBridgeNumber[i];
    HostBridge->ResourceSubmited = FALSE;
    HostBridge->CanRestarted     = TRUE;

    HostBridge->ResAlloc.NotifyPhase          = NotifyPhase;
    HostBridge->ResAlloc.GetNextRootBridge    = GetNextRootBridge;
    HostBridge->ResAlloc.GetAllocAttributes   = GetAttributes;
    HostBridge->ResAlloc.StartBusEnumeration  = StartBusEnumeration;
    HostBridge->ResAlloc.SetBusNumbers        = SetBusNumbers;
    HostBridge->ResAlloc.SubmitResources      = SubmitResources;
    HostBridge->ResAlloc.GetProposedResources = GetProposedResources;
    HostBridge->ResAlloc.PreprocessController = PreprocessController;

    HostBridge->HostBridgeHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                   &HostBridge->HostBridgeHandle,
                   &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   &HostBridge->ResAlloc
                   );

    if (EFI_ERROR (Status)) {
      gBS->FreePool (HostBridge);
      return EFI_DEVICE_ERROR;
    }

    //
    // Create Root Bridge Device Handle in this Host Bridge
    //
    InitializeListHead (&HostBridge->Head);

    for (j = 0; j < HostBridge->RootBridgeNumber; j++) {
      Status = gBS->AllocatePool(
                  EfiBootServicesData,
                  sizeof(WINNT_PCI_ROOT_BRIDGE_INSTANCE),
                  &PrivateData
                  );
      ASSERT(!EFI_ERROR(Status));

      PrivateData->Signature = WINNT_PCI_ROOT_BRIDGE_SIGNATURE;

      PrivateData->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&mEfiPciRootBridgeDevicePath[i][j];
      WinNtRootBridgeConstructor (&PrivateData->Io, HostBridge->HostBridgeHandle, RootBridgeAttribute[i][j], mResAppeture[i][j]);

      PrivateData->Handle = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces(
                      &PrivateData->Handle,
                      &gEfiDevicePathProtocolGuid,       PrivateData->DevicePath,
                      &gEfiPciRootBridgeIoProtocolGuid,  &PrivateData->Io,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        gBS->FreePool(PrivateData);
        return EFI_DEVICE_ERROR;
      }

      InsertTailList (&HostBridge->Head, &PrivateData->Link);
    }

  }

  Status = gDS->AddIoSpace (
                              EfiGcdIoTypeIo,
                              RES_IO_BASE,
                              0xF000);

  Status = gDS->AddMemorySpace (
                              EfiGcdMemoryTypeMemoryMappedIo,
                              RES_MEM_BASE,
                              0x20000000,
                              0
                             );

 return EFI_SUCCESS;
}

/**

  Enter a certain phase of the PCI enumeration process

  @param[in] This  - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance
  @param[in] Phase - The phase during enumeration

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong phase parameter passed in.
  @retval EFI_NOT_READY          -  Resources have not been submitted yet.

**/
EFI_STATUS
NotifyPhase(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  )
{
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  PCI_RESOURCE_TYPE                     Index;
  EFI_LIST_ENTRY                        *List;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  UINT64                                AddrLen;
  EFI_STATUS                            Status;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);

  switch (Phase) {
    case EfiPciHostBridgeBeginEnumeration:
      if (HostBridgeInstance->CanRestarted) {
        //
        // Reset the Each Root Bridge
        //
        List = HostBridgeInstance->Head.ForwardLink;

        while (List != &HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          for (Index = TypeIo; Index < TypeMax; Index++) {
            RootBridgeInstance->ResAllocNode[Index].Type      = Index;
            RootBridgeInstance->ResAllocNode[Index].Base      = 0;
            RootBridgeInstance->ResAllocNode[Index].Length    = 0;
            RootBridgeInstance->ResAllocNode[Index].Status    = ResNone;
          }

          List = List->ForwardLink;
        }

        HostBridgeInstance->ResourceSubmited = FALSE;
        HostBridgeInstance->CanRestarted     = TRUE;
      } else {
        //
        // Can not restart
        //
        return EFI_NOT_READY;
      }
      break;
    case EfiPciHostBridgeBeginBusAllocation:
      //
      // No specific action is required here, can perform any chipset specific programing
      //
      HostBridgeInstance->CanRestarted = FALSE;
      return EFI_SUCCESS;
      break;
    case EfiPciHostBridgeEndBusAllocation:
      //
      // No specific action is required here, can perform any chipset specific programing
      //
      //HostBridgeInstance->CanRestarted = FALSE;
      return EFI_SUCCESS;
      break;
    case EfiPciHostBridgeBeginResourceAllocation:
      //
      // No specific action is required here, can perform any chipset specific programing
      //
      //HostBridgeInstance->CanRestarted = FALSE;
      return EFI_SUCCESS;
      break;
    case EfiPciHostBridgeAllocateResources:
      if (HostBridgeInstance->ResourceSubmited) {
        //
        // Take care of the resource dependencies between the root bridges
        // Ea815 does not take care of it
        //
        List = HostBridgeInstance->Head.ForwardLink;

        while (List != &HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          for (Index = TypeIo; Index < TypeBus; Index++) {
            if (RootBridgeInstance->ResAllocNode[Index].Status == ResRequested) {
              switch (Index) {
                case TypeIo:

                  AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
                  Status = gDS->AllocateIoSpace (
                                    AllocateAddress,
                                    EfiGcdIoTypeIo,
                                    0,
                                    AddrLen,
                                    &BaseAddress,
                                    mDriverImageHandle,
                                    NULL
                                    );
                  if (!EFI_ERROR (Status)) {
                    RootBridgeInstance->ResAllocNode[Index].Base   = (UINTN)BaseAddress;
                    RootBridgeInstance->ResAllocNode[Index].Status = ResAllocated;
                  }
                  break;
                case TypeMem32:

                  AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
                  Status = gDS->AllocateMemorySpace (
                                    AllocateAddress,
                                    EfiGcdMemoryTypeMemoryMappedIo,
                                    0,
                                    AddrLen,
                                    &BaseAddress,
                                    mDriverImageHandle,
                                    NULL
                                    );
                  if (!EFI_ERROR (Status)) {
                    RootBridgeInstance->ResAllocNode[Index].Base   = (UINTN)BaseAddress;
                    RootBridgeInstance->ResAllocNode[Index].Status = ResAllocated;
                  }
                  break;
                case TypePMem32:
                  break;
                case TypeMem64:
                  break;
                case TypePMem64:
                  break;
              } //end switch
            }
          }

          List = List->ForwardLink;
        }

        return EFI_SUCCESS;
      } else {
        return EFI_NOT_READY;
      }
      //HostBridgeInstance->CanRestarted = FALSE;
      break;
    case EfiPciHostBridgeSetResources:
      //HostBridgeInstance->CanRestarted = FALSE;
      break;
    case EfiPciHostBridgeFreeResources:
      //HostBridgeInstance->CanRestarted = FALSE;
      break;
    case EfiPciHostBridgeEndResourceAllocation:
      //HostBridgeInstance->CanRestarted = FALSE;
      break;
  case EfiPciHostBridgeEndEnumeration:
    //
    // The Host Bridge Enumeration is completed. No specific action is required here.
    // This notification can be used to perform any chipset specific programming.
    //
    break;
    default:
    return EFI_INVALID_PARAMETER;
  } // end switch

  return EFI_SUCCESS;
}

/**

  Return the device handle of the next PCI root bridge that is associated with
  this Host Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - Returns the device handle of the next PCI Root Bridge.
                      On input, it holds the RootBridgeHandle returned by the most
                      recent call to GetNextRootBridge().The handle for the first
                      PCI Root Bridge is returned if RootBridgeHandle is NULL on input

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_NOT_FOUND          -  Next PCI root bridge not found.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameter passed in.

**/
EFI_STATUS
GetNextRootBridge(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT EFI_HANDLE                                   *RootBridgeHandle
  )
{
  BOOLEAN                               NoRootBridge = TRUE;
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;


  while (List != &HostBridgeInstance->Head) {
    NoRootBridge = FALSE;
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (*RootBridgeHandle == NULL) {
      //
      // Return the first Root Bridge Handle of the Host Bridge
      //
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    } else {
      if (*RootBridgeHandle == RootBridgeInstance->Handle) {
        //
        // Get next if have
        //
        List = List->ForwardLink;
        if (List!=&HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          *RootBridgeHandle = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_FOUND;
        }
      }
    }

    List = List->ForwardLink;
  } //end while

  if (NoRootBridge) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

/**

  Returns the attributes of a PCI Root Bridge.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The device handle of the PCI Root Bridge
                      that the caller is interested in
  @param[in] Attributes        - The pointer to attributes of the PCI Root Bridge

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Attributes parameter passed in is NULL or
                            RootBridgeHandle is not an EFI_HANDLE
                            that was returned on a previous call to
                            GetNextRootBridge().

**/
EFI_STATUS
GetAttributes(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                          *Attributes
  )
{
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;

  if (Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      *Attributes = RootBridgeInstance->RootBridgeAttrib;
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }

  //
  // RootBridgeHandle is not an EFI_HANDLE
  // that was returned on a previous call to GetNextRootBridge()
  //
  return EFI_INVALID_PARAMETER;
}

/**

  This is the request from the PCI enumerator to set up
  the specified PCI Root Bridge for bus enumeration process.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The PCI Root Bridge to be set up.
  @param[in] Configuration     - Pointer to the pointer to the PCI bus resource descriptor.

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
StartBusEnumeration(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  VOID                                  *Buffer;
  UINT8                                 *Temp;
  EFI_STATUS                            Status;
  UINTN                                 BusStart;
  UINTN                                 BusEnd;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      //
      // Set up the Root Bridge for Bus Enumeration
      //
      BusStart = RootBridgeInstance->BusBase;
      BusEnd   = RootBridgeInstance->BusLimit;
      //
      // Program the Hardware(if needed) if error return EFI_DEVICE_ERROR
      //

      Status = gBS->AllocatePool(
                  EfiBootServicesData,
                  sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR),
                  &Buffer
                  );
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      }


      Temp = (UINT8 *)Buffer;

      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Desc = 0x8A;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Len  = 0x2B;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->ResType = 2;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->GenFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->SpecificFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrSpaceGranularity = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMin = BusStart;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMax = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrLen = BusEnd - BusStart + 1;

      Temp = Temp + sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;

      *Configuration = Buffer;
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function programs the PCI Root Bridge hardware so that
  it decodes the specified PCI bus range

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose bus range is to be programmed
  @param[in] Configuration     - The pointer to the PCI bus resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.

**/
EFI_STATUS
SetBusNumbers(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  UINT8                                 *Ptr;
  UINTN                                 BusStart, BusEnd;
  UINTN                                 BusLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Ptr = Configuration;

  //
  // Check the Configuration is valid
  //
  if(*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->ResType != 2) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr += sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  if (*Ptr != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Ptr = Configuration;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      BusStart = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMin;
      BusLen = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrLen;
      BusEnd = BusStart + BusLen - 1;

      if (BusStart > BusEnd) {
        return EFI_INVALID_PARAMETER;
      }

      if ((BusStart < RootBridgeInstance->BusBase) || (BusEnd > RootBridgeInstance->BusLimit)) {
        return EFI_INVALID_PARAMETER;
      }

      //
      // Update the Bus Range
      //
      RootBridgeInstance->ResAllocNode[TypeBus].Base      = BusStart;
      RootBridgeInstance->ResAllocNode[TypeBus].Length    = BusLen;
      RootBridgeInstance->ResAllocNode[TypeBus].Status    = ResAllocated;

      //
      // Program the Root Bridge Hardware
      //

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  Submits the I/O and memory resource requirements for the specified PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose I/O and memory resource requirements
                      are being submitted
  @param[in] Configuration     - The pointer to the PCI I/O and PCI memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.

**/
EFI_STATUS
SubmitResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *ptr;
  UINTN                                 AddrLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Temp = (UINT8 *)Configuration;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      if (*Temp == ACPI_END_TAG_DESCRIPTOR) {
        HostBridgeInstance->ResourceSubmited = TRUE;
      }
      while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
        ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;
        switch ( ptr -> ResType ) {
          case 0:
            AddrLen = (UINTN)ptr -> AddrLen;
            if ( ptr -> AddrSpaceGranularity == 32 ) {
              if ( ptr -> SpecificFlag == 0x06 ) {
                //
                // Apply from GCD
                //
              } else {
                RootBridgeInstance->ResAllocNode[TypeMem32].Length = AddrLen;
                RootBridgeInstance->ResAllocNode[TypeMem32].Status = ResRequested;
                HostBridgeInstance->ResourceSubmited = TRUE;
              }
            }

            if ( ptr -> AddrSpaceGranularity == 64 ) {
             if ( ptr -> SpecificFlag == 0x06 ) {

             } else {

             }
            }
            break;
          case 1:
            AddrLen = (UINTN)ptr -> AddrLen;
            RootBridgeInstance->ResAllocNode[TypeIo].Length  = AddrLen;
            RootBridgeInstance->ResAllocNode[TypeIo].Status  = ResRequested;
            HostBridgeInstance->ResourceSubmited = TRUE;
            break;
          default:
            break;
        }

        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;
      }
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function returns the proposed resource settings for the specified
  PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] Configuration     - The pointer to the pointer to the PCI I/O
                      and memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
GetProposedResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  EFI_LIST_ENTRY                        *List;
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  UINTN                                 Index, Number = 0;
  VOID                                  *Buffer = NULL;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *ptr;
  EFI_STATUS                            Status;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      for (Index = 0; Index < TypeBus; Index ++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
          Number ++;
        }
      }

      if (Number > 0) {
        Status = gBS->AllocatePool(
                  EfiBootServicesData,
                  Number * sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR),
                  &Buffer
                  );
        if (EFI_ERROR(Status)) {
          return EFI_OUT_OF_RESOURCES;
        }
      } else {
        Buffer = AllocateZeroPool (sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
        ((EFI_ACPI_END_TAG_DESCRIPTOR *)Buffer)->Desc = ACPI_END_TAG_DESCRIPTOR;
        ((EFI_ACPI_END_TAG_DESCRIPTOR *)Buffer)->Checksum = 0x0;
        *Configuration = Buffer;
        return EFI_SUCCESS;
      }

      Temp = Buffer;
      for (Index = 0; Index < TypeBus; Index ++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
          ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;
          switch (Index) {
            case TypeIo:
              ptr->Desc = 0x8A;
              ptr->Len  = 0x2B;
              ptr->ResType = 1;
              ptr->GenFlag = 0;
              ptr->SpecificFlag = 0;
              ptr->AddrSpaceGranularity = 0;
              ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base;
              ptr->AddrRangeMax = 0;
              ptr->AddrTranslationOffset = 0;
              ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
              break;
            case TypeMem32:
              ptr->Desc = 0x8A;
              ptr->Len  = 0x2B;
              ptr->ResType = 0;
              ptr->GenFlag = 0;
              ptr->SpecificFlag = 0;
              ptr->AddrSpaceGranularity = 32;
              ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base;
              ptr->AddrRangeMax = 0;
              ptr->AddrTranslationOffset = 0;
              ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
              break;
            case TypePMem32:
              break;
            case TypeMem64:
              break;
            case TypePMem64:
              break;
          }

          Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
        }
      }

      if (Temp) {
        ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
        ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;
      }

      *Configuration = Buffer;

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function is called for all the PCI controllers that the PCI
  bus driver finds. Can be used to Preprogram the controller.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] PciAddress        - Address of the controller on the PCI bus
  @param[in] Phase             - The Phase during resource allocation

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
PreprocessController (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS                PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE               Phase
  )
{
  WINNT_PCI_HOST_BRIDGE_INSTANCE        *HostBridgeInstance;
  WINNT_PCI_ROOT_BRIDGE_INSTANCE        *RootBridgeInstance;
  EFI_LIST_ENTRY                        *List;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  //
  // Enumerate the root bridges in this host bridge
  //
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }
  return  EFI_INVALID_PARAMETER;
}
