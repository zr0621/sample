INCLUDE_DIRECTORIES(${TOP_DIR}/source/mppive/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/thirdparty/hisisdk/${PLATFORM}/include)

PROJECT(MPPIVELIB)

SET(MPPIVE_SRC_LIST ./src/sample_vio.c
				./src/sample_comm_sys.c
				./src/mppive.c)

MESSAGE(STATUS "This is mppive source list " ${MPPIVE_SRC_LIST})

ADD_DEFINITIONS(-D SENSOR_TYPE)
SET(SENSOR_TYPE CACHE PANASONIC_MN34220_SUBLVDS_1080P_30FPS "sensor type is 8")

ADD_LIBRARY(mppive STATIC ${MPPIVE_SRC_LIST})

#install libmppive.a
INSTALL(TARGETS mppive
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PLATFORM}/)

#install mppive heder files
FILE(GLOB INSTALL_INCLUDE_FILE
	${TOP_DIR}/source/mppive/inc/*.h
#	${TOP_DIR}/source/mppive/inc/sample_comm.h
	)
INSTALL(FILES ${INSTALL_INCLUDE_FILE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/)
