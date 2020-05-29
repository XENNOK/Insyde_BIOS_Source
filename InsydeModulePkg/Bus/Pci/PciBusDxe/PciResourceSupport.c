/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  PCI resouces support functions implemntation for PCI Bus module.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciBus.h"

//
// The default policy for the PCI bus driver is NOT to reserve I/O ranges for both ISA aliases and VGA aliases.
//
BOOLEAN mReserveIsaAliases = FALSE;
BOOLEAN mReserveVgaAliases = FALSE;
BOOLEAN mPolicyDetermined  = FALSE;

/**
  The function is used to skip VGA range.

  @param Start    Returned start address including VGA range.
  @param Length   The length of VGA range.

**/
VOID
SkipVGAAperture (
  OUT UINT64   *Start,
  IN  UINT64   Length
  )
{
  UINT64  Original;
  UINT64  Mask;
  UINT64  StartOffset;
  UINT64  LimitOffset;

  ASSERT (Start != NULL);
  //
  // For legacy VGA, bit 10 to bit 15 is not decoded
  //
  Mask        = 0x3FF;

  Original    = *Start;
  StartOffset = Original & Mask;
  LimitOffset = ((*Start) + Length - 1) & Mask;
  if (LimitOffset >= VGABASE1) {
    *Start = *Start - StartOffset + VGALIMIT2 + 1;
  }
}

/**
  This function is used to skip ISA aliasing aperture.

  @param Start    Returned start address including ISA aliasing aperture.
  @param Length   The length of ISA aliasing aperture.

**/
VOID
SkipIsaAliasAperture (
  OUT UINT64   *Start,
  IN  UINT64   Length
  )
{

  UINT64  Original;
  UINT64  Mask;
  UINT64  StartOffset;
  UINT64  LimitOffset;

  ASSERT (Start != NULL);

  //
  // For legacy ISA, bit 10 to bit 15 is not decoded
  //
  Mask        = 0x3FF;

  Original    = *Start;
  StartOffset = Original & Mask;
  LimitOffset = ((*Start) + Length - 1) & Mask;

  if (LimitOffset >= ISABASE) {
    *Start = *Start - StartOffset + ISALIMIT + 1;
  }
}

/**
  This function inserts a resource node into the resource list.
  The resource list is sorted in descend order.

  @param Bridge  PCI resource node for bridge.
  @param ResNode Resource node want to be inserted.

**/
VOID
InsertResourceNode (
  IN OUT PCI_RESOURCE_NODE   *Bridge,
  IN     PCI_RESOURCE_NODE   *ResNode
  )
{
  LIST_ENTRY        *CurrentLink;
  PCI_RESOURCE_NODE *Temp;
  UINT64            ResNodeAlignRest;
  UINT64            TempAlignRest;

  ASSERT (Bridge  != NULL);
  ASSERT (ResNode != NULL);

  InsertHeadList (&Bridge->ChildList, &ResNode->Link);

  CurrentLink = Bridge->ChildList.ForwardLink->ForwardLink;
  while (CurrentLink != &Bridge->ChildList) {
    Temp = RESOURCE_NODE_FROM_LINK (CurrentLink);

    if (ResNode->Alignment > Temp->Alignment) {
      break;
    } else if (ResNode->Alignment == Temp->Alignment) {
      ResNodeAlignRest  = ResNode->Length & ResNode->Alignment;
      TempAlignRest     = Temp->Length & Temp->Alignment;
      if ((ResNodeAlignRest == 0) || (ResNodeAlignRest >= TempAlignRest)) {
        break;
      }
    }

    SwapListEntries (&ResNode->Link, CurrentLink);

    CurrentLink = ResNode->Link.ForwardLink;
  }
}

/**
  This routine is used to merge two different resource trees in need of
  resoure degradation.

  For example, if an upstream PPB doesn't support,
  prefetchable memory decoding, the PCI bus driver will choose to call this function
  to merge prefectchable memory resource list into normal memory list.

  If the TypeMerge is TRUE, Res resource type is changed to the type of destination resource
  type.
  If Dst is NULL or Res is NULL, ASSERT ().

  @param Dst        Point to destination resource tree.
  @param Res        Point to source resource tree.
  @param TypeMerge  If the TypeMerge is TRUE, Res resource type is changed to the type of
                    destination resource type.

**/
VOID
MergeResourceTree (
  IN PCI_RESOURCE_NODE   *Dst,
  IN PCI_RESOURCE_NODE   *Res,
  IN BOOLEAN             TypeMerge
  )
{

  LIST_ENTRY        *CurrentLink;
  PCI_RESOURCE_NODE *Temp;

  ASSERT (Dst != NULL);
  ASSERT (Res != NULL);

  while (!IsListEmpty (&Res->ChildList)) {
    CurrentLink = Res->ChildList.ForwardLink;

    Temp        = RESOURCE_NODE_FROM_LINK (CurrentLink);

    if (TypeMerge) {
      Temp->ResType = Dst->ResType;
    }

    RemoveEntryList (CurrentLink);
    InsertResourceNode (Dst, Temp);
  }
}

/**
  This function is used to calculate the IO16 aperture
  for a bridge.

  @param Bridge    PCI resource node for bridge.

**/
VOID
CalculateApertureIo16 (
  IN PCI_RESOURCE_NODE    *Bridge
  )
{
  EFI_STATUS              Status;
  UINT64                  Aperture;
  LIST_ENTRY              *CurrentLink;
  PCI_RESOURCE_NODE       *Node;
  UINT64                  Offset;
  EFI_PCI_PLATFORM_POLICY PciPolicy;

  if (!mPolicyDetermined) {
    //
    // Check PciPlatform policy
    //
    Status = EFI_NOT_FOUND;
    PciPolicy = 0;
    if (gPciPlatformProtocol != NULL) {
      Status = gPciPlatformProtocol->GetPlatformPolicy (
                                       gPciPlatformProtocol,
                                       &PciPolicy
                                       );
    }

    if (EFI_ERROR (Status) && gPciOverrideProtocol != NULL) {
      Status = gPciOverrideProtocol->GetPlatformPolicy (
                                       gPciOverrideProtocol,
                                       &PciPolicy
                                       );
    }

    if (!EFI_ERROR (Status)) {
      if ((PciPolicy & EFI_RESERVE_ISA_IO_ALIAS) != 0) {
        mReserveIsaAliases = TRUE;
      }
      if ((PciPolicy & EFI_RESERVE_VGA_IO_ALIAS) != 0) {
        mReserveVgaAliases = TRUE;
      }
    }
    mPolicyDetermined = TRUE;
  }

  Aperture = 0;

  if (Bridge == NULL) {
    return ;
  }

  CurrentLink = Bridge->ChildList.ForwardLink;

  //
  // Assume the bridge is aligned
  //
  while (CurrentLink != &Bridge->ChildList) {

    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);

    //
    // Consider the aperture alignment
    //
    Offset = Aperture & (Node->Alignment);

    if (Offset != 0) {

      Aperture = Aperture + (Node->Alignment + 1) - Offset;

    }

    //
    // IsaEnable and VGAEnable can not be implemented now.
    // If both of them are enabled, then the IO resource would
    // become too limited to meet the requirement of most of devices.
    //
    if (mReserveIsaAliases || mReserveVgaAliases) {
      if (!IS_PCI_BRIDGE (&(Node->PciDev->Pci)) && !IS_CARDBUS_BRIDGE (&(Node->PciDev->Pci))) {
        //
        // Check if there is need to support ISA/VGA decoding
        // If so, we need to avoid isa/vga aliasing range
        //
        if (mReserveIsaAliases) {
          SkipIsaAliasAperture (
            &Aperture,
            Node->Length
            );
          Offset = Aperture & (Node->Alignment);
          if (Offset != 0) {
            Aperture = Aperture + (Node->Alignment + 1) - Offset;
          }
        } else if (mReserveVgaAliases) {
          SkipVGAAperture (
            &Aperture,
            Node->Length
            );
          Offset = Aperture & (Node->Alignment);
          if (Offset != 0) {
            Aperture = Aperture + (Node->Alignment + 1) - Offset;
          }
        }
      }
    }

    Node->Offset = Aperture;

    //
    // Increment aperture by the length of node
    //
    Aperture += Node->Length;

    CurrentLink = CurrentLink->ForwardLink;
  }

  //
  // At last, adjust the aperture with the bridge's
  // alignment
  //
  Offset = Aperture & (Bridge->Alignment);

  if (Offset != 0) {
    Aperture = Aperture + (Bridge->Alignment + 1) - Offset;
  }

  Bridge->Length = Aperture;
  //
  // At last, adjust the bridge's alignment to the first child's alignment
  // if the bridge has at least one child
  //
  CurrentLink = Bridge->ChildList.ForwardLink;
  if (CurrentLink != &Bridge->ChildList) {
    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);
    if (Node->Alignment > Bridge->Alignment) {
      Bridge->Alignment = Node->Alignment;
    }
  }
}

/**
  This function is used to calculate the resource aperture
  for a given bridge device.

  @param Bridge      PCI resouce node for given bridge device.

**/
VOID
CalculateResourceAperture (
  IN PCI_RESOURCE_NODE    *Bridge
  )
{
  UINT64            Aperture;
  LIST_ENTRY        *CurrentLink;
  PCI_RESOURCE_NODE *Node;

  UINT64            Offset;

  Aperture = 0;

  if (Bridge == NULL) {
    return ;
  }

  if (Bridge->ResType == PciBarTypeIo16) {

    CalculateApertureIo16 (Bridge);
    return ;
  }

  CurrentLink = Bridge->ChildList.ForwardLink;

  //
  // Assume the bridge is aligned
  //
  while (CurrentLink != &Bridge->ChildList) {

    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);

    //
    // Apply padding resource if available
    //
    Offset = Aperture & (Node->Alignment);

    if (Offset != 0) {

      Aperture = Aperture + (Node->Alignment + 1) - Offset;

    }

    //
    // Recode current aperture as a offset
    // this offset will be used in future real allocation
    //
    Node->Offset = Aperture;

    //
    // Increment aperture by the length of node
    //
    Aperture += Node->Length;

    //
    // Consider the aperture alignment
    //
    CurrentLink = CurrentLink->ForwardLink;
  }

  //
  // At last, adjust the aperture with the bridge's
  // alignment
  //
  Offset = Aperture & (Bridge->Alignment);
  if (Offset != 0) {
    Aperture = Aperture + (Bridge->Alignment + 1) - Offset;
  }

  //
  // If the bridge has already padded the resource and the
  // amount of padded resource is larger, then keep the
  // padded resource
  //
  if (Bridge->Length < Aperture) {
    Bridge->Length = Aperture;
  }

  //
  // At last, adjust the bridge's alignment to the first child's alignment
  // if the bridge has at least one child
  //
  CurrentLink = Bridge->ChildList.ForwardLink;
  if (CurrentLink != &Bridge->ChildList) {
    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);
    if (Node->Alignment > Bridge->Alignment) {
      Bridge->Alignment = Node->Alignment;
    }
  }
}

/**

 @param [in]   Bridge

**/
VOID
ResourceAllocationOnRoot (
  IN PCI_RESOURCE_NODE *Bridge
  )
{
  UINT64                            Aperture;
  LIST_ENTRY                        *CurrentLink;
  LIST_ENTRY                        *AvailiableSpaceLink;
  PCI_RESOURCE_NODE                 *Node;
  UINT64                            Offset;
  UINT64                            NodeBaseAddress;
  UINT64                            NodeEndAddress;
  BOOLEAN                           FoundAvailableSpace;
  PCI_AVAILIABLE_SPACE_DESCRIPTOR   *TotalUsedSpace = NULL;
  PCI_AVAILIABLE_SPACE_DESCRIPTOR   *AvailiableSpace = NULL;
  PCI_AVAILIABLE_SPACE_DESCRIPTOR   *AvailiableSpacePart1 = NULL;
  PCI_AVAILIABLE_SPACE_DESCRIPTOR   *AvailiableSpacePart2 = NULL;

  Aperture             = 0;
  NodeBaseAddress      = 0;
  NodeEndAddress       = 0;

  if (!Bridge) {
    return;
  }

  if (Bridge->ResType == PciBarTypeIo16) {
    CalculateApertureIo16 (Bridge);
    return;
  }
  //
  // Initialize the info of availiable Space.
  // The header of this list describles the usage of total space.
  //
  TotalUsedSpace = AllocateZeroPool (sizeof (PCI_AVAILIABLE_SPACE_DESCRIPTOR));
  if (TotalUsedSpace == NULL) {
    return;
  }
  InitializeListHead (&TotalUsedSpace->Link);

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != &Bridge->ChildList) {

    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);
    //
    // To check any spcace that is availiable.
    //
    FoundAvailableSpace = FALSE;

    if (!IsListEmpty (&TotalUsedSpace->Link)) {

      AvailiableSpaceLink = TotalUsedSpace->Link.ForwardLink;

      while (AvailiableSpaceLink != &TotalUsedSpace->Link) {

        AvailiableSpace = AVAILIABLE_SPACE_FROM_LINK (AvailiableSpaceLink);

        Offset = AvailiableSpace->BaseAddress & Node->Alignment;

        if (Offset) {
          NodeBaseAddress = AvailiableSpace->BaseAddress + (Node->Alignment + 1) - Offset;
        } else {
          NodeBaseAddress = AvailiableSpace->BaseAddress;
        }

        NodeEndAddress = NodeBaseAddress + Node->Length;

        if (NodeEndAddress <= AvailiableSpace->EndAddress) {

          Node->Offset = NodeBaseAddress;

          FoundAvailableSpace = TRUE;
          //
          // Update AvailiableSpace List.
          //
          if ((Offset == 0) && (NodeEndAddress == AvailiableSpace->EndAddress)) {
            RemoveEntryList (AvailiableSpaceLink);
            FreePool (AvailiableSpace);
          }

          if ((Offset == 0) && (NodeEndAddress < AvailiableSpace->EndAddress)) {
            AvailiableSpace->BaseAddress = NodeEndAddress;
          }

          if ((Offset != 0) && (NodeEndAddress == AvailiableSpace->EndAddress)) {
            AvailiableSpace->EndAddress = NodeBaseAddress;
          }

          if ((Offset != 0) && (NodeEndAddress < AvailiableSpace->EndAddress)) {
            AvailiableSpacePart1 = AllocateZeroPool (sizeof (PCI_AVAILIABLE_SPACE_DESCRIPTOR));
            AvailiableSpacePart2 = AllocateZeroPool (sizeof (PCI_AVAILIABLE_SPACE_DESCRIPTOR));
            if ((AvailiableSpacePart1 == NULL) || (AvailiableSpacePart2 == NULL)) {
              return;
            }
            AvailiableSpacePart1->BaseAddress = AvailiableSpace->BaseAddress;
            AvailiableSpacePart1->EndAddress = NodeBaseAddress;
            AvailiableSpacePart2->BaseAddress = NodeEndAddress;
            AvailiableSpacePart2->EndAddress = AvailiableSpace->EndAddress;
            RemoveEntryList (AvailiableSpaceLink);
            FreePool (AvailiableSpace);
            InsertHeadList (&TotalUsedSpace->Link, &AvailiableSpacePart1->Link);
            InsertHeadList (&TotalUsedSpace->Link, &AvailiableSpacePart2->Link);
          }
          //
          // found the available space, leaving the loop.
          //
          break;
        }
        //
        // Too small, try to find other space.
        //
        AvailiableSpaceLink = AvailiableSpaceLink->ForwardLink;
      }

    }

    if (FoundAvailableSpace == FALSE) {
      //
      // Have not AvailiableSpace.
      //
      Offset = TotalUsedSpace->EndAddress & Node->Alignment;

      if (Offset) {
        NodeBaseAddress = TotalUsedSpace->EndAddress + (Node->Alignment + 1) - Offset;
      } else {
        NodeBaseAddress = TotalUsedSpace->EndAddress;
      }

      Node->Offset = NodeBaseAddress;
      NodeEndAddress = NodeBaseAddress + Node->Length;
      //
      // Update AvailiableSpace List.
      //
      if (NodeBaseAddress > TotalUsedSpace->EndAddress) {
        AvailiableSpace = AllocateZeroPool (sizeof (PCI_AVAILIABLE_SPACE_DESCRIPTOR));
        if (AvailiableSpace == NULL) {
          return;
        }
        AvailiableSpace->BaseAddress = TotalUsedSpace->EndAddress;
        AvailiableSpace->EndAddress = NodeBaseAddress;
        InsertHeadList (&TotalUsedSpace->Link, &AvailiableSpace->Link);
      }
      //
      // Update Total space info.
      //
      TotalUsedSpace->EndAddress = NodeEndAddress;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  Aperture = TotalUsedSpace->EndAddress;

  Offset = Aperture & (Bridge->Alignment);

  if (Offset) {
    Aperture = Aperture + (Bridge->Alignment + 1) - Offset;
  }

  if (Bridge->Length < Aperture) {
    Bridge->Length = Aperture;
  }

  CurrentLink = Bridge->ChildList.ForwardLink;
  if (CurrentLink != &Bridge->ChildList) {
    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);
    if (Node->Alignment > Bridge->Alignment) {
      Bridge->Alignment = Node->Alignment;
    }
  }
  //
  // Free memory
  //
  AvailiableSpace = TotalUsedSpace;

  while (!IsListEmpty (&TotalUsedSpace->Link)) {

    AvailiableSpaceLink = TotalUsedSpace->Link.ForwardLink;

    AvailiableSpace = AVAILIABLE_SPACE_FROM_LINK (AvailiableSpaceLink);

    RemoveEntryList (AvailiableSpaceLink);

    if (AvailiableSpace != NULL) {
      FreePool (AvailiableSpace);
    }
  }

  if (TotalUsedSpace != NULL) {
    FreePool (TotalUsedSpace);
  }
}

/**
  Get IO/Memory resource infor for given PCI device.

  @param PciDev     Pci device instance.
  @param IoNode     Resource info node for IO .
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.
  @return EFI_SUCCESS            Valid resource request
          EFI_INVALID_PARAMETER  Mem32 request of a single device >= 4GB

**/
EFI_STATUS
GetResourceFromDevice (
  IN     PCI_IO_DEVICE     *PciDev,
  IN OUT PCI_RESOURCE_NODE *IoNode,
  IN OUT PCI_RESOURCE_NODE *Mem32Node,
  IN OUT PCI_RESOURCE_NODE *PMem32Node,
  IN OUT PCI_RESOURCE_NODE *Mem64Node,
  IN OUT PCI_RESOURCE_NODE *PMem64Node
  )
{

  UINT8             Index;
  PCI_RESOURCE_NODE *Node;
  BOOLEAN           ResourceRequested;
  UINT64            Mem32Aperture;
  Mem32Aperture = 0;

  Node              = NULL;
  ResourceRequested = FALSE;

  for (Index = 0; Index < PCI_MAX_BAR; Index++) {

    switch ((PciDev->PciBar)[Index].BarType) {

    case PciBarTypeMem32:

      Node = CreateResourceNode (
               PciDev,
               (PciDev->PciBar)[Index].Length,
               (PciDev->PciBar)[Index].Alignment,
               Index,
               PciBarTypeMem32,
               PciResUsageTypical
               );
      Mem32Aperture += Node->Length;
      if (Mem32Aperture > 0xFFFFFFFF) {
	FreePool (Node);
        return EFI_INVALID_PARAMETER;
      }

      InsertResourceNode (
        Mem32Node,
        Node
        );

      ResourceRequested = TRUE;
      break;

    case PciBarTypeMem64:

      Node = CreateResourceNode (
               PciDev,
               (PciDev->PciBar)[Index].Length,
               (PciDev->PciBar)[Index].Alignment,
               Index,
               PciBarTypeMem64,
               PciResUsageTypical
               );

      InsertResourceNode (
        Mem64Node,
        Node
        );

      ResourceRequested = TRUE;
      break;

    case PciBarTypePMem64:

      Node = CreateResourceNode (
               PciDev,
               (PciDev->PciBar)[Index].Length,
               (PciDev->PciBar)[Index].Alignment,
               Index,
               PciBarTypePMem64,
               PciResUsageTypical
               );

      InsertResourceNode (
        PMem64Node,
        Node
        );

      ResourceRequested = TRUE;
      break;

    case PciBarTypePMem32:

      Node = CreateResourceNode (
               PciDev,
               (PciDev->PciBar)[Index].Length,
               (PciDev->PciBar)[Index].Alignment,
               Index,
               PciBarTypePMem32,
               PciResUsageTypical
               );

      InsertResourceNode (
        PMem32Node,
        Node
        );
      ResourceRequested = TRUE;
      break;

    case PciBarTypeIo16:
    case PciBarTypeIo32:

      Node = CreateResourceNode (
               PciDev,
               (PciDev->PciBar)[Index].Length,
               (PciDev->PciBar)[Index].Alignment,
               Index,
               PciBarTypeIo16,
               PciResUsageTypical
               );

      InsertResourceNode (
        IoNode,
        Node
        );
      ResourceRequested = TRUE;
      break;

    case PciBarTypeUnknown:
      break;

    default:
      break;
    }
  }

  //
  // Add VF resource
  //
  for (Index = 0; Index < PCI_MAX_BAR; Index++) {

    switch ((PciDev->VfPciBar)[Index].BarType) {

    case PciBarTypeMem32:

      Node = CreateVfResourceNode (
               PciDev,
               (PciDev->VfPciBar)[Index].Length,
               (PciDev->VfPciBar)[Index].Alignment,
               Index,
               PciBarTypeMem32,
               PciResUsageTypical
               );

      InsertResourceNode (
        Mem32Node,
        Node
        );

      break;

    case PciBarTypeMem64:

      Node = CreateVfResourceNode (
               PciDev,
               (PciDev->VfPciBar)[Index].Length,
               (PciDev->VfPciBar)[Index].Alignment,
               Index,
               PciBarTypeMem64,
               PciResUsageTypical
               );

      InsertResourceNode (
        Mem64Node,
        Node
        );

      break;

    case PciBarTypePMem64:

      Node = CreateVfResourceNode (
               PciDev,
               (PciDev->VfPciBar)[Index].Length,
               (PciDev->VfPciBar)[Index].Alignment,
               Index,
               PciBarTypePMem64,
               PciResUsageTypical
               );

      InsertResourceNode (
        PMem64Node,
        Node
        );

      break;

    case PciBarTypePMem32:

      Node = CreateVfResourceNode (
               PciDev,
               (PciDev->VfPciBar)[Index].Length,
               (PciDev->VfPciBar)[Index].Alignment,
               Index,
               PciBarTypePMem32,
               PciResUsageTypical
               );

      InsertResourceNode (
        PMem32Node,
        Node
        );
      break;

    case PciBarTypeIo16:
    case PciBarTypeIo32:
      break;

    case PciBarTypeUnknown:
      break;

    default:
      break;
    }
  }
  //
  // If there is no resource requested from this device,
  // then we indicate this device has been allocated naturally.
  //
  if (!ResourceRequested) {
    PciDev->Allocated = TRUE;
  }
  return EFI_SUCCESS;
}

/**
  This function is used to create a resource node.

  @param PciDev       Pci device instance.
  @param Length       Length of Io/Memory resource.
  @param Alignment    Alignment of resource.
  @param Bar          Bar index.
  @param ResType      Type of resource: IO/Memory.
  @param ResUsage     Resource usage.

  @return PCI resource node created for given PCI device.
          NULL means PCI resource node is not created.

**/
PCI_RESOURCE_NODE *
CreateResourceNode (
  IN PCI_IO_DEVICE         *PciDev,
  IN UINT64                Length,
  IN UINT64                Alignment,
  IN UINT8                 Bar,
  IN PCI_BAR_TYPE          ResType,
  IN PCI_RESOURCE_USAGE    ResUsage
  )
{
  PCI_RESOURCE_NODE *Node;

  Node    = NULL;

  Node    = AllocateZeroPool (sizeof (PCI_RESOURCE_NODE));
  ASSERT (Node != NULL);
  if (Node == NULL) {
    return NULL;
  }

  Node->Signature     = PCI_RESOURCE_SIGNATURE;
  Node->PciDev        = PciDev;
  Node->Length        = Length;
  Node->Alignment     = Alignment;
  Node->Bar           = Bar;
  Node->ResType       = ResType;
  Node->Reserved      = FALSE;
  Node->ResourceUsage = ResUsage;
  InitializeListHead (&Node->ChildList);

  return Node;
}

/**
  This function is used to create a IOV VF resource node.

  @param PciDev       Pci device instance.
  @param Length       Length of Io/Memory resource.
  @param Alignment    Alignment of resource.
  @param Bar          Bar index.
  @param ResType      Type of resource: IO/Memory.
  @param ResUsage     Resource usage.

  @return PCI resource node created for given VF PCI device.
          NULL means PCI resource node is not created.

**/
PCI_RESOURCE_NODE *
CreateVfResourceNode (
  IN PCI_IO_DEVICE         *PciDev,
  IN UINT64                Length,
  IN UINT64                Alignment,
  IN UINT8                 Bar,
  IN PCI_BAR_TYPE          ResType,
  IN PCI_RESOURCE_USAGE    ResUsage
  )
{
  PCI_RESOURCE_NODE *Node;

  Node = CreateResourceNode (PciDev, Length, Alignment, Bar, ResType, ResUsage);
  if (Node == NULL) {
    return Node;
  }

  Node->Virtual = TRUE;

  return Node;
}

/**
  This function is used to extract resource request from
  device node list.

  @param Bridge     Pci device instance.
  @param IoNode     Resource info node for IO.
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.

**/
VOID
CreateResourceMap (
  IN     PCI_IO_DEVICE     *Bridge,
  IN OUT PCI_RESOURCE_NODE *IoNode,
  IN OUT PCI_RESOURCE_NODE *Mem32Node,
  IN OUT PCI_RESOURCE_NODE *PMem32Node,
  IN OUT PCI_RESOURCE_NODE *Mem64Node,
  IN OUT PCI_RESOURCE_NODE *PMem64Node
  )
{
  PCI_IO_DEVICE     *Temp;
  PCI_RESOURCE_NODE *IoBridge;
  PCI_RESOURCE_NODE *Mem32Bridge;
  PCI_RESOURCE_NODE *PMem32Bridge;
  PCI_RESOURCE_NODE *Mem64Bridge;
  PCI_RESOURCE_NODE *PMem64Bridge;
  LIST_ENTRY        *CurrentLink;
  EFI_STATUS        Status;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != NULL && CurrentLink != &Bridge->ChildList) {

    Temp = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    //
    // Create resource nodes for this device by scanning the
    // Bar array in the device private data
    // If the upstream bridge doesn't support this device,
    // no any resource node will be created for this device
    //
    Status = GetResourceFromDevice (
               Temp,
               IoNode,
               Mem32Node,
               PMem32Node,
               Mem64Node,
               PMem64Node
               );
    if (Status == EFI_INVALID_PARAMETER) {
      RemoveInvalidResRequest (
        Temp,
        &CurrentLink,
        IoNode,
        Mem32Node,
        PMem32Node,
        Mem64Node,
        PMem64Node
        );

      continue;    //continue to next device node
    }

    if (IS_PCI_BRIDGE (&Temp->Pci)) {

      //
      // If the device has children, create a bridge resource node for this PPB
      // Note: For PPB, memory aperture is aligned with 1MB and IO aperture
      // is aligned with 4KB (smaller alignments may be supported).
      //
      IoBridge = CreateResourceNode (
                   Temp,
                   0,
                   Temp->BridgeIoAlignment,
                   PPB_IO_RANGE,
                   PciBarTypeIo16,
                   PciResUsageTypical
                   );

      Mem32Bridge = CreateResourceNode (
                      Temp,
                      0,
                      0xFFFFF,
                      PPB_MEM32_RANGE,
                      PciBarTypeMem32,
                      PciResUsageTypical
                      );

      PMem32Bridge = CreateResourceNode (
                       Temp,
                       0,
                       0xFFFFF,
                       PPB_PMEM32_RANGE,
                       PciBarTypePMem32,
                       PciResUsageTypical
                       );

      Mem64Bridge = CreateResourceNode (
                      Temp,
                      0,
                      0xFFFFF,
                      PPB_MEM64_RANGE,
                      PciBarTypeMem64,
                      PciResUsageTypical
                      );

      PMem64Bridge = CreateResourceNode (
                       Temp,
                       0,
                       0xFFFFF,
                       PPB_PMEM64_RANGE,
                       PciBarTypePMem64,
                       PciResUsageTypical
                       );

      //
      // Recursively create resouce map on this bridge
      //
      CreateResourceMap (
        Temp,
        IoBridge,
        Mem32Bridge,
        PMem32Bridge,
        Mem64Bridge,
        PMem64Bridge
        );

      //
      // check if the overall resource requests of the bridge is invalid.
      // (e.g., the Mem32 requests > 4GB)
      //
      Status = CheckResRequestOnBridge (
                 Temp,
                 IoBridge,
                 Mem32Bridge,
                 PMem32Bridge,
                 Mem64Bridge,
                 PMem64Bridge
                 );

      if (ResourceRequestExisted (IoBridge)) {
        InsertResourceNode (
          IoNode,
          IoBridge
          );
      } else {
        FreePool (IoBridge);
        IoBridge = NULL;
      }

      //
      // If there is node under this resource bridge,
      // then calculate bridge's aperture of this type
      // and insert it into the respective resource tree.
      // If no, delete this resource bridge
      //
      if (ResourceRequestExisted (Mem32Bridge)) {
        InsertResourceNode (
          Mem32Node,
          Mem32Bridge
          );
      } else {
        FreePool (Mem32Bridge);
        Mem32Bridge = NULL;
      }

      //
      // If there is node under this resource bridge,
      // then calculate bridge's aperture of this type
      // and insert it into the respective resource tree.
      // If no, delete this resource bridge
      //
      if (ResourceRequestExisted (PMem32Bridge)) {
        InsertResourceNode (
          PMem32Node,
          PMem32Bridge
          );
      } else {
        FreePool (PMem32Bridge);
        PMem32Bridge = NULL;
      }

      //
      // If there is node under this resource bridge,
      // then calculate bridge's aperture of this type
      // and insert it into the respective resource tree.
      // If no, delete this resource bridge
      //
      if (ResourceRequestExisted (Mem64Bridge)) {
        InsertResourceNode (
          Mem64Node,
          Mem64Bridge
          );
      } else {
        FreePool (Mem64Bridge);
        Mem64Bridge = NULL;
      }

      //
      // If there is node under this resource bridge,
      // then calculate bridge's aperture of this type
      // and insert it into the respective resource tree.
      // If no, delete this resource bridge
      //
      if (ResourceRequestExisted (PMem64Bridge)) {
        InsertResourceNode (
          PMem64Node,
          PMem64Bridge
          );
      } else {
        FreePool (PMem64Bridge);
        PMem64Bridge = NULL;
      }

    }

    //
    // If it is P2C, apply hard coded resource padding
    //
    if (IS_CARDBUS_BRIDGE (&Temp->Pci)) {
      ResourcePaddingForCardBusBridge (
        Temp,
        IoNode,
        Mem32Node,
        PMem32Node,
        Mem64Node,
        PMem64Node
        );
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  //
  // To do some platform specific resource padding ...
  //
  ResourcePaddingPolicy (
    Bridge,
    IoNode,
    Mem32Node,
    PMem32Node,
    Mem64Node,
    PMem64Node
    );

  //
  // Degrade resource if necessary
  //
  DegradeResource (
    Bridge,
    Mem32Node,
    PMem32Node,
    Mem64Node,
    PMem64Node
    );

  if (Bridge->Parent == NULL) {
    //
    // Find the better memory allocation on the bus of the Root bridge.
    //
    ResourceAllocationOnRoot (Mem32Node);
    ResourceAllocationOnRoot (PMem32Node);
    ResourceAllocationOnRoot (Mem64Node);
    ResourceAllocationOnRoot (PMem64Node);
    ResourceAllocationOnRoot (IoNode);
  } else {
    //
    // Calculate resource aperture for this bridge device
    // the rule of memory allocation bases on alignment from high to low.
    //
    CalculateResourceAperture (Mem32Node);
    CalculateResourceAperture (PMem32Node);
    CalculateResourceAperture (Mem64Node);
    CalculateResourceAperture (PMem64Node);
    CalculateResourceAperture (IoNode);
  }

  AdjustResourceRange (
    Bridge,
    IoNode,
    Mem32Node,
    PMem32Node,
    Mem64Node,
    PMem64Node
    );

}

/**
  This function is used to do the resource padding for a specific platform.

  @param PciDev     Pci device instance.
  @param IoNode     Resource info node for IO.
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.

**/
VOID
ResourcePaddingPolicy (
  IN PCI_IO_DEVICE     *PciDev,
  IN PCI_RESOURCE_NODE *IoNode,
  IN PCI_RESOURCE_NODE *Mem32Node,
  IN PCI_RESOURCE_NODE *PMem32Node,
  IN PCI_RESOURCE_NODE *Mem64Node,
  IN PCI_RESOURCE_NODE *PMem64Node
  )
{
  //
  // Create padding resource node
  //
  if (PciDev->ResourcePaddingDescriptors != NULL) {
    ApplyResourcePadding (
      PciDev,
      IoNode,
      Mem32Node,
      PMem32Node,
      Mem64Node,
      PMem64Node
      );
  }
}

/**
  This function is used to degrade resource if the upstream bridge
  doesn't support certain resource. Degradation path is
  PMEM64 -> MEM64  -> MEM32
  PMEM64 -> PMEM32 -> MEM32
  IO32   -> IO16.

  @param Bridge     Pci device instance.
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.

**/
VOID
DegradeResource (
  IN PCI_IO_DEVICE     *Bridge,
  IN PCI_RESOURCE_NODE *Mem32Node,
  IN PCI_RESOURCE_NODE *PMem32Node,
  IN PCI_RESOURCE_NODE *Mem64Node,
  IN PCI_RESOURCE_NODE *PMem64Node
  )
{
  UINT64               GetPciHostBridgeDecodeAttributes;
  PCI_IO_DEVICE        *Temp;
  LIST_ENTRY           *ChildDeviceLink;
  LIST_ENTRY           *ChildNodeLink;
  LIST_ENTRY           *NextChildNodeLink;
  PCI_RESOURCE_NODE    *TempNode;

  GetPciHostBridgeDecodeAttributes = 0;

  //
  // Get Pci Host Bridge Decode Atrributes
  //
  if (Bridge->Parent != NULL) {
    GetPciHostBridgeDecodeAttributes = Bridge->Parent->Decodes;
  } else {
    GetPciHostBridgeDecodeAttributes = Bridge->Decodes;
  }
  //
  // If any child device has both option ROM and 64-bit BAR, degrade its PMEM64/MEM64
  // requests in case that if a legacy option ROM image can not access 64-bit resources.
  //
  ChildDeviceLink = Bridge->ChildList.ForwardLink;
  while (ChildDeviceLink != NULL && ChildDeviceLink != &Bridge->ChildList) {
    Temp = PCI_IO_DEVICE_FROM_LINK (ChildDeviceLink);
    if (Temp->RomSize != 0) {
      if (!IsListEmpty (&Mem64Node->ChildList)) {
        ChildNodeLink = Mem64Node->ChildList.ForwardLink;
        while (ChildNodeLink != &Mem64Node->ChildList) {
          TempNode = RESOURCE_NODE_FROM_LINK (ChildNodeLink);
          NextChildNodeLink = ChildNodeLink->ForwardLink;

          if (TempNode->PciDev == Temp) {
            RemoveEntryList (ChildNodeLink);
            InsertResourceNode (Mem32Node, TempNode);
          }
          ChildNodeLink = NextChildNodeLink;
        }
      }

      if (!IsListEmpty (&PMem64Node->ChildList)) {
        ChildNodeLink = PMem64Node->ChildList.ForwardLink;
        while (ChildNodeLink != &PMem64Node->ChildList) {
          TempNode = RESOURCE_NODE_FROM_LINK (ChildNodeLink);
          NextChildNodeLink = ChildNodeLink->ForwardLink;

          if (TempNode->PciDev == Temp) {
            RemoveEntryList (ChildNodeLink);
            InsertResourceNode (PMem32Node, TempNode);
          }
          ChildNodeLink = NextChildNodeLink;
        }
      }

    }
    ChildDeviceLink = ChildDeviceLink->ForwardLink;
  }

  //
  // If firmware is in 32-bit mode,
  // then degrade PMEM64/MEM64 requests
  //
  if (sizeof (UINTN) <= 4) {
    MergeResourceTree (
      Mem32Node,
      Mem64Node,
      TRUE
      );

    MergeResourceTree (
      PMem32Node,
      PMem64Node,
      TRUE
      );
  } else {
    //
    // if the bridge does not support MEM64, degrade MEM64 to MEM32
    //
    if(!(GetPciHostBridgeDecodeAttributes & EFI_BRIDGE_PMEM64_DECODE_SUPPORTED)) {
        MergeResourceTree (
          Mem32Node,
          Mem64Node,
          TRUE
          );
    }

    //
    // if the bridge does not support PMEM64, degrade PMEM64 to PMEM32
    //
    if (!BridgeSupportResourceDecode (Bridge, EFI_BRIDGE_PMEM64_DECODE_SUPPORTED)) {
      MergeResourceTree (
        PMem32Node,
        PMem64Node,
        TRUE
        );
    }

    //
    // if both PMEM64 and PMEM32 requests from child devices, which can not be satisfied
    // by a P2P bridge simultaneously, keep PMEM64 and degrade PMEM32 to MEM32.
    //
    if (!IsListEmpty (&PMem64Node->ChildList) && Bridge->Parent != NULL) {
      MergeResourceTree (
        Mem32Node,
        PMem32Node,
        TRUE
        );
    }

    //
    // If bridge doesn't support Mem64
    // degrade it to mem32
    //
    if (!BridgeSupportResourceDecode (Bridge, EFI_BRIDGE_MEM64_DECODE_SUPPORTED)) {
      MergeResourceTree (
        Mem32Node,
        Mem64Node,
        TRUE
        );
    }
  }

  //
  // If bridge doesn't support Pmem32
  // degrade it to mem32
  //
  if (!BridgeSupportResourceDecode (Bridge, EFI_BRIDGE_PMEM32_DECODE_SUPPORTED)) {
    MergeResourceTree (
      Mem32Node,
      PMem32Node,
      TRUE
      );
  }

  //
  // if root bridge supports combined Pmem Mem decoding
  // merge these two type of resource
  //
  if (BridgeSupportResourceDecode (Bridge, EFI_BRIDGE_PMEM_MEM_COMBINE_SUPPORTED)) {
    MergeResourceTree (
      Mem32Node,
      PMem32Node,
      FALSE
      );

    //
    // No need to check if to degrade MEM64 after merge, because
    // if there are PMEM64 still here, 64-bit decode should be supported
    // by the root bride.
    //
    MergeResourceTree (
      Mem64Node,
      PMem64Node,
      FALSE
      );
  }
}

/**
  Test whether bridge device support decode resource.

  @param Bridge    Bridge device instance.
  @param Decode    Decode type according to resource type.

  @return TRUE     The bridge device support decode resource.
  @return FALSE    The bridge device don't support decode resource.

**/
BOOLEAN
BridgeSupportResourceDecode (
  IN PCI_IO_DEVICE *Bridge,
  IN UINT32        Decode
  )
{
  if (((Bridge->Decodes) & Decode) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  This function is used to program the resource allocated
  for each resource node under specified bridge.

  @param Base     Base address of resource to be progammed.
  @param Bridge   PCI resource node for the bridge device.

  @retval EFI_SUCCESS            Successfully to program all resouces
                                 on given PCI bridge device.
  @retval EFI_OUT_OF_RESOURCES   Base is all one.

**/
EFI_STATUS
ProgramResource (
  IN UINT64            Base,
  IN PCI_RESOURCE_NODE *Bridge
  )
{
  LIST_ENTRY        *CurrentLink;
  PCI_RESOURCE_NODE *Node;
  EFI_STATUS        Status;

  if (Base == gAllOne) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != &Bridge->ChildList) {

    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);

    if (!IS_PCI_BRIDGE (&(Node->PciDev->Pci))) {

      if (IS_CARDBUS_BRIDGE (&(Node->PciDev->Pci))) {
        //
        // Program the PCI Card Bus device
        //
        ProgramP2C (Base, Node);
      } else {
        //
        // Program the PCI device BAR
        //
        DEBUG_CODE (
          {
            DEBUG ((EFI_D_INFO, " PCI device: 0x%02x/0x%02x/0x%02x, BarOffset_0x%02x, Type_%d, Address_0x%lx\n", \
                    Node->PciDev->BusNumber, Node->PciDev->DeviceNumber, Node->PciDev->FunctionNumber, \
                    (Node->PciDev->PciBar[Node->Bar]).Offset, (Node->PciDev->PciBar[Node->Bar]).BarType, Base + Node->Offset));
          }
        );
        ProgramBar (Base, Node);
      }
    } else {
      //
      // Program the PCI devices under this bridge
      //
      Status = ProgramResource (Base + Node->Offset, Node);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      DEBUG_CODE (
        {
          DEBUG ((EFI_D_INFO, " <PCI Bridge>: 0x%02x/0x%02x/0x%02x, BarOffset_0x%02x, Type_%d, Address_0x%lx\n", \
                  Node->PciDev->BusNumber, Node->PciDev->DeviceNumber, Node->PciDev->FunctionNumber, \
                  (Node->PciDev->PciBar[Node->Bar]).Offset, Node->Bar, Base + Node->Offset));
        }
      );
      ProgramPpbApperture (Base, Node);
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return EFI_SUCCESS;
}

/**
  Program Bar register for PCI device.

  @param Base  Base address for PCI device resource to be progammed.
  @param Node  Point to resoure node structure.

**/
VOID
ProgramBar (
  IN UINT64            Base,
  IN PCI_RESOURCE_NODE *Node
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINT64              Address;
  UINT32              Address32;
  UINT64              BootScriptPciAddress;
  UINTN               Segment;
  UINTN               Bus;
  UINTN               Device;
  UINTN               Function;

  ASSERT (Node->Bar < PCI_MAX_BAR);

  //
  // Check VF BAR
  //
  if (Node->Virtual) {
    ProgramVfBar (Base, Node);
    return;
  }

  Address = 0;
  PciIo   = &(Node->PciDev->PciIo);
  PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);

  Address = Base + Node->Offset;

  //
  // Indicate pci bus driver has allocated
  // resource for this device
  // It might be a temporary solution here since
  // pci device could have multiple bar
  //
  Node->PciDev->Allocated = TRUE;

  switch ((Node->PciDev->PciBar[Node->Bar]).BarType) {

  case PciBarTypeIo16:
  case PciBarTypeIo32:
  case PciBarTypeMem32:
  case PciBarTypePMem32:

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (Node->PciDev->PciBar[Node->Bar]).Offset,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;

    break;

  case PciBarTypeMem64:
  case PciBarTypePMem64:

    Address32 = (UINT32) (Address & 0x00000000FFFFFFFF);

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (Node->PciDev->PciBar[Node->Bar]).Offset,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address32
      );


    Address32 = (UINT32) RShiftU64 (Address, 32);

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (UINT8) ((Node->PciDev->PciBar[Node->Bar]).Offset + 4),
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset + 4);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;

    break;

  default:
    break;
  }
}

/**
  Program IOV VF Bar register for PCI device.

  @param Base  Base address for PCI device resource to be progammed.
  @param Node  Point to resoure node structure.

**/
EFI_STATUS
ProgramVfBar (
  IN UINT64            Base,
  IN PCI_RESOURCE_NODE *Node
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINT64              Address;
  UINT32              Address32;

  ASSERT (Node->Bar < PCI_MAX_BAR);
  ASSERT (Node->Virtual);

  Address = 0;
  PciIo   = &(Node->PciDev->PciIo);

  Address = Base + Node->Offset;

  //
  // Indicate pci bus driver has allocated
  // resource for this device
  // It might be a temporary solution here since
  // pci device could have multiple bar
  //
  Node->PciDev->Allocated = TRUE;

  switch ((Node->PciDev->VfPciBar[Node->Bar]).BarType) {

  case PciBarTypeMem32:
  case PciBarTypePMem32:

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (Node->PciDev->VfPciBar[Node->Bar]).Offset,
                 1,
                 &Address
                 );

    Node->PciDev->VfPciBar[Node->Bar].BaseAddress = Address;
    break;

  case PciBarTypeMem64:
  case PciBarTypePMem64:

    Address32 = (UINT32) (Address & 0x00000000FFFFFFFF);

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (Node->PciDev->VfPciBar[Node->Bar]).Offset,
                 1,
                 &Address32
                 );

    Address32 = (UINT32) RShiftU64 (Address, 32);

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 ((Node->PciDev->VfPciBar[Node->Bar]).Offset + 4),
                 1,
                 &Address32
                 );

    Node->PciDev->VfPciBar[Node->Bar].BaseAddress = Address;
    break;

  case PciBarTypeIo16:
  case PciBarTypeIo32:
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}

/**
  Program PCI-PCI bridge apperture.

  @param Base  Base address for resource.
  @param Node  Point to resoure node structure.

**/
VOID
ProgramPpbApperture (
  IN UINT64            Base,
  IN PCI_RESOURCE_NODE *Node
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINT64              Address;
  UINT32              Address32;
  UINTN               Segment;
  UINTN               Bus;
  UINTN               Device;
  UINTN               Function;
  UINT64              BootScriptPciAddress;

  Address = 0;
  //
  // If no device resource of this PPB, return anyway
  // Apperture is set default in the initialization code
  //
  if (Node->Length == 0 || Node->ResourceUsage == PciResUsagePadding) {
    //
    // For padding resource node, just ignore when programming
    //
    return ;
  }

  PciIo   = &(Node->PciDev->PciIo);
  PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  Address = Base + Node->Offset;

  //
  // Indicate the PPB resource has been allocated
  //
  Node->PciDev->Allocated = TRUE;

  switch (Node->Bar) {

  case PPB_BAR_0:
  case PPB_BAR_1:
    if ((Node->Bar == PPB_BAR_0) && (Address > 0xFFFFFFFF)) {
      Address32 = (UINT32) (Address & 0x00000000FFFFFFFF);
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint32,
                   (Node->PciDev->PciBar[Node->Bar]).Offset,
                   1,
                   &Address32
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint32,
        BootScriptPciAddress,
        1,
        &Address32
        );
  
      Address32 = (UINT32) RShiftU64 (Address, 32);
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint32,
                   (UINT8) ((Node->PciDev->PciBar[Node->Bar]).Offset + 4),
                   1,
                   &Address32
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset + 4);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint32,
        BootScriptPciAddress,
        1,
        &Address32
        );

    } else {
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint32,
                   (Node->PciDev->PciBar[Node->Bar]).Offset,
                   1,
                   &Address
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint32,
        BootScriptPciAddress,
        1,
        &Address
        );
    }

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;

    break;

  case PPB_IO_RANGE:

    Address32 = ((UINT32) (Address)) >> 8;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint8,
                 0x1C,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x1C);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint8,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 >>= 8;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x30,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x30);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 = (UINT32) (Address + Node->Length - 1);
    Address32 = ((UINT32) (Address32)) >> 8;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint8,
                 0x1D,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x1D);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint8,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 >>= 8;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x32,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x32);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    break;

  case PPB_MEM32_RANGE:

    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x20,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x20);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 = (UINT32) (Address + Node->Length - 1);
    Address32 = ((UINT32) (Address32)) >> 16;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x22,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x22);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    break;

  case PPB_PMEM32_RANGE:
  case PPB_PMEM64_RANGE:

    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x24,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x24);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 = (UINT32) (Address + Node->Length - 1);
    Address32 = ((UINT32) (Address32)) >> 16;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x26,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x26);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 = (UINT32) RShiftU64 (Address, 32);
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0x28,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x28);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Address32 = (UINT32) RShiftU64 ((Address + Node->Length - 1), 32);
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0x2C,
                 1,
                 &Address32
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, 0x2C);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address32
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    break;

  default:
    break;
  }
}

/**
  Program parent bridge for Option Rom.

  @param PciDevice      Pci deivce instance.
  @param OptionRomBase  Base address for Optiona Rom.
  @param Enable         Enable or disable PCI memory.

**/
VOID
ProgrameUpstreamBridgeForRom (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT32          OptionRomBase,
  IN BOOLEAN         Enable
  )
{
  PCI_IO_DEVICE     *Parent;
  PCI_RESOURCE_NODE Node;
  //
  // For root bridge, just return.
  //
  Parent = PciDevice->Parent;
  ZeroMem (&Node, sizeof (Node));
  while (Parent != NULL) {
    if (!IS_PCI_BRIDGE (&Parent->Pci)) {
      break;
    }

    Node.PciDev     = Parent;
    Node.Length     = PciDevice->RomSize;
    Node.Alignment  = 0;
    Node.Bar        = PPB_MEM32_RANGE;
    Node.ResType    = PciBarTypeMem32;
    Node.Offset     = 0;

    //
    // Program PPB to only open a single <= 16MB apperture
    //
    if (Enable) {
      ProgramPpbApperture (OptionRomBase, &Node);
      PCI_ENABLE_COMMAND_REGISTER (Parent, EFI_PCI_COMMAND_MEMORY_SPACE);
    } else {
      InitializePpb (Parent);
      PCI_DISABLE_COMMAND_REGISTER (Parent, EFI_PCI_COMMAND_MEMORY_SPACE);
    }

    Parent = Parent->Parent;
  }
}

/**
  Test whether resource exists for a bridge.

  @param Bridge  Point to resource node for a bridge.

  @retval TRUE   There is resource on the given bridge.
  @retval FALSE  There isn't resource on the given bridge.

**/
BOOLEAN
ResourceRequestExisted (
  IN PCI_RESOURCE_NODE    *Bridge
  )
{
  if (Bridge != NULL) {
    if (!IsListEmpty (&Bridge->ChildList) || Bridge->Length != 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Initialize resource pool structure.

  @param ResourcePool Point to resource pool structure. This pool
                      is reset to all zero when returned.
  @param ResourceType Type of resource.

**/
VOID
InitializeResourcePool (
  IN OUT PCI_RESOURCE_NODE   *ResourcePool,
  IN     PCI_BAR_TYPE        ResourceType
  )
{
  ZeroMem (ResourcePool, sizeof (PCI_RESOURCE_NODE));
  ResourcePool->ResType   = ResourceType;
  ResourcePool->Signature = PCI_RESOURCE_SIGNATURE;
  InitializeListHead (&ResourcePool->ChildList);
}

/**
  Destory given resource tree.

  @param Bridge  PCI resource root node of resource tree.

**/
VOID
DestroyResourceTree (
  IN PCI_RESOURCE_NODE *Bridge
  )
{
  PCI_RESOURCE_NODE *Temp;
  LIST_ENTRY        *CurrentLink;

  while (!IsListEmpty (&Bridge->ChildList)) {

    CurrentLink = Bridge->ChildList.ForwardLink;

    Temp        = RESOURCE_NODE_FROM_LINK (CurrentLink);
    ASSERT (Temp);

    RemoveEntryList (CurrentLink);

    if (IS_PCI_BRIDGE (&(Temp->PciDev->Pci))) {
      DestroyResourceTree (Temp);
    }

    FreePool (Temp);
  }
}

/**
  Insert resource padding for P2C.

  @param PciDev     Pci device instance.
  @param IoNode     Resource info node for IO.
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.

**/
VOID
ResourcePaddingForCardBusBridge (
  IN PCI_IO_DEVICE        *PciDev,
  IN PCI_RESOURCE_NODE    *IoNode,
  IN PCI_RESOURCE_NODE    *Mem32Node,
  IN PCI_RESOURCE_NODE    *PMem32Node,
  IN PCI_RESOURCE_NODE    *Mem64Node,
  IN PCI_RESOURCE_NODE    *PMem64Node
  )
{
  PCI_RESOURCE_NODE *Node;

  Node = NULL;

  //
  // Memory Base/Limit Register 0
  // Bar 1 denodes memory range 0
  //
  Node = CreateResourceNode (
           PciDev,
           0x2000000,
           0x1ffffff,
           1,
           PciBarTypeMem32,
           PciResUsagePadding
           );

  InsertResourceNode (
    Mem32Node,
    Node
    );

  //
  // Memory Base/Limit Register 1
  // Bar 2 denodes memory range1
  //
  Node = CreateResourceNode (
           PciDev,
           0x2000000,
           0x1ffffff,
           2,
           PciBarTypePMem32,
           PciResUsagePadding
           );

  InsertResourceNode (
    PMem32Node,
    Node
    );

  //
  // Io Base/Limit
  // Bar 3 denodes io range 0
  //
  Node = CreateResourceNode (
           PciDev,
           0x100,
           0xff,
           3,
           PciBarTypeIo16,
           PciResUsagePadding
           );

  InsertResourceNode (
    IoNode,
    Node
    );

  //
  // Io Base/Limit
  // Bar 4 denodes io range 0
  //
  Node = CreateResourceNode (
           PciDev,
           0x100,
           0xff,
           4,
           PciBarTypeIo16,
           PciResUsagePadding
           );

  InsertResourceNode (
    IoNode,
    Node
    );
}

/**
  Program PCI Card device register for given resource node.

  @param Base    Base address of PCI Card device to be programmed.
  @param Node    Given resource node.

**/
VOID
ProgramP2C (
  IN UINT64            Base,
  IN PCI_RESOURCE_NODE *Node
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINT64              Address;
  UINT64              TempAddress;
  UINT16              BridgeControl;
  UINTN               Segment;
  UINTN               Bus;
  UINTN               Device;
  UINTN               Function;
  UINT64              BootScriptPciAddress;

  Address = 0;
  PciIo   = &(Node->PciDev->PciIo);
  PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  Address = Base + Node->Offset;

  //
  // Indicate pci bus driver has allocated
  // resource for this device
  // It might be a temporary solution here since
  // pci device could have multiple bar
  //
  Node->PciDev->Allocated = TRUE;

  switch (Node->Bar) {

  case P2C_BAR_0:
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 (Node->PciDev->PciBar[Node->Bar]).Offset,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, (Node->PciDev->PciBar[Node->Bar]).Offset);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    break;

  case P2C_MEM_1:
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_MEMORY_BASE_0,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_MEMORY_BASE_0);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    TempAddress = Address + Node->Length - 1;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_MEMORY_LIMIT_0,
                 1,
                 &TempAddress
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_MEMORY_LIMIT_0);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &TempAddress
      );

    if (Node->ResType == PciBarTypeMem32) {
      //
      // Set non-prefetchable bit
      //
      PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );

      BridgeControl &= (UINT16) ~PCI_CARD_PREFETCHABLE_MEMORY_0_ENABLE;
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_BRIDGE_CONTROL);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint16,
        BootScriptPciAddress,
        1,
        &BridgeControl
        );

    } else {
      //
      // Set pre-fetchable bit
      //
      PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );

      BridgeControl |= PCI_CARD_PREFETCHABLE_MEMORY_0_ENABLE;
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_BRIDGE_CONTROL);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint16,
        BootScriptPciAddress,
        1,
        &BridgeControl
        );

    }

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    Node->PciDev->PciBar[Node->Bar].BarType     = Node->ResType;

    break;

  case P2C_MEM_2:
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_MEMORY_BASE_1,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_MEMORY_BASE_1);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    TempAddress = Address + Node->Length - 1;

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_MEMORY_LIMIT_1,
                 1,
                 &TempAddress
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_MEMORY_LIMIT_1);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &TempAddress
      );

    if (Node->ResType == PciBarTypeMem32) {

      //
      // Set non-prefetchable bit
      //
      PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );

      BridgeControl &= (UINT16) ~(PCI_CARD_PREFETCHABLE_MEMORY_1_ENABLE);
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_BRIDGE_CONTROL);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint16,
        BootScriptPciAddress,
        1,
        &BridgeControl
        );

    } else {

      //
      // Set pre-fetchable bit
      //
      PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );

      BridgeControl |= PCI_CARD_PREFETCHABLE_MEMORY_1_ENABLE;
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint16,
                   PCI_CARD_BRIDGE_CONTROL,
                   1,
                   &BridgeControl
                   );
      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_BRIDGE_CONTROL);
      S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint16,
        BootScriptPciAddress,
        1,
        &BridgeControl
        );

    }

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    Node->PciDev->PciBar[Node->Bar].BarType     = Node->ResType;
    break;

  case P2C_IO_1:
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_IO_BASE_0_LOWER,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_IO_BASE_0_LOWER);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    TempAddress = Address + Node->Length - 1;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_IO_LIMIT_0_LOWER,
                 1,
                 &TempAddress
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_IO_LIMIT_0_LOWER);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &TempAddress
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    Node->PciDev->PciBar[Node->Bar].BarType     = Node->ResType;

    break;

  case P2C_IO_2:
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_IO_BASE_1_LOWER,
                 1,
                 &Address
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_IO_BASE_1_LOWER);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &Address
      );

    TempAddress = Address + Node->Length - 1;
    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint32,
                 PCI_CARD_IO_LIMIT_1_LOWER,
                 1,
                 &TempAddress
                 );
    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, PCI_CARD_IO_LIMIT_1_LOWER);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      BootScriptPciAddress,
      1,
      &TempAddress
      );

    Node->PciDev->PciBar[Node->Bar].BaseAddress = Address;
    Node->PciDev->PciBar[Node->Bar].Length      = Node->Length;
    Node->PciDev->PciBar[Node->Bar].BarType     = Node->ResType;
    break;

  default:
    break;
  }
}

/**
  Create padding resource node.

  @param PciDev     Pci device instance.
  @param IoNode     Resource info node for IO.
  @param Mem32Node  Resource info node for 32-bit memory.
  @param PMem32Node Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node  Resource info node for 64-bit memory.
  @param PMem64Node Resource info node for 64-bit Prefetchable Memory.

**/
VOID
ApplyResourcePadding (
  IN PCI_IO_DEVICE         *PciDev,
  IN PCI_RESOURCE_NODE     *IoNode,
  IN PCI_RESOURCE_NODE     *Mem32Node,
  IN PCI_RESOURCE_NODE     *PMem32Node,
  IN PCI_RESOURCE_NODE     *Mem64Node,
  IN PCI_RESOURCE_NODE     *PMem64Node
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Ptr;
  PCI_RESOURCE_NODE                 *Node;
  UINT8                             DummyBarIndex;

  DummyBarIndex = 0;
  Ptr           = PciDev->ResourcePaddingDescriptors;

  while (((EFI_ACPI_END_TAG_DESCRIPTOR *) Ptr)->Desc != ACPI_END_TAG_DESCRIPTOR) {

    if (Ptr->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR && Ptr->ResType == ACPI_ADDRESS_SPACE_TYPE_IO) {
      if (Ptr->AddrLen != 0) {

        Node = CreateResourceNode (
                 PciDev,
                 Ptr->AddrLen,
                 Ptr->AddrRangeMax,
                 DummyBarIndex,
                 PciBarTypeIo16,
                 PciResUsagePadding
                 );
        InsertResourceNode (
          IoNode,
          Node
          );
      }

      Ptr++;
      continue;
    }

    if (Ptr->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR && Ptr->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM) {

      if (Ptr->AddrSpaceGranularity == 32) {

        //
        // prefechable
        //
        if (Ptr->SpecificFlag == 0x6) {
          if (Ptr->AddrLen != 0) {
            Node = CreateResourceNode (
                     PciDev,
                     Ptr->AddrLen,
                     Ptr->AddrRangeMax,
                     DummyBarIndex,
                     PciBarTypePMem32,
                     PciResUsagePadding
                     );
            InsertResourceNode (
              PMem32Node,
              Node
              );
          }

          Ptr++;
          continue;
        }

        //
        // Non-prefechable
        //
        if (Ptr->SpecificFlag == 0) {
          if (Ptr->AddrLen != 0) {
            Node = CreateResourceNode (
                     PciDev,
                     Ptr->AddrLen,
                     Ptr->AddrRangeMax,
                     DummyBarIndex,
                     PciBarTypeMem32,
                     PciResUsagePadding
                     );
            InsertResourceNode (
              Mem32Node,
              Node
              );
          }

          Ptr++;
          continue;
        }
      }

      if (Ptr->AddrSpaceGranularity == 64) {

        //
        // prefechable
        //
        if (Ptr->SpecificFlag == 0x6) {
          if (Ptr->AddrLen != 0) {
            Node = CreateResourceNode (
                     PciDev,
                     Ptr->AddrLen,
                     Ptr->AddrRangeMax,
                     DummyBarIndex,
                     PciBarTypePMem64,
                     PciResUsagePadding
                     );
            InsertResourceNode (
              PMem64Node,
              Node
              );
          }

          Ptr++;
          continue;
        }

        //
        // Non-prefechable
        //
        if (Ptr->SpecificFlag == 0) {
          if (Ptr->AddrLen != 0) {
            Node = CreateResourceNode (
                     PciDev,
                     Ptr->AddrLen,
                     Ptr->AddrRangeMax,
                     DummyBarIndex,
                     PciBarTypeMem64,
                     PciResUsagePadding
                     );
            InsertResourceNode (
              Mem64Node,
              Node
              );
          }

          Ptr++;
          continue;
        }
      }
    }

    Ptr++;
  }
}

/**
  Get padding resource for PCI-PCI bridge.

  @param  PciIoDevice     PCI-PCI bridge device instance.

  @note   Feature flag PcdPciBusHotplugDeviceSupport determines
          whether need to pad resource for them.
**/
VOID
GetResourcePaddingPpb (
  IN  PCI_IO_DEVICE                  *PciIoDevice
  )
{
  if (gPciHotPlugInit != NULL && FeaturePcdGet (PcdPciBusHotplugDeviceSupport)) {
    if (PciIoDevice->ResourcePaddingDescriptors == NULL) {
      GetResourcePaddingForHpb (PciIoDevice);
    }
  }
}

VOID
SearchPaddingResourceNode (
  IN       PCI_RESOURCE_NODE  *Bridge,
  OUT      PCI_RESOURCE_NODE  **PaddingResourceNode
  )
{
  LIST_ENTRY        *CurrentLink;
  PCI_RESOURCE_NODE *Temp;

  *PaddingResourceNode = NULL;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != &Bridge->ChildList) {

    Temp = RESOURCE_NODE_FROM_LINK (CurrentLink);

    if (Temp->ResourceUsage == PciResUsagePadding && \
        Temp->PciDev->BusNumber == Bridge->PciDev->BusNumber && \
        Temp->PciDev->DeviceNumber == Bridge->PciDev->DeviceNumber && \
        Temp->PciDev->FunctionNumber == Bridge->PciDev->FunctionNumber) {
      *PaddingResourceNode = Temp;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }
}

VOID
HotplugReCalculateResourceAperture (
  IN  PCI_RESOURCE_NODE                   *Bridge
  )
{
  EFI_STATUS              Status;
  UINT64                  Aperture;
  LIST_ENTRY              *CurrentLink;
  PCI_RESOURCE_NODE       *Node;
  PCI_RESOURCE_NODE       *PaddingResourceNode;
  UINT64                  offset;
  UINTN                   ResourceNodeIndex;
  EFI_PCI_PLATFORM_POLICY PciPolicy;
  UINT64                  MinDevAlignment;

  if (Bridge == NULL) {
    return;
  }

  if (!mPolicyDetermined) {
    //
    // Check PciPlatform policy
    //
    Status = EFI_NOT_FOUND;
    PciPolicy = 0;
    if (gPciPlatformProtocol != NULL) {
      Status = gPciPlatformProtocol->GetPlatformPolicy (
                                       gPciPlatformProtocol,
                                       &PciPolicy
                                       );
    }

    if (EFI_ERROR (Status) && gPciOverrideProtocol != NULL) {
      Status = gPciOverrideProtocol->GetPlatformPolicy (
                                       gPciOverrideProtocol,
                                       &PciPolicy
                                       );
    }

    if (!EFI_ERROR (Status)) {
      if ((PciPolicy & EFI_RESERVE_ISA_IO_ALIAS) != 0) {
        mReserveIsaAliases = TRUE;
      }
      if ((PciPolicy & EFI_RESERVE_VGA_IO_ALIAS) != 0) {
        mReserveVgaAliases = TRUE;
      }
    }
    mPolicyDetermined = TRUE;
  }

  PaddingResourceNode   = NULL;
  SearchPaddingResourceNode (Bridge, &PaddingResourceNode);
  if (PaddingResourceNode == NULL) {
    return;
  }
  //
  // Remove the Hotplug Resource Node and ReCalculate resource.
  //
  RemoveEntryList (&PaddingResourceNode->Link);

  Aperture          = 0;
  ResourceNodeIndex = 0;
  MinDevAlignment   = 0;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink != &Bridge->ChildList) {

    Node = RESOURCE_NODE_FROM_LINK (CurrentLink);

    ResourceNodeIndex++;

    //
    // Find Min Device Node Alignment, mainly for hotplug function
    //
    if (MinDevAlignment == 0) {
      MinDevAlignment = Node->Alignment;
    } else {
      if (MinDevAlignment > Node->Alignment) {
        MinDevAlignment = Node->Alignment;
      }
    }

    //
    // Assume the start address is the bridge's alignment.
    // Check with the first node's alignment. make sure the alignment of first node is correct.
    //
    if (ResourceNodeIndex == 1) {
      offset = (Bridge->Alignment + 1) & (Node->Alignment);
    } else {
      offset = Aperture & (Node->Alignment);
    }

    if (offset) {
      Aperture = Aperture + (Node->Alignment + 1) - offset;
    }

    //
    // IsaEnable and VGAEnable can not be implemented now.
    // If both of them are enabled, then the IO resource would
    // become too limited to meet the requirement of most of devices.
    //
    if ((mReserveIsaAliases || mReserveVgaAliases) && (Bridge->ResType == PciBarTypeIo16)) {
      if (!IS_PCI_BRIDGE (&(Node->PciDev->Pci)) && !IS_CARDBUS_BRIDGE (&(Node->PciDev->Pci))) {
        //
        // Check if there is need to support ISA/VGA decoding
        // If so, we need to avoid isa/vga aliasing range
        //
        if (mReserveIsaAliases) {
          SkipIsaAliasAperture (
            &Aperture,
            Node->Length
            );
          offset = Aperture & (Node->Alignment);
          if (offset != 0) {
            Aperture = Aperture + (Node->Alignment + 1) - offset;
          }
        } else if (mReserveVgaAliases) {
          SkipVGAAperture (
            &Aperture,
            Node->Length
            );
          offset = Aperture & (Node->Alignment);
          if (offset != 0) {
            Aperture = Aperture + (Node->Alignment + 1) - offset;
          }
        }
      }
    }

    Node->Offset = Aperture;

    Aperture += Node->Length;

    CurrentLink = CurrentLink->ForwardLink;
  }
  //
  // At last, adjust the aperture with the bridge's alignment.
  //
  if (Bridge->ResType == PciBarTypeIo16) {
    offset = Aperture & (Bridge->Alignment);
    if (offset) {
      Aperture = Aperture + (Bridge->Alignment + 1) - offset;
    }
  } else {
    //
    // Bridge MMIO, 1M Alignment, mainly for hotplug function
    // Follow PCI-to-PCI Bridge Architecture Specification Revision 1.2, chapter 3.2.5.8
	//
    if (MinDevAlignment < 0xFFFFF) {
      MinDevAlignment = 0xFFFFF;
    }
    
    offset = Aperture & (MinDevAlignment);
    if (offset) {
      Aperture = Aperture + (MinDevAlignment + 1) - offset;
    }
  }
  //
  // adjust the bridge length with the Padding resource's length.
  //
  if (Aperture < PaddingResourceNode->Length) {
    Aperture = PaddingResourceNode->Length;
  }
  //
  // Update Bridge length.
  //
  Bridge->Length = Aperture;
}

VOID
AdjustResvRange (
  IN  PCI_RESOURCE_NODE                   *Node,
  IN  UINT64                              ResvLength
  )
{
  //
  // Node->Length is the "ResvLength + plug-in Device request length".
  //
  if (Node->Length != ResvLength && ResvLength != 0x00) {
    //
    // There is the device adding in the bridge which has reserved resource requirement.
    // Check the Reserved resource length and the device resource length.
    // Case1: if device resource length larger than Reserved resource length
    //        ==> the resource length of bridge will be the device resource length.
    // Case2: if Reserved resource length larger than device resource.
    //        ==> the resource length of bridge will be the Reserved resource length.
    //
    HotplugReCalculateResourceAperture (Node);
  }
}

/**
 This routine is used to adjust the resource range of the bridge.
 If the total device request resource behind this bridge is not larger then the default reserved resource for this bridge,
 then set the resource length of this bridge as reserved resource length,
 else set the resource length as total device request resource length behind this bridge.

 @param [in]   Bridge
 @param [in]   IoNode
 @param [in]   Mem32Node
 @param [in]   PMem32Node
 @param [in]   Mem64Node
 @param [in]   PMem64Node


**/
VOID
AdjustResourceRange (
  IN PCI_IO_DEVICE      *Bridge,
  IN PCI_RESOURCE_NODE  *IoNode,
  IN PCI_RESOURCE_NODE  *Mem32Node,
  IN PCI_RESOURCE_NODE  *PMem32Node,
  IN PCI_RESOURCE_NODE  *Mem64Node,
  IN PCI_RESOURCE_NODE  *PMem64Node
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR       *AcpiAddrPtr  = NULL;
  UINT64                                  ResvIoLen     = 0x00;
  UINT64                                  ResvMem32Len  = 0x00;
  UINT64                                  ResvPMem32Len = 0x00;
  UINT64                                  ResvMem64Len  = 0x00;
  UINT64                                  ResvPMem64Len = 0x00;

  if (Bridge->ResourcePaddingDescriptors != NULL) {
    AcpiAddrPtr = Bridge->ResourcePaddingDescriptors;

    while (((EFI_ACPI_END_TAG_DESCRIPTOR *) AcpiAddrPtr)->Desc != ACPI_END_TAG_DESCRIPTOR) {
      if ((AcpiAddrPtr->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) &&
          (AcpiAddrPtr->ResType == ACPI_ADDRESS_SPACE_TYPE_IO)) {
        ResvIoLen = AcpiAddrPtr->AddrLen;
        AcpiAddrPtr++;
        continue;
      }
      if ((AcpiAddrPtr->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) &&
          (AcpiAddrPtr->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM)) {
        if (AcpiAddrPtr->AddrSpaceGranularity == 32) {
          //
          // Not-Prefetctable
          //
          if (AcpiAddrPtr->SpecificFlag == 0x0) {
            ResvMem32Len = AcpiAddrPtr->AddrLen;
            AcpiAddrPtr++;
            continue;
          }
          //
          // Prefetctable
          //
          if (AcpiAddrPtr->SpecificFlag == 0x6) {
            ResvPMem32Len = AcpiAddrPtr->AddrLen;
            AcpiAddrPtr++;
            continue;
          }
        }
        if (AcpiAddrPtr->AddrSpaceGranularity == 64) {
          //
          // Not-Prefetctable
          //
          if (AcpiAddrPtr->SpecificFlag == 0x0) {
            ResvMem64Len = AcpiAddrPtr->AddrLen;
            AcpiAddrPtr++;
            continue;
          }
          //
          // Prefetctable
          //
          if (AcpiAddrPtr->SpecificFlag == 0x6) {
            ResvPMem64Len = AcpiAddrPtr->AddrLen;
            AcpiAddrPtr++;
            continue;
          }
        }
      }
      AcpiAddrPtr++;
    }

    AdjustResvRange (IoNode, ResvIoLen);
    AdjustResvRange (Mem32Node, ResvMem32Len);
    AdjustResvRange (PMem32Node, ResvPMem32Len);
    AdjustResvRange (Mem64Node, ResvMem64Len);
    AdjustResvRange (PMem64Node, ResvPMem64Len);
  }
}

/**
  Remove the Mem32 >= 4GB request from the ResNode,
  remove the requesting device node from the ChildList of its parent bridge,
  and remove all its other resource requests.

  @param PciDev         Requesting Pci device instance.
  @param PciDevLink     Point to the next device node when the function returns.
  @param IoNode         Resource info node for IO .
  @param Mem32Node      Resource info node for 32-bit memory.
  @param PMem32Node     Resource info node for 32-bit Prefetchable Memory.
  @param Mem64Node      Resource info node for 64-bit memory.
  @param PMem64Node     Resource info node for 64-bit Prefetchable Memory.

**/
VOID
RemoveInvalidResRequest (
  IN OUT PCI_IO_DEVICE     *PciDev,
  IN OUT LIST_ENTRY        **PciDevLink,
  IN OUT PCI_RESOURCE_NODE *IoNode,
  IN OUT PCI_RESOURCE_NODE *Mem32Node,
  IN OUT PCI_RESOURCE_NODE *PMem32Node,
  IN OUT PCI_RESOURCE_NODE *Mem64Node,
  IN OUT PCI_RESOURCE_NODE *PMem64Node
  )
{
  LIST_ENTRY           *CurLink;
  LIST_ENTRY           *RemoveLink;
  PCI_RESOURCE_NODE    *CurResNode;
  PCI_IO_DEVICE        *ParentBridge;
  PCI_IO_DEVICE        *RemoveDev;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT8                Offset;
  UINT64               AllOne = 0xFFFFFFFFFFFFFFFF;

  //
  // Remove the ResNode of the requesting device
  //
  CurLink = IoNode->ChildList.ForwardLink;
  while (CurLink != &IoNode->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    if (CurResNode->PciDev == PciDev) {
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  CurLink = Mem32Node->ChildList.ForwardLink;
  while (CurLink != &Mem32Node->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    if (CurResNode->PciDev == PciDev) {
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  CurLink = PMem32Node->ChildList.ForwardLink;
  while (CurLink != &PMem32Node->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    if (CurResNode->PciDev == PciDev) {
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  CurLink = Mem64Node->ChildList.ForwardLink;
  while (CurLink != &Mem64Node->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    if (CurResNode->PciDev == PciDev) {
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  CurLink = PMem64Node->ChildList.ForwardLink;
  while (CurLink != &PMem64Node->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    if (CurResNode->PciDev == PciDev) {
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  //
  // Remove PciDev from the ChildList of its parent bridge
  //
  (*PciDevLink) = (*PciDevLink)->ForwardLink;
  ParentBridge = PciDev->Parent;
  CurLink = ParentBridge->ChildList.ForwardLink;
  while (CurLink && CurLink != &ParentBridge->ChildList) {
    RemoveDev = PCI_IO_DEVICE_FROM_LINK (CurLink);
    if (RemoveDev == PciDev) {
      PciIo = &(RemoveDev->PciIo);
      for (Offset = 0x10; Offset <= 0x24; Offset += sizeof (UINT32)) {
        PciIo->Pci.Write (
	         PciIo,
		 EfiPciIoWidthUint32,
		 Offset,
		 1,
		 &AllOne);
      }

      RemoveEntryList (CurLink);

      if (RemoveDev->ResourcePaddingDescriptors != NULL) {
        FreePool (RemoveDev->ResourcePaddingDescriptors);
      }
      if (RemoveDev->DevicePath != NULL) {
        FreePool (RemoveDev->DevicePath);
      }
      FreePool (RemoveDev);

      break;
    }

    CurLink = CurLink->ForwardLink;
  }

  return;
}

/**
  Check if the overall Mem32 MMIO request of a bridge is valid. (i.e, < 4GB)
  If it's invalid, zero the overall Mem32 request of the bridge,
  and remove the childs Mem32 requests and the requesting device nodes.

  @param  PciDev                   Requesting Pci device instance.
  @param  PciDevLink               Point to the next device node when the function returns.
  @param  IoNode                   Resource info node for IO .
  @param  Mem32Node                Resource info node for 32-bit memory.
  @param  PMem32Node               Resource info node for 32-bit Prefetchable Memory.
  @param  Mem64Node                Resource info node for 64-bit memory.
  @param  PMem64Node               Resource info node for 64-bit Prefetchable Memory.

  @retval EFI_SUCCESS              The overall Mem32 request is valid.
  @retval EFI_INVALID_PARAMETER    The overall Mem32 request is invalid.

**/
EFI_STATUS
CheckResRequestOnBridge (
  IN OUT PCI_IO_DEVICE     *PciDev,
  IN OUT PCI_RESOURCE_NODE *IoNode,
  IN OUT PCI_RESOURCE_NODE *Mem32Node,
  IN OUT PCI_RESOURCE_NODE *PMem32Node,
  IN OUT PCI_RESOURCE_NODE *Mem64Node,
  IN OUT PCI_RESOURCE_NODE *PMem64Node
  )
{
  LIST_ENTRY           *CurLink;
  LIST_ENTRY           *RemoveLink;
  PCI_RESOURCE_NODE    *CurResNode;
  PCI_IO_DEVICE        *RemoveDev;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT8                Offset;
  UINT64               AllOne = 0xFFFFFFFFFFFFFFFF;
  UINT64               Mem32Aperture;
  UINTN                Temp;
  BOOLEAN              Remove;
  EFI_STATUS           Status = EFI_SUCCESS;

  //
  // check if there's Mem32 request > 4GB under the bridge.
  // if yes, remove all Mem32 requests under the bridge,
  // and remove the requesting device nodes.
  //

  Mem32Aperture = 0;

  if (IsListEmpty (&Mem32Node->ChildList)) {
    return Status;
  }

  //
  // only check Mem32Node here, the other nodes are reserved for future use.
  //
  CurLink = Mem32Node->ChildList.ForwardLink;
  while (CurLink != &Mem32Node->ChildList) {
    CurResNode = RESOURCE_NODE_FROM_LINK (CurLink);
    Mem32Aperture += CurResNode->Length;
    if (Mem32Aperture > 0xFFFFFFFF) {
      //
      // the overall Mem32 request under the bridge is invalid
      //
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    CurLink = CurLink->ForwardLink;
  }

  if (!EFI_ERROR (Status)) {
    //
    // the overall Mem32 request under the bridge is valid,
    // simply return.
    //
    return Status;
  }

  //
  // remove all childs of Mem32Node,
  // and zeros the overall Mem32 request of the bridge.
  //
  CurLink = Mem32Node->ChildList.ForwardLink;
  while (CurLink != &Mem32Node->ChildList) {
    RemoveLink = CurLink;
    CurLink = CurLink->ForwardLink;
    RemoveEntryList (RemoveLink);
  }
  Mem32Node->Length = 0;              // zeros the Mem32 request of the bridge.

  //
  // remove all childs with Mem32 requests under the bridge.
  //
  Remove = FALSE;
  CurLink = PciDev->ChildList.ForwardLink;
  while (CurLink && CurLink != &PciDev->ChildList) {
    RemoveDev = PCI_IO_DEVICE_FROM_LINK (CurLink);
    for (Temp = 0; Temp < PCI_MAX_BAR; Temp++) {
      if ((RemoveDev->PciBar[Temp].BarType == PciBarTypeMem32) &&
          (RemoveDev->PciBar[Temp].Length != 0)) {
	//
        // remove this device node
	//
	Remove = TRUE;
	break;
      }
    }

    if (Remove) {
      PciIo = &(RemoveDev->PciIo);
      for (Offset = 0x10; Offset <= 0x24; Offset += sizeof (UINT32)) {
        PciIo->Pci.Write (
                 PciIo,
        	 EfiPciIoWidthUint32,
        	 Offset,
        	 1,
        	 &AllOne);
      }
      RemoveLink = CurLink;
      CurLink = CurLink->ForwardLink;
      RemoveEntryList (RemoveLink);
      Remove = FALSE;
      continue;
    }
    CurLink = CurLink->ForwardLink;
  }

  return Status;
}

