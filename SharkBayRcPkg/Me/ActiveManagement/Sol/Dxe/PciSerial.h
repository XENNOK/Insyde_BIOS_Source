/** @file

  Include for Pci Serial Driver

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

#ifndef _PCI_SERIAL_H
#define _PCI_SERIAL_H

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>

#include <Library/Amt/Dxe/AmtLib.h>

///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/PciIo.h>

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/SerialIo.h>

///
/// Internal Data Structures
///
#define SERIAL_DEV_SIGNATURE    SIGNATURE_32 ('s', 'e', 'r', 'd')
#define SERIAL_MAX_BUFFER_SIZE  16
#define TIMEOUT_STALL_INTERVAL  300

///
///  Name:   SERIAL_DEV_FIFO
///  Purpose:  To define Receive FIFO and Transmit FIFO
///  Context:  Used by serial data transmit and receive
///  Fields:
///      First UINT32: The index of the first data in array Data[]
///      Last  UINT32: The index, which you can put a new data into array Data[]
///      Surplus UINT32: Identify how many data you can put into array Data[]
///      Data[]  UINT8 : An array, which used to store data
///
typedef struct {
  UINT32  First;
  UINT32  Last;
  UINT32  Surplus;
  UINT8   Data[SERIAL_MAX_BUFFER_SIZE];
} SERIAL_DEV_FIFO;

typedef enum {
  UART8250  = 0,
  UART16450 = 1,
  UART16550 = 2,
  UART16550A= 3
} EFI_UART_TYPE;

///
///  Name:   SERIAL_DEV
///  Purpose:  To provide device specific information
///  Context:
///  Fields:
///      Signature   UINTN: The identity of the serial device
///      SerialIo    SERIAL_IO_PROTOCOL: Serial I/O protocol interface
///      SerialMode  SERIAL_IO_MODE:
///      DevicePath  EFI_DEVICE_PATH_PROTOCOL *: Device path of the serial device
///      Handle      EFI_HANDLE: The handle instance attached to serial device
///      BarIndex    UINT16: The bar index in pci cfg space that contains the base address
///                  of specific serial device
///      Receive     SERIAL_DEV_FIFO: The FIFO used to store data,
///                  which is received by UART
///      Transmit    SERIAL_DEV_FIFO: The FIFO used to store data,
///                  which you want to transmit by UART
///      SoftwareLoopbackEnable BOOLEAN:
///      Type    EFI_UART_TYPE: Specify the UART type of certain serial device
///
typedef struct {
  UINTN                     Signature;

  EFI_HANDLE                Handle;
  EFI_SERIAL_IO_PROTOCOL    SerialIo;
  EFI_SERIAL_IO_MODE        SerialMode;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  EFI_DEVICE_PATH_PROTOCOL  *ParentDevicePath;
  UART_DEVICE_PATH          UartDevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;

  UINT16                    BarIndex;
  SERIAL_DEV_FIFO           Receive;
  SERIAL_DEV_FIFO           Transmit;
  BOOLEAN                   SoftwareLoopbackEnable;
  BOOLEAN                   HardwareFlowControl;
  EFI_UART_TYPE             Type;
  EFI_UNICODE_STRING_TABLE  *ControllerNameTable;
} SERIAL_DEV;

#define SERIAL_DEV_FROM_THIS(a) CR (a, SERIAL_DEV, SerialIo, SERIAL_DEV_SIGNATURE)

///
/// Globale Variables
///
extern EFI_DRIVER_BINDING_PROTOCOL  mPciSerialControllerDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  mPciSerialComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL mPciSerialComponentName2;

///
/// Serial Driver Defaults
///
#define SERIAL_PORT_DEFAULT_BAUD_RATE           115200
#define SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH  1
#define SERIAL_PORT_DEFAULT_TIMEOUT             1000000 ///< 1 seconds
#define SERIAL_PORT_DEFAULT_PARITY              NoParity
#define SERIAL_PORT_DEFAULT_DATA_BITS           8
#define SERIAL_PORT_DEFAULT_STOP_BITS           1
#define SERIAL_PORT_DEFAULT_CONTROL_MASK        EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE

///
/// (24000000/13)MHz input clock
///
#define SERIAL_PORT_INPUT_CLOCK 1843200

///
/// 115200 baud with rounding errors
///
#define SERIAL_PORT_MAX_BAUD_RATE           115400
#define SERIAL_PORT_MIN_BAUD_RATE           50

#define SERIAL_PORT_MAX_RECEIVE_FIFO_DEPTH  16
#define SERIAL_PORT_MIN_TIMEOUT             1           ///< 1 uS
#define SERIAL_PORT_MAX_TIMEOUT             10000000000 ///< 10000 seconds
///
/// UART Registers
///
#define SERIAL_REGISTER_THR           0 ///< WO   Transmit Holding Register
#define SERIAL_REGISTER_RBR           0 ///< RO   Receive Buffer Register
#define SERIAL_REGISTER_DLL           0 ///< R/W  Divisor Latch LSB
#define SERIAL_REGISTER_DLM           1 ///< R/W  Divisor Latch MSB
#define SERIAL_REGISTER_IER           1 ///< R/W  Interrupt Enable Register
#define SERIAL_REGISTER_IIR           2 ///< RO   Interrupt Identification Register
#define SERIAL_REGISTER_FCR           2 ///< WO   FIFO Cotrol Register
#define SERIAL_REGISTER_LCR           3 ///< R/W  Line Control Register
#define SERIAL_REGISTER_MCR           4 ///< R/W  Modem Control Register
#define SERIAL_REGISTER_LSR           5 ///< R/W  Line Status Register
#define SERIAL_REGISTER_MSR           6 ///< R/W  Modem Status Register
#define SERIAL_REGISTER_SCR           7 ///< R/W  Scratch Pad Register
#define TEKOA_SOL_VENDOR_ID           0x8086
#define TEKOA_SOL_DEVICE_ID           0x108F

#define PCI_CLASS_SCC                 0x07
#define PCI_SUBCLASS_SERIAL           0x00
#define PCI_IF_GENERIC_XT             0x00
#define PCI_IF_16450                  0x01
#define PCI_IF_16550                  0x02
#define PCI_IF_16650                  0x03
#define PCI_IF_16750                  0x04
#define PCI_IF_16850                  0x05
#define PCI_IF_16950                  0x06
#define PCI_SUBCLASS_PARALLEL         0x01
#define PCI_SUBCLASS_MULTIPORT_SERIAL 0x02
#define PCI_SUBCLASS_MODEM            0x03
#define PCI_SUBCLASS_OTHER            0x80

#pragma pack(1)
///
///  Name:   SERIAL_PORT_IER_BITS
///  Purpose:  Define each bit in Interrupt Enable Register
///  Context:
///  Fields:
///     RAVIE  Bit0: Receiver Data Available Interrupt Enable
///     THEIE  Bit1: Transmistter Holding Register Empty Interrupt Enable
///     RIE      Bit2: Receiver Interrupt Enable
///     MIE      Bit3: Modem Interrupt Enable
///     Reserved Bit4-Bit7: Reserved
///
typedef struct {
  UINT8 RAVIE : 1;
  UINT8 THEIE : 1;
  UINT8 RIE : 1;
  UINT8 MIE : 1;
  UINT8 Reserved : 4;
} SERIAL_PORT_IER_BITS;

///
///  Name:   SERIAL_PORT_IER
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_IER_BITS:  Bits of the IER
///      Data    UINT8: the value of the IER
///
typedef union {
  SERIAL_PORT_IER_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_IER;

///
///  Name:   SERIAL_PORT_IIR_BITS
///  Purpose:  Define each bit in Interrupt Identification Register
///  Context:
///  Fields:
///      IPS    Bit0: Interrupt Pending Status
///      IIB    Bit1-Bit3: Interrupt ID Bits
///      Reserved Bit4-Bit5: Reserved
///      FIFOES   Bit6-Bit7: FIFO Mode Enable Status
///
typedef struct {
  UINT8 IPS : 1;
  UINT8 IIB : 3;
  UINT8 Reserved : 2;
  UINT8 FIFOES : 2;
} SERIAL_PORT_IIR_BITS;

///
///  Name:   SERIAL_PORT_IIR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_IIR_BITS:  Bits of the IIR
///      Data    UINT8: the value of the IIR
///
typedef union {
  SERIAL_PORT_IIR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_IIR;

///
///  Name:   SERIAL_PORT_FCR_BITS
///  Purpose:  Define each bit in FIFO Control Register
///  Context:
///  Fields:
///      TRFIFOE    Bit0: Transmit and Receive FIFO Enable
///      RESETRF    Bit1: Reset Reciever FIFO
///      RESETTF    Bit2: Reset Transmistter FIFO
///      DMS        Bit3: DMA Mode Select
///      Reserved   Bit4-Bit5: Reserved
///      RTB        Bit6-Bit7: Receive Trigger Bits
///
typedef struct {
  UINT8 TRFIFOE : 1;
  UINT8 RESETRF : 1;
  UINT8 RESETTF : 1;
  UINT8 DMS : 1;
  UINT8 Reserved : 2;
  UINT8 RTB : 2;
} SERIAL_PORT_FCR_BITS;

///
///  Name:   SERIAL_PORT_FCR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_FCR_BITS:  Bits of the FCR
///      Data    UINT8: the value of the FCR
///
typedef union {
  SERIAL_PORT_FCR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_FCR;

///
///  Name:   SERIAL_PORT_LCR_BITS
///  Purpose:  Define each bit in Line Control Register
///  Context:
///  Fields:
///      SERIALDB  Bit0-Bit1: Number of Serial Data Bits
///      STOPB   Bit2: Number of Stop Bits
///      PAREN   Bit3: Parity Enable
///      EVENPAR   Bit4: Even Parity Select
///      STICPAR   Bit5: Sticky Parity
///      BRCON   Bit6: Break Control
///      DLAB    Bit7: Divisor Latch Access Bit
///
typedef struct {
  UINT8 SERIALDB : 2;
  UINT8 STOPB : 1;
  UINT8 PAREN : 1;
  UINT8 EVENPAR : 1;
  UINT8 STICPAR : 1;
  UINT8 BRCON : 1;
  UINT8 DLAB : 1;
} SERIAL_PORT_LCR_BITS;

///
///  Name:   SERIAL_PORT_LCR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_LCR_BITS:  Bits of the LCR
///      Data    UINT8: the value of the LCR
///
typedef union {
  SERIAL_PORT_LCR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_LCR;

///
///  Name:   SERIAL_PORT_MCR_BITS
///  Purpose:  Define each bit in Modem Control Register
///  Context:
///  Fields:
///      DTRC     Bit0: Data Terminal Ready Control
///      RTS      Bit1: Request To Send Control
///      OUT1     Bit2: Output1
///      OUT2     Bit3: Output2, used to disable interrupt
///      LME;     Bit4: Loopback Mode Enable
///      Reserved Bit5-Bit7: Reserved
///
typedef struct {
  UINT8 DTRC : 1;
  UINT8 RTS : 1;
  UINT8 OUT1 : 1;
  UINT8 OUT2 : 1;
  UINT8 LME : 1;
  UINT8 Reserved : 3;
} SERIAL_PORT_MCR_BITS;

///
///  Name:   SERIAL_PORT_MCR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_MCR_BITS:  Bits of the MCR
///      Data    UINT8: the value of the MCR
///
typedef union {
  SERIAL_PORT_MCR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_MCR;

///
///  Name:   SERIAL_PORT_LSR_BITS
///  Purpose:  Define each bit in Line Status Register
///  Context:
///  Fields:
///      DR    Bit0: Receiver Data Ready Status
///      OE    Bit1: Overrun Error Status
///      PE    Bit2: Parity Error Status
///      FE    Bit3: Framing Error Status
///      BI    Bit4: Break Interrupt Status
///      THRE  Bit5: Transmistter Holding Register Status
///      TEMT  Bit6: Transmitter Empty Status
///      FIFOE Bit7: FIFO Error Status
///
typedef struct {
  UINT8 DR : 1;
  UINT8 OE : 1;
  UINT8 PE : 1;
  UINT8 FE : 1;
  UINT8 BI : 1;
  UINT8 THRE : 1;
  UINT8 TEMT : 1;
  UINT8 FIFOE : 1;
} SERIAL_PORT_LSR_BITS;

///
///  Name:   SERIAL_PORT_LSR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_LSR_BITS:  Bits of the LSR
///      Data    UINT8: the value of the LSR
///
typedef union {
  SERIAL_PORT_LSR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_LSR;

///
///  Name:   SERIAL_PORT_MSR_BITS
///  Purpose:  Define each bit in Modem Status Register
///  Context:
///  Fields:
///      DeltaCTS      Bit0: Delta Clear To Send Status
///      DeltaDSR        Bit1: Delta Data Set Ready Status
///      TrailingEdgeRI  Bit2: Trailing Edge of Ring Indicator Status
///      DeltaDCD        Bit3: Delta Data Carrier Detect Status
///      CTS             Bit4: Clear To Send Status
///      DSR             Bit5: Data Set Ready Status
///      RI              Bit6: Ring Indicator Status
///      DCD             Bit7: Data Carrier Detect Status
///
typedef struct {
  UINT8 DeltaCTS : 1;
  UINT8 DeltaDSR : 1;
  UINT8 TrailingEdgeRI : 1;
  UINT8 DeltaDCD : 1;
  UINT8 CTS : 1;
  UINT8 DSR : 1;
  UINT8 RI : 1;
  UINT8 DCD : 1;
} SERIAL_PORT_MSR_BITS;

///
///  Name:   SERIAL_PORT_MSR
///  Purpose:
///  Context:
///  Fields:
///      Bits    SERIAL_PORT_MSR_BITS:  Bits of the MSR
///      Data    UINT8: the value of the MSR
///
typedef union {
  SERIAL_PORT_MSR_BITS  Bits;
  UINT8                 Data;
} SERIAL_PORT_MSR;

#pragma pack()
///
/// Define serial register I/O macros
///
#define READ_RBR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_RBR)
#define READ_DLL(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_DLL)
#define READ_DLM(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_DLM)
#define READ_IER(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_IER)
#define READ_IIR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_IIR)
#define READ_LCR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_LCR)
#define READ_MCR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_MCR)
#define READ_LSR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_LSR)
#define READ_MSR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_MSR)
#define READ_SCR(IO, B)       PciSerialReadPort (IO, B, SERIAL_REGISTER_SCR)

#define WRITE_THR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_THR, D)
#define WRITE_DLL(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_DLL, D)
#define WRITE_DLM(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_DLM, D)
#define WRITE_IER(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_IER, D)
#define WRITE_FCR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_FCR, D)
#define WRITE_LCR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_LCR, D)
#define WRITE_MCR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_MCR, D)
#define WRITE_LSR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_LSR, D)
#define WRITE_MSR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_MSR, D)
#define WRITE_SCR(IO, B, D)   PciSerialWritePort (IO, B, SERIAL_REGISTER_SCR, D)

#define PCI_SERIAL_PORT_NAME  "PCI Serial Port # "

#define R_PCI_SVID            0x2C

/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.

  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                 a pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested.
  @param[in] RemainingDevicePath  A pointer to the device path. Ignored by device
                                  driver but used by bus driver

  @retval EFI_SUCCESS             Have device to support
  @retval EFI_NOT_FOUND           The device doesn't support or relative environment not ready
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  This routine is called right after the .Supported() called
  and return EFI_SUCCESS. Notes: The supported protocols are
  checked but the Protocols are closed.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested. Parameter
                                  passed by the caller
  @param[in] RemainingDevicePath  A pointer to the device path. Should be ignored by
                                  device driver

  @retval EFI_SUCCESS             The driver ready and initial complete.
  @retval Other                   The device doesn't initial.
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Stop.

  @param[in] This                 Pointer to driver binding protocol
  @param[in] Controller           Controller handle to connect
  @param[in] NumberOfChildren     Number of children handle created by this driver
  @param[in] ChildHandleBuffer    Buffer containing child handle created

  @retval EFI_SUCCESS             Driver disconnected successfully from controller
  @retval EFI_DEVICE_ERROR        Cannot find BIOS_VIDEO_DEV structure
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

/**
  Detect whether specific FIFO is full or not

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO

  @retval TRUE                    The FIFO is full
  @retval FALSE                   The FIFO is not full
**/
BOOLEAN
PciSerialFifoFull (
  IN SERIAL_DEV_FIFO              *Fifo
  );

/**
  Detect whether specific FIFO is empty or not

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO

  @retval TRUE                    The FIFO is empty
  @retval FALSE                   The FIFO is not empty
**/
BOOLEAN
PciSerialFifoEmpty (
  IN SERIAL_DEV_FIFO              *Fifo
  );

/**
  Add data to specific FIFO

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in] Data                 The data added to FIFO

  @retval EFI_SUCCESS             Add data to specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to add data because FIFO is already full
**/
EFI_STATUS
PciSerialFifoAdd (
  IN SERIAL_DEV_FIFO              *Fifo,
  IN UINT8                        Data
  );

/**
  Remove data from specific FIFO

  @param[in] Fifo                 A pointer to the Data Structure SERIAL_DEV_FIFO
  @param[in] Data                 The data removed from FIFO

  @retval EFI_SUCCESS             Remove data from specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to remove data because FIFO is empty
**/
EFI_STATUS
PciSerialFifoRemove (
  IN  SERIAL_DEV_FIFO             *Fifo,
  OUT UINT8                       *Data
  );

/**
  Reads and writes all avaliable data.

  @param[in] SerialDevice         The device to flush

  @retval EFI_SUCCESS             Data was read/written successfully.
  @retval EFI_OUT_OF_RESOURCES    Failed because software receive FIFO is full. Note, when
                                  this happens, pending writes are not done.
**/
EFI_STATUS
PciSerialReceiveTransmit (
  IN SERIAL_DEV                   *SerialDevice
  );

///
/// Serial I/O Protocol Interface
///
/**
  Reset serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL

  @retval EFI_SUCCESS             Reset successfully
  @retval EFI_DEVICE_ERROR        Failed to reset
**/
EFI_STATUS
EFIAPI
PciSerialReset (
  IN EFI_SERIAL_IO_PROTOCOL  *This
  );

/**
  Set new attributes to a serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BaudRate             The baudrate of the serial device
  @param[in] ReceiveFifoDepth     Fifo depth
  @param[in] Timeout              The request timeout for a single char
  @param[in] Parity               The type of parity used in serial device
  @param[in] DataBits             Number of databits used in serial device
  @param[in] StopBits             Number of stopbits used in serial device

  @retval EFI_SUCCESS             The new attributes were set
  @retval EFI_INVALID_PARAMETERS  One or more attributes have an unsupported value
  @exception EFI_UNSUPPORTED      Data Bits can not set to 5 or 6
  @retval EFI_DEVICE_ERROR        The serial device is not functioning correctly (no return)
**/
EFI_STATUS
EFIAPI
PciSerialSetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN UINT64                       BaudRate,
  IN UINT32                       ReceiveFifoDepth,
  IN UINT32                       Timeout,
  IN EFI_PARITY_TYPE              Parity,
  IN UINT8                        DataBits,
  IN EFI_STOP_BITS_TYPE           StopBits
  );

/**
  Set ControlBits

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] Control              Control bits that can be settable

  @retval EFI_SUCCESS             New Control bits were set successfully
  @retval EFI_UNSUPPORTED         The Control bits wanted to set are not supported
**/
EFI_STATUS
EFIAPI
PciSerialSetControl (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN UINT32                       Control
  );

/**
  Get ControlBits

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] Control              Control signals of the serial device

  @retval EFI_SUCCESS             Get Control signals successfully
**/
EFI_STATUS
EFIAPI
PciSerialGetControl (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  OUT UINT32                      *Control
  );

/**
  Write the specified number of bytes to serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BufferSize           On input the size of Buffer, on output the amount of data actually written
  @param[in] Buffer               The buffer of data to write

  @retval EFI_SUCCESS             The data were written successfully
  @retval EFI_DEVICE_ERROR        The device reported an error
  @retval EFI_TIMEOUT             The write operation was stopped due to timeout
**/
EFI_STATUS
EFIAPI
PciSerialWrite (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN OUT UINTN                    *BufferSize,
  IN VOID                         *Buffer
  );

/**
  Read the specified number of bytes from serial device

  @param[in] This                 Pointer to EFI_SERIAL_IO_PROTOCOL
  @param[in] BufferSize           On input the size of Buffer, on output the amount of data returned in buffer
  @param[in] Buffer               The buffer to return the data into

  @retval EFI_SUCCESS             The data were read successfully
  @retval EFI_DEVICE_ERROR        The device reported an error
  @retval EFI_TIMEOUT             The read operation was stopped due to timeout
**/
EFI_STATUS
EFIAPI
PciSerialRead (
  IN EFI_SERIAL_IO_PROTOCOL       *This,
  IN OUT UINTN                    *BufferSize,
  OUT VOID                        *Buffer
  );

///
/// Internal Functions
///
/**
  Check serial port status.

  @param[in] SerialDevice         The serial device instance

  @retval True                    It is present.
  @retval False                   No present.
**/
BOOLEAN
PciSerialPortPresent (
  IN SERIAL_DEV                   *SerialDevice
  );

/**
  PCI I/O read for byte only

  @param[in] PciIo                Pointer of Pci IO protocol
  @param[in] BarIndex             Index of the BAR within PCI device
  @param[in] Offset               Offset of the BARIndex within PCI device

  @retval                         Return value read
**/
UINT8
PciSerialReadPort (
  IN EFI_PCI_IO_PROTOCOL          *PciIo,
  IN UINT16                       BarIndex,
  IN UINT16                       Offset
  );

/**
  PCI I/O - write a byte

  @param[in] PciIo                Pointer of Pci IO protocol
  @param[in] BarIndex             Index of the BAR within PCI device
  @param[in] Offset               Offset of the BARIndex within PCI device
  @param[in] Data                 Written value
**/
VOID
PciSerialWritePort (
  IN EFI_PCI_IO_PROTOCOL          *PciIo,
  IN UINT16                       BarIndex,
  IN UINT16                       Offset,
  IN UINT8                        Data
  );
/**
  Sol driver entry

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PciSerialControllerDriverEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  );
#endif
