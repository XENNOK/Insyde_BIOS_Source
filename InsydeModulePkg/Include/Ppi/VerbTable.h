/** @file
  A Ppi to provide a interface to program codec with VerbTable.

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

#ifndef _PEI_VERB_TABLE_PPI_H_
#define _PEI_VERB_TABLE_PPI_H_

#define PEI_VERB_TABLE_PPI_GUID  \
  {0xb9fe9515, 0xa5c9, 0x4976, 0x97, 0xda, 0x4d, 0x4d, 0x5e, 0x92, 0xaf, 0x33}

typedef struct _PEI_VERB_TABLE_PPI PEI_VERB_TABLE_PPI;

#pragma pack(1)

typedef struct {
  UINT32  VendorDeviceId;
  UINT32  SubSystemAssemblyId;
  UINT8   RevisionId;
  UINT8   FrontPanelSupport;
  UINT16  NumberOfRearJacks;
  UINT16  NumberOfFrontJacks;
} COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER;

typedef struct {
  COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  *VerbTableHeader;
  UINT32                                   *VerbTableData;
} COMMON_CHIPSET_AZALIA_VERB_TABLE;

#pragma pack()

typedef
EFI_STATUS
(EFIAPI *INSTALL_VERB_TABLE_FUNCTION) (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HDABar,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *OemVerbTableAddress
  );

struct _PEI_VERB_TABLE_PPI {
  INSTALL_VERB_TABLE_FUNCTION              InstallVerbTable;
};

extern EFI_GUID gPeiInstallVerbTablePpiGuid;

#endif
