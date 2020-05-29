/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Trt.asl

Abstract:

  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

--*/

Scope(\_SB.IETM)
{

    Name(TRTI, Package()    // TRT package when TFN1 is at 0% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                28,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           28,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                03,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })
 
    Name(TRTH, Package()    // TRT package when TFN1 is at 15% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTG, Package()    // TRT package when TFN1 is at 20% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTF, Package()    // TRT package when TFN1 is at 25% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTE, Package()    // TRT package when TFN1 is at 30% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTD, Package()    // TRT package when TFN1 is at 35% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTC, Package()    // TRT package when TFN1 is at 40% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTB, Package()    // TRT package when TFN1 is at 45% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

   Name(TRTA, Package()    // TRT package when TFN1 is at 50% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                36,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           36,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT9, Package()    // TRT package when TFN1 is at 55% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                34,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           34,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT8, Package()    // TRT package when TFN1 is at 60% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                32,     50,   8, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           32,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT7, Package()    // TRT package when TFN1 is at 65% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                30,     50,   7, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           30,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT6, Package()    // TRT package when TFN1 is at 70% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                28,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           28,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT5, Package()    // TRT package when TFN1 is at 75% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                26,     50,   5, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           26,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT4, Package()    // TRT package when TFN1 is at 80% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                24,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           24,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT3, Package()    // TRT package when TFN1 is at 85% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                22,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           22,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT2, Package()    // TRT package when TFN1 is at 90% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                20,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           21,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT1, Package()    // TRT package when TFN1 is at 95% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                18,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           20,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })

    Name(TRT0, Package()    // TRT package when TFN1 is at 100% speed
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     20,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.TCPU,                16,     50,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.TMEM,           10,    100,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.TCPU,            \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           18,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TMEM,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.T_VR, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TEFN, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.TCPU,                 3,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           30,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.TMEM,           44,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.T_VR,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TEFN,  1,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.TMEM,            20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.TCPU,                 20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TSKN,  20,   50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.WWAN,            20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.WWAN,            20,   50,   0, 0, 0, 0}
    })     

    // TRTR (Thermal Relationship Table Revision)
    //
    // This object evaluates to an integer value that defines the revision of the _TRT object. 
    //
    // Arguments: (0)
    //   None
    // Return Value:
    // 0: Traditional TRT as defined by the ACPI Specification.
    // 1: Priority based TRT
    //
    Method(TRTR)
    {
      Return(TRTV)
    }

    // _TRT (Thermal Relationship Table)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Thermal Relationship Packages as described below.
    //
    // Return Value Information
    //   Package {
    //   ThermalRelationship[0] // Package
    //    ...
    //   ThermalRelationship[n] // Package
    //   }
    //
    Method(_TRT,,,,PkgObj)
    {
      If(\ECON)
      {
        If(\_SB.PCI0.LPCB.H_EC.CFAN)    // CFAN Holds On/Off information
        {
          If(LEqual(\_SB.PCI0.LPCB.H_EC.PENV, 0))  // PENV holds duty cycle in percent
          {
            Return(TRTI)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 15))  // PENV holds duty cycle in percent
          {
            Return(TRTH)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 20))
          {
            Return(TRTG)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 25))
          {
            Return(TRTF)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 30))
          {
            Return(TRTE)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 35))
          {
            Return(TRTD)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 40))
          {
            Return(TRTC)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 45))
          {
            Return(TRTB)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 50))
          {
            Return(TRTA)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 55))
          {
            Return(TRT9)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 60))
          {
            Return(TRT8)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 65))
          {
            Return(TRT7)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 70))
          {
            Return(TRT6)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 75))
          {
            Return(TRT5)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 80))
          {
            Return(TRT4)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 85))
          {
            Return(TRT3)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 90))
          {
            Return(TRT2)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 95))
          {
            Return(TRT1)
          }

          If(LLessEqual(\_SB.PCI0.LPCB.H_EC.PENV, 100))
          {
            Return(TRT0)
          }
        }  
      }

      Return(TRTI)  // Return 0% State TRT by default
    } // End _TRT 
}
