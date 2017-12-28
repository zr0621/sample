INCLUDE_DIRECTORIES(${TOP_DIR}/source/ddc/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/thread/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/log/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/thirdparty/hisisdk/${PLATFORM}/include)

PROJECT(DDCLIB)

SET(DDC_SRC_LIST ./src/ddc.c
				./src/datacenter.c
				./src/datapool.c
				./src/mem.c)

#MESSAGE(STATUS "This is BINARY dir " ${DDC_LIBRARY_DIR})
MESSAGE(STATUS "This is ddc source list " ${DDC_SRC_LIST})

ADD_LIBRARY(ddc STATIC ${DDC_SRC_LIST})

#install libddc.a
INSTALL(TARGETS ddc
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install ddc header files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/ddc/inc/*.h
	${TOP_DIR}/source/ddc/inc/ddc.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
