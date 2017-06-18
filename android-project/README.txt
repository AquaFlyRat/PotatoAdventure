    *** NEEDED TOOLS ***

	
Android SDK     with following components installed:
                  * Tools
				      * Android SDK Tools
					  * Android SDK Platform-Tools
					  * Android SDK Build-Tools
			      * Android 3.1 (API 12)
				      * SDK Platform
				  * Android 2.3.3 (API 10)
				      * SDK Platform

Android NDK

Apache Ant

Cmake
  (only for libs modification)
  
MSYS            or MSYS2 or Cygwin or another linux terminal emulator
  (only for libs modification)
			

-----------------------


    *** PATH SETUP ***

Following needs to be appended to PATH:

Y:\android-sdk\tools;Y:\android-sdk\platform-tools;Y:\android-ndk;Y:\apache-ant\bin;Y:\cmake\bin

Where
Y:\android-sdk = path to SDK
Y:\android-ndk = path to NDK
Y:\apache-ant  = path to Apache Ant
Y:\cmake       = path to Cmake


-----------------------


    *** BUILDING ***
	

Run this:

android update project --path ../android-project
ndk-build
ant debug

If you want to put that in a .bat file, use following
	
cmd /C "android update project --path ../android-project"
cmd /C "ndk-build"
cmd /C "ant debug"

You can replace `debug` with `release` for final build, but it may prevent the application from working on the emulator.
`ant` can also sign your executables, look into command-line flags list.

Following will push your .apk to a running emulator:

adb install -rg blah.apk

Temporary files are in android-project/obj. Deleting them greatly reduces project size. After that you will need a complete rebuild.
	

-----------------------


    *** PROJECT CONFIGURATION ***
	

Human-readable name     @ res/values/strings.xml:3  ` <string name="app_name">Super awesome game</string>`
Icon                    @ res/drawable-   dpi/ic_launcher.png
									    m      48x48
										h      72x72
									    xh     96x96
										xxh   144x144
										For best quality draw in 2304x2304 and resize to a proper size.
									  
									  
Project name        +-  @ build.xml:3               ` name="projectname" `
					+-  @ AndroidManifest.xml:24    ` name="projectname" `
Source files        |   @ jni/src/Android.mk:3      ` 1.cpp 2.cpp 3.cpp `     You can also use `\` + `\n` as separators.
Compiler flags      |   @ jni/Application.mk:10     ` APP_CPPFLAGS := -blah -blah `
Package name        +-  @ AndroidManifest.xml:4     ` package="com.author.appname" `
                    |
					+----> ALSO CHANGE FOLDER STRUCTURE INSIDE `src` ACCORDINGLY. It must contain following:
						   `org/libsdl/app/SDLActivity.java` Don't touch this.
					       `org/author/appname/projectname.java` That file must contain this:
						     package org.author.appname;
                             import org.libsdl.app.SDLActivity;
                             public class projectname extends SDLActivity {}
						   Replace that `projectname` above with your project name.
						   No other files must be there. You may safely delete any empty folders.
			

-----------------------


    *** NEEDED FILE CONTENTS ***
	

jni/Application.mk:
--BEGIN--
APP_ABI := armeabi armeabi-v7a x86 mips
APP_STL := c++_shared
LOCAL_SHARED_LIBRARIES := c++_shared
APP_CFLAGS += -w
APP_CPPFLAGS += -fexceptions -frtti
APP_CPPFLAGS += -std=c++14 -O3 -s
NDK_TOOLCHAIN_VERSION := clang
--END--

jni/src/Android.mk:
--BEGIN--
LOCAL_PATH := $(call my-dir)
# -- libs definitions --
include $(CLEAR_VARS)
LOCAL_MODULE := zlib-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libz.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := openal-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libopenal.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ogg-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbis-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbisenc-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libvorbisenc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbisfile-prebuilt
LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libvorbisfile.so
include $(PREBUILT_SHARED_LIBRARY)
# -- end libs definitions --
include $(CLEAR_VARS)
LOCAL_MODULE := main
SDL_PATH := ../SDL
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
# -- sources list --
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	 ../../../src/1.cpp \
	 ../../../src/2.cpp \
# -- libs list --
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_net zlib-prebuilt openal-prebuilt ogg-prebuilt vorbis-prebuilt vorbisenc-prebuilt vorbisfile-prebuilt
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -latomic
include $(BUILD_SHARED_LIBRARY)
--END--
	

-----------------------


    *** GENERATING STANDALONE TOOLCHAINS ***
	
	
Useful when adding external libs.

(You can adjust flags and paths if needed.)

    mkdir clang_3.8_android_api12_androideabi
	cd    clang_3.8_android_api12_androideabi
	python ..\android-ndk\build\tools\make_standalone_toolchain.py --arch arm --stl libc++ --api 12 --force
	cd ..
	mkdir clang_3.8_android_api12_x86
	cd    clang_3.8_android_api12_x86
	python ..\android-ndk\build\tools\make_standalone_toolchain.py --arch x86 --stl libc++ --api 12 --force
	cd ..
	mkdir clang_3.8_android_api12_mips
	cd    clang_3.8_android_api12_mips
	python ..\android-ndk\build\tools\make_standalone_toolchain.py --arch mips --stl libc++ --api 12 --force
	cd ..
	# Extract the resulting toolchains from archives and folders.
	
	
    android-ndk/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --platform=android-12 --toolchain=arm-linux-androideabi-clang --install-dir=Y:/clang_3.8_android_api12_androideabi/
	android-ndk/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --platform=android-12                   --toolchain=x86-clang --install-dir=Y:/clang_3.8_android_api12_x86/
	android-ndk/build/tools/make-standalone-toolchain.sh --use-llvm --stl=libc++ --platform=android-12  --toolchain=mipsel-linux-android-clang --install-dir=Y:/clang_3.8_android_api12_mips/

	
-----------------------


    *** RUNNING STANDALONE TOOLCHAINS ***
	
	
Use following compilers and flags when building:
(You can additionally add `-O3 -s`. `-l...` ones are only for linker. MIPS LINKER FOR *EXECUTABLES* GIVES YOU ERROR IF YOU USE -lc++_shared WITHOUT -lm AFTER IT.)

  * armeabi:        arm-linux-androideabi:    -lc++_shared -mthumb
  * armeabi-v7a:    arm-linux-androideabi:    -lc++_shared -mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
  * x86:            i686-linux-android:       -lc++_shared
  * mips:           mipsel-linux-android:     -lc++_shared
  
 
@ configure + make      NOT TESTED

// Warning, this lacks C++ compiler paths setup. If you need to compile a C++ lib, run `configure --help` to figure out what variable controls C++ compiler path. Then assign to it as to `CC` variable. Same with C++ flags.
// Put your arguments after `autogen.sh` or `configure` if needed.

// You may need to adjust versions inside aclocal.m4 is make fails.

WITH AUTOGEN.SH
*---BEGIN-SCRIPT---* // RUN THIS UNDER MSYS OR EQUIVALENT

rm -r -d -f ./../_build-android
mkdir ./../_build-android

make clean
autogen.sh CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb" --host=arm-linux-androideabi
make
mkdir ./../_build-android/armeabi
find -name "*.so" -exec mv {} ./../_build-android/armeabi/ \;

make clean
autogen.sh CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" --host=arm-linux-androideabi
make
mkdir ./../_build-android/armeabi-v7a
find -name "*.so" -exec mv {} ./../_build-android/armeabi-v7a/ \;

make clean
autogen.sh CC="/y/clang_3.8_android_api12_x86/bin/i686-linux-android-clang" LIBS="-lc++_shared" CFLAGS="" --host=i686-linux-android
make
mkdir ./../_build-android/x86
find -name "*.so" -exec mv {} ./../_build-android/x86/ \;

make clean
autogen.sh CC="/y/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang" LIBS="-lc++_shared -lm" CFLAGS="" --host=mipsel-linux-android
make
mkdir ./../_build-android/mips
find -name "*.so" -exec mv {} ./../_build-android/mips/ \;

rm -r -d -f ./build-android
mv ./../_build-android ./
mv ./_build-android ./build-android

*---END-SCRIPT---*

WITH CONFIGURE
*---BEGIN-SCRIPT---* // RUN THIS UNDER MSYS OR EQUIVALENT

rm -r -d -f ./../_build-android
mkdir ./../_build-android

make clean
configure CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb" --host=arm-linux-androideabi
make
mkdir ./../_build-android/armeabi
find -name "*.so" -exec mv {} ./../_build-android/armeabi/ \;

make clean
configure CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" --host=arm-linux-androideabi
make
mkdir ./../_build-android/armeabi-v7a
find -name "*.so" -exec mv {} ./../_build-android/armeabi-v7a/ \;

make clean
configure CC="/y/clang_3.8_android_api12_x86/bin/i686-linux-android-clang" LIBS="-lc++_shared" CFLAGS="" --host=i686-linux-android
make
mkdir ./../_build-android/x86
find -name "*.so" -exec mv {} ./../_build-android/x86/ \;

make clean
configure CC="/y/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang" LIBS="-lc++_shared -lm" CFLAGS="" --host=mipsel-linux-android
make
mkdir ./../_build-android/mips
find -name "*.so" -exec mv {} ./../_build-android/mips/ \;

rm -r -d -f ./build-android
mv ./../_build-android ./
mv ./_build-android ./build-android

*---END-SCRIPT---*

SEPARATE FOLDERS WITH AUTOGEN.SH
*---BEGIN-SCRIPT---* // RUN THIS UNDER MSYS OR EQUIVALENT

make clean

rm -r -d -f ./../_build-android
rm -r -d -f ./build-android
mkdir ./../_build-android
mkdir ./../_build-android/armeabi
mkdir ./../_build-android/armeabi-v7a
mkdir ./../_build-android/x86
mkdir ./../_build-android/mips
cp -r -f . ../_build-android/armeabi/lib
cp -r -f . ../_build-android/armeabi-v7a/lib
cp -r -f . ../_build-android/x86/lib
cp -r -f . ../_build-android/mips/lib
mv ./../_build-android ./
mv ./_build-android ./build-android
cd build-android

cd armeabi/lib
autogen.sh CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb" --host=arm-linux-androideabi
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd armeabi-v7a/lib
autogen.sh CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" --host=arm-linux-androideabi
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd x86/lib
autogen.sh CC="/y/clang_3.8_android_api12_x86/bin/i686-linux-android-clang" LIBS="-lc++_shared" CFLAGS="" --host=i686-linux-android
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd mips/lib
autogen.sh CC="/y/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang" LIBS="-lc++_shared -lm" CFLAGS="" --host=mipsel-linux-android
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

*---END-SCRIPT---*

SEPARATE FOLDERS WITH CONFIGURE
*---BEGIN-SCRIPT---* // RUN THIS UNDER MSYS OR EQUIVALENT

make clean

rm -r -d -f ./../_build-android
rm -r -d -f ./build-android
mkdir ./../_build-android
mkdir ./../_build-android/armeabi
mkdir ./../_build-android/armeabi-v7a
mkdir ./../_build-android/x86
mkdir ./../_build-android/mips
cp -r -f . ../_build-android/armeabi/lib
cp -r -f . ../_build-android/armeabi-v7a/lib
cp -r -f . ../_build-android/x86/lib
cp -r -f . ../_build-android/mips/lib
mv ./../_build-android ./
mv ./_build-android ./build-android
cd build-android

cd armeabi/lib
configure CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb" --host=arm-linux-androideabi
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd armeabi-v7a/lib
configure CC="/y/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang" LIBS="-lc++_shared" CFLAGS="-mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" --host=arm-linux-androideabi
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd x86/lib
configure CC="/y/clang_3.8_android_api12_x86/bin/i686-linux-android-clang" LIBS="-lc++_shared" CFLAGS="" --host=i686-linux-android
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

cd mips/lib
configure CC="/y/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang" LIBS="-lc++_shared -lm" CFLAGS="" --host=mipsel-linux-android
make
find -name "*.so" -exec mv {} .. \;
cd ..
rm -r -d -f lib
cd ..

*---END-SCRIPT---*
 
@ cmake gui

Press `specify options for cross-compiling`, set system to `android` and compilers to appropriate clang.cmd and clang++.cmd.
BEFORE pressing configure, set following variables:
  * When using C:              CMAKE_C_FLAGS             = your flags
  * When using C++:            CMAKE_CXX_FLAGS           = your flags
  * When building .so libs:    CMAKE_SHARED_LINKER_FLAGS = your flags
  * When building static libs: CMAKE_STATIC_LINKER_FLAGS = your flags
  * When building executables: CMAKE_EXE_LINKER_FLAGS    = your flags
  * I dunno what it does:      CMAKE_MODULE_LINKER_FLAGS = your flags
  
@ cmake

Open source directory, then run following script. And then put contents of generated 
If you need to specify custom variables, put them into the cmake command before `..`. Use `-D Name:Type=Value`. Hint: Use cmake-gui first to determine what variables you want to set.
(You can adjust flags and paths if needed.)

IMPORTANT: OpenAL (tested on 1.17.2 and .1) must NOT be built with "Release" mode because it crashes. Use "RelWithDebInfo" (default) or "Debug".

*---BEGIN-SCRIPT---*
rd /S /Q build-android
mkdir build-android
cd build-android

mkdir armeabi
cd armeabi
mkdir temporary_stuff
cd temporary_stuff
cmake -D CMAKE_SYSTEM_NAME:string=android -D CMAKE_C_COMPILER:filepath="Y:/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang.cmd" ^
										  -D CMAKE_CXX_COMPILER:filepath="Y:/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang++.cmd" ^
										  -D CMAKE_C_FLAGS:string="-w -O3 -mthumb" ^
										  -D CMAKE_CXX_FLAGS:string="-w -O3 -mthumb" ^
										  -D CMAKE_SHARED_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_MODULE_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_BUILD_TYPE:string="Release" ^
										  -G "MinGW Makefiles" ^
										  ../../..
mingw32-make.exe
move /Y *.so ..
cd ..
rd /S /Q temporary_stuff
cd ..

mkdir armeabi-v7a
cd armeabi-v7a
mkdir temporary_stuff
cd temporary_stuff

cmake -D CMAKE_SYSTEM_NAME:string=android -D CMAKE_C_COMPILER:filepath="Y:/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang.cmd" ^
										  -D CMAKE_CXX_COMPILER:filepath="Y:/clang_3.8_android_api12_androideabi/bin/arm-linux-androideabi-clang++.cmd" ^
										  -D CMAKE_C_FLAGS:string="-w -O3 -mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" ^
										  -D CMAKE_CXX_FLAGS:string="-w -O3 -mthumb -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16" ^
										  -D CMAKE_SHARED_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_MODULE_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_BUILD_TYPE:string="Release" ^
										  -G "MinGW Makefiles" ^
										  ../../..
mingw32-make.exe
move /Y *.so ..
cd ..
rd /S /Q temporary_stuff
cd ..

mkdir x86
cd x86
mkdir temporary_stuff
cd temporary_stuff
cmake -D CMAKE_SYSTEM_NAME:string=android -D CMAKE_C_COMPILER:filepath="Y:/clang_3.8_android_api12_x86/bin/i686-linux-android-clang.cmd" ^
										  -D CMAKE_CXX_COMPILER:filepath="Y:/clang_3.8_android_api12_x86/bin/i686-linux-android-clang++.cmd" ^
										  -D CMAKE_C_FLAGS:string="-w -O3" ^
										  -D CMAKE_CXX_FLAGS:string="-w -O3" ^
										  -D CMAKE_SHARED_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_MODULE_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_BUILD_TYPE:string="Release" ^
										  -G "MinGW Makefiles" ^
										  ../../..
mingw32-make.exe
move /Y *.so ..
cd ..
rd /S /Q temporary_stuff
cd ..

mkdir mips
cd mips
mkdir temporary_stuff
cd temporary_stuff
cmake -D CMAKE_SYSTEM_NAME:string=android -D CMAKE_C_COMPILER:filepath="Y:/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang.cmd" ^
										  -D CMAKE_CXX_COMPILER:filepath="Y:/clang_3.8_android_api12_mips/bin/mipsel-linux-android-clang++.cmd" ^
										  -D CMAKE_C_FLAGS:string="-w -O3" ^
										  -D CMAKE_CXX_FLAGS:string="-w -O3" ^
										  -D CMAKE_SHARED_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_MODULE_LINKER_FLAGS:string="-lc++_shared" ^
										  -D CMAKE_BUILD_TYPE:string="Release" ^
										  -G "MinGW Makefiles" ^
										  ../../..
mingw32-make.exe
move /Y *.so ..
cd ..
rd /S /Q temporary_stuff
cd ..

cd ..
pause
*---END-SCRIPT---*


@ Useful things:

Fix for `autogen.sh` under MSYS, nice for libvorbis. Put this on a new line after `autogen.sh` if it complains about `aclocal.m4`.
             sed -i -e "s/\[2\.4\]/[2.4.6]/g" -e "s/\[1\.3294\]/[2.4.6]/g" -e "s/'2\.4'/'2.4.6'/g" -e "s/'1\.3294'/'2.4.6'/g" aclocal.m4

-mno-ieee-fp    It doesn't work on clang for i686, remove it from build scripts if needed. Useful for libvorbis.
                Auto script for all files:    find -name "*.*" -exec sed -i "s/-mno-ieee-fp/ /g" {} \;

libvorbis:   Compile libogg and put an entire directory with it to the libvorbis source directory, rename it to `ogg`. Pass following to autoconf.sh:
			 --with-ogg=/z/Lander/Libs/libvorbis-1.3.5/ogg/ --with-ogg-libraries=/z/Lander/Libs/libvorbis-1.3.5/ogg/src/.libs/ --with-ogg-includes=/z/Lander/Libs/libvorbis-1.3.5/ogg/include/
			 
			 
IF IT FAILS AT RUNTIME SAYING `CAN'T LOAD lib__.so.0` (or other numbers or dots after `.so` extension), THEN OPEN THAT .so IN A HEX EDITOR AND PUT A [NUL] LIKE THIS: `libfoo.so.0` -> `libfoo.so[NUL]0`.
Another option is to use this command (under MSYS or equivalent):
             sed -i -b "s/\(lib[A-z]*\.so\)\.\([0-9]\)/\1\x00\2/g" libfoo.so
Or this one for processing multiple files at once:
             find -name "*.so" -exec sed -i -b "s/\(lib[A-z]*\.so\)\.\([0-9]\)/\1\x00\2/g" {} \;

-----------------------


    *** ADDING EXTERNAL LIBRARIES ***
	
SDL plugins like SDL_net are special case. Put directory with the source into jni/. You can rename it if you want. Then add the library to LOCAL_SHARED_LIBRARIES as described below.

For any other library do following:
	
Compile your lib for each ABI you're using. (List of them can be found at jni/Application.mk:APP_ABI.) Use standalone toolchains.
Then move resulting .so files to appropriate prebuilt-libs/<arch>/ directories.
Then add your lib description AFTER first line of jni/src/Android.mk:

	include $(CLEAR_VARS)
	LOCAL_MODULE := LIBNAME
	LOCAL_SRC_FILES := ../../prebuilt-libs/$(TARGET_ARCH_ABI)/libFILENAME.so
	include $(PREBUILT_SHARED_LIBRARY)
	
Then add your library name (LIBNAME from previous step) to the list at jni/src/Android.mk:LOCAL_SHARED_LIBRARIES. Use spaces or `\` + `\n` as delimiters. `SDL2` must remain the first one.
AND THEN add your library file name (FILENAME from previous step) to the list at src/bla/bla/SDLActivity.java:71, at `getLibraries()`.
FIRST ITEM MUST REMAIN `c++_shared`. YOU NEED TO ADD THIS ITEM MANUALLY IF IT DOES NOT EXIST. ADD YOUR LIBS BETWEEN `SDL2` AND `main`.