/** @file
 Pch Serial ATA devices and control methods

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

DefinitionBlock (
	"SataPri.aml",
	"SSDT",
	0x01,
	"INTEL ",
	"SataPri",
	0x1000
	)
{
External(\_SB.PCI0, DeviceObj)
External(\_SB.PCI0.GPI0, MethodObj)
External(\_SB.PCI0.GDMA, MethodObj)
External(\_SB.PCI0.SFLG, MethodObj)
External(\_SB.PCI0.SPIO, MethodObj)
External(\_SB.PCI0.SDMA, MethodObj)
External(\GNVS)
Scope(\_SB.PCI0) {
Device(SATA)
{
    Field(GNVS,AnyAcc,Lock,Preserve)
    {
//[-start-130625-IB05400421-modify]//
        //
        // Sync with GloblNvs.asl and GlobalNvsArea.h
        //
        Offset (622),   // offset:622 (SATA1Channel0PioTiming)
//[-end-130625-IB05400421-modify]//
        PFT0   , 1 ,    // Drive 0 Fast Timing Bank (TIME0)
        PIE0   , 1 ,    // Drive 0 IORDY Sample Point Enable (IE0)
        PPE0   , 1 ,    // Drive 0 Prefetch/Posting Enable (PPE0)
        PDE0   , 1 ,    // Drive 0 DMA Timing Enable (DTE0)
        PFT1   , 1 ,    // Drive 1 Fast Timing Bank (TIME1)
        PIE1   , 1 ,    // Drive 1 IORDY Sample Point Enable (IE1)
        PPE1   , 1 ,    // Drive 1 Prefetch/Posting Enable (PPE1)
        PDE1   , 1 ,    // Drive 1 DMA Timing Enable (DTE1)
        PRT0   , 2 ,    // Drive 0 Recovery Time (RCT)
               , 2 ,    // Reserved
        PIP0   , 2 ,    // Drive 0 IORDY Sample Point (ISP)
        PSIT   , 1 ,    // Drive 1 Timing Register Enable (SITRE) 
               , 1 ,
//[-start-130625-IB05400421-modify]//
        Offset (624),   // offset:624 (SATA1Channel1PioTiming)
//[-end-130625-IB05400421-modify]//
        SFT0   , 1 ,    // Drive 0 Fast Timing Bank (TIME0)
        SIE0   , 1 ,    // Drive 0 IORDY Sample Point Enable (IE0)
        SPE0   , 1 ,    // Drive 0 Prefetch/Posting Enable (PPE0)
        SDE0   , 1 ,    // Drive 0 DMA Timing Enable (DTE0)
        SFT1   , 1 ,    // Drive 1 Fast Timing Bank (TIME1) 
        SIE1   , 1 ,    // Drive 1 IORDY Sample Point Enable (IE1)
        SPE1   , 1 ,    // Drive 1 Prefetch/Posting Enable (PPE1)
        SDE1   , 1 ,    // Drive 1 DMA Timing Enable (DTE1) 
        SRT0   , 2 ,    // Drive 0 Recovery Time (RCT)
               , 2 ,    // Reserved
        SIP0   , 2 ,    // Drive 0 IORDY Sample Point (ISP) 
        SSIT   , 1 ,    // Drive 1 Timing Register Enable (SITRE)
               , 1 ,
    }
    
    OperationRegion (IDER,PCI_Config,0x40,0x20)
    Field (IDER, AnyAcc, NoLock, Preserve) 
    {
               , 15 ,   // Reserved
        PIDE   , 1 ,    // IDE Decode Enable (IDE)
        offset (0x2) ,
               , 15 ,   // Reserved
        SIDE   , 1 ,    // IDE Decode Enable (IDE) 
    
        PRT1   , 2 ,    // Drive 1 Recovery Time (RCT) 
        PIP1   , 2 ,    // Drive 1 IORDY Sample Point (ISP)
        SRT1   , 2 ,    // Drive 1 Recovery Time (RCT) 
        SIP1   , 2 ,    // Drive 1 IORDY Sample Point (ISP) 
    
        offset (0x08)  ,  
    
        UDM0   , 1 ,    // Primary Drive 0 Synchronous DMA Mode Enable 
        UDM1   , 1 ,    // Primary Drive 1 Synchronous DMA Mode Enable 
        UDM2   , 1 ,    // Secondary Drive 0 Synchronous DMA Mode Enable 
        UDM3   , 1 ,    // Secondary Drive 1 Synchronous DMA Mode Enable 
    
        offset (0x0A)  ,
    
        PCT0   , 2 ,   // Primary Drive 0 Cycle Time (PCT0)
               , 2 ,   // Reserved
        PCT1   , 2 ,   // Primary Drive 1 Cycle Time (PCT1)
               , 2 ,   // Reserved
        SCT0   , 2 ,   // Secondary Drive 0 Cycle Time (SCT0)
               , 2 ,   // Reserved
        SCT1   , 2 ,   // Secondary Drive 1 Cycle Time (SCT1)
    
        offset (0x14)  ,
        PCB0   , 1 ,   // Primary Drive 0 Base Clock (PCB0) 
        PCB1   , 1 ,   // Primary Drive 0 Base Clock (PCB0)
        SCB0   , 1 ,   // Secondary Drive 1 Base Clock (SCB0)
        SCB1   , 1 ,   // Secondary Drive 1 Base Clock (SCB1)
        PCCR   , 2 ,   // Primary Channel Cable Reporting
        SCCR   , 2 ,   // Secondary Channel Cable Reporting
               , 4 ,   // Reserved
        PUM0   , 1 ,   // Primary Drive 0 UDMA 5 Supported
        PUM1   , 1 ,   // Primary Drive 1 UDMA 5 Supported
        SUM0   , 1 ,   // Secondary Drive 0 UDMA 5 Supported
        SUM1   , 1 ,   // Secondary Drive 1 UDMA 5 Supported
        PSIG   , 2 ,   // PRIM_SIG_MODE
        SSIG   , 2 ,   // SEC_SIG_MODE  
        UMT0   , 1 ,   // UDMA 6 Support check
        UMT1   , 1 ,   // UDMA 6 Support check
        UMT2   , 1 ,   // UDMA 6 Support check
        UMT3   , 1 ,   // UDMA 6 Support check
             
    }
  
 
    Name(_ADR,0x001F0002)
    //
    // Primary ide channel
    //
    Device(PRID)
    {
        Name(_ADR,0)
        Name(TDM0, 0)          // Drive 0 Ultra DMA Type
        Name(TPI0, 0)          // Drive 0 PIO Type
        Name(TDM1, 0)          // Drive 1 Ultra DMA Type
        Name(TPI1, 0)          // Drive 1 PIO Type
        Method(_GTM)
        {
            Name(PBUF, Buffer(20) { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00 })

            CreateDwordField(PBUF, 0,  PIO0)
            CreateDwordField(PBUF, 4,  DMA0)
            CreateDwordField(PBUF, 8,  PIO1)
            CreateDwordField(PBUF, 12, DMA1)
            CreateDwordField(PBUF, 16, FLAG)    
      
            Store ( GPI0 (PFT0, PDE0, PRT0, PIP0 ), PIO0)      
            //
            // Setting the Drive1 PIO Timing, check if we use the same timging for
            // both Drive0 and Drive1, and if the Drive0 is attached, else use 
            // separate timing
            //           
            If ( And (PSIT, 1) ) {
                Store ( GPI0 (PFT1, PDE1, PRT1, PIP1 ), PIO1)
            } Else {                
                Store ( GPI0 (PFT1, PDE1, PRT0, PIP0 ), PIO1)
            }
//[-start-130625-IB05400421-modify]//
            //
            // Synchronous primary driver 0 DMA
            //
            Store (GDMA (UDM0, PUM0, PCB0, PCT0), DMA0)
//[-end-130625-IB05400421-modify]//
            If (LAnd (LEqual(DMA0, 20), UMT0)) {
              Store (15, DMA0)
            }            
            If ( LGreater ( DMA0, PIO0)) {
                Store(PIO0, DMA0)        
            }       
//[-start-130625-IB05400421-modify]//
            //
            // Synchronous primary driver 1 DMA
            //
            Store (GDMA (UDM1, PUM1, PCB1, PCT1), DMA1)
//[-end-130625-IB05400421-modify]//
            If (LAnd (LEqual(DMA1, 20), UMT1)) {
              Store (15, DMA1)
            }
            If ( LGreater ( DMA1, PIO1)) {
                Store(PIO1, DMA1)
            }
            //}

            //======================================================
            // UDM0  // Primary Drive 0 Synchronous DMA Mode Enable 
            // UDM1  // Primary Drive 1 Synchronous DMA Mode Enable
            //
            // PIE0  // Drive 0 IORDY Sample Point Enable (IE0)
            // PIE1  // Drive 1 IORDY Sample Point Enable (IE1)
            //======================================================

//[-start-130625-IB05400421-modify]//
            Store (SFLG (UDM0, PIE0, UDM1, PIE1, 1), FLAG)
//[-end-130625-IB05400421-modify]//
      
            Return (PBUF)
        }
        Method(_STM,3)
        {
            CreateDwordField(Arg0, 0,  PIO0)
            CreateDwordField(Arg0, 4,  DMA0)
            CreateDwordField(Arg0, 8,  PIO1)
            CreateDwordField(Arg0, 12, DMA1)
            CreateDwordField(Arg0, 16, FLAG)      
            //
            // Device 0 Raw data
            //
            CreateWordField(Arg1, 106, RPS0)    // word 53
            CreateWordField(Arg1, 128, IOM0)    // word 64
            CreateWordField(Arg1, 176, DMM0)    // Word 88
      
            //
            // Device 1 Raw data
            //
            CreateWordField(Arg2, 106, RPS1)    // word 53
            CreateWordField(Arg2, 128, IOM1)    // word 64
            CreateWordField(Arg2, 176, DMM1)    // Word 88
      
            Name(IOTM, Buffer(5) { 0x00,0x00,0x00,0x00})
      
            CreateByteField(IOTM, 0,  RCT)
            CreateByteField(IOTM, 1,  ISP)
            CreateByteField(IOTM, 2,  FAST) 
            CreateByteField(IOTM, 3,  DMAE)
            CreateByteField(IOTM, 4,  TPIO)     // PIO Type
      
            Name(DMAT, Buffer(5) { 0x00,0x00,0x00,0x00})

            CreateByteField(DMAT, 0,  PCT)
            CreateByteField(DMAT, 1,  PCB)
            CreateByteField(DMAT, 2,  UDMT)     // ATA 100 Support
            CreateByteField(DMAT, 3,  UDME)     // Ultra DMA Enable 
            CreateByteField(DMAT, 4,  TDMA)     // UDMA Type
      
            Store (SPIO (PIO0,RPS0,IOM0), IOTM)
            
            If (Or (DMAE, FAST)) {
                Store (RCT, PRT0)
                Store (ISP, PIP0)
                Store (FAST, PFT0)
                Store (DMAE, PDE0)
                Store (TPIO, TPI0)
            }
            Store (SPIO (PIO1,RPS1,IOM1), IOTM)
                
            If (Or (DMAE, FAST)) {
                Store (FAST, PFT1)
                Store (DMAE, PDE1)
                Store (TPIO, TPI1)
                If (And (PSIT,1)) {
                    //
                    // Need set Drive 1 PIO Timing seperate
                    //        
                    Store (RCT, PRT1)
                    Store (ISP, PIP1)         
                } Else {
                    Store (RCT, PRT0)
                    Store (ISP, PIP0)
                }
            }
            If (And (FLAG , 0x01)) {
                Store (SDMA (DMA0,RPS0,DMM0), DMAT)
                Store (PCT , PCT0)
                Store (PCB , PCB0)
                Store (UDME, UDM0)
                Store (UDMT, PUM0)
                Store (TDMA, TDM0)
            } Else {
                Store (0, UDM0)
            }
      
            If (And (FLAG , 0x04)) {
                Store (SDMA (DMA1,RPS1,DMM1), DMAT)
                Store (PCT , PCT1)
                Store (PCB , PCB1)
                Store (UDME, UDM1)
                Store (UDMT, PUM1)
                Store (TDMA, TDM1)
            } Else {
                Store (0, UDM1)
            }      
            //
            //  Check IORDY Support 
            //
            If (And (FLAG , 0x2)) { 
                Store (1 , PIE0)
            }
            If (And (FLAG , 0x8)) { 
                Store (1 , PIE1)
            }
                
        }
        Device(MAST)
        {
            Name(_ADR,0)
            Method(_GTF)
            {
                //
                // Set ATA Device to corresponding Mode
                //
                Name(ATA0, Buffer(14)
                    { 0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF,
                      0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF })
        
                CreateByteField(ATA0,1,PIO0)  // PIO0 = PIO Mode, Drive 0
                CreateByteField(ATA0,8,DMA0)  // DMA0 = DMA Mode, Drive 0
        
        
                Store (TPI0, PIO0)            // Type we Already get
        
                Or (PIO0, 0x08 ,PIO0)
        
                If ( And (UDM0, 1)) {
                    Store (TDM0, DMA0)          // Ultra DMA
                    Or (DMA0, 0x40, DMA0)
                } Else {
                    Store (TPI0, DMA0)          // Use PIO Timing
                    If ( LNotEqual (DMA0, 0)) {
                        Subtract(DMA0, 2, DMA0)
                    }
                    Or (DMA0, 0x20, DMA0)
                }
                Return (ATA0)
            }   
        }
        Device(SLAV)
        {
            Name(_ADR,1)
            Method(_GTF)
            {
                //
                // Set ATA Device to corresponding Mode
                //
                Name(ATA1, Buffer(14)
                    { 0x03, 0x00, 0x00, 0x00, 0x00, 0xB0, 0xEF,
                      0x03, 0x00, 0x00, 0x00, 0x00, 0xB0, 0xEF })
       
                CreateByteField(ATA1,1,PIO1)  // PIO0 = PIO Mode, Drive 0
                CreateByteField(ATA1,8,DMA1)  // DMA0 = DMA Mode, Drive 0
                
                Store (TPI1, PIO1)            // Type we Already get
        
                Or (PIO1, 0x08 ,PIO1)
        
                If ( And (UDM1, 1)) {
                    Store (TDM1, DMA1)          // Ultra DMA
                    Or (DMA1, 0x40, DMA1)
                } Else {
                    Store (TPI1, DMA1)          // Use PIO Timing
                    If ( LNotEqual (DMA1, 0)) {
                        Subtract(DMA1, 2, DMA1)
                    }
                    Or (DMA1, 0x20, DMA1)
                }
                Return(ATA1)
            }   
        }
    }
    //
    // Secondary SATA channel
    //
    Device(SECD)
    {
        Name(_ADR,1)
        Name(TDM0, 0)
        Name(TPI0, 0)
        Name(TDM1, 0)
        Name(TPI1, 0)
        
        Name(DMT1, Buffer(5) { 0x00,0x00,0x00,0x00})
        Name(DMT2, Buffer(5) { 0x00,0x00,0x00,0x00})
        Name(POT1, Buffer(5) { 0x00,0x00,0x00,0x00})        
        Name(POT2, Buffer(5) { 0x00,0x00,0x00,0x00})
        
        Name(STMI, Buffer(20) { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00 })
     
        Method(_GTM)
        {
            Name(PBUF, Buffer(20) { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00 })

            CreateDwordField(PBUF, 0,  PIO0)
            CreateDwordField(PBUF, 4,  DMA0)
            CreateDwordField(PBUF, 8,  PIO1)
            CreateDwordField(PBUF, 12, DMA1)
            CreateDwordField(PBUF, 16, FLAG)
  
            Store ( GPI0 (SFT0, SDE0, SRT0, SIP0 ), PIO0)  
            //
            // Setting the Drive1 PIO Timing, check if we use the same timging for
            // both Drive0 and Drive1, and if the Drive0 is attached, else use 
            // separate timing
            //
          
            If ( And (SSIT, 1) ) {
                Store ( GPI0 (SFT1, SDE1, SRT1, SIP1 ), PIO1)
            } Else {                
                Store ( GPI0 (SFT1, SDE1, SRT0, SIP0 ), PIO1)
            }
          
            //If (LEqual (PIO0, 0xFFFFFFFF)) {
            //    Store(PIO0, DMA0)
            //} Else {
//[-start-130625-IB05400421-modify]//
            //
            // Synchronous secondary driver 0 DMA
            //
            Store (GDMA (UDM2, SUM0, SCB0, SCT0), DMA0)
//[-end-130625-IB05400421-modify]//
            If (LAnd (LEqual(DMA0, 20), UMT2)) {
              Store (15, DMA0)
            }
            If ( LGreater ( DMA0, PIO0)) {
                Store(PIO0, DMA0)
            }
            //}
      
            //If (LEqual (PIO1, 0xFFFFFFFF)) {
            //    Store(PIO1, DMA1)   
            //} Else {
//[-start-130625-IB05400421-modify]//
            //
            // Synchronous secondary driver 1 DMA
            //
            Store (GDMA (UDM3, SUM1, SCB1, SCT1), DMA1)
//[-end-130625-IB05400421-modify]//
            If (LAnd (LEqual(DMA1, 20), UMT3)) {
              Store (15, DMA1)
            }
            If ( LGreater ( DMA1, PIO1)) {
                Store(PIO1, DMA1)
            }
            //}
      
//[-start-130625-IB05400421-modify]//
            Store (SFLG (UDM2, SIE0, UDM3, SIE1, 1), FLAG)
//[-end-130625-IB05400421-modify]//
            
            Return (PBUF)
        }

        Method(_STM,3)
        {
            CreateDwordField(Arg0, 0,  PIO0)
            CreateDwordField(Arg0, 4,  DMA0)
            CreateDwordField(Arg0, 8,  PIO1)
            CreateDwordField(Arg0, 12, DMA1)
            CreateDwordField(Arg0, 16, FLAG)
            
            Store (Arg0, STMI)
            //
            // Device 0 Raw data
            //
            CreateWordField(Arg1, 106, RPS0)    // word 53
            CreateWordField(Arg1, 128, IOM0)    // word 64
            CreateWordField(Arg1, 176, DMM0)    // Word 88
      
            //
            // Device 1 Raw data
            //
            CreateWordField(Arg2, 106, RPS1)    // word 53
            CreateWordField(Arg2, 128, IOM1)    // word 64
            CreateWordField(Arg2, 176, DMM1)    // Word 88
      
            Name(IOTM, Buffer(5) { 0x00,0x00,0x00,0x00})

            CreateByteField(IOTM, 0,  RCT)
            CreateByteField(IOTM, 1,  ISP)
            CreateByteField(IOTM, 2,  FAST) 
            CreateByteField(IOTM, 3,  DMAE)
            CreateByteField(IOTM, 4,  TPIO)     // PIO Type

            Name(DMAT, Buffer(5) { 0x00,0x00,0x00,0x00})

            CreateByteField(DMAT, 0,  PCT)
            CreateByteField(DMAT, 1,  PCB)
            CreateByteField(DMAT, 2,  UDMT)     // ATA 100 Support
            CreateByteField(DMAT, 3,  UDME)     // Ultra DMA Enable 
            CreateByteField(DMAT, 4,  TDMA)     // UDMA Type
            
            //
            // Get Timing and Flag Setting
            //            
            Store (SPIO (PIO0,RPS0,IOM0), IOTM)
            //
            // If no drive0 connect, do nothing to program Drive0 timing
            //
            If (Or (DMAE, FAST)) {
                Store (RCT, SRT0)
                Store (ISP, SIP0)
                Store (FAST, SFT0)
                Store (DMAE, SDE0)
                Store (TPIO, TPI0)
            }             
            
            Store (SPIO (PIO1,RPS1,IOM1), IOTM)
            
            Store (IOTM,POT2)
            
            If (Or (DMAE, FAST)) {
                Store (FAST, SFT1)
                Store (DMAE, SDE1)
                Store (TPIO, TPI1)
                If (And (SSIT,1)) {
                    //
                    // Need set Drive 1 PIO Timing separately
                    //        
                    Store (RCT, SRT1)
                    Store (ISP, SIP1)
                } Else {
                    Store (RCT, SRT0)
                    Store (ISP, SIP0)
                } 
            }
            
            If (And (FLAG , 0x01)) {
                Store (SDMA (DMA0,RPS0,DMM0), DMAT)
                Store (PCT , SCT0)
                Store (PCB , SCB0)
                Store (UDME , UDM2)
                Store (UDMT , SUM0)
                Store (TDMA, TDM0)
            } Else {
                Store (0, UDM2)
            }
            If (And (FLAG , 0x04)) {
                Store (SDMA (DMA1,RPS1,DMM1), DMAT)
                Store (PCT , SCT1)
                Store (PCB , SCB1)
                Store (UDME , UDM3)
                Store (UDMT , SUM1)
                Store (TDMA , TDM1)
            } Else {
                Store (0, UDM3)
            }
            //
            //  Check IORDY Support 
            //
            If (And (FLAG , 0x2)) { 
                Store (1 , SIE0)
            }
            If (And (FLAG , 0x8)) { 
                Store (1 , SIE1)
            }
                    
        }
        Device(MAST)
        {
            Name(_ADR,0)
            Method(_GTF)
            {
                //
                // Set ATA Device to corresponding Mode
                //
                Name(ATA0, Buffer(14)
                { 0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF,
                  0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF })
       
                CreateByteField(ATA0,1,PIO0)  // PIO0 = PIO Mode, Drive 0
                CreateByteField(ATA0,8,DMA0)  // DMA0 = DMA Mode, Drive 0
        
                Store (TPI0, PIO0)            // Type we Already get
        
                Or (PIO0, 0x08 ,PIO0)
        
                If ( And (UDM2, 1)) {
                    Store (TDM0, DMA0)          // Ultra DMA
                    Or (DMA0, 0x40, DMA0)
                } Else {
                    Store (TPI0, DMA0)          // Use PIO Timing
                    If ( LNotEqual (DMA0, 0)) {
                        Subtract(DMA0, 2, DMA0)
                    }
                    Or (DMA0, 0x20, DMA0)
                }
                Return (ATA0)
            }   
        }
        Device(SLAV)
        {
            Name(_ADR,1)
            Method(_GTF)
            {
                //
                // Set ATA Device to corresponding Mode
                //
                Name(ATA1, Buffer(14)
                { 0x03, 0x00, 0x00, 0x00, 0x00, 0xB0, 0xEF,
                  0x03, 0x00, 0x00, 0x00, 0x00, 0xB0, 0xEF })
        
                CreateByteField(ATA1,1,PIO1)  // PIO0 = PIO Mode, Drive 0
                CreateByteField(ATA1,8,DMA1)  // DMA0 = DMA Mode, Drive 0
        
                Store (TPI1, PIO1)            // Type we Already get
        
                Or (PIO1, 0x08 ,PIO1)
        
                If ( And (UDM3, 1)) {
                    Store (TDM1, DMA1)          // Ultra DMA
                    Or (DMA1, 0x40, DMA1)
                } Else {
                    Store (TPI1, DMA1)          // Use PIO Timing
                    If ( LNotEqual (DMA1, 0)) {
                        Subtract(DMA1, 2, DMA1)
                    }
                    Or (DMA1, 0x20, DMA1)
                }
                Return(ATA1)
            }   
        }
    }    
}
}
}
