message ("Setting up iOS toolchain")



set (CMAKE_SYSTEM_NAME iOS)
# set (IOS_SDK_VER 8.1) AMIR
set (IOS_SDK_VER 10.0)

# for rest of archs
set (IOS_SYS_ROOT /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS12.1.sdk)
SET(TOOLCHAIN_PREFIX /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/)

#SET(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
#SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

# TODO: parametrize sdk ver
#
SET(FLAGS "-O -DNABTO_IOS -DDarwin -fPIC -arch arm64  -miphoneos-version-min=${IOS_SDK_VER} -isysroot ${IOS_SYS_ROOT}" )
SET(COMPILE_FLAGS ${FLAGS})
SET(CC xcrun)

#SET(CMAKE_C_COMPILER "xcrun clang $COMPILE_FLAGS")
SET(CMAKE_C_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang)

SET(CMAKE_EXE_LINKER_FLAGS ${FLAGS})
SET(CMAKE_SHARED_LINKER_FLAGS ${FLAGS})
SET(CMAKE_MODULE_LINKER_FLAGS ${FLAGS})
SET(CMAKE_C_LINK_FLAGS ${FLAGS})
SET(CMAKE_CXX_LINK_FLAGS ${FLAGS})

add_definitions(${FLAGS})

# Skip the platform compiler checks for cross compiling
#set (CMAKE_CXX_COMPILER_WORKS TRUE)
#set (CMAKE_C_COMPILER_WORKS TRUE)

message ("iOS toolchain loaded")
