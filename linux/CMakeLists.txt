PROJECT( aruspix )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}" )

FIND_PACKAGE(wxWidgets COMPONENTS base core net html adv aui )
FIND_PACKAGE( LibXml2 )
FIND_PACKAGE( LibMei )

option(AX_BUILD_TESTS "Build all unit tests" ON)

if(AX_BUILD_TESTS)
      enable_testing()
endif(AX_BUILD_TESTS)

SET( aruspixSources ${aruspixSources}
	../src/app/axabout_wdr.cpp
	../src/app/axapp.cpp
	../src/app/axapp_wdr.cpp
	../src/app/axctrl.cpp
	../src/app/axdc.cpp
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
	../src/app/axwxdc.cpp
	../src/app/wxprec.cpp

	../src/comparison/cmp.cpp
	../src/comparison/cmpctrl.cpp
	../src/comparison/cmpfile.cpp
	../src/comparison/cmpim.cpp
	../src/comparison/cmpmlf.cpp
	../src/comparison/cmpmus.cpp
	../src/comparison/cmp_wdr.cpp

	../src/edition/edt.cpp
	../src/edition/edtfile.cpp
	../src/edition/edt_wdr.cpp

	../src/im/imbrink3classes.cpp
	../src/im/imbrink.cpp
	../src/im/imext.cpp
	../src/im/imkmeans.cpp
	../src/im/imoperator.cpp
	../src/im/impage.cpp
	../src/im/imregister.cpp
	../src/im/imstaff.cpp
	../src/im/imstaffsegment.cpp

	../src/ml/mldecoderpage.cpp
	../src/ml/mldecoderstaff.cpp
	../src/ml/mlhmm.cpp
	../src/ml/mlngrambuilder.cpp
	../src/ml/mlngramentry.cpp

	../src/mus/musbarlines.cpp
	../src/mus/musbeam.cpp
	../src/mus/musbezier.cpp
	../src/mus/musclef.cpp
	../src/mus/mus.cpp
	../src/mus/muselement.cpp
	../src/mus/musfile.cpp
	../src/mus/musgraph.cpp
	../src/mus/musiobin.cpp
	../src/mus/musiocmme.cpp
    #../src/mus/musiomei.cpp
	../src/mus/musiomei2.cpp
	../src/mus/musiomlf.cpp
	../src/mus/musiowwg.cpp
	../src/mus/musioxml.cpp
	../src/mus/musmlfdic.cpp
	../src/mus/musneume.cpp
	../src/mus/musneumesymbol.cpp
	../src/mus/musnote.cpp
	../src/mus/musobject.cpp
	../src/mus/muspage.cpp
	../src/mus/musrc.cpp
	../src/mus/musstaff.cpp
	../src/mus/mussvgdc.cpp
	../src/mus/mussymbol.cpp
	../src/mus/mustoolpanel.cpp
	../src/mus/muswindow.cpp

	../src/recognition/recbookctrl.cpp
	../src/recognition/recbookfile.cpp
	../src/recognition/rec.cpp
	../src/recognition/recfile.cpp
	../src/recognition/recim.cpp
	../src/recognition/recmlfbmp.cpp
	../src/recognition/recmodels.cpp
	../src/recognition/recmus.cpp
	../src/recognition/rec_wdr.cpp

	../src/rtmidi/RtMidi.cpp

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

	/usr/include/im
	${LIBXML2_INCLUDE_DIR}
    ${LIBMEI_INCLUDE_DIR}
	/usr/include/torch
)

INCLUDE(${wxWidgets_USE_FILE})

ADD_DEFINITIONS(
	-DAX_SUPERIMPOSITION 
	-DAX_COMPARISON
	-DAX_EDT
	-DWX_PRECOMP
	-DAX_RECOGNITION
	-D__AXDEBUG__
	${LIBXML2_DEFINITIONS}
	-D__LINUX_ALSASEQ__
	-g
	-Wno-write-strings
	-Wno-format-security
	-Wextra
	)

ADD_EXECUTABLE( aruspix ${aruspixSources} )

TARGET_LINK_LIBRARIES( aruspix
	-lz
	-lpthread
	-lasound
	-lim
	-lim_process
	-lim_fftw
	-ltorch
	${wxWidgets_LIBRARIES}
	${LIBXML2_LIBRARIES}
    ${LIBMEI_LIBRARY}
)

if( AX_BUILD_TESTS )
    set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
    include_directories( ${CMAKE_CURRENT_BINARY_DIR})

    find_package( GoogleTest )

    macro(add_ax_test _source)
        get_filename_component(_name ${_source} NAME_WE)
        add_executable(${_name} ${ARGV} ../test/gtest_main.cc)
        target_link_libraries(${_name} 
            ${GTEST_LIBRARIES}
            pthread
        	${wxWidgets_LIBRARIES}
        	${LIBXML2_LIBRARIES}
            ${LIBMEI_LIBRARY}
            )
        # tests depend on the latest version of the library
        add_test(${_name}-test ${EXECUTABLE_OUTPUT_PATH}/${_name})
    endmacro(add_ax_test)

    add_ax_test ( ../test/mus/musneume_test.cpp
                  ../src/mus/musobject.cpp
                  ../src/mus/muselement.cpp
                  ../src/mus/musneume.cpp
                  ../test/fakemusrc.cpp )

#    add_ax_test ( ../test/mus/musiomei2_test.cpp
#                  ../src/mus/musiomei2.cpp
#                  ../src/mus/musfile.cpp
#                  ../src/mus/muspage.cpp
#                  ../src/mus/mussymbol.cpp
#                  ../src/mus/musstaff.cpp )

endif( AX_BUILD_TESTS )
