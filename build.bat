@echo off
setlocal enabledelayedexpansion

:: Colors for output (Windows 10+)
set "GREEN=[92m"
set "YELLOW=[93m"
set "RED=[91m"
set "BLUE=[94m"
set "RESET=[0m"

:: Check if CMake is available
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo %RED%Error: CMake not found in PATH%RESET%
    echo Please install CMake and add it to your PATH
    exit /b 1
)

:: Check if Ninja is available (optional, but recommended)
where ninja >nul 2>&1
if %errorlevel% neq 0 (
    echo %YELLOW%Warning: Ninja not found. Using default generator.%RESET%
)

:: Show help if no arguments
if "%1"=="" (
    echo %BLUE%========================================%RESET%
    echo %GREEN%Game Engine Build Script%RESET%
    echo %BLUE%========================================%RESET%
    echo.
    echo Usage: build.bat [preset] [options]
    echo.
    echo Presets:
    echo   runtime         - Build only Sandbox
    echo   editor          - Build only Editor
    echo   editor-runtime  - Build Editor + Sandbox
    echo   server          - Build only Server
    echo   full            - Build everything
    echo   web             - Build for Web/Emscripten
    echo.
    echo Options:
    echo   --clean         - Clean build directory before building
    echo   --configure     - Only configure, don't build
    echo   --build         - Only build (after configuration)
    echo   --verbose       - Verbose output
    echo   --help          - Show this help
    echo.
    echo Examples:
    echo   build.bat runtime
    echo   build.bat editor --clean
    echo   build.bat full --verbose
    echo.
    exit /b 0
)

:: Parse arguments
set "PRESET=%1"
set "CLEAN=0"
set "CONFIGURE_ONLY=0"
set "BUILD_ONLY=0"
set "VERBOSE=0"
shift

:parse_args
if not "%1"=="" (
    if "%1"=="--clean" set "CLEAN=1"
    if "%1"=="--configure" set "CONFIGURE_ONLY=1"
    if "%1"=="--build" set "BUILD_ONLY=1"
    if "%1"=="--verbose" set "VERBOSE=1"
    if "%1"=="--help" (
        echo Usage: build.bat [preset] [options]
        echo Options: --clean, --configure, --build, --verbose, --help
        exit /b 0
    )
    shift
    goto parse_args
)

:: Validate preset
set "VALID_PRESETS=runtime editor editor-runtime server full web"
set "IS_VALID=0"
for %%p in (%VALID_PRESETS%) do (
    if "%%p"=="%PRESET%" set "IS_VALID=1"
)

if %IS_VALID%==0 (
    echo %RED%Error: Unknown preset '%PRESET%'%RESET%
    echo Available presets: %VALID_PRESETS%
    exit /b 1
)

:: Check for Emscripten if building web
if "%PRESET%"=="web" (
    where emcmake >nul 2>&1
    if %errorlevel% neq 0 (
        echo %RED%Error: emcmake not found. Please install Emscripten.%RESET%
        echo Set EMSDK environment variable or add Emscripten to PATH
        exit /b 1
    )
)

:: Set build directory
set "BUILD_DIR=build\%PRESET%"

:: Clean if requested
if %CLEAN%==1 (
    echo %YELLOW%Cleaning build directory: %BUILD_DIR%%RESET%
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
        echo %GREEN%Cleaned successfully%RESET%
    ) else (
        echo %YELLOW%Build directory doesn't exist, nothing to clean%RESET%
    )
)

:: Configure
if %BUILD_ONLY%==0 (
    echo %BLUE%Configuring preset: %PRESET%%RESET%
    
    if "%PRESET%"=="web" (
        emcmake cmake --preset %PRESET%
    ) else (
        cmake --preset %PRESET%
    )
    
    if %errorlevel% neq 0 (
        echo %RED%Configuration failed!%RESET%
        exit /b %errorlevel%
    )
    echo %GREEN%Configuration successful%RESET%
)

:: Build
if %CONFIGURE_ONLY%==0 (
    echo %BLUE%Building preset: %PRESET%%RESET%
    
    if %VERBOSE%==1 (
        cmake --build "%BUILD_DIR%" --verbose
    ) else (
        cmake --build "%BUILD_DIR%"
    )
    
    if %errorlevel% neq 0 (
        echo %RED%Build failed!%RESET%
        exit /b %errorlevel%
    )
    echo %GREEN%Build successful%RESET%
)

echo %GREEN%========================================%RESET%
echo %GREEN%Build completed successfully!%RESET%
echo %GREEN%Output directory: %BUILD_DIR%%RESET%
echo %GREEN%========================================%RESET%
