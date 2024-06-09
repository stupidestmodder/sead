#include <filedevice/seadFileDevice.h>

#include <filedevice/seadFileDeviceMgr.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <math/seadMathCalcCommon.h>

namespace sead {

FileDevice::~FileDevice()
{
    FileDeviceMgr* mgr = FileDeviceMgr::instance();
    if (mgr)
        mgr->unmount(this);
}

bool FileDevice::isAvailable() const
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    return doIsAvailable_();
}

u8* FileDevice::tryLoad(LoadArg& arg)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return nullptr;

    return doLoad_(arg);
}

bool FileDevice::trySave(SaveArg& arg)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    return doSave_(arg);
}

FileDevice* FileDevice::tryOpen(FileHandle* handle, const SafeString& filename, FileOpenFlag flag, u32 divSize)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return nullptr;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return nullptr;
    }

    setFileHandleDivSize_(handle, divSize);

    FileDevice* device = doOpen_(handle, filename, flag);
    setHandleBaseFileDevice_(handle, device);

    if (device)
        setHandleBaseOriginalFileDevice_(handle, this);

    return device;
}

bool FileDevice::tryClose(FileHandle* handle)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    bool success = doClose_(handle);
    if (success)
    {
        setHandleBaseFileDevice_(handle, nullptr);
        setHandleBaseOriginalFileDevice_(handle, nullptr);
    }

    return success;
}

bool FileDevice::tryFlush(FileHandle* handle)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    return doFlush_(handle);
}

bool FileDevice::tryRemove(const SafeString& path)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    return doRemove_(path);
}

bool FileDevice::tryRead(u32* readSize, FileHandle* handle, u8* buf, u32 size)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    if (!buf)
    {
        SEAD_ASSERT_MSG(false, "buf is null");
        return false;
    }

    if (handle->getDivSize() == 0)
    {
        bool success = doRead_(readSize, handle, buf, size);
        SEAD_ASSERT_MSG(!readSize || *readSize <= size, "buffer overflow");
        return success;
    }

    u32 totalReadSize = 0;

    do
    {
        u32 bufSize = Mathi::min(size, handle->getDivSize());
        u32 tmpReadSize = 0;

        if (!doRead_(&tmpReadSize, handle, buf, bufSize))
        {
            if (readSize)
                *readSize = totalReadSize;

            return false;
        }

        totalReadSize += tmpReadSize;
        if (tmpReadSize < bufSize)
            break;

        buf += tmpReadSize;
        size -= bufSize;
    } while (size != 0);

    if (readSize)
        *readSize = totalReadSize;

    return true;
}

bool FileDevice::tryWrite(u32* writeSize, FileHandle* handle, const u8* buf, u32 size)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!buf)
    {
        SEAD_ASSERT_MSG(false, "buf is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    return doWrite_(writeSize, handle, buf, size);
}

bool FileDevice::trySeek(FileHandle* handle, s32 offset, SeekOrigin origin)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    return doSeek_(handle, offset, origin);
}

bool FileDevice::tryGetCurrentSeekPos(u32* pos, FileHandle* handle)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    if (!pos)
    {
        SEAD_ASSERT_MSG(false, "pos is null");
        return false;
    }

    return doGetCurrentSeekPos_(pos, handle);
}

bool FileDevice::tryGetFileSize(u32* size, const SafeString& path)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!size)
    {
        SEAD_ASSERT_MSG(false, "size is null");
        return false;
    }

    return doGetFileSize_(size, path);
}

bool FileDevice::tryGetFileSize(u32* size, FileHandle* handle)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!size)
    {
        SEAD_ASSERT_MSG(false, "size is null");
        return false;
    }

    return doGetFileSize_(size, handle);
}

bool FileDevice::tryIsExistFile(bool* isExist, const SafeString& path)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!isExist)
    {
        SEAD_ASSERT_MSG(false, "isExist is null");
        return false;
    }

    return doIsExistFile_(isExist, path);
}

bool FileDevice::tryIsExistDirectory(bool* isExist, const SafeString& path)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!isExist)
    {
        SEAD_ASSERT_MSG(false, "isExist is null");
        return false;
    }

    return doIsExistDirectory_(isExist, path);
}

FileDevice* FileDevice::tryOpenDirectory(DirectoryHandle* handle, const SafeString& dirname)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return nullptr;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return nullptr;
    }

    FileDevice* device = doOpenDirectory_(handle, dirname);
    setHandleBaseFileDevice_(handle, device);

    if (device)
        setHandleBaseOriginalFileDevice_(handle, this);

    return device;
}

bool FileDevice::tryCloseDirectory(DirectoryHandle* handle)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    bool success = doCloseDirectory_(handle);
    if (success)
    {
        setHandleBaseFileDevice_(handle, nullptr);
        setHandleBaseOriginalFileDevice_(handle, nullptr);
    }

    return success;
}

bool FileDevice::tryReadDirectory(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    if (!handle)
    {
        SEAD_ASSERT_MSG(false, "handle is null");
        return false;
    }

    if (!isMatchDevice_(handle))
    {
        SEAD_ASSERT_MSG(false, "handle device miss match");
        return false;
    }

    u32 tmpReadNum = 0;
    bool success = doReadDirectory_(&tmpReadNum, handle, entry, num);

    if (readNum)
        *readNum = tmpReadNum;

    if (tmpReadNum > num)
    {
        SEAD_ASSERT_MSG(false, "buffer overflow");
        return false;
    }

    return success;
}

bool FileDevice::tryMakeDirectory(const SafeString& path, u32 permission)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    return doMakeDirectory_(path, permission);
}

bool FileDevice::tryMakeDirectoryWithParent(const SafeString& path, u32 permission)
{
    SEAD_ASSERT_MSG(hasPermission(), "Device permission error.");
    if (!hasPermission())
        return false;

    // TODO
    SEAD_ASSERT(false);
    SEAD_UNUSED(path);
    SEAD_UNUSED(permission);
    return false;
}

RawErrorCode FileDevice::getLastRawError() const
{
    return doGetLastRawError_();
}

bool FileDevice::isMatchDevice_(const HandleBase* handle) const
{
    return handle->getDevice() == this;
}

u8* FileDevice::doLoad_(LoadArg& arg)
{
    if (arg.buffer && arg.buffer_size == 0)
    {
        SEAD_WARNING("arg.buffer is specified, but arg.buffer_size is zero");
        return nullptr;
    }

    if (arg.buffer_size_alignment % cBufferMinAlignment != 0)
    {
        SEAD_WARNING("arg.buffer_size_alignment[%u] is not multipe of FileDevice::cBufferMinAlignment[%u]",
                     arg.buffer_size_alignment, cBufferMinAlignment);
        return nullptr;
    }

    FileHandle handle;
    if (!tryOpen(&handle, arg.path, FileOpenFlag::eReadOnly, arg.div_size))
        return nullptr;

    u32 bufferSize = arg.buffer_size;
    if (!arg.buffer || arg.check_read_whole)
    {
        u32 fileSize = 0;
        if (!tryGetFileSize(&fileSize, &handle))
            return nullptr;

        if (fileSize == 0)
        {
            SEAD_WARNING("fileSize is zero.[%s]", arg.path.cstr());
            return nullptr;
        }

        if (bufferSize != 0)
        {
            if (bufferSize < fileSize)
            {
                SEAD_WARNING("arg.buffer_size[%u] is smaller than file size[%u]", bufferSize, fileSize);
                return nullptr;
            }

            if (arg.buffer_size_alignment != 0 && bufferSize % arg.buffer_size_alignment != 0)
            {
                SEAD_WARNING("arg.buffer_size[%u] is not multipe of arg.buffer_size_alignment[%u]", bufferSize, arg.buffer_size_alignment);
                return nullptr;
            }
        }
        else
        {
            if (arg.buffer_size_alignment != 0)
                bufferSize = Mathi::roundUpN(fileSize, arg.buffer_size_alignment);
            else
                bufferSize = Mathi::roundUpPow2(fileSize, cBufferMinAlignment);
        }
    }

    u8* buffer = arg.buffer;
    bool needUnload = false;

    if (!buffer)
    {
        s32 alignment = Mathi::sign(arg.alignment) * Mathi::max(Mathi::abs(arg.alignment), cBufferMinAlignment);

        Heap* heap = arg.heap;
        if (!heap)
            heap = HeapMgr::instance()->getCurrentHeap();

        buffer = static_cast<u8*>(heap->tryAlloc(bufferSize, alignment));
        if (!buffer)
        {
            if (arg.enable_alloc_assert)
                SEAD_ASSERT_MSG(false, "alloc size[%u] failed in heap[%s] for file[%s]", bufferSize, heap->getName().cstr(), arg.path.cstr());

            return nullptr;
        }

        needUnload = true;
    }

    u32 readSize = 0;
    if (!tryRead(&readSize, &handle, buffer, bufferSize))
    {
        if (needUnload)
            delete[] buffer;

        return nullptr;
    }

    if (!tryClose(&handle))
    {
        if (needUnload)
            delete[] buffer;

        return nullptr;
    }

    arg.read_size = readSize;
    arg.roundup_size = bufferSize;
    arg.need_unload = needUnload;

    return buffer;
}

bool FileDevice::doSave_(SaveArg& arg)
{
    if (!arg.buffer)
    {
        SEAD_ASSERT_MSG(false, "arg.buffer must be set for save file[%s]", arg.path.cstr());
        return false;
    }

    FileHandle handle;
    if (!tryOpen(&handle, arg.path, FileOpenFlag::eWriteOnly, 0))
        return false;

    bool success = true;
    if (arg.buffer_size != 0)
        success = tryWrite(&arg.write_size, &handle, arg.buffer, arg.buffer_size);

    if (!tryClose(&handle))
        return false;

    return success;
}

void FileDevice::doTracePath_(const SafeString& path) const
{
    SEAD_PRINT("[%s] %s\n", mDriveName.cstr(), path.cstr());

    FixedSafeString<512> tmp;
    doResolvePath_(&tmp, path);

    SEAD_PRINT("  -> %s\n", tmp.cstr());
}

void FileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    out->copy(path);
}

HandleBuffer& FileDevice::getHandleBaseHandleBuffer_(HandleBase* handle) const
{
    return handle->mHandleBuffer;
}

void FileDevice::setHandleBaseFileDevice_(HandleBase* handle, FileDevice* device) const
{
    handle->mDevice = device;
}

void FileDevice::setHandleBaseOriginalFileDevice_(HandleBase* handle, FileDevice* device) const
{
    handle->mOriginalDevice = device;
}

void FileDevice::setFileHandleDivSize_(FileHandle* handle, u32 divSize) const
{
    handle->mDivSize = divSize;
}

bool FileHandle::close()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->close(this);
}

bool FileHandle::tryClose()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->tryClose(this);
}

bool FileHandle::flush()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->flush(this);
}

bool FileHandle::tryFlush()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->tryFlush(this);
}

u32 FileHandle::read(u8* buf, u32 size)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return 0;
    }

    return mDevice->read(this, buf, size);
}

bool FileHandle::tryRead(u32* readSize, u8* buf, u32 size)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->tryRead(readSize, this, buf, size);
}

u32 FileHandle::write(const u8* buf, u32 size)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return 0;
    }

    return mDevice->write(this, buf, size);
}

bool FileHandle::tryWrite(u32* writeSize, const u8* buf, u32 size)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->tryWrite(writeSize, this, buf, size);
}

bool FileHandle::seek(s32 offset, FileDevice::SeekOrigin origin)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->seek(this, offset, origin);
}

bool FileHandle::trySeek(s32 offset, FileDevice::SeekOrigin origin)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->trySeek(this, offset, origin);
}

u32 FileHandle::getCurrentSeekPos()
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return 0;
    }

    return mDevice->getCurrentSeekPos(this);
}

bool FileHandle::tryGetCurrentSeekPos(u32* pos)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->tryGetCurrentSeekPos(pos, this);
}

u32 FileHandle::getFileSize()
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return 0;
    }

    return mDevice->getFileSize(this);
}

bool FileHandle::tryGetFileSize(u32* size)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->tryGetFileSize(size, this);
}

bool DirectoryHandle::close()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->closeDirectory(this);
}

bool DirectoryHandle::tryClose()
{
    if (!mOriginalDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mOriginalDevice->tryCloseDirectory(this);
}

u32 DirectoryHandle::read(DirectoryEntry* entry, u32 num)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return 0;
    }

    return mDevice->readDirectory(this, entry, num);
}

bool DirectoryHandle::tryRead(u32* readNum, DirectoryEntry* entry, u32 num)
{
    if (!mDevice)
    {
        SEAD_ASSERT_MSG(false, "handle not opened");
        return false;
    }

    return mDevice->tryReadDirectory(readNum, this, entry, num);
}

} // namespace sead
