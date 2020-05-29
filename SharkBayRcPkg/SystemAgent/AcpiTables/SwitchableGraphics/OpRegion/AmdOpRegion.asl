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
    OperationRegion (APXM, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (APXM, AnyAcc, Lock, Preserve)
    {
        ///
        /// OpRegion Header             ///< Byte offset(decimal)
        ///    
        APSG,    128,   ///< 0            APXM OpRegion Signature
        APSZ,     32,   ///< 16           APXM OpRegion Size in KB
        APVR,     32,   ///< 20           APXM OpRegion Version
        ///
        /// OpRegion Data
        ///        
        APXA,     32,   ///< 24           PX OpRegion Address
        RVBS,     32,   ///< 28           PX Runtime VBIOS Image Size
        NTLE,     16,   ///< 32           Total number of toggle list entries
        TLEX,    240,   ///< 34           The display combinations in the list...
        TGXA,     16,   ///< 64           Target GFX adapter as notified by ATPX function 5
        AGXA,     16,   ///< 66           Active GFX adapter as notified by ATPX function 6
        GSTP,      8,   ///< 68           GPU switch transition in progress
        DSWR,      8,   ///< 69           Display Switch Request
        EMDR,      8,   ///< 70           Expansion Mode Change Request
        PXGS,      8,   ///< 71           PowerXpress graphics switch toggle request
        CACD,     16,   ///< 72           Currently Active Displays
        CCND,     16,   ///< 74           Currently Connected Displays
        NACD,     16,   ///< 76           Next Active Index
        EXPM,      8,   ///< 78           Expansion Mode
        TLSN,     16,   ///< 79           Toggle list sequence index
            ,     16,   ///< 81           Endpoint Link Contol Register Value (ELCT)
        VBS1, 262144,   ///< 83          32KB VBIOS
        VBS2, 262144,   ///<              32KB VBIOS
//[-start-140220-IB10920078-add]//
        SLVS,      8,   /// Slave dGPU Support  
        PXDY,      8,   /// Dynamic Support State 1: dynamic, 0: Non-dynamic
        PXFD,      8,   /// Fixed   Support State 1: Fixed,   0: Non-fixed
        FDPD,      8    /// Full dGPU PowerOff Dynamic mode         
//[-end-140220-IB10920078-add]//        
    }
}
