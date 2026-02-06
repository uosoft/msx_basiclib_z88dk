@echo off
REM Build an example program
REM Usage: build_sample.bat <example_name>
REM Example: build_sample.bat 01_hello
REM
REM Requires: Z88DK (zcc in PATH)
REM Run build.bat first to create the library.

setlocal

set LIBDIR=%~dp0
set INCDIR=%LIBDIR%include
set OUTDIR=%LIBDIR%lib
set EXDIR=%LIBDIR%examples

if "%~1"=="" (
    echo Usage: build_sample.bat ^<example_name^>
    echo Example: build_sample.bat 01_hello
    exit /b 1
)

set SAMPLE=%~1

echo === Building Example: %SAMPLE% ===
echo.

REM Check if library exists
if not exist "%OUTDIR%\msxbasic.lib" (
    echo ERROR: Library not found. Run build.bat first.
    exit /b 1
)

REM Check if source exists
if not exist "%EXDIR%\%SAMPLE%.c" (
    echo ERROR: Source file not found: %EXDIR%\%SAMPLE%.c
    exit /b 1
)

echo Compiling %SAMPLE%.c to ROM...
zcc +msx -vn -O3 -compiler=sccz80 -lm -I"%INCDIR%" -L"%OUTDIR%" -lmsxbasic -subtype=rom "%EXDIR%\%SAMPLE%.c" -o "%EXDIR%\%SAMPLE%" -create-app

if errorlevel 1 (
    echo ERROR: Compilation failed
    exit /b 1
)

echo.
echo === Build Complete ===
echo ROM created: %EXDIR%\%SAMPLE%.rom
echo.

endlocal
