/** @file

  Header file for Me Chipset Lib
  Note: Only for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_CHIPSET_LIB_H_
#define _SPS_ME_CHIPSET_LIB_H_

typedef enum {
  SpsHECI1 = 1,
  SpsHECI2,
  SpsIDER,
  SpsSOL,
  SpsUSBR1,
  SpsUSBR2,
  SpsFDSWL,
} SPS_ME_DEVICE;

typedef enum {
  SpsDeviceDisabled = 0,
  SpsDeviceEnabled,
} SPS_ME_DEVICE_FUNC_CTRL;

/**
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control

  @retval None
**/
VOID
SpsMeDeviceControl (
  IN SPS_ME_DEVICE                     WhichDevice,
  IN SPS_ME_DEVICE_FUNC_CTRL           DeviceFuncCtrl
  );

/**
  Get HECI-1 BAR

  @param[out] Heci1Bar            HECI 1 BAR.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetHeci1Bar (
  OUT UINT64                           *Heci1Bar
  );

/**
  Get HECI-2 BAR

  @param[out] Heci2Bar            HECI 2 BAR.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetHeci2Bar (
  OUT UINT64                           *Heci2Bar
  );

/**
  Get HECI 1 Firmware Status # 1

  @param[out] MeFs1               HECI 1 Firmware Status # 1

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetMeFs1 (
  OUT UINT32                           *MeFs1
  );

/**
  Get HECI 1 Firmware Status #2 

  @param[out] MeFs2               HECI 1 Firmware Status # 2 

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetMeFs2 (
  OUT UINT32                           *MeFs2
  );

///
/// SPS Node Manager
///
/**
  Get HECI 2 Firmware Status # 1 

  @param[out] Nmfs                HECI 2 Firmware Status # 1 

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetNmFs (
  OUT UINT32                           *Nmfs
  );


/**
  Initialize PCH power management by clearing the PCH wake reason.

  @param None

  @retval EFI_SUCCESS             Heci initialization completed successfully.
**/
VOID
SpsMeChipsetClearWakeStatus (
  VOID
  );

/**
  Save device setting to S3 script table.

  @param None.

**/
VOID
SpsDeviceStatusSave (
  VOID
  );

/**
  Initial HECI-2.

  @param None.

**/
VOID
SpsMeChipsetHeci2Init (
  VOID
  );

/**
  Lock CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Lock (
  VOID
  );

/**
  Disable CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Disable (
  VOID
  );

/**
  Do global reset by CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Reset (
  VOID
  );

///
/// SPS ME Firmware upgrade library function
///
/**
  Enable HECI#1 Device.

  @param None.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetEnableHeciDevice (
  VOID
  );

/**
  Get ROM linear base address of system from descriptor region

  @param[out] RomBaseAddress      The pointer of ROM linear base address

  @retval EFI_SUCCESS             Get Rom Base Address Successfully.
**/
EFI_STATUS
SpsMeChipsetGetRomBaseAddress (
  OUT UINTN                            *RomBaseAddress
  );

/**
  Get descriptor region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetDescRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get Me region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetMeRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get GBE region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetGBERegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get Platform Data Regione(PDR) region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetPDRRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get Device Expansion Region(DER) region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetDERRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Check Region write access for Region 0 Flash Descriptor.

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckDescBRWA (
  VOID
  );

/**
  Check Region write access for Region 1 BIOS.

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckBiosBRWA (
  VOID
  );

/**
  Check Region write access for Region 2 Management Engine Firmware(ME).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckMeBRWA (
  VOID
  );

/**
  Check Region write access for Region 3 Location for Integrated LAN firmware and MAC address(GBE).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckGbeBRWA (
  VOID
  );
/**
  Check Region write access for Region 4 Platform Data Region(PDR).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckPDRBRWA (
  VOID
  );

/**
  Check Region write access for Region 5 Device Expansion Region(DER) (SPS FW Only).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckDERBRWA (
  VOID
  );

/**
  Check Region read access for Region 0 Flash Descriptor.

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckDescBRRA (
  VOID
  );

/**
  Check Region read access for Region 1 Region 1 BIOS.

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckBiosBRRA (
  VOID
  );

/**
  Check Region read access for Region 2 Management Engine Firmware(ME).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckMeBRRA (
  VOID
  );

/**
  Check Region read access for Region 3 Location for Integrated LAN firmware and MAC address(GBE).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckGbeBRRA (
  VOID
  );

/**
  Check Region read access for Region 4 Platform Data Region(PDR).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckPDRBRRA (
  VOID
  );

/**
  Check Region read access for Region 5 Device Expansion Region(DER) (SPS FW Only).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckDERBRRA (
  VOID
  );

/**
  Protect descriptor region by PRx.

  @param None.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetProtectDescRegion (
  VOID
  );

/**
  Protect ME region by PRx.

  @param[out] FactoryDefaultBase  ME Region Base.
  @param[out] FactoryDefaultBase  ME Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetProtectMeRegion (
  IN UINT32                            FactoryDefaultBase,
  IN UINT32                            FactoryDefaultLimit
  );

/**
  Early BIOS message to SPS ME firmware.

  @param[in] Value                Message will send to SPS ME firmware.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeEarlyBiosMessage (
  IN UINT32                            Value
  );

/**
  Early Get mPhy table version from SPS ME firmware.

  @param[out] Version             mPhy table version.
  @param[out] CRC16               mPhy table CRC16.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetmPhySurvivabilityTableVesrion (
  OUT UINT16                           *Version,
  OUT UINT16                           *CRC16
  );

#define SpsHeciEnable()                SpsMeDeviceControl (SpsHECI1, SpsDeviceEnabled);
#define SpsHeci2Enable()               SpsMeDeviceControl (SpsHECI2, SpsDeviceEnabled);
#define SpsIderEnable()                SpsMeDeviceControl (SpsIDER,  SpsDeviceEnabled);
#define SpsSolEnable()                 SpsMeDeviceControl (SpsSOL,   SpsDeviceEnabled);
#define SpsUsbr1Enable()               SpsMeDeviceControl (SpsUSBR1, SpsDeviceEnabled);
#define SpsUsbr2Enable()               SpsMeDeviceControl (SpsUSBR2, SpsDeviceEnabled);

#define SpsHeciDisable()               SpsMeDeviceControl (SpsHECI1, SpsDeviceDisabled);
#define SpsHeci2Disable()              SpsMeDeviceControl (SpsHECI2, SpsDeviceDisabled);
#define SpsIderDisable()               SpsMeDeviceControl (SpsIDER,  SpsDeviceDisabled);
#define SpsSolDisable()                SpsMeDeviceControl (SpsSOL,   SpsDeviceDisabled);
#define SpsUsbr1Disable()              SpsMeDeviceControl (SpsUSBR1, SpsDeviceDisabled);
#define SpsUsbr2Disable()              SpsMeDeviceControl (SpsUSBR2, SpsDeviceDisabled);
#define SpsDisableAllMEDevices()       \
                                       SpsHeciDisable ();  \
                                       SpsHeci2Disable (); \
                                       SpsIderDisable ();  \
                                       SpsSolDisable ();

///
/// Function Disable SUS well lockdown
///
#define SpsFunctionDisableWellLockdown()         SpsMeDeviceControl (SpsFDSWL, SpsDeviceEnabled);


#endif
