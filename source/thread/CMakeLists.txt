INCLUDE_DIRECTORIES(${TOP_DIR}/source/thread/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)

PROJECT(THREADLIB)

SET(THREAD_SRC_LIST ./src/thread.c
					./src/timer.c)

MESSAGE(STATUS "This is thread source list " ${THREAD_SRC_LIST})

ADD_LIBRARY(thread STATIC ${THREAD_SRC_LIST})

#install libthread.a
INSTALL(TARGETS thread
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install thread heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/thread/inc/*.h
	${TOP_DIR}/source/thread/inc/thread.h
	${TOP_DIR}/source/thread/inc/timer.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)

