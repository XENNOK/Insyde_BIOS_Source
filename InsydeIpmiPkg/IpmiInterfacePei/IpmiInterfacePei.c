/** @file
 H2O IPMI Interface PEI module entry.

 This file contains implementation of PEI module entry point.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "IpmiInterfacePei.h"


/**
 Output lowest level rad data for debugging.

 @param[in]        Request              TRUE for send request data to BMC;
                                        FALSE for receive response data from BMC;
 @param[in]        Header               Request/Response common header.
                                        and various IPMI command, to send to BMC.
 @param[in]        Data                 Data buffer that contains request/response data.
 @param[in]        DataSize             Data buffer size.
 @param[out]       Status               Status of read/write transaction.
 @param[out]       CompletionCode       Completion code that BMC returned.
                                        Ignore when Request is set to TRUE.

*/
VOID
SystemInterfaceDebug (
  IN  BOOLEAN                  Request,
  IN  H2O_IPMI_CMD_HEADER      *Header,
  IN  VOID                     *Data,
  IN  UINT8                    DataSize,
  IN  EFI_STATUS               Status,
  IN  UINT8                    CompletionCode
  )
{
  UINTN  Index;
  UINT8  *Buffer;

  if (Request) {
    DEBUG ((DEBUG_VERBOSE, "  Request (%02x, %02x)\n", *(UINT8*)Header, Header->Cmd));
  } else {
    DEBUG ((DEBUG_VERBOSE, "  Response (%02x, %02x)\n", *(UINT8*)Header, Header->Cmd));
  }
  DEBUG ((DEBUG_VERBOSE, "    Status          : %r\n", Status));
  DEBUG ((DEBUG_VERBOSE, "    Network Function: %02x\n", Header->NetFn));
  DEBUG ((DEBUG_VERBOSE, "    LUN             : %02x\n", Header->Lun));
  DEBUG ((DEBUG_VERBOSE, "    Command         : %02x\n", Header->Cmd));
  if (!Request) {
    DEBUG ((DEBUG_VERBOSE, "    Completion Code : %02x\n", CompletionCode));
  }
  DEBUG ((DEBUG_VERBOSE, "    Data Size       : %02x\n", DataSize));
  DEBUG ((DEBUG_VERBOSE, "    Data            : "));

  if (Data != NULL) {
    Buffer = (UINT8*)Data;
    for (Index = 1; Index <= DataSize; ++Index) {
      if ((Index & 0x0F) == 0 && Index != 0x00) {
        DEBUG ((DEBUG_VERBOSE, "\n                      "));
      }
      DEBUG ((DEBUG_VERBOSE, "%02x ", Buffer[Index]));
    }
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));
}


/**
 Empty function for internal use.

 @param[in]        This                 A pointer to H2O_IPMI_INTERFACE_PPI structure.
 @param[in]        Request              H2O_IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.
 @param[out]       StatusCodes          The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        For KCS system interface:
                                        StatusCodes is valid when return value is EFI_ABORTED. If the return
                                        value is EFI_DEVICE_ERROR,it does not guarantee StatusCodes is valid,
                                        the caller must check bit 15.
                                        For SMIC system interface: It is always valid.
                                        For BT system interface: It is no function.

 @retval EFI_UNSUPPORTED                Always return EFI_UNSUPPORTED since there is no BMC.
*/
EFI_STATUS
EmptyExecuteIpmiCmd (
  IN  H2O_IPMI_INTERFACE_PPI            *This,
  IN  H2O_IPMI_CMD_HEADER               Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINT8                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Create H2O IPMI HOB and Initial it.

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[out]        IpmiHob             A pointer to H2O_IPMI_HOB structure that will be created.

 @retval EFI_SUCCESS                    Create and initial HOB successfully.
 @retval !EFI_SUCCESS                   Create HOB fail.
*/
EFI_STATUS
InitialHob (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  OUT      H2O_IPMI_HOB                 **IpmiHob
  )
{
  EFI_STATUS                            Status;
  EFI_HOB_GUID_TYPE                     *Hob;
  UINT16                                Size;
  UINT8                                 InterfaceType;
  UINT8                                 AddressType;

  //
  // Build GUIDed Hobs
  //
  Size = sizeof (EFI_HOB_GUID_TYPE) + sizeof (H2O_IPMI_HOB);
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             Size,
                             (VOID**)&Hob
                             );
  if (EFI_ERROR (Status)) {
   return Status;
  }

  InterfaceType = FixedPcdGet8 (PcdIpmiInterfaceType);
  AddressType = FixedPcdGet8 (PcdIpmiAddressType);

  *IpmiHob = (H2O_IPMI_HOB *)((UINT8*)Hob + sizeof (EFI_HOB_GUID_TYPE));
  Hob->Name = gH2OIpmiHobGuid;
  (*IpmiHob)->BmcStatus = BMC_STATUS_UNKNOWN;
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr = FixedPcdGet16 (PcdIpmiSmmBaseAddress);
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Offset = FixedPcdGet16 (PcdIpmiSmmRegOffset);
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr = FixedPcdGet16 (PcdIpmiPostBaseAddress);
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset = FixedPcdGet16 (PcdIpmiPostRegOffset);
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr = FixedPcdGet16 (PcdIpmiOsBaseAddress);
  (*IpmiHob)->InterfaceAddress[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Offset = FixedPcdGet16 (PcdIpmiOsRegOffset);
  (*IpmiHob)->BmcFirmwareVersion = 0xFFFF;
  (*IpmiHob)->IpmiVersion = H2O_IPMI_VERSION_DEFAULT;

  if (InterfaceType > H2O_IPMI_UNKNOWN && InterfaceType < H2O_IPMI_MAX_INTERFACE_TYPE) {
    (*IpmiHob)->InterfaceType = InterfaceType;
  } else {
    (*IpmiHob)->InterfaceType = H2O_IPMI_KCS;
  }

  if (AddressType < H2O_IPMI_MAX_INTERFACE_ADDRESS_TYPE) {
    (*IpmiHob)->InterfaceAddressType = AddressType;
  } else {
    (*IpmiHob)->InterfaceAddressType = H2O_IPMI_IO;
  }

  return EFI_SUCCESS;

}


/**
 Allocate pool and initial H2O_IPMI_PRIVATE_DATA structure.

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[out]        IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA structure, storing PEIM
                                        information.

 @retval EFI_SUCCESS                    Create and initial H2O_IPMI_PRIVATE_DATA data successfully.
 @retval !EFI_SUCCESS                   Create and initial H2O_IPMI_PRIVATE_DATA data fail.
*/
EFI_STATUS
InitialContext (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  OUT      H2O_IPMI_PRIVATE_DATA        **IpmiData
  )
{
  EFI_STATUS                            Status;

  //
  // Allocate and fill H2O_IPMI_PRIVATE_DATA data
  //
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (H2O_IPMI_PRIVATE_DATA), (VOID**)IpmiData);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  (*IpmiData)->Signature = H2O_IPMI_PRIVATE_DATA_SIGNATURE;
  (*IpmiData)->TotalTimeTicks = FixedPcdGet8 (PcdIpmiPeiExecuteTime) * 1000000 * TICK_OF_TEN_MS / 10;
  (*IpmiData)->DelayTimeTicks = 0;
  (*IpmiData)->IpmiPpiDescriptor.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  (*IpmiData)->IpmiPpiDescriptor.Guid = &gH2OIpmiInterfacePpiGuid;
  (*IpmiData)->IpmiPpiDescriptor.Ppi = &(*IpmiData)->IpmiPpi;
  (*IpmiData)->Sequence = 0;
  (*IpmiData)->TimerAddress = FixedPcdGet16 (PcdIpmiAcpiTimerAddress);
  (*IpmiData)->TimerMaxValue = FixedPcdGet32 (PcdIpmiAcpiTimerMaxValue);
  (*IpmiData)->IpmiBaseAddress = FixedPcdGet16 (PcdIpmiPostBaseAddress);
  (*IpmiData)->IpmiBaseAddressOffset = FixedPcdGet16 (PcdIpmiPostRegOffset);

  //
  // Link to correct ExecuteIpmiCmd function based on system interface type
  //
  switch (FixedPcdGet8 (PcdIpmiInterfaceType)) {
  case H2O_IPMI_KCS:
    InitialKcsSystemInterface (*IpmiData);
    break;
  case H2O_IPMI_SMIC:
    InitialSmicSystemInterface (*IpmiData);
    break;
  case H2O_IPMI_BT:
    InitialBtSystemInterface (*IpmiData);
    break;
  default:
    InitialKcsSystemInterface (*IpmiData);
    break;
  }

  return EFI_SUCCESS;

}


/**
 Update IPMI policy according to the IPMI Device Manager Config Variable "IpmiConfigData".

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

 @retval EFI_SUCCESS                    Update Policy success.
 @return EFI_ERROR                      Locate gEfiPeiReadOnlyVariable2PpiGuid or execute GetVariable error.
*/
EFI_STATUS
UpdatePolicy (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *VariablePpi;
  EFI_GUID                              VariableGuid = IPMI_DM_CONFIG_FORMSET_GUID;
  UINTN                                 Size;
  UINT8                                 *VariableBuffer;
  IPMI_DM_CONFIG                        *IpmiConfig;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Size = 0;
  VariableBuffer = NULL;

  do {
    Status = VariablePpi->GetVariable (
                            VariablePpi,
                            IPMI_CONFIG_DATA_NAME,
                            &VariableGuid,
                            NULL,
                            &Size,
                            VariableBuffer
                            );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableBuffer = AllocateZeroPool (Size);
    }

  } while (Status == EFI_BUFFER_TOO_SMALL);

  if (!EFI_ERROR (Status)) {
    IpmiConfig = (IPMI_DM_CONFIG*) VariableBuffer;

    PcdSet8 (PcdIpmiEnable, IpmiConfig->IpmiEnable);
    PcdSet8 (PcdBmcWarmupTime, IpmiConfig->BmcWarmupTime);
    PcdSet8 (PcdBmcWdtEnable, IpmiConfig->BmcWdtEnable);
    PcdSet8 (PcdBmcWdtAction, IpmiConfig->BmcWdtAction);
    PcdSet8 (PcdBmcWdtTimeout, IpmiConfig->BmcWdtTimeout);
    PcdSet8 (PcdIpmiSpmiEnable, IpmiConfig->IpmiSpmiEnable);
    PcdSet8 (PcdIpmiSetBiosVersion, IpmiConfig->IpmiSetBiosVersion);
    PcdSet8 (PcdIpmiBootOption, IpmiConfig->IpmiBootOption);
  }

  return Status;

}


/**
 Get basic BMC information.

 This function use "GetDeviceId" IPMI command to get BMC information, especially
 BMC Firmware Version and IPMI Specification Version.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PPI structure.
 @param[in]         BmcInfo             Structure for storing data from Get Device ID command.

 @retval EFI_SUCCESS                    Execute "GetDeviceId" IPMI command success
                                        and the information is correct.
 @retval EFI_TIMEOUT                    BMC no response or BMC is busy.
 @retval EFI_NOT_FOUND                  Corresponding register is not be decode.
 @retval EFI_DEVICE_ERROR               BMC has other issue other then no response and busy.
*/
EFI_STATUS
GetBmcInfo (
  IN  H2O_IPMI_INTERFACE_PPI            *Ipmi,
  IN  H2O_IPMI_BMC_INFO                 *BmcInfo
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;
  UINT32                                StartTick;
  UINT32                                EndTick;
  UINT8                                 RecvSize;
  H2O_IPMI_CMD_HEADER                   Request;
  UINT8                                 Retry;
  UINT8                                 Temp;

  //
  // Some BMCs will return garbage data after BMC just ready.
  // Retry several times to read correct data
  //
  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_GET_DEVICE_ID;
  Request.Lun = H2O_IPMI_BMC_LUN;

  for (Retry = 0; Retry < 3; ++Retry) {

    Status = Ipmi->ExecuteIpmiCmd (
                     Ipmi,
                     Request,
                     NULL,
                     0,
                     BmcInfo,
                     &RecvSize,
                     NULL
                     );

    if (Status == EFI_SUCCESS) {
      Temp = BmcInfo->IpmiVersionMostSigBits << 4 | BmcInfo->IpmiVersionLeastSigBits;
      if (Temp == 0x20 || Temp == 0x15) {
        return EFI_SUCCESS;
      }

      DEBUG ((DEBUG_ERROR, "[IpmiInterfacePei] Strange IPMI Version! Maybe BMC Firmware Bug!\n"));

    } else if (Status == EFI_TIMEOUT) {
      return EFI_TIMEOUT;

    } else if (Status == EFI_NOT_FOUND) {
      return EFI_NOT_FOUND;

    } else if (Status == EFI_LOAD_ERROR) {
      //
      // Consume rest time tick to fit execute cycle time to treat it as timeout
      //
      if (*(UINT8*)BmcInfo == H2O_IPMI_COMPLETE_CODE_NODE_BUSY) {
        DEBUG ((DEBUG_ERROR, "[IpmiInterfacePei] BMC Node Busy ... Wait ...\n"));

        IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (Ipmi);
        StartTick = IoRead32 (IpmiData->TimerAddress);

        while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
          EndTick = IoRead32 (IpmiData->TimerAddress);

          if (EndTick > StartTick) {
            IpmiData->DelayTimeTicks += (EndTick - StartTick);
          } else {
            IpmiData->DelayTimeTicks += (EndTick + IpmiData->TimerMaxValue - StartTick);
          }

          StartTick = EndTick;
        }

        return EFI_TIMEOUT;
      }

    }

  }

  return EFI_DEVICE_ERROR;

}


/**
 Try to communication with BMC.

 Use Get Device ID to find out which I/O address OS/SMM/FW should use,
 and get BMC information.

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA structure.
 @param[in]         IpmiHob             A pointer to H2OIpmiHob.
 @param[in]         BmcWarmupTime       The time to wait BMC ready in seconds.

 @retval EFI_SUCCESS                    Find BMC and BMC is O.K.
 @retval EFI_NOT_FOUND                  Cannot find BMC at those base address.
 @retval EFI_NOT_READY                  BMC exist, but not ready.
 @retval EFI_DEVICE_ERROR               Find BMC, but it has a problem.
*/
EFI_STATUS
InitialBmc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_PRIVATE_DATA        *IpmiData,
  IN       H2O_IPMI_HOB                 *IpmiHob,
  IN       UINT8                        BmcWarmupTime
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            ReturnStatus;
  UINT8                                 Index;
  UINTN                                 TotalTime;
  UINT16                                FirstBaseAddress;
  UINT16                                SecondBaseAddress;
  H2O_IPMI_BMC_INFO                     BmcInfo;

  //
  // WaitBmc function should return EFI_SUCCESS, EFI_TIMEOUT or EFI_UNSUPPORTED.
  //
  // EFI_SUCCESS means platform waits BMC via their own way and BMC should be ready.
  // In this case we just simply send Get Device Id command once and no retry.
  // If BMC is ready we can get BMC information.
  // If BMC is not ready we treat it as EFI_NOT_FOUND.
  //
  // EFI_TIMEOUT means platform waits BMC via their own way but BMC is not ready.
  // In this case we just treat it as EFI_NOT_FOUND.
  //
  // EFI_UNSUPPORTED means platform does not wait BMC via their own way.
  // In this case we do things we do originally.
  //
  Status = OemIpmiWaitBmc (PeiServices, &IpmiData->IpmiPpi);
  if (Status == EFI_UNSUPPORTED) {
    Status = IpmiWaitBmc (PeiServices, &IpmiData->IpmiPpi);
  }
  
  if (!EFI_ERROR (Status)) {
    BmcWarmupTime = 0;
  } else if (Status != EFI_UNSUPPORTED) {
    IpmiHob->BmcStatus = BMC_STATUS_NOT_FOUND;
    return EFI_NOT_FOUND;
  }
  
  TotalTime = 0;
  Status = EFI_NOT_FOUND;
  ReturnStatus = EFI_NOT_FOUND;
  FirstBaseAddress = IpmiHob->InterfaceAddress[0].Addr;
  SecondBaseAddress = IpmiHob->InterfaceAddress[1].Addr;

  for (Index = 0; Index < H2O_IPMI_MAX_BASE_ADDRESS_NUM; ++Index) {

    if (Index != 0 && IpmiHob->InterfaceAddress[Index].Addr == FirstBaseAddress) {
      continue;
    } else if (Index == 2 && IpmiHob->InterfaceAddress[Index].Addr == SecondBaseAddress) {
      IpmiHob->InterfaceAddress[Index].Addr = IpmiHob->InterfaceAddress[Index-1].Addr;
      IpmiHob->InterfaceAddress[Index].Offset = IpmiHob->InterfaceAddress[Index-1].Offset;
      continue;
    }

    IpmiData->IpmiBaseAddress = IpmiHob->InterfaceAddress[Index].Addr;
    IpmiData->IpmiBaseAddressOffset = IpmiHob->InterfaceAddress[Index].Offset;

    while (TRUE) {
      Status = GetBmcInfo (&IpmiData->IpmiPpi, &BmcInfo);

      if (Status == EFI_SUCCESS) {
        if (Index == 0) {
          IpmiHob->BmcStatus = BMC_STATUS_OK;
          ReturnStatus = EFI_SUCCESS;

          IpmiHob->BmcFirmwareVersion =
            BmcInfo.MajorFmRevision << 8 | BmcInfo.MinorFmRevision;

          IpmiHob->IpmiVersion =
            BmcInfo.IpmiVersionMostSigBits << 4 | BmcInfo.IpmiVersionLeastSigBits;

        } else if ((Index == 2) &&
                   (IpmiHob->InterfaceAddress[0].Addr == IpmiHob->InterfaceAddress[1].Addr) &&
                   (FirstBaseAddress != SecondBaseAddress)) {
          //
          // If Index POST(1) fail, but Index OS(2) success, set Index POST address & offset to Index OS value.
          //
          IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr = IpmiData->IpmiBaseAddress;
          IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset = IpmiData->IpmiBaseAddressOffset;

        }

        break;

      } else {

        if (Index == 0 && (Status == EFI_TIMEOUT || Status == EFI_NOT_FOUND)) {
          TotalTime = TotalTime + FixedPcdGet8 (PcdIpmiPeiExecuteTime);

          if (TotalTime >= BmcWarmupTime) {
            if (Status == EFI_TIMEOUT) {
              IpmiHob->BmcStatus = BMC_STATUS_NOT_READY;
              return EFI_NOT_READY;
            } else {
              IpmiHob->BmcStatus = BMC_STATUS_NOT_FOUND;
              return EFI_NOT_FOUND;
            }
          }

        } else if (Index == 0 && Status == EFI_DEVICE_ERROR) {
          IpmiHob->BmcStatus = BMC_STATUS_ERROR;
          return EFI_DEVICE_ERROR;

        } else {
          //
          // If fail, set to previous successful value.
          //
          IpmiHob->InterfaceAddress[Index].Addr = IpmiHob->InterfaceAddress[Index-1].Addr;
          IpmiHob->InterfaceAddress[Index].Offset = IpmiHob->InterfaceAddress[Index-1].Offset;
          break;
        }

      }

    }

  }

  //
  // Remember change IpmiBaseAddress to POST base address.
  //
  IpmiData->IpmiBaseAddress = IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr;
  IpmiData->IpmiBaseAddressOffset = IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset;

  return ReturnStatus;
}


/**
 Read time from RTC, and store in a EFI_TIME structure.

 @param[out]        Time                EFI_TIME structure to store time.
*/
VOID
GetTimeFromCmos (
  OUT EFI_TIME                          *Time
  )
{
  UINT8                                 Value;

  IoWrite8 (CMOS_INDEX, CMOS_YEAR);
  Value = IoRead8 (CMOS_DATA);
  Time->Year =  BcdToDecimal8 (Value) + 2000;

  IoWrite8 (CMOS_INDEX, CMOS_MONTH);
  Value = IoRead8 (CMOS_DATA);
  Time->Month = BcdToDecimal8 (Value);

  IoWrite8 (CMOS_INDEX, CMOS_DAY);
  Value = IoRead8 (CMOS_DATA);
  Time->Day = BcdToDecimal8 (Value);

  IoWrite8 (CMOS_INDEX, CMOS_HOUR);
  Value = IoRead8 (CMOS_DATA);
  Time->Hour = BcdToDecimal8 (Value);

  IoWrite8 (CMOS_INDEX, CMOS_MINUTE);
  Value = IoRead8 (CMOS_DATA);
  Time->Minute = BcdToDecimal8 (Value);

  IoWrite8 (CMOS_INDEX, CMOS_SECOND);
  Value = IoRead8 (CMOS_DATA);
  Time->Second = BcdToDecimal8 (Value);

}


/**
 Execute "SetSelTime" command to sync SEL time of BMC with BIOS time.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PPI structure.
 @param[in]         TimeStamp           TimeStamp to send to BMC.
*/
VOID
SetSelTimeStamp (
  IN  H2O_IPMI_INTERFACE_PPI            *Ipmi,
  IN  UINT32                            TimeStamp
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvBuf[0x20];
  H2O_IPMI_CMD_HEADER                   Request;

  Request.NetFn = H2O_IPMI_NETFN_STORAGE;
  Request.Cmd = H2O_IPMI_CMD_SET_SEL_TIME;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   &TimeStamp,
                   sizeof(TimeStamp),
                   &RecvBuf,
                   &RecvSize,
                   NULL
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IpmiInterfacePei] Set SEL Time: %r\n", Status));
  }

}


/**
 Set BMC watchdog.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PPI structure.
*/
VOID
SetBmcWatchdogTimer (
  IN  H2O_IPMI_INTERFACE_PPI            *Ipmi
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvBuf;
  H2O_IPMI_SET_WATCHDOG_TIMER           Wdt;
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (&Wdt, sizeof (H2O_IPMI_SET_WATCHDOG_TIMER));
  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_SET_WATCHDOG_TIMER;
  Request.Lun = H2O_IPMI_BMC_LUN;

  //
  // Set timeout action and timeout value if enable
  //
  Wdt.TimerAction |= PcdGet8 (PcdBmcWdtAction);
  Wdt.CountDown = (UINT16)((UINT16)PcdGet8 (PcdBmcWdtTimeout) * 600);
  Wdt.NotLog = PcdGet8 (PcdBmcWdtNotLog);
  Wdt.TimerUse = PcdGet8 (PcdBmcWdtTimerUse);

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   &Wdt,
                   sizeof (Wdt),
                   &RecvBuf,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status) && PcdGet8 (PcdBmcWdtEnable)) {
    Request.Cmd = H2O_IPMI_CMD_RESET_WATCHDOG_TIMER;
    Ipmi->ExecuteIpmiCmd (Ipmi, Request, NULL, 0, &RecvBuf, &RecvSize, NULL);
  }

}


/**
 Callback function of boot mode ready event

 This function will check boot mode to know if it is necessary to stop IPMI
 watchdog timer or not.

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[in]         NotifyDescriptor    Address of the notification descriptor data structure.
 @param[in]         Ppi                 Address of the PPI that was installed.

 @retval EFI_SUCCESS                    Always return EFI_SUCCESS.
*/
EFI_STATUS
BootModeReadyNotifyCallBack (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN  VOID                              *Ppi
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  H2O_IPMI_INTERFACE_PPI                *IpmiPpi;

  //
  // Get boot mode, do nothing if not S3 resume
  //
  Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
  if (!EFI_ERROR (Status) && BootMode != BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  if (PcdGet8 (PcdBmcWdtEnable) == H2O_IPMI_FUNCTION_DISABLE) {
    return EFI_SUCCESS;
  }

  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gH2OIpmiInterfacePpiGuid, 0, NULL, (VOID **)&IpmiPpi);

  if (!EFI_ERROR (Status)) {
    SetBmcWatchdogTimer (IpmiPpi);
  }

  return EFI_SUCCESS;

}


EFI_PEI_NOTIFY_DESCRIPTOR mBootModeNotifyDescriptor = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  BootModeReadyNotifyCallBack
};


/**
 Entry of this module.

 The entry point will get user settings from PCDs, try to communicate with BMC
 to know BMC status and settings BMC based on user settings. If all procedures
 are success, install H2O IPMI Interface PPI for other modules use.

 @param[in]         FileHandle          The file handle of the image.
 @param[in]         PeiServices         General purpose services available to every PEIM.

 @retval EFI_SUCCESS                    Initial PEIM success.
 @retval !EFI_SUCCESS                   Initial PEIM fail.
*/
EFI_STATUS
EFIAPI
IpmiInterfacePeiEntry (
  IN       EFI_PEI_FILE_HANDLE          FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_HOB                          *IpmiHob;
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;
  EFI_TIME                              Time;
  UINT32                                TimeStamp;

  //
  // Create Hob
  //
  Status = InitialHob (PeiServices, &IpmiHob);
  if (EFI_ERROR (Status)) {
   return Status;
  }

  //
  // Initial H2O_IPMI_PRIVATE_DATA data
  //
  Status = InitialContext (PeiServices, &IpmiData);
  if (EFI_ERROR (Status)) {
   return Status;
  }

  //
  // Reserved for OEM to update policy by OEM specific way.
  //
  Status = OemIpmiUpdatePolicy (PeiServices);
  if (Status == EFI_UNSUPPORTED) {

    //
    // Get default policy or update by platform
    //
    Status = IpmiUpdatePolicy (PeiServices);
    if ((Status == EFI_UNSUPPORTED) && (FeaturePcdGet (PcdIpmiConfigInDeviceManager) == TRUE)) {
      //
      // Update policy when IPMI Config in Device Manager
      //
      UpdatePolicy (PeiServices);
    }

  }

  //
  // Program LPC (Low Pin Count) Interface Bridge Registers for BMC
  //
  Status = OemIpmiProgramLpc (
             PeiServices,
             FixedPcdGet16 (PcdIpmiSmmBaseAddress),
             FixedPcdGet16 (PcdIpmiSmmRegOffset),
             FixedPcdGet16 (PcdIpmiPostBaseAddress),
             FixedPcdGet16 (PcdIpmiPostRegOffset),
             FixedPcdGet16 (PcdIpmiOsBaseAddress),
             FixedPcdGet16 (PcdIpmiOsRegOffset)
             );
  if (Status == EFI_UNSUPPORTED) {
    IpmiProgramLpc (
      PeiServices,
      FixedPcdGet16 (PcdIpmiSmmBaseAddress),
      FixedPcdGet16 (PcdIpmiSmmRegOffset),
      FixedPcdGet16 (PcdIpmiPostBaseAddress),
      FixedPcdGet16 (PcdIpmiPostRegOffset),
      FixedPcdGet16 (PcdIpmiOsBaseAddress),
      FixedPcdGet16 (PcdIpmiOsRegOffset)
      );
  }

  //
  // Program SIO (Super I/O) Configuration for BMC
  //
  Status = OemIpmiProgramSio (
             PeiServices,
             FixedPcdGet16 (PcdIpmiSmmBaseAddress),
             FixedPcdGet16 (PcdIpmiSmmRegOffset),
             FixedPcdGet16 (PcdIpmiPostBaseAddress),
             FixedPcdGet16 (PcdIpmiPostRegOffset),
             FixedPcdGet16 (PcdIpmiOsBaseAddress),
             FixedPcdGet16 (PcdIpmiOsRegOffset)
             );
  if (Status == EFI_UNSUPPORTED) {
    IpmiProgramSio (
      PeiServices,
      FixedPcdGet16 (PcdIpmiSmmBaseAddress),
      FixedPcdGet16 (PcdIpmiSmmRegOffset),
      FixedPcdGet16 (PcdIpmiPostBaseAddress),
      FixedPcdGet16 (PcdIpmiPostRegOffset),
      FixedPcdGet16 (PcdIpmiOsBaseAddress),
      FixedPcdGet16 (PcdIpmiOsRegOffset)
      );
  }


  //
  // Wait & detect corresponding register is decoded by BMC.
  // If return status is EFI_NOT_FOUND, set BMC status to "BMC_STATUS_NOT_FOUND".
  if (PcdGet8 (PcdIpmiEnable) != H2O_IPMI_FUNCTION_DISABLE) {
    if (FixedPcdGet8 (PcdIpmiWaitBmcDecodeRetry) != 0) {
      Status = IpmiData->IpmiWaitBmcDecode (IpmiData);
      if (Status == EFI_NOT_FOUND) {
        IpmiHob->BmcStatus = BMC_STATUS_NOT_FOUND;
      }
    }
  }


  //
  // Do basic initial if IPMI is enabled
  //
  if (PcdGet8 (PcdIpmiEnable) != H2O_IPMI_FUNCTION_DISABLE && IpmiHob->BmcStatus != BMC_STATUS_NOT_FOUND) {
    //
    // Find BMC/Base Address, Get BMC Status
    //
    Status = InitialBmc (
               PeiServices,
               IpmiData,
               IpmiHob,
               PcdGet8 (PcdBmcWarmupTime)
               );
    
    if (!EFI_ERROR (Status)) {
      //
      // Sync time with BMC
      //
      GetTimeFromCmos (&Time);
      EfiTimeToTimeStamp (&Time, &TimeStamp);
      SetSelTimeStamp (&IpmiData->IpmiPpi, TimeStamp);

      //
      // Set Watchdog Timer if enable, Stop timer if disable
      //
      SetBmcWatchdogTimer (&IpmiData->IpmiPpi);

      (*PeiServices)->NotifyPpi (PeiServices, &mBootModeNotifyDescriptor);
	  
	  DEBUG ((DEBUG_INFO, "[IpmiInterfacePei] BMC Exist and Ready.\n"));

    } else {
      IpmiHob->IpmiVersion = H2O_IPMI_VERSION_DEFAULT;
      IpmiData->IpmiPpi.ExecuteIpmiCmd = EmptyExecuteIpmiCmd;

      DEBUG ((DEBUG_ERROR, "[IpmiInterfacePei] Initial BMC: %r\n", Status));
    }

  } else {
    IpmiHob->IpmiVersion = H2O_IPMI_VERSION_DEFAULT;
    IpmiData->IpmiPpi.ExecuteIpmiCmd = EmptyExecuteIpmiCmd;
  }

  //
  // Install our ppi
  //
  Status = (*PeiServices)->InstallPpi (PeiServices, &IpmiData->IpmiPpiDescriptor);

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after PEI IPMI Interface PPI has been installed.
    //
    OemIpmiPeiAfterInstallInterface (PeiServices, &IpmiData->IpmiPpi);
  }

  return Status;

}

