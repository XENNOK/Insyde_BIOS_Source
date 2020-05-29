/** @file

  This file includes all the DDR3 specific characteristic definitions.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MRC_DDR3_H_
#define _MRC_DDR3_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcDdr3Registers.h>
#include <MrcIoControl.h>
#include <MrcOem.h>
#include <MrcPowerModes.h>
#include <MrcRefreshConfiguration.h>

#pragma pack (push, 1)
///
/// in write leveling mode Rtt_Nom = Rtt_Wr
///
typedef struct {
  U8  RttWr;  ///< Wa - Write ODT on active rank
  U8  RttNom; ///< Wp - ODT on one of the ranks on passive DIMM during Write operation
} TOdtValue;

typedef enum {
  oi1DPC1R  = 0,
  oi1DPC2R,
  oi2DPC1R1R,
  oi2DPC1R2R,
  oi2DPC2R1R,
  oi2DPC2R2R,
  oiNotValid
} TOdtIndex;

typedef enum {
  ODIC_RZQ_6,
  ODIC_RZQ_7,
  ODIC_RSVD_0,
  ODIC_RSVD_1
} TOutputDriverImpedanceControl;

///
/// ZQ Calibration types
///
typedef enum {
  MRC_ZQ_INIT,  ///< DDR3: ZQCL with tZQinit, LPDDR3: ZQ Init  with tZQinit
  MRC_ZQ_LONG,  ///< DDR3: ZQCL with tZQoper, LPDDR3: ZQ Long  with tZQCL
  MRC_ZQ_SHORT, ///< DDR3: ZQCS with tZQCS,   LPDDR3: ZQ Short with tZQCS
  MRC_ZQ_RESET  ///< DDR3: not used,          LPDDR3: ZQ Reset with tZQreset
} MrcZqType;

#ifndef tZQinit
#define tZQinit                     (512)   ///< define the tZQinit as define in jedec spec
#endif

#ifndef tWLMRD
#define tWLMRD                      (40)    ///< First DQS/DQS# rising edge after write leveling mode is programmed.
#endif

#ifndef tWLOE
#define tWLOE                       (40)    ///< Write leveling output error the time is 2ns ~ 2 nCK
#endif

#ifndef tZQCS_TIME
#define tZQCS_TIME                  (64)    ///< jedec timing
#endif

#define MRC_DDR3_SDRAM_TYPE_NUMBER  (0x0B)  ///< use to know the DDR type that data came from Jedec SPD byte 2
#define MRC_UDIMM_TYPE_NUMBER       (0x02)  ///< use to know if the DIMM type is UDIMM define in Jedec SPD byte 3
#define MRC_SOIMM_TYPE_NUMBER       (0x03)  ///< use to know if the DIMM type is SO-DIMM define in Jedec SPD byte 3
#define MRC_SDRAM_DEVICE_WIDTH_8    (0x1)   ///< use to know if the DDRAM is 8 bits width
#define MRC_SDRAM_DEVICE_WIDTH_16   (0x2)   ///< use to know if the DDRAM is 16 bits width
#define MRC_PRIMARY_BUS_WIDTH_64    (0x3)   ///< use to know if the DIMM primary bus width is not 64
#define MRC_CL_MAX_OFFSET           (0xF)   ///< in the spd data include cl from bit 0 to bit 15 each bit represent different support CL
#define MRC_CL_IN_NANO_SEC          (20)    ///< define the nax CL value in nano second

/**
@brief
  this function reverses MA and BA bits for Rank1

  @param[in] BA              - MRS command to be sent
  @param[in] MA              - Value to be sent

  @retval Proper MA and BA BITS.
**/
extern
U32
MrcMirror (
  IN U8              BA,
  IN U16             MA
  );

/**
@brief
  this function writes to CADB

  @param[in] MrcData         - include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to.
  @param[in] CMD             - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param[in] BA              - MRS command to be sent
  @param[in] MA              - Value to be sent
  @param[in] Delay           - Delay in Dclocks

  @retval MrcStatus
**/
extern
MrcStatus
MrcWriteCADBCmd (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask,
  IN const U8             CMD,
  IN const U8             BA,
  IN const U16 *const     MA,
  IN const U8             Delay
  );

/**
@brief
  This function sends the proper MRS command for specific ranks as indicated by RankMask

  @param[in] MrcData         - include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to
  @param[in] MR              - MRS command to be sent
  @param[in] DimmValue       - Dimm Values to be sent

  @retval MrcStatus
**/
extern
MrcStatus
MrcWriteMRSAll (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask,
  IN const U8             MR,
  IN const U16 *const     DimmValue
  );

/**
@brief
  This function sends the proper MRS command for specific ranks as indicated by RankMask

  @param[in] MrcData         - Include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to
  @param[in] MR              - MRS command to be sent
  @param[in] Value           - Value to be sent

  @retval MrcStatus
**/
extern
MrcStatus
MrcWriteMRS (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask,
  IN const U8             MR,
  IN const U16            Value
  );

/**
@brief
  Issue ZQ calibration command on all ranks.
  When done, wait appropriate delay depending on the ZQ type.

  @param[in] MrcData         - include all the MRC data
  @param[in] chBitMask       - Channel bit mask to be sent to.
  @param[in] ZqType          - Type of ZQ Calibration: see MrcZqType enum

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
extern
MrcStatus
MrcIssueZQ (
  IN MrcParameters *const MrcData,
  IN const U8             chBitMask,
  IN const MrcZqType      ZqType
  );

/**
@brief
  This function writes the MR2 register for all the ranks and channels

  @param[in, out] MrcData    - general data
  @param[in]      Pasr       - Partial array self refresh bit A0-A2

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
extern
MrcStatus
MrcSetMR2 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Pasr
  );

/**
@brief
  This function writes the MR3 register for all the ranks and channels

  @param[in] MrcData         - include all the MRC data
  @param[in] MPRLoc          - MPR Location bit A0-A1
  @param[in] Mpr             - MPR bit A2

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
extern
MrcStatus
MrcSetMR3 (
  IN MrcParameters *const MrcData,
  IN const U8             MPRLoc,
  IN const U8             Mpr
  )
;

/**
@brief
  This function writes the MR1 register for all the ranks and channels

  @param[in, out] MrcData         - include all the MRC data
  @param[in]      DLLEnable       - DLL enable bit A0
  @param[in]      Odic            - Output driver impedance control A5, A1
  @param[in]      AdditiveLatency - Additive latency bit A3-A4
  @param[in]      WlEnable        - Write leveling enable bit A7
  @param[in]      Tdqs            - TDQS enable bit A11
  @param[in]      Qoff            - Qoff bit A12

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
extern
MrcStatus
MrcSetMR1 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             DLLEnable,
  IN     const U8             Odic,
  IN     const U8             AdditiveLatency,
  IN     const U8             WlEnable,
  IN     const U8             Tdqs,
  IN     const U8             Qoff
  );

/**
@brief
  This function writes the MR0 register for all the ranks

  @param[in, out] MrcData         - include all the MRC data
  @param[in]      CommandControl  - include the command control params
  @param[in]      BurstLength     - Burst length bit A0-A1
  @param[in]      ReadBurstType   - Read burst type bit A3
  @param[in]      TestMode        - Test mode type bit A7
  @param[in]      DllReset        - DLL reset bit A8

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
extern
MrcStatus
MrcSetMR0 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Bl,
  IN     const U8             Rbt,
  IN     const U8             Tm,
  IN     const U8             Dll
  );

/**
@brief
  This function return tWLO time. this time is Write leveling output delay.

  @param[in] Frequency       - MC frequency.

  @retval tWLO timein nCK.
**/
extern
U32
GetTwloTime (
  IN const MrcFrequency  Frequency
  );

/**
@brief
  This funtion returns the odt table index for the given Dimm/Channel.

  @param[in] MrcData         - Include all the mrc global data.
  @param[in] Channel         - Channel to work on.
  @param[in] Dimm            - Rank to work on.

  @retval OdtValue - iThe pointer to the relevant Odt values.
**/
extern
TOdtValue *
GetOdtTableIndex (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             Dimm
  );

/**
@brief
  This funtion takes the MR1 register value and updates the odt value
 inside the register.

  @param[in] MrcData          - Include all the MRC general data.
  @param[in] OdtValue         - Selected odt index.
  @param[in] Ddr3ModeRegister - Register to update.

  @retval Ddr3ModeRegister  - Updated register
**/
extern
DDR3_MODE_REGISTER_1_STRUCT
UpdateRttNomValue (
  IN MrcParameters *const        MrcData,
  IN const U8                    OdtValue,
  IN DDR3_MODE_REGISTER_1_STRUCT Register
  );

/**
@brief
  This function updates the Rtt value in the MR2 value passed in.

  @param[in] MrcData          - Include all the MRC general data.
  @param[in] OdtValue         - Selected odt index.
  @param[in] Ddr3ModeRegister - Register to update.

  @retval Ddr3ModeRegister - Updated MR2 register
**/
extern
DDR3_MODE_REGISTER_2_STRUCT
UpdateRttWrValue (
  IN MrcParameters *const        MrcData,
  IN const U8                    OdtValue,
  IN DDR3_MODE_REGISTER_2_STRUCT Ddr3ModeRegister
  );

/**
@brief
  this funtion select the ODT table according OEM/USER decision.
  In the MRC have 4 table type Mb,Dt,User1,User2.
  User1,User2 use as internal usage.

  @param[in] MrcData         - Include all the MRC general data.
  @param[in] Dimm            - selected DIMM.
  @param[in] OdtIndex        - selected odt index.

  @retval TOdtValue * - Pointer to the relevant table.
                        The return value is NULL if the table could
                        not be found
**/
extern
TOdtValue *
SelectTable (
  IN MrcParameters *const MrcData,
  IN const U8             Dimm,
  IN const TOdtIndex      OdtIndex
  );

#ifdef ULT_FLAG

/**
@brief
  Issue LPDDR MRW (Mode Register Write) command using MRH (Mode Register Handler).

  @param[in] MrcData - Include all MRC global data.
  @param[in] Channel - the channel to work on
  @param[in] Rank    - the rank to work on
  @param[in] Address - MRW address
  @param[in] Data    - MRW Data
  @param[in] InitMrw - when TRUE, command is stretched (used before CA training is done)
  @param[in] ChipSelect2N - when TRUE, use 2N stretch mode for CS (used before CA training is done)

  @retval mrcSuccess    - MRW was sent successfully
  @retval mrcDeviceBusy - timed out waiting for MRH
**/
extern
MrcStatus
MrcIssueMrw (
  IN MrcParameters *const MrcData,
  IN U32                  Channel,
  IN U32                  Rank,
  IN U32                  Address,
  IN U32                  Data,
  IN BOOL                 InitMrw,
  IN BOOL                 ChipSelect2N
  );

/**
@brief
  Issue LPDDR MRR (Mode Register Read) command using MRH (Mode Register Handler).
  Use DQ mapping array to deswizzle the MR data.

  @param[in]  MrcData - Include all MRC global data.
  @param[in]  Channel - the channel to work on
  @param[in]  Rank    - the rank to work on
  @param[in]  Address - MRR address
  @param[out] Data    - MRR Data array per SDRAM device

  @retval mrcSuccess    - MRR was executed successfully
  @retval mrcDeviceBusy - timed out waiting for MRH
**/
extern
MrcStatus
MrcIssueMrr (
  IN MrcParameters *const MrcData,
  IN U32                  Channel,
  IN U32                  Rank,
  IN U32                  Address,
  OUT U8                  Data[4]
  );

/**
@brief
  Issue LPDDR PRECHARGE ALL command using CADB.

  @param[in]  MrcData   - Include all MRC global data.
  @param[in]  Channel   - The channel to work on
  @param[in]  RankMask  - The rank(s) to work on

  @retval none
**/
void
MrcIssuePrechargeAll (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask
  );

#endif // ULT_FLAG

#pragma pack (pop)
#endif // _MrcDdr3_h_
