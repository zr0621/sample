INCLUDE_DIRECTORIES(${TOP_DIR}/source/log/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)

PROJECT(LOGLIB)

SET(LOG_SRC_LIST ./src/log.c)

#MESSAGE(STATUS "This is BINARY dir " ${DSA_LIBRARY_DIR})
MESSAGE(STATUS "This is log source list " ${LOG_SRC_LIST})

ADD_LIBRARY(log STATIC ${LOG_SRC_LIST})

#install liblog.a
INSTALL(TARGETS log
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install log heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/log/inc/*.h
	${TOP_DIR}/source/log/inc/log.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
