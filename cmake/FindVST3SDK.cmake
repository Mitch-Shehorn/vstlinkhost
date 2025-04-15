# FindVST3SDK.cmake
# Finds and configures the VST3 SDK

set(VST3SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/vst3sdk" CACHE PATH "Path to VST3 SDK root directory")

if(NOT EXISTS "${VST3SDK_ROOT}")
    message(WARNING "VST3 SDK not found at ${VST3SDK_ROOT}")
    set(VST3SDK_FOUND FALSE)
    return()
endif()

set(VST3SDK_INCLUDE_DIRS
    "${VST3SDK_ROOT}"
    "${VST3SDK_ROOT}/pluginterfaces"
    "${VST3SDK_ROOT}/base"
    "${VST3SDK_ROOT}/public.sdk"
)

set(VST3SDK_FOUND TRUE)
