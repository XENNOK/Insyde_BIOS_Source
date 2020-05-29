//;******************************************************************************
//;* Copyright (c) 1983-2010, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   DebugAgentLib.c
//; 
//; Abstract:
//; 
//;   The EDKII DebugAgent Library used for H2ODDT
//;

#include <Library/DebugAgentLib.h>

VOID AsmSaveIdt();
VOID AsmRestoreIdt();
VOID AsmSmmSetup(BOOLEAN);

/**
  Initialize debug agent.

  This function is used to set up debug environment to support source level debugging.
  If certain Debug Agent Library instance has to save some private data in the stack,
  this function must work on the mode that doesn't return to the caller, then
  the caller needs to wrap up all rest of logic after InitializeDebugAgent() into one
  function and pass it into InitializeDebugAgent(). InitializeDebugAgent() is
  responsible to invoke the passing-in function at the end of InitializeDebugAgent().

  If the parameter Function is not NULL, Debug Agent Libary instance will invoke it by
  passing in the Context to be its parameter.

  If Function() is NULL, Debug Agent Library instance will return after setup debug
  environment.

  @param[in] InitFlag     Init flag is used to decide the initialize process.
  @param[in] Context      Context needed according to InitFlag; it was optional.
  @param[in] Function     Continue function called by debug agent library; it was
                          optional.

**/
VOID
EFIAPI
InitializeDebugAgent (
  IN UINT32                InitFlag,
  IN VOID                  *Context, OPTIONAL
  IN DEBUG_AGENT_CONTINUE  Function  OPTIONAL
  )
{
  switch (InitFlag) {
  case DEBUG_AGENT_INIT_SMM:
    AsmSaveIdt();
    break;
  case DEBUG_AGENT_INIT_ENTER_SMI:
    AsmRestoreIdt();
    AsmSmmSetup(TRUE);
    break;
  case DEBUG_AGENT_INIT_EXIT_SMI:
    AsmSmmSetup(FALSE);
    break;
  }
  if (Function != NULL) {
    Function (Context);
  }
}

/**
  Enable/Disable the interrupt of debug timer and return the interrupt state
  prior to the operation.

  If EnableStatus is TRUE, enable the interrupt of debug timer.
  If EnableStatus is FALSE, disable the interrupt of debug timer.

  @param[in] EnableStatus    Enable/Disable.

  @return FALSE always.

**/
BOOLEAN
EFIAPI
SaveAndSetDebugTimerInterrupt (
  IN BOOLEAN                EnableStatus
  )
{
  return FALSE;
}

