LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

JNI_PATH := ../tools/android/jni

SDL_PATH := $(JNI_PATH)/SDL

LOCAL_CPP_FEATURES += exceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/$(JNI_PATH)/SDL_image \
	$(LOCAL_PATH)/$(JNI_PATH)/SDL_mixer \
	$(LOCAL_PATH)/$(JNI_PATH)/SDL_net \
	$(LOCAL_PATH)/$(JNI_PATH)/bullet/src \
	$(LOCAL_PATH)/$(JNI_PATH)/freetype/include \
	$(LOCAL_PATH)/$(JNI_PATH)/assimp/include \
	$(LOCAL_PATH)/$(JNI_PATH)/lua/src \
	$(LOCAL_PATH)/confuse \
	$(LOCAL_PATH)/guichan \
	$(LOCAL_PATH)/spark \
	$(LOCAL_PATH)/../tools/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	platform/android.cpp \
	client.cpp \
	events.cpp \
	touch.cpp \
	game_state.cpp \
	game_engine.cpp \
	game_manager.cpp \
	game_settings.cpp \
	physics_bullet.cpp \
	$(subst $(LOCAL_PATH)/,, \
		$(wildcard $(LOCAL_PATH)/audio/*.cpp) \
		$(wildcard $(LOCAL_PATH)/confuse/*.c) \
		$(wildcard $(LOCAL_PATH)/entity/*.cpp) \
		$(wildcard $(LOCAL_PATH)/fx/*.cpp) \
		$(wildcard $(LOCAL_PATH)/gui/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/Core/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/RenderingAPIs/OpenGL/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/Extensions/Emitters/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/Extensions/Modifiers/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/Extensions/Renderers/*.cpp) \
		$(wildcard $(LOCAL_PATH)/spark/Extensions/Zones/*.cpp) \
		$(wildcard $(LOCAL_PATH)/guichan/*.cpp) \
		$(wildcard $(LOCAL_PATH)/guichan/opengl/*.cpp) \
		$(wildcard $(LOCAL_PATH)/guichan/sdl/*.cpp) \
		$(wildcard $(LOCAL_PATH)/guichan/widgets/*.cpp) \
		$(wildcard $(LOCAL_PATH)/http/*.cpp) \
		$(wildcard $(LOCAL_PATH)/lua/*.cpp) \
		$(wildcard $(LOCAL_PATH)/mod/*.cpp) \
		$(wildcard $(LOCAL_PATH)/map/*.cpp) \
		$(wildcard $(LOCAL_PATH)/net/*.cpp) \
		$(wildcard $(LOCAL_PATH)/i18n/*.cpp) \
		$(wildcard $(LOCAL_PATH)/render/*.cpp) \
		$(wildcard $(LOCAL_PATH)/render_opengl/*.cpp) \
		$(wildcard $(LOCAL_PATH)/util/*.cpp) \
		$(wildcard $(LOCAL_PATH)/weapons/*.cpp) \
	)

DONT_COMPILE_FILES := render_opengl/gl_debug_drawer.cpp \
	gui/controls.cpp

LOCAL_SRC_FILES := $(filter-out $(DONT_COMPILE_FILES),$(LOCAL_SRC_FILES))

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer SDL2_net bullet libft2 bullet assimp lua

LOCAL_LDLIBS := -lGLESv2 -llog

LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES=1

include $(BUILD_SHARED_LIBRARY)
