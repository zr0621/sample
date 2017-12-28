INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)

PROJECT(PLATFORMLIB)

SET(PLATFORM_SRC_LIST ./src/platform.c)

MESSAGE(STATUS "This is platform source list " ${PLATFORM_SRC_LIST})

ADD_LIBRARY(platform STATIC ${PLATFORM_SRC_LIST})

#install libplatform.a
INSTALL(TARGETS platform
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install platform heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/platform/inc/*.h
	${TOP_DIR}/source/platform/inc/platform.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)

