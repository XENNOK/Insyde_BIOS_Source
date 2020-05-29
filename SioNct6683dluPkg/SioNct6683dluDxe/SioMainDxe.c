/** @file

SIO main code, it include create SCU, install device protocol, init in DXE stage

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioInitDxe.h"

EFI_CPU_IO2_PROTOCOL  *mCpuIo;
SIO_DEVICE_LIST_TABLE *mTablePtr;

extern UINT16 mSioCfgPortList[];
extern EFI_SIO_TABLE mSioTable[];
extern VOID EnterConfigMode ();
extern VOID ExitConfigMode ();
extern CHAR16 *mSioVariableName;

//[-start-140116-IB12970054-modify]//
extern UINT16 mSioCfgPortListSize;
//[-end-140116-IB12970054-modify]//

extern EFI_INSTALL_DEVICE_FUNCTION mDeviceFunction[];
extern EFI_SIO_RESOURCE_FUNCTION mSioResourceFunction[];

//[-start-131231-IB12970047-add]//
BOOLEAN
KbcExistence (
)
/*++
	
Routine Description:
  Check physical existence of KBC
	  
Arguments:
  None
	  
Returns:
  Return true of false
	  
--*/
{
  // Send device test command to keyboard 
  //
  if (IoRead8 (0x60) == 0xFF) {
    return FALSE;
  }
  WriteData(0xEE);
  WaitObf();

  if (IoRead8(0x60) == 0xEE) {
    return TRUE;
  }
  return FALSE;
}


BOOLEAN
MouseExistence (
)
/*++
	
Routine Description:
  Check physical existence of MOUSE
	  
Arguments:
  NONE
	  
Returns:
  Return true or false
	  
--*/
{
  UINT8          WrapMode;
  UINT8          NormalMode;
  UINT8          TestValue;
  UINT8          NextToMouse;
  BOOLEAN        Result;

  WrapMode    = 0xEE;
  TestValue   = 0x05; 
  NormalMode  = 0xEC;
  NextToMouse = 0xD4;
  Result      = FALSE;

//[-start-140107-IB12970050-add]//
  if (IoRead8 (0x60) == 0xFF) {
    return FALSE;
  }
//[-end-140107-IB12970050-add]//

  //
  // Send EE command to detect mouse. 
  // (This command will set mouse to wrap mode)
  //
  WriteCmd(NextToMouse);
  WriteCmd2Kbd (WrapMode);
  WriteCmd(NextToMouse);
  WriteData(TestValue);

  WaitObf();

  if (IoRead8(0x60) == TestValue) {
    Result = TRUE;
  }
  
  WriteCmd(NextToMouse);
  WriteCmd2Kbd(NormalMode);

  //
  // Clear buffer to avoid that KBC can't transfer data later
  //
  WaitObf();
  IoRead8(0x60);
  
  return Result;
}

BOOLEAN
WriteCmd (
  UINT8  Cmd
  )
{
  WaitObe();
  WaitIbe();
  IoWrite8(0x64, Cmd);
  WaitIbe();

  return TRUE;
}

BOOLEAN
WriteData (
  UINT8  Data
  )
{
  WaitObe();
  WaitIbe();
  IoWrite8 (0x60, Data); 
  WaitIbe();

  return TRUE;
}

BOOLEAN
WriteCmd2Kbd (
  UINT8  Cmd
  )
{
  WaitObe();
  WaitIbe();
  IoWrite8(0x60, Cmd);
  gBS->Stall (8000);
  WaitIbe();

  return TRUE;
}

BOOLEAN
WaitObf (
  )
/*++

Routine Description:
  Wait for keyboard output buffer is full.
  
Arguments:
  NONE
	  
Returns:
  Return true or false 
  
--*/  
{  
  while(!(IoRead8(0x64) & BIT0))
    ;     //Output buffer full

  return TRUE;
}

BOOLEAN
WaitObe(
  )
/*++

Routine Description:
  Wait for keyboard output buffer is empty.
  
Arguments:
  NONE
	  
Returns:
  Return true or false
	  
--*/  
{
  while(IoRead8(0x64) & BIT0)     //Output buffer empty
    IoRead8(0x60);
  return TRUE;
}

BOOLEAN
WaitIbe(
  )
/*++

Routine Description:
  Wait for keyboard input buffer is empty.
  
Arguments:
  NONE
	  
Returns:
  Return true or false
	  
--*/ 
{

  while(IoRead8(0x64) & BIT1)
    ;     //wait until input buffer empty

  return TRUE;
}
//[-end-131231-IB12970047-add]//

/**
  The entry point of the SIO driver.  

  @param [in] ImageHandle            A handle for the image that is initializing this driver
  @param [in] SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Function complete successfully. 
**/
EFI_STATUS
EFIAPI
SioDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_EVENT                         ReadyToBootEvent; 
  EFI_SIO_TABLE                     *OemTable;

  mTablePtr = (SIO_DEVICE_LIST_TABLE *)PcdGetPtr (PcdSioNct6683dluCfg);
  
  //
  // must locate CPUIO in here for test cfg port
  //
  Status = gBS->LocateProtocol (
                  &gEfiCpuIo2ProtocolGuid, 
                  NULL, 
                  (VOID **) &mCpuIo
                  );
  
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Get SIO from PCD, if it fail, we don't install SIO DXE driver
  //
  Status = CheckDevice ();  
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  mSioResourceFunction->ScriptFlag = TRUE;
  EnterConfigMode ();

  //
  // OemService
  //
  Status = OemSvcRegSioDxe (
           mSioResourceFunction->Instance,
           &OemTable
           );
  if (Status != EFI_SUCCESS) {
    OemTable = mSioTable;
  } 

  while ((OemTable->Register != 0) || (OemTable->Value != 0)) {
    IDW8 (OemTable->Register, OemTable->Value);
    OemTable++;
  }

  //
  // Create SCU page and Variable, then update to PCD data
  //
  if (FeaturePcdGet (PcdSioNct6683dluSetup)) {
    Status = SioScu ();
    ASSERT_EFI_ERROR (Status);
  }

  //
  // OemService, modify PCD data
  //
  OemSvcDeviceResourceSioDxe (
    mSioResourceFunction->Instance,
    mTablePtr, 
    mSioResourceFunction->SioCfgPort
    );

  // Install each device protocol
  InstallEnabledDeviceProtocol ();

  //
  // OemService
  //
  OemSvcAfterInitSioDxe (
    mSioResourceFunction->Instance,
    mTablePtr, 
    mSioResourceFunction->SioCfgPort
    );

  //
  // Unlock super IO config mode
  //
  ExitConfigMode();
  mSioResourceFunction->ScriptFlag = FALSE;

  //
  // Create a Ready to Boot Event for initialization of SIO SSDT
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             UpdateSsdt,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  To get correct SIO data. 

  @retval EFI_SUCCESS           Found SIO data. 
  @retval EFI_NOT_FOUND         Not found. 
**/
EFI_STATUS
CheckDevice (
  VOID
  ) 
{
  SIO_DEVICE_LIST_TABLE             *PcdPointer;

  PcdPointer = mTablePtr;

  //
  // Calculate the number of non-zero entries in the table
  //
  while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
    if (PcdPointer->Device == CFG_ID) {
      if (CheckConfig (PcdPointer) != 0) {
        return EFI_SUCCESS;          
      } 
    }
    PcdPointer++;
  }
  
  return EFI_NOT_FOUND;
}

/**
  Check ID of SIO whether it's correct

  @param[in]  *DeviceTable       SIO_DEVICE_LIST_TABLE from PCD structure.

  @retval     IndexPort          return value as zero, it means "not match".
                                 return value is not zero, it means "match".
**/
UINT16
CheckConfig (
  IN SIO_DEVICE_LIST_TABLE      *DeviceTable
  )
{
  UINT16                IndexPort;
  UINT16                DataPort;
  UINT8                 Index;

  IndexPort = UpsideDown (DeviceTable->DeviceBaseAdr);
  DataPort  = IndexPort + 1;
  mSioResourceFunction->SioCfgPort = IndexPort;

  //
  // Verify define config port
  //
  if (IndexPort != 0) {
    //
    // Enter Config Mode
    //
    EnterConfigMode ();
  
    SioWrite8 (IndexPort, SIO_CHIP_ID1);
    if (SioRead8 (DataPort) != DeviceTable->TypeH) {
      IndexPort = 0;
    }

    if ((DeviceTable->TypeL != NONE_ID) && (IndexPort != 0)) {
      SioWrite8 (IndexPort, SIO_CR21);
      if (SioRead8 (DataPort) != DeviceTable->TypeL) {
        IndexPort = 0;
      }
    }
  }  

  //
  // Auto scan config port
  //
  if (IndexPort == 0) {
    
//[-start-140116-IB12970054-modify]//
    for (Index = 0; Index < mSioCfgPortListSize; Index++) {
//[-end-140116-IB12970054-modify]//

      IndexPort = mSioCfgPortList[Index];
      DataPort  = IndexPort + 1;
      mSioResourceFunction->SioCfgPort = IndexPort;
      //
      // Enter Config Mode
      //
      EnterConfigMode (
        );
  
      SioWrite8 (IndexPort, SIO_CHIP_ID1);
      if (SioRead8 (DataPort) == DeviceTable->TypeH) {
        if (DeviceTable->TypeL != NONE_ID) {
          SioWrite8 (IndexPort, SIO_CR21);
          if (SioRead8 (DataPort) == DeviceTable->TypeL) {
            break;
          }
        } else {
          break;
        }
      }
    }

//[-start-140116-IB12970054-modify]//
    if (Index == mSioCfgPortListSize) {  // if true, means not found SIO ID
//[-end-140116-IB12970054-modify]//

      IndexPort = 0;
      return IndexPort;
    }
  }

  //
  // if it's right, update to EFI_SIO_RESOURCE_FUNCTION 
  //
  DeviceTable->DeviceBaseAdr      = IndexPort;
  mSioResourceFunction->Instance   = DeviceTable->TypeInstance;

  return IndexPort;
}

/**
  Write data to the specific register.  

  @param[in]  Index       SIO index port.
  @param[in]  Data        The 8 bits value write to index port.
**/
VOID
IDW8 (
  IN UINT8  Index,
  IN UINT8  Data
  )
{
  SioWrite8 (
    mSioResourceFunction->SioCfgPort, 
    Index
    );
  SioWrite8 (
    mSioResourceFunction->SioCfgPort+1, 
    Data
    );
}

/**
  Read data to the specific register.  

  @param[in]   Index                SIO index port.

  @retval      UINT8                Read value from SIO. 
**/
UINT8
IDR8 (
  IN UINT8  Index
  )  
{
  BOOLEAN FlagTemp;

  FlagTemp = mSioResourceFunction->ScriptFlag;
  mSioResourceFunction->ScriptFlag = FALSE;
  
  SioWrite8 (
    mSioResourceFunction->SioCfgPort, 
    Index
    );
  mSioResourceFunction->ScriptFlag = FlagTemp;

  return SioRead8((mSioResourceFunction->SioCfgPort)+1);
}

/**
  Writes an I/O port using the CPU I/O Protocol.  

  @param[in]  Port        I/O Port to write.
  @param[in]  Value       The 8 bits value write to port.
**/
VOID
SioWrite8 (
  IN UINT16  Port,
  IN UINT8   Value
  )
{
  mCpuIo->Io.Write (
           mCpuIo, 
           EfiCpuIoWidthUint8, 
           Port, 
           1, 
           &Value
           );
  if (mSioResourceFunction->ScriptFlag == TRUE) {
    S3BootScriptSaveIoWrite (
    EfiBootScriptWidthUint8,
    Port,
    1,      
    &Value    
    );  
  }  
}

/**
  Writes an I/O port using the CPU I/O Protocol to get value.   

  @param[in]  Port                 I/O Port to write.

  @retval     Value                Get data from SIO. 
**/
UINT8
SioRead8 (
  IN UINT16  Port
  )
{
  UINT8   Value;

  mCpuIo->Io.Read (
           mCpuIo, 
           EfiCpuIoWidthUint8, 
           Port, 
           1, 
           &Value
           );
  return Value;
}

/**
    Writes an I/O port using the CPU I/O Protocol to get value.    

  @param[in]   Port                 I/O Port to write.

  @retval      Value                Get data from SIO. 
**/
UINT16
SioRead16 (
  IN UINT16  Port
  )
{
  UINT8 Value;

  mCpuIo->Io.Read (
            mCpuIo, 
            EfiCpuIoWidthUint16, 
            Port, 
            1, 
            &Value
            );
  return Value;
}

/**
  Transfer from SIO_DEVICE_LIST_TABLE to EFI_ISA_DEVICE_RESOURCE for using of device protocol

  @param[in]  *DeviceList            SIO_DEVICE_LIST_TABLE structure.
  @param[in]  *DeviceResource        EFI_ISA_DEVICE_RESOURCE structure.

  @retval EFI_SUCCESS                The function completed successfully.
**/
EFI_STATUS
DeviceListToDeviceResource (
  IN SIO_DEVICE_LIST_TABLE        *DeviceList,
  IN EFI_ISA_DEVICE_RESOURCE      *DeviceResource
  )
{
  DeviceResource->IoPort = UpsideDown(DeviceList->DeviceBaseAdr);
  DeviceResource->IrqNumber = DeviceList->DeviceIrq;
  DeviceResource->DmaChannel = DeviceList->DeviceDma;

  return EFI_SUCCESS;
}

/**
  Find SSDT table from RAW section of DXE driver, then publish it to ACPI table

  @param [in]  Event           Event whose notification function is being invoked
  @param [in]  Context         Pointer to the notification function's context
  
**/
VOID
UpdateSsdt (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_STATUS                    Status;
  VOID                          *Buffer ;
  UINTN                         Size;
  UINTN                         Instance;
  UINTN                         AcpiTableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_SUPPORT_PROTOCOL     *AcpiSupport;
  EFI_ACPI_DESCRIPTION_HEADER   *SioAcpiTable;
  EFI_GUID                      DriverGuid = SIO_DXE_DRIVER_GUID;

  Size = 0;
  Status = EFI_SUCCESS;
  Buffer = NULL;
  Instance = 0;
  AcpiSupport = NULL;
  SioAcpiTable = NULL;
  AcpiTableHandle = 0;

  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid, 
                  NULL, 
                  (VOID **) &AcpiSupport
                  );
  ASSERT_EFI_ERROR (Status);

  while (Status == EFI_SUCCESS) {

    Status = GetSectionFromAnyFv (&DriverGuid, EFI_SECTION_RAW, Instance, &Buffer, &Size);
  
    if (Status == EFI_SUCCESS) {
      //
      // Check the Signature ID to modify the table
      //
      switch (((EFI_ACPI_DESCRIPTION_HEADER*) Buffer)->OemTableId) {

      //
      // If find SSDT table, we'll update NVS area resource and name string
      //
      case (EFI_SIGNATURE_64 ('S', 'I', 'O', 'A', 'C', 'P', 'I', 0)):
        SioAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER*) Buffer;
        if(FeaturePcdGet(PcdSioNct6683dluUpdateAsl)) {
          UpdateSioMbox (SioAcpiTable);
        }
      break;

      default:
      break;     
      }
    }  

    Instance++;
    Buffer = NULL;
  }

  if (SioAcpiTable != NULL) {
    //
    // Update the Sio SSDT table in the ACPI tables.
    //
    Status = AcpiSupport->SetAcpiTable (
                             AcpiSupport, 
                             SioAcpiTable, 
                             TRUE, 
                             Version, 
                             &AcpiTableHandle
                             );
  
    ASSERT_EFI_ERROR (Status);
    AcpiChecksum ((SioAcpiTable), SioAcpiTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
     
    //
    // Publish all ACPI Tables
    //
    Status = AcpiSupport->PublishTables (AcpiSupport, Version);
    ASSERT_EFI_ERROR (Status);

    //
    // ACPI support makes its own copy of the table, so clean up
    //
    gBS->FreePool (SioAcpiTable);
  }

  //
  // Close event
  //
  gBS->CloseEvent (Event);

}

/**
  Install Device protocol from PCD structure.
**/
VOID
InstallEnabledDeviceProtocol (
  VOID
  )
{
  EFI_STATUS                         Status;
  UINT8                              Index;
  SIO_DEVICE_LIST_TABLE              *PcdPointer;

  Status       = EFI_NOT_FOUND;
  
  Index = 0;
  while (mDeviceFunction[Index].Device != NULL_ID) {
    PcdPointer   = mTablePtr;
    while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
      if ((PcdPointer->TypeInstance == mSioResourceFunction->Instance)  && (PcdPointer->Device == mDeviceFunction[Index].Device) 
        && (PcdPointer->DeviceInstance == mDeviceFunction[Index].Instance)) {  
        PcdPointer->DeviceLdn= mDeviceFunction[Index].Ldn;
        //
        // if the device is enable, then install it
        //
        if ((PcdPointer->DeviceEnable != FALSE) || (PcdPointer->DeviceDma == NULL_ID)) {
          if (mDeviceFunction[Index].InstallDeviceProtocol != NULL) {
            Status = mDeviceFunction[Index].InstallDeviceProtocol (
                       PcdPointer
                       ); 
          }
        }
        if (PcdPointer->DeviceEnable == FALSE) {
          IDW8 (SIO_LDN, mDeviceFunction[Index].Ldn);
          IDW8 (SIO_DEV_ACTIVE, FALSE);
        } 
        break;
      } 
      PcdPointer++;
    }
    Index++;
  }  
}

/**
  Find SSDT table from RAW section of DXE driver, then publish it to ACPI table.

  @param[in]  Event           Event whose notification function is being invoked.
  @param[in]  Context         Pointer to the notification function's context.
**/
EFI_STATUS
UpdateSioMbox (
  IN EFI_ACPI_DESCRIPTION_HEADER      *SioAcpiTable
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *SsdtPointer;
  UINT8                                 NumOfEntries;
  UINT8                                 DeviceEntries;
  UINT8                                 DeviceH;
  UINT8                                 DeviceL;  
  UINT8                                 DeviceI;
  EFI_ACPI_DESCRIPTION_HEADER           *Table;
  SIO_DEVICE_LIST_TABLE                 *PcdPointer;
  SIO_NVS_AREA                          *SioNvsArea;
  EFI_ASL_RESOURCE                      *SioAslResource;
  BOOLEAN                               UpdateConfig;  

  UpdateConfig = FALSE;
  PcdPointer = mTablePtr;
  Status = EFI_SUCCESS;
  Table = SioAcpiTable;
  DeviceEntries = 0;
  NumOfEntries = sizeof (SIO_NVS_AREA) / sizeof (EFI_ASL_RESOURCE);
  DeviceH = mSioResourceFunction->TypeIdHigh;
  DeviceL = mSioResourceFunction->TypeIdLow;
  DeviceI = mSioResourceFunction->Instance;

  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  sizeof (SIO_NVS_AREA),
                  &SioNvsArea
                  );
                  
  ASSERT_EFI_ERROR (Status);  
  ZeroMem (SioNvsArea, sizeof (SIO_NVS_AREA));
  SioAslResource = (EFI_ASL_RESOURCE*) SioNvsArea;
  
  for (SsdtPointer = ((UINT8 *)((UINTN)Table)); SsdtPointer <= (UINT8 *)((UINTN)Table + (Table->Length)); SsdtPointer++) {
    switch ( *((UINT32*) SsdtPointer)) {

    //
    //  Search LGDN String
    //
    case (EFI_SIGNATURE_32 ('L', 'G', 'D', 'N')):
      //
      // Calculate the number of non-zero entries in the debug code table
      //
      while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
        //
        // Update PCD data to NVS area
        //
		
        if (DeviceEntries >= NumOfEntries) {
          break;
        }
		
        if ((PcdPointer->Device != CFG_ID) && (mSioResourceFunction->Instance == PcdPointer->TypeInstance)) {
          SioAslResource->Device = PcdPointer->DeviceEnable;
          SioAslResource->DeviceBaseIo = PcdPointer->DeviceBaseAdr;
          SioAslResource->DeviceLdn = PcdPointer->DeviceLdn; 

//[-start-131231-IB12970047-add]//
          if (PcdPointer->Device == KYBD) {
            if (!KbcExistence ()) {
              SioAslResource->Device = 0;
            }
          }
          
          if (PcdPointer->Device == MOUSE) {
            if (!MouseExistence()) {
              SioAslResource->Device = 0;
            }
          }
//[-end-131231-IB12970047-add]//
          
          SioAslResource++;
		  
          DeviceEntries++;
        }
        PcdPointer++;
      }
      
      if (IsAmlOpRegionObject (SsdtPointer)) {
        ASSERT_EFI_ERROR (Status);
        SetOpRegion (SsdtPointer, SioNvsArea, sizeof (SIO_NVS_AREA));   
      }
      break; 

    //
    // Update config port
    //   
    case (EFI_SIGNATURE_32 ('S','I','D','P')):
      if (!UpdateConfig) {      
        *(UINT8*) (SsdtPointer + 5) = (UINT8) mSioResourceFunction->SioCfgPort;
        UpdateConfig = TRUE;        
      }
      break;

    //
    // Update ASL name string to avoid the same device name in multi SIOs
    //
    case (EFI_SIGNATURE_32 ('M', 'o', 'd', 'u')):
      *(UINT16*) (SsdtPointer) = UintToChar (DeviceH);
      *((UINT16*) SsdtPointer + 1) = UintToChar (DeviceL);
      *((UINT16*) SsdtPointer + 2) = UintToChar (DeviceI);
      break;

    case (EFI_SIGNATURE_32 ('S', 'I', 'O', 'X')):
      *(SsdtPointer + 3) = (UINT8) (UintToChar (DeviceI) >> 8);
      break;

    default:
      break;
    }   
  } 
  
  return EFI_SUCCESS;
}

/**
  Convert a single number to character.
  It assumes the input Char is in the scope of L'0' ~ L'9' and L'A' ~ L'F'.
  
  @param[in] Num    The input char which need to change to a hex number.
**/
UINT16
UintToChar (
  IN UINT8                           Num
  )
{
  UINT8      Index;
  UINT16     Value;
  UINT8      TempValue;

  Value = 0;

  for (Index = 0; Index < (sizeof(UINT16)/sizeof(UINT8)); Index++) {
    TempValue = Num & 0xF;
    if ((TempValue >= 0) && (TempValue <= 9)) {
      Value = (Value << 8) + ((UINT16) (TempValue + L'0'));
    }

    if ((TempValue >= 0xA) && (TempValue <= 0xF)) {
      Value = (Value << 8) + ((UINT16) (TempValue + L'A' - 0xA ));
    }
    Num = Num >> 4;
  }
  
  return Value;
}

/**
  Create SCU by SIO and update value to PCD data.

  @retval      EFI_SUCCESS            Function complete successfully. 
**/
EFI_STATUS
SioScu (
  VOID
  )
{
  UINT8                             Index;
  UINT8                             ConfigurationNum;
  EFI_STATUS                        Status;
  UINTN                             BufferSize; 
  SIO_DEVICE_LIST_TABLE             *GetByConfiguration;
  SIO_CONFIGURATION                 *SioConfiguration;
  SIO_DEVICE_LIST_TABLE             *PcdPointer;

  PcdPointer = mTablePtr;
  ConfigurationNum = 0;
  SioConfiguration = NULL;

  //
  // Create variable and SIO page itself
  //
  Status = CreateSetupPage ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = sizeof (SIO_CONFIGURATION);
  SioConfiguration = AllocateZeroPool (sizeof (SIO_CONFIGURATION));
  Status = gRT->GetVariable (
                  mSioVariableName,
                  &gSioNct6683dluSetup00FormSetGuid,
                  NULL,
                  &BufferSize,
                  SioConfiguration
                  );

  if (Status == EFI_SUCCESS) {
    ConfigurationNum = sizeof(SIO_CONFIGURATION) / sizeof (SIO_DEVICE_LIST_TABLE);
    GetByConfiguration = (SIO_DEVICE_LIST_TABLE*) SioConfiguration;

    for (Index = 0; Index < ConfigurationNum; Index++) {
      //
      // Calculate the number of non-zero entries in the table
      //
      while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {
        //
        // Check device and instance, if it's right, update to PCD data 
        //
        if ((PcdPointer->Device == GetByConfiguration->Device) && (PcdPointer->DeviceInstance == GetByConfiguration->DeviceInstance)
          && (GetByConfiguration->DeviceEnable != SELECT_AUTO)) {
          PcdPointer->DeviceEnable = GetByConfiguration->DeviceEnable;
          PcdPointer->DeviceBaseAdr = UpsideDown(GetByConfiguration->DeviceBaseAdr);
          PcdPointer->DeviceIrq = GetByConfiguration->DeviceIrq;
          //
          // if user disable device, it still need to install protocol, but doesn't create the table of the IsaAcpi
          //          
          if (GetByConfiguration->DeviceEnable == SELECT_DISABLE) {
            PcdPointer->DeviceDma = NULL_ID;          
          }            
          PcdPointer++;
          break;
        }
        PcdPointer++;
      }
      GetByConfiguration++;
      PcdPointer = mTablePtr; 
    }
  }
  FreePool (SioConfiguration);
  return Status;
}
