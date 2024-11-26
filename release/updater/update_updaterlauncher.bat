@echo off

REM 현재 디렉토리에서 작업 (UpdaterLauncher_new.exe와 UpdaterLauncher.exe가 동일 디렉터리에 있다고 가정)
set CURRENT_DIR=%~dp0

REM 프로세스 종료
echo Terminating UpdaterLauncher process if running...
taskkill /IM UpdaterLauncher.exe /F >nul 2>&1

REM 대기 시간 (2초)
timeout /t 2 >nul

REM 기존 UpdaterLauncher.exe 삭제
if exist "%CURRENT_DIR%UpdaterLauncher.exe" (
    echo Deleting old UpdaterLauncher.exe...
    del "%CURRENT_DIR%UpdaterLauncher.exe"
)

REM 새로운 UpdaterLauncher_new.exe를 UpdaterLauncher.exe로 이름 변경
if exist "%CURRENT_DIR%UpdaterLauncher_new.exe" (
    echo Renaming UpdaterLauncher_new.exe to UpdaterLauncher.exe...
    ren "%CURRENT_DIR%UpdaterLauncher_new.exe" UpdaterLauncher.exe
) else (
    echo UpdaterLauncher_new.exe not found. Exiting...
    exit /b 1
)

REM 대기 시간 (2초) 후 새 UpdaterLauncher.exe 실행
echo Starting new UpdaterLauncher.exe...
timeout /t 2 >nul

REM 새 UpdaterLauncher.exe 실행 (명령어 창 닫지 않음)
start /d "%CURRENT_DIR%" UpdaterLauncher.exe

REM 완료 메시지
echo Update complete. Exiting batch file.
exit /b 0
