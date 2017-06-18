cmd /C "android update project --path ./"
cls
cmd /C "ndk-build"
cmd /C "ant debug"

del /F /S /Q apk\* >nul 2>nul
move /Y bin\*debug.apk apk >nul 2>nul
rd /S /Q bin >nul 2>nul
md bin >nul 2>nul

adb install -rg apk/LXengine-debug.apk