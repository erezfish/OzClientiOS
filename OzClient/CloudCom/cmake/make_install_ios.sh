#!/bin/bash -vx

PROJECT_ROOT=`pwd`/../../osn_tunnel_client/
#source $PROJECT_ROOT/cmake/env-iphone.sh


TARGET_ARG="$1"
#TARGET_ARG="armv7"

if [ x"$TARGET_ARG" == x"" ]; then
  echo "No target specified, defaulting to armv7"
  export APPLE_IOS="armv7"
elif [ x"$TARGET_ARG" == x"armv7" ] || [ x"$TARGET_ARG" == x"armv7s" ] || [ x"$TARGET_ARG" == x"arm64" ] || [ x"$TARGET_ARG" == x"i386" ] || [ x"$TARGET_ARG" == x"x86_64" ]; then
  export APPLE_IOS=$TARGET_ARG
else
  echo "Non-existing target specified. Exiting..."
  exit 1
fi

export NABTO_ARCH=arm

# SDK version
#AMIR IOS_SDK_VER=8.1
#Atzmon IOS_SDK_VER=8.2
IOS_SDK_VER=12.1

if [ "$APPLE_IOS" == "" ]; then
    echo "Error: No iOS architecture set! Please set the APPLE_IOS environment variable to one of:"
    echo "       armv7 armv7s arm64 i386 x86_64"
    exit 1
fi

if [[ "$APPLE_IOS" == "i386" || "$APPLE_IOS" == "x86_64" ]]; then
    export IOS_SDK_ID="iphonesimulator$IOS_SDK_VER"
else
    export IOS_SDK_ID="iphoneos$IOS_SDK_VER"
fi

# Force libudt core.cpp to use small UDP buffer
export NABTO_UNIX_TARGET="ios"
#export NABTO_UNIX_TARGET="Generic"

export UDT_ARCH=$APPLE_IOS

 
export IOS_SYS_ROOT=`xcrun --sdk $IOS_SDK_ID --show-sdk-path`
#export IOS_SYS_ROOT=`xcrun --sdk iphoneos --find texturetool`

export NABTO_FLAGS="-fPIC -arch $UDT_ARCH -miphoneos-version-min=$IOS_SDK_VER -DNABTO_IOS -DDarwin -isysroot $IOS_SYS_ROOT"

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


[ x"$INSTALL_ROOT" = x"" ]  && { export INSTALL_ROOT=`pwd`/../ios_output ; } 

INSTALL_ROOT_IOS=$INSTALL_ROOT/$UDT_ARCH
export NABTO_ARCH=arm
ls  | grep -v sh | xargs rm -Rf
INSTALL_ROOT=`pwd`/install/ios
export TARGET=ios
env > env.txt
cmake -DNABTO_BOOST_VERSION=1_54 -DCMAKE_SYSTEM_NAME=${NABTO_UNIX_TARGET}  -DCMAKE_INSTALL_PREFIX=${INSTALL_ROOT_IOS} ${PROJECT_ROOT}
#make -j3 nabto_client_api_static
#cd nabto_client_api
make VERBOSE=1 install
