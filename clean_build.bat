@echo off
echo Cleaning STM32 project build files and temporary data...

:: 1. Delete build directories
if exist "build" (
    rmdir /s /q "build"
    echo Deleted build/ directory
)
if exist "bin" (
    rmdir /s /q "bin"
    echo Deleted bin/ directory
)
if exist "obj" (
    rmdir /s /q "obj"
    echo Deleted obj/ directory
)
if exist ".cache" (
    rmdir /s /q ".cache"
    echo Deleted .cache/ directory
)

:: 2. Delete compiled binaries
del /f /q "*.elf" 2>nul
del /f /q "*.hex" 2>nul
del /f /q "*.bin" 2>nul
del /f /q "*.map" 2>nul
del /f /q "*.lst" 2>nul
echo Deleted all compiled binaries

:: 3. Delete VS Code cache
if exist ".vscode/ipch" (
    rmdir /s /q ".vscode/ipch"
    echo Deleted .vscode/ipch/ cache
)

echo Cleanup completed!
pause