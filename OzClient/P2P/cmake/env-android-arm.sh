#  !/bin/bash -vx 


_DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_ROOT="$( cd -P "$_DIR/../../../external" && pwd )"
unset _DIR

PLATFORM=android-9
#return 0
INSTALL_DIR=$EXTERNAL_ROOT/${PLATFORM}

if [ ! -d $EXTERNAL_ROOT/android-ndk-r10b ] || [ ! -d ${INSTALL_DIR} ]; then
    #android from repo-ext
    echo "Unpacking android ndk..."
    tar jxf $EXTERNAL_ROOT/../android/android-ndk32-r10b-linux-x86_64.tar.bz2 -C $EXTERNAL_ROOT || return 1

	NDK=android-ndk-r10b
	NDK_DIR=$EXTERNAL_ROOT/$NDK
	
	echo "Building $NDK toolchain..."
	$NDK_DIR/build/tools/make-standalone-toolchain.sh --platform=$PLATFORM --install-dir=$INSTALL_DIR --system=linux-x86_64 --arch=arm || return 1

    unset NDK
    unset NDK_DIR
fi

export NABTO_UNIX_TARGET=android
export HOST_FLAG=--host=arm-linux
export SYSTEMROOT=${INSTALL_DIR}
export TOOLCHAIN_PREFIX=arm-linux-androideabi-
export CFLAGS="-mthumb"
export CXXFLAGS="${CFLAGS} -DNABTO_STATIC_LIBRARY -DAndroid"
export LDFLAGS="-lgcc"

export UDT_ARCH=MIPS32
export CXX=${TOOLCHAIN_PREFIX}g++
export CC=${TOOLCHAIN_PREFIX}gcc
export LD=${TOOLCHAIN_PREFIX}ld
export PATH=${PATH}:${SYSTEMROOT}/bin

export OPENSSL_TARGET=linux-generic32
export CUSTOM_CURL_FLAGS="--with-random=/dev/urandom"
export USE_STLPORT=1
export CMAKE_C_COMPILER_ENV_VAR=`which cmake`
export CMAKE_C_COMPILER_ENV_VAR=`which cmake`

