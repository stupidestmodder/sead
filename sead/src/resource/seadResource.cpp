#include <resource/seadResource.h>

#include <filedevice/seadFileDeviceMgr.h>
#include <math/seadMathCalcCommon.h>
#include <resource/seadDecompressor.h>
#include <stream/seadFileDeviceStream.h>
#include <stream/seadRamStream.h>

namespace sead {

Resource::Resource()
{
}

Resource::~Resource()
{
}

DirectResource::DirectResource()
    : Resource()
    , mRawData(nullptr)
    , mRawSize(0)
    , mBufferSize(0)
    , mSettingFlag()
{
}

DirectResource::~DirectResource()
{
    if (mSettingFlag.isOnBit(SettingFlag::eNeedDelete))
    {
        delete[] mRawData;
    }
}

void DirectResource::create(u8* data, u32 size, u32 bufferSize, bool needDelete, Heap* instanceHeap)
{
    if (mRawData)
    {
        SEAD_ASSERT_MSG(false, "read twice");
        return;
    }

    mRawData = data;
    mRawSize = size;
    mBufferSize = bufferSize;
    mSettingFlag.changeBit(SettingFlag::eNeedDelete, needDelete);
    doCreate_(data, size, instanceHeap);
}

IndirectResource::IndirectResource()
    : Resource()
{
}

IndirectResource::~IndirectResource()
{
}

void IndirectResource::create(ReadStream* stream, u32 size, Heap* instanceHeap)
{
    doCreate_(stream, size, instanceHeap);
}

ResourceFactory::~ResourceFactory()
{
    ResourceMgr* mgr = ResourceMgr::instance();
    if (mgr)
    {
        mgr->unregisterFactory(this);
        if (mgr->getDefaultFactory() == this)
        {
            mgr->setDefaultFactory(nullptr);
        }
    }
}

Resource* DirectResourceFactoryBase::tryCreate(const ResourceMgr::LoadArg& arg)
{
    DirectResource* res = newResource_(arg.instance_heap, arg.instance_alignment);
    if (!res)
    {
        return nullptr;
    }

    FileDevice::LoadArg fdArg;
    fdArg.path = arg.path;
    fdArg.buffer = arg.load_data_buffer;
    fdArg.buffer_size = arg.load_data_buffer_size;
    fdArg.heap = arg.load_data_heap;
    fdArg.div_size = arg.div_size;
    fdArg.enable_alloc_assert = arg.enable_alloc_assert;
    fdArg.buffer_size_alignment = arg.load_data_buffer_size_alignment;

    if (arg.load_data_alignment != 0)
    {
        fdArg.alignment = arg.load_data_alignment;
    }
    else
    {
        fdArg.alignment = res->getLoadDataAlignment() * Mathi::sign(arg.instance_alignment);
    }

    u8* data;
    if (arg.device)
    {
        data = arg.device->tryLoad(fdArg);
    }
    else
    {
        data = FileDeviceMgr::instance()->tryLoad(fdArg);
    }

    if (!data)
    {
        delete res;
        return nullptr;
    }

    res->create(data, fdArg.read_size, fdArg.roundup_size, fdArg.need_unload, arg.instance_heap);
    return res;
}

Resource* DirectResourceFactoryBase::tryCreateWithDecomp(const ResourceMgr::LoadArg& arg, Decompressor* decomp)
{
    DirectResource* res = newResource_(arg.instance_heap, arg.instance_alignment);
    if (!res)
    {
        return nullptr;
    }

    u32 size = 0;
    u32 bufferSize = 0;
    bool needDelete = false;

    u8* data = decomp->tryDecompFromDevice(arg, res, &size, &bufferSize, &needDelete);
    if (!data)
    {
        delete res;
        return nullptr;
    }

    res->create(data, size, bufferSize, needDelete, arg.instance_heap);
    return res;
}

Resource* DirectResourceFactoryBase::create(const ResourceMgr::CreateArg& arg)
{
    DirectResource* res = newResource_(arg.heap, arg.alignment);
    if (!res)
    {
        SEAD_ASSERT_MSG(false, "resource new failed.");
        return nullptr;
    }

    if (!PtrUtil::isAligned(arg.buffer, res->getLoadDataAlignment()))
    {
        SEAD_ASSERT_MSG(false, "buffer alignment invalid: %p, %d", arg.buffer, res->getLoadDataAlignment());
        delete res;
        return nullptr;
    }

    res->create(arg.buffer, arg.file_size, arg.buffer_size, arg.need_unload, arg.heap);
    return res;
}

Resource* IndirectResourceFactoryBase::tryCreate(const ResourceMgr::LoadArg& arg)
{
    IndirectResource* res = newResource_(arg.instance_heap, arg.instance_alignment);
    if (!res)
    {
        return nullptr;
    }

    FileHandle handle;

    FileDevice* ret;
    if (arg.device)
    {
        ret = arg.device->tryOpen(&handle, arg.path, FileDevice::FileOpenFlag::eReadOnly, 0);
    }
    else
    {
        ret = FileDeviceMgr::instance()->tryOpen(&handle, arg.path, FileDevice::FileOpenFlag::eReadOnly, 0);
    }

    if (!ret)
    {
        delete res;
        return nullptr;
    }

    FileDeviceReadStream stream(&handle, Stream::Modes::eBinary);
    res->create(&stream, handle.getFileSize(), arg.instance_heap);

    bool success = handle.tryClose();
    if (!success)
    {
        delete res;
        return nullptr;
    }

    return res;
}

Resource* IndirectResourceFactoryBase::tryCreateWithDecomp(const ResourceMgr::LoadArg& arg, Decompressor* decomp)
{
    IndirectResource* res = newResource_(arg.instance_heap, arg.instance_alignment);
    if (!res)
    {
        SEAD_ASSERT_MSG(false, "resource new failed.");
        return nullptr;
    }

    u32 size = 0;
    u32 bufferSize = 0;
    bool needDelete = false;

    u8* data = decomp->tryDecompFromDevice(arg, res, &size, &bufferSize, &needDelete);
    if (!data)
    {
        delete res;
        return nullptr;
    }

    RamReadStream stream(data, size, Stream::Modes::eBinary);
    res->create(&stream, size, arg.instance_heap);
    delete[] data;

    return res;
}

Resource* IndirectResourceFactoryBase::create(const ResourceMgr::CreateArg& arg)
{
    IndirectResource* res = newResource_(arg.heap, arg.alignment);
    if (!res)
    {
        SEAD_ASSERT_MSG(false, "resource new failed.");
        return nullptr;
    }

    RamReadStream stream(arg.buffer, arg.file_size, Stream::Modes::eBinary);
    res->create(&stream, arg.file_size, arg.heap);

    return res;
}

} // namespace sead
