APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -DCOCOS2D_DEBUG=1 -Wno-error=format-security
APP_ABI := armeabi x86
#APP_ABI := x86
NDK_TOOLCHAIN_VERSION := clang
