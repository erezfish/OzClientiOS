_DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_ROOT="$( cd -P "$_DIR/../../../external" && pwd )"
unset _DIR

PLATFORM=android-9

ANDROID_ARCH=x86

INSTALL_DIR=$EXTERNAL_ROOT/${PLATFORM}-$ANDROID_ARCH

if [ ! -d $EXTERNAL_ROOT/android-ndk-r10b ] || [ ! -d ${INSTALL_DIR} ]; then
    #android from repo-ext
    echo "Unpacking android ndk..."
    tar jxf $EXTERNAL_ROOT/../android/android-ndk32-r10b-linux-x86_64.tar.bz2 -C $EXTERNAL_ROOT || return 1

	NDK=android-ndk-r10b
	NDK_DIR=$EXTERNAL_ROOT/$NDK
	
	echo "Building $NDK toolchain..."
	$NDK_DIR/build/tools/make-standalone-toolchain.sh --platform=$PLATFORM --install-dir=$INSTALL_DIR --system=linux-x86_64 --arch=$ANDROID_ARCH || return 1 
    
    unset NDK
    unset NDK_DIR
fi

export NABTO_UNIX_TARGET=android
export HOST_FLAG=--host=i686-linux
export SYSTEMROOT=${INSTALL_DIR}
export TOOLCHAIN_PREFIX=i686-linux-android-
export CFLAGS="-mssse3"
export CXXFLAGS="${CFLAGS} -DNABTO_STATIC_LIBRARY -DAndroid -DBOOST_ASIO_DISABLE_SERIAL_PORT=1"
export LDFLAGS="-lgcc"

export UDT_ARCH=MIPS32
export CXX=${TOOLCHAIN_PREFIX}g++
export CC=${TOOLCHAIN_PREFIX}gcc
export LD=${TOOLCHAIN_PREFIX}ld
export PATH=${PATH}:${SYSTEMROOT}/bin

export OPENSSL_TARGET=linux-generic32
export CUSTOM_CURL_FLAGS="--with-random=/dev/urandom"
export USE_STLPORT=1
