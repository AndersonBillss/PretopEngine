@echo off
call "%~dp0shared\enter-root-dir.bat" || exit /b 1

uv run python cli.py build web