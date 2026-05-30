@echo off
call "%~dp0shared\enter-root-dir.bat" || exit /b 1

python tools/cli.py run web

