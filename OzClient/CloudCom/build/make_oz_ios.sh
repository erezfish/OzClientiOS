
#export BUILD_DIR=$PWD
#export ANDROID_NDK=${BUILD_DIR}../../android-ndk-r13b
#export IOS_SDK=$BUILD_DIR/../../../..//android-ndk-r13b

#### x86_64build:
rm -Rf ios ; mkdir ios ; cd ios  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DTARGET=darwin -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../ios_output -DCMAKE_TOOLCHAIN_FILE=./toolchain/iOS.cmake ../../ ; make VERBOSE=1  install ;


