export BUILD_DIR=$PWD

cd ${BUILD_DIR} ;
rm -Rf ios ; mkdir ios ; cd ios  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DTARGET=iOS -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../ios_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/ios-arm.cmake ../../ ; make VERBOSE=1  install ;

