project(OzClient)
cmake_minimum_required(VERSION 2.6.2)

set(OSN_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(OSN_INCLUDE ${OSN_ROOT}/../../include)
set(OSN_INIT ${OSN_ROOT}/../../OzInit)
set(OSN_COMMON ${OSN_ROOT}/../../OzCommon)
set(OSN_P2P ${CMAKE_CURRENT_SOURCE_DIR})
set(OSN_AGENT ${OSN_ROOT}/../../AgentCom/OzAgent)
set(OSN_CLOUD ${OSN_ROOT}/../../CloudCom/OzCloud)
set(OSN_LIBS ${OSN_ROOT}/../../lib)
set(ANDROID_CURL_INC ${OSN_ROOT}/../../)
set(LNX_CURL_INC /usr/include/)
set(LNX_CURL_LIBS /usr/lib)
set(SSL_INC /usr/local/ssl/include)
set(LNX_SSL_LIBS /usr/local/ssl/lib)
set(OSNCLIENT_EXPORTS on)


if (${CMAKE_SYSTEM_NAME} MATCHES Windows)
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}
/NODEFAULTLIB:libboost_date_time-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_date_time-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_filesystem-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_filesystem-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_prg_exec_monitor-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_prg_exec_monitor-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_program_options-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_program_options-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_regex-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_regex-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_system-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_system-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_test_exec_monitor-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_test_exec_monitor-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_thread-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_thread-vc100-mt-sgd-1_47.lib
/NODEFAULTLIB:libboost_unit_test_framework-vc100-mt-s-1_47.lib
/NODEFAULTLIB:libboost_unit_test_framework-vc100-mt-sgd-1_47.lib
/VERBOSE:LIB
")
endif()

set(src
  ${OSN_CLOUD}/oz_cloudApi.cpp
  ${OSN_CLOUD}/OSN_GW_agent.c
  ${OSN_AGENT}/client_agent.cpp
  ${OSN_AGENT}/ClientAgentEncoder.c
  ${OSN_P2P}/OzClientAPI.cpp
  ${OSN_P2P}/OzUtils.cpp
  ${OSN_INIT}/OzInit.c
  ${OSN_COMMON}/base64.cpp
  ${OSN_COMMON}/sha256.cpp
  ${OSN_COMMON}/urlcode.cpp
)

if(${TARGET} MATCHES android)
  include_directories(
 	 ${OSN_INCLUDE}
  	 ${OSN_COMMON}
  	 ${ANDROID_CURL_INC}
  	 ${SSL_INC}
    )    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  include_directories(
  	${OSN_INCLUDE}
  	${OSN_COMMON}
 	${LNX_CURL_INC}
  	${SSL_INC}
)
endif()

if (${TARGET} MATCHES android)
	set(log_lib  ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/lib )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  link_directories(
    ${OSN_ROOT}/../../build/${TARGET}/OzClient/Release
    )
elseif(${TARGET} MATCHES android)
  link_directories(
    ${OSN_ROOT}/../build/${TARGET}_output/libs/${ANDROID_ABI}/
    ${OSN_LIBS}/Android/${ANDROID_ABI}
    )    
else()
  link_directories(
    ${OSN_ROOT}/../build/${TARGET}/OzClient/
	${OSN_LIBS}/Linux
	${LNX_SSL_LIBS}
	${LNX_CURL_LIBS}
    )
endif()

if (WIN32)
  list(APPEND link_libs -mt wininet )
elseif (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  list(APPEND link_libs pthread z dl)
elseif (${TARGET} MATCHES android)
  list(APPEND link_libs libOsnClient.so libcurl.so libssl.so libcrypto.so log)
else()
  #list(APPEND link_libs pthread dl rt  libOsnClient.so libcurl.so libssl.so libcrypto.so)
  list(APPEND link_libs pthread dl  libOsnClient.so libcurl.so libssl.so libcrypto.so)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "ios")
   add_library(${PROJECT_NAME} STATIC ${src})
else()
   add_library(${PROJECT_NAME} SHARED ${src})
endif()

target_link_libraries(${PROJECT_NAME} ${link_libs} )

install(TARGETS ${PROJECT_NAME} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
