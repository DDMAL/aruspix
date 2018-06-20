PROJECT( aruspix )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}" )

OPTION(wxDir "")
SET(wxWidgets_CONFIG_EXECUTABLE "${wxDir}wx-config") 
FIND_PACKAGE(wxWidgets COMPONENTS base core net html adv aui )

FIND_PACKAGE( LibXml2 )

OPTION(imDir "/usr")
FIND_LIBRARY(IM_LIBRARY libim.a "${imDir}/lib")
FIND_LIBRARY(IM_PROCESS_LIBRARY libim_process.a "${imDir}/lib")
FIND_LIBRARY(IM_FFTW_LIBRARY libim_fftw.a "${imDir}/lib")


SET( aruspixSources ${aruspixSources}
	../src/app/axabout_wdr.cpp
	../src/app/axapp.cpp
	../src/app/axapp_wdr.cpp
	../src/app/axctrl.cpp
	../src/app/axenv.cpp
	../src/app/axfile.cpp
	../src/app/axframe.cpp
	../src/app/axgotodlg.cpp
	../src/app/aximagecontroller.cpp
	../src/app/aximage.cpp
	../src/app/aximcontrol_wdr.cpp
	../src/app/axoptionsdlg.cpp
	../src/app/axoptions_wdr.cpp
	../src/app/axprocess.cpp
	../src/app/axprogressdlg.cpp
	../src/app/axscrolledwindow.cpp
	../src/app/axundo.cpp
	../src/app/wxprec.cpp

	../src/comparison/cmp.cpp
	../src/comparison/cmpctrl.cpp
	../src/comparison/cmpfile.cpp
	../src/comparison/cmpmus.cpp
	../src/comparison/cmp_wdr.cpp

	../src/edition/edt.cpp
	../src/edition/edtfile.cpp
	../src/edition/edt_wdr_inc.cpp

	../src/im/imbrink3classes.cpp
	../src/im/imbrink.cpp
	../src/im/imext.cpp
	../src/im/imkmeans.cpp
	../src/im/imoperator.cpp
	../src/im/impage.cpp
	../src/im/imregister.cpp
	../src/im/imstaff.cpp
	../src/im/imstaffsegment.cpp

	../src/mus/musapp.cpp
	../src/mus/musbarline.cpp 
	../src/mus/musbboxdc.cpp 
	../src/mus/musbeam.cpp
	../src/mus/musclef.cpp 
	../src/mus/musdc.cpp 
	../src/mus/musdoc.cpp 	
	../src/mus/musdurationinterface.cpp 
	../src/mus/musiobin.cpp
	../src/mus/musio.cpp 
	../src/mus/musiodarms.cpp
	../src/mus/musiomei.cpp
	../src/mus/musiomlf.cpp 
	../src/mus/musiomusxml.cpp
	../src/mus/musiopae.cpp 
	../src/mus/musiowwg.cpp
	../src/mus/muskeysig.cpp
	../src/mus/muslayer.cpp
	../src/mus/muslayerelement.cpp 
	../src/mus/musleipzigbbox.cpp
	../src/mus/musmensur.cpp
	../src/mus/musneume.cpp
	../src/mus/musneumesymbol.cpp 
	../src/mus/musnote.cpp 
	../src/mus/musmlfdic.cpp
	../src/mus/musobject.cpp 
	../src/mus/muspage.cpp 
	../src/mus/muspitchinterface.cpp 
	../src/mus/muspositioninterface.cpp 
	../src/mus/musrc.cpp 
	../src/mus/musrc_beam.cpp  
	../src/mus/musrc_bezier.cpp 
	../src/mus/musrc_element.cpp 
	../src/mus/musrc_graph.cpp 
	../src/mus/musrc_page.cpp
	../src/mus/musrc_neume.cpp
	../src/mus/musrc_tuplet.cpp
	../src/mus/musrest.cpp 
	../src/mus/musslur.cpp
	../src/mus/musstaff.cpp 
	../src/mus/mussvgdc.cpp 
	../src/mus/mussymbol.cpp 
	../src/mus/mussystem.cpp 
	../src/mus/mustie.cpp
	../src/mus/mustoolpanel.cpp
	../src/mus/mustuplet.cpp
	../src/mus/muswindow.cpp
	../src/mus/muswwg.cpp
	../src/mus/muswxdc.cpp

	../src/recognition/recbookctrl.cpp
	../src/recognition/recbookfile.cpp
	../src/recognition/rec.cpp
	../src/recognition/recfile.cpp
	../src/recognition/recim.cpp
	../src/recognition/recmlfbmp.cpp
	../src/recognition/recmodels.cpp
	../src/recognition/recmus.cpp
	../src/recognition/rec_wdr.cpp

	../src/superimposition/supbookctrl.cpp
	../src/superimposition/supbookfile.cpp
	../src/superimposition/sup.cpp
	../src/superimposition/supfile.cpp
	../src/superimposition/supim.cpp
	../src/superimposition/sup_wdr.cpp

	../src/tinyxml/tinystr.cpp
	../src/tinyxml/tinyxml.cpp
	../src/tinyxml/tinyxmlerror.cpp
	../src/tinyxml/tinyxmlparser.cpp

)

INCLUDE_DIRECTORIES(
	../src
	../src/app
	../src/comparison
	../src/recognition
	../src/rtmidi
	../src/edition
	../src/superimposition
	../src/tinyxml
	../src/im
	../src/ml
	../src/mus

	${imDir}/include
	${LIBXML2_INCLUDE_DIR}
	/usr/include/torch
)

INCLUDE(${wxWidgets_USE_FILE})

ADD_DEFINITIONS(
	-DAX_SUPERIMPOSITION 
	-DAX_COMPARISON
	-DAX_EDT
	-DAX_RECOGNITION
	-D__AXDEBUG__
	-g
	-D_FILE_OFFSET_BITS=64 	
	-D_LARGE_FILES 	
	-D__WXGTK__
	-pthread
	${LIBXML2_DEFINITIONS}
	-Wno-write-strings
	-Wno-format-security
	)

ADD_EXECUTABLE( aruspix ${aruspixSources} )

TARGET_LINK_LIBRARIES( aruspix
	-lz
	-lpthread
	-luuid
	${IM_LIBRARY}
	${IM_PROCESS_LIBRARY}
	${IM_FFTW_LIBRARY}
	${wxWidgets_LIBRARIES}
	${LIBXML2_LIBRARIES}
)
