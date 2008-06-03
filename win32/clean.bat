@echo off
move ..\bin\%1\*.exe ..\bin
del /Q /S ..\bin\%1
move ..\bin\*.exe ..\bin\%1
echo output dir cleaned


