@echo off
del /Q /S ..\bin\fr
del /Q /S ..\bin\en
xcopy /S /Y ..\local ..\bin
echo local files copied


