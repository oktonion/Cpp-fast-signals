mkdir .\tests\bin
mkdir .\tests\obj

setlocal enabledelayedexpansion

set "build_ok=1"


for /f %%f in ('dir /b ".\tests\*.cpp"') do (
  echo "compiling test Borland C++ %%~nf"
  bcc32 -X- -w-inl -w-ccc -tWC -Q -n.\tests\obj\ -c ".\tests\%%f"
  IF ERRORLEVEL 1 (
    set "build_ok=0"
    exit /B 1
  )

  if "%build_ok%"=="1" (
    bcc32 -X- -w-inl -w-ccc -tWC -Q -lap -e.\tests\bin\%%~nf.exe cw32mt.lib .\tests\obj\%%~nf.obj
    IF ERRORLEVEL 1 (
      set "build_ok=0"
      exit /B 1
    )
  )
)

if /I "%build_ok%" NEQ "1" (
  echo "tests build failed"
  exit /B 1
)

echo "tests build ok"

del /Q .\tests\obj\*.obj