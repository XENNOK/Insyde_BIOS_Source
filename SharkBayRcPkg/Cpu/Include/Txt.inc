;@file
;  This file contains definitions used in Txtpeibsp and txtpeiap.asm
;
;@copyright
;  Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;  This software and associated documentation (if any) is furnished
;  under a license and may only be used or copied in accordance
;  with the terms of the license. Except as permitted by such
;  license, no part of this software or documentation may be
;  reproduced, stored in a retrieval system, or transmitted in any
;  form or by any means without the express written consent of
;  Intel Corporation.
;
;  This file contains an 'Intel Peripheral Driver' and uniquely
;  identified as "Intel Reference Module" and is
;  licensed for Intel CPUs and chipsets under the terms of your
;  license agreement with Intel or your vendor.  This file may
;  be modified by the user, subject to additional terms of the
;  license agreement
;

;-----------------------------------------------------------------------------
; Common equates
;

UINT32  TYPEDEF DWORD
UINT64  TYPEDEF QWORD

;-----------------------------------------------------------------------------
; Bit definitions
;
BIT31   EQU     (1 SHL 31)
BIT30   EQU     (1 SHL 30)
BIT29   EQU     (1 SHL 29)
BIT28   EQU     (1 SHL 28)
BIT27   EQU     (1 SHL 27)
BIT26   EQU     (1 SHL 26)
BIT25   EQU     (1 SHL 25)
BIT24   EQU     (1 SHL 24)
BIT23   EQU     (1 SHL 23)
BIT22   EQU     (1 SHL 22)
BIT21   EQU     (1 SHL 21)
BIT20   EQU     (1 SHL 20)
BIT19   EQU     (1 SHL 19)
BIT18   EQU     (1 SHL 18)
BIT17   EQU     (1 SHL 17)
BIT16   EQU     (1 SHL 16)
BIT15   EQU     (1 SHL 15)
BIT14   EQU     (1 SHL 14)
BIT13   EQU     (1 SHL 13)
BIT12   EQU     (1 SHL 12)
BIT11   EQU     (1 SHL 11)
BIT10   EQU     (1 SHL 10)
BIT9    EQU     (1 SHL  9)
BIT8    EQU     (1 SHL  8)
BIT7    EQU     (1 SHL  7)
BIT6    EQU     (1 SHL  6)
BIT5    EQU     (1 SHL  5)
BIT4    EQU     (1 SHL  4)
BIT3    EQU     (1 SHL  3)
BIT2    EQU     (1 SHL  2)
BIT1    EQU     (1 SHL  1)
BIT0    EQU     (1 SHL  0)

PORTB			EQU	061h
PORTBMASK		EQU	(1 shl 4)
;
; BIOS ACM functions
;
TXT_LAUNCH_SCLEAN       EQU     00h
TXT_RESET_EST_BIT       EQU     01h
TXT_RESET_AUX           EQU     02h
TXT_LAUNCH_SCHECK       EQU     04h

PORT80_CODE_PREFIX      EQU     0A0h

;
; EDI parameter sent to SCHECK function
;
COLD_BOOT_PATH          EQU     0
S3_RESUME_PATH          EQU     BIT1
;----------------------------------------------------------------------------
; Common stackless calling macros
;
CALL_NS MACRO lbl 
	local	retaddr
	if	@WordSize EQ 2 
	  mov	sp, retaddr 
	else 
	  mov	esp, retaddr 
	endif

        pslldq  xmm4, 4
        pinsrw  xmm4, esp, 0
        rol     esp, 10h
        pinsrw  xmm4, esp, 1
                
	jmp	lbl 

retaddr:        
ENDM              

RET_NS MACRO            
        movd    esp, xmm4
        psrldq  xmm4, 4
 
        if @WordSize EQ 2
          jmp     sp
        else
          jmp     esp
        endif
ENDM

;-----------------------------------------------------------------------------
; AC Module header
;
ACM_HEADER STRUCT 
     ORG	24 
     AcmSize    dd	?               ; // 24 4 Module size (in multiples of four bytes)
ACM_HEADER ENDS

;-----------------------------------------------------------------------------
;  TXT enabling and execution
;

CAPABILITIES    EQU     00h
ENTERACCS       EQU     02h

CR0_NE_MASK     EQU     (1 SHL 5)
CR0_NW_MASK     EQU     (1 SHL 29)
CR0_CD_MASK     EQU     (1 SHL 30)
CR0_PG_MASK     EQU     (1 SHL 31)

CR4_VME         EQU     (1 SHL 0)
CR4_PVI         EQU     (1 SHL 1)
CR4_TSD         EQU     (1 SHL 2)
CR4_DE          EQU     (1 SHL 3)
CR4_PSE         EQU     (1 SHL 4)
CR4_PAE         EQU     (1 SHL 5)
CR4_MSE         EQU     (1 SHL 6)
CR4_PGE         EQU     (1 SHL 7)
CR4_PCE         EQU     (1 SHL 8)
CR4_OSFXSR      EQU     (1 SHL 9)
CR4_VMXE        EQU     (1 SHL 13)
CR4_SMXE        EQU     (1 SHL 14)

_GETSEC         EQU     db 0fh, 37h

;----------------------------------------------------------------------------
;               DescriptorAccess Rights Definitions
;
PAGEGRANULARITY EQU     01H	; Page Granularity
BYTEGRANULARITY EQU     00H	; Byte Granularity
BIGSEGMENT      EQU     01H	; Big Segment 32 bit
SMALLSEGMENT    EQU     00H	; Small Segment 16 bit
SEG_PRESENT     EQU     01H	; Segment Present
SEG_NOTPRESENT  EQU     00H	; Segment Not Present
APPLSEGMENT     EQU     01H	; Application Segment
SYSTEMSEGMENT   EQU     00H	; System Segment

DATATYPE        EQU     03H	; Read, Write, accessed
STACKTYPE       EQU     03H	; Read, Write, accessed
CODETYPE        EQU     0BH	; Execute, Read, accessed

;----------------------------------------------------------------------------
;Descritor      table   structures and records
;

R_AR0_7 RECORD P:1=0, DPL:2=0, D_T:1=0, sType:4=0

; P      - Segment present
;	0=NOTPRESENT
;	1=PRESENT
; DPL   	- Descriptor priviledge level
; D_T	- Descriptor type
;	0=SYSTEMSEGMENT
;	1=APPLSEGMENT
; sType	- Segment Type
;	For Application segment:
;	03h - DATATYPE (Read/Write/Accessed)
;	07h - STACKTYPE (Expand Down/Read/Write/Accessed)
;	0Fh - CODETYPE (Execute/Conforming/Read/Accessed)
;
;	For System segment:
;	01h - Available 16 bit TSS
;	02h - LDT
;	03h - Active 16 bit TSS
;	04h - 16 bit call gate
;	05h - 16 + 32 bit task gate
;	06h - 16 bit interrupt gate
;	07h - 16 bit trap gate
;	09h - Available 32 bit TSS
;	0Bh - Active 32 bit TSS
;	0Ch - 32 bit call gate
;	0Eh - 32 bit interrupt gate
;	0Fh - 32 bit trap gate
                
R_LAR16_23 RECORD G:1=0, D_B:1=0, L:1=0, Avl:1=0, Lim:4=0

; G	- Granularity
;	0=BYTEGRANULARITY
;	1=PAGEGRANULARITY
; D_B	- Segment size
;	0=SMALLSEGMENT
;	1=BIGSEGMENT
; Res	- Reserved
; Avl	- Available for OS (Not used)
; Lim	- Limit bits 16-19

SEG_DESCRIPTOR STRUCT 4 
        Limit0_15	DW	0000            ; Limit Bits 0-15
        Base0_15	DW	0000            ; Base address Bits 0-15
        Base16_23	DB	00              ; Base address Bits 16-23
        AR0_7	R_AR0_7 <>              ; Access Rights 0-7
	LAR16_23	R_LAR16_23 <>   ; Limit Bits 16-19 and Access Bits 8-11
        Base24_31	DB	00              ; Base address Bits 24-31
SEG_DESCRIPTOR ENDS 
                 
;-----------------------------------------------------------------------------
; Machine Check register definitions
;
MCG_CAP                                 EQU 179h

MC0_STATUS                              EQU 401h

;-----------------------------------------------------------------------------
; TXT register space definitiions
;
TXT_PRIVATE_BASE                        EQU     0FED20000h
TXT_PUBLIC_BASE                         EQU     0FED30000h
TXT_REGISTER_SPACE_LENGTH               EQU     070000h

TXT_STS                                 EQU     0000h
  TXT_STS_SENTER_DONE_MASK              EQU     BIT0
  TXT_STS_SEXIT_DONE_MASK               EQU     BIT1
  TXT_STS_MEM_UNLOCK_STS_MASK           EQU     BIT4
  TXT_STS_NODMAEN_MASK                  EQU     BIT5
  TXT_STS_MEMCONFIGLOCK_STS_MASK        EQU     BIT6
  TXT_STS_PRIVATEOPEN_STS_MASK          EQU     BIT7
  TXT_STS_BLOCKMAP_STS_MASK             EQU     BIT8
  TXT_STS_NODMACACHE_STS_MASK           EQU     BIT9
  TXT_STS_NODMATABLEPROTECT_STS_MASK    EQU     BIT10
  TXT_ESTS                              EQU     0008h
  TXT_WAKE_ERROR_STS                    EQU     BIT6

TXT_THREADS_EXIST                       EQU     0010h
TXT_THREADS_JOIN                        EQU     0020h
TXT_CRASH                               EQU     0030h
TXT_CMD_SYS_RESET                       EQU     0038h
TXT_CMD_OPEN_PRIVATE                    EQU     0040h
TXT_CMD_CLOSE_PRIVATE                   EQU     0048h
TXT_POISON                              EQU     00B0h
TXT_VER_DID                             EQU     0110h
TXT_VER_EID                             EQU     0118h
TXT_VER_MIF                             EQU     0200h
TXT_CMD_LOCK_MEM_CONFIG                 EQU     0210h
TXT_CMD_UNLOCK_MEM_CONFIG               EQU     0218h
TXT_CMD_UNLOCK_MEMORY                   EQU     0220h
TXT_CMD_NODMA_EN                        EQU     0230h
TXT_CMD_NODMA_DIS                       EQU     0238h
TXT_SINIT_BASE                          EQU     0270h
TXT_SINIT_SIZE                          EQU     0278h
TXT_SINIT_SIZE2                         EQU     027Ch
TXT_SVMM_JOIN                           EQU     0290h
TXT_SVMM_JOIN2                          EQU     0294h
TXT_HEAP_BASE                           EQU     0300h
TXT_HEAP_SIZE                           EQU     0308h
TXT_MSEG_BASE                           EQU     0310h
TXT_MSEG_SIZE                           EQU     0318h
TXT_SCRATCHPAD                          EQU     0320h
TXT_SCRATCHPAD2                         EQU     0324h
TXT_SCRATCHPAD3                         EQU     0328h
TXT_SCRATCHPAD4                         EQU     032Ch
TXT_DPR                                 EQU     0330h
TXT_CMD_OPEN_LOCALITY1                  EQU     0380h
TXT_CMD_CLOSE_LOCALITY1                 EQU     0388h
TXT_CMD_OPEN_LOCALITY3                  EQU     03a0h
TXT_CMD_CLOSE_LOCALITY3                 EQU     03a8h
TXT_PUBLIC_KEY                          EQU     0400h
TXT_TGFX_CMD                            EQU     0504h
TXT_TGA_BASE                            EQU     0510h
TXT_TGR_BASE                            EQU     0518h
TXT_TGTT_BASE                           EQU     0520h
TXT_TMSAC                               EQU     0540h
  TGTT_128MB                            EQU     BIT0
  TGTT_256MB                            EQU     BIT1
  TGTT_512MB                            EQU     BIT1+BIT0
TXT_VER_FTIF                            EQU     0800h
TXT_PCH_DIDVID                          EQU     0810h
TXT_CMD_SECRETS                         EQU     08E0h
TXT_CMD_NO_SECRETS                      EQU     08E8h
TXT_E2STS                               EQU     08F0h
  TXT_BLOCK_MEM_STS                     EQU     BIT2
  TXT_SECRETS_STS                       EQU     BIT1
  TXT_SLP_ENTRY_ERROR_STS               EQU     BIT0

MCU_MAX_SIZE                            EQU     10000h		; 64KB max size per MCU
MCU_SLOT_SZ_BITS                        EQU     11              ; 2KB MCU slot size
MCU_SLOT_SZ                             EQU     1 SHL MCU_SLOT_SZ_BITS

;-----------------------------------------------------------------------------
;
;                                       MCU     Constants
;
MCU_HEADER_VER                          EQU     1
MCU_LOADER_REV                          EQU     1
;
;                                       ProcessorMSR Equates
;
PLAT_ID_MSR     EQU     17h		; Platform ID
MCU_LOAD_MSR    EQU     79h		; Trigger to load MCU
MCU_REV_MSR     EQU     8Bh		; MCU Revision register
;
PLAT_ID_BIT     EQU     50		; Bits 52:50 has the Platform ID
PLAT_ID_BITMASK EQU     07h		; Bits 52:50 has the Platform ID

EFI_MSR_IA32_THERM_INTERRUPT   EQU     019Bh
;              
; Microcode Update (MCU) Header
;
MCU STRUCT 
        headerVer	DD	?               ; MCU Header Version ( = 00000001h )
        revision	DD	?               ; MCU Revision
        date		DD	?               ; MCU Date
        signature	DD	?               ; MCU Processor Signature
        checksum	DD	?               ; MCU Main checksum
        loaderRev	DD	?               ; MCU Loader Revision
        procFlags	DD	?               ; MCU Processor Flags (Platform ID)
        dataSize	DD	?               ; MCU Data Size
        totalSize	DD	?               ; MCU Total Size
        reserved	DD	3 dup(?) 
MCU ENDS 
;
; Extended Signature Table (EST)
;
EST	STRUCT
        count		DD	?		; EST Count
        checksum	DD	?		; EST Checksum
        reserved	DD	3 dup(?)
EST	ENDS
;
; Processor Signature Structure	(PSS)
;
PSS STRUCT 
        signature      DD	?               ; PSS Processor Signature
        procFlags      DD	?               ; PSS Processor Flags (Platform ID)
        checksum       DD	?               ; PSS Checksum
PSS ENDS 

;----------------------------------------------------------------------------
; APIC definitions
;
IA32_APIC_BASE				EQU	001Bh		; APIC base MSR
  BASE_ADDR_MASK			EQU	0FFFFF000h
   EFI_MSR_EXT_XAPIC_LVT_THERM          EQU     0833h
APIC_ID					EQU	20h
  APIC_ID_SHIFT                         EQU     24
ICR_LOW					EQU	300h
ICR_HIGH				EQU	310h
SPURIOUS_VECTOR_1			EQU	0F0h
LOCAL_APIC_THERMAL_DEF      EQU 330h
  B_INTERRUPT_MASK          EQU (1 SHL 16)
  B_DELIVERY_MODE           EQU (07h SHL 8)
  V_MODE_SMI                EQU (02h SHL 8)
  B_VECTOR                  EQU (0FFh SHL 0)


;-----------------------------------------------------------------------------
; MTRRs
;
IA32_MTRR_CAP   			EQU	0FEh 
IA32_MTRR_PHYSBASE0			EQU	200h 
IA32_MTRR_PHYSMASK0			EQU	201h 
IA32_MTRR_PHYSBASE1 			EQU	202h 
IA32_MTRR_PHYSMASK1 			EQU	203h 
IA32_MTRR_PHYSBASE2 			EQU	204h 
IA32_MTRR_PHYSMASK2 			EQU	205h 
IA32_MTRR_PHYSBASE3 			EQU	206h 
IA32_MTRR_PHYSMASK3 			EQU	207h 
IA32_MTRR_PHYSBASE4 			EQU	208h 
IA32_MTRR_PHYSMASK4 			EQU	209h 
IA32_MTRR_PHYSBASE5 			EQU	20Ah 
IA32_MTRR_PHYSMASK5 			EQU	20Bh 
IA32_MTRR_PHYSBASE6 			EQU	20Ch 
IA32_MTRR_PHYSMASK6 			EQU	20Dh 
IA32_MTRR_PHYSBASE7 			EQU	20Eh 
IA32_MTRR_PHYSMASK7 			EQU	20Fh 
IA32_MTRR_PHYSBASE8 			EQU	210h 
IA32_MTRR_PHYSMASK8 			EQU	211h 
IA32_MTRR_PHYSBASE9 			EQU	212h 
IA32_MTRR_PHYSMASK9 			EQU	213h 
IA32_MTRR_FIX64K_00000 			EQU	250h 
IA32_MTRR_FIX16K_80000 			EQU	258h 
IA32_MTRR_FIX16K_A0000 			EQU	259h 
IA32_MTRR_FIX4K_C0000 			EQU	268h 
IA32_MTRR_FIX4K_C8000 			EQU	269h 
IA32_MTRR_FIX4K_D0000 			EQU	26Ah 
IA32_MTRR_FIX4K_D8000 			EQU	26Bh 
IA32_MTRR_FIX4K_E0000 			EQU	26Ch 
IA32_MTRR_FIX4K_E8000 			EQU	26Dh 
IA32_MTRR_FIX4K_F0000 			EQU	26Eh 
IA32_MTRR_FIX4K_F8000 			EQU	26Fh 
IA32_CR_PAT 				EQU	277h 
IA32_MTRR_DEF_TYPE			EQU	2FFh 
NO_EVICT_MODE                           EQU     2E0h
							 
;
; Only low order bits are assumed
;
MTRR_MASK				EQU	0FFFFF000h

MTRR_ENABLE				EQU	(1 SHL 11)
MTRR_FIXED_ENABLE			EQU	(1 SHL 10)
MTRR_VALID 				EQU	(1 SHL 11)
UC					EQU	00h
WB					EQU	06h
      
MTRR_VCNT				EQU	8

;-----------------------------------------------------------------------------
;
; CPU generic definition
;    
IA32_CPUID_SMX_B        EQU     6

;-----------------------------------------------------------------------------
;
; AP communication area
;                              

MCU_BASE_ADDR           EQU     TXT_SINIT_SIZE
BIOACM_ADDR             EQU     TXT_SINIT_SIZE2
APINIT_ADDR             EQU     TXT_SVMM_JOIN
SEMAPHORE               EQU     TXT_SVMM_JOIN2

IA32_MISC_ENABLE_MSR    EQU     1A0h
IA32_EFER_MSR           EQU     0C0000080h
LME                     EQU     BIT8

IFNDEF  MKF_TXT_RLP_INIT
MKF_TXT_RLP_INIT        EQU     01h
ENDIF
