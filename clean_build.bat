@echo off
chcp 65001 > nul
title STM32编译文件一键清理 (适配Keil5/STM32Cube/VSCode)
color 0A
echo ==============================================
echo          STM32编译文件一键清理工具
echo          适配：Keil5 / STM32CubeMX / VSCode
echo          清理：中间文件/编译产物/日志/缓存
echo ==============================================
echo.

:: 定义需要清理的文件后缀（覆盖三大环境的编译产物）
set "del_suffix=.o .elf .hex .bin .lst .map .axf .dep .bak .tmp .log .crf .htm .sct .bak0 .i .ii .s .d"
:: 定义需要清理的文件夹（三大环境的默认编译目录）
set "del_folders=build cmake-build-* Debug Release obj out bin dist .vscode/ipch .vscode/cmake .mxproject"
:: Keil5专属清理（UV4编译的零散文件+临时目录）
set "keil_del=*.lnp *.plg *.bak *.tmp __vm *.uvopt.bak *.uvproj.bak"

echo 【1/3】正在删除编译产物文件（.hex/.bin/.elf/.axf等）...
for %%s in (%del_suffix%) do (
    for /r . %%f in (*%%s) do (
        if exist "%%f" del /f /q "%%f" > nul
    )
)

echo 【2/3】正在删除Keil5专属编译文件（.plg/.lnp/备份等）...
for %%k in (%keil_del%) do (
    if exist "%%k" del /f /q "%%k" > nul
)
:: 删除Keil5的临时编译目录
if exist "UV4" rmdir /s /q "UV4" > nul

echo 【3/3】正在删除编译中间文件夹（build/obj/Debug/Release等）...
for %%d in (%del_folders%) do (
    if exist "%%d" rmdir /s /q "%%d" > nul
)

:: 额外清理VSCode/CMake的缓存文件
if exist "CMakeCache.txt" del /f /q "CMakeCache.txt" > nul
if exist "Makefile" del /f /q "Makefile" > nul
if exist "cmake_install.cmake" del /f /q "cmake_install.cmake" > nul

echo.
echo ==============================================
echo ✅ 清理完成！项目已恢复干净的初始状态
echo ❗ 保留文件：源码/工程文件/配置文件/STM32CubeMX配置
echo ==============================================
echo.
pause