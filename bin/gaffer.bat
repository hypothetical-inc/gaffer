setlocal EnableDelayedExpansion

set GAFFER_ROOT=%~dp0%..

set HOME=%USERPROFILE%

call :prependToPath "%GAFFER_ROOT%\glsl" IECOREGL_SHADER_PATHS
call :prependToPath "%GAFFER_ROOT%\glsl" IECOREGL_SHADER_INCLUDE_PATHS

call :prependToPath "%GAFFER_ROOT%\fonts" IECORE_FONT_PATHS
call :prependToPath "%GAFFER_ROOT%\ops" IECORE_OP_PATHS

call :prependToPath "%USERPROFILE%\gaffer\opPresets;%GAFFER_ROOT%\opPresets" IECORE_OP_PRESET_PATHS
call :prependToPath "%USERPROFILE%\gaffer\procedurals;%GAFFER_ROOT%\procedurals" IECORE_PROCEDURAL_PATHS
call :prependToPath "%USERPROFILE%\gaffer\proceduralPresets;%GAFFER_ROOT%\proceduralPresets" IECORE_PROCEDURAL_PRESET_PATHS

set CORTEX_POINTDISTRIBUTION_TILESET=%GAFFER_ROOT%\resources\cortex\tileset_2048.dat

call :prependToPath "%USERPROFILE%\gaffer\apps;%GAFFER_ROOT%\apps" GAFFER_APP_PATHS

call :prependToPath "%USERPROFILE%\gaffer\startup" GAFFER_STARTUP_PATHS
call :appendToPath "%GAFFER_ROOT%\startup" GAFFER_STARTUP_PATHS

call :prependToPath "%GAFFER_ROOT%\graphics" GAFFERUI_IMAGE_PATHS

set OSLHOME=%GAFFER_ROOT%

call :prependToPath "%USERPROFILE%\gaffer\shaders;%GAFFER_ROOT%\shaders" OSL_SHADER_PATHS

set GAFFEROSL_CODE_DIRECTORY=%USERPROFILE%\gaffer\oslCode
call :prependToPath "%GAFFER_OSL_CODE_DIRECTORY%" PATH

set PYTHONHOME=%GAFFER_ROOT%

call :prependToPath "%GAFFER_ROOT%\python" PYTHONPATH

call :prependToPath "%GAFFER_ROOT%\lib" PATH

set QT_OPENGL=desktop
set QT_QPA_PLATFORM_PLUGIN_PATH=%GAFFER_ROOT%\qt\plugins

call :prependToPath "%GAFFER_ROOT%\bin" PATH

rem Appleseed
if "%APPLESEED%" == "" (
	if EXIST %GAFFER_ROOT%\appleseed (
		set APPLESEED=%GAFFER_ROOT%\appleseed
	)
)

rem Appleseed
if "%APPLESEED%" NEQ "" (
	call :prependToPath "%APPLESEED%\bin;%APPLESEED%\lib" PATH
	call :prependToPath "%APPLESEED%\lib\python2.7" PYTHONPATH
	call :prependToPath "%APPLESEED%\shaders\gaffer" OSL_SHADER_PATHS
	call :prependToPath "%OSL_SHADER_PATHS%;%GAFFER_ROOT%\appleseedDisplay" APPLESEED_SEARCHPATH
)

rem Arnold
if "%ARNOLD_ROOT%" NEQ "" (
	call :prependToPath "%GAFFER_ROOT%\arnold\plugins" ARNOLD_PLUGIN_PATH
	call :appendToPath "%ARNOLD_ROOT%\bin" PATH
	call :appendToPath "%ARNOLD_ROOT%\python" PYTHONPATH
)

@echo on
%GAFFER_ROOT%\bin\python.exe %GAFFER_ROOT%/bin/gaffer.py %*
if %ERRORLEVEL% NEQ 0 (
	echo "Error(s) running Gaffer"
	exit /b %ERRORLEVEL%
)

ENDLOCAL
exit /B 0

:prependToPath
	set "%~2=%~1;!%~2!"
	echo !%~2!
	exit /B 0

:appendToPath
	set "%~2=!%~2!;%~1"
	exit /B 0