This document describe the proccess of building OzClient lib for all android platforms.
OzClient is the library that provides the interface for communication of client-cloud and client-agent..
In addition it provides P2P capabilities (open tunnel, destroy tunnel etc.)

OzClient depend on libcurl which depends on libssl and libcrypto,
So the order this need to be built is: 
1. libssl + libcrypto
2. libcurl
3. libOzClient 

download the oz_build_client package from Ozvisoin drive:
https://drive.google.com/open?id=14j385T85Qbbd-v71WgIhRnBKL_u3pqgS

1. SSL:
extract openssl-1.0.1g_oz.tar.gz
cd openssl
in setenv-android-mod.sh:
set _ANDROID_NDK_ROOT to your root
[optional] set _ANDROID_API (you can leave it on 21)

- run build-all-arch.sh script.
this will build libssl.so and libcrypto.so for all platforms (under prebuilt)

install desired architecure libraries:
cd openssl-1.0.1g
cp ../prebuilt/<ARCH>/* . (where ARCH can be arm64-v8a, armeabi and so on..)
sudo -E make install_sw CC=$ANDROID_TOOLCHAIN/<SE_TOOLCHAIN>-gcc RANLIB=$ANDROID_TOOLCHAIN/<SE_TOOLCHAIN>-ranlib
(SE_TOOLCHAIN can be arm-linux-androideabi-4.9 or aarch64-linux-android-4.9 for example)

now ssl libs are intalled under /usr/local/ssl/android-21/lib/

2. CURL:
extract curl-7.64.0_oz.tar.gz
cd curl-7.64.0/cross-compile
run appropriate script.

make
make install (sudo)

find libcurl in /usr/local/lib/libcurl.so  


3. build OzClient:
cp to OzClient/lib/<architecture> libssl.so and libcrypto.so from /usr/local/ssl/android-21/lib/
cp to OzClient/lib/<architecture> libcurl.so from /usr/local/lib/
cd OzClient/CloudCom/build/
./make_oz_android.sh (enable build of right platform)


if you get error size of array '__curl_rule_01__' is negative:
open curl/curlbuild.h and change the following defines to 8 or 4 (depends on the architecture)
define CURL_SIZEOF_LONG 4
define CURL_SIZEOF_CURL_OFF_T 4


For questions and comments please contact Erez Fishhimer via email:
erez.fishhimer@ozvision.com
fishkerez@gmail.com



