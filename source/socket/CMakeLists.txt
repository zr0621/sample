INCLUDE_DIRECTORIES(${TOP_DIR}/source/socket/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/cfgfile/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/log/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/dsa/inc)

PROJECT(SOCKETLIB)

SET(SOCKET_SRC_LIST ./src/socket.cpp)

MESSAGE(STATUS "This is socket source list " ${SOCKET_SRC_LIST})

ADD_LIBRARY(socket STATIC ${SOCKET_SRC_LIST})

#install libsocket.a
INSTALL(TARGETS socket
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install socket heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/socket/inc/*.h
	${TOP_DIR}/source/socket/inc/socket.hpp
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)

