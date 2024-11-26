::[Bat To Exe Converter]
::
::YAwzoRdxOk+EWAjk
::fBw5plQjdCuDJFyQ934yKQ9rWAu9OXO/OqcZ6+ft6vienUscQus8d4GV07eBQA==
::YAwzuBVtJxjWCl3EqQJgSA==
::ZR4luwNxJguZRRnk
::Yhs/ulQjdF+5
::cxAkpRVqdFKZSDk=
::cBs/ulQjdF+5
::ZR41oxFsdFKZSDk=
::eBoioBt6dFKZSDk=
::cRo6pxp7LAbNWATEpCI=
::egkzugNsPRvcWATEpCI=
::dAsiuh18IRvcCxnZtBJQ
::cRYluBh/LU+EWAnk
::YxY4rhs+aU+JeA==
::cxY6rQJ7JhzQF1fEqQJQ
::ZQ05rAF9IBncCkqN+0xwdVs0
::ZQ05rAF9IAHYFVzEqQJQ
::eg0/rx1wNQPfEVWB+kM9LVsJDGQ=
::fBEirQZwNQPfEVWB+kM9LVsJDGQ=
::cRolqwZ3JBvQF1fEqQJQ
::dhA7uBVwLU+EWDk=
::YQ03rBFzNR3SWATElA==
::dhAmsQZ3MwfNWATElA==
::ZQ0/vhVqMQ3MEVWAtB9wSA==
::Zg8zqx1/OA3MEVWAtB9wSA==
::dhA7pRFwIByZRRnk
::Zh4grVQjdCyDJGyX8VAjFAlTTjimOXixEroM1Pr+7OWRp18Ea+sxa5va1riLMq4W8kCE
::YB416Ek+ZG8=
::
::
::978f952a14a936cc963da21a135fa983
@echo off
setlocal enabledelayedexpansion

REM Store the original directory
set "ORIGINAL_DIR=%CD%"

REM Change to the script's directory
cd /d "%~dp0"

echo [Debug] Script started.
echo [Debug] Changed directory to %CD%.

REM Set directories
set "INSTALLER_DIR=installer"
set "UPDATER_DIR=updater"

REM Set required files
set "REQUIRED_FILES=UpdaterLauncher.exe UpdaterRenamer.exe update_updaterlauncher.bat DynamoDBModule.dll"

echo [Debug] Checking for _installer.exe files.
if exist "%INSTALLER_DIR%\" (
    for %%f in ("%INSTALLER_DIR%\*_installer.exe") do (
        echo Found installer file: %%~dpnxf
        set "INSTALLER_FILE=%%f"
        goto :check_files
    )
)
echo [Error] No installer files found in /%INSTALLER_DIR%/.
goto :end_script

:check_files
echo [Debug] Checking required files in /%UPDATER_DIR%/.
set REQUIRED_FILES_MISSING=0
for %%f in (%REQUIRED_FILES%) do (
    if not exist "%UPDATER_DIR%\%%f" (
        echo [Error] Missing file: %CD%\%UPDATER_DIR%\%%f
        set /a REQUIRED_FILES_MISSING+=1
    ) else (
        echo Found file: %CD%\%UPDATER_DIR%\%%f
    )
)

echo [Debug] Checking UpdaterLauncher.exe size.
set "FILE_PATH=%UPDATER_DIR%\UpdaterLauncher.exe"
if exist "%FILE_PATH%" (
    for %%f in ("%FILE_PATH%") do set "FILE_SIZE=%%~zf"
    echo Debug: %FILE_PATH% size is %FILE_SIZE% bytes.
    if !FILE_SIZE! LSS 1024 (
        echo [Error] UpdaterLauncher.exe size is less than 1KB.
        set /a REQUIRED_FILES_MISSING+=1
    )
) else (
    echo [Debug] %FILE_PATH% not found.
    set /a REQUIRED_FILES_MISSING+=1
)

if !REQUIRED_FILES_MISSING! GTR 0 (
    echo [Error] Missing or invalid files. Executing installer with forced options.
    call "%INSTALLER_FILE%" --root "%CD%" ^
        --accept-licenses ^
        --default-answer ^
        --confirm-command ^
        --auto-answer OverwriteTargetDirectory=Yes ^
        install ^
        com.mycompany.rockpaperscissors
) else (
    echo [Debug] All required files are present. Starting UpdaterLauncher.exe.
    call "%FILE_PATH%"
)

:end_script
echo [Debug] Script finished.
echo Press any key to continue...
pause >nul

REM Return to the original directory
cd /d "%ORIGINAL_DIR%"

endlocal
