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
# ADD CPP /nologo /MDd /W3 /GX /Zd /Od /I ".\..\src\\" /I "$(WXWIN)\include" /I "$(WXWIN)\include\msvc" /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(IMLIB)\include" /I "$(TINYXML)" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "__AXDEBUG__" /D "AX_DISPLAY" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "AX_RECOGNITION" /D "AX_WG" /D "AX_WGVIEWER" /D "AX_SUPERIMPOSITION" /FR"../obj/AruspixDebug/" /Fp"../obj/AruspixDebug/Aruspix.pch" /YX /Fo"../obj/AruspixDebug/" /Fd"../obj/AruspixDebug/" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "../include" /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw$(WXWIN_VERSION)d_core.lib wxbase$(WXWIN_VERSION)d.lib wxmsw$(WXWIN_VERSION)d_html.lib wxpngd.lib wxzlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib im.lib im_process.lib im_fftw.lib tinyxmld.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /out:"../bin/Release/Aruspix.exe" /libpath:"$(IMLIB)\lib" /libpath:"$(TINYXML)\Debug" /libpath:"$(WXWIN)\lib\vc_lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=clean.bat Release
PostBuild_Cmds=copy.bat Release
# End Special Build Tool

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
# ADD CPP /nologo /MDd /W3 /GX /Zd /Od /I ".\..\src\\" /I "$(WXWIN)\include" /I "$(WXWIN)\include\msvc" /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(IMLIB)\include" /I "$(TINYXML)" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "__AXDEBUG__" /D "AX_DISPLAY" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "AX_RECOGNITION" /D "AX_WG" /D "AX_WGVIEWER" /D "AX_SUPERIMPOSITION" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "../include" /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw$(WXWIN_VERSION)d_core.lib wxbase$(WXWIN_VERSION)d.lib wxmsw$(WXWIN_VERSION)d_html.lib wxpngd.lib wxzlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib im.lib im_process.lib im_fftw.lib tinyxmld.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /out:"../bin/Debug/Aruspix.exe" /libpath:"$(IMLIB)\lib" /libpath:"$(TINYXML)\Debug" /libpath:"$(WXWIN)\lib\vc_lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=clean.bat Debug
PostBuild_Cmds=copy.bat Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Aruspix - Win32 Release"
# Name "Aruspix - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\src\app\axabout_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axapp.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axapp_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axctrl.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axenv.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axfile.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axframe.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axgotodlg.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\aximage.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\aximagecontroller.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\aximcontrol_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axoptions_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axoptionsdlg.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axprocess.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axprogressdlg.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axscrolledwindow.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\app\axundo.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmp.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmp_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpctrl.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpfile.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpim.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpmlf.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpmus.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\imbrink.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\imbrink3classes.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imext.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\imkmeans.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imoperator.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\impage.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\imregister.cpp
# End Source File
# Begin Source File

SOURCE=..\src\im\imstaff.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\im\imstaffsegment.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\mus.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\mus_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musbarlines.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musbezier.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musclef.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\muselement.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musfile.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musgraph.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musiomlf.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musiowwg.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musmlfdic.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mus\musnote.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musobject.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\muspage.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\musstaff.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\mussymbol.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\mustoolpanel.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mus\muswindow.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recbookctrl.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recbookfile.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recfile.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recim.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmlfbmp.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmodels.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmus.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\superimposition\supbookctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\supbookfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\supfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\supim.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer_wdr.cpp

!IF  "$(CFG)" == "Aruspix - Win32 Release"

# ADD CPP /I "D:\libs\wx2.8.7\lib\vc_lib\msw" /D "AX_COMPARISON"
# SUBTRACT CPP /D "AX_WG" /D "AX_DISPLAY"

!ELSEIF  "$(CFG)" == "Aruspix - Win32 Debug"

# ADD CPP /I ".\.." /I ".\..\src" /D "AX_COMPARISON"
# SUBTRACT CPP /I ".\..\src\\" /D "AX_DISPLAY" /D "AX_WG"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\src\app\axabout_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axapp.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axapp_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axctrl.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axdefs.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axenv.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axfile.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axframe.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axgotodlg.h
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

SOURCE=..\src\app\axoptions_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axoptionsdlg.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axprocess.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axprogressdlg.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axscrolledwindow.h
# End Source File
# Begin Source File

SOURCE=..\src\app\axundo.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmp.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmp_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpctrl.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpfile.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpim.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpmlf.h
# End Source File
# Begin Source File

SOURCE=..\src\comparison\cmpmus.h
# End Source File
# Begin Source File

SOURCE=..\src\im\imext.h
# End Source File
# Begin Source File

SOURCE=..\src\im\imkmeans.h
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

SOURCE=..\src\mus\mus.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\mus_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musdef.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\muselement.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musfile.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musiomlf.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musiowwg.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musnote.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musobject.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\muspage.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\musstaff.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\mussymbol.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\mustoolpanel.h
# End Source File
# Begin Source File

SOURCE=..\src\mus\muswindow.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\rec_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recbookctrl.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recbookfile.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recfile.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recim.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmlfbmp.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmodels.h
# End Source File
# Begin Source File

SOURCE=..\src\recognition\recmus.h
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup.h
# End Source File
# Begin Source File

SOURCE=..\src\superimposition\sup_wdr.h
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer.h
# End Source File
# Begin Source File

SOURCE=..\src\wgviewer\wgviewer_wdr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\..\app\res\ax.ico
# End Source File
# Begin Source File

SOURCE=..\src\app\res\ax.ico
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
