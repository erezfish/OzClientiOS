# build static curl libs with ssl inside:

enter folder curl-android-ios/curl-compile-scripts
run script build_iOS.sh 

this will build curl for all platforms under prebuilt-with-ssl 


# build static ozClient for archs: armv7, arm64, x86_64

open file OzProject/OzClient/AgentCom/cmake/ios-arm.cmake 

in the line:
SET(FLAGS "-O -DNABTO_IOS -DDarwin -fPIC -arch arm64  -miphoneos-version-min=${IOS_SDK_VER} -isysroot ${IOS_SYS_ROOT}" )
change arm64 to the architecture you want to build for, except for x86_64

for building to x86_64, uncomment the lines where it says for x86_64, and comments the lines where it says for other platform.

move each library under oz_lib_archs/<arch>

# Create a fat library from all archs libs:

    lipo -create <libOzArch1.a> <libOzArch2.a> <...> -output <libOZUniversal.a>

