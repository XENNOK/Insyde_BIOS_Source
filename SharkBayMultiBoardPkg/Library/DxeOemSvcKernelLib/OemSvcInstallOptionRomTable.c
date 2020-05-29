/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

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

#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <SaRegs.h>
#include <PchRegs.h>
#include <Library/DxeOemSvcKernelLib.h>

//
// data type definitions
//

//[-start-120208-IB05300280-remove]//
////
//// BUGBUG : The SA Device IDs should be defined in SaRegs.h
////
//#define GT1_MOBILE_DID    0x106
//#define GT2_MOBILE_DID    0x116
//#define GT2P_MOBILE_DID   0x126
//
//#define GT1_DESKTOP_DID   0x102
//#define GT2_DESKTOP_DID   0x112
//#define GT2P_DESKTOP_DID  0x122
//
//#define GT1_MOBILE_DID_IVB   0x156
//#define GT2_MOBILE_DID_IVB   0x166
//
//#define GT1_DESKTOP_DID_IVB   0x152
//#define GT2_DESKTOP_DID_IVB   0x162
//
//[-end-120208-IB05300280-remove]//

#define ON_BOARD_BMC_VID    0x102B
#define ON_BOARD_BMC_DID    0x0522

//[-start-130905-IB08620307-add]//
#define V_I210_LAN_DEVICE_ID      0x1533
//[-end-130905-IB08620307-add]//

//
// Global variables for PCI Option Roms
//
#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

#define ONBOARD_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x8dfae5d4, 0xb50e, 0x4c10, 0x96, 0xe6, 0xf2, 0xc2, 0x66, 0xca, 0xcb, 0xb6 \
  }

#define ONBOARD_VIDEO_OPTION_ROM_FILE_GUID_2 \
  { \
    0x5264d44a, 0x7c8c, 0x4289, 0xbb, 0x5d, 0xd1, 0xb5, 0xce, 0x42, 0x57, 0xb2 \
  }

#define IDE_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x501737ab, 0x9d1a, 0x4856, 0x86, 0xd3, 0x7f, 0x12, 0x87, 0xfa, 0x5a, 0x55 \
  }

//[-start-130905-IB08620307-add]//
#define IDE_RAID_SV_OPTION_ROM_FILE_GUID \
  { \
    0x5d156eb2, 0xda4e, 0x4c16, 0xb3, 0xe7, 0xb4, 0x10, 0x53, 0x29, 0x85, 0xb4 \
  }
//[-end-130905-IB08620307-add]//

#define IDE_AHCI_OPTION_ROM_FILE_GUID \
  { \
    0xB017C09D, 0xEDC1, 0x4940, 0xB1, 0x3E, 0x57, 0xE9, 0x56, 0x60, 0xC9, 0x0F \
  }
                               
#define PXE_UNDI_OPTION_ROM_FILE_GUID \
  { \
    0x4c316c9a, 0xafd9, 0x4e33, 0xae, 0xab, 0x26, 0xc4, 0xa4, 0xac, 0xc0, 0xf7 \
  }

//[-start-130905-IB08620307-add]//
#define PXE_UNDI_I210_OPTION_ROM_FILE_GUID \
  { \
    0x7a1e4d9f, 0x6230, 0x8514, 0x85, 0x14, 0x21, 0x5e, 0xfa, 0xb4, 0xa5, 0xb1 \
  }

#define PXE_UNDI_I218_OPTION_ROM_FILE_GUID \
  { \
    0x6cc3ba49, 0xc7b7, 0x4a19, 0xb9, 0x1a, 0xd2, 0xdb, 0x03, 0xf3, 0x1c, 0x4a \
  }
//[-end-130905-IB08620307-add]//

//
// Global variables for Non-PCI Option Roms
//
#define SYSTEM_ROM_FILE_GUID \
  { \
    0x1547B4F3, 0x3E8A, 0x4FEF, 0x81, 0xC8, 0x32, 0x8E, 0xD6, 0x47, 0xAB, 0x1A \
  }

#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

#define PXE_BASE_OPTION_ROM_FILE_GUID \
  { \
    0x6f2bc426, 0x8d83, 0x4f17, 0xbe, 0x2c, 0x6b, 0x1f, 0xcb, 0xd1, 0x4c, 0x80 \
  }

#define BIS_OPTION_ROM_FILE_GUID \
  { \
    0xAE21640A, 0x21D2, 0x4ec4, 0xA9, 0x96, 0x86, 0xC2, 0x6C, 0x9E, 0xD0, 0x91 \
  }

#define BIS_LOADER_FILE_GUID \
  { \
    0xf14aa356, 0x9774, 0x481c, 0xb5, 0xf1, 0x79, 0xc1, 0xb5, 0xea, 0xe8, 0xa1 \
  }

#define OEM_I15VARS_FILE_GUID \
  { \
    0x340AD445, 0x06EF, 0x43f1, 0x89, 0x97, 0xFC, 0xD6, 0x7F, 0xC2, 0x7E, 0xEF \
  }

#define OEM_INT15CB_FILE_GUID \
  { \
    0x340AD445, 0x06EF, 0x43f1, 0x89, 0x97, 0xFC, 0xD6, 0x7F, 0xC2, 0x7E, 0xF0 \
  }

#define TPMMP_GUID \
  { \
    0x4B8D2F76, 0xD9E7, 0x46dc, 0xB5, 0xED, 0xEF, 0xEF, 0x94, 0x76, 0xAF, 0x4A \
  }

//
// module variables
//

//
// Pci Option Rom Table
//
EFI_GUID mPxeFileGuid = PXE_UNDI_OPTION_ROM_FILE_GUID;

//[-start-130528-IB10920027-modify]//
PCI_OPTION_ROM_TABLE      PciOptionRomTable[] = {
//[-start-120404-IB05300309-modify]//
//[-start-120208-IB05300280-modify]//
  //
  // Video Option ROM for HSW IGD
  //
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2P_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2P_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_HSS_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT_HSS_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2P_HSS_ID
  },
//[-start-120529-IB06460401-add]//
//[-start-120716-IB06460419-add]//
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT1_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT3_HSM_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT1_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT3_HSD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT1_HSS_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2_HSS_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT3_HSS_ID
  },
//[-end-120716-IB06460419-add]//
//[-end-130528-IB10920027-modify]//
//[-start-120627-IB05330350-modify]//
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT1_HSD_CRW
  },
//[-end-120627-IB05330350-modify]//
//[-end-120529-IB06460401-add]//
//[-start-120627-IB05330350-add]//
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2_HSD_CRW
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT3_HSD_CRW
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT1_HUD_ID
  },
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT2_HUD_ID
  },

  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID,
    V_SA_IGD_VID,
    V_SA_PCI_DEV_2_GT3_HUD_ID
  },
//[-end-120627-IB05330350-add]//
//[-end-120208-IB05300280-modify]//
//[-end-120404-IB05300309-modify]//
  //
  // ONBOARD_VIDEO_OPTION_ROM_FILE_GUID_2 for Pilot3 VBIOS
  //
  {
    ONBOARD_VIDEO_OPTION_ROM_FILE_GUID_2,
    ON_BOARD_BMC_VID,
    ON_BOARD_BMC_DID
  },
//[-start-120628-IB06460410-modify]//
//[-start-130905-IB08620307-modify]//
  //
  // RAID Option Rom
  //
  {
    IDE_RAID_OPTION_ROM_FILE_GUID,
    V_PCH_SATA_VENDOR_ID,
    V_PCH_LPTH_SATA_DEVICE_ID_D_RAID_ALTDIS
  },
  {
    IDE_RAID_OPTION_ROM_FILE_GUID,
    V_PCH_SATA_VENDOR_ID,
    V_PCH_LPTH_SATA_DEVICE_ID_M_RAID_ALTDIS
  },
  {
    IDE_RAID_SV_OPTION_ROM_FILE_GUID,
    V_PCH_SATA_VENDOR_ID,
    V_PCH_LPTH_SATA_DEVICE_ID_D_RAID_SERVER
  },
  //
  // AHCI Option Rom
  //
  {
    IDE_AHCI_OPTION_ROM_FILE_GUID,
    V_PCH_SATA_VENDOR_ID,
    V_PCH_LPTH_SATA_DEVICE_ID_D_AHCI
  },
  {
    IDE_AHCI_OPTION_ROM_FILE_GUID,
    V_PCH_SATA_VENDOR_ID,
    V_PCH_LPTH_SATA_DEVICE_ID_M_AHCI
  },
  //
  // PXE UNDI Option Rom
  //
  {
    PXE_UNDI_OPTION_ROM_FILE_GUID,
    V_PCH_LAN_VENDOR_ID,
    V_PCH_LPTH_LAN_DEVICE_ID
  }, // Ibex Peak Internal LAN
  {
    PXE_UNDI_I210_OPTION_ROM_FILE_GUID,
    V_PCH_LAN_VENDOR_ID,
    V_I210_LAN_DEVICE_ID
  }, 
  {
    PXE_UNDI_I218_OPTION_ROM_FILE_GUID,
    V_PCH_LAN_VENDOR_ID,
    V_PCH_LPTLP_LAN_DEVICE_ID
  },  
//[-end-130905-IB08620307-modify]//
//[-end-120628-IB06460410-modify]//
  {
    NULL_ROM_FILE_GUID,
    0xffff,
    0xffff
  }
};

//
// Non Pci Option Rom Table
//

//
// System Rom table
//
SYSTEM_ROM_TABLE    SystemRomTable[] = {
  {
    //
    // CSM16 binary
    //
    SYSTEM_ROM_FILE_GUID,
    TRUE,
    SYSTEM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};


// 
// Oem Int table 
//
SYSTEM_ROM_TABLE    SystemOemIntTable[] = {
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};

    
//
//  Service ROM Table
//
SERVICE_ROM_TABLE  ServiceRomTable[] = {
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    FALSE,
    MAX_NUM
  }
};

//
// TPM Rom table
//
SYSTEM_ROM_TABLE    TpmTable[] = {
  {
    //
    // TPMMP binary
    //
    TPMMP_GUID,
    TRUE,
    TPM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};

//
//  AMT ROM Table
//
SERVICE_ROM_TABLE  AMTRomTable[] = {
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    FALSE,
    MAX_NUM
  }
};

/**
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table. 
  The detail refers to the document "OptionRomTable Restructure User Guide".

  @param[in]   *RomType              The type of option rom. This parameter decides which kind of option ROM table will be access.
  @param[out]  **RomTable            A pointer to the option ROM table.
  
  @retval      EFI_SUCCESS           Get Option ROM Table info failed.
  @retval      EFI_MEDIA_CHANGED     Get Option ROM Table info success.
  @retval      Others                Depends on customization.
**/
EFI_STATUS 
OemSvcInstallOptionRomTable (
  IN  UINT8                                 RomType,
  OUT VOID                                  **mOptionRomTable
  )
{
  VOID                                  *OptionRomTable;
  UINTN                                 OptionRomTableSize;

  switch (RomType) {
  case PCI_OPROM:
    OptionRomTable      = (void *)PciOptionRomTable;
    OptionRomTableSize  = sizeof(PciOptionRomTable);
    break;    
    
  case SYSTEM_ROM:
    OptionRomTable      = (void *)SystemRomTable;
    OptionRomTableSize  = sizeof(SystemRomTable);
    break;

  case SYSTEM_OEM_INT_ROM:
    OptionRomTable      = (void *)SystemOemIntTable;
    OptionRomTableSize  = sizeof(SystemOemIntTable);
    break;    
    
  case SERVICE_ROM:
    OptionRomTable      = (void *)ServiceRomTable;
    OptionRomTableSize  = sizeof(ServiceRomTable);
    break;    

  case TPM_ROM:
    OptionRomTable      = (void *)TpmTable;
    OptionRomTableSize  = sizeof(TpmTable);
    break; 
    
  case AMT_ROM:
    OptionRomTable      = (void *)AMTRomTable;
    OptionRomTableSize  = sizeof(AMTRomTable);
    break; 
    
  default:
    return EFI_SUCCESS;
  }

  (*mOptionRomTable)   = AllocateZeroPool (OptionRomTableSize);
  CopyMem ((*mOptionRomTable), OptionRomTable, OptionRomTableSize);

  return EFI_MEDIA_CHANGED;
}

