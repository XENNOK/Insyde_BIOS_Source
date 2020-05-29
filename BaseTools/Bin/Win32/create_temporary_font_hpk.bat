@echo off

@REM Retrieve TARGET variable from target.txt
@REM Exclude comment and TARGET_ARCH
set CONFIG_FILE=%WORKSPACE%\Conf\target.txt
for /f "tokens=1,2* delims== " %%i in ('type %CONFIG_FILE% ^|
  find "TARGET" ^| find /V "#" ^| find /V "TARGET_ARCH"') do set TARGET=%%j

set FONT_DIR=%WORKSPACE%\Build\%PROJECT_PKG%\%TARGET%_%TOOL_CHAIN%\FV
if not exist %FONT_DIR% mkdir %FONT_DIR%
echo.>%WORKSPACE%\Build\%PROJECT_PKG%\%TARGET%_%TOOL_CHAIN%\FV\Font.hpk