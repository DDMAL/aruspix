PROJECT( aruspix )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

FIND_PACKAGE(wxWidgets COMPONENTS base core net html adv aui )
FIND_PACKAGE( LibXml2 )

SET( aruspixSources ${aruspixSources}
	app/axabout_wdr.cpp
	app/axapp.cpp
	app/axapp_wdr.cpp
	app/axctrl.cpp
	app/axenv.cpp
	app/axfile.cpp
	app/axframe.cpp
	app/axgotodlg.cpp
	app/aximagecontroller.cpp
	app/aximage.cpp
	app/aximcontrol_wdr.cpp
	app/axoptionsdlg.cpp
	app/axoptions_wdr.cpp
	app/axprocess.cpp
	app/axprogressdlg.cpp
	app/axscrolledwindow.cpp
	app/axundo.cpp
	app/wxprec.cpp

	comparison/cmp.cpp
	comparison/cmpctrl.cpp
	comparison/cmpfile.cpp
	comparison/cmpim.cpp
	comparison/cmpmlf.cpp
	comparison/cmpmus.cpp
	comparison/cmp_wdr.cpp

	edition/edt.cpp
	edition/edtfile.cpp
	edition/edt_wdr.cpp

	im/imbrink3classes.cpp
	im/imbrink.cpp
	im/imext.cpp
	im/imkmeans.cpp
	im/imoperator.cpp
	im/impage.cpp
	im/imregister.cpp
	im/imstaff.cpp
	im/imstaffsegment.cpp

	ml/mldecoderpage.cpp
	ml/mldecoderstaff.cpp
	ml/mlhmm.cpp
	ml/mlngrambuilder.cpp
	ml/mlngramentry.cpp

	mus/musbarlines.cpp
	mus/musbeam.cpp
	mus/musbezier.cpp
	mus/musclef.cpp
	mus/mus.cpp
	mus/muselement.cpp
	mus/musfile.cpp
	mus/musgraph.cpp
	mus/musiobin.cpp
	mus/musiocmme.cpp
	mus/musiomei.cpp
	mus/musiomlf.cpp
	mus/musiowwg.cpp
	mus/musioxml.cpp
	mus/muskeyboardeditor.cpp
	mus/musligature.cpp
	mus/musmlfdic.cpp
	mus/musneume.cpp
	mus/musnote.cpp
	mus/musobject.cpp
	mus/muspage.cpp
	mus/musstaff.cpp
	mus/mussvg.cpp
	mus/mussymbol.cpp
	mus/mustoolpanel.cpp
	mus/muswindow.cpp

	recognition/recbookctrl.cpp
	recognition/recbookfile.cpp
	recognition/rec.cpp
	recognition/recfile.cpp
	recognition/recim.cpp
	recognition/recmlfbmp.cpp
	recognition/recmodels.cpp
	recognition/recmus.cpp
	recognition/rec_wdr.cpp

	rtmidi/RtMidi.cpp

	superimposition/supbookctrl.cpp
	superimposition/supbookfile.cpp
	superimposition/sup.cpp
	superimposition/supfile.cpp
	superimposition/supim.cpp
	superimposition/sup_wdr.cpp

	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)

INCLUDE_DIRECTORIES(
	.
	app
	comparison
	recognition
	rtmidi
	edition
	superimposition
	tinyxml
	im
	ml
	mus

	/usr/include/im
	${LIBXML2_INCLUDE_DIR}
	/usr/include/torch
)

INCLUDE(${wxWidgets_USE_FILE})

ADD_DEFINITIONS( -DAX_SUPERIMPOSITION -DAX_COMPARISON -DAX_EDT -DWX_PRECOMP -DAX_RECOGNITION ${LIBXML2_DEFINITIONS} -D__LINUX_ALSASEQ__)

ADD_EXECUTABLE( aruspix ${aruspixSources} )

TARGET_LINK_LIBRARIES( aruspix
	-lz
	-lpthread
	-lasound
	-lim
	-lim_process
	-lim_fftw
	-lxml2
	-ltorch
	${wxWidgets_LIBRARIES}
	${LIBXML2_LIBRARIES}
)
