# linux specific, whitout this flag, openssl cries :(
if (NOT NABTO MATCHES android)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
endif()

add_definitions(-DLinux)
add_definitions(-fPIC)

option(ANCIENT_LINUX_COMPATIBILITY "Support for older linux systems than the building system" ON)

if(ANCIENT_LINUX_COMPATIBILITY)
  # remove dependency on glibc 2.8
  add_definitions(-DBOOST_ASIO_DISABLE_EVENTFD=1)
  # remove dependency on glibc 2.9 but this also removes usage of
  # epoll_create which is in glibc 2.3.2+ 
  add_definitions(-DBOOST_ASIO_DISABLE_EPOLL=1)
  # remove dependency on __isoc99_sscanf from GLIBC 2.7
  add_definitions(-D_GNU_SOURCE)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
