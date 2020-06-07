。/** @file
  This sample application bases on HelloWorld PCD setting 
  to print "UEFI Hello World!" to the UEFI Console.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>


#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
//
// String token ID of help message text.
// Shell supports to find help message in the resource section of an application image if
// .MAN file is not found. This global variable is added to make build tool recognizes
// that the help string is consumed by user and then build tool will add the string into
// the resource section. Thus the application can use '-?' option to show help message in
// Shell.
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_HELLO_WORLD_HELP_INFORMATION);

typedef struct {
  UINT16 Mp;                /* Maximum Power */
  UINT8  Rsvd1;             /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Mps:1;             /* Max Power Scale */
  UINT8  Nops:1;            /* Non-Operational State */
  UINT8  Rsvd2:6;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Enlat;             /* Entry Latency */
  UINT32 Exlat;             /* Exit Latency */
  UINT8  Rrt:5;             /* Relative Read Throughput */
  UINT8  Rsvd3:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rrl:5;             /* Relative Read Leatency */
  UINT8  Rsvd4:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwt:5;             /* Relative Write Throughput */
  UINT8  Rsvd5:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwl:5;             /* Relative Write Leatency */
  UINT8  Rsvd6:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rsvd7[16];         /* Reserved as of Nvm Express 1.1 Spec */
} NVME_PSDESCRIPTOR;
//
//  Identify Controller Data
//
typedef struct {
  //
  // Controller Capabilities and Features 0-255
  //
  UINT16 Vid;                 /* PCI Vendor ID */
  UINT16 Ssvid;               /* PCI sub-system vendor ID */
  UINT8  Sn[20];              /* Product serial number */

  UINT8  Mn[40];              /* Proeduct model number */
  UINT8  Fr[8];               /* Firmware Revision */
  UINT8  Rab;                 /* Recommended Arbitration Burst */
  UINT8  Ieee_oui[3];         /* Organization Unique Identifier */
  UINT8  Cmic;                /* Multi-interface Capabilities */
  UINT8  Mdts;                /* Maximum Data Transfer Size */
  UINT8  Cntlid[2];           /* Controller ID */
  UINT8  Rsvd1[176];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Admin Command Set Attributes
  //
  UINT16 Oacs;                /* Optional Admin Command Support */
  UINT8  Acl;                 /* Abort Command Limit */
  UINT8  Aerl;                /* Async Event Request Limit */
  UINT8  Frmw;                /* Firmware updates */
  UINT8  Lpa;                 /* Log Page Attributes */
  UINT8  Elpe;                /* Error Log Page Entries */
  UINT8  Npss;                /* Number of Power States Support */
  UINT8  Avscc;               /* Admin Vendor Specific Command Configuration */
  UINT8  Apsta;               /* Autonomous Power State Transition Attributes */
  UINT8  Rsvd2[246];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // NVM Command Set Attributes
  //
  UINT8  Sqes;                /* Submission Queue Entry Size */
  UINT8  Cqes;                /* Completion Queue Entry Size */
  UINT16 Rsvd3;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Nn;                  /* Number of Namespaces */
  UINT16 Oncs;                /* Optional NVM Command Support */
  UINT16 Fuses;               /* Fused Operation Support */
  UINT8  Fna;                 /* Format NVM Attributes */
  UINT8  Vwc;                 /* Volatile Write Cache */
  UINT16 Awun;                /* Atomic Write Unit Normal */
  UINT16 Awupf;               /* Atomic Write Unit Power Fail */
  UINT8  Nvscc;               /* NVM Vendor Specific Command Configuration */
  UINT8  Rsvd4;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT16 Acwu;                /* Atomic Compare & Write Unit */
  UINT16 Rsvd5;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Sgls;                /* SGL Support  */
  UINT8  Rsvd6[164];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // I/O Command set Attributes
  //
  UINT8 Rsvd7[1344];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Power State Descriptors
  //
  NVME_PSDESCRIPTOR PsDescriptor[32];

  UINT8  VendorData[1024];    /* Vendor specific data */
} NVME_ADMIN_CONTROLLER_DATA;

/**
 Function To Convert ASCII String To Unicode

 @param [in]   AsciiString
 @param [in]   UnicodeString

**/
EFI_STATUS
AsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString
  )
{
  UINT8           Index;

  Index = 0;
  while (AsciiString[Index] != 0) {
    UnicodeString[Index] = (CHAR16)AsciiString[Index];
    Index++;
  }
  UnicodeString[Index] = 0;

  return EFI_SUCCESS;
}

EFI_STATUS
NvmeIdentifyController (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *NvmePassThruProtocol,
  IN UINT32                                NamespaceId,
  IN NVME_ADMIN_CONTROLLER_DATA            *NvmeAdminControllerData
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  ZeroMem (&CommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof(EFI_NVM_EXPRESS_COMPLETION));

  Command.Cdw0.Opcode = 0x06;

  Command.Nsid = NamespaceId;

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = NvmeAdminControllerData;
  CommandPacket.TransferLength = sizeof (NVME_ADMIN_CONTROLLER_DATA);
  CommandPacket.CommandTimeout = EFI_TIMER_PERIOD_SECONDS (5);
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  Command.Cdw10                = 1;
  Command.Flags                = CDW10_VALID;

  Status = NvmePassThruProtocol->PassThru (
                                   NvmePassThruProtocol,
                                   NamespaceId,
                                   &CommandPacket,
                                   NULL
                                   );

  return Status;
}

/*++
  Check if input device path is nvme device path.

  @param[in]      DevicePath              Data pointer of the device path.

  @retval TRUE                            YES.
  @retval FALSE                           NO.

--*/
BOOLEAN
IsNvmeDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath
  )
{
  BOOLEAN                                   IsNvme;
  EFI_DEVICE_PATH_PROTOCOL                  *pDevicePath;
  
  IsNvme      = FALSE;
  pDevicePath = DevicePath;

  while (!IsDevicePathEnd (pDevicePath)) {
    if ((DevicePathType (pDevicePath) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (pDevicePath) == MSG_NVME_NAMESPACE_DP)) {
      IsNvme = TRUE;
	  break;
    }
    pDevicePath = NextDevicePathNode (pDevicePath);
  }
  return IsNvme;
}


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                 Status; 
	UINT32 Index;
	//Nvme BlockIo parameter 
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL          *BlkIoDevicePath;
  EFI_BLOCK_IO_PROTOCOL             *BlockIo;
  VOID                              *Buffer;
  UINTN                             ImageSize;

  UINTN                 BlockIoNvmeDevCount;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;

  // 
  // NvmePassThruProtocol Parameter
  // 
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL              *NvmePassThruProtocol;
  EFI_HANDLE                                      *NvmePassThruHandleBuffer;
  UINTN                                           NvmePassThruHandleCount;
  UINTN                                           NvmePassThruHandleIndex;
  UINT32                                          NamespaceId;
  NVME_ADMIN_CONTROLLER_DATA                      NvmeAdminControllerData;
  CHAR8                                           *TempChar8Str;
  CHAR16                                          *TempChar16Str;

  EFI_DEVICE_PATH_PROTOCOL                        *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL                        *DevicePathNode;
  PCI_DEVICE_PATH                                 *PciDevicePath;

  UINTN                                           NumOfAllStorageDevice;

  NvmePassThruProtocol      = NULL;

	Index = 0;
  ImageSize = 0;
  BlockIoNvmeDevCount = 0;
  NumOfAllStorageDevice = 0;
  //
  // Three PCD type (FeatureFlag, UINT32 and String) are used as the sample.
  //
  if (FeaturePcdGet (PcdHelloWorldPrintEnable)) {
  	for (Index = 0; Index < PcdGet32 (PcdHelloWorldPrintTimes); Index ++) {
  	  //
  	  // Use UefiLib Print API to print string to UEFI console
  	  //
    	Print ((CHAR16*)PcdGetPtr (PcdHelloWorldPrintString));
    }
  }

  // 
  //  Nvme BlockIoProtocol for read Nvme Size and path.
  // 
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,  
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  // A01NvmeBlkIoData = AllocateZeroPool ((sizeof(A01_NVME_BLK_IO_DATA) * HandleCount));
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      ImageSize = 0;
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *)&BlkIoDevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (IsNvmeDevicePath(BlkIoDevicePath)) {
        gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE); 
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiBlockIoProtocolGuid,
                        (VOID **)&BlockIo
                        );
        Buffer = AllocatePool (BlockIo->Media->BlockSize);
        
        if (Buffer != NULL) {
          BlockIo->ReadBlocks (
                    BlockIo,
                    BlockIo->Media->MediaId,
                    0,
                    BlockIo->Media->BlockSize,
                    Buffer
                    );
          FreePool(Buffer);
          ImageSize = (UINTN)(( BlockIo->Media->BlockSize) * (UINTN)(BlockIo->Media->LastBlock)) / (1000 * 1000 * 1000);
          Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiDiskInfoProtocolGuid,
                  (VOID **) &DiskInfo
                  );
          if (!EFI_ERROR (Status)) {
            Print ( L"<%d> Path: %s , %d GB.", BlockIoNvmeDevCount, ConvertDevicePathToText (BlkIoDevicePath, FALSE, FALSE), ImageSize);
            BlockIoNvmeDevCount++;
          }
        }
      }
    }
  }


  // 
  //  NvmePassThruProtocol for Read Nvme Name and Device Path.
  // 
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNvmExpressPassThruProtocolGuid,
                  NULL,
                  &NvmePassThruHandleCount,
                  &NvmePassThruHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (NvmePassThruHandleIndex = 0x00; NvmePassThruHandleIndex < NvmePassThruHandleCount; NvmePassThruHandleIndex++) {
      Status = gBS->HandleProtocol (
                    NvmePassThruHandleBuffer[NvmePassThruHandleIndex],
                    &gEfiNvmExpressPassThruProtocolGuid,
                    (VOID**) &NvmePassThruProtocol
                    );
      if (!EFI_ERROR (Status)) {
        NamespaceId = 0xFFFFFFFF;
        Status = EFI_SUCCESS;

        while (1) {
          Status = NvmePassThruProtocol->GetNextNamespace (NvmePassThruProtocol, &NamespaceId);

          if (EFI_ERROR (Status)) {
            break;
          }

          if (NamespaceId > 0x01) {
            Status = gBS->HandleProtocol (
                            NvmePassThruHandleBuffer[NvmePassThruHandleIndex],
                            &gEfiDevicePathProtocolGuid,
                            (VOID *)&DevicePath
                            );
            ZeroMem (&NvmeAdminControllerData, sizeof(NVME_ADMIN_CONTROLLER_DATA));
            Status = NvmeIdentifyController (NvmePassThruProtocol, NamespaceId, &NvmeAdminControllerData);
            if (!EFI_ERROR (Status)) {
              TempChar8Str = AllocateZeroPool (sizeof(NvmeAdminControllerData.Mn) + 1);
              CopyMem (TempChar8Str, NvmeAdminControllerData.Mn,sizeof(NvmeAdminControllerData.Mn));
              TempChar16Str = AllocateZeroPool ((sizeof(TempChar8Str)/sizeof(CHAR8))*sizeof(CHAR16) );
              AsciiToUnicode (TempChar8Str,TempChar16Str);
              Print ( L"<%d> Path: %s , Name : %s.", NumOfAllStorageDevice, ConvertDevicePathToText (DevicePath, FALSE, FALSE), TempChar16Str);  
              FreePool (TempChar8Str);
              FreePool (TempChar16Str);

              NumOfAllStorageDevice++;
            }
          } else {
            Status = gBS->HandleProtocol (
                            NvmePassThruHandleBuffer[NvmePassThruHandleIndex],
                            &gEfiDevicePathProtocolGuid,
                            (VOID *)&DevicePath
                            );
            if (!EFI_ERROR (Status)) {
              DevicePathNode = DevicePath;
              PciDevicePath = NULL;
              while (!IsDevicePathEnd (DevicePathNode)) {
                if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
                  (DevicePathSubType (DevicePathNode) == HW_PCI_DP)) {
                  if (PciDevicePath == NULL) {
                    PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
                    break;
                  }
                }
                DevicePathNode = NextDevicePathNode (DevicePathNode);
              }

              ZeroMem (&NvmeAdminControllerData, sizeof(NVME_ADMIN_CONTROLLER_DATA));

              //
              // NVM_Express 1.2 spec -- Admin Command Set
              //
              // A subset of commands uses the Namespace Identifier field (CDW1.NSID).
              // When not used, the field shall be cleared to 0h.
              //
              Status = NvmeIdentifyController (NvmePassThruProtocol, 0, &NvmeAdminControllerData);
              if (!EFI_ERROR (Status) && PciDevicePath != NULL) {
                TempChar8Str = AllocateZeroPool (sizeof(NvmeAdminControllerData.Mn) + 1);
                CopyMem (TempChar8Str, NvmeAdminControllerData.Mn,sizeof(NvmeAdminControllerData.Mn));
                TempChar16Str = AllocateZeroPool ((sizeof(TempChar8Str)/sizeof(CHAR8))*sizeof(CHAR16) );
                AsciiToUnicode (TempChar8Str,TempChar16Str);
                Print ( L"<%d> Path: %s , Name : %s.", NumOfAllStorageDevice, ConvertDevicePathToText (DevicePath, FALSE, FALSE), TempChar16Str);
                
                FreePool (TempChar8Str);
                FreePool (TempChar16Str);

                NumOfAllStorageDevice++;
              }
            }
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}
