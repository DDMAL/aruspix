# Microsoft Developer Studio Project File - Name="Aruspix" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Aruspix - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Aruspix.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Aruspix.mak" CFG="Aruspix - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Aruspix - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Aruspix - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../obj/AruspixRelease"
# PROP Intermediate_Dir "../obj/AruspixRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /I ".\..\src" /I "D:\libs\wx2.6.0\lib\vc_lib\msw" /I "D:\libs\im3.0\include" /I "D:\libs\tinyxml" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "AX_RECOGNITION" /D "AX_WG" /D "AX_WGVIEWER" /D "AX_SUPERIMPOSITION" /D "AX_DISPLAY" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "../include" /d "NDEBUG"
# ADD RSC /l 0x409 /i "D:\libs\wx2.6.0\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw26_core.lib wxbase26.lib wxmsw26_html.lib wxtiff.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib im.lib im_process.lib im_fftw.lib tinyxml.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"../bin/Aruspix.exe" /libpath:"D:\libs\im3.0\lib\\" /libpath:"D:\libs\tinyxml\Release"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../obj/AruspixDebug"
# PROP Intermediate_Dir "../obj/AruspixDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I ".\..\src\\" /I "D:\libs\wx2.6.0\lib\vc_lib\mswd" /I "D:\libs\im3.0\include" /I "D:\libs\tinyxml" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "AX_DISPLAY" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "AX_RECOGNITION" /D "AX_WG" /D "AX_WGVIEWER" /D "AX_SUPERIMPOSITION" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "../include" /d "_DEBUG"
# ADD RSC /l 0x409 /i "D:\libs\wx2.4.0\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw26d_core.lib wxbase26d.lib wxmsw26d_html.lib wxtiffd.lib wxpngd.lib wxjpegd.lib wxzlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib im.lib im_process.lib im_fftw.lib tinyxmld.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /out:"../bin/AruspixD.exe" /libpath:"D:\libs\im3.0\lib\dll" /libpath:"D:\libs\tinyxml\Debug"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Aruspix - Win32 Release"
# Name "Aruspix - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\src\app\about_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\app_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axapp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axenv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axframe.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\aximage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\aximagecontroller.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\aximcontrol_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axprocess.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axscrolledwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\axundo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\display\display.cpp
# End Source File
# Begin Source File

SOURCE=..\src\display\display_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\gotodlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\im_ext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imoperator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\impage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imstaff.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imstaffsegment.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\iomlf.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\iomlfclass.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\iowwg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\mlfbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\models.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\options_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\optionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\progressdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recim.cpp
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recwg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_barmes.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_bezier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_cle.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_graph.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgelement.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgnote.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgobject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgpage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgstaff.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgsymbole.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgtoolpanel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer_wdr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\app\wxprec.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\src\app\about_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\app_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axapp.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axenv.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axenvrow.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axfile.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axframe.h
# End Source File
# Begin Source File

SOURCE=..\src\app\aximage.h
# End Source File
# Begin Source File

SOURCE=..\src\app\aximagecontroller.h
# End Source File
# Begin Source File

SOURCE=..\src\app\aximcontrol_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axoperator.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axprocess.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axscrolledwindow.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axundo.h
# End Source File
# Begin Source File

SOURCE=..\src\app\defs.h
# End Source File
# Begin Source File

SOURCE=..\src\display\display.h
# End Source File
# Begin Source File

SOURCE=..\src\display\display_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\gotodlg.h
# End Source File
# Begin Source File

SOURCE=..\src\im\im_ext.h
# End Source File
# Begin Source File

SOURCE=..\src\im\imoperator.h
# End Source File
# Begin Source File

SOURCE=..\src\im\impage.h
# End Source File
# Begin Source File

SOURCE=..\src\im\imstaff.h
# End Source File
# Begin Source File

SOURCE=..\src\im\imstaffsegment.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\iomlf.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\iomlfclass.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\iowwg.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\mlfbmp.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\models.h
# End Source File
# Begin Source File

SOURCE=..\src\app\options_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\optionsdlg.h
# End Source File
# Begin Source File

SOURCE=..\src\app\progressdlg.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recfile.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recim.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recwg.h
# End Source File
# Begin Source File

SOURCE=..\src\app\resource.h
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup.h
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wg_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgdef.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgelement.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgfile.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgnote.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgobject.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgpage.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgstaff.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgsymbole.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgtoolpanel.h
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer.h
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\wg\wgwindow.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\..\app\res\ax.ico
# End Source File
# Begin Source File

SOURCE=..\src\app\ax.rc
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\..\app\wx\msw\watch1.cur
# End Source File
# End Target
# End Project
