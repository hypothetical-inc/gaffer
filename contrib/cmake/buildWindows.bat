rem We hide the CMakeLists.txt here and copy it to root so it works correctly
set ROOT_DIR=%~dp0%..\..
cd %ROOT_DIR%

del /f CMakeCache.txt

cmake -Wno-dev -G %CMAKE_GENERATOR% ^
-DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
-DCMAKE_INSTALL_PREFIX=%BUILD_DIR% ^
-DCMAKE_PREFIX_PATH=%BUILD_DIR% ^
-DPYTHON_LIBRARY=%BUILD_DIR%\lib ^
-DPYTHON_INCLUDE_DIR=%BUILD_DIR%\include ^
-DCORTEX_LOCATION=%BUILD_DIR% ^
-DBOOST_ROOT=%BUILD_DIR% ^
-DILMBASE_LOCATION=%BUILD_DIR% ^
-DOPENEXR_LOCATION=%BUILD_DIR% ^
-DOSL_INCLUDE_DIR=%BUILD_DIR%\include ^
-DOSL_EXEC_LIBRARY=%BUILD_DIR%\lib\oslexec.lib ^
-DOSL_COMP_LIBRARY=%BUILD_DIR%\lib\oslcomp.lib ^
-DOSL_QUERY_LIBRARY=%BUILD_DIR%\lib\oslquery.lib ^
-DAPPLESEED_INCLUDE_DIR=%BUILD_DIR%\appleseed\include ^
-DAPPLESEED_LIBRARY=%BUILD_DIR%\appleseed\lib\appleseed.lib ^
-DOPENVDB_LOCATION=%BUILD_DIR% -DARNOLD_ROOT=%ARNOLD_ROOT% ^
%ROOT_DIR%\contrib\cmake

if %ERRORLEVEL% NEQ 0 (exit /b %ERRORLEVEL%)
cmake --build . --config %BUILD_TYPE% --target install
if %ERRORLEVEL% NEQ 0 (exit /b %ERRORLEVEL%)
