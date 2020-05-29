/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

SCOPE (IGPU_SCOPE)
{
    Method (SSNR, 1)
    {
        Switch (ToInteger (Arg0))
        {
            case (VEN_SENSOR_HEADER_STRUC)
            {
                // Ventura Sensor parameters header structure
                Name (SBHS, Buffer(0x8) {})
                CreateDWordField (SBHS, 0, VERV)
                CreateDWordField (SBHS, 4, NUMS)
  
                // Populate Header Structure
                Store (VEN_VERSION_HEADER, VERV)
                Store (VEN_NUM_SENSORS,    NUMS)
                Return (SBHS)
            }
            case (VEN_SENSOR_CPU_STRUC)
            {
                // Ventura CPU Parameters Structure
                Name (SCPP, Buffer(76) {})
                CreateDWordField (SCPP,  0, VRV1)
                CreateDWordField (SCPP,  4, VCAP)
                CreateDWordField (SCPP,  8, VCCP)
                CreateDWordField (SCPP, 12, VCDP)
                CreateDWordField (SCPP, 16, VCEP)
                CreateDWordField (SCPP, 20, VCGP)
                CreateDWordField (SCPP, 24, VCHP)
                CreateDWordField (SCPP, 28, VCXP)
                CreateDWordField (SCPP, 32, VCYP)
                CreateDWordField (SCPP, 36, VCZP)
                CreateDWordField (SCPP, 40, VCKP)
                CreateDWordField (SCPP, 44, VCMP)
                CreateDWordField (SCPP, 48, VCNP)
                CreateDWordField (SCPP, 52, VCAL)
                CreateDWordField (SCPP, 56, VCBE)
                CreateDWordField (SCPP, 60, VCGA)
                CreateDWordField (SCPP, 64, VCPP)
                CreateDWordField (SCPP, 68, VCDE)
  
                Store (VEN_VERSION_CPU,   VRV1)
                Store (VEN_CPU_PARAM_A,   VCAP)
                Store (VEN_CPU_PARAM_C,   VCCP)
                Store (VEN_CPU_PARAM_D,   VCDP)
                Store (VEN_CPU_PARAM_E,   VCEP)
                Store (VEN_CPU_PARAM_G,   VCGP)
                Store (VEN_CPU_PARAM_H,   VCHP)
                Store (VEN_CPU_PARAM_X,   VCXP)
                Store (VEN_CPU_PARAM_Y,   VCYP)
                Store (VEN_CPU_PARAM_Z,   VCZP)
                Store (VEN_CPU_PARAM_K,   VCKP)
                Store (VEN_CPU_PARAM_M,   VCMP)
                Store (VEN_CPU_PARAM_N,   VCNP)
                Store (VEN_CPU_PARAM_AL,  VCAL)
                Store (VEN_CPU_PARAM_BE,  VCBE)
                Store (VEN_CPU_PARAM_GA,  VCGA)
                Store (VEN_CPU_PARAM_P,   VCPP)
                Store (VEN_CPU_PARAM_DEL, VCDE)
                Return (SCPP)
            }
            case (VEN_SENSOR_GPU_STRUC)
            {
                // Ventura GPU Parameters Structure
                Name (SGPP, Buffer(40) {})
                CreateDWordField (SGPP,  0, VRV2)
                CreateDWordField (SGPP,  4, VGWP)
                CreateDWordField (SGPP,  8, VGPP)
                CreateDWordField (SGPP, 12, VGQP)
                CreateDWordField (SGPP, 16, VGRP)
                CreateDWordField (SGPP, 20, VGAP)
                CreateDWordField (SGPP, 24, VGBP)
                CreateDWordField (SGPP, 28, VGCP)
                CreateDWordField (SGPP, 32, VGDP)
                CreateDWordField (SGPP, 36, VGDE)
  
                Store (VEN_VERSION_GPU, VRV2)
                Store (VEN_GPU_PARAM_W, VGWP)
                Store (VEN_GPU_PARAM_P, VGPP)
                Store (VEN_GPU_PARAM_Q, VGQP)
                Store (VEN_GPU_PARAM_R, VGRP)
                Store (VEN_GPU_PARAM_A, VGAP)
                Store (VEN_GPU_PARAM_B, VGBP)
                Store (VEN_GPU_PARAM_C, VGCP)
                Store (VEN_GPU_PARAM_D, VGDP)
                Store (VEN_GPU_PARAM_DE, VGDE)
                Return (SGPP)
            }
            case (VEN_SENSOR_PARAM_STRUC)
            {
                // Ventura CPU Sensor structure
                Name (SSCP, Buffer(44) {})
                CreateDWordField (SSCP,  4, CSNT)
                CreateDWordField (SSCP,  8, CPTI)
                CreateDWordField (SSCP, 12, CICA)
                CreateDWordField (SSCP, 16, CIRC)
                CreateDWordField (SSCP, 20, CICV)
                CreateDWordField (SSCP, 24, CIRA)
                CreateDWordField (SSCP, 28, CIAV)
                CreateDWordField (SSCP, 32, CIEP)
                CreateDWordField (SSCP, 36, CPPF)
                CreateDWordField (SSCP, 40, CSNR)
  
                // Ventura GPU Sensor structure
                Name (SSGP, Buffer(44) {})
                CreateDWordField (SSGP,  4, GSNT)
                CreateDWordField (SSGP,  8, GPTI)
                CreateDWordField (SSGP, 12, GICA)
                CreateDWordField (SSGP, 16, GIRC)
                CreateDWordField (SSGP, 20, GICV)
                CreateDWordField (SSGP, 24, GIRA)
                CreateDWordField (SSGP, 28, GIAV)
                CreateDWordField (SSGP, 32, GIEP)
                CreateDWordField (SSGP, 36, GPPF)
                CreateDWordField (SSGP, 40, GSNR)
  
                // The below sensor parameter values for GPU and CPU
                // are board specific. To support for ventura, fill
                // the SSCP and SSGP structures
                
                // Populate CPU Sensor values
                Store (0x0, Index(SSCP, 0))  // Indicate CPU sensor
                Store (VENSNS_CPU_SENSOR_TYPE,        CSNT)
                Store (VENSNS_CPU_I2C_PORT,           CPTI)
                Store (VENSNS_CPU_I2C_ADDR,           CICA)
                Store (VENSNS_CPU_INA219_CFG_LOC,     CIRC)
                Store (VENSNS_CPU_INA219_CFG_VALUE,   CICV)
                Store (VENSNS_CPU_INA219_CALIB_LOC,   CIRA)
                Store (VENSNS_CPU_INA219_CALIB_VALUE, CIAV)
                Store (VENSNS_CPU_INA219_POWER_LOC,   CIEP)
                Store (VENSNS_CPU_PMU_POLLING_FREQ,   CPPF)
                Store (VENSNS_CPU_SENSE_RESISTOR,     CSNR)
                
                // Populate GPU Sensor values
                Store (0x1, Index(SSGP, 0))  // Indicate GPU sensor
                Store (VENSNS_GPU_SENSOR_TYPE,        GSNT)
                Store (VENSNS_GPU_I2C_PORT,           GPTI)
                Store (VENSNS_GPU_I2C_ADDR,           GICA)
                Store (VENSNS_GPU_INA219_CFG_LOC,     GIRC)
                Store (VENSNS_GPU_INA219_CFG_VALUE,   GICV)
                Store (VENSNS_GPU_INA219_CALIB_LOC,   GIRA)
                Store (VENSNS_GPU_INA219_CALIB_VALUE, GIAV)
                Store (VENSNS_GPU_INA219_POWER_LOC,   GIEP)
                Store (VENSNS_GPU_PMU_POLLING_FREQ,   GPPF)
                Store (VENSNS_GPU_SENSE_RESISTOR,     GSNR)
                
                Return (Concatenate (SSCP, SSGP))
            }
        }
        Return (STATUS_ERROR_UNSUPPORTED)  
    }

    Name (VSTS, 1)                            // Ventura Status
    Name (TBUD, SPB_VEN_THERMAL_BUDGET)       // Thermal Budget
    Name (PSCP, 0)                            // P-State capacity, mainly for s/w debugging

    Method (SPB, 4, NotSerialized)
    {
        // Only Interface Revision 0x0101 is supported
        If (LNotEqual (Arg1, 0x101))
        {
            Return (STATUS_ERROR_UNSUPPORTED)  // Status Error Unsupported
        }
 
        // (Arg2) Sub-Function
        Switch (ToInteger (Arg2))
        {
            //
            // Function 0: SPB_FUNC_SUPPORT - Bit list of supported functions.
            //
            case (SPB_FUNC_SUPPORT)
            {
                // Sub Function 0, 32, 33, 34, 35, 42 are supported
                Return (Buffer(8) {0x01, 0x00, 0x00, 0x00, 0x0F, 0x04, 0x00, 0x00})
            }
 
            //
            // Function 32: SPB_FUNC_VENTURASTATUS
            //
            case (SPB_FUNC_VENTURASTATUS)
            {
                Store (TBUD, Local1)
                // failsafe to clear ventura status bit
                And (Local1, 0xFFFFF, Local1)
                // Just return SPB status for now (bit[0]=1 SPB enabled)
                If (CondRefOf (DGPU_SCOPE.PBCM, Local0)){  // Make sure this object is present.
                    If (DGPU_SCOPE.PBCM){
                        // Software/EC have another chance to disable ventura through VSTS
                        If (LNotEqual (VSTS, 0)) {
                            Or (Local1, 0x40000000, Local1)
                        }
                    }
                }
                Return (Local1)
            }
 
            //
            // Function 33: SPB_FUNC_GETPSS
            //
            case (SPB_FUNC_GETPSS)
            {
                Return (CPU0_SCOPE._PSS)
            }
 
            //
            // Function 34: SPB_FUNC_SETPPC
            //
            case (SPB_FUNC_SETPPC)
            {
                Name (MCHH, Zero)
                CreateByteField (Arg3, 0, PCAP)
 
                Store (PCAP, PSCP)
 
                If (LNotEqual (PCAP, CPU0_SCOPE._PPC))
                {
                   If (LEqual (CPU0_SCOPE._PPC, Zero))
                   {
                       // Save IGPU Enhanced Perf Mode EN/DIS state
                       // Disable IGPU Enhanced Perf Mode
                       Store (MMCH, MCHH)
                       Store (One, MMCH)
                   }
                   Else
                   {
                       If (LEqual (PCAP, Zero))
                       {
                           // Restore IGPU Enhanced Perf Mode EN/DIS state
                           Store (MCHH, MMCH)
                       }
                   }
                }
 
                Store (PCAP, CPU0_SCOPE._PPC)
                Notify (CPU0_SCOPE, 0x80)
 
                If (CondRefOf (CPU1_SCOPE._PPC, Local0)) {
                  Store (PCAP, CPU1_SCOPE._PPC)
                  Notify (CPU1_SCOPE, 0x80)
                }
 
                If (CondRefOf (CPU2_SCOPE._PPC, Local0)) {
                  Store (PCAP, CPU2_SCOPE._PPC)
                  Notify (CPU2_SCOPE, 0x80)
                }
 
                If (CondRefOf (CPU3_SCOPE._PPC, Local0)) {
                  Store (PCAP, CPU3_SCOPE._PPC)
                  Notify (CPU3_SCOPE, 0x80)
                }
 
                Return (PCAP)
            }
 
            //
            // Function 35: SPB_FUNC_GETPPC
            //
            case (SPB_FUNC_GETPPC)
            {
                Return (PSCP)
            }
 
           //
           // Function 36: SPB_FUNC_CALLBACK
           //
           case (SPB_FUNC_CALLBACK)
           {
               CreateField (Arg3, 0, 20, THBG)
               CreateField (Arg3, 30, 1, DDVE)

               Return (DGPU_SCOPE.PBCM)
           }
  
            //
            // Function 42: SPB_FUNC_SYSPARAMS
            //
            case (SPB_FUNC_SYSPARAMS)
            {
                Return (SSNR (Arg3))
            }
 
        } // end of switch
        Return (STATUS_ERROR_UNSUPPORTED)  // Status Error Unsupported
    }
}
