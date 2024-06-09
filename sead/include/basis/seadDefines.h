#pragma once

#ifdef _MSC_VER
    #define SEAD_COMPILER_MSVC
#elif defined(__clang__)
    #define SEAD_COMPILER_CLANG
#elif defined(__GNUC__)
    #define SEAD_COMPILER_GCC
#else
    #error "Unsupported compiler"
#endif // _MSC_VER

#define SEAD_NO_COPY(CLASS)                  \
public:                                      \
    CLASS(const CLASS&) = delete;            \
    CLASS& operator=(const CLASS&) = delete

#define SEAD_UNUSED(VARIABLE) static_cast<void>(VARIABLE)

#define SEAD_GFX_GL 0
#define SEAD_GFX_VK 1

// TODO: Unhardcode
#define SEAD_GFX_BACKEND SEAD_GFX_GL
