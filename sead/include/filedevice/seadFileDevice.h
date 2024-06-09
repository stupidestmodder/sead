#pragma once

#include <basis/seadWarning.h>
#include <container/seadSafeArray.h>
#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>

namespace sead {

struct DirectoryEntry;
class DirectoryHandle;
class FileHandle;
class HandleBase;

using HandleBuffer = SafeArray<u8, 32>;

using RawErrorCode = s32;

class FileDevice : public TListNode<FileDevice*>, public IDisposer
{
    SEAD_RTTI_BASE(FileDevice);

public:
    enum class SeekOrigin
    {
        eBegin = 0,
        eCurrent,
        eEnd
    };

    enum class FileOpenFlag
    {
        eReadOnly = 0,
        eWriteOnly,
        eReadWrite,
        eCreate
    };

    struct LoadArg
    {
        LoadArg()
            : path()
            , buffer(nullptr)
            , buffer_size(0)
            , heap(nullptr)
            , alignment(0)
            , buffer_size_alignment(0)
            , div_size(0)
            , enable_alloc_assert(true)
            , check_read_whole(true)
            , read_size(0)
            , roundup_size(0)
            , need_unload(false)
        {
        }

        LoadArg(const LoadArg&) = default;

        SafeString path;
        u8* buffer;
        u32 buffer_size;
        Heap* heap;
        s32 alignment;
        s32 buffer_size_alignment;
        u32 div_size;
        bool enable_alloc_assert;
        bool check_read_whole;
        u32 read_size;
        u32 roundup_size;
        bool need_unload;
    };

    struct SaveArg
    {
        SaveArg()
            : path()
            , buffer(nullptr)
            , buffer_size(0)
            , write_size(0)
        {
        }

        SaveArg(const SaveArg&) = default;

        SafeString path;
        const u8* buffer;
        u32 buffer_size;
        u32 write_size;
    };

    static const s32 cDriveNameBufferSize = 32;

#ifdef SEAD_PLATFORM_WINDOWS
    static const s32 cBufferMinAlignment = 32;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

public:
    explicit FileDevice(const SafeString& defaultDriveName)
        : TListNode<FileDevice*>(this)
        , IDisposer()
        , mDriveName()
        , mPermission(true)
    {
#ifdef SEAD_DEBUG
        if (defaultDriveName.include(':'))
            SEAD_WARNING("drive name should not include ':'. (in %s)", defaultDriveName.cstr());
#endif // SEAD_DEBUG

        mDriveName.copy(defaultDriveName);
    }

    ~FileDevice() override;

    const SafeString& getDriveName() const
    {
        return mDriveName;
    }

    void setDriveName(const SafeString& name)
    {
#ifdef SEAD_DEBUG
        if (name.include(':'))
            SEAD_WARNING("drive name should not include ':'. (in %s)", name.cstr());
#endif // SEAD_DEBUG

        mDriveName.copy(name);
    }

    void setPermission(bool permission)
    {
        mPermission = permission;
    }

    bool hasPermission() const
    {
        return mPermission;
    }

    bool isAvailable() const;

    u8* load(LoadArg& arg)
    {
        u8* ret = tryLoad(arg);
        SEAD_ASSERT_MSG(ret, "load failed. [%s]", arg.path.cstr());
        return ret;
    }

    u8* tryLoad(LoadArg& arg);

    // TODO
    u8* load(const SafeString&, Heap* heap);
    u8* tryLoad(const SafeString&, Heap* heap);

    void unload(u8* data)
    {
        SEAD_ASSERT(data);
        delete data;
    }

    bool save(SaveArg& arg)
    {
        bool success = trySave(arg);
        SEAD_ASSERT_MSG(success, "save failed. [%s]", arg.path.cstr());
        return success;
    }

    bool trySave(SaveArg& arg);

    // TODO
    bool save(const SafeString&, const u8*, u32);
    bool trySave(const SafeString&, const u8*, u32);

    FileDevice* open(FileHandle* handle, const SafeString& filename, FileOpenFlag flag, u32 divSize)
    {
        FileDevice* ret = tryOpen(handle, filename, flag, divSize);
        SEAD_ASSERT_MSG(ret, "open failed. [%s]", filename.cstr());
        return ret;
    }

    FileDevice* tryOpen(FileHandle* handle, const SafeString& filename, FileOpenFlag flag, u32 divSize);

    bool close(FileHandle* handle)
    {
        bool success = tryClose(handle);
        SEAD_ASSERT_MSG(success, "file close error");
        return success;
    }

    bool tryClose(FileHandle* handle);

    bool flush(FileHandle* handle)
    {
        bool success = tryFlush(handle);
        SEAD_ASSERT_MSG(success, "file flush error");
        return success;
    }

    bool tryFlush(FileHandle* handle);

    bool remove(const SafeString& path)
    {
        bool success = tryRemove(path);
        SEAD_ASSERT_MSG(success, "file remove error");
        return success;
    }

    bool tryRemove(const SafeString& path);

    u32 read(FileHandle* handle, u8* buf, u32 size)
    {
        u32 readSize = 0;
        bool success = tryRead(&readSize, handle, buf, size);
        SEAD_ASSERT_MSG(success, "file read error");
        return readSize;
    }

    bool tryRead(u32* readSize, FileHandle* handle, u8* buf, u32 size);

    u32 write(FileHandle* handle, const u8* buf, u32 size)
    {
        u32 writeSize = 0;
        bool success = tryWrite(&writeSize, handle, buf, size);
        SEAD_ASSERT_MSG(success, "file write error");
        return writeSize;
    }

    bool tryWrite(u32* writeSize, FileHandle* handle, const u8* buf, u32 size);

    bool seek(FileHandle* handle, s32 offset, SeekOrigin origin)
    {
        bool success = trySeek(handle, offset, origin);
        SEAD_ASSERT_MSG(success, "file seek error");
        return success;
    }

    bool trySeek(FileHandle* handle, s32 offset, SeekOrigin origin);

    u32 getCurrentSeekPos(FileHandle* handle)
    {
        u32 pos = 0;
        bool success = tryGetCurrentSeekPos(&pos, handle);
        SEAD_ASSERT_MSG(success, "getCurrentSeekPos error");
        return pos;
    }

    bool tryGetCurrentSeekPos(u32* pos, FileHandle* handle);

    u32 getFileSize(const SafeString& path)
    {
        u32 size = 0;
        bool success = tryGetFileSize(&size, path);
        SEAD_ASSERT_MSG(success, "getFileSize error");
        return size;
    }

    bool tryGetFileSize(u32* size, const SafeString& path);

    u32 getFileSize(FileHandle* handle)
    {
        u32 size = 0;
        bool success = tryGetFileSize(&size, handle);
        SEAD_ASSERT_MSG(success, "getFileSize error");
        return size;
    }

    bool tryGetFileSize(u32* size, FileHandle* handle);

    bool isExistFile(const SafeString& path)
    {
        bool isExist = false;
        bool success = tryIsExistFile(&isExist, path);
        SEAD_ASSERT_MSG(success, "isExistFile failed");
        return isExist;
    }

    bool tryIsExistFile(bool* isExist, const SafeString& path);

    bool isExistDirectory(const SafeString& path)
    {
        bool isExist = false;
        bool success = tryIsExistDirectory(&isExist, path);
        SEAD_ASSERT_MSG(success, "isExistDirectory failed");
        return isExist;
    }

    bool tryIsExistDirectory(bool* isExist, const SafeString& path);

    FileDevice* openDirectory(DirectoryHandle* handle, const SafeString& dirname)
    {
        FileDevice* ret = tryOpenDirectory(handle, dirname);
        SEAD_ASSERT_MSG(ret, "open directory failed. [%s]", dirname.cstr());
        return ret;
    }

    FileDevice* tryOpenDirectory(DirectoryHandle* handle, const SafeString& dirname);

    bool closeDirectory(DirectoryHandle* handle)
    {
        bool success = tryCloseDirectory(handle);
        SEAD_ASSERT_MSG(success, "directory close error");
        return success;
    }

    bool tryCloseDirectory(DirectoryHandle* handle);

    u32 readDirectory(DirectoryHandle* handle, DirectoryEntry* entry, u32 num)
    {
        u32 readNum = 0;
        bool success = tryReadDirectory(&readNum, handle, entry, num);
        SEAD_ASSERT_MSG(success, "readDirectory failed");
        return readNum;
    }

    bool tryReadDirectory(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num);

    // TODO
    bool makeDirectory(const SafeString& path, u32 permission);

    bool tryMakeDirectory(const SafeString& path, u32 permission);

    // TODO
    bool makeDirectoryWithParent(const SafeString& path, u32 permission);

    // TODO
    bool tryMakeDirectoryWithParent(const SafeString& path, u32 permission);

    RawErrorCode getLastRawError() const;

    virtual void traceFilePath(const SafeString& path) const
    {
        doTracePath_(path);
    }

    virtual void traceDirectoryPath(const SafeString& path) const
    {
        doTracePath_(path);
    }

    virtual void resolveFilePath(BufferedSafeString* out, const SafeString& path) const
    {
        doResolvePath_(out, path);
    }

    virtual void resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
    {
        doResolvePath_(out, path);
    }

    virtual bool isMatchDevice_(const HandleBase* handle) const;

protected:
    virtual bool doIsAvailable_() const = 0;
    virtual u8* doLoad_(LoadArg& arg);
    virtual bool doSave_(SaveArg& arg);
    virtual FileDevice* doOpen_(FileHandle* handle, const SafeString& filename, FileOpenFlag flag) = 0;
    virtual bool doClose_(FileHandle* handle) = 0;
    virtual bool doFlush_(FileHandle* handle) = 0;
    virtual bool doRemove_(const SafeString& path) = 0;
    virtual bool doRead_(u32* readSize, FileHandle* handle, u8* buf, u32 size) = 0;
    virtual bool doWrite_(u32* writeSize, FileHandle* handle, const u8* buf, u32 size) = 0;
    virtual bool doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin) = 0;
    virtual bool doGetCurrentSeekPos_(u32* pos, FileHandle* handle) = 0;
    virtual bool doGetFileSize_(u32* size, const SafeString& path) = 0;
    virtual bool doGetFileSize_(u32* size, FileHandle* handle) = 0;
    virtual bool doIsExistFile_(bool* isExist, const SafeString& path) = 0;
    virtual bool doIsExistDirectory_(bool* isExist, const SafeString& path) = 0;
    virtual FileDevice* doOpenDirectory_(DirectoryHandle* handle, const SafeString& dirname) = 0;
    virtual bool doCloseDirectory_(DirectoryHandle* handle) = 0;
    virtual bool doReadDirectory_(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num) = 0;
    virtual bool doMakeDirectory_(const SafeString& path, u32 permission) = 0;
    virtual RawErrorCode doGetLastRawError_() const = 0;
    virtual void doTracePath_(const SafeString& path) const;
    virtual void doResolvePath_(BufferedSafeString* out, const SafeString& path) const;

    HandleBuffer& getHandleBaseHandleBuffer_(HandleBase* handle) const;
    void setHandleBaseFileDevice_(HandleBase* handle, FileDevice* device) const;
    void setHandleBaseOriginalFileDevice_(HandleBase* handle, FileDevice* device) const;
    void setFileHandleDivSize_(FileHandle* handle, u32 divSize) const;

protected:
    FixedSafeString<cDriveNameBufferSize> mDriveName;
    bool mPermission;
};

class HandleBase
{
    SEAD_NO_COPY(HandleBase);

public:
    HandleBase()
        : mDevice(nullptr)
        , mOriginalDevice(nullptr)
        , mHandleBuffer()
    {
    }

    virtual ~HandleBase()
    {
    }

    FileDevice* getDevice() const
    {
        return mDevice;
    }

    FileDevice* getOriginalDevice() const
    {
        return mOriginalDevice;
    }

    bool isOpen() const
    {
        return mDevice != nullptr;
    }

protected:
    FileDevice* mDevice;
    FileDevice* mOriginalDevice;
    HandleBuffer mHandleBuffer;

    friend class FileDevice;
};

class FileHandle : public HandleBase
{
public:
    FileHandle()
        : HandleBase()
        , mDivSize(0)
    {
    }

    ~FileHandle() override
    {
        if (mOriginalDevice)
            mOriginalDevice->tryClose(this);
    }

    u32 getDivSize() const
    {
        return mDivSize;
    }

    bool close();
    bool tryClose();

    bool flush();
    bool tryFlush();

    u32 read(u8* buf, u32 size);
    bool tryRead(u32* readSize, u8* buf, u32 size);

    u32 write(const u8* buf, u32 size);
    bool tryWrite(u32* writeSize, const u8* buf, u32 size);

    bool seek(s32 offset, FileDevice::SeekOrigin origin);
    bool trySeek(s32 offset, FileDevice::SeekOrigin origin);

    u32 getCurrentSeekPos();
    bool tryGetCurrentSeekPos(u32* pos);

    u32 getFileSize();
    bool tryGetFileSize(u32* size);

protected:
    u32 mDivSize;

    friend class FileDevice;
};

class DirectoryHandle : public HandleBase
{
public:
    DirectoryHandle()
        : HandleBase()
    {
    }

    ~DirectoryHandle() override
    {
        if (mOriginalDevice)
            mOriginalDevice->tryCloseDirectory(this);
    }

    bool close();
    bool tryClose();

    u32 read(DirectoryEntry* entry, u32 num);
    bool tryRead(u32* readNum, DirectoryEntry* entry, u32 num);
};

struct DirectoryEntry
{
public:
    DirectoryEntry()
        : name()
        , is_directory(false)
    {
    }

    FixedSafeString<256> name;
    bool is_directory;
};

} // namespace sead
