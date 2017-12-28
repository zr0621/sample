INCLUDE_DIRECTORIES(${TOP_DIR}/source/dsa/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)

PROJECT(DSALIB)

SET(DSA_SRC_LIST
				./src/dsa.c
				./src/vector.c
				./src/dlist.c
				./src/queue.c)

MESSAGE(STATUS "This is dsa source list " ${DSA_SRC_LIST})

ADD_LIBRARY(dsa STATIC ${DSA_SRC_LIST})

#install libdsa.a
INSTALL(TARGETS dsa
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install dsa heder files
FILE(GLOB INSTALL_INCLUDE_FILE
	${TOP_DIR}/source/dsa/inc/*.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
