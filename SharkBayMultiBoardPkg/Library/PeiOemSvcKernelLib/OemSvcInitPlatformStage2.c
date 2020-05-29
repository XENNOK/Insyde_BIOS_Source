/** @file
  CPU/Chipset/Platform Initial depends on project characteristic.

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

#include <Library/IoLib.h>
#include <SetupConfig.h>
#include <Library/PeiOemSvcKernelLib.h>

//
// data type definitions
//
#define KBC_TIME_OUT                      0x10000
#define SMC_DATA                          0x62
#define SMC_CMD_STATE                     0x66
#define EC_ACPI_MODE_EN_CMD               0xAA
#define EC_ACPI_MODE_DIS_CMD              0xAB
#define SHUTDOWN                          0x22
#define EN_DTEMP                          0x5E
#define DIS_DTEMP                         0x5F
#define GET_DTEMP                         0x50
#define QUERY_SYS_STATUS                  0x06
#define B_SYS_STATUS_AC                   0x10 // AC power (1 = AC powered)
#define EC_READ_VERSION_CMD               0x90
#define EC_READ_BORAD_ID_CMD              0x0D
#define EMERALD_LAKE_2_CRB_FAB1           0x81
#define EMERALD_LAKE_2_CRB_FAB2           0xA1
#define ELK_HILL_2_CRB_FAB1               0x82
#define RAINBOW_HILL_2_CRB_FAB1           0x83
#define RTACOMA_FALLS_2_CRB_FAB1          0x84
#define SET_DSW_MODE                      0x29
#define DSW_DISABlE                       0x0
#define DSW_IN_S5                         0x1
#define DSW_IN_S4_S5                      0x3
#define DSW_IN_S3_S4_S5                   0x5
#define B_EC_GET_CRB_BOARD_ID_PLTID                          ( BIT7 )
#define N_EC_GET_CRB_BOARD_ID_PLTID                          7
#define V_EC_GET_CRB_BOARD_ID_PLTID                          1
#define B_EC_GET_CRB_BOARD_ID_FAB_ID                         ( BIT6 | BIT5 )
#define N_EC_GET_CRB_BOARD_ID_FAB_ID                         5
#define V_EC_GET_CRB_BOARD_ID_FAB_ID_FAB1                    0x00
#define V_EC_GET_CRB_BOARD_ID_FAB_ID_FAB2                    0x01
#define B_EC_GET_CRB_BOARD_ID_BOARD_ID                       ( BIT4 | BIT3 | BIT2 | BIT1 | BIT0 )
#define N_EC_GET_CRB_BOARD_ID_BOARD_ID                       0
#define V_EC_GET_CRB_BOARD_ID_BOARD_ID_EMERALD_LAKE_2        0x01
#define V_EC_GET_CRB_BOARD_ID_BOARD_ID_ELK_HILL_2            0x02
#define V_EC_GET_CRB_BOARD_ID_BOARD_ID_RAINBOW_HILL_2        0x03
#define V_EC_GET_CRB_BOARD_ID_BOARD_ID_TACOMA_FALLS_2        0x04

//
// helper function prototypes
//
EFI_STATUS
EcTurboControlMode (
  IN      BOOLEAN        EnableEcTurboCtrlMode,
  IN      UINT8          EcBrickCap,
  IN      UINT8          EcPollingPeriod,
  IN      UINT8          EcGuardBandVal,
  IN      UINT8          EcAlgorithmSel
  );

/**
  Write Io port to control Ec.

  @param  EnableEcTurboControlMode  To enable Ec Turbo Mode or not.
  @param  EcBrickCap                Value written to data port.
  @param  EcPollingPeriod           Value written to data port.
  @param  EcGuardBandVal            Value written to data port.
  @param  EcAlgorithmSel            Value written to data port.


  @retval EFI_SUCCESS               Always returns success.
**/
EFI_STATUS
EcTurboControlMode (
  IN      BOOLEAN        EnableEcTurboControlMode,
  IN      UINT8          EcBrickCap,
  IN      UINT8          EcPollingPeriod,
  IN      UINT8          EcGuardBandVal,
  IN      UINT8          EcAlgorithmSel
  )
{
  EFI_STATUS        Status;

  Status = EFI_SUCCESS;

  if ( EnableEcTurboControlMode ) {
    IoWrite8 ( SMC_CMD_STATE, 0x63 );
    IoWrite8 ( SMC_DATA, EcBrickCap );
    IoWrite8 ( SMC_DATA, EcPollingPeriod );
    IoWrite8 ( SMC_DATA, EcGuardBandVal );
    IoWrite8 ( SMC_DATA, EcAlgorithmSel );
  } else {
    IoWrite8 ( SMC_CMD_STATE, 0x64 );
  }

  return EFI_SUCCESS;
}

/**
  CPU/Chipset/Platform Initial depends on project characteristic.

  @param[in]  *Buffer               A pointer to SYSTEM_CONFIGURATION.
  @param[in]  SetupVariableExist    Setup variable be found in variable storage or not.

  @retval     EFI_UNSUPPORTED       Returns unsupported by default.
  @retval     EFI_SUCCESS           The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval     Others                Depends on customization.
**/
EFI_STATUS
OemSvcInitPlatformStage2 (
  IN  VOID                                 *Buffer,
  IN  BOOLEAN                              SetupVariableExist
  )
{
  SYSTEM_CONFIGURATION                  *SystemConfiguration;

  SystemConfiguration = (SYSTEM_CONFIGURATION *)Buffer;

  EcTurboControlMode (
    SystemConfiguration->EcTurboCtrlMode,
    SystemConfiguration->EcBrickCap,
    SystemConfiguration->EcPollingPeriod,
    SystemConfiguration->EcGuardBandVal,
    SystemConfiguration->EcAlgorithmSel
    );

  return EFI_SUCCESS;
}

