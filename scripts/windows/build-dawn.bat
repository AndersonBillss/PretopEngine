@echo off
setlocal
call "%~dp0shared\load-msvc.bat" || exit /b 1
call "%~dp0shared\enter-root-dir.bat" || exit /b 1

echo Building Dawn
python tools/build.py dawn-debug-setup