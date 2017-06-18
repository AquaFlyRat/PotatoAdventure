cmd /C "android update project --path ./"
cls
cmd /C "ndk-build"

cmd /C "ant release"

del /F /S /Q apk\* >nul 2>nul
move /Y bin\*release*.apk apk >nul 2>nul
rd /S /Q bin >nul 2>nul
md bin >nul 2>nul

pause