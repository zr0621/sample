INCLUDE_DIRECTORIES(${TOP_DIR}/source/cfgfile/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/log/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/dsa/inc)

PROJECT(CFGFILELIB)

SET(CFGFILE_SRC_LIST ./src/cfgfile.c 
                    ./src/cjson_wrap.c 
                    ./src/cjson.c)

MESSAGE(STATUS "This is cfgfile source list " ${CFGFILE_SRC_LIST})

ADD_LIBRARY(cfgfile STATIC ${CFGFILE_SRC_LIST})

#install libcfgfile.a
INSTALL(TARGETS cfgfile
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install cfgfile heder files
FILE(GLOB INSTALL_INCLUDE_FILE
#	${TOP_DIR}/source/cfgfile/inc/*.h
	${TOP_DIR}/source/cfgfile/inc/cfgfile.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
