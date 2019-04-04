project(OzClient)
cmake_minimum_required(VERSION 2.6.1)

set(OSN_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(OSN_INCLUDE ${OSN_ROOT}/../../include)
set(OSN_INIT ${OSN_ROOT}/../../OzInit)
set(OSN_COMMON ${OSN_ROOT}/../../OzCommon)
#set(OSN_P2P ${OSN_ROOT}/../../AgentCom)
set(OSN_CLOUD ${OSN_ROOT}/../../CloudCom/OzCloud)
set(OSN_P2P ${OSN_ROOT}/../../P2P/OzClient)
set(OSN_AGENT ${OSN_ROOT}/../../AgentCom/OzAgent)
set(IOS_CURL_INC ${OSN_ROOT}/../../)
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

include_directories(
  ${OSN_INCLUDE}
  ${SSL_INC}
  ${OSN_COMMON}
  ${IOS_CURL_INC}
)

if (${TARGET} MATCHES android)
	set(log_lib  ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/lib )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES android)
	link_directories(
		${OSN_ROOT}/../../lib/Android
		${OSN_ROOT}/../../lib/Android/${ANDROID_ABI}
			)
elseif (${CMAKE_SYSTEM_NAME} MATCHES iOS)
	link_directories(
		${OSN_ROOT}/../../lib/iOS
		${OSN_ROOT}/../../lib/iOS/${ANDROID_ABI}
			)
endif()


if (WIN32)
  list(APPEND link_libs -mt wininet )
elseif (${CMAKE_SYSTEM_NAME} MATCHES iOS)
  list(APPEND link_libs pthread z dl libcurl.a)
elseif (${TARGET} MATCHES android)
  list(APPEND link_libs libssl.so libcrypto.so log)
else()
  #list(APPEND link_libs pthread dl rt libssl.so libcrypto.so)
  list(APPEND link_libs pthread dl libssl.so libcrypto.so)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
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
