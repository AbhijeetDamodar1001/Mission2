@echo off
set PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%
mingw32-make
if errorlevel 1 (
  echo Build failed. Check that MSYS2 UCRT64 GCC and make are installed.
  exit /b 1
)
echo Build complete.
