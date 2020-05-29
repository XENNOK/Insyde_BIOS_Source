//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   SMM I/O access utility implementation file, for Ia32
//;

//
// Include files
//
#include "SmmIoLib.h"

UINT8
SmmIoRead8 (
  IN  UINT16    Address
  )
/*++

Routine Description:

  Do a one byte IO read

Arguments:

  Address - IO address to read

Returns:

  Data read

--*/
{
  UINT8   Buffer;

  ASSERT (mSmst);

  mSmst->SmmIo.Io.Read (
                  &mSmst->SmmIo,
                  SMM_IO_UINT8,
                  Address,
                  1,
                  &Buffer
                  );
  return Buffer;
}

VOID
SmmIoWrite8 (
  IN  UINT16    Address,
  IN  UINT8     Data
  )
/*++

Routine Description:

  Do a one byte IO write

Arguments:

  Address - IO address to write
  Data    - Data to write

Returns:

  None.

--*/
{
  ASSERT (mSmst);

  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT8,
                    Address,
                    1,
                    &Data
                    );
}

UINT16
SmmIoRead16 (
  IN  UINT16    Address
  )
/*++

Routine Description:

  Do a two byte IO read

Arguments:

  Address - IO address to read

Returns:

  Data read

--*/
{
  UINT16      Buffer;

  ASSERT (mSmst);

  mSmst->SmmIo.Io.Read (
                  &mSmst->SmmIo,
                  SMM_IO_UINT16,
                  Address,
                  1,
                  &Buffer
                  );
  return Buffer;
}

VOID
SmmIoWrite16 (
  IN  UINT16    Address,
  IN  UINT16    Data
  )
/*++

Routine Description:

  Do a two byte IO write

Arguments:

  Address - IO address to write
  Data    - Data to write

Returns:

  None.

--*/
{
  ASSERT (mSmst);

  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT16,
                    Address,
                    1,
                    &Data
                    );
}

UINT32
SmmIoRead32 (
  IN  UINT16    Address
  )
/*++

Routine Description:

  Do a four byte IO read

Arguments:

  Address - IO address to read

Returns:

  Data read

--*/
{
  UINT32        Buffer;

  ASSERT (mSmst);

  mSmst->SmmIo.Io.Read (
                    &mSmst->SmmIo,
                    SMM_IO_UINT32,
                    Address,
                    1,
                    &Buffer
                    );
  return Buffer;
}

VOID
SmmIoWrite32 (
  IN  UINT16    Address,
  IN  UINT32    Data
  )
/*++

Routine Description:

  Do a four byte IO write

Arguments:

  Address - IO address to write
  Data    - Data to write

Returns:

  None.

--*/
{
  ASSERT (mSmst);

  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT32,
                    Address,
                    1,
                    &Data
                    );
}

VOID
SmmStall (
  IN  UINTN   Microseconds
  )
/*++

Routine Description:

  Delay for at least the request number of microseconds.
  Timer used is DMA refresh timer, which has 15us granularity.
  You can call with any number of microseconds, but this
  implementation cannot support 1us granularity.

Arguments:

  Microseconds - Number of microseconds to delay.

Returns:

  None

--*/
{
  UINT8 Data;
  UINT8 InitialState;
  UINTN CycleIterations;

  CycleIterations = 0;
  Data            = 0;
  InitialState    = 0;

  //
  // The time-source is 15 us granular, so calibrate the timing loop
  // based on this baseline
  // Error is possible 15us.
  //
  CycleIterations = (Microseconds / 15) + 1;

  //
  // Use the DMA Refresh timer in port 0x61.  Cheap but effective.
  // The only issue is that the granularity is 15us, and we want to
  // guarantee "at least" one full transition to avoid races.
  //
  //
  //   _____________/----------\__________/--------
  //
  //                |<--15us-->|
  //
  // --------------------------------------------------> Time (us)
  //
  while (CycleIterations--) {
    Data = SmmIoRead8 (0x61);
    InitialState = Data;

    //
    // Capture first transition (strictly less than one period)
    //
    while (InitialState == Data) {
      Data = SmmIoRead8 (0x61);
    }

    InitialState = Data;
    //
    // Capture next transition (guarantee at least one full pulse)
    //
    while (InitialState == Data) {
      Data = SmmIoRead8 (0x61);
    }
  }
}
