project(OzClientApp)
cmake_minimum_required(VERSION 2.6.1)

set(OSN_ROOT_APP ${CMAKE_CURRENT_SOURCE_DIR})
set(OSN_ROOT ${OSN_ROOT_APP}../OzClient)
set(OSN_INCLUDE ${OSN_ROOT_APP}/../../include)
set(OSN_LIBS ${OSN_ROOT_APP}/../../lib)
#set(SSL_LIBS /usr/local/ssl/lib)

set(src
  ${OSN_ROOT_APP}/OzClientApp.cpp
  ${OSN_ROOT_APP}/../../OzInit/OzInit.c
  ${OSN_ROOT_APP}/../../OzCommon/OZ_memory.c
  ${OSN_ROOT_APP}/../../OzCommon/json/cJSON.c
  ${OSN_ROOT_APP}/../../OzCommon/api/agent_api.cpp
)

include_directories(
  ${OSN_ROOT}
  ${OSN_INCLUDE}
  ${OSN_ROOT_APP}/../../OzCommon
  ${OSN_ROOT_APP}/../../OzCommon/json
  ${OSN_ROOT_APP}/../../OzCommon/api
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  link_directories(
    ${OSN_ROOT}/../../build/${TARGET}/OzClient/Release
    )
elseif(${TARGET} MATCHES android)
  link_directories(
    ${OSN_ROOT_APP}/../build/${TARGET}_output/libs/${ANDROID_ABI}/
    ${OSN_LIBS}/Android/${ANDROID_ABI}
    )

    
else()
  link_directories(
    ${OSN_ROOT_APP}/../build/${TARGET}/OzClient/
	${OSN_LIBS}/Linux
	#${SSL_LIBS}
    )
endif()

add_executable(${PROJECT_NAME} ${src})

# Since we are using a single toplevel project to build both this
# application and the OsnClient library we cant simply link to
# OsnClient since it then thinks we mean the CMake project
# OsnClient and not the OsnClient library artifact from that build
# hence we will write the full library name.

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(lib_name libOzClient.dylib)
elseif(${TARGET} MATCHES "android")
  set(lib_name libOzClient.so libOsnClient.so log)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(lib_name libpthread.so libP2PClient.so libOzClient.so libOsnClient.so)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "ios")
  set(lib_name libOzClient.a)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(lib_name libOzClient.dll)
endif()

add_dependencies(OzClientApp OzClient )

target_link_libraries(${PROJECT_NAME} ${lib_name})

install(TARGETS ${PROJECT_NAME} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
