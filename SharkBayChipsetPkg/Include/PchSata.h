//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#define   R_PCH_SIDE_TIMP                               0x40
#define   R_PCH_SIDE_TIMS                               0x42
#define   B_PCH_SIDE_TIM_IDE                            BIT15 // IDE Decode Enable
#define   B_PCH_SIDE_TIM_SITRE                          BIT14 // Drive 1 Timing Register Enable
#define   B_PCH_SIDE_TIM_ISP_MASK                       (BIT13 | BIT12) //IORDY Sample Point
#define   V_PCH_SIDE_TIM_ISP_5_CLOCK                    0x00   //IORDY Sample Point = 5 clocks
#define   V_PCH_SIDE_TIM_ISP_4_CLOCK                    0x01   //IORDY Sample Point = 4 clocks
#define   V_PCH_SIDE_TIM_ISP_3_CLOCK                    0x02   //IORDY Sample Point = 3 clocks
#define   B_PCH_SIDE_TIM_RCT_MASK                       (BIT9 | BIT8) //Recovery time
#define   V_PCH_SIDE_TIM_RCT_4_CLOCK                    0x00   //Recovery time = 4 clocks
#define   V_PCH_SIDE_TIM_RCT_3_CLOCK                    0x01   //Recovery time = 3 clocks         
#define   V_PCH_SIDE_TIM_RCT_2_CLOCK                    0x02   //Recovery time = 2 clocks         
#define   V_PCH_SIDE_TIM_RCT_1_CLOCK                    0x03   //Recovery time = 1 clock          
#define   B_PCH_SIDE_TIM_DTE1                           BIT7 //Drive 1 DMA Timing Enable          
#define   B_PCH_SIDE_TIM_PPE1                           BIT6 //Drive 1 Prefetch/Posting Enable    
#define   B_PCH_SIDE_TIM_IE1                            BIT5 //Drive 1 IORDY Sample Point Enable  
#define   B_PCH_SIDE_TIM_TIME1                          BIT4 //Drive 1 Fast Timing Bank           
#define   B_PCH_SIDE_TIM_DTE0                           BIT3 //Drive 0 DMA Timing Enable          
#define   B_PCH_SIDE_TIM_PPE0                           BIT2 //Drive 0 Prefetch/Posting Enable    
#define   B_PCH_SIDE_TIM_IE0                            BIT1 //Drive 0 IORDY Sample Point Enable  
#define   B_PCH_SIDE_TIM_TIME0                          BIT0 //Drive 0 Fast Timing Bank           
#define   R_PCH_SIDE_SIDETIM                            0x44
#define   B_PCH_SIDE_SIDETIM_SISP1_MASK                 (BIT7 | BIT6) //IORDY Sample Point 
#define   B_PCH_SIDE_SIDETIM_SRCT1_MASK                 (BIT5 | BIT4) //Recovery time      
#define   B_PCH_SIDE_SIDETIM_PISP1_MASK                 (BIT3 | BIT2) //IORDY Sample Point 
#define   B_PCH_SIDE_SIDETIM_PRCT1_MASK                 (BIT1 | BIT0) //Recovery time      
#define   R_PCH_SIDE_SDMA_CNT                           0x48
#define   B_PCH_SIDE_SDMA_CNT_SSDE1                     BIT3
#define   B_PCH_SIDE_SDMA_CNT_SSDE0                     BIT2
#define   B_PCH_SIDE_SDMA_CNT_PSDE1                     BIT1
#define   B_PCH_SIDE_SDMA_CNT_PSDE0                     BIT0
#define   R_PCH_SIDE_SDMA_TIM                           0x4A
#define   B_PCH_SDMA_TIM_SCT1_MASK                      0x3000
#define   B_PCH_SDMA_TIM_SCT0_MASK                      0x0300
#define   B_PCH_SDMA_TIM_PCT1_MASK                      0x0030
#define   B_PCH_SDMA_TIM_PCT0_MASK                      0x0003
#define   V_PCH_SIDE_SDMA_TIM_CT4_RP6                   0x00
#define   V_PCH_SIDE_SDMA_TIM_CT3_RP5                   0x01
#define   V_PCH_SIDE_SDMA_TIM_CT2_RP4                   0x02
#define   V_PCH_SIDE_SDMA_TIM_CT3_RP8                   0x01
#define   V_PCH_SIDE_SDMA_TIM_CT2_RP8                   0x02
#define   V_PCH_SIDE_SDMA_TIM_CT3_RP16                  0x01

#define   R_PCH_SIDE_IDE_CONFIG                         0x54
#define   B_PCH_SIDE_IDE_CONFIG_SEC_SIG_MODE_MASK       (BIT19 | BIT18)
#define   B_PCH_SIDE_IDE_CONFIG_PRIM_SIG_MODE_MASK      (BIT17 | BIT16)
#define   B_PCH_SIDE_IDE_CONFIG_FAST_SCB1               BIT15
#define   B_PCH_SIDE_IDE_CONFIG_FAST_SCB0               BIT14
#define   B_PCH_SIDE_IDE_CONFIG_FAST_PCB1               BIT13
#define   B_PCH_SIDE_IDE_CONFIG_FAST_PCB0               BIT12
#define   B_PCH_SIDE_IDE_CONFIG_SCB1                    BIT3 
#define   B_PCH_SIDE_IDE_CONFIG_SCB0                    BIT2 
#define   B_PCH_SIDE_IDE_CONFIG_PCB1                    BIT1 
#define   B_PCH_SIDE_IDE_CONFIG_PCB0                    BIT0
#define   PCH_SATA_1_MAX_DEVICES                        2
