#include <filedevice/sdl/seadSDLFileDeviceSDL.h>

#include <cerrno>

namespace sead {

FileDevice* SDLFileDevice::doOpen_(FileHandle* handle, const SafeString& filename, FileOpenFlag flag)
{
    FixedSafeString<512> filepath;
    doResolvePath_(&filepath, filename);

    const char* accessMode = nullptr;

    switch (flag)
    {
        case FileOpenFlag::eReadOnly:
            accessMode = "r";
            break;

        case FileOpenFlag::eWriteOnly:
            accessMode = "w";
            break;

        case FileOpenFlag::eReadWrite:
            accessMode = "r+";
            break;

        case FileOpenFlag::eCreate:
            accessMode = "w"; // TODO: Is this correct ?
            break;

        default:
            return nullptr;
    }

    std::FILE* file = std::fopen(filepath.cstr(), accessMode);

    mLastRawError = errno;

    if (!file)
        return nullptr;

    setHANDLE_(handle, file);
    return this;
}

bool SDLFileDevice::doClose_(FileHandle* handle)
{
    bool success = std::fclose(*getHANDLE_(handle));
    if (!success)
        mLastRawError = errno;

    return (success == false);
}

bool SDLFileDevice::doFlush_(FileHandle* handle)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doRemove_(const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doRead_(u32* readSize, FileHandle* handle, u8* buf, u32 size)
{
    size_t bytesRead = std::fread(buf, sizeof(u8), size, *getHANDLE_(handle));

    mLastRawError = errno;

    if (readSize)
        *readSize = bytesRead;

    return bytesRead == size;
}

bool SDLFileDevice::doWrite_(u32* writeSize, FileHandle* handle, const u8* buf, u32 size)
{
    bool bytesWritten = std::fwrite(buf, sizeof(u8), size, *getHANDLE_(handle));

    mLastRawError = errno;

    if (writeSize)
        *writeSize = bytesWritten;

    return bytesWritten == size;
}

bool SDLFileDevice::doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin)
{
    s32 moveMethod = SEEK_SET;
    switch (origin)
    {
        case SeekOrigin::eBegin:
            moveMethod = SEEK_SET;
            break;

        case SeekOrigin::eCurrent:
            moveMethod = SEEK_CUR;
            break;

        case SeekOrigin::eEnd:
            moveMethod = SEEK_END;
            break;

        default:
            return false;
    }

    s32 res = std::fseek(*getHANDLE_(handle), offset, moveMethod);

    mLastRawError = errno;

    return res == 0;
}

bool SDLFileDevice::doGetCurrentSeekPos_(u32* pos, FileHandle* handle)
{
    s32 currentPos = std::ftell(*getHANDLE_(handle));

    mLastRawError = errno;

    if (currentPos == -1)
    {
        *pos = 0;
        return false;
    }

    *pos = static_cast<u32>(currentPos);
    return true;
}

bool SDLFileDevice::doGetFileSize_(u32* size, const SafeString& path)
{
    // TODO
    SEAD_UNUSED(size);
    SEAD_UNUSED(path);
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doGetFileSize_(u32* size, FileHandle* handle)
{
    u32 prevPos = 0;
    if (!doGetCurrentSeekPos_(&prevPos, handle))
    {
        *size = 0;
        return false;
    }

    if (!doSeek_(handle, 0, SeekOrigin::eEnd))
    {
        *size = 0;
        return false;
    }

    u32 fileSize = 0;
    if (!doGetCurrentSeekPos_(&fileSize, handle))
    {
        *size = 0;
        return false;
    }

    if (!doSeek_(handle, prevPos, SeekOrigin::eBegin))
    {
        *size = 0;
        return false;
    }

    *size = fileSize;
    return true;
}

bool SDLFileDevice::doIsExistFile_(bool* isExist, const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doIsExistDirectory_(bool* isExist, const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

FileDevice* SDLFileDevice::doOpenDirectory_(DirectoryHandle* handle, const SafeString& dirname)
{
    // TODO
    SEAD_ASSERT(false);
    return nullptr;
}

bool SDLFileDevice::doCloseDirectory_(DirectoryHandle* handle)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doReadDirectory_(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool SDLFileDevice::doMakeDirectory_(const SafeString& path, u32 permission)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

RawErrorCode SDLFileDevice::doGetLastRawError_() const
{
    return mLastRawError;
}

void SDLFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    // TODO: ?
    // CHAR exPath[512];
    // ExpandEnvironmentStringsA(path.cstr(), exPath, 512);

    out->format("%s/%s", mCWD.cstr(), path.cstr());
}

} // namespace sead
