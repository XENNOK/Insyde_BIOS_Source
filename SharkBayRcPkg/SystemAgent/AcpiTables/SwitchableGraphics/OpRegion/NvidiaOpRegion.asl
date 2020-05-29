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

Scope (DGPU_SCOPE) {
    OperationRegion (NVIG, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (NVIG, AnyAcc, Lock, Preserve)
    {
        ///
        /// OpRegion Header             ///< Byte offset(decimal)
        ///    
        NISG,    128,   ///< 0            NVIG OpRegion Signature
        NISZ,     32,   ///< 16           NVIG OpRegion Size in KB
        NIVR,     32,   ///< 20           NVIG OpRegion Version
        ///
        /// OpRegion Data
        ///      
        GPSS,     32,   ///< 24           Policy Selection Switch Status (Current GPU)
        GACD,     16,   ///< 32           Active Displays
        GATD,     16,   ///< 34           Attached Displays
        LDES,      8,   ///< 36           Lid Event State
        DKST,      8,   ///< 37           Dock State
        DACE,      8,   ///< 38           Display ACPI Event
        DHPE,      8,   ///< 39           Display Hot-Plug Event
        DHPS,      8,   ///< 40           Display Hot-Plug Status
        SGNC,      8,   ///< 41           Notify Code (Cause of Notify(..,0xD0))
        GPPO,      8,   ///< 42           Policy Override
        USPM,      8,   ///< 43           Update Scaling Preference Mask
        GPSP,      8,   ///< 44           Panel Scaling Preference
        TLSN,      8,   ///< 45           Toggle List Sequence Number
        DOSF,      8,   ///< 46           Flag for _DOS
        ELCR,     16    ///< 47           Endpoint Link Contol Register Value (ELCT)
    }

    OperationRegion (NVHM, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (NVHM, AnyAcc, Lock, Preserve)
    {
        ///
        /// OpRegion Header             ///< Byte offset(decimal)
        ///    
        NVSG,    128,   ///< 0            NVHM OpRegion Signature
        NVSZ,     32,   ///< 16           NVHM OpRegion Size in KB
        NVVR,     32,   ///< 20           NVHM OpRegion Version
        ///
        /// OpRegion Data
        ///        
        NVHO,     32,   ///< 24           NVHM Opregion Address
        RVBS,     32,   ///< 28           Nvidia VBIOS Image Size
        VBS1, 262144,   ///< 32           32KB VBIOS
        VBS2, 262144    ///<               32KB VBIOS
    }
}
