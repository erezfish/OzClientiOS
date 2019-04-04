# cd  \rm -Rf android ; mkdir android ; cd android  ; \

#WARNNIG: libOsnClient doesn't exist in SVN!!!
#You have to put LibOsnClient.so to each Android ARCH in ../../lib/Android

export BUILD_DIR=$PWD
#export ANDROID_NDK=${BUILD_DIR}../../android-ndk-r13b
export ANDROID_NDK=$BUILD_DIR/../../../..//android-ndk-r13b

#### x86_64build:
#rm -Rf android ; mkdir android ; cd android  ; \
#cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DANDROID_NATIVE_API_LEVEL=24 -DTARGET=android -DANDROID_ABI=x86_64 -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../android_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ../../ ; make VERBOSE=1  install ;

#### armeabi-v7a build:
cd ${BUILD_DIR} ;
rm -Rf android ; mkdir android ; cd android  ; \
cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DANDROID_NATIVE_API_LEVEL=24 -DTARGET=android -DANDROID_ABI=armeabi-v7a _DANDROID_PLATFORM=android-24 -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../android_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ../../ ; make VERBOSE=1  install ;

####arm64-v8a build:
#cd ${BUILD_DIR} ;
#rm -Rf android ; mkdir android ; cd android  ; \
#cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DANDROID_NATIVE_API_LEVEL=24 -DTARGET=android -DANDROID_ABI=arm64-v8a -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../android_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ../../ ; make VERBOSE=1  install ;

#### x86 build:
#cd ${BUILD_DIR} ; 
#rm -Rf android ; mkdir android ; cd android  ; \
#cmake -DCMAKE_CXX_FLAGS:STRING="-std=c++11" -DANDROID_NATIVE_API_LEVEL=24 -DTARGET=android -DANDROID_ABI=x86 -DLIBRARY_OUTPUT_PATH_ROOT=`pwd`/../android_output -DCMAKE_TOOLCHAIN_FILE=../../cmake/android.toolchain.cmake ../../ ; make VERBOSE=1  install ;


echo
echo =============
echo output files:
echo =============
export OUT_BIN=${BUILD_DIR}/android_output/bin
printf "At %s" "${OUT_BIN}:"
ls ${OUT_BIN} -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'
echo

export OUT_LIB_ARM7=${BUILD_DIR}/android_output/libs/armeabi-v7a
printf "At %s" "${OUT_LIB_ARM7}:"
ls ${OUT_LIB_ARM7} -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'
echo

export OUT_LIB_ARM8=${BUILD_DIR}/android_output/libs/arm64-v8a
printf "At %s" "${OUT_LIB_ARM8}:"
ls ${OUT_LIB_ARM8} -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'

export OUT_LIB_X86_64=${BUILD_DIR}/android_output/libs/x86_64
printf "At %s" "${OUT_LIB_X86_64}:"
ls ${OUT_LIB_X86_64} -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'

export OUT_LIB_X86=${BUILD_DIR}/android_output/libs/x86
printf "At %s" "${OUT_LIB_X86}:"
ls ${OUT_LIB_X86} -l --time-style=long-iso | awk '{print $5, $6, $7, $8, $9}'


echo ==================================================================


