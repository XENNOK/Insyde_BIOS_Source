/** @file
  _H2O_SD_HOST_IO_PPI

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_SD_HOST_IO_PEI_H_
#define _H2O_SD_HOST_IO_PEI_H_

//
#define H2O_SD_HOST_IO_PPI_GUID \
  {  \
    0xaf6e2bb9, 0x96fd, 0x4f8f, 0xaf, 0x73, 0x1c, 0x27, 0x92, 0x51, 0x8c, 0xbf \
  }

typedef struct _H2O_SD_HOST_IO_PPI H2O_SD_HOST_IO_PPI;

#define H2O_SD_HOST_IO_PPI_REVISION_01 0x01 

#define TIMEOUT_1S                  (1000)
#define TIMEOUT_250MS               (250)
//
// Command timeout will be max 100 ms 
//
#define  TIMEOUT_COMMAND            (100)
#define  TIMEOUT_DATA               (5000)

//
// Sd Host MMIO Registers
//
#define SDMA_ADDR                   0x00
#define BLOCK_SIZE                  0x04
#define BLEN_512BYTES               (0x200 << 0)
#define BLOCK_COUNT                 0x06
#define ARGUMENT                    0x08        //It is Specified as bit39-8 of Command Format.
#define TRANSMODE                   0x0C
#define   DE_ENABLE                 BIT0
#define   BCE_ENABLE                BIT1
#define   ACMD12_ENABLE             BIT2
#define   TRANS_DIRECTION           BIT4
#define   DDIR_READ                 BIT4
#define   DDIR_WRITE                (0x0 << 4)
#define   MSBS_SGLEBLK              (0x0 << 5)
#define   MSBS_MULTBLK              BIT5
#define   RSP_TYPE_MASK             (0x3 << 16) // 00Eh bit0 ~ bit1
#define   RSP_TYPE_136BITS          BIT16       // 00Eh bit0 ~ bit1
#define   RSP_TYPE_48BITS           (0x2 << 16) // 00Eh bit0 ~ bit1
#define   RSP_48_CHECK_BUSY         (0x3 << 16) // 00Eh bit0 ~ bit1
#define   CCCE_ENABLE               BIT19       // 00Eh bit3
#define   CICE_ENABLE               BIT20       // 00Eh bit4
#define   DATA_PRESENT              BIT21       // 00Eh bit5
#define   COMMAND_TYPE              (0x3 << 22)
#define   NORMAL_CMD                (0x0 << 22) // 00Eh bit6 ~ bit1
#define   SUSPEND_CMD               (0x1 << 22) // 00Eh bit6 ~ bit7
#define   RESUME_CMD                (0x2 << 22) // 00Eh bit6 ~ bit7
#define   ABORT_CMD                 (0x3 << 22) // 00Eh bit6 ~ bit7
#define COMMAND_REG                 0x0E
#define RESPONSE                    0x10
#define BUFFER                      0x20
#define PRESENT_STATE               0x24
#define   CMDI_MASK                 BIT0
#define   CMDI_ALLOWED              (0x0 << 0)
#define   CMDI_NOT_ALLOWED          BIT0
#define   DATAI_MASK                BIT1
#define   DATAI_ALLOWED             (0x0 << 1)
#define   DATAI_NOT_ALLOWED         BIT1
#define   DATA_LINE_ACT             BIT2
#define   CARD_INSERTED             BIT16
#define   SDWP                      BIT19
#define HOST_CONTROL                0x28
#define   DTW                       BIT1
#define   DTW_1_BIT                 (0x0 << 1)
#define   DTW_4_BIT                 BIT1
#define   DTW_8_BIT                 BIT5
#define   HS_EN                     BIT2
#define   DMA_SELECT_MASK           (0x03 << 3)
#define   SDMA_SELECT               (0x00 << 3)
#define   ADMA1_32_SELECT           (0x01 << 3)
#define   ADMA2_32_SELECT           (0x02 << 3)
#define   ADMA2_64_SELECT           (0x03 << 3)
#define   SDBP_MASK                 BIT8        // 029h bit0
#define   SDBP_OFF                  (0x0 << 8)  // 029h bit0
#define   SDBP_ON                   BIT8        // 029h bit0
#define   SDVS_1_8_V                (0x5 << 9)  // 029h bit1 ~ bit3
#define   SDVS_3_0_V                (0x6 << 9)  // 029h bit1 ~ bit3
#define   SDVS_3_3_V                (0x7 << 9)  // 029h bit1 ~ bit3
#define   IWE                       BIT24       // 02Bh bit0
#define POWER_CONTROL               0x29
#define BLK_GAP_CONTROL             0x2A
#define WAKEUP_CONTROL              0x2B
#define CLOCK_CONTROL               0x2C
#define   ICE                       BIT0
#define   ICS_MASK                  BIT1
#define   ICS                       BIT1
#define   CEN                       BIT2
#define   CLKD_8BITS_MASK           (0xFF << 8)
#define   CLKD_10BITS_MASK          (0xFFC << 4)
#define   DTO_MASK                  (0xF << 16) // 02Eh bit0 ~ bit3
#define   DTO_VAL                   (0xE << 16) // 02Eh bit0 ~ bit3
#define   SOFT_RESET_ALL            BIT24       // 02Fh bit0
#define   SOFT_RESET_CMD            BIT25       // 02Fh bit1
#define   SOFT_RESET_DATA           BIT26       // 02Fh bit2
#define TIMEOUT_CONTROL             0x2E
#define SOFT_RESET                  0x2F
#define   RESET_ALL                 BIT0
#define INTERRUPT_STATUS            0x30
#define   CC                        BIT0
#define   TC                        BIT1
#define   BGAP                      BIT2
#define   DMA_INT                   BIT3
#define   BWR                       BIT4
#define   BRR                       BIT5
#define   ERRI                      BIT15
#define   CTO                       BIT16       // 032h bit0
#define   CMD_CRC_ERR               BIT17       // 032h bit1
#define   CMD_END_BIT_ERR           BIT18       // 032h bit2
#define   CMD_IDX_ERR               BIT19       // 032h bit3
#define   DATA_TIMEOUT_ERR          BIT20       // 032h bit4
#define   DATA_CRC_ERR              BIT21       // 032h bit5
#define   DATA_END_BIT_ERR          BIT22       // 032h bit6
#define   ADMA_ERR                  BIT25       // 032h bit9
#define ERROR_INT_STATUS            0x32
#define INT_STATUS_ENABLE           0x34
#define   CC_EN                     BIT0
#define   TC_EN                     BIT1
#define   BGAP_EN                   BIT2
#define   DMA_EN                    BIT3
#define   BWR_EN                    BIT4
#define   BRR_EN                    BIT5
#define   ERROR_INT_STSATUS_EN_ALL  (0xFFFF << 16)
#define   CTO_EN                    BIT16       // 036h bit0
#define   CCRC_EN                   BIT17       // 036h bit1
#define   CEB_EN                    BIT18       // 036h bit2
#define   CIE_EN                    BIT19       // 036h bit3
#define   DTO_EN                    BIT20       // 036h bit4
#define   DCRC_EN                   BIT21       // 036h bit5
#define   DEB_EN                    BIT22       // 036h bit6
#define   CLME_EN                   BIT23       // 036h bit7
#define   ACMDE_EN                  BIT24       // 036h bit8
#define   ADMAERR_EN                BIT25       // 036h bit9
#define   CERR_EN                   BIT28       // 036h bit12
#define   BADA_EN                   BIT29       // 036h bit13
#define ERROR_INT_STATUS_ENABLE     0x36
#define INT_SIGNAL_ENABLE           0x38
#define   CC_SIGEN                  BIT0
#define   TC_SIGEN                  BIT1
#define   BWR_SIGEN                 BIT4
#define   BRR_SIGEN                 BIT5
#define   CTO_SIGEN                 BIT16       // 03Ah bit0
#define   CCRC_SIGEN                BIT17       // 03Ah bit1
#define   CEB_SIGEN                 BIT18       // 03Ah bit2
#define   CIE_SIGEN                 BIT19       // 03Ah bit3
#define   DTO_SIGEN                 BIT20       // 03Ah bit4
#define   DCRC_SIGEN                BIT21       // 03Ah bit5
#define   DEB_SIGEN                 BIT22       // 03Ah bit6
#define   CERR_SIGEN                BIT28       // 03Ah bit12
#define   BADA_SIGEN                BIT29       // 03Ah bit13
#define ERROR_INT_SIGNAL_ENABLE     0x3A
#define AUTO_CMD12_ERROR_STATUS     0x3C
#define HOST_CONTROL_2              0x3E
#define   V18_SIGNALING             BIT3
#define CAPABILITIES                0x40
#define VS33                        BIT24
#define VS30                        BIT25
#define VS18                        BIT26
#define MAX_CURRENT_CAPABILITIES    0x48
#define FORCE_EVENT_FOR_CMD12       0x50
#define FORCE_EVENT_FOR_ERR_INT     0x52
#define ADMA_ERROR_STATUS           0x54
#define ADMA_ADDR_LOW32             0x58
#define ADMA_ADDR_HIGH32            0x5C
#define SLOT_INT_STATUS             0xFC
#define HOST_CONTROLLER_VER         0xFE

#define SD_HOST_SPEC_VER_100        0x00
#define SD_HOST_SPEC_VER_200        0x01
#define SD_HOST_SPEC_VER_300        0x02

//
// Commands
//
#define  CMD0              0
#define  CMD1              1
#define  CMD2              2
#define  CMD3              3
#define  CMD4              4
#define  CMD5              5
#define  CMD6              6
#define  CMD7              7
#define  CMD8              8
#define  CMD9              9
#define  CMD10             10
#define  CMD11             11
#define  CMD12             12
#define  CMD13             13
#define  CMD14             14
#define  CMD15             15
#define  CMD16             16
#define  CMD17             17
#define  CMD18             18
#define  CMD19             19
#define  CMD20             20
#define  CMD23             23
#define  CMD24             24
#define  CMD25             25
#define  CMD26             26
#define  CMD27             27
#define  CMD28             28
#define  CMD29             29
#define  CMD30             30
#define  CMD35             35
#define  CMD36             36
#define  CMD38             38
#define  CMD39             39
#define  CMD40             40
#define  CMD42             42
#define  CMD52             52
#define  CMD53             53
#define  CMD55             55
#define  CMD56             56

#define  ACMD6             6
#define  ACMD13            13
#define  ACMD23            23 
#define  ACMD41            41
#define  ACMD42            42
#define  ACMD51            51

#define CHECK_PATTERN      0xAA
#define VOLTAGE_27_36      0x1
//#define CMD8_ARG          (0x0 << 12 | BIT8 | 0xAA << 0)  // Reserved(0)[12:31], Supply voltage(1)[11:8], check pattern(0xCE)[7:0] = 0xAA
#define HCS                BIT30    // For ACMD41: Host capacity support/1 = Supporting high capacity
#define CLOCK_400KHZ       400
#define CLOCK_26MHZ        26000
#define BUS_1BIT           0x00
#define BUS_4BIT           0x02
//
// DMA related definitions
//
#define SDMA_BOUNDARY_SIZE          (4*1024)                //4k. In CloverTrail, it's 512k.
#define MAX_TRANS_SIZE_PER_ENTR     0x10000

#define ADMA1_PAGE_SIZE             0x1000
#define ADMA2_64BIT_ALIGN           0x0008
#define ADMA2_32BIT_ALIGN           0x0004
#define ADMA1_32BIT_ALIGN           ADMA1_PAGE_SIZE
#define SDMA_ALIGN                  ADMA2_32BIT_ALIGN       //Minimum of ADMA alignment.

//
// Card Status in ResponseR1
//
#define ADDRESS_OUT_OF_RANGE    BIT31
#define ADDRESS_MISALIGN        BIT30
#define BLOCK_LEN_ERROR         BIT29
#define ERASE_SEQ_ERROR         BIT28 
#define ERASE_PARAM             BIT27 
#define WP_VIOLATION            BIT26 
#define CARD_IS_LOCKED          BIT25 
#define LOCK_UNLOCK_FAILED      BIT24 
#define COM_CRC_ERROR           BIT23 
#define ILLEGAL_COMMAND         BIT22 
#define CARD_ECC_FAILED         BIT21 
#define CC_ERROR                BIT20 
#define UNKNOWN_ERROR           BIT19 
#define CSD_OVERWRITE           BIT16 
#define WP_ERASE_SKIP           BIT15 
#define CARD_ECC_DISABLED       BIT14 
#define ERASE_RESET             BIT13 
#define CURRENT_STATE           (BIT12 | BIT11 | BIT10 | BIT9) 
#define READY_FOR_DATA          BIT8 
#define APP_CMD_STATUS          BIT5 
#define AKE_SEQ_ERROR           BIT3 

#define INDX(CMD_INDX)    ((CMD_INDX & 0x3F) << 24)

//
// Capability Slot Type
//
#define REMOVABLE_CARD_SLOT                0
#define EMBEDDED_SLOT_FOR_ONE_DEVICE       1
#define SHARED_BUS_SLOT                    2

#define MAX_SLOT_NUM                       6

#pragma pack(1)

typedef struct {
  UINT32    Valid         :1;
  UINT32    End           :1;
  UINT32    Int           :1;
  UINT32    Reserved0     :1;
  UINT32    Act           :2;
  UINT32    Reserved      :6;
  UINT32    AddrOrDataLen :20;
} ADMA1_DESCRIPTOR_TABLE;

typedef struct {
  UINT32    Valid     :1;
  UINT32    End       :1;
  UINT32    Int       :1;
  UINT32    Reserved0 :1;
  UINT32    Act       :2;
  UINT32    Reserved  :10;
  UINT32    Length    :16;
  UINT32    Address;
} ADMA2_32BIT_DESCRIPTOR_TABLE;

typedef struct {
  UINT32    Valid     :1;
  UINT32    End       :1;
  UINT32    Int       :1;
  UINT32    Reserved0 :1;
  UINT32    Act       :2;
  UINT32    Reserved  :10;
  UINT32    Length    :16;
  UINT64    Address;
} ADMA2_64BIT_DESCRIPTOR_TABLE;

typedef enum {
  NoOperation    = 0,
  Reserved       = 1,
  SetDataLength  = 1,
  TransferData   = 2,
  LinkDescriptor = 3
} ADMA_ACTION_SYMBOLS;

typedef struct {
  UINT32  TimeoutClockFreq  :6;
  UINT32  Reserved_0        :1;
  UINT32  TimeoutClockUnit  :1;
  UINT32  SDBaseClockFreq   :8;
  UINT32  MaxBlockLenth     :2;
  UINT32  ExtBusWidth       :1;
  UINT32  ADMA2Support      :1;
  UINT32  ADMA1Support      :1;
  UINT32  HighSpeedSupport  :1;
  UINT32  SDMASupport       :1;
  UINT32  SusResSupport     :1;
  UINT32  Support3_3V       :1;
  UINT32  Support3_0V       :1;
  UINT32  Support1_8V       :1;
  UINT32  Reserved_1        :1;
  UINT32  Support64bitBus   :1;
  UINT32  AsyncIntSupport   :1;
  UINT32  SlotType          :2;
  UINT32  SDR50Support      :1;      
  UINT32  SDR104Support     :1;  
  UINT32  DDR50Support      :1; 
  UINT32  Reserved_2        :1;
  UINT32  TypeASupport      :1; 
  UINT32  TypeCSupport      :1; 
  UINT32  TypeDSupport      :1; 
  UINT32  Reserved_3        :1;
  UINT32  ReTuneTimer       :4;
  UINT32  Reserved_4        :1;
  UINT32  SDR50Tuning       :1;
  UINT32  ReTuneMode        :2; 
  UINT32  ClockMultipler    :8;
  UINT32  Reserved_5        :8; 
} HOST_CAPABILITY;

typedef enum {
  SdHostIoWidthUint8      = 0,
  SdHostIoWidthUint16,
  SdHostIoWidthUint32,
  SdHostIoWidthUint64
} H2O_SD_HOST_IO_WIDTH;

typedef enum {
  ResponseNo = 0,
  ResponseR1,
  ResponseR1b,
  ResponseR2,
  ResponseR3,
  ResponseR4,
  ResponseR5,
  ResponseR5b,
  ResponseR6,
  ResponseR7
} RESPONSE_TYPE;

typedef enum {
  NoData = 0,
  InData,
  OutData
} TRANSFER_TYPE;

typedef enum {
  ResetAuto = 0,
  ResetDat,
  ResetCmd,
  ResetDatCmd,
  ResetAll,
} RESET_TYPE;

#pragma pack()

/**
  Reads data from specific host register.
  
  @param[in]           This            Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]           Width           Data width
  @param[in]           Offset          Offset of host register
  @param[in,out]       Buffer          Pointer to output
  
  @retval EFI_SUCCESS                  Operation succeed
  @retval EFI_UNSUPPORTED              Unsupported data width

**/            
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_READ) (
  IN     H2O_SD_HOST_IO_PPI          *This,
  IN     H2O_SD_HOST_IO_WIDTH        Width,
  IN     UINT64                      Offset,
  IN OUT VOID                        *Buffer                      
  );

/**
  Writes data to specific host register.
  
  @param[in]           This            Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]           Width           Data width
  @param[in]           Offset          Offset of host register
  @param[in]           Buffer          Pointer to input data
  
  @retval EFI_SUCCESS                  Operation succeed
  @retval EFI_UNSUPPORTED              Unsupported data width

**/          
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_WRITE) (
  IN H2O_SD_HOST_IO_PPI              *This,
  IN H2O_SD_HOST_IO_WIDTH            Width,
  IN UINT64                          Offset,
  IN VOID                            *Buffer                      
  );

/**
  Executes a logic OR operation on Data and specific host register.
  
  @param[in]           This            Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]           Width           Data width
  @param[in]           Offset          Offset of host register
  @param[in]           Data            Input data
  
  @retval EFI_SUCCESS                  Operation succeed
  @retval EFI_UNSUPPORTED              Unsupported data width

**/           
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_OR) (
  IN  H2O_SD_HOST_IO_PPI            *This,
  IN  H2O_SD_HOST_IO_WIDTH          Width,
  IN  UINT64                        Offset,
  IN  UINTN                         Data                      
  );

/**
  Executes a logic AND operation on Data and specific host register.
  
  @param[in]           This            Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]           Width           Data width
  @param[in]           Offset          Offset of host register
  @param[in]           Data            Input data
  
  @retval EFI_SUCCESS                  Operation succeed
  @retval EFI_UNSUPPORTED              Unsupported data width

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_AND) (
  IN  H2O_SD_HOST_IO_PPI           *This,
  IN  H2O_SD_HOST_IO_WIDTH          Width,
  IN  UINT64                        Offset,
  IN  UINTN                         Data                      
  );

/**
  The main function used to send the command to the card inserted into the SD host
  slot.
  It will assemble the arguments to set the command register and wait for the command 
  and transfer completed until timeout. Then it will read the response register to fill
  the ResponseData 
 
  @param[in]      This             Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]      CommandIndex     The command index to set the command index field of command register
  @param[in]      Argument         Command argument to set the argument field of command register
  @param[in]      DataType         TRANSFER_TYPE, indicates no data, data in or data out
  @param[in]      Buffer           Contains the data read from / write to the device
  @param[in]      BufferSize       The size of the buffer
  @param[in]      ResponseType     RESPONSE_TYPE
  @param[in]      TimeOut          Time out value in 1 ms unit
  @param[in]      ResponseData     Depending on the ResponseType, such as CSD or card status

  @retval EFI_SUCCESS              Operation succeed
  @retval EFI_UNSUPPORTED          Transfer mode is unsupported
  @retval EFI_DEVICE_ERROR         Data transfer error
  @retval EFI_TIMEOUT              Data transfer time-out

**/   
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SEND_COMMAND) (
  IN   H2O_SD_HOST_IO_PPI          *This,
  IN   UINT16                      CommandIndex,
  IN   UINT32                      Argument,
  IN   TRANSFER_TYPE               DataType,
  IN   UINT8                       *Buffer, OPTIONAL
  IN   UINT32                      BufferSize,    
  IN   RESPONSE_TYPE               ResponseType,
  IN   UINT32                      TimeOut,
  OUT  UINT32                      *ResponseData OPTIONAL
  );

/**
  Set max clock frequency of the host, the actual frequency 
  may not be the same as MaxFrequencyInKHz. It depends on 
  the max frequency the host can support, divider, and host 
  speed mode. 
 
  @param[in]      This                  Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in]      MaxFrequencyInKHz     Max frequency in Khz

  @retval EFI_SUCCESS                   Operation succeed
  @retval EFI_TIMEOUT                   Data transfer time-out

**/     
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SET_CLOCK_FREQUENCY) (
  IN  H2O_SD_HOST_IO_PPI          *This,
  IN  UINT32                      *MaxFrequency          
  );

/**
  Set bus width of the host.
 
  @param[in]      This           Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in]      BusWidth       Bus width in 1, 4, 8 bits

  @retval EFI_SUCCESS            Operation succeed
  @retval EFI_INVALID_PARAMETER  Invalid bus width
  @retval EFI_UNSUPPORTED        Target width is unsupported

**/       
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SET_BUS_WIDTH) (
  IN  H2O_SD_HOST_IO_PPI          *This,
  IN  UINT32                      BusWidth          
  );

/**
  Set voltage which could supported by the host.
  Support 0(Power off the host), 1.8V, 3.0V, 3.3V

  @param[in]      This           Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in]      Voltage        Units in 0.1 V

  @retval EFI_SUCCESS            Operation succeed
  @retval EFI_UNSUPPORTED        Target voltage is unsupported

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SET_HOST_VOLTAGE) (
  IN  H2O_SD_HOST_IO_PPI          *This,
  IN  UINT32                      Voltage                       
  );

/**
  Reset the host.
  
  @param[in]      This           Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in]      ResetType      RESET_TYPE

  @retval EFI_SUCCESS            Operation succeed
  @retval EFI_TIMEOUT            Operation time-out

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_RESET_SD_HOST) (
  IN  H2O_SD_HOST_IO_PPI          *This,
  IN  RESET_TYPE                  ResetType   
  );

/**
  Find whether these is a card inserted into the slot. If so
  init the host. If not, return EFI_NOT_FOUND.
  
  @param[in]      This           Pointer to EFI_SD_HOST_IO_PROTOCOL

  @retval EFI_SUCCESS            Operation succeed
  @retval EFI_NOT_FOUND          No card is inserted

**/
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_DETECT_CARD_AND_INIT_HOST) (
  IN  H2O_SD_HOST_IO_PPI         *This
  );

/**
  Set Host mode in DDR.
  
  @param[in]      This           Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]      DdrMode        TRUE for DDR Mode set, FALSE returns EFI_SUCCESS

  @retval EFI_SUCCESS            Operation succeed

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SET_HOST_DDR_MODE) (
  IN  H2O_SD_HOST_IO_PPI         *This,
  IN  BOOLEAN                    DdrMode                       
  );

/**
  Set Host in High Speed.
  
  @param[in]      This           Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]      HighSpeed      TRUE for High Speed Mode set, FALSE for normal mode

  @retval EFI_SUCCESS            Operation succeed

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_SET_HOST_SPEED_MODE) (
  IN  H2O_SD_HOST_IO_PPI         *This,
  IN  BOOLEAN                    HighSpeed                       
  );

/**
  Set auto CMD12
  
  @param[in]      This           Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in]      Enable         TRUE for auto CMD12 set, FALSE for auto CMD12 not set

  @retval EFI_SUCCESS            Operation succeed

**/   
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_ENABLE_AUTO_STOP_CMD) (
  IN  H2O_SD_HOST_IO_PPI         *This,
  IN  BOOLEAN                    Enable   
  );

/**
  Get slot number.
  
  @param[in]           This         Pointer to H2O_SD_HOST_IO_PROTOCOL
  @param[in,out]       SlotNum      Pointer to slot number

  @retval EFI_SUCCESS               Operation succeed
  @retval EFI_INVALID_PARAMETER     Invalid input

**/         
typedef
EFI_STATUS
(EFIAPI *H2O_SD_HOST_IO_PPI_GET_SLOT_NUM) (
  IN H2O_SD_HOST_IO_PPI          *This,
  IN OUT UINT8                   *SlotNum   
  );

struct _H2O_SD_HOST_IO_PPI {
  UINT32                                       Revision;
  HOST_CAPABILITY                              HostCapability;
  H2O_SD_HOST_IO_PPI_READ                      SdHostRead;
  H2O_SD_HOST_IO_PPI_WRITE                     SdHostWrite;
  H2O_SD_HOST_IO_PPI_OR                        SdHostOr;
  H2O_SD_HOST_IO_PPI_AND                       SdHostAnd;
  H2O_SD_HOST_IO_PPI_SEND_COMMAND              SendCommand;
  H2O_SD_HOST_IO_PPI_SET_CLOCK_FREQUENCY       SetClockFrequency;
  H2O_SD_HOST_IO_PPI_SET_BUS_WIDTH             SetBusWidth;
  H2O_SD_HOST_IO_PPI_SET_HOST_VOLTAGE          SetHostVoltage;
  H2O_SD_HOST_IO_PPI_RESET_SD_HOST             ResetSdHost;
  H2O_SD_HOST_IO_PPI_DETECT_CARD_AND_INIT_HOST DetectCardAndInitHost;
  H2O_SD_HOST_IO_PPI_SET_HOST_SPEED_MODE       SetHostSpeedMode;
  H2O_SD_HOST_IO_PPI_SET_HOST_DDR_MODE         SetHostDdrMode;    
  H2O_SD_HOST_IO_PPI_ENABLE_AUTO_STOP_CMD      EnableAutoStopCmd;
  H2O_SD_HOST_IO_PPI_GET_SLOT_NUM              GetSlotNum;
};

extern EFI_GUID gH2OSdHostIoPpiGuid;

#endif
