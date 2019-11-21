# Android.mk
LOCAL_PATH := $(call my-dir)

include ./thirdparty/godot_cpp/Android.mk

include $(CLEAR_VARS)
LOCAL_MODULE := gdsqlite
LOCAL_CPPFLAGS := -std=c++14
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_LDLIBS := -llog

LOCAL_SRC_FILES := \
../src/gdsqlite.cpp \
../src/library.cpp \
../thirdparty/sqlite/sqlite3.c \
../thirdparty/sqlite/spmemvfs.c

LOCAL_C_INCLUDES := \
./thirdparty/sqlite \
./thirdparty/godot_cpp/include \
./thirdparty/godot_cpp/include/core \
./thirdparty/godot_cpp/include/gen \
./thirdparty/godot_cpp/godot_headers

LOCAL_STATIC_LIBRARIES := godot-prebuilt

include $(BUILD_SHARED_LIBRARY)
