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
	../../../src/audio.cpp \
	../../../src/graphics.cpp \
	../../../src/input.cpp \
	../../../src/logic.cpp \
	../../../src/network.cpp \
	../../../src/system.cpp \
	../../../src/utils.cpp \
	../../../src/window.cpp \

# -- libs list --                       
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_net SDL2_ttf openal-prebuilt zlib-prebuilt ogg-prebuilt vorbis-prebuilt vorbisenc-prebuilt vorbisfile-prebuilt
  
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -latomic -lm

include $(BUILD_SHARED_LIBRARY)
