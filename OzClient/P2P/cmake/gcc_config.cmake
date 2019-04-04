
# GCC specific stuff

if(CMAKE_COMPILER_IS_GNUCC)

  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION)
  
  # force gcc to use static version of libstdc++ as described on http://www.trilithium.com/johan/2005/06/static-libstdc/.
  if (NOT GCC_VERSION VERSION_LESS 4.5) 
    SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libstdc++ -static-libgcc")
  else()
    # old way to include static c++ runtime
    # does not work on mac
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      SET(staticlinkdir ${CMAKE_BINARY_DIR}/libstdcpp)
      execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.a
	OUTPUT_VARIABLE GCC_STDCPP OUTPUT_STRIP_TRAILING_WHITESPACE)
      FILE(MAKE_DIRECTORY ${staticlinkdir})
      ADD_CUSTOM_TARGET(staticlibcpp ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${GCC_STDCPP} ${staticlinkdir}/libstdc++.a)
      # link to static stdc++ 
      #      if(NOT NABTO_TARGET MATCHES android)
      SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc -L${staticlinkdir}")
      SET(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -static-libgcc -L${staticlinkdir}")
      
      if (${TARGET} MATCHES "android")
	LINK_DIRECTORIES(${staticlinkdir})
	
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Bstatic -lstdc++")
      endif()
      #endif()
      #      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Bstatic -lstdc++")
      #      SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Bstatic -lstdc++")

    endif()
  endif()
endif(CMAKE_COMPILER_IS_GNUCC)
