#!/bin/bash -vx



TARGET_ARG="$1"

if [ "$TARGET_ARG" == "" ]; then
  echo "No target specified, defaulting to armv7"
  export APPLE_IOS="armv7"
elif [ $TARGET_ARG == "armv7" ] || [ $TARGET_ARG == "armv7s" ] || [ $TARGET_ARG == "arm64" ] || [ $TARGET_ARG == "i386" ] || [ $TARGET_ARG == "x86_64" ]; then
  export APPLE_IOS=$TARGET_ARG
else
  echo "Non-existing target specified. Exiting..."
  exit 1
fi

export NABTO_ARCH=arm

# SDK version
IOS_SDK_VER=7.1

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

export UDT_ARCH=$APPLE_IOS


[ x"$INSTALL_ROOT" = x"" ]  && { export INSTALL_ROOT=`pwd`/../../install ; } 

INSTALL_ROOT_IOS=$INSTALL_ROOT/ios
export NABTO_ARCH=arm
source env-iphone.sh
ls  | grep -v sh | xargs rm -Rf
PROJECT_ROOT=`pwd`/../../osn_tunnel_client/
INSTALL_ROOT=`pwd`/install/ios
export TARGET=ios
env > env.txt
cmake -DNABTO_BOOST_VERSION=1_54 -DCMAKE_SYSTEM_NAME="ios"  -DCMAKE_INSTALL_PREFIX=${INSTALL_ROOT_IOS} ${PROJECT_ROOT}
#make -j3 nabto_client_api_static
#cd nabto_client_api
make VERBOSE=1 install
