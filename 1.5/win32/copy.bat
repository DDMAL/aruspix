@echo off
xcopy /Q /S /Y ..\local ..\bin\%1\
xcopy /Q /S /Y ..\data ..\bin\%1\
echo local and data files copied


