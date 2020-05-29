/** @file
  Header file for Registers and Structure definitions

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef RAPID_START_AHCIREG_H_
#define RAPID_START_AHCIREG_H_

#define SECTOR_SIZE                       0x200
#define SECTOR_SHIFT                      9

#define MEM_TO_SECT(mem)                  ((UINT32) RShiftU64 ((mem), SECTOR_SHIFT))

//
// Known issues with 3rd party SSDs : some 3rd party SSD was not compliant with the ATA spec (transfer size of 0 sector issue)
// We have seen it on more than one vendor's drive so far,
// The w/a does not cause performance issue to our code or any special checks
// #define AHCI_MAX_SECTORS                  0x10000
//
#define AHCI_MAX_SECTORS                  0xFF80
#define AHCI_MAX_TRANSFER                 (SECTOR_SIZE * AHCI_MAX_SECTORS)

#define AHCI_NO_DATA                      0
#define AHCI_DIR_HOST2DEV                 1
#define AHCI_DIR_DEV2HOST                 0

#define ATA_CMD_IDENTIFY_DEVICE           0xEC
#define ATA_CMD_READ_DMA_EXT              0x25
#define ATA_CMD_WRITE_DMA_EXT             0x35
#define ATA_CMD_WRITE_DMA_FUA_EXT         0x3D
#define ATA_CMD_STANDBY_IMMEDIATE         0xE0
#define ATA_CMD_DATA_SET_MANAGEMENT       0x06
#define   V_ATA_TRIM_FEATURE                0x01

#define ATA_CMD_SECURITY_UNLOCK           0xF2
#define ATA_CMD_SECURITY_FREEZE_LOCK      0xF5
#define ATA_CMD_SECURITY_SET_PASSWORD     0xF1
#define ATA_CMD_FLUSH_CACHE               0xE7

#define ATA_CMD_SET_FEATURES              0xEF
#define   V_ATA_FEATURE_ENABLE              0x10
#define   V_ATA_FEATURE_DISABLE             0x90
#define ATA_CMD_READ_LOG_EXT              0x2F

#define ATA_ID_DEV_HYBRID_FEATURE_SUPPORT  78
#define B_ATA_ID_DEV_HYBRID_FEATURE_SUPPORT  BIT9
#define ATA_ID_DEV_HYBRID_FEATURE_ENABLE   79
#define B_ATA_ID_DEV_HYBRID_FEATURE_ENABLE   BIT9

#define ATA_ID_DEV_DATA_SET_MGMNT_BLOCKS  105
#define ATA_ID_DEV_DATA_SET_MGMNT_SUPPORT 169
#define B_ATA_ID_DEV_DATA_SET_TRIM        0x01

#define ATA_ID_DEV_SECURITY_STATUS        128
#define B_ATA_ID_DEV_SEC_SUPPORTED        0x01
#define B_ATA_ID_DEV_SEC_ENABLED          0x02
#define B_ATA_ID_DEV_SEC_LOCKED           0x04
#define B_ATA_ID_DEV_SEC_FROZEN           0x08
#define B_ATA_ID_DEV_SEC_COUNT_EXP        0x10

#define ALIGN(v, a)                       ((((v) - 1) | ((a) - 1)) + 1)

#define AHCI_HBA_SIZE                     0x100
#define AHCI_PORT_SIZE                    0x80
#define AHCI_PORT_BASE_X(Ahci, port)      (Ahci->Abar + AHCI_HBA_SIZE + AHCI_PORT_SIZE * port)
#define AHCI_PORT_BASE(Ahci)              AHCI_PORT_BASE_X (Ahci, Ahci->Port)
#define AHCI_MAX_CMD                      16
#define AHCI_CMD_HEADER_SIZE              0x20
#define AHCI_CMD_LIST_BASE(Ahci)          (Ahci->PortBase)
#define AHCI_CMD_LIST_SIZE                (AHCI_MAX_CMD * AHCI_CMD_HEADER_SIZE)

#define AHCI_RXFIS_BASE(Ahci)             ALIGN ((AHCI_CMD_LIST_BASE (Ahci) + AHCI_CMD_LIST_SIZE), 0x100) ///< align 256
#define AHCI_RXFIS_SIZE                   0x100
#define AHCI_CMD_TABLE_BASE(Ahci)         ALIGN ((AHCI_RXFIS_BASE (Ahci) + AHCI_RXFIS_SIZE), 0x80)        ///< align 128
#define AHCI_CMD_TABLE_HEADER_SIZE        0x80
#define AHCI_MAX_PRDT                     8192  ///< We need 8192 PRT entries of 4KB transfer size for single 32MB command
#define AHCI_PRD_SIZE                     0x10
#define AHCI_CMD_TABLE_SIZE               (AHCI_CMD_TABLE_HEADER_SIZE + AHCI_PRD_SIZE * AHCI_MAX_PRDT)

#define AHCI_CMD_HEADER(Ahci, cmd)        (AHCI_CMD_LIST_BASE (Ahci) + (cmd) * AHCI_CMD_HEADER_SIZE)
#define AHCI_CMD_TABLE(Ahci, cmd)         (AHCI_CMD_TABLE_BASE (Ahci) + (cmd) * AHCI_CMD_TABLE_SIZE)  ///< align 128

#define AHCI_ID_BLOCK_SIZE                SECTOR_SIZE
#define AHCI_ID_BLOCK(Ahci)               (AHCI_CMD_TABLE (Ahci, AHCI_MAX_CMD))

#define AHCI_TMP_BLOCK_SIZE               SECTOR_SIZE
#define AHCI_TMP_BLOCK(Ahci)              (AHCI_ID_BLOCK (Ahci) + AHCI_ID_BLOCK_SIZE)

#define AHCI_MEM_MAX(Ahci)                (AHCI_TMP_BLOCK (Ahci) + AHCI_TMP_BLOCK_SIZE)
#define AHCI_MEM_MAX_SIZE                 (AHCI_TMP_BLOCK_SIZE + \
                                           AHCI_ID_BLOCK_SIZE + \
                                           AHCI_MAX_CMD * \
                                           AHCI_CMD_TABLE_SIZE + \
                                           AHCI_RXFIS_SIZE + \
                                           AHCI_CMD_LIST_SIZE \
                                           )

#define AHCI_INIT_WAIT                    10000     ///< us
#define AHCI_INIT_TIMEOUT                 10000000  ///< us
#define AHCI_INIT_RETRY_COUNT             3
#define AHCI_CMD_WAIT                     100       ///< us
#define AHCI_CMD_TIMEOUT                  20000000  ///< us

#define AHCI_CMD_PRDTL(prdtl)             ((prdtl) << 16)
#define AHCI_CMD_CFL(cfl)                 ((cfl) & 0x0F)
#define AHCI_CMD_WRITE                    BIT6
#define AHCI_CMD_PREFETCHABLE             BIT7

#define AHCI_REGION_MAX                   0x400000  ///< 4MB
#define R_PCH_SATA_AHCI_PXCLB             0x000
#define R_PCH_SATA_AHCI_PXCLBU            0x004
#define R_PCH_SATA_AHCI_PXFB              0x008
#define R_PCH_SATA_AHCI_PXFBU             0x00C
#define R_PCH_SATA_AHCI_PXIS              0x010
#define R_PCH_SATA_AHCI_PXIE              0x014
#define R_PCH_SATA_AHCI_PXCMD             0x018
#define R_PCH_SATA_AHCI_PXTFD             0x020
#define B_PCH_SATA_AHCI_PXTFD_STS_ERR     BIT0
#define B_PCH_SATA_AHCI_PXTFD_STS_DRQ     BIT3
#define B_PCH_SATA_AHCI_PXTFD_STS_BSY     BIT7
#define R_PCH_SATA_AHCI_PXSIG             0x024
#define R_PCH_SATA_AHCI_PXSSTS            0x028
#define B_PCH_SATA_AHCI_PXSSTS_DET        0x0000000F
#define R_PCH_SATA_AHCI_PXSCTL            0x02C
#define R_PCH_SATA_AHCI_PXSERR            0x030
#define R_PCH_SATA_AHCI_PXSACT            0x034
#define R_PCH_SATA_AHCI_PXCI              0x038

#define B_PCH_SATA_AHCI_PXIS_FATAL        (B_PCH_SATA_AHCI_PXIS_HBFS | \
                                           B_PCH_SATA_AHCI_PXIS_HBDS | \
                                           B_PCH_SATA_AHCI_PXIS_IFS | \
                                           B_PCH_SATA_AHCI_PXIS_TFES | \
                                           B_PCH_SATA_AHCI_PXIS_PCS \
                                           )

#define AHCI_ERROR(PxBase)                ((MmioRead32 ((PxBase) + R_PCH_SATA_AHCI_PXIS) & B_PCH_SATA_AHCI_PXIS_FATAL) != 0)

#define SATA_FIS_HOST2DEVICE              0x27
#define SATA_FIS_C                        0x8000
#define SATA_FIS_CMD(cmd)                 (((cmd) & 0xFF) << 16)
#define SATA_FIS_DEV_LBA                  0x40000000
#define SATA_FIS_LBA(lba)                 ((UINT32) ((lba) & 0x00FFFFFF))
#define SATA_FIS_LBA_EXP(lba)             ((UINT32) RShiftU64 ((lba), 24) & 0x00FFFFFF)
#define SATA_FIS_SECT_COUNT(count)        ((UINT16) (count))
#define SATA_FIS_FEAT(feat)               ((((UINT16) (feat)) & 0xFF) << 24)
#define SATA_FIS_FEAT_EXP(feat)           ((((UINT16) (feat)) & 0xFF00) << 16)

#endif
