cmake_minimum_required(VERSION 2.8)

project(Aruspix)

add_definitions(-g -D__WXBASE__ -DHEADLESS -D"wxUSE_GUI=0")

find_package(wxWidgets)

FIND_PACKAGE(wxWidgets REQUIRED base)
INCLUDE(${wxWidgets_USE_FILE})

include_directories(/usr/local/include)

#link_directories(/Users/xhero/devel/ARUSPIX/wxMac-2.8.12/osx-static-debug/lib
#		/Users/xhero/devel/ARUSPIX/libmei-fork/build/Release/)

link_libraries(iconv xml2 mei ${wxWidgets_LIBRARIES})

include_directories(src/mus src/ src/tinyxml)

add_executable (aruspix 
	src/mus/headless_main.cpp 
	src/mus/mustuplet.cpp
	src/mus/musslur.cpp
	src/mus/musleipzigbbox.cpp
	src/mus/musiopae.cpp 
	src/mus/musiodarms.cpp
	src/mus/musbeam.cpp
	src/mus/muskeysig.cpp
	src/mus/musnote.cpp 
	src/mus/musrest.cpp 
	src/mus/musbarline.cpp 
	src/mus/musiowwg.cpp 
	src/mus/musobject.cpp 
	src/mus/mussection.cpp 
	src/mus/musbboxdc.cpp 
	src/mus/muslaidoutlayer.cpp 
	src/mus/muspage.cpp 
	src/mus/musstaff.cpp 
	src/mus/musclef.cpp 
	src/mus/muslaidoutlayerelement.cpp 
	src/mus/muspitchinterface.cpp 
	src/mus/mussvgdc.cpp 
	src/mus/musdc.cpp 
	src/mus/muslaidoutstaff.cpp 
	src/mus/muspositioninterface.cpp 
	src/mus/mussymbol.cpp 
	src/mus/musdiv.cpp 
	src/mus/muslayer.cpp 
	src/mus/musrc.cpp 
	src/mus/mussystem.cpp 
	src/mus/musdoc.cpp 
	src/mus/muslayout.cpp	
	src/mus/musrc_beam.cpp  
	src/mus/musdurationinterface.cpp 
	src/mus/musmeasure.cpp 
	src/mus/musrc_bezier.cpp 
	src/mus/musio.cpp 
	src/mus/musmensur.cpp 
	src/mus/musrc_element.cpp 
	src/mus/muswwg.cpp 
	src/mus/musiobin.cpp 
	src/mus/musmlfdic.cpp 
	src/mus/musrc_graph.cpp 
	src/mus/musiomei.cpp 
	src/mus/musneume.cpp 
	src/mus/musrc_neume.cpp 
	src/mus/musneumesymbol.cpp 
	src/mus/musrc_page.cpp
	src/mus/mustie.cpp
	src/mus/musiomusxml.cpp
	src/mus/musrc_tuplet.cpp
	src/tinyxml/tinystr.cpp
	src/tinyxml/tinyxml.cpp
	src/tinyxml/tinyxmlerror.cpp
	src/tinyxml/tinyxmlparser.cpp
	)
