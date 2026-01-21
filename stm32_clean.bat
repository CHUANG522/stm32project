@echo off
chcp 65001 >nul
echo 🧹 正在清理 STM32 项目编译产物和临时文件...

:: --------------------------
:: 1. 清理 Keil 编译产物
:: --------------------------
del /q /s *.o 2>nul
del /q /s *.obj 2>nul
del /q /s *.axf 2>nul
del /q /s *.elf 2>nul
del /q /s *.hex 2>nul
del /q /s *.bin 2>nul
del /q /s *.map 2>nul
del /q /s *.lst 2>nul
del /q /s *.d 2>nul
del /q /s *.crf 2>nul
del /q /s *.lnp 2>nul
del /q /s *.bak 2>nul
del /q /s *.plg 2>nul
del /q /s *.htm 2>nul

:: --------------------------
:: 2. 清理 Keil 编译目录
:: --------------------------
rd /s /q Objects 2>nul
rd /s /q Listings 2>nul
rd /s /q Debug 2>nul
rd /s /q Release 2>nul

:: --------------------------
:: 3. 清理 CubeMX 临时文件
:: --------------------------
del /q /s *.ioc.bak 2>nul
del /q /s *.mxproject.bak 2>nul
del /q /s *.ioc.user 2>nul

:: --------------------------
:: 4. 清理其他 IDE 临时文件
:: --------------------------
rd /s /q STM32CubeIDE 2>nul
rd /s /q .settings 2>nul
del /q /s .cproject 2>nul
del /q /s .project 2>nul

:: --------------------------
:: 5. 清理系统临时文件
:: --------------------------
del /q /s Thumbs.db 2>nul
del /q /s .DS_Store 2>nul

echo ✅ 清理完成！
pause