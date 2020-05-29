//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EVENT_LOG_HOB_H_
#define _EVENT_LOG_HOB_H_

#define EFI_EVENT_LOG_STATUS_VARIABLE_NAME     L"EventLogStatus"

#define EVENT_LOG_HOB_GUID  \
  { \
    0x5106ba40, 0x59a7, 0x4bf6, 0x97, 0xfc, 0xa7, 0xde, 0x16, 0x75, 0xcb, 0x86 \
  }

typedef UINT32  EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD;
typedef UINT32  EVENT_LOG_POST_ERROR_BITMAP_SECOND_DWORD;

typedef struct {

  //
  //Log Header is optional, default is no header
  //
  UINT8   Type;
  UINT8   Length;

  UINT8   Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;

  UINT8   Data[8];
  //
  //Log Variable Data is optional
  //
} EVENT_LOG_ORGANIZATION;

#define EVENT_LOG_BASE_LENGTH                               0x08

#define EVENT_LOG_DATA_FORMAT_NONE_LENGTH                   0x00
#define EVENT_LOG_DATA_FORMAT_HANDLE_LENGTH                 0x02
#define EVENT_LOG_DATA_FORMAT_MULTI_EVENT_LENGTH            0x04
#define EVENT_LOG_DATA_FORMAT_MULTI_EVENT_HANDLE_LENGTH     0x06
#define EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH     0x08
#define EVENT_LOG_DATA_FORMAT_SYSTEM_MANAGEMENT_TYPE_LENGTH 0x04
#define EVENT_LOG_DATA_FORMAT_MULTI_EVENT_SYSTEM_MANAGEMENT_TYPE_LENGTH 0x08
//
//07h~7Fh Unused, available for assignment by SMBIOS spec.
//80h~FFh, OEM assigned. Available for system- and OEM-specific assignment.
//
#define bit(a)   (1 << (a))

//
// POST Results Bitmap First DWORD
//
#define CHANEL2_TIMER_ERROR         bit(0)
#define MASTER_PIC_ERROR            bit(1)
#define SLAVE_PIC_ERROR             bit(2)
#define CMOS_BATTERY_FAIL           bit(3)
#define CMOS_SYS_OPTION_NOT_SET     bit(4)
#define CMOS_CHECKSUM_ERROR         bit(5)
#define CMOS_CONFIG_ERROR           bit(6)
#define MOUSE_KYB_SWAP              bit(7)
#define KYB_LOCKED                  bit(8)
#define KYB_NOT_FUNCTIONAL          bit(9)
#define KYC_NOT_FUNCTIONAL          bit(10)
#define CMOS_SIZE_MEM_DIFFERENT     bit(11)
#define MEM_DECREASED_IN_SIZE       bit(12)
#define CACHE_MEM_ERROR             bit(13)
#define FLOPPY_DRIVE_0_ERROR        bit(14)
#define FLOPPY_DRIVE_1_ERROR        bit(15)
#define FLOPPY_CONTROLLER_ERROR     bit(16)
#define NUM_ATA_DRIVES_REDUCE_ERROR bit(17)
#define CMOS_TIME_NOT_SET           bit(18)
#define DDC_MONITOR_CONFIG_CHANGE   bit(19)
//
//20~23 Reserved, set to 0
//
#define SECOND_DWORD_VALID          bit(24)
//
//25~27 Reserved, set to 0
//28~31 Normally 0; OEM assignment
//

//
// POST Results Bitmap Second DWORD
//
//0~6 Normally 0; OEM assignment
#define PCI_MEMCONFLICT             bit(7)
#define PCI_IO_CONFLICT             bit(8)
#define PCI_IRQ_CONFLICT            bit(9)
#define PNP_MEM_CONFLICT            bit(10)
#define PNP_32BIT_MEM_CONFLICT      bit(11)
#define PNP_IO_CONFLICT             bit(12)
#define PNP_IRQ_CONFLICT            bit(13)
#define PNP_DMA_CONFLICT            bit(14)
#define BAD_PNPSERIAL_ID_CHECKSUM   bit(15)
#define BAD_RESOURCE_DATA_CHECKSUM  bit(16)
#define STATIC_RESOURCE_CONFLICT    bit(17)
#define NVRAM_CHECKSUM_ERROR        bit(18)
#define SYS_BOARD_DEVICERESOURCE_CONFLICT bit(19)
#define PRIMARY_OUTPUT_DEVICE_NOT_FOUND   bit(20)
#define PRIMARY_INPUT_DEVICE_NOT_FOUND    bit(21)
#define PRIMARY_BOOT_DEVICE_NOT_FOUND     bit(22)
#define NVRAM_CLEARED_BY_JUMPER     bit(23)
#define NVRAM_DATA_INVALID          bit(24)
#define FDC_RESOURCE_CONFLICT       bit(25)
#define PRIMARY_ATA_CONTROLLER_RESOURCE_CONFLICT   bit(26)
#define SECONDARY_ATA_CONTROLLER_RESOURCE_CONFLICT bit(27)
#define PARALLEL_PORT_RESOURCE_CONFLICT   bit(28)
#define SERIAL_PORT1_RESOURCE_CONFLICT    bit(29)
#define SERIAL_PORT2_RESOURCE_CONFLICT    bit(30)
#define AUDIO_RESOURCE_CONFLICT     bit(31)

extern EFI_GUID gEventLogHobGuid;

#endif
