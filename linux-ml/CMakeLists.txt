PROJECT( decoder )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" )

# FIND_PACKAGE( LibTorch REQUIRED )

OPTION(torchDir "/usr")
FIND_LIBRARY(TORCH_LIBRARY torch "${torchDir}/lib")

ADD_DEFINITIONS( -Wno-write-strings )

INCLUDE_DIRECTORIES( ${torchDir}/include
	../src/ml)
set(LIBS ${LIBS} ${TORCH_LIBRARY})

ADD_EXECUTABLE( decoder ../src/torch/Decoder.cpp
       ../src/ml/mldecoderpage.cpp
       ../src/ml/mldecoderstaff.cpp )
TARGET_LINK_LIBRARIES( decoder ${LIBS} )

PROJECT( ngram )
ADD_EXECUTABLE( ngram ../src/torch/NGram.cpp
       ../src/ml/mlngrambuilder.cpp
       ../src/ml/mlngramentry.cpp )
TARGET_LINK_LIBRARIES( ngram ${LIBS} )

PROJECT( init )
ADD_EXECUTABLE( init ../src/torch/Init.cpp
	../src/torch/Common.cpp )
TARGET_LINK_LIBRARIES( init ${LIBS} )

PROJECT( trainer )
ADD_EXECUTABLE( trainer ../src/torch/Trainer.cpp
	../src/torch/Common.cpp )
TARGET_LINK_LIBRARIES( trainer ${LIBS} )

PROJECT( adapt )
ADD_EXECUTABLE( adapt ../src/torch/Adapter.cpp
       ../src/ml/mlhmm.cpp )
TARGET_LINK_LIBRARIES( adapt ${LIBS} )

PROJECT( eval )
ADD_EXECUTABLE( eval ../src/torch/Eval.cpp
       ../src/ml/mldecoderpage.cpp
       ../src/ml/mldecoderstaff.cpp )
TARGET_LINK_LIBRARIES( eval ${LIBS} )

