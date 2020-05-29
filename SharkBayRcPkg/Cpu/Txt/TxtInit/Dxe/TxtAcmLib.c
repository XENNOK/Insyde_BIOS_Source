//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "TxtAcmLib.h"
#include "TxtMisc.h"

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

STATIC
ACM_INFO_TABLE *
GetAcmInfoTable (
  IN ACM_HDR      *Hdr
  )
{
  UINT32          UserAreaOff;

  /* overflow? */
  if (PlusOverflowU32 (Hdr->header_len, Hdr->scratch_size)) {
    DEBUG ((EFI_D_ERROR, "ACM header length plus scratch size overflows\n"));
    return NULL;
  }

  if (MultiplyOverflowU32 ((Hdr->header_len + Hdr->scratch_size), 4)) {
    DEBUG ((EFI_D_ERROR, "ACM header length and scratch size in bytes overflows\n"));
    return NULL;
  }

  /* this fn assumes that the ACM has already passed at least the initial */
  /* IsAcm() checks */

  UserAreaOff = (Hdr->header_len + Hdr->scratch_size) * 4;

  /* overflow? */
  if (PlusOverflowU32 (UserAreaOff, sizeof(ACM_INFO_TABLE))) {
    DEBUG ((EFI_D_ERROR, "UserAreaOff plus ACM_INFO_TABLE size overflows\n"));
    return NULL;
  }

  /* check that table is within module */
  if (UserAreaOff + sizeof(ACM_INFO_TABLE) > Hdr->size*4) {
    DEBUG ((EFI_D_ERROR, "ACM info table size too large: %x\n", UserAreaOff + (UINT32)sizeof(ACM_INFO_TABLE)));
    return NULL;
  }

  /* overflow? */
  if (PlusOverflowU32 ((UINT32)(UINTN) Hdr, UserAreaOff)) {
    DEBUG ((EFI_D_ERROR, "hdr plus UserAreaOff overflows\n"));
    return NULL;
  }

  return (ACM_INFO_TABLE *)((UINTN)Hdr + UserAreaOff);
}

STATIC
BOOLEAN
IsAcm (
  IN VOID         *AcmBase,
  IN UINTN        AcmSize,
  IN UINT8        *Type,
  IN BOOLEAN      Quiet
  )
{
  ACM_HDR         *AcmHdr;
  ACM_INFO_TABLE  *InfoTable;
  STATIC EFI_GUID AcmUuid = ACM_UUID_V3;

  AcmHdr = (ACM_HDR *)AcmBase;

  /* first check size */
  if (AcmSize < sizeof(ACM_HDR)) {
    if (!Quiet)
      DEBUG ((EFI_D_ERROR, "\t ACM size is too small: AcmSize=%x, sizeof(AcmHdr)=%x\n",
        (UINT32)AcmSize, (UINT32)sizeof(AcmHdr)));
    return FALSE;
  }

  /* then check overflow */
  if (MultiplyOverflowU32(AcmHdr->size, 4)) {
    if (!Quiet)
      DEBUG ((EFI_D_ERROR, "\t ACM header size in bytes overflows\n"));
    return FALSE;
  }

  /* then check size equivalency */
  if (AcmSize != AcmHdr->size * 4) {
    if (!Quiet)
      DEBUG ((EFI_D_ERROR, "\t ACM size is too small: AcmSize=%x, AcmHdr->size*4=%x\n",
        (UINT32)AcmSize, AcmHdr->size*4));
    return FALSE;
  }

  /* then check Type and vendor */
  if ((AcmHdr->module_type != ACM_TYPE_CHIPSET) ||
       (AcmHdr->module_vendor != ACM_VENDOR_INTEL)) {
    if (!Quiet)
      DEBUG ((EFI_D_ERROR, "\t ACM Type/vendor mismatch: module_type=%x, module_vendor=%x\n",
        AcmHdr->module_type, AcmHdr->module_vendor));
    return FALSE;
  }

  InfoTable = GetAcmInfoTable (AcmHdr);
  if (InfoTable == NULL)
    return FALSE;

  /* check if ACM UUID is present */
  if (!CompareGuid (&(InfoTable->uuid), &AcmUuid)) {
    if (!Quiet) {
      DEBUG ((EFI_D_ERROR, "\t unknown UUID: %g\n", &InfoTable->uuid));
    }
    return FALSE;
  }

  if (Type != NULL)
    *Type = InfoTable->chipset_acm_type;

  if (InfoTable->version < 3) {
    if (!Quiet)
      DEBUG ((EFI_D_ERROR, "\t ACM InfoTable version unsupported (%u)\n", (UINT32)InfoTable->version));
    return FALSE;
  }
  /* there is forward compatibility, so this is just a warning */
  else if (InfoTable->version > 3) {
    if (!Quiet)
      DEBUG ((EFI_D_INFO, "\t ACM InfoTable version mismatch (%u)\n", (UINT32)InfoTable->version));
  }

  return TRUE;
}

BOOLEAN
EFIAPI
IsSinitAcm (
  IN VOID         *AcmBase,
  IN UINTN        AcmSize,
  IN BOOLEAN      Quiet
  )
{
  UINT8           Type;

  if (!IsAcm(AcmBase, AcmSize, &Type, Quiet))
    return FALSE;

  if (Type != ACM_CHIPSET_TYPE_SINIT) {
    DEBUG ((EFI_D_ERROR, "ACM is not an SINIT ACM (%x)\n", Type));
    return FALSE;
  }

  return TRUE;
}

UINT16
EFIAPI
GetSinitAcmChipsetId (
  IN VOID         *AcmBase,
  IN UINTN        AcmSize
  )
{
  if (!IsSinitAcm (AcmBase, AcmSize, FALSE)) {
    return (UINT16) 0xffff;
  }

  return (((ACM_HDR *)(UINTN) AcmBase)->chipset_id);
}
