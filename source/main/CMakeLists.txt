PROJECT(main)

ADD_DEFINITIONS(-std=c++0x)

INCLUDE_DIRECTORIES(${TOP_DIR}/source/main/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/log/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/dsa/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/utils/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/platform/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/thread/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/cfgfile/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/socket/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/ddc/inc)
INCLUDE_DIRECTORIES(${TOP_DIR}/source/mppive/inc)

LINK_DIRECTORIES(${TOP_DIR}/install/lib/${CROSS_COMPILE})

INCLUDE_DIRECTORIES(${TOP_DIR}/thirdparty/hisisdk/${PLATFORM}/include)
LINK_DIRECTORIES(${TOP_DIR}/thirdparty/hisisdk/${PLATFORM}/lib)
FILE(GLOB HISI_LIBS ${TOP_DIR}/thirdparty/hisisdk/${PLATFORM}/lib/*.a)

SET(MODULE_LIBRARY
				socket
				ddc
				mppive
				thread
				cfgfile
				dsa
				utils
				log
				platform
	)
	
SET(THIRD_PARTY_LIB
				${HISI_LIBS}
				${HISI_LIBS}
	)
#上面之所以链接两遍，是因为只链接一遍会出错，这里面的库应该有互相依赖的关系
#FILE拿到的文件列表不会控制链接顺序，链接顺序是由GCC控制的
				
SET(MAIN_SRC_LIST 
				./src/main.cpp)

MESSAGE(STATUS "This is main source list " ${MAIN_SRC_LIST})

ADD_EXECUTABLE(sample.bin 
				${MAIN_SRC_LIST})
					

TARGET_LINK_LIBRARIES(
					sample.bin  
					${MODULE_LIBRARY}
					${THIRD_PARTY_LIB}
					-Wl,--whole-archive
					pthread
					dl
					-Wl,--no-whole-archive
					m
					rt
					stdc++
					-static
					)
					
SET_TARGET_PROPERTIES(sample.bin PROPERTIES LINK_DEPENDS "${MODULE_LIBRARY}")
SET_TARGET_PROPERTIES(sample.bin PROPERTIES LINK_DEPENDS "${THIRD_PARTY_LIB}")

INSTALL(TARGETS sample.bin
	RUNTIME DESTINATION bin)
INSTALL(DIRECTORY ${TOP_DIR}/config/
		DESTINATION cfg)
