export BUILD_DIR=$PWD

cd ${BUILD_DIR} ;
mkdir oz_libs_archs
cd oz_libs_archs
mkdir armv7
mkdir arm64
mkdir x86_64

cd ${BUILD_DIR} ;
rm -Rf ios ; mkdir ios ; cd ios  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DTARGET=iOS -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../ios_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/ios-armv7.cmake ../../ ; make VERBOSE=1  install ;
mv OzAgent/libOzClient.so oz_libs_archs/armv7

cd ${BUILD_DIR} ;
rm -Rf ios ; mkdir ios ; cd ios  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DTARGET=iOS -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../ios_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/ios-arm64.cmake ../../ ; make VERBOSE=1  install ;
mv OzAgent/libOzClient.so oz_libs_archs/arm64

cd ${BUILD_DIR} ;
rm -Rf ios ; mkdir ios ; cd ios  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DTARGET=iOS -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../ios_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/ios-x86_64.cmake ../../ ; make VERBOSE=1  install ;
mv OzAgent/libOzClient.so oz_libs_archs/x86_64

cd ${BUILD_DIR}/oz_libs_archs


