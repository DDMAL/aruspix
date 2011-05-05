# Find libmei
# Find the libmei includes and the libmei libraries
# This module defines
# LIBMEI_INCLUDE_DIR, root mei include dir
# LIBMEI_LIBRARY, the path to libmei
# LIBMEI_FOUND, whether libmei was found


find_path( LIBMEI_INCLUDE_DIR NAMES mei.h
   HINTS
   /opt/local/include
   /usr/local/include
   /usr/include
   PATH_SUFFIXES mei
)

find_library( LIBMEI_LIBRARY NAMES mei
   PATHS
   /opt/local/lib
   /usr/local/lib
   /usr/lib
   /usr/lib64
)

if(LIBMEI_INCLUDE_DIR AND LIBMEI_LIBRARY)
   set(LIBMEI_FOUND TRUE)
   message(STATUS "Found libmei: ${LIBMEI_INCLUDE_DIR}, ${LIBMEI_LIBRARY}")
else(LIBMEI_INCLUDE_DIR AND LIBMEI_LIBRARY)
   set(LIBMEI_FOUND FALSE)   
   if (LIBMEI_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find required package libmei")
   endif(LIBMEI_FIND_REQUIRED)
endif(LIBMEI_INCLUDE_DIR AND LIBMEI_LIBRARY)

mark_as_advanced(LIBMEI_INCLUDE_DIR LIBMEI_LIBRARY)
