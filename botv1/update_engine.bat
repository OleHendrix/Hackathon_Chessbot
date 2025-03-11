@echo off
taskkill /IM cutechess.exe /F  >nul 2>&1
echo Closed Cute Chess.

cd /d "C:\Users\31640\Desktop\hackathong\hackathong\botv1"
echo Navigated to botv1 directory.

make
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)
echo Build successful.

start "" "C:\Users\31640\Desktop\hackathong\cutechess-1.4.0-beta3-win64\cutechess.exe"
echo Restarted Cute Chess.
