INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)

PROJECT(UTILSLIB)

SET(UTILS_SRC_LIST ./src/utils.c)

MESSAGE(STATUS "This is utils source list " ${UTILS_SRC_LIST})

ADD_LIBRARY(utils STATIC ${UTILS_SRC_LIST})

#install libutils.a
INSTALL(TARGETS utils
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install utils heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/utils/inc/*.h
	${TOP_DIR}/source/utils/inc/utils.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
