PROJECT( aruspix )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET(CMAKE_OSX_ARCHITECTURES i386 CACHE STRING "change to needed architecture for a smaller library" FORCE)

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}" )

OPTION(torchDir "/usr")
FIND_LIBRARY(TORCH_LIBRARY torch "${torchDir}/lib")

SET( decoderSources ${decoderSources}
   
    ../src/torch/Decoder.cpp
    ../src/ml/mldecoderpage.cpp
    ../src/ml/mldecoderstaff.cpp
)

INCLUDE_DIRECTORIES(
    
    ../src/torch/
    ../src/ml
    
    ${torchDir}/include
)

ADD_DEFINITIONS(
	-g
	-D_FILE_OFFSET_BITS=64 	
	-D_LARGE_FILES 	
	-D__WXGTK__
	-pthread
	-Wno-write-strings
	-Wno-format-security
    -Wno-non-pod-varargs
	)

ADD_EXECUTABLE( decoder-cmdline ${decoderSources} )

TARGET_LINK_LIBRARIES( decoder-cmdline
	-lz
	-lpthread
    -liconv
    ${TORCH_LIBRARY}
)

install (TARGETS decoder-cmdline DESTINATION bin)