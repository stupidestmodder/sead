#pragma once

#if !defined(SEAD_TARGET_DEBUG) && !defined(SEAD_TARGET_RELEASE)
    #error "No build target defined"
#endif

#if !defined(SEAD_PLATFORM_WINDOWS)
    #error "No plaform defined"
#endif

#if !defined(SEAD_USE_GL) && !defined(SEAD_USE_VK)
    #error "No graphics backend defined"
#endif

#if defined(_MSC_VER)
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
