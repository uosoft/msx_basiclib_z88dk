@echo off
REM MSX BASIC Library Build Script
REM Requires: Z88DK (zcc and z80asm in PATH)

setlocal enabledelayedexpansion

set LIBDIR=%~dp0
set INCDIR=%LIBDIR%include
set SRCDIR=%LIBDIR%src\msxbasic
set OUTDIR=%LIBDIR%lib

REM Create output directory
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

echo === MSX BASIC Library Build ===
echo.

REM Compiler settings
set ZCC=zcc
set TARGET=+msx
set CFLAGS=-vn -O3 -compiler=sccz80

echo Compiling source files...

REM Compile each source file
for %%f in (screen graphics sound input bstring bmath system vdp) do (
    echo   Compiling %%f.c...
    %ZCC% %TARGET% %CFLAGS% -I"%INCDIR%" -c "%SRCDIR%\%%f.c" -o "%SRCDIR%\%%f.o"
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f.c
        exit /b 1
    )
)

echo.
echo Creating library...

REM Create library using z80asm
z80asm -x"%OUTDIR%\msxbasic.lib" "%SRCDIR%\screen.o" "%SRCDIR%\graphics.o" "%SRCDIR%\sound.o" "%SRCDIR%\input.o" "%SRCDIR%\bstring.o" "%SRCDIR%\bmath.o" "%SRCDIR%\system.o" "%SRCDIR%\vdp.o"

if errorlevel 1 (
    echo ERROR: Failed to create library
    exit /b 1
)

echo.
echo === Build Complete ===
echo Library created: %OUTDIR%\msxbasic.lib
echo.

endlocal
