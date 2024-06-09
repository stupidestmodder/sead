#include <filedevice/win/seadWinFileDeviceWin.h>

namespace sead {

FileDevice* WinFileDevice::doOpen_(FileHandle* handle, const SafeString& filename, FileOpenFlag flag)
{
    FixedSafeString<512> filepath;
    doResolvePath_(&filepath, filename);

    DWORD accessFlag = 0;
    DWORD createFlag = 0;

    switch (flag)
    {
        case FileOpenFlag::eReadOnly:
            accessFlag = GENERIC_READ;
            createFlag = OPEN_EXISTING;
            break;

        case FileOpenFlag::eWriteOnly:
            accessFlag = GENERIC_WRITE;
            createFlag = CREATE_ALWAYS;
            break;

        case FileOpenFlag::eReadWrite:
            accessFlag = GENERIC_READ | GENERIC_WRITE;
            createFlag = OPEN_EXISTING;
            break;

        case FileOpenFlag::eCreate:
            accessFlag = GENERIC_WRITE;
            createFlag = CREATE_NEW;
            break;

        default:
            return nullptr;
    }

    HANDLE file = CreateFileA(filepath.cstr(), accessFlag, FILE_SHARE_READ, nullptr, createFlag, FILE_ATTRIBUTE_NORMAL, nullptr);

    mLastRawError = GetLastError();

    if (file == INVALID_HANDLE_VALUE)
        return nullptr;

    setHANDLE_(handle, file);
    return this;
}

bool WinFileDevice::doClose_(FileHandle* handle)
{
    bool success = CloseHandle(*getHANDLE_(handle));
    if (!success)
        mLastRawError = GetLastError();

    return success;
}

bool WinFileDevice::doFlush_(FileHandle* handle)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool WinFileDevice::doRemove_(const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool WinFileDevice::doRead_(u32* readSize, FileHandle* handle, u8* buf, u32 size)
{
    DWORD bytesRead = 0;
    bool success = ReadFile(*getHANDLE_(handle), buf, size, &bytesRead, nullptr);

    mLastRawError = GetLastError();

    if (readSize)
        *readSize = bytesRead;

    return success;
}

bool WinFileDevice::doWrite_(u32* writeSize, FileHandle* handle, const u8* buf, u32 size)
{
    DWORD bytesWritten = 0;
    bool success = WriteFile(*getHANDLE_(handle), buf, size, &bytesWritten, nullptr);

    mLastRawError = GetLastError();

    if (writeSize)
        *writeSize = bytesWritten;

    return success;
}

bool WinFileDevice::doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin)
{
    DWORD moveMethod = FILE_BEGIN;
    switch (origin)
    {
        case SeekOrigin::eBegin:
            moveMethod = FILE_BEGIN;
            break;

        case SeekOrigin::eCurrent:
            moveMethod = FILE_CURRENT;
            break;

        case SeekOrigin::eEnd:
            moveMethod = FILE_END;
            break;

        default:
            return false;
    }

    DWORD currentPos = SetFilePointer(*getHANDLE_(handle), offset, nullptr, moveMethod);

    mLastRawError = GetLastError();

    return currentPos != INVALID_SET_FILE_POINTER;
}

bool WinFileDevice::doGetCurrentSeekPos_(u32* pos, FileHandle* handle)
{
    DWORD currentPos = SetFilePointer(*getHANDLE_(handle), 0, nullptr, FILE_CURRENT);

    mLastRawError = GetLastError();

    if (currentPos == INVALID_SET_FILE_POINTER)
    {
        *pos = 0;
        return false;
    }

    *pos = currentPos;
    return true;
}

bool WinFileDevice::doGetFileSize_(u32* size, const SafeString& path)
{
    FixedSafeString<512> filepath;
    doResolvePath_(&filepath, path);

    WIN32_FIND_DATAA findData;
    HANDLE findFile = FindFirstFileA(filepath.cstr(), &findData);

    mLastRawError = GetLastError();

    if (findFile != INVALID_HANDLE_VALUE)
    {
        bool success = FindClose(findFile);

        mLastRawError = GetLastError();

        if (success)
        {
            SEAD_ASSERT(findData.nFileSizeHigh == 0);

            *size = findData.nFileSizeLow;
            return true;
        }
    }

    *size = 0;
    return false;
}

bool WinFileDevice::doGetFileSize_(u32* size, FileHandle* handle)
{
    DWORD fileSizeHigh = 0;
    DWORD fileSize = GetFileSize(*getHANDLE_(handle), &fileSizeHigh);

    mLastRawError = GetLastError();

    if (fileSize == INVALID_FILE_SIZE)
    {
        *size = 0;
        return false;
    }

    SEAD_ASSERT(fileSizeHigh == 0);

    *size = fileSize;
    return true;
}

bool WinFileDevice::doIsExistFile_(bool* isExist, const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool WinFileDevice::doIsExistDirectory_(bool* isExist, const SafeString& path)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

FileDevice* WinFileDevice::doOpenDirectory_(DirectoryHandle* handle, const SafeString& dirname)
{
    // TODO
    SEAD_ASSERT(false);
    return nullptr;
}

bool WinFileDevice::doCloseDirectory_(DirectoryHandle* handle)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool WinFileDevice::doReadDirectory_(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

bool WinFileDevice::doMakeDirectory_(const SafeString& path, u32 permission)
{
    // TODO
    SEAD_ASSERT(false);
    return false;
}

RawErrorCode WinFileDevice::doGetLastRawError_() const
{
    return mLastRawError;
}

void WinFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    CHAR exPath[512];
    ExpandEnvironmentStringsA(path.cstr(), exPath, 512);

    out->format("%s/%s", mCWD.cstr(), exPath);
}

} // namespace sead
