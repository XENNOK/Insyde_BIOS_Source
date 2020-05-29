/** @file
  This file contains platform related functions.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
  license agreement.

**/

#ifdef SSA_FLAG
#include "EviCallbackPeim.h"
extern
VOID
SsaBiosInitialize (
  IN MrcParameters       *MrcData
  );
#endif // SSA_FLAG
//
// Include files
//
#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>
#include <MrcOem.h>
#include <MrcOemPlatform.h>

#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/HobLib.h>
#include <PchRegsLpc.h>
#include <MrcReset.h>
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
#include <Guid/DDR3LVoltageHob.h>
#endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//

//[-start-120711-IB05330357-add]//
//[-start-130812-IB06720232-modify]//
//#include <OemEcLib.h>
//[-end-130812-IB06720232-modify]//
//[-end-120711-IB05330357-add]//
//[-start-130326-IB06720210-add]//
#include <Library/PeiOemSvcChipsetLib.h>
#include <Guid/OemSpdDataGuid.h>
#include <Guid/SlotStatusGuid.h>
//[-end-130326-IB06720210-add]//

#ifdef MRC_DEBUG_PRINT
extern const char CcdString[];
const char TrainEnString[]  = "TrainingEnables";
const char GdxcString[]     = "Gdxc";
const char BaseTimeString[] = "BaseTime";
const char ThermEnString[]  = "ThermalEnables";
#endif // MRC_DEBUG_PRINT

#ifdef ULT_FLAG

//
// The following section contains board-specific CMD/CTL/CLK and DQ/DQS mapping, needed for LPDDR3
//

//
// DQByteMap[0] - ClkDQByteMap:
//   If clock is per rank, program to [0xFF, 0xFF]
//   If clock is shared by 2 ranks, program to [0xFF, 0] or [0, 0xFF]
//   If clock is shared by 2 ranks but does not go to all bytes,
//           Entry[i] defines which DQ bytes Group i services
// DQByteMap[1] - CmdNDQByteMap: Entry[0] is CmdN/CAA and Entry[1] is CmdN/CAB
// DQByteMap[2] - CmdSDQByteMap: Entry[0] is CmdS/CAA and Entry[1] is CmdS/CAB
// DQByteMap[3] - CkeDQByteMap : Entry[0] is CKE /CAA and Entry[1] is CKE /CAB
//                For DDR, DQByteMap[3:1] = [0xFF, 0]
// DQByteMap[4] - CtlDQByteMap : Always program to [0xFF, 0] since we have 1 CTL / rank
//                               Variable only exists to make the code easier to use
// DQByteMap[5] - CmdVDQByteMap: Always program to [0xFF, 0] since we have 1 CA Vref
//                               Variable only exists to make the code easier to use
//

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for Sawtooth Peak and Harris Beach
//
const U8 DqByteMapRvpCh0[6][2] = {
  { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
  { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
  { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
  { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
  { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
  { 0xFF, 0x00 }  // CA Vref is one for all bytes
};

const U8 DqByteMapRvpCh1[6][2] = {
  { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
  { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
  { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
  { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
  { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
  { 0xFF, 0x00 }  // CA Vref is one for all bytes
};

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for Big Creek
//
const U8 DqByteMapSvCh0[6][2] = {
  { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
  { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
  { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
  { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
  { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
  { 0xFF, 0x00 }  // CA Vref is one for all bytes
};

const U8 DqByteMapSvCh1[6][2] = {
  { 0xE8, 0x17 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
  { 0x00, 0x17 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
  { 0xE8, 0x17 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
  { 0xE8, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
  { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
  { 0xFF, 0x00 }  // CA Vref is one for all bytes
};

//
// DQS byte swizzling between CPU and DRAM - for Sawtooth Peak and Harris Beach
//
const U8 DqsMapCpu2DramRvpCh0[8] = { 2, 0, 1, 3, 6, 4, 7, 5 };
const U8 DqsMapCpu2DramRvpCh1[8] = { 1, 3, 2, 0, 5, 7, 6, 4 };

//
// DQS byte swizzling between CPU and DRAM - for Big Creek
//
const U8 DqsMapCpu2DramSvCh0[8] = { 0, 1, 2, 3, 5, 6, 7, 4 };
const U8 DqsMapCpu2DramSvCh1[8] = { 7, 6, 5, 2, 4, 3, 1, 0 };

//
// DQ bit swizzling between CPU and DRAM - for Sawtooth Peak and Harris Beach
//
const U8 DqMapCpu2DramRvpCh0[8][8] = {
  { 16, 21, 18, 19, 20, 17, 22, 23 },  // Byte 0
  {  3,  6,  1,  5,  2,  7,  0,  4 },  // Byte 1
  {  9,  8, 14, 15, 10, 11, 13, 12 },  // Byte 2
  { 29, 28, 27, 31, 24, 25, 30, 26 },  // Byte 3
  { 53, 49, 50, 51, 48, 52, 54, 55 },  // Byte 4
  { 35, 38, 33, 37, 34, 39, 32, 36 },  // Byte 5
  { 63, 59, 61, 57, 56, 60, 58, 62 },  // Byte 6
  { 44, 45, 46, 42, 40, 41, 43, 47 }   // Byte 7
};
const U8 DqMapCpu2DramRvpCh1[8][8] = {
  { 15, 11,  8,  9, 10, 14, 12, 13 },  // Byte 0
  { 24, 29, 30, 26, 28, 25, 27, 31 },  // Byte 1
  { 16, 20, 22, 23, 17, 21, 19, 18 },  // Byte 2
  {  6,  3,  1,  5,  2,  7,  4,  0 },  // Byte 3
  { 47, 42, 40, 41, 43, 46, 44, 45 },  // Byte 4
  { 57, 56, 62, 58, 61, 60, 59, 63 },  // Byte 5
  { 51, 49, 54, 53, 48, 50, 55, 52 },  // Byte 6
  { 38, 35, 36, 32, 34, 39, 33, 37 }   // Byte 7
};

//
// DQ bit swizzling between CPU and DRAM - for Big Creek
//
const U8 DqMapCpu2DramSvCh0[8][8] = {
  {  1,  0,  2,  6,  5,  4,  3,  7 },  // Byte 0
  { 13,  9, 14, 10, 12,  8, 15, 11 },  // Byte 1
  { 22, 18, 21, 16, 17, 20, 19, 23 },  // Byte 2
  { 29, 28, 26, 27, 30, 31, 24, 25 },  // Byte 3
  { 41, 45, 46, 42, 40, 44, 43, 47 },  // Byte 4
  { 53, 49, 54, 50, 52, 48, 55, 51 },  // Byte 5
  { 63, 62, 61, 60, 59, 58, 57, 56 },  // Byte 6
  { 34, 35, 37, 36, 38, 39, 33, 32 }   // Byte 7
};
const U8 DqMapCpu2DramSvCh1[8][8] = {
  { 58, 62, 57, 61, 59, 63, 56, 60 },  // Byte 0
  { 54, 50, 53, 49, 55, 51, 52, 48 },  // Byte 1
  { 46, 47, 45, 44, 43, 42, 41, 40 },  // Byte 2
  { 22, 19, 23, 18, 16, 21, 20, 17 },  // Byte 3
  { 38, 34, 37, 33, 39, 35, 36, 32 },  // Byte 4
  { 26, 30, 25, 29, 27, 31, 24, 28 },  // Byte 5
  { 15, 11,  9, 13, 14, 10, 12,  8 },  // Byte 6
  {  6,  7,  0,  1,  4,  5,  3,  2 }   // Byte 7
};

#endif // ULT_FLAG

const MrcVddSelect MemoryVoltageTable[] = {
  //
  //                       MB     DT                  MB    DT
  // Voltage         //  GPIO24/GPIO60   GPIO46     GPIO8/GPIO45
  //
  1650,           //    0               0            0
  1600,           //    0               0            1
  1550,           //    0               1            0
  1503,           //    0               1            1
  1500,           //    1               0            0
  1450,           //    1               0            1
  1400,           //    1               1            0
  1350            //    1               1            1
};

/**
  Gets CPU ratio

  @param[in] Nothing

  @retval Cpu ratio.
**/
U32
MrcGetCpuRatio (
  void
  )
{
  PCU_CR_PLATFORM_INFO_STRUCT Msr;

  Msr.Data = AsmReadMsr64 (PCU_CR_PLATFORM_INFO);
  return (Msr.Bits.MAX_NON_TURBO_LIM_RATIO);
}

/**
  Gets CPU current time.

  @param[in] Nothing

  @retval The current CPU time in milliseconds.
**/
U64
MrcGetCpuTime (
  void
  )
{
  U32 TimeBase;

  TimeBase = (1000 * MRC_SYSTEM_BCLK) * MrcGetCpuRatio (); //In Millisec
  return ((TimeBase == 0) ? 0 : MrcOemMemoryDivideU64ByU64 (AsmReadTsc (), TimeBase));
}

/**
  Sets CpuModel and CpuStepping in MrcData based on CpuModelStep.

  @param[out] MrcData     - The Mrc Host data structure
  @param[in]  CpuModel    - The CPU Family Model.
  @param[in]  CpuStepping - The CPU Stepping.

  @retval - mrcSuccess if the model and stepping is found.  Otherwise mrcFail
**/
MrcStatus
MrcSetCpuInformation (
  OUT MrcParameters  *MrcData,
  IN  MrcCpuModel    CpuModel,
  IN  MrcCpuStepping CpuStepping
  )
{
  MrcInput  *Inputs;
  MrcDebug  *Debug;
  MrcStatus Status;

  Inputs = &MrcData->SysIn.Inputs;
  Debug  = &MrcData->SysIn.Inputs.Debug;
  Status = mrcFail;

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    Inputs->CpuModel = cmHSW_ULT;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Haswell ULT:");

    switch (CpuStepping) {
      case csHswUltB0:
        Inputs->CpuStepping = csHswUltB0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping B0\n");
        break;

      case csHswUltC0:
        Inputs->CpuStepping = csHswUltC0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping C0\n");
        break;

      default:
        Inputs->CpuStepping = csHswUltB0;   // @todo: Update for C0.
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_WARNING,
          "\nWARNING: Unknown CPU stepping, using MRC for last known step. Step = %Xh\n",
          Inputs->CpuStepping
          );
        break;
    }
    Status = mrcSuccess;
  }
#endif // ULT_FLAG

#ifdef TRAD_FLAG
  if (CpuModel == cmHSW) {
    Inputs->CpuModel = cmHSW;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Haswell:");

    switch (CpuStepping) {
      case csHswA0:
        Inputs->CpuStepping = csHswA0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping A0\n");
        break;

      case csHswB0:
        Inputs->CpuStepping = csHswB0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping B0\n");
        break;

      case csHswC0:
        Inputs->CpuStepping = csHswC0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping C0\n");
        break;

      default:
        Inputs->CpuStepping = csHswC0;
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_WARNING,
          "\nWARNING: Unknown CPU stepping, using MRC for last known step. Step = %Xh\n",
          Inputs->CpuStepping
          );
        break;
    }
    Status = mrcSuccess;
  }

  if (CpuModel == cmCRW) {
    Inputs->CpuModel = cmCRW;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Crystalwell:");

    switch (CpuStepping) {
      case csCrwB0:
        Inputs->CpuStepping = csCrwB0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping B0\n");
        break;

      case csCrwC0:
        Inputs->CpuStepping = csCrwC0;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Stepping C0\n");
        break;

      default:
        Inputs->CpuStepping = csCrwB0;  // @todo: Update for C0
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_WARNING,
          "\nWARNING: Unknown CPU stepping, using MRC for last known step. Step = %Xh\n",
          Inputs->CpuStepping
          );
        break;
    }
    Status = mrcSuccess;
  }
#endif // TRAD_FLAG

  return Status;
}

/**
  Gets a number from the CPU's random number generator.

  @param[in] Nothing

  @retval   Random number or zero if random number is not generated or is invalid.
**/
U32
AsmGetRandomNumber (
  void
  )
{
  U32 Status;
  U32 RandomNumber;

  // Assembly instruction to read CPU's random number generator
  // Instruction is only available 100k cycles after reset
  // rdrand eax
  // db 0Fh, 0C7h, 0F0h
#if defined __GNUC__  // GCC compiler
  __asm__ __volatile__ (
    "\n\t .byte 0x0F, 0xC7, 0xF0"
    "\n\t movl %%eax, %0"
    "\n\t pushf"
    "\n\t pop %%eax"
    "\n\t movl %%eax, %1"
    : "=m" (RandomNumber),
      "=m" (Status)
    );
#else //MSFT compiler
  ASM {
    _emit   0x0F
    _emit   0xC7
    _emit   0xF0
    mov     RandomNumber, eax

    pushfd
    pop     eax
    mov     Status, eax
  }
#endif
  // If CF is cleared, return 0
  return (((Status & 1) == 0) ? 0 : RandomNumber);
}

/**
  Gets a random number from the CPU's random number generator.

  @param[in] Nothing

  @retval Random number returned by the CPU instruction or generated from real time clock data.
**/
U32
MrcGetRandomNumber (
  void
  )
{
  U32 RandomNumber;
  U32 Retry;
  U16 Year;
  U8  Month;
  U8  DayOfMonth;
  U8  Hours;
  U8  Minutes;
  U8  Seconds;

  RandomNumber = 0;
  for (Retry = 100000; ((Retry != 0) && (RandomNumber == 0)); --Retry) {
    RandomNumber = AsmGetRandomNumber ();
  }
  if ((Retry == 0) && (RandomNumber == 0)) {
    MrcOemGetRtcTime (&Seconds, &Minutes, &Hours, &DayOfMonth, &Month, &Year);
    RandomNumber = Seconds + (Minutes * 60) + (Hours * 60 * 60);
  }

  return (RandomNumber);
}

/**
  This function enables 2x Refresh through the mailbox.

  @param[in]  MrcData - Pointer to the MRC global data structure

  @retval - Nothing.
**/
void
MrcOemEnable2xRefresh (
  IN MrcParameters *const MrcData
  )
{
#ifndef MRC_MINIBIOS_BUILD
  MrcDebug            *Debug;
  MrcMailbox2xRefresh Write2xRefreshData;
  U32                 MailboxStatus;

  Debug  = &MrcData->SysIn.Inputs.Debug;

  MailboxRead(MAILBOX_TYPE_PCODE, READ_DDR_FORCE_2X_REFRESH, &Write2xRefreshData.Data, &MailboxStatus);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Read Write2xRefreshData: 0x%x\n", Write2xRefreshData.Data);


  if (!Write2xRefreshData.Bits.Lock_Bit) {
    Write2xRefreshData.Bits.Lock_Bit          = 1;
    Write2xRefreshData.Bits.Enable_2x_Refresh = 1;
#ifdef ULT_FLAG
    if (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3) {
      Write2xRefreshData.Bits.LPDDR_Min_MR4 = RefRate2x; // Matches the JEDEC MR4 Encoding.
    }
#endif
    MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_DDR_FORCE_2X_REFRESH, Write2xRefreshData.Data, &MailboxStatus);

    if (MailboxStatus != PCODE_MAILBOX_CC_SUCCESS) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "WRITE_DDR_FORCE_2X_REFRESH failed.  MailboxStatus = 0x%x\n", MailboxStatus);
    } else {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Writing 0x%x to WRITE_DDR_FORCE_2X_REFRESH\n", Write2xRefreshData.Data);
    }
  }
#endif
}

/**
  This function changes the DIMM Voltage to the closest desired voltage without
  going higher. Default wait time is the minimum value of 200us, if more time
  is needed before deassertion of DIMM Reset#, then change the parameter.

  @param[in, out] MrcData            - The MRC "global data" area.
  @param[in]      VddVoltage         - Selects the DDR voltage to use, in mV.
  @param[in, out] VddSettleWaitTime  - Time needed for Vdd to settle after the update

  @retval TRUE if a voltage change occurred, otherwise FALSE.
**/
//[-start-130218-IB01831004-modify]//
BOOL
MrcOemVDDVoltageCheckAndSwitch (
  IN OUT MrcParameters      *MrcData,
  IN     const MrcVddSelect VddVoltage,
  IN OUT U32 * const        VddSettleWaitTime
  )
{
  MrcInput  *Inputs;
  MrcOutput *Outputs;
  BOOL      Status;
#ifdef DDR3LV_SUPPORT
  U32       GPIOBase;
  U32       GPIO_In_31_0;
  U32       GPIO_In_63_32;
  U32       GPIO_Out_31_0;
  U32       GPIO_Out_63_32;
  U32       Current;
  U8        Index;
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
// #ifdef DDR3LV_SUPPORT
#ifdef CUSTOMIZED_DDR3L_VOLTAGE
  U8        OEMSelectGPIO[3] = {OEM_DDR3L_VOLTAGE_PIN0,OEM_DDR3L_VOLTAGE_PIN1,OEM_DDR3L_VOLTAGE_PIN2};
  U8        IndexPin = 0;
#else
  U8        OEMSelectGPIO[3] = {0};
#endif
  U32       GPIO_In_95_64;
  U32       GPIO_Out_95_64;
  DDR3L_VOLTAGE_SETTING      SaveDDR3LDate;
  EFI_HOB_GUID_TYPE          *GuidHob = NULL;
#endif
//[-start-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//

//[-start-130108-IB01830995-modify]//
//[-end-121019-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
  Index = 0;
#endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Status  = FALSE;

#ifdef DDR3LV_SUPPORT
  if (((Inputs->MobilePlatform == TRUE) && (Inputs->BoardType != btUser4)) || (Inputs->MobilePlatform == FALSE)) {
    //
    // Read GPIO base.
    //
    MrcOemMmioRead (
      MrcOemGetPcieDeviceAddress (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE),
      &GPIOBase,
      Inputs->PciEBaseAddress
      );
    GPIOBase &= ~MRC_BIT0;

#ifdef MRC_MINIBIOS_BUILD
    if ((Inputs->MobilePlatform == TRUE) && (Inputs->BoardType != btUser4)) {
      //
      // Setup GPIOs (8,24,46)
      //
      GPIO_In_31_0 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL));
      if ((GPIO_In_31_0 & (MRC_BIT8 | MRC_BIT24)) != (MRC_BIT8 | MRC_BIT24)) {
        GPIO_In_31_0 |= (MRC_BIT8 | MRC_BIT24);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL), GPIO_In_31_0);
      }

      GPIO_In_63_32 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL2));
      if ((GPIO_In_63_32 & MRC_BIT14) != MRC_BIT14) {
        GPIO_In_63_32 |= MRC_BIT14;
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL2), GPIO_In_63_32);
      }

      GPIO_In_31_0 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL));
      if ((GPIO_In_31_0 & (MRC_BIT8 | MRC_BIT24)) != 0) {
        GPIO_In_31_0 &= ~(MRC_BIT8 | MRC_BIT24);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL), GPIO_In_31_0);
      }

      GPIO_In_63_32 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL2));
      if ((GPIO_In_63_32 & MRC_BIT14) != 0) {
        GPIO_In_63_32 &= ~MRC_BIT14;
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL2), GPIO_In_63_32);
      }
    } else if ((Inputs->MobilePlatform == FALSE) && (Inputs->BoardType == btCRBDT)) {
      //
      // Setup GPIOs (45,46,60)
      //
      GPIO_In_63_32 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL2));
      if ((GPIO_In_63_32 & (MRC_BIT28 | MRC_BIT14 | MRC_BIT13)) != (MRC_BIT28 | MRC_BIT14 | MRC_BIT13)) {
        GPIO_In_63_32 |= (MRC_BIT28 | MRC_BIT14 | MRC_BIT13);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_USE_SEL2), GPIO_In_63_32);
      }

      GPIO_In_63_32 = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL2));
      if ((GPIO_In_63_32 & (MRC_BIT28 | MRC_BIT14 | MRC_BIT13)) != 0) {
        GPIO_In_63_32 &= ~(MRC_BIT28 | MRC_BIT14 | MRC_BIT13);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_IO_SEL2), GPIO_In_63_32);
      }
    }
#endif // MRC_MINIBIOS_BUILD

    GPIO_In_31_0   = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_LVL));
    GPIO_In_63_32  = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_LVL2));
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
// #ifdef DDR3LV_SUPPORT
    GPIO_In_95_64  = MrcOemInPort32 ((U16) (GPIOBase + R_PCH_GPIO_LVL3));
// #endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//
    
    GPIO_Out_31_0  = GPIO_In_31_0;
    GPIO_Out_63_32 = GPIO_In_63_32;
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
// #ifdef DDR3LV_SUPPORT
    GPIO_Out_95_64 = GPIO_In_95_64;    
// #endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//

    for (Index = 0; Index < (sizeof (MemoryVoltageTable) / sizeof (MemoryVoltageTable[0])); Index++) {
      if (VddVoltage >= MemoryVoltageTable[Index]) {
        break;
      }
    }

#ifdef EMBEDDED_FLAG
    if (Inputs->BoardType == btCRBEMB) {
      //
      // Set GP24 to the required value.
      //
      Current = (((GPIO_Out_31_0 & MRC_BIT24) >> 22) ^ MRC_BIT2) | MRC_BIT1 | MRC_BIT0;
      (Index & MRC_BIT1) ? (GPIO_Out_31_0  &= (~MRC_BIT24)) : (GPIO_Out_31_0  |= MRC_BIT24);
    } else
#endif

//[-start-121019-IB05330384-add]//
//[-start-130108-IB01830995-modify]//
// #if defined(CUSTOMIZED_DDR3L_VOLTAGE) && defined (DDR3LV_SUPPORT)
#ifdef CUSTOMIZED_DDR3L_VOLTAGE
//[-end-130108-IB01830995-modify]//
    for (IndexPin = 0; IndexPin < 3; IndexPin++) {
      if (OEMSelectGPIO[IndexPin] < 32) {
        Current |= (GPIO_In_31_0 & (MRC_BIT0 >> (OEMSelectGPIO[IndexPin] - IndexPin)));
        (Index & (MRC_BIT0 << IndexPin)) ? (GPIO_Out_31_0 |= (MRC_BIT0 << OEMSelectGPIO[IndexPin])) : (GPIO_Out_31_0  &= (~(MRC_BIT0 << OEMSelectGPIO[IndexPin])));

      } else if ((OEMSelectGPIO[IndexPin] > 31) && (OEMSelectGPIO[IndexPin] < 64)){
        Current |= GPIO_In_63_32 & (MRC_BIT0 >> (OEMSelectGPIO[IndexPin] - IndexPin - 32));
        (Index & (MRC_BIT0 << IndexPin)) ? (GPIO_Out_63_32 |= (MRC_BIT0 << (OEMSelectGPIO[IndexPin] - 32))) : (GPIO_Out_63_32  &= (~(MRC_BIT0 << (OEMSelectGPIO[IndexPin]- 32))));

      } else {
        Current |= GPIO_In_63_32 & (MRC_BIT0 >> (OEMSelectGPIO[IndexPin] - IndexPin - 64));   
        (Index & (MRC_BIT0 << IndexPin)) ? (GPIO_Out_31_0 |= (MRC_BIT0 << (OEMSelectGPIO[IndexPin] - 64))) : (GPIO_Out_31_0  &= (~(MRC_BIT0 << (OEMSelectGPIO[IndexPin] - 64))));
      }
    }
#else
//[-end-121019-IB05330384-add]//
    if ((Inputs->MobilePlatform == TRUE) && (Inputs->BoardType != btUser4)) {
      //
      // Set GP8, GP24, and GP46 to the required value.
      //
      Current = (((GPIO_Out_31_0 & MRC_BIT24) >> 22) | ((GPIO_Out_63_32 & MRC_BIT14) >> 13) | ((GPIO_Out_31_0 & MRC_BIT8) >> 8));
      (Index & MRC_BIT2) ? (GPIO_Out_31_0  |= MRC_BIT24) : (GPIO_Out_31_0  &= (~MRC_BIT24));
      (Index & MRC_BIT1) ? (GPIO_Out_63_32 |= MRC_BIT14) : (GPIO_Out_63_32 &= (~MRC_BIT14));
      (Index & MRC_BIT0) ? (GPIO_Out_31_0  |= MRC_BIT8)  : (GPIO_Out_31_0  &= (~MRC_BIT8));
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
// #ifdef DDR3LV_SUPPORT
        OEMSelectGPIO[0] = 8;
        OEMSelectGPIO[1] = 46;
        OEMSelectGPIO[2] = 24;
// #endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//
    } else if ((Inputs->MobilePlatform == FALSE) && (Inputs->BoardType == btCRBDT)) {
      //
      // Set GP45, GP46, and GP60 to the required value.
      //
      Current = (((GPIO_Out_63_32 & MRC_BIT28) >> 26) | ((GPIO_Out_63_32 & MRC_BIT14) >> 13) | ((GPIO_Out_63_32 & MRC_BIT13) >> 13));
      (Index & MRC_BIT2) ? (GPIO_Out_63_32 |= MRC_BIT28) : (GPIO_Out_63_32 &= (~MRC_BIT28));
      (Index & MRC_BIT1) ? (GPIO_Out_63_32 |= MRC_BIT14) : (GPIO_Out_63_32 &= (~MRC_BIT14));
      (Index & MRC_BIT0) ? (GPIO_Out_63_32 |= MRC_BIT13) : (GPIO_Out_63_32 &= (~MRC_BIT13));
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
// #ifdef DDR3LV_SUPPORT
        OEMSelectGPIO[0] = 45;
        OEMSelectGPIO[1] = 46;
        OEMSelectGPIO[2] = 60;
// #endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//
    } else {
      Current = 4;
    }
//[-start-121019-IB05330384-add]//
#endif
//[-end-121019-IB05330384-add]//
    MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Current VddVoltage is %u mV\n", MemoryVoltageTable[Current]);
    if ((GPIO_In_31_0 != GPIO_Out_31_0) || (GPIO_In_63_32 != GPIO_Out_63_32)) {
        MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "**** VddVoltage updated to %u mV\n", VddVoltage);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_LVL), GPIO_Out_31_0);
        MrcOemOutPort32 ((U16) (GPIOBase + R_PCH_GPIO_LVL2), GPIO_Out_63_32);
        Status = TRUE;
    }

    //
    // Increase the VddSettleWaitTime by the amount requested in the Input structure
    //
    *VddSettleWaitTime += Inputs->VddSettleWaitTime;

    //
    // Either update was already done or change is not necessary every time this is called
    //
    Outputs->VddVoltageDone = TRUE;
  }
#endif
//[-start-130108-IB01830995-modify]//
//[-start-121019-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
  if(Outputs->VddVoltageDone) {

    GuidHob = GetFirstGuidHob ( &gDDR3LVoltageHobGuid );
    if (GuidHob == NULL) {
      SaveDDR3LDate.GPIOPingSelect[0] = OEMSelectGPIO[0];
      SaveDDR3LDate.GPIOPingSelect[1] = OEMSelectGPIO[1];
      SaveDDR3LDate.GPIOPingSelect[2] = OEMSelectGPIO[2];
      SaveDDR3LDate.GPIOPingSetting = Index;

      BuildGuidDataHob( &gDDR3LVoltageHobGuid,
                        &SaveDDR3LDate,
                        sizeof(DDR3L_VOLTAGE_SETTING)
                       );
    }
  }
#endif
//[-end-121019-IB05330384-add]//
//[-end-130108-IB01830995-modify]//
  return (Status);
}
//[-end-130218-IB01831004-modify]//

/**
  Hook before normal mode is enabled.

  @param[in, out] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
void
MrcOemBeforeNormalModeTestMenu (
  IN OUT MrcParameters *const MrcData
  )
{

  return;
}

/**
  Hook after normal mode is enabled

  @param[in] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
void
MrcOemAfterNormalModeTestMenu (
  IN MrcParameters *MrcData
  )
{

  MrcThermalOverwrites (MrcData);

  // @todo: Add lates code  DDR Thermal Management, throttling control. Also UP CLTT code
  //
  return;
}

/**
  Overwrite Thermal settings

  @param[in] MrcData  - The MRC "global data" area.

  @retval Nothing.
**/
void
MrcThermalOverwrites (
  IN MrcParameters *MrcData
  )
{
  MrcInput                                        *Inputs;
  MrcOutput                                       *Outputs;
  MrcDebug                                        *Debug;
  MrcDdrType                                      DdrType;
  ThermalMngmtEn                                  *ThermalEnables;
  U8                                              Channel;
  U32                                             Offset;
  PCU_CR_DDR_PTM_CTL_PCU_STRUCT                   DdrPtmCtl;
  PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_STRUCT        DdrEnergyScaleFactor;
  PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_STRUCT  DdrRaplChannelPowerFloor;
  PCU_CR_DDR_RAPL_LIMIT_PCU_STRUCT                DdrRaplLimit;
  PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_STRUCT        DdrWarmThresholdCh0;
  PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_STRUCT        DdrWarmThresholdCh1;
  PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_STRUCT         DdrHotThresholdCh0;
  PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_STRUCT         DdrHotThresholdCh1;
  PCU_CR_DDR_WARM_BUDGET_CH0_PCU_STRUCT           DdrWarmBudgetCh0;
  PCU_CR_DDR_WARM_BUDGET_CH1_PCU_STRUCT           DdrWarmBudgetCh1;
  PCU_CR_DDR_HOT_BUDGET_CH0_PCU_STRUCT            DdrHotBudgetCh0;
  PCU_CR_DDR_HOT_BUDGET_CH1_PCU_STRUCT            DdrHotBudgetCh1;
  MCHBAR_CH0_CR_PM_DIMM_IDLE_ENERGY_STRUCT        PmDimmIdleEnergy;
  MCHBAR_CH0_CR_PM_DIMM_PD_ENERGY_STRUCT          PmDimmPdEnergy;
  MCHBAR_CH0_CR_PM_DIMM_ACT_ENERGY_STRUCT         PmDimmActEnergy;
  MCHBAR_CH0_CR_PM_DIMM_RD_ENERGY_STRUCT          PmDimmRdEnergy;
  MCHBAR_CH0_CR_PM_DIMM_WR_ENERGY_STRUCT          PmDimmWrEnergy;
  MCHBAR_CH0_CR_PM_THRT_CKE_MIN_STRUCT            PmThrtCkeMin;
  MCDECS_CR_PM_SREF_CONFIG_MCMAIN_STRUCT          PmSrefConfig;

  Inputs          = &MrcData->SysIn.Inputs;
  Debug           = &Inputs->Debug;
  ThermalEnables  = &Inputs->ThermalEnables;
  Outputs         = &MrcData->SysOut.Outputs;
  DdrType         = Outputs->DdrType;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "...Thermal Overwrite ...\n");

  if (Inputs->ThermalEnables.UserPowerWeightsEn) {
    //
    // ENERGY SCALE FACTOR
    //
    DdrEnergyScaleFactor.Data             = 0;
    DdrEnergyScaleFactor.Bits.SCALEFACTOR = ThermalEnables->EnergyScaleFact;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "DDR_ENERGY_SCALEFACTOR %Xh: %Xh \n",
      PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_REG,
      DdrEnergyScaleFactor.Data
      );
    MrcWriteCR (MrcData, PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_REG, DdrEnergyScaleFactor.Data);

    //
    // RAPL POWER FLOOR
    //
    DdrRaplChannelPowerFloor.Data     = 0;
    DdrRaplChannelPowerFloor.Bits.CH0 = ThermalEnables->RaplPwrFl[0];
    DdrRaplChannelPowerFloor.Bits.CH1 = ThermalEnables->RaplPwrFl[1];
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "DDR_RAPL_CHANNEL_POWER_FLOOR %Xh: %Xh \n",
      PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_REG,
      DdrRaplChannelPowerFloor.Data
      );
    MrcWriteCR (MrcData, PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_REG, DdrRaplChannelPowerFloor.Data);
  }

  //
  // RAPL LIMIT
  //
  DdrRaplLimit.Data                       = 0;
  DdrRaplLimit.Bits.LIMIT1_TIME_WINDOW_X  = ThermalEnables->RaplLim1WindX;
  DdrRaplLimit.Bits.LIMIT1_TIME_WINDOW_Y  = ThermalEnables->RaplLim1WindY;
  DdrRaplLimit.Bits.LIMIT1_ENABLE         = ThermalEnables->RaplLim1Ena;
  DdrRaplLimit.Bits.LIMIT1_POWER          = ThermalEnables->RaplLim1Pwr;
  DdrRaplLimit.Bits.LOCKED                = ThermalEnables->RaplLim2Lock;
  DdrRaplLimit.Bits.LIMIT2_TIME_WINDOW_X  = ThermalEnables->RaplLim2WindX;
  DdrRaplLimit.Bits.LIMIT2_TIME_WINDOW_Y  = ThermalEnables->RaplLim2WindY;
  DdrRaplLimit.Bits.LIMIT2_ENABLE         = ThermalEnables->RaplLim2Ena;
  DdrRaplLimit.Bits.LIMIT2_POWER          = ThermalEnables->RaplLim2Pwr;
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_RAPL_LIMIT1 %Xh: %Xh \n",
    PCU_CR_DDR_RAPL_LIMIT_PCU_REG,
    DdrRaplLimit.Data32[0]
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_RAPL_LIMIT2 %Xh: %Xh \n",
    PCU_CR_DDR_RAPL_LIMIT_PCU_REG + 4,
    DdrRaplLimit.Data32[1]
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_RAPL_LIMIT_PCU_REG, DdrRaplLimit.Data32[0]);
  MrcWriteCR (MrcData, PCU_CR_DDR_RAPL_LIMIT_PCU_REG + 4, DdrRaplLimit.Data32[1]);

  //
  // DDR WARM AND HOT THRESHOLD
  //
  DdrWarmThresholdCh0.Data        = 0;
  DdrWarmThresholdCh0.Bits.DIMM1  = ThermalEnables->WarmThreshold[0][1];
  DdrWarmThresholdCh0.Bits.DIMM0  = ThermalEnables->WarmThreshold[0][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_WARM_THRESHOLD_CH0 %Xh: %Xh \n",
    PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_REG,
    DdrWarmThresholdCh0.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_REG, DdrWarmThresholdCh0.Data);

  DdrWarmThresholdCh1.Data        = 0;
  DdrWarmThresholdCh1.Bits.DIMM1  = ThermalEnables->WarmThreshold[1][1];
  DdrWarmThresholdCh1.Bits.DIMM0  = ThermalEnables->WarmThreshold[1][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_WARM_THRESHOLD_CH1 %Xh: %Xh \n",
    PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_REG,
    DdrWarmThresholdCh1.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_REG, DdrWarmThresholdCh1.Data);

  DdrHotThresholdCh0.Data       = 0;
  DdrHotThresholdCh0.Bits.DIMM1 = ThermalEnables->HotThreshold[0][1];
  DdrHotThresholdCh0.Bits.DIMM0 = ThermalEnables->HotThreshold[0][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_HOT_THRESHOLD_CH0 %Xh: %Xh \n",
    PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_REG,
    DdrHotThresholdCh0.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_REG, DdrHotThresholdCh0.Data);

  DdrHotThresholdCh1.Data       = 0;
  DdrHotThresholdCh1.Bits.DIMM1 = ThermalEnables->HotThreshold[1][1];
  DdrHotThresholdCh1.Bits.DIMM0 = ThermalEnables->HotThreshold[1][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_HOT_THRESHOLD_CH1 %Xh: %Xh \n",
    PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_REG,
    DdrHotThresholdCh1.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_REG, DdrHotThresholdCh1.Data);

  //
  // DDR WARM AND HOT BUDGET
  //
  DdrWarmBudgetCh0.Data       = 0;
  DdrWarmBudgetCh0.Bits.DIMM1 = ThermalEnables->WarmBudget[0][1];
  DdrWarmBudgetCh0.Bits.DIMM0 = ThermalEnables->WarmBudget[0][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_WARM_BUDGET_CH0 %Xh: %Xh \n",
    PCU_CR_DDR_WARM_BUDGET_CH0_PCU_REG,
    DdrWarmBudgetCh0.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_WARM_BUDGET_CH0_PCU_REG, DdrWarmBudgetCh0.Data);

  DdrWarmBudgetCh1.Data       = 0;
  DdrWarmBudgetCh1.Bits.DIMM1 = ThermalEnables->WarmBudget[1][1];
  DdrWarmBudgetCh1.Bits.DIMM0 = ThermalEnables->WarmBudget[1][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_WARM_BUDGET_CH1 %Xh: %Xh \n",
    PCU_CR_DDR_WARM_BUDGET_CH1_PCU_REG,
    DdrWarmBudgetCh1.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_WARM_BUDGET_CH1_PCU_REG, DdrWarmBudgetCh1.Data);

  DdrHotBudgetCh0.Data        = 0;
  DdrHotBudgetCh0.Bits.DIMM1  = ThermalEnables->HotBudget[0][1];
  DdrHotBudgetCh0.Bits.DIMM0  = ThermalEnables->HotBudget[0][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_HOT_BUDGET_CH0 %Xh: %Xh \n",
    PCU_CR_DDR_HOT_BUDGET_CH0_PCU_REG,
    DdrHotBudgetCh0.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_HOT_BUDGET_CH0_PCU_REG, DdrHotBudgetCh0.Data);

  DdrHotBudgetCh1.Data        = 0;
  DdrHotBudgetCh1.Bits.DIMM1  = ThermalEnables->HotBudget[1][1];
  DdrHotBudgetCh1.Bits.DIMM0  = ThermalEnables->HotBudget[1][0];
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "DDR_HOT_BUDGET_CH1 %Xh: %Xh \n",
    PCU_CR_DDR_HOT_BUDGET_CH1_PCU_REG,
    DdrHotBudgetCh1.Data
    );
  MrcWriteCR (MrcData, PCU_CR_DDR_HOT_BUDGET_CH1_PCU_REG, DdrHotBudgetCh1.Data);

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      if (Inputs->ThermalEnables.UserPowerWeightsEn) {
        PmDimmIdleEnergy.Data                   = 0;
        PmDimmIdleEnergy.Bits.DIMM1_IDLE_ENERGY = ThermalEnables->IdleEnergy[Channel][1];
        PmDimmIdleEnergy.Bits.DIMM0_IDLE_ENERGY = ThermalEnables->IdleEnergy[Channel][0];
        Offset = MCHBAR_CH0_CR_PM_DIMM_IDLE_ENERGY_REG +
          (MCHBAR_CH1_CR_PM_DIMM_IDLE_ENERGY_REG - MCHBAR_CH0_CR_PM_DIMM_IDLE_ENERGY_REG) * Channel;
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "C%u PM_DIMM_IDLE_ENERGY %Xh: %Xh \n",
          Channel,
          Offset,
          PmDimmIdleEnergy.Data
          );
        MrcWriteCR (MrcData, Offset, PmDimmIdleEnergy.Data);

        PmDimmPdEnergy.Data                 = 0;
        PmDimmPdEnergy.Bits.DIMM1_PD_ENERGY = ThermalEnables->PdEnergy[Channel][1];
        PmDimmPdEnergy.Bits.DIMM0_PD_ENERGY = ThermalEnables->PdEnergy[Channel][0];
        Offset = MCHBAR_CH0_CR_PM_DIMM_PD_ENERGY_REG +
          (MCHBAR_CH1_CR_PM_DIMM_PD_ENERGY_REG - MCHBAR_CH0_CR_PM_DIMM_PD_ENERGY_REG) * Channel;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u PM_DIMM_PD_ENERGY %Xh: %Xh \n", Channel, Offset, PmDimmPdEnergy.Data);
        MrcWriteCR (MrcData, Offset, PmDimmPdEnergy.Data);

        PmDimmActEnergy.Data                  = 0;
        PmDimmActEnergy.Bits.DIMM1_ACT_ENERGY = ThermalEnables->ActEnergy[Channel][1];
        PmDimmActEnergy.Bits.DIMM0_ACT_ENERGY = ThermalEnables->ActEnergy[Channel][0];
        Offset = MCHBAR_CH0_CR_PM_DIMM_ACT_ENERGY_REG +
          (MCHBAR_CH1_CR_PM_DIMM_ACT_ENERGY_REG - MCHBAR_CH0_CR_PM_DIMM_ACT_ENERGY_REG) * Channel;
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "C%u PM_DIMM_ACT_ENERGY %Xh: %Xh \n",
          Channel,
          Offset,
          PmDimmActEnergy.Data
          );
        MrcWriteCR (MrcData, Offset, PmDimmActEnergy.Data);

        PmDimmRdEnergy.Data                 = 0;
        PmDimmRdEnergy.Bits.DIMM1_RD_ENERGY = ThermalEnables->RdEnergy[Channel][1];
        PmDimmRdEnergy.Bits.DIMM0_RD_ENERGY = ThermalEnables->RdEnergy[Channel][0];
        Offset = MCHBAR_CH0_CR_PM_DIMM_RD_ENERGY_REG +
          (MCHBAR_CH1_CR_PM_DIMM_RD_ENERGY_REG - MCHBAR_CH0_CR_PM_DIMM_RD_ENERGY_REG) * Channel;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u PM_DIMM_RD_ENERGY %Xh: %Xh \n", Channel, Offset, PmDimmRdEnergy.Data);
        MrcWriteCR (MrcData, Offset, PmDimmRdEnergy.Data);

        PmDimmWrEnergy.Data                 = 0;
        PmDimmWrEnergy.Bits.DIMM1_WR_ENERGY = ThermalEnables->WrEnergy[Channel][1];
        PmDimmWrEnergy.Bits.DIMM0_WR_ENERGY = ThermalEnables->WrEnergy[Channel][0];
        Offset = MCHBAR_CH0_CR_PM_DIMM_WR_ENERGY_REG +
          (MCHBAR_CH1_CR_PM_DIMM_WR_ENERGY_REG - MCHBAR_CH0_CR_PM_DIMM_WR_ENERGY_REG) * Channel;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u PM_DIMM_WR_ENERGY %Xh: %Xh \n", Channel, Offset, PmDimmWrEnergy.Data);
        MrcWriteCR (MrcData, Offset, PmDimmWrEnergy.Data);
      }

      PmThrtCkeMin.Data                   = 0;
#ifdef ULT_FLAG
      if (DdrType == MRC_DDR_TYPE_LPDDR3) {
        PmThrtCkeMin.Bits.CKE_MIN_DEFEATURE = ThermalEnables->ThrtCkeMinDefeatLpddr;
        PmThrtCkeMin.Bits.CKE_MIN           = ThermalEnables->ThrtCkeMinTmrLpddr;
      } else
#endif // ULT_FLAG
      {
        PmThrtCkeMin.Bits.CKE_MIN_DEFEATURE = ThermalEnables->ThrtCkeMinDefeat;
        PmThrtCkeMin.Bits.CKE_MIN           = ThermalEnables->ThrtCkeMinTmr;
      }
      Offset = MCHBAR_CH0_CR_PM_THRT_CKE_MIN_REG +
        (MCHBAR_CH1_CR_PM_THRT_CKE_MIN_REG - MCHBAR_CH0_CR_PM_THRT_CKE_MIN_REG) * Channel;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u PM_THRT_CKE_MIN %Xh: %Xh \n", Channel, Offset, PmThrtCkeMin.Data);
      MrcWriteCR (MrcData, Offset, PmThrtCkeMin.Data);
    }
  }

  PmSrefConfig.Data             = 0;
  PmSrefConfig.Bits.SR_Enable   = ThermalEnables->SrefCfgEna;
  PmSrefConfig.Bits.Idle_timer  = ThermalEnables->SrefCfgIdleTmr;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "PM_SREF_CONFIG %Xh: %Xh\n", MCDECS_CR_PM_SREF_CONFIG_MCMAIN_REG, PmSrefConfig.Data);
  MrcWriteCR (MrcData, MCDECS_CR_PM_SREF_CONFIG_MCMAIN_REG, PmSrefConfig.Data);

  //
  // POWER THERMAL MANAGEMENT CONTROL
  //
  DdrPtmCtl.Data                    = MrcReadCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG);
  DdrPtmCtl.Bits.OLTM_ENABLE        = ThermalEnables->EnableOltm;
  DdrPtmCtl.Bits.CLTM_ENABLE        = ThermalEnables->EnableCltm;
  DdrPtmCtl.Bits.EXTTS_ENABLE       = ThermalEnables->EnableExtts;
  DdrPtmCtl.Bits.REFRESH_2X_MODE    = ThermalEnables->Refresh2X;
#ifdef ULT_FLAG
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    DdrPtmCtl.Bits.PDWN_CONFIG_CTL = ThermalEnables->EnablePwrDnLpddr;
    //
    // When enabling 2x Refresh for LPDDR through the Mailbox we must
    // ensure DDR_PTM_CTL.DISABLE_DRAM_TS = 0.  Thus we ignore LpddrThermalSensor.
    //
    if (Inputs->RefreshRate2x == FALSE) {
      DdrPtmCtl.Bits.DISABLE_DRAM_TS = (ThermalEnables->LpddrThermalSensor == 0) ? 1 : 0;
    } else {
       MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Ignoring ThermalEnables->LpddrThermal Sensor as 2x Refresh is enabled\n");
    }
  } else
#endif // ULT_FLAG
  {
    DdrPtmCtl.Bits.PDWN_CONFIG_CTL = ThermalEnables->EnablePwrDn;
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DDR_PTM_CTL %Xh: %Xh\n", PCU_CR_DDR_PTM_CTL_PCU_REG, DdrPtmCtl.Data);
  MrcWriteCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG, DdrPtmCtl.Data);

  return;
}


/**
  this function use by the OEM to do dedicated task during the MRC.

  @param[in] MrcData           - include all the MRC data
  @param[in] OemStatusCommand  - A command that indicates the task to perform.
  @param[in] ptr               - general ptr for general use.

  @retval The status of the task.
**/
MrcStatus
MrcOemCheckPoint (
  IN MrcParameters        *MrcData,
  IN MRC_OemStatusCommand OemStatusCommand,
  IN void                 *ptr
  )
{
  MrcInput  *Inputs;
  MrcOutput *Outputs;
  MrcStatus Status;
  MrcDdrType DdrType;

  Status  = mrcSuccess;
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  DdrType = Outputs->DdrType;


  switch (OemStatusCommand) {
  case OemSpdProcessingRun:
    break;

  case OemPreTraining:
    break;

  case OemMcTrainingRun:
    break;

  case OemEarlyCommandTraining:
    if (DdrType == MRC_DDR_TYPE_LPDDR3) {
      Status = mrcSuccess;  // This is required for LPDDR
    } else if (!Inputs->TrainingEnables.ECT) {
      Status = mrcFail; // Skip this training step
    }
    break;

#ifdef ULT_FLAG
  case OemJedecInitLpddr3:    // MrcJedecInitLpddr3
    if (DdrType == MRC_DDR_TYPE_LPDDR3) {
      Status = mrcSuccess;
    } else {
      Status = mrcFail;  // Skip this step for non-LPDDR
    }
    break;
#endif // ULT_FLAG

  case OemSenseAmpTraining:
    if (!Inputs->TrainingEnables.SOT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadMprTraining:
    if (!Inputs->TrainingEnables.RDMPRT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReceiveEnable:
    if (!Inputs->TrainingEnables.RCVET) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemJedecWriteLeveling:
    if (!Inputs->TrainingEnables.JWRL) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteLeveling:
    if (!Inputs->TrainingEnables.FWRL) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteDqDqs:
    if (!Inputs->TrainingEnables.WRTC1D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadDqDqs:
    if (!Inputs->TrainingEnables.RDTC1D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemDimmODTTraining:
    if (!Inputs->TrainingEnables.DIMMODTT) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemDimmRonTraining:
    if (!Inputs->TrainingEnables.DIMMRONT) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteSlewRate:
    if (!Inputs->TrainingEnables.WRSRT) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemDimmODT1dTraining:
    if (!Inputs->TrainingEnables.DIMMODTT1D) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteDriveStrength:
    if (!Inputs->TrainingEnables.WRDST) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteEQTraining:
    if (!Inputs->TrainingEnables.WREQT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadODTTraining:
    if (!Inputs->TrainingEnables.RDODTT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadEQTraining:
    if (!Inputs->TrainingEnables.RDEQT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemPostTraining:
    if (DdrType == MRC_DDR_TYPE_LPDDR3) {
      Status = mrcFail; // Skip this training step
      break;
    }
    break;

  case OemReadAmplifierPower:
    if (!Inputs->TrainingEnables.RDAPT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemOptimizeComp:
    break;

  case OemWriteDqDqs2D:
    if (!Inputs->TrainingEnables.WRTC2D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadDqDqs2D:
    if (!Inputs->TrainingEnables.RDTC2D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemCmdVoltCentering:
    if (!Inputs->TrainingEnables.CMDVC) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemWriteVoltCentering2D:
    if (!Inputs->TrainingEnables.WRVC2D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemReadVoltCentering2D:
    if (!Inputs->TrainingEnables.RDVC2D) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemLateCommandTraining:
    if (!Inputs->TrainingEnables.LCT) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemRoundTripLatency:
    if (!Inputs->TrainingEnables.RTL) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemTurnAroundTimes:
    //
    // @todo: TAT has to be updated for LPDDR3, skip it for now.
    //
    if ((!Inputs->TrainingEnables.TAT) || (DdrType == MRC_DDR_TYPE_LPDDR3)) {
      Status = mrcFail; // Skip this training step
    }
    break;

#ifdef ULT_FLAG
    case OemRcvEnCentering1D:
      if ((!Inputs->TrainingEnables.RCVENC1D) || (DdrType != MRC_DDR_TYPE_LPDDR3)) {
        Status = mrcFail;  // Skip this step for non-LPDDR
      }
      break;
#endif // ULT_FLAG

  case OemRetrainMarginCheck:
    if (!Inputs->TrainingEnables.RMC) {
        Status = mrcFail; // Skip this training step
    }
    break;


  case OemRmt:
    if (!Inputs->TrainingEnables.RMT) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemMemTest:
    if (!Inputs->TrainingEnables.MEMTST) {
        Status = mrcFail; // Skip this training step
    }
    break;

  case OemAliasCheck:
    if (!Inputs->TrainingEnables.ALIASCHK) {
      Status = mrcFail; // Skip this training step
    }
    break;

  case OemBeforeNormalMode:
    MrcOemBeforeNormalModeTestMenu (MrcData);
    break;

  case OemAfterNormalMode:
    MrcOemAfterNormalModeTestMenu (MrcData);
    break;

  case OemFrequencySetDone:
#ifdef SSA_FLAG
#ifndef MRC_MINIBIOS_BUILD
    SsaBiosInitialize (MrcData);
#endif
#endif // SSA_FLAG
    break;

  default:
    break;
  }

  return Status;
}

/**
  This function display on port 80 number.
  It can be different debug interface.
  This function can be use for any debug ability according to OEM requirements.

  @param[in] MrcData            - Mrc Global Data
  @param[in] DisplayDebugNumber - the number to display on port 80.

  @retval Nothing
**/
void
MrcOemDebugHook (
  IN MrcParameters *MrcData,
  IN U16           DisplayDebugNumber
  )
{
  MrcInput  *Inputs;
  U8        temp;
  U16       BreakCmos;

  Inputs                    = &MrcData->SysIn.Inputs;
  Inputs->Debug.PostCode[1] = DisplayDebugNumber;
  MrcOemOutPort16 (0x80, DisplayDebugNumber);
  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Post Code: 0x%X\n", DisplayDebugNumber);

  //
  // Check if we should break on this post code.
  //
  do {
    temp = RtcRead (MRC_POST_CODE_LOW_BYTE_ADDR);
    BreakCmos = (RtcRead (MRC_POST_CODE_HIGH_BYTE_ADDR) << 8) | temp;
  } while (DisplayDebugNumber == BreakCmos);

#ifdef SSA_FLAG
#ifndef MRC_MINIBIOS_BUILD
  if ((void *) (Inputs->SsaCallbackPpi) != NULL) {
    (((SSA_BIOS_CALLBACKS_PPI *) (Inputs->SsaCallbackPpi))->MrcCheckpoint) ((EFI_PEI_SERVICES **) (Inputs->Debug.Stream), ((SSA_BIOS_CALLBACKS_PPI *)Inputs->SsaCallbackPpi), DisplayDebugNumber, NULL);
  }
#endif
#endif // SSA_FLAG

  return;
}

#ifdef MRC_DEBUG_PRINT
/**
  Print the input parameters to the debug message output port.

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess
**/
MrcStatus
MrcPrintInputParameters (
  IN MrcParameters *const MrcData
  )
{
  const MrcDebug                        *Debug;
  const MrcInput                        *Inputs;
  const MrcControllerIn                 *ControllerIn;
  const MrcChannelIn                    *ChannelIn;
  const MrcDimmIn                       *DimmIn;
  const TrainingStepsEn                 *TrainingSteps;
  U8                                    Controller;
  U8                                    Channel;
  U8                                    Dimm;

  Inputs = &MrcData->SysIn.Inputs;
  Debug  = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*****MRC INPUT PARAMS DUMP START*****\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  //
  // The following are system level definitions. All memory controllers in the system are set to these values.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Debug.Stream : %Xh\n", Inputs->Debug.Stream);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Debug.Level : %Xh\n", Inputs->Debug.Level);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "FreqMax : %u\n", Inputs->FreqMax);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Ratio : %u\n", Inputs->Ratio);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RefClk : %uMHz\n", (Inputs->RefClk == MRC_REF_CLOCK_100) ? 100 : 133);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BClk : %uHz\n", Inputs->BClkFrequency);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BoardType : %Xh\n", Inputs->BoardType);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CpuStepping : %Xh\n", Inputs->CpuStepping);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CpuModel : %Xh\n", Inputs->CpuModel);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GraphicsStolenSize : %Xh\n", Inputs->GraphicsStolenSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GraphicsGttSize : %Xh\n", Inputs->GraphicsGttSize);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Seconds : %u\n", BaseTimeString, Inputs->BaseTime.Seconds);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Minutes : %u\n", BaseTimeString, Inputs->BaseTime.Minutes);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Hours : %u\n", BaseTimeString, Inputs->BaseTime.Hours);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.DayOfMonth : %u\n", BaseTimeString, Inputs->BaseTime.DayOfMonth);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Month : %u\n", BaseTimeString, Inputs->BaseTime.Month);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Year : %u\n", BaseTimeString, Inputs->BaseTime.Year);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Iteration : %Xh\n", Inputs->Iteration);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MrcMode : %Xh\n", Inputs->MrcMode);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "VddVoltage : %u mV\n", Inputs->VddVoltage);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MemoryProfile : %Xh\n", Inputs->MemoryProfile);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BootMode : %Xh\n", Inputs->BootMode);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TxtFlag : %Xh\n", Inputs->TxtFlag);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MobilePlatform : %Xh\n", Inputs->MobilePlatform);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "EccSupport : %Xh\n", Inputs->EccSupport);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SetRxDqs32 : %Xh\n", Inputs->SetRxDqs32);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GfxIsVersatileAcceleration : %Xh\n", Inputs->GfxIsVersatileAcceleration);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ScramblerEnable : %Xh\n", Inputs->ScramblerEnable);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "McLock : %Xh\n", Inputs->McLock);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RemapEnable : %Xh\n", Inputs->RemapEnable);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "PowerDownMode : %Xh\n", Inputs->PowerDownMode);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "PwdwnIdleCounter : %Xh\n", Inputs->PwdwnIdleCounter);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RankInterleave : %Xh\n", Inputs->RankInterleave);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "EnhancedInterleave : %Xh\n", Inputs->EnhancedInterleave);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "WeaklockEn : %Xh\n", Inputs->WeaklockEn);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "EnCmdRate : %Xh\n", Inputs->EnCmdRate);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CmdTriStateDis : %Xh\n", Inputs->CmdTriStateDis);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RefreshRate2x : %Xh\n", Inputs->RefreshRate2x);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BaseAddresses\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  PciE : %Xh\n", Inputs->PciEBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  MchBar : %Xh\n", Inputs->MchBarBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Smbus : %Xh\n", Inputs->SmbusBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Gdxc : %Xh\n", Inputs->GdxcBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Hpet : %Xh\n\n", Inputs->HpetBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MeStolenSize : %Xh\n", Inputs->MeStolenSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MmioSize : %Xh\n", Inputs->MmioSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TsegSize : %Xh\n", Inputs->TsegSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "IedSize : %Xh\n", Inputs->IedSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DprSize : %Xh\n", Inputs->DprSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "VddSettleWaitTime : %Xh\n", Inputs->VddSettleWaitTime);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "VccIomV : %Xh\n", Inputs->VccIomV);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "AutoSelfRefreshSupport : %u\n", Inputs->AutoSelfRefreshSupport);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ExtTemperatureSupport : %u\n", Inputs->ExtTemperatureSupport);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ChHashEnable : %Xh\n", Inputs->ChHashEnable);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ChHashMask : %Xh\n", Inputs->ChHashMask);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ChHashInterleaveBit : %Xh\n", Inputs->ChHashInterleaveBit);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%sEnable : %Xh\n", GdxcString, Inputs->Gdxc.GdxcEnable);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%sIotSize : %Xh\n", GdxcString, Inputs->Gdxc.GdxcIotSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%sMotSize : %Xh\n", GdxcString, Inputs->Gdxc.GdxcMotSize);
  
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MemoryTrace: %u\n", Inputs->MemoryTrace);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*****    MRC TRAINING STEPS     *****\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  TrainingSteps = &Inputs->TrainingEnables;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s : %Xh\n", TrainEnString, Inputs->TrainingEnables);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ECT : %u\n", TrainEnString, TrainingSteps->ECT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.SOT : %u\n", TrainEnString, TrainingSteps->SOT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDMPRT : %u\n", TrainEnString, TrainingSteps->RDMPRT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RCVET : %u\n", TrainEnString, TrainingSteps->RCVET);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.JWRL : %u\n", TrainEnString, TrainingSteps->JWRL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.FWRL : %u\n", TrainEnString, TrainingSteps->FWRL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WRTC1D : %u\n", TrainEnString, TrainingSteps->WRTC1D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDTC1D : %u\n", TrainEnString, TrainingSteps->RDTC1D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.DIMMODTT : %u\n", TrainEnString, TrainingSteps->DIMMODTT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WRDST : %u\n", TrainEnString, TrainingSteps->WRDST);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WREQT : %u\n", TrainEnString, TrainingSteps->WREQT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDODTT : %u\n", TrainEnString, TrainingSteps->RDODTT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDEQT : %u\n", TrainEnString, TrainingSteps->RDEQT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDAPT : %u\n", TrainEnString, TrainingSteps->RDAPT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WRTC2D : %u\n", TrainEnString, TrainingSteps->WRTC2D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDTC2D : %u\n", TrainEnString, TrainingSteps->RDTC2D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WRVC2D : %u\n", TrainEnString, TrainingSteps->WRVC2D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RDVC2D : %u\n", TrainEnString, TrainingSteps->RDVC2D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.LCT : %u\n", TrainEnString, TrainingSteps->LCT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RTL : %u\n", TrainEnString, TrainingSteps->RTL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.TAT : %u\n", TrainEnString, TrainingSteps->TAT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RMT : %u\n", TrainEnString, TrainingSteps->RMT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.MEMTST : %u\n", TrainEnString, TrainingSteps->MEMTST);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.DIMMODTT1D : %u\n", TrainEnString, TrainingSteps->DIMMODTT1D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WRSRT : %u\n", TrainEnString, TrainingSteps->WRSRT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.DIMMRONT : %u\n", TrainEnString, TrainingSteps->DIMMRONT);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ALIASCHK : %u\n", TrainEnString, TrainingSteps->ALIASCHK);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RCVENC1D : %u\n", TrainEnString, TrainingSteps->RCVENC1D);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RMC : %u\n", TrainEnString, TrainingSteps->RMC);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*****      MRC TIMING DATA      *****\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn = &Inputs->Controller[Controller];
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Controller[%u] ChannelCount : %Xh\n", Controller, ControllerIn->ChannelCount);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn = &ControllerIn->Channel[Channel];
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel[%u].Status : %Xh\n", Channel, ChannelIn->Status);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel[%u].DimmCount : %Xh\n", Channel, ChannelIn->DimmCount);

      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmIn = &ChannelIn->Dimm[Dimm];
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u Status : %Xh\n", CcdString, Controller, Channel, Dimm, DimmIn->Status);
        if (Inputs->MemoryProfile == USER_PROFILE) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tCK    : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tCK);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u NMode  : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.NMode);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tCL    : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tCL);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tCWL   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tCWL);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tFAW   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tFAW);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRAS   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRAS);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRC    : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRC);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRCD   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRCD);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tREFI  : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tREFI);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRFC   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRFC);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRP    : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRP);
#ifdef ULT_FLAG
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRPab  : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRPab);
#endif // ULT_FLAG
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRRD   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRRD);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRTP   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tRTP);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tWR    : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tWR);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tWTR   : %u\n", CcdString, Controller, Channel, Dimm, DimmIn->Timing.tWTR);
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u SpdAddress : %Xh\n", CcdString, Controller, Channel, Dimm, DimmIn->SpdAddress);
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*****    THERMAL OVERWRITE    *******\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnableExtts     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnableExtts);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnableCltm      : %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnableCltm);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnableOltm      : %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnableOltm);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnablePwrDn     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnablePwrDn);
#ifdef ULT_FLAG
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnablePwrDnLpddr: %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnablePwrDnLpddr);
#endif // ULT_FLAG
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.Refresh2X       : %Xh\n",   ThermEnString, Inputs->ThermalEnables.Refresh2X);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.LpddrThermalSensor: %Xh\n", ThermEnString, Inputs->ThermalEnables.LpddrThermalSensor);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.LockPTMregs     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.LockPTMregs);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.UserPowerWeightsEn: %Xh\n", ThermEnString, Inputs->ThermalEnables.UserPowerWeightsEn);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.EnergyScaleFact : %Xh\n",   ThermEnString, Inputs->ThermalEnables.EnergyScaleFact);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim2Lock    : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim2Lock);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim2WindX   : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim2WindX);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim2WindY   : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim2WindY);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim2Ena     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim2Ena);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim2Pwr     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim2Pwr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim1WindX   : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim1WindX);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim1WindY   : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim1WindY);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim1Ena     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim1Ena);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplLim1Pwr     : %Xh\n",   ThermEnString, Inputs->ThermalEnables.RaplLim1Pwr);
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RaplPwrFlCh[%u]  : %Xh\n", ThermEnString, Channel, Inputs->ThermalEnables.RaplPwrFl[Channel]);
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WarmThresholdCh[%u]Dimm[%u] : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.WarmThreshold[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.HotThresholdCh[%u]Dimm[%u]  : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.HotThreshold[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WarmBudgetCh[%u]Dimm[%u]    : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.WarmBudget[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.HotBudgetCh[%u]Dimm[%u]     : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.HotBudget[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.IdleEnergyCh[%u]Dimm[%u]    : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.IdleEnergy[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.PdEnergyCh[%u]Dimm[%u]      : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.PdEnergy[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ActEnergyCh[%u]Dimm[%u]     : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.ActEnergy[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.RdEnergyCh[%u]Dimm[%u]      : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.RdEnergy[Channel][Dimm]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.WrEnergyCh[%u]Dimm[%u]      : %Xh\n", ThermEnString, Channel, Dimm, Inputs->ThermalEnables.WrEnergy[Channel][Dimm]);
    }
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.SrefCfgEna      : %Xh\n", ThermEnString, Inputs->ThermalEnables.SrefCfgEna);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.SrefCfgIdleTmr  : %Xh\n", ThermEnString, Inputs->ThermalEnables.SrefCfgIdleTmr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ThrtCkeMinDefeat: %Xh\n", ThermEnString, Inputs->ThermalEnables.ThrtCkeMinDefeat);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ThrtCkeMinTmr   : %Xh\n", ThermEnString, Inputs->ThermalEnables.ThrtCkeMinTmr);
#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ThrtCkeMinDefeatLpddr: %Xh\n", ThermEnString, Inputs->ThermalEnables.ThrtCkeMinDefeatLpddr);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s.ThrtCkeMinTmrLpddr   : %Xh\n", ThermEnString, Inputs->ThermalEnables.ThrtCkeMinTmrLpddr);
  }
#endif // ULT_FLAG

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*****MRC INPUT PARAMS DUMP END*******\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*************************************\n\n");

  return mrcSuccess;
}

/**
  Print the specified memory to the serial message debug port.

  @param[in] Debug - Serial message debug structure.
  @param[in] Start - The starting address to dump.
  @param[in] Size  - The amount of data in bytes to dump.

  @retval Nothing.
**/
void
MrcPrintMemory (
  IN const MrcDebug *const Debug,
  IN const U8 *const       Start,
  IN const U32              Size
  )
{
  const U8  *Address;
  const U8  *End;
  U32       Line;
  U32       Offset;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "          ");
  for (Offset = 0; Offset < 16; Offset++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%02X ", ((U32) Start + Offset) % 16);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  End = Start + Size;
  for (Line = 0; Line < ((Size / 16) + 1); Line++) {
    Address = Start + (Line * 16);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 8X: ", Address);
    for (Offset = 0; Offset < 16; Offset++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, ((Address + Offset) < End) ? "%02X " : "   ", Address[Offset]);
    }
    for (Offset = 0; (Offset < 16) && ((Address + Offset) < End); Offset++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%c", isprint (Address[Offset]) ? Address[Offset] : '.');
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  }
  return;
}
#endif

#if (defined MEMORY_DOWN_SUPPORT && (MEMORY_DOWN_SUPPORT > 0))

/******************************************************************************
  Memory down configuration code starts here.
  Add SPD, and channel/slot population settings here.

  Even though this is a memory down configuration, the MRC needs to know how
  the memory appears to the controller, so indicate here which channels are
  populated. Also, the MRC needs to know which slots are valid, even though
  there are technically no physical slots in a memory down configuration.
  The MRC also needs a valid SPD data for the configuration.
******************************************************************************/
//[-start-130314-IB06720210-remove]//
//typedef enum {
//  MEMORY_ABSENT,    ///< No memory down and no physical memory slot.
//  MEMORY_SLOT_ONLY, ///< No memory down and a physical memory slot.
//  MEMORY_DOWN_ONLY, ///< Memory down and not a physical memory slot.
//} MemorySlotStatus;
//[-end-130314-IB06720210-remove]//

///
/// Example board support
///
//[-start-120712-IB05330357-modify]//
#define EXAMPLE_BOARD_SUPPORT 1
//[-end-120712-IB05330357-modify]//
#if (defined EXAMPLE_BOARD_SUPPORT && (EXAMPLE_BOARD_SUPPORT > 0))
///
/// For this example board, we have a dual channel, single slot configuration
/// with the same memory configuration in each channel (DDR3).
///
const MemorySlotStatus ExampleSlotStatus[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL] = {
  MEMORY_DOWN_ONLY,                     ///< Channel 0, Slot 0
  MEMORY_ABSENT,                        ///< Channel 0, Slot 1
  MEMORY_DOWN_ONLY,                     ///< Channel 1, Slot 0
  MEMORY_ABSENT,                        ///< Channel 1, Slot 1
};


//[-start-120813-IB05330369-modify]//
const U8 ExampleSpd[] = {
  0x91,                                 ///< 0   Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage 1, 2
  0x20,                                 ///< 1   SPD Revision
  0xF1,                                 ///< 2   DRAM Device Type
  0x03,                                 ///< 3   Module Type
  0x04,                                 ///< 4   SDRAM Density and Banks
  0x11,                                 ///< 5   SDRAM Addressing
  0x05,                                 ///< 6   Module Nominal Voltage
  0x0B,                                 ///< 7   Module Organization
  0x03,                                 ///< 8   Module Memory Bus Width
  0x11,                                 ///< 9   Fine Timebase (FTB) Dividend / Divisor
  0x01,                                 ///< 10  Medium Timebase (MTB) Dividend
  0x08,                                 ///< 11  Medium Timebase (MTB) Divisor
  0x0A,                                 ///< 12  SDRAM Minimum Cycle Time (tCKmin)
  0x00,                                 ///< 13  Reserved0
  0x00,                                 ///< 14  CAS Latencies Supported, Least Significant Byte
  0x01,                                 ///< 15  CAS Latencies Supported, Most Significant Byte
  0x78,                                 ///< 16  Minimum CAS Latency Time (tAAmin)
  0x78,                                 ///< 17  Minimum Write Recovery Time (tWRmin)
  0x90,                                 ///< 18  Minimum RAS# to CAS# Delay Time (tRCDmin)
  0x50,                                 ///< 19  Minimum Row Active to Row Active Delay Time (tRRDmin)
  0x90,                                 ///< 20  Minimum Row Precharge Delay Time (tRPmin)
  0x11,                                 ///< 21  Upper Nibbles for tRAS and tRC
  0x50,                                 ///< 22  Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
  0xE0,                                 ///< 23  Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
  0x10,                                 ///< 24  Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
  0x04,                                 ///< 25  Minimum Refresh Recovery Delay Time (tRFCmin), Most Significant Byte
  0x3C,                                 ///< 26  Minimum Internal Write to Read Command Delay Time (tWTRmin)
  0x3C,                                 ///< 27  Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
  0x01,                                 ///< 28  Upper Nibble for tFAW
  0x90,                                 ///< 29  Minimum Four Activate Window Delay Time (tFAWmin)
  0x00,                                 ///< 30  SDRAM Optional Features
  0x00,                                 ///< 31  SDRAMThermalAndRefreshOptions
  0x00,                                 ///< 32  ModuleThermalSensor
  0x00,                                 ///< 33  SDRAM Device Type
  0x00,                                 ///< 34  Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  0x00,                                 ///< 35  Fine Offset for Minimum CAS Latency Time (tAAmin)
  0x00,                                 ///< 36  Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  0x00,                                 ///< 37  Fine Offset for Minimum Row Precharge Delay Time (tRPmin)
  0x00,                                 ///< 38  Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
  0xA8,                                 ///< 39
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 40 - 49
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 50 - 59
  0, 0,                                 ///< 60 - 61
  0x00,                                 ///< 62  Reference Raw Card Used
  0x00,                                 ///< 63  Address Mapping from Edge Connector to DRAM
  0x00,                                 ///< 64  ThermalHeatSpreaderSolution
  0, 0, 0, 0, 0,                        ///< 65 - 69
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 70 - 79
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 80 - 89
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 90 - 99
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 100 - 109
  0, 0, 0, 0, 0, 0, 0,                  ///< 110 - 116
  0x00,                                 ///< 117 Module Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 118 Module Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 119 Module Manufacturing Location
  0x00,                                 ///< 120 Module Manufacturing Date Year
  0x00,                                 ///< 121 Module Manufacturing Date creation work week
  0x00,                                 ///< 122 Module Serial Number A
  0x00,                                 ///< 123 Module Serial Number B
  0x00,                                 ///< 124 Module Serial Number C
  0x00,                                 ///< 125 Module Serial Number D
  0x00,                                 ///< 126 CRC A
  0x00                                  ///< 127 CRC B
};
//[-end-120813-IB05330369-modify]//
#endif // EXAMPLE_BOARD_SUPPORT


//[-start-130326-IB06720210-add]//
/**
  Chipset need to create gOemSpdDataGuid HOB and gSlotStatusGuid HOB 
  for SmbiosMemory driver to present memory information.

  HOB structure "CUSTOMER_SPD_DATA" is filled with channel number, DIMMs number in a channel and 
  customized SPD data. If gOemSpdDataGuid HOB doesn't be created here, SmbiosMemory driver doesn't overwrite  
  any SPD data.
  
  HOB structure "SLOT_STATUS_INFO" is filled with max. channel number, max. DIMMs number in a channel and 
  all slot status. The slot status only need to be filled NOT_PRESENT, When a slot is absent. 
  If gSlotStatusGuid HOB doesn't be created here, SmbiosMemory driver will present 
  all four slot information.

  @param          OemSpdData[MAX_CHANNEL][MAX_SLOTS] 
                          - Point to a pointer array, each member indicate an OEM SPD data table.
  @param          SlotStatus     
                          - Point to a slotStatus array. 

  @retval - Nothing.
**/
EFI_STATUS
CreatHobforSmbiosMemory (
  const U8                *SpdIn[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL],
  const MemorySlotStatus  *Slot,
  U16                     SpdSize
)
{
  U8        Channel;
  U8        Dimm;
  U8        SpdDataNum;
  U8        SlotIndex;
  U8        CustomerSpdIndex;
  MemorySlotStatus        *TempSlotPtr;
  SLOT_STATUS_INFO        *SlotStatusDataHobPtr;
  CUSTOMER_SPD_DATA       *CustomerSpdDataHobPtr;

  SpdDataNum = 0;
  SlotIndex = 0;
  SlotStatusDataHobPtr  = NULL;
  CustomerSpdDataHobPtr = NULL;
  CustomerSpdIndex = 0;
  TempSlotPtr = (MemorySlotStatus *)Slot;
  
  if (Slot == NULL || Slot == NULL || Slot == 0) {
    return EFI_UNSUPPORTED;
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++, TempSlotPtr++) {
      if (((*TempSlotPtr) == MEMORY_DOWN_ONLY) && ( SpdIn[Channel][Dimm] != NULL)){
        SpdDataNum ++;
      }  
    }
  }
  
  TempSlotPtr = (MemorySlotStatus *)Slot;  
  SlotStatusDataHobPtr = BuildGuidHob (&gSlotStatusGuid, sizeof(SLOT_STATUS_INFO) - sizeof (UINT8) + (MAX_CHANNEL * MAX_DIMMS_IN_CHANNEL));
  SlotStatusDataHobPtr->MaxChannel        = MAX_CHANNEL;
  SlotStatusDataHobPtr->MaxDimmsInChannel = MAX_DIMMS_IN_CHANNEL;

  CustomerSpdDataHobPtr = BuildGuidHob (&gOemSpdDataGuid, SpdDataNum * sizeof(CUSTOMER_SPD_DATA));

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++, TempSlotPtr++) {
      switch (*TempSlotPtr) {
      case MEMORY_DOWN_ONLY:
        if (SpdIn[Channel][Dimm] != NULL) {
          CustomerSpdDataHobPtr[CustomerSpdIndex].Channel = Channel;
          CustomerSpdDataHobPtr[CustomerSpdIndex].Dimm    = Dimm;
          ZeroMem(CustomerSpdDataHobPtr[CustomerSpdIndex].SpdData, MAX_SPD_DATA_SIZE);
          CopyMem (CustomerSpdDataHobPtr[CustomerSpdIndex].SpdData, (U8 *) SpdIn[Channel][Dimm], SpdSize);
          CustomerSpdIndex ++;
        }
        break;

      case MEMORY_ABSENT:
        SlotStatusDataHobPtr->SlotStatus[SlotIndex] = NOT_PRESENT;
        break;

      case MEMORY_SLOT_ONLY:
      default:
        break;
      }
      SlotIndex++;
    }
  }

  TempSlotPtr = (MemorySlotStatus *)Slot;
  return EFI_SUCCESS;
}
//[-end-130121-IB06720210-add]//


/**
  Copies information from the Memory Down SPD structure to the SPD Input structure
  in the Host structure.

  Setting the SpdBaseAddress to zero means this slot has a memory down configuration.
  For systems that have both memory down and slots, it is recommended to have the
  memory down in the slot position farthest from the controller.

  @param[in, out] Inputs  - MRC Host Input structure.
  @param[in]      SpdIn   - Pointer to the Memory Down SPD structure to copy.
  @param[in]      Slot    - Pointer to the Memory Down MemorySlotStatus structure.
  @param[in]      SpdSize - Size of the SPD structure to limit MemoryCpy.

  @retval - Nothing.
**/
void
CopyMemoryDownSpd (
  IN OUT MrcInput *const         Inputs,
  IN     const U8                *SpdIn[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL],
  IN     const MemorySlotStatus  *Slot,
  IN     U16                     SpdSize
  )
{
  MrcDimmIn *DimmIn;
  U8        Channel;
  U8        Dimm;

  if (SpdIn == NULL || Slot == NULL || SpdSize == 0) {
    return;
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++, Slot++) {
      DimmIn = &Inputs->Controller[0].Channel[Channel].Dimm[Dimm];
      switch (*Slot) {
      case MEMORY_DOWN_ONLY:
        DimmIn->SpdAddress  = 0;
        // Check user request to disable DIMM/rank pair.
        if (DimmIn->Status != DIMM_DISABLED) {
          DimmIn->Status = DIMM_ENABLED;
        MrcOemMemoryCpy ((U8 *) &DimmIn->Spd, (U8 *) SpdIn[Channel][Dimm], SpdSize);
        MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Memory down present on channel %u, dimm %u\n", Channel, Dimm);
        } else {
          MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Memory down absent on channel %u, dimm %u\n", Channel, Dimm);
        }
        break;

      case MEMORY_ABSENT:
        DimmIn->Status      = DIMM_DISABLED;
        DimmIn->SpdAddress  = 0;
        MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Memory down absent on channel %u, dimm %u\n", Channel, Dimm);
        break;

      case MEMORY_SLOT_ONLY:
      default:
        break;
      }
    }
  }

  return;
}

/**
  Enables Memory Down support and sets SPD data for all DIMMs needing support.

  @param[in] Inputs  - MRC Input data structure.
  @param[in] BoardId - The ID of the board.

  @retval Nothing
**/
void
EnableMemoryDown (
  IN MrcInput *const Inputs,
  IN U16             BoardId
  )
{
  const U8                *SpdIn[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  const MemorySlotStatus  *Slot;
  U16                     SpdSize;
  U8                      Channel;
  U8                      Dimm;
//[-start-130326-IB06720210-add]//
  EFI_STATUS              Status;
//[-end-130326-IB06720210-add]//

  Channel = 0;
  Dimm    = 0;
  SpdSize = 0;
//[-start-130121-IB06720210-add]//
  Slot   = NULL;
//[-end-130121-IB06720210-add]//

  switch (BoardId) {
#if (defined EXAMPLE_BOARD_SUPPORT && (EXAMPLE_BOARD_SUPPORT > 0))

//[-start-120712-IB05330357-modify]//
  case V_EC_GET_CRB_BOARD_ID_BOARD_ID_SAWTOOTH_PEAK:
//[-end-120712-IB05330357-modify]//
    //
    // BoardIdExample:
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        SpdIn[Channel][Dimm] = ExampleSpd;
      }
    }

    Slot = (const MemorySlotStatus *) &ExampleSlotStatus[0][0];
    SpdSize = sizeof(ExampleSpd);
    break;
#endif // EXAMPLE_BOARD_SUPPORT


  //
  // Add additional boards that support memory down here.
  //

  //
  // The default case means the board ID was not recognized.  Instead
  // we set Slot = NULL thus forcing us to read from the SPD.
  //
  default:
    Slot  = NULL;
  }
//[-start-130121-IB06720210-add]//
  Status = OemSpdDataAndSlotStatus (&SpdIn[0][0], &Slot, &SpdSize);

  if (!EFI_ERROR(Status)) {
    return;
  }
  
  CreatHobforSmbiosMemory (SpdIn, Slot, SpdSize);
//[-end-130121-IB06720210-add]//

  CopyMemoryDownSpd (Inputs, SpdIn, Slot, SpdSize);

  return;
}
#endif // MEMORY_DOWN_SUPPORT

#ifdef ULT_FLAG
/**
  Initialize the board-specific CMD/CTL/CLK and DQ/DQS mapping for LPDDR3.

  @param[in, out] Inputs  - MRC Input data structure.
  @param[in]      BoardId - The ID of the board.

  @retval none
**/
void
MrcOemLpddrBoardMapping (
  IN OUT  MrcInput *Inputs,
  IN      U16      BoardId
  )
{
  MrcControllerIn *ControllerIn;
  U8              *DqByteMapCh0;
  U8              *DqByteMapCh1;
  U8              *DqsMapCpu2DramCh0;
  U8              *DqsMapCpu2DramCh1;
  U8              *DqMapCpu2DramCh0;
  U8              *DqMapCpu2DramCh1;
  U32             Channel;
  U32             Byte;


  ControllerIn = &Inputs->Controller[0];
  DqByteMapCh0      = NULL;
  DqByteMapCh1      = NULL;
  DqsMapCpu2DramCh0 = NULL;
  DqsMapCpu2DramCh1 = NULL;
  DqMapCpu2DramCh0  = NULL;
  DqMapCpu2DramCh1  = NULL;

  //
  // CKE to Rank mapping:       CKE  | 0 1 2 3
  // (same on both channels)    --------------
  //                            Rank | 0 1 0 1
  //
  Inputs->CkeRankMapping = 0xAA;

  //
  // @todo: pass these via SaPlatformPolicy PPI
  //
  DqByteMapCh0      = (U8 *) DqByteMapRvpCh0;
  DqByteMapCh1      = (U8 *) DqByteMapRvpCh1;
  DqsMapCpu2DramCh0 = (U8 *) DqsMapCpu2DramRvpCh0;
  DqsMapCpu2DramCh1 = (U8 *) DqsMapCpu2DramRvpCh1;
  DqMapCpu2DramCh0  = (U8 *) DqMapCpu2DramRvpCh0;
  DqMapCpu2DramCh1  = (U8 *) DqMapCpu2DramRvpCh1;


  //
  // DQ byte mapping to CMD/CTL/CLK
  //
  MrcOemMemoryCpy ((U8 *) ControllerIn->Channel[0].DQByteMap, DqByteMapCh0, sizeof (DqByteMapRvpCh0));
  MrcOemMemoryCpy ((U8 *) ControllerIn->Channel[1].DQByteMap, DqByteMapCh1, sizeof (DqByteMapRvpCh1));

  //
  // DQS byte swizzling between CPU and DRAM
  //
  MrcOemMemoryCpy (ControllerIn->Channel[0].DqsMapCpu2Dram, DqsMapCpu2DramCh0, sizeof (DqsMapCpu2DramRvpCh0));
  MrcOemMemoryCpy (ControllerIn->Channel[1].DqsMapCpu2Dram, DqsMapCpu2DramCh1, sizeof (DqsMapCpu2DramRvpCh1));

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_ERROR, "Ch %d DqsMapCpu2Dram: ", Channel);
    for (Byte = 0; Byte < 8; Byte++) {
      MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_ERROR, "%d ", ControllerIn->Channel[Channel].DqsMapCpu2Dram[Byte]);
    }
    MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_ERROR, "\n");
  }

  //
  // DQ bit swizzling between CPU and DRAM
  //
  MrcOemMemoryCpy ((U8 *) ControllerIn->Channel[0].DqMapCpu2Dram, DqMapCpu2DramCh0, sizeof (DqMapCpu2DramRvpCh0));
  MrcOemMemoryCpy ((U8 *) ControllerIn->Channel[1].DqMapCpu2Dram, DqMapCpu2DramCh1, sizeof (DqMapCpu2DramRvpCh0));
}
#endif // ULT_FLAG
