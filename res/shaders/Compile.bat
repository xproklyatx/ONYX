@echo off
setlocal enabledelayedexpansion

:: ============================================
:: Compile.bat
:: ONYX Engine — Automated Shader Compilation
:: ============================================

set SCRIPT_DIR=%~dp0
set OUTPUT_DIR=%SCRIPT_DIR%
set DXC=dxc.exe

:: Check if DXC is available
where %DXC% >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] dxc.exe not found! Install DirectX Shader Compiler.
    echo Download from: https://github.com/microsoft/DirectXShaderCompiler/releases
    exit /b 1
)

echo ==================================================
echo ONYX Engine — Shader Compilation
echo ==================================================
echo.

set COMPILED_COUNT=0
set TOTAL_FILES=0
set FAILED_COUNT=0
set SKIPPED_COUNT=0

:: Loop through all .hlsl files in the directory
for %%f in ("%SCRIPT_DIR%*.hlsl") do (
    set /a TOTAL_FILES+=1
    set FILE_NAME=%%~nf
    set FILE_PATH=%%f
    
    echo [%%~nxf]
    
    :: Check if file has any ENTRY markers
    findstr /C:"// ENTRY:" "%%f" >nul 2>&1
    if errorlevel 1 (
        echo   [SKIP] No entry points found
        set /a SKIPPED_COUNT+=1
        echo.
    ) else (
        :: Parse all // ENTRY: lines in the file
        :: Format: // ENTRY: EntryPoint ShaderProfile
        for /f "usebackq tokens=1-4" %%a in (`findstr /C:"// ENTRY:" "%%f"`) do (
            :: %%a = "//"  %%b = "ENTRY:"  %%c = EntryPoint  %%d = ShaderProfile
            set ENTRY_POINT=%%c
            set SHADER_PROFILE=%%d
            
            echo   Compiling !ENTRY_POINT! ^(!SHADER_PROFILE!^)...
            
            set OUTPUT_CSO=!OUTPUT_DIR!\!FILE_NAME!_!ENTRY_POINT!.cso
            
            %DXC% -T !SHADER_PROFILE! -E !ENTRY_POINT! -Fo "!OUTPUT_CSO!" -WX -Zi "%%f" 2>&1
            
            if !errorlevel! equ 0 (
                echo     [OK] !FILE_NAME!_!ENTRY_POINT!.cso
                set /a COMPILED_COUNT+=1
            ) else (
                echo     [FAIL] !ENTRY_POINT! compilation failed!
                set /a FAILED_COUNT+=1
            )
        )
        echo.
    )
)

echo ==================================================
echo Compiled:     !COMPILED_COUNT! shaders
echo Files:        !TOTAL_FILES! total
if !SKIPPED_COUNT! gtr 0 echo Skipped:      !SKIPPED_COUNT! files (no entry points)
if !FAILED_COUNT! gtr 0 echo Failed:       !FAILED_COUNT! shaders
echo ==================================================

if !FAILED_COUNT! gtr 0 exit /b 1
exit /b 0