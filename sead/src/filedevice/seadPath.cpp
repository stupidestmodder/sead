#include <filedevice/seadPath.h>

#include <math/seadMathCalcCommon.h>

static s32 rfindCharIndex(const sead::SafeString& src, char token)
{
    s32 length = src.calcLength();
    const char* cstr = src.cstr();

    for (s32 i = length; i >= 0; i--)
    {
        if (cstr[i] == token)
            return i;
    }

    return -1;
}

static bool isIncludeChar(const sead::SafeString& str, char token)
{
    return str.include(token);
}

namespace sead {

bool Path::getDriveName(BufferedSafeString* dst, const SafeString& src)
{
    SEAD_ASSERT_MSG(dst, "destination buffer is null");

    dst->trim(0);

    s32 idx = src.findIndex(":");
    if (idx != -1)
        dst->copy(src, idx);

    return idx != -1;
}

bool Path::getExt(BufferedSafeString* dst, const SafeString& src)
{
    SEAD_ASSERT_MSG(dst, "destination buffer is null");

    dst->trim(0);

    s32 idx = rfindCharIndex(src, '.');
    if (idx < 0)
    {
        return false;
    }

    if (isIncludeChar(src.getPart(idx), '/') || isIncludeChar(src.getPart(idx), '\\'))
    {
        return false;
    }

    dst->copy(src.getPart(idx + 1));
    return true;
}

void Path::getPathExceptDrive(BufferedSafeString* dst, const SafeString& src)
{
    SEAD_ASSERT_MSG(dst, "destination buffer is null");

    dst->trim(0);

    s32 idx = src.findIndex("://");
    if (idx != -1)
        dst->copyAt(0, src.getPart(idx + 3));
    else
        dst->copyAt(0, src);
}

bool Path::getFileName(BufferedSafeString* dst, const SafeString& src)
{
    SEAD_ASSERT_MSG(dst, "destination buffer is null");

    dst->trim(0);

    s32 idx = Mathi::max(rfindCharIndex(src, '/'), rfindCharIndex(src, '\\'));
    dst->copy(src.getPart(idx + 1));

    return true;
}

bool Path::getDirectoryName(BufferedSafeString* dst, const SafeString& src)
{
    SEAD_ASSERT_MSG(dst, "destination buffer is null");

    if (dst == &src)
    {
        s32 idx = Mathi::max(rfindCharIndex(src, '/'), rfindCharIndex(src, '\\'));
        if (idx > 0)
        {
            dst->trim(idx);
            return true;
        }
    }
    else
    {
        dst->trim(0);

        s32 idx = Mathi::max(rfindCharIndex(src, '/'), rfindCharIndex(src, '\\'));
        if (idx > 0)
        {
            dst->copy(src, idx);
            return true;
        }
    }

    return false;
}

} // namespace sead
