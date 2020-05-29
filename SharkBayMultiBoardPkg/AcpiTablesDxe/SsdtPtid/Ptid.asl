/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/


//
// Defined as an SSDT to be able to dynamically load based on BIOS
// setup options
// 
DefinitionBlock (
  "PTID.aml",
  "SSDT",
  0x01,
  "TrmRef",
  "PtidDevc",
  0x1000
  )
  

{
  Scope(\_SB)
  { 
    // External References to the actual data locations that stores
    // various temperature and power values (either from EC or by
    // other means)
    //

    // Embedded Controller Availability Flag
    External(\_SB.PCI0.LPCB.H_EC.ECAV, IntObj)
    // Board id checks
    External(\BID, IntObj)
    External(\BHB, IntObj)
    External(\BFS2, IntObj)
    External(\BFS3, IntObj)
    External(\BFS4, IntObj)
    
    // externs for TSDD
    External(DTS1)  // DTS Core 0 Temp
    External(DTS2)  // DTS Core 1 Temp
    External(DTS3)  // DTS Core 2 Temp
    External(DTS4)  // DTS Core 3 Temp
    External(PDTS)  // Package DTS Core Temp
    External(\_SB.PCI0.LPCB.H_EC.CVRT)    // CPU Core VR (IMVP) Temperature
    External(\_SB.PCI0.LPCB.H_EC.GTVR)    // CPU GT VR (IMVP) Temperature
    External(\_SB.PCI0.LPCB.H_EC.FANT)    // Heat Exchanger Fan Temperature
    External(\_SB.PCI0.LPCB.H_EC.SKNT)    // Skin Temperature
    External(\_SB.PCI0.LPCB.H_EC.AMBT)    // Ambient Temperature
    External(\_SB.PCI0.LPCB.H_EC.DIM0)    // Channel 0 DIMM Temperature
    External(\_SB.PCI0.LPCB.H_EC.DIM1)    // Channel 1 DIMM Temperature     
    External(\_SB.PCI0.LPCB.H_EC.PMAX)    // CPU, MCH & PCH Max Temperature
    External(\_SB.PCI0.LPCB.H_EC.PPDT)    // PCH DTS Temperature from PCH
    External(\_SB.PCI0.LPCB.H_EC.PECH)    // CPU PECI reading
    External(\_SB.PCI0.LPCB.H_EC.PMDT)    // MCH DTS Temperature from PCH
    External(\_SB.PCI0.LPCB.H_EC.TSD0)    // TS-on-DIMM0 Temperature
    External(\_SB.PCI0.LPCB.H_EC.TSD1)    // TS-on-DIMM1 Temperature
    External(\_SB.PCI0.LPCB.H_EC.TSD2)    // TS-on-DIMM2 Temperature
    External(\_SB.PCI0.LPCB.H_EC.TSD3)    // TS-on-DIMM3 Temperature
    External(\_TZ.TZ00._TMP)              // Thermal Zone 00 Temperature
    External(\_TZ.TZ01._TMP)              // Thermal Zone 01 Temperature

    // externs for PSDD
    External(\_SB.PCI0.LPCB.H_EC.CPUP)    // Platform Power mW
    External(\_SB.PCI0.LPCB.H_EC.BPWR)    // Brick Power cW(100ths)
    External(\_SB.PCI0.LPCB.H_EC.PPWR)    // Platform Power cW(100ths)
    External(\_SB.PCI0.LPCB.H_EC.CPAP)    // Platform Average Power mW
    External(\_SB.PCI0.LPCB.H_EC.BKAP)    // Brick average power in cW(0.01)
    External(\_SB.PCI0.LPCB.H_EC.PLAP)    // Platform average power in cW(0.01)
    External(\_SB.PCI0.LPCB.H_EC.B1DC)    // Battery 1 Design Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1RC)    // Battery 1 Remaining Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1FC)    // Battery 1 Full Charge Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1FV)    // Battery 1 Full Resolution Voltage (mV)
    External(\_SB.PCI0.LPCB.H_EC.B1DI)    // Battery 1 Full Resolution Discharge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B1CI)    // Battery 1 Full Resolution Charge Current (mA)    
    External(\_SB.PCI0.LPCB.H_EC.B2RC)    // Battery 2 Remaining Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B2FC)    // Battery 2 Full Charge Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B2FV)    // Battery 2 Full Resolution Voltage (mV)
    External(\_SB.PCI0.LPCB.H_EC.B2DI)    // Battery 2 Full Resolution Discharge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B2CI)    // Battery 2 Full Resolution Charge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B1ML)    // Battery Pack A maximum low byte
    External(\_SB.PCI0.LPCB.H_EC.B1MH)    // Battery Pack A maximum high byte
    External(\_SB.PCI0.LPCB.H_EC.B2ML)    // Battery Pack B maximum low byte
    External(\_SB.PCI0.LPCB.H_EC.B2MH)    // Battery Pack B maximum high byte

    // externs for OSDD
    External(\_SB.PCI0.LPCB.H_EC.CFSP)    // CPU Fan #1 speed 
    External(\_SB.PCI0.LPCB.H_EC.CFS2)    // CPU Fan #2 speed 
    External(\_SB.PCI0.LPCB.H_EC.MFSP)    // GMCH Fan speed 
    External(\_SB.PCI0.LPCB.H_EC.PAKN)    // Packet sequence number
    External(\_SB.PCI0.LPCB.H_EC.CPUE)    // CPU energy 
    // STS - SMBus Turbo Status (48 bits) from EC name space
    External(\_SB.PCI0.LPCB.H_EC.SMTL)    // MCP Temperature Limit(MTL)
    External(\_SB.PCI0.LPCB.H_EC.SGTD)    // GMCH Turbo Disabled(GTD)
    External(\_SB.PCI0.LPCB.H_EC.SCTD)    // CPU Turbo Disabled(CTD)
    External(\_SB.PCI0.LPCB.H_EC.SPOL)    // Policy Preference(PP)
    External(\_SB.PCI0.LPCB.H_EC.SGPL)    // GMCH Power Limit(GPL)
    External(\_SB.PCI0.LPCB.H_EC.SCPL)    // CPU Power Limit(CPL)
    External(\_SB.PCI0.LPCB.H_EC.SMPL)    // MPC Power Limit(MPL)
    // HTS - Host Turbo Status(TBARB offset 50h) from MMIO 
    External(\_SB.PCI0.LPCB.H_EC.MMTL)    // MCP Temperature Limit(MTL)
    External(\_SB.PCI0.LPCB.H_EC.MGTD)    // GMCH Turbo Disabled(GTD)
    External(\_SB.PCI0.LPCB.H_EC.MCTD)    // CPU Turbo Disabled(CTD)
    External(\_SB.PCI0.LPCB.H_EC.MPOL)    // Policy Preference(PP)
    External(\_SB.PCI0.LPCB.H_EC.MGPL)    // GMCH Power Limit(GPL)
    External(\_SB.PCI0.LPCB.H_EC.MCPL)    // CPU Power Limit(CPL)
    External(\_SB.PCI0.LPCB.H_EC.MMPL)    // MPC Power Limit(MPL)
    // STS - SMBus Turbo Status(TBARB offset 98h) from MMIO
    External(\_SB.PCI0.LPCB.H_EC.NMTL)    // MCP Temperature Limit(MTL)
    External(\_SB.PCI0.LPCB.H_EC.NGTD)    // GMCH Turbo Disabled(GTD)
    External(\_SB.PCI0.LPCB.H_EC.NCTD)    // CPU Turbo Disabled(CTD)
    External(\_SB.PCI0.LPCB.H_EC.NPOL)    // Policy Preference(PP)
    External(\_SB.PCI0.LPCB.H_EC.NGPL)    // GMCH Power Limit(GPL)
    External(\_SB.PCI0.LPCB.H_EC.NCPL)    // CPU Power Limit(CPL)
    // TC1 - Thermal Compares 1(TBARB offset A8h) from MMIO
    External(\_SB.PCI0.LPCB.H_EC.TMPL)    // MPC Power Limit(MPL)
    // HTS - Host Turbo Status (48 bits) from EC name space
    External(\_SB.PCI0.LPCB.H_EC.HMTL)
    External(\_SB.PCI0.LPCB.H_EC.HGTD)
    External(\_SB.PCI0.LPCB.H_EC.HCTD)
    External(\_SB.PCI0.LPCB.H_EC.HPOL)
    External(\_SB.PCI0.LPCB.H_EC.HGPL)
    External(\_SB.PCI0.LPCB.H_EC.HCPL)
    External(\_SB.PCI0.LPCB.H_EC.HMPL)
    // Thermal Device Base Address Register for BIOS use.
    External(TBAB)
    // EC Data Buffer for ME SMBus Write Commands
    External(\_SB.PCI0.LPCB.H_EC.MPCT)
    External(\_SB.PCI0.LPCB.H_EC.MDT0)
    // EC Command register
    External(\_SB.PCI0.LPCB.H_EC.CMDR)
    // EC Buffer to for ACPIPECIPacket
    External(\_SB.PCI0.LPCB.H_EC.PRCL)
    External(\_SB.PCI0.LPCB.H_EC.PRC0)
    External(\_SB.PCI0.LPCB.H_EC.PRC1)
    External(\_SB.PCI0.LPCB.H_EC.PRCM)
    External(\_SB.PCI0.LPCB.H_EC.PRIN)
    External(\_SB.PCI0.LPCB.H_EC.PSTE)
    External(\_SB.PCI0.LPCB.H_EC.PCAD)
    External(\_SB.PCI0.LPCB.H_EC.PEWL)
    External(\_SB.PCI0.LPCB.H_EC.PWRL)
    External(\_SB.PCI0.LPCB.H_EC.PECD)
    External(\_SB.PCI0.LPCB.H_EC.PEHI)
    External(\_SB.PCI0.LPCB.H_EC.PECI)
    External(\_SB.PCI0.LPCB.H_EC.PEPL)
    External(\_SB.PCI0.LPCB.H_EC.PEPM)
    External(\_SB.PCI0.LPCB.H_EC.PWFC)
    External(\_SB.PCI0.LPCB.H_EC.PECC)
    External(\_SB.PCI0.LPCB.H_EC.PDT0)
    External(\_SB.PCI0.LPCB.H_EC.PDT1)
    External(\_SB.PCI0.LPCB.H_EC.PDT2)
    External(\_SB.PCI0.LPCB.H_EC.PDT3)
    External(\_SB.PCI0.LPCB.H_EC.PRFC)
    External(\_SB.PCI0.LPCB.H_EC.PRS0)
    External(\_SB.PCI0.LPCB.H_EC.PRS1)
    External(\_SB.PCI0.LPCB.H_EC.PRS2)
    External(\_SB.PCI0.LPCB.H_EC.PRS3)
    External(\_SB.PCI0.LPCB.H_EC.PRS4)
    External(\_SB.PCI0.LPCB.H_EC.PRCS)
    External(\_SB.PCI0.LPCB.H_EC.PEC0)
    External(\_SB.PCI0.LPCB.H_EC.PEC1)
    External(\_SB.PCI0.LPCB.H_EC.PEC2)
    External(\_SB.PCI0.LPCB.H_EC.PEC3)
    External(\_SB.PCI0.LPCB.H_EC.TER1)
    External(\_SB.PCI0.LPCB.H_EC.TER2)

    External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj)
    External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)
    External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)

    External(\PAMT)


    Device(PTID)
    {
      Name(_HID, EISAID("INT340E")) 
      Name(_CID, EISAID("PNP0C02"))
    
      Name(IVER, 0x00030000)      // Version 3

                 Method (_STA)
                 {
                   Return (0x0F)
                 }
            
      Name(TSDL, Package()
      { //Device Class  //Name of Temperature Value
        0x00000000, "CPU Core 0 DTS", //not from ec
        0x00000000, "CPU Core 1 DTS", //not from ec
        0x00000000, "CPU Core 2 DTS", //not from ec
        0x00000000, "CPU Core 3 DTS", //not from ec
        0x00000000, "CPU Core Package DTS", //not from ec
        0x00000003, "CPU Core VR (IMVP) Temperature",
        0x00000003, "Heat Exchanger Fan Temperature",
        0x00000003, "Skin Temperature",
        0x00000003, "Ambient Temperature",
        0x00000002, "Channel 0 DIMM Temperature",
        0x00000002, "Channel 1 DIMM Temperature",
        0x00000000, "CPU Package Temperature",
        0x00000005, "PCH DTS Temperature from PCH",
        0x00000000, "CPU PECI reading",
        0x00000005, "SA DTS Temperature from PCH",
        0x00000002, "TS-on-DIMM0 Temperature",
        0x00000002, "TS-on-DIMM1 Temperature",
        0x00000002, "TS-on-DIMM2 Temperature",
        0x00000002, "TS-on-DIMM3 Temperature",
        0x00000002, "TZ00 _TMP", // not from EC
        0x00000002, "TZ01 _TMP" // not from EC
      })

      Name(PSDL, Package()
      { // Device Class   //Name of Power Value
        0x0000000B,       "Platform Power (mW)",
        0x0000000B,       "Brick Power cW(100ths)",
        0x0000000B,       "Battery Discharge Power cW(100ths)",
        0x0000000B,       "Platform Average Power (mW)",
        0x0000000B,       "Brick Average Power cW(0.01)",
        0x0000000B,       "Battery Discharge Average Power cW(0.01)",
        0x0000000C,       "Battery 1 Design Capacity (mWh)",
        0x0000000C,       "Battery 1 Remaining Capacity (mWh)",
        0x0000000C,       "Battery 1 Full Charge Capacity (mWh)",
        0x0000000C,       "Battery 1 Full Resolution Voltage (mV)",
        0x0000000C,       "Battery 1 Full Resolution Discharge Current (mA)",
        0x0000000C,       "Battery 1 Full Resolution Charge Current (mA)",
        0x0000000C,       "Battery 2 Remaining Capacity (mWh)",
        0x0000000C,       "Battery 2 Full Charge Capacity (mWh)",
        0x0000000C,       "Battery 2 Full Resolution Voltage (mV)",
        0x0000000C,       "Battery 2 Full Resolution Discharge Current (mA)",
        0x0000000C,       "Battery 2 Full Resolution Charge Current (mA)",
        0x0000000C,       "Battery Pack 1 maximum power (mW)",
        0x0000000C,       "Battery Pack 2 maximum power (mW)"
      })

      Name(OSDL, Package()
      { // Device Class   // Descriptive Name   //Unit
        0x00000000, "CPU Fan #1 Speed", "RPM",
        0x00000000, "CPU Fan #2 Speed", "RPM",
        0x00000003, "Skin Temp 0", "RAW",
        0x00000003, "Thermistor 1 ", "RAW",
        0x00000003, "Thermistor 2 ", "RAW",
      })

      Method(TSDD, 0)
      {             
                              Name (TMPV, Package() {
                                          0x80000000, 0x80000000,0x80000000, 0x80000000,
                                          0x80000000, 0x80000000,0x80000000, 0x80000000,
                                          0x80000000, 0x80000000,0x80000000, 0x80000000,
                                          0x80000000, 0x80000000,0x80000000, 0x80000000,
                                          0x80000000, 0x80000000,0x80000000, 0x80000000,
                                          0x80000000 })
                                   
        Store(Add(Multiply(DTS1, 10), 2732), Index(TMPV, 0))
        Store(Add(Multiply(DTS2, 10), 2732), Index(TMPV, 1))
        Store(Add(Multiply(DTS3, 10), 2732), Index(TMPV, 2))
        Store(Add(Multiply(DTS4, 10), 2732), Index(TMPV, 3))
        Store(Add(Multiply(PDTS, 10), 2732), Index(TMPV, 4))
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CVRT)), 10), 2732), Index(TMPV, 5))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.FANT)), 10), 2732), Index(TMPV, 6))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.SKNT)), 10), 2732), Index(TMPV, 7))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.AMBT)), 10), 2732), Index(TMPV, 8))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DIM0)), 10), 2732), Index(TMPV, 9))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DIM1)), 10), 2732), Index(TMPV, 10))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PMAX)), 10), 2732), Index(TMPV, 11))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPDT)), 10), 2732), Index(TMPV, 12))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECH)), 10), 2732), Index(TMPV, 13))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PMDT)), 10), 2732), Index(TMPV, 14))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSD0)), 10), 2732), Index(TMPV, 15))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSD1)), 10), 2732), Index(TMPV, 16))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSD2)), 10), 2732), Index(TMPV, 17))
        Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSD3)), 10), 2732), Index(TMPV, 18))
        }
        Store(\_TZ.TZ00._TMP, Index(TMPV, 19))
        If(CondRefOf(\_TZ.TZ01)){
          Store(\_TZ.TZ01._TMP, Index(TMPV, 20))
        }
        Return(TMPV)
      }

      Method(PSDD, 0, Serialized)
      {
        Name (PWRV, Package() {
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000
        })
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
        {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CPUP)), Index(PWRV, 0))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.BPWR)), 10, Index(PWRV, 1))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPWR)), 10, Index(PWRV, 2))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CPAP)), Index(PWRV, 3))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.BKAP)), 10, Index(PWRV, 4))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PLAP)), 10, Index(PWRV, 5))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1DC)), Index(PWRV, 6))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)), Index(PWRV, 7))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)), Index(PWRV, 8))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FV)), Index(PWRV, 9))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1DI)), Index(PWRV, 10))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1CI)), Index(PWRV, 11))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2RC)), Index(PWRV, 12))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2FC)), Index(PWRV, 13))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2FV)), Index(PWRV, 14))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2DI)), Index(PWRV, 15))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2CI)), Index(PWRV, 16))
        // convert PMAX from signed to unsigned integer
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
        Add(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1MH)), 8), \_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1ML)),Local0) // Get high and low byte from EC.
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
        Store(Local0,Index(PWRV, 17))
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
        Add(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2MH)), 8),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2ML)),Local0) // Get high and low byte from EC.
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
         Store(Local0,Index(PWRV, 18))
        }
        Return (PWRV)
      }

      Method(OSDD, 0)
      {
        Name (OSDV, Package () {0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000})
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFSP)), Index(OSDV, 0))
          If(LOr(LOr(LEqual(BID,BHB), LEqual(BID,BFS2)), LOr(LEqual(BID,BFS3), LEqual(BID,BFS4)))) //BoardIdHarrisBeachFfrd, BoardIdFfrdSku2, BoardIdFfrdSku3, BoardIdFfrdSku4
          {
            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFS2)), Index(OSDV, 1))
          }
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.SKNT)), Index(OSDV, 2))
          If(LOr(LOr(LEqual(BID,BHB), LEqual(BID,BFS2)), LOr(LEqual(BID,BFS3), LEqual(BID,BFS4)))) //BoardIdHarrisBeachFfrd, BoardIdFfrdSku2, BoardIdFfrdSku3, BoardIdFfrdSku4
          {
            // Thermistor values are 10 bit but EC only exposes the high 8 bits.  Shift left 2 gives the real number.
            Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER1)),2), Index(OSDV, 3))
            Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER2)),2), Index(OSDV, 4))
          }
        }
        Return(OSDV)
      }

      Method (SDSP)
      {
        Return(10)      // Sampling period .
      }

      // PECI ACPI Access method
      Name(PADA, Package()
      { 
                           0x01,              // Access Mode -ACPI
                           0xFFFFFFFF,        // Command Status port
                           0xFFFFFFFF,        // Data port
                           26,                // MailBox length
                           0xFFFFFFFF,        // WritePECIMailBoxDataCommand 
                           0xFFFFFFFF,        // ReadPECIMailBoxDataCommand
                           0xFFFFFFFF,        // IssuePECIcommand
                           0xFFFFFFFF,        // EndPECICommand
                           0xFFFFFFFF,        // ReadStatusCommand
                           0xFFFFFFFF         // ReadErrorCountCommand
      })

                       // PECI Direct I/O Access method
                        Name(PADD, Package()
                        {
                           0x02,        // Access Mode
                           0x6A4,       // Command Status port
                           0x6A0,       // Data port
                           26,          // MailBox length
                           0x62,        // WritePECIMailBoxDataCommand 
                           0x63,        // ReadPECIMailBoxDataCommand
                           0x60,        // IssuePECIcommand
                           0x61,        // EndPECICommand
                           0x65,        // ReadStatusCommand
                           0x64         // ReadErrorCountCommand
      })

                       Method (PADT) 
                       {
                         If(PAMT) {
                            Return (PADA)
                          }
                         Return (PADD)
                       }
            
                       Method (RPMD)
                       {
                          Name (MTMP, Buffer(26) {})
                            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                            {
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRCL)), Index(MTMP,0))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRC0)), Index(MTMP,1))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRC1)), Index(MTMP,2))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRCM)), Index(MTMP,3))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRIN)), Index(MTMP,4))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PSTE)), Index(MTMP,5))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PCAD)), Index(MTMP,6))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEWL)), Index(MTMP,7))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWRL)), Index(MTMP,8))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECD)), Index(MTMP,9))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEHI)), Index(MTMP,10))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECI)), Index(MTMP,11))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEPL)), Index(MTMP,12))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEPM)), Index(MTMP,13))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWFC)), Index(MTMP,14))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECC)), Index(MTMP,15))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT0)), Index(MTMP,16))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT1)), Index(MTMP,17))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT2)), Index(MTMP,18))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT3)), Index(MTMP,19))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRFC)), Index(MTMP,20))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS0)), Index(MTMP,21))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS1)), Index(MTMP,22))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS2)), Index(MTMP,23))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS3)), Index(MTMP,24))
                            Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS4)), Index(MTMP,25))
                          }
                          Return (MTMP)
                       }

                       Method (WPMD,1)
                       {
                          If (LNotEqual(Sizeof(Arg0), 26)) {
                             Return (0xFFFFFFFF)
                            }

                            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                           {
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,0)), RefOf(\_SB.PCI0.LPCB.H_EC.PRCL))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,1)), RefOf(\_SB.PCI0.LPCB.H_EC.PRC0))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,2)), RefOf(\_SB.PCI0.LPCB.H_EC.PRC1))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,3)), RefOf(\_SB.PCI0.LPCB.H_EC.PRCM))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,4)), RefOf(\_SB.PCI0.LPCB.H_EC.PRIN))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,5)), RefOf(\_SB.PCI0.LPCB.H_EC.PSTE))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,6)), RefOf(\_SB.PCI0.LPCB.H_EC.PCAD))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,7)), RefOf(\_SB.PCI0.LPCB.H_EC.PEWL))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,8)), RefOf(\_SB.PCI0.LPCB.H_EC.PWRL))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,9)), RefOf(\_SB.PCI0.LPCB.H_EC.PECD))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,10)), RefOf(\_SB.PCI0.LPCB.H_EC.PEHI))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,11)), RefOf(\_SB.PCI0.LPCB.H_EC.PECI))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,12)), RefOf(\_SB.PCI0.LPCB.H_EC.PEPL))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,13)), RefOf(\_SB.PCI0.LPCB.H_EC.PEPM))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,14)), RefOf(\_SB.PCI0.LPCB.H_EC.PWFC))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,15)), RefOf(\_SB.PCI0.LPCB.H_EC.PECC))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,16)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT0))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,17)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT1))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,18)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT2))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,19)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT3))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,20)), RefOf(\_SB.PCI0.LPCB.H_EC.PRFC))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,21)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS0))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,22)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS1))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,23)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS2))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,24)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS3))
                            \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,25)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS4))
                            \_SB.PCI0.LPCB.H_EC.ECMD (0x67)
                          }
                          Return (0)
                       }

                       Method (ISPC)
                       {
                         If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                          {                 
                            \_SB.PCI0.LPCB.H_EC.ECMD (0x65)
                          }
                          Return (0)
                       }

                       Method (ENPC)
                       {
                         If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                         {  
                          \_SB.PCI0.LPCB.H_EC.ECMD (0x66)
                         }
                          Return (0)
                       }

                       Method (RPCS)
                       {
                         If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                         {  
                          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRCS)), Local0)
                         }
                        Return (Local0)
                       }   

                       Method (RPEC)
                       {
                        Store(0,Local0)
                         If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // check EC opregion available
                         {  
                          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC0)), Local1)
                          Or(Local0,Local1,Local0)

                          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC1)), Local1)
                          Or(Local0,ShiftLeft(Local1,8),Local0)
 
                          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC2)), Local1)
                          Or(Local0,ShiftLeft(Local1,16),Local0)

                          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC3)), Local1)
                          Or(Local0,ShiftLeft(Local1,24),Local0)
                         }
                        return (Local0)
                       }   

                 }
  } // end \_SB scope
} // end SSDT
