@REM
@REM Project post-build batch file
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************

@REM Run kernel post-build process
@if exist %WORKSPACE%\BaseTools\KernelPostBuild.bat call %WORKSPACE%\BaseTools\KernelPostBuild.bat %1

@REM Run Chipset specific post-build process
@GetProjectEnv CHIPSET_PKG > NUL && for /f %%a in ('GetProjectEnv CHIPSET_PKG') do set %%a
@if exist %WORKSPACE%\%CHIPSET_PKG%\ChipsetPostBuild.bat call %WORKSPACE%\%CHIPSET_PKG%\ChipsetPostBuild.bat %1

@REM [-start-120927-IB10820127-modify]
FitInfo.exe -i $(PROJECT_PKG)/PlatformConfig/PatchFitConfig.ini
@REM [-end-120927-IB10820127-modify]

for /f "tokens=2 delims=[.]," %%a in ('findstr /R "\[FD\." Project.fdf') do Set FileName=%%a

@REM ---run TXT ApFix up tool to patch ApInit module when TXT is enabled---
@REM [-start-130304-IB10130032-modify]
@REM [-start-120525-IB10820058-modify]
for /f "tokens=4" %%a in ('find "TXT_SUPPORT" Project.env') do if %%a==NO goto End_StaFixup
:StaFixUp
for /f "tokens=3" %%a in ('findstr /R "\<TARGET\>" ..\Conf\target.txt') do (
  %WORKSPACE%\SharkBayRcPkg\CPU\Txt\Tools\Stafixup.exe "D1E59F50-E8C3-4545-BF61-11F002233C97" %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\FV\%FileName%.fd "%WORKSPACE%\SharkBayRcPkg\CPU\Txt\Tools\Apfixup.txt" 0x20018
)
:End_StaFixup
@REM [-end-120525-IB10820058-modify]
@REM [-end-130304-IB10130032-modify]

@REM [-start-130625-IB10920030-add]
@FOR /f "tokens=4" %%a IN ('find "UNSIGNED_FV_SUPPORT" Project.env') DO IF %%a==NO GOTO End_Gen_Unsigned_Fv_Bios
:Gen_Unsigned_Fv_Bios
@FOR /f "tokens=3" %%a IN ('findstr /R "\<TARGET\>" ..\Conf\target.txt') DO (
  @IF NOT EXIST %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\Fv\%FileName%.fd GOTO End_Gen_Unsigned_Fv_Bios
  %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\Ia32\GenUnsignedFv.exe %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\Fv\%FileName%.fd
)
@IF NOT %ERRORLEVEL%==0 EXIT /B
:End_Gen_Unsigned_Fv_Bios
@REM [-end-130625-IB10920030-add]

for /f "tokens=3" %%a in ('findstr /R "\<TARGET\>" ..\Conf\target.txt') do (
  @if exist %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\FV\*.fd   copy %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\FV\*.fd   .\BIOS /y
)

:Gen_Pfat_Bios
@REM
@REM Generate FD with PFAT Header
@REM -b : BIOS Offset in Physical SPI address
@if not exist .\BIOS\%FileName%.fd goto End_Gen_Pfat_Bios
copy .\BIOS\%FileName%.fd .\BIOS\%FileName%_PFAT.fd /y
GenPfatHdr /l .\PlatformConfig\PFATSetting.ini .\BIOS\%FileName%_PFAT.fd
:End_Gen_Pfat_Bios

@REM FvAlignChecker.exe for 4K Align Check 
FvAlignChecker.exe --PCD-report-file %WORKSPACE%/Build/%PROJECT_PKG%/BuildReport.txt

:Fitc
cd ".\ME_Kit\Tools\System Tools\Flash Image Tool"
Fitc.exe /b ICBD_70K.xml
copy .\ICBD_70K.bin .\..\..\..\..\BIOS /y
:End_Fitc
