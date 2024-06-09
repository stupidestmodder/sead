#pragma once

#include <prim/seadSafeString.h>

namespace sead {

// TODO
class Path
{
public:
    static bool getDriveName(BufferedSafeString* dst, const SafeString& src);
    static bool getExt(BufferedSafeString* dst, const SafeString& src);
    static void getPathExceptDrive(BufferedSafeString* dst, const SafeString& src);
    static bool getFileName(BufferedSafeString* dst, const SafeString& src);
    static bool getBaseFileName(BufferedSafeString* dst, const SafeString& src);
    static bool getDirectoryName(BufferedSafeString* dst, const SafeString& src);
    static void join(BufferedSafeString* dst, const char* a, const char* b);
    static void changeDelimiter(BufferedSafeString* path, char delimiter);
};

} // namespace sead
