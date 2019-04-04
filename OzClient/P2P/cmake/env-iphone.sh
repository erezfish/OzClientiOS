# SDK version
# AMIR export IOS_SDK_VER=8.1
export IOS_SDK_VER=8.2

# Force libudt core.cpp to use small UDP buffer
export APPLE_IOS="armv7"
# Force libudt makefile to build for armv6
export UDT_ARCH="armv7"
#Revision 6177 of nabto makefile introduced a dependency on this - temp. workaround 
export NABTO_MAC_ENV_OK=true

export IOS_SYS_ROOT=`xcrun --sdk iphoneos$IOS_SDK_VER --show-sdk-path`
export NABTO_FLAGS="-fPIC -arch armv7 -miphoneos-version-min=$IOS_SDK_VER -DNABTO_IOS -DDarwin -isysroot $IOS_SYS_ROOT"

export CFLAGS="$NABTO_FLAGS"
export LDFLAGS="$NABTO_FLAGS"
export CPFLAGS="$NABTO_FLAGS"
export CXXFLAGS="$NABTO_FLAGS"
export CPPFLAGS="$NABTO_FLAGS"

export CC="xcrun clang  $NABTO_FLAGS"
#export CXX="xcrun clang++ $NABTO_FLAGS -std=c++11 -stdlib=libc++"
export CXX="xcrun clang++ $NABTO_FLAGS "
export LD="xcrun ld "
export AR="xcrun ar"
export GCC=$CC
