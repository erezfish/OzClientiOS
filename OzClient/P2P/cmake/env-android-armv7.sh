_DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_ROOT="$( cd -P "$_DIR/../../../external" && pwd )"
unset _DIR

PLATFORM=android-9

INSTALL_DIR=$EXTERNAL_ROOT/${PLATFORM}

if [ ! -d $EXTERNAL_ROOT/android-ndk-r10b ] || [ ! -d ${INSTALL_DIR} ]; then
    #android from repo-ext
    echo "Unpacking android ndk..."
    tar jxf $EXTERNAL_ROOT/../android/android-ndk32-r10b-linux-x86_64.tar.bz2 -C $EXTERNAL_ROOT || return 1
	TOOLCHAIN=arm-linux-androideabi-4.6

	NDK=android-ndk-r10b
	NDK_DIR=$EXTERNAL_ROOT/$NDK
	
	echo "Building $NDK toolchain..."
	$NDK_DIR/build/tools/make-standalone-toolchain.sh --platform=$PLATFORM --install-dir=$INSTALL_DIR --system=linux-x86_64 --arch=arm || return 1 # --toolchain=$TOOLCHAIN

    unset TOOLCHAIN
    unset NDK
    unset NDK_DIR
fi

export NABTO_UNIX_TARGET=android
export HOST_FLAG=--host=arm-linux
export SYSTEMROOT=${INSTALL_DIR}
export TOOLCHAIN_PREFIX=arm-linux-androideabi-

# The following flags are required to build for armv7-a
# CFLAGS -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
# LDFLAGS -march=armv7a -Wl,--fix-cortex-a8

# the following is recommended
# CFLAGS -mthumb

export CFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb"
export CXXFLAGS="${CFLAGS} -DNABTO_STATIC_LIBRARY -DAndroid"
export LDFLAGS="-march=armv7-a -Wl,--fix-cortex-a8 -lgcc"

export UDT_ARCH=MIPS32
export CXX=${TOOLCHAIN_PREFIX}g++
export CC=${TOOLCHAIN_PREFIX}gcc
export LD=${TOOLCHAIN_PREFIX}ld
export PATH=${PATH}:${SYSTEMROOT}/bin

export OPENSSL_TARGET=linux-generic32
export CUSTOM_CURL_FLAGS="--with-random=/dev/urandom"
export USE_STLPORT=1
