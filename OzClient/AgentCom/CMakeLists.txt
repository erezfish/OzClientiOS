project(Oz)
cmake_minimum_required(VERSION 2.6.1)

set(TARGET "iOS" CACHE STRING "Set the target ios mac32/64, linux32/64, android, win32/64")

# Static link the c++ library in case of a gcc compiler.
if (CMAKE_COMPILER_IS_GNUCC)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc_config.cmake)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/darwin.cmake)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux.cmake)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc_config.cmake)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios-arm.cmake)
endif()

if (${TARGET} MATCHES "android")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/android.cmake)
endif()

add_subdirectory(OzAgent)

