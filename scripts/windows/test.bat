
@echo off
setlocal
call "%~dp0shared\load-msvc.bat" || exit /b 1
call "%~dp0shared\enter-root-dir.bat" || exit /b 1

python tools/cli.py run test