@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set BASEDIR = %MAKEDIR%

set L= --hpkdir %MAKEDIR% --fif %WORKSPACE%\BaseTools\Fonts\Standard.fif --fuf %MAKEDIR%\FV\Font.fuf --output_hpk %MAKEDIR%\FV\Font.hpk %*

if exist %WORKSPACE%\%PROJECT_PKG%\Project.fif (
  set L= !L! --fif %WORKSPACE%\%PROJECT_PKG%\Project.fif
)

GenFont.exe %L% 

@exit %ERRORLEVEL%

