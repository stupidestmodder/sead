#pragma once

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadSafeString.h>

namespace sead {

class Decompressor;
class FileDevice;
class Resource;
class ResourceFactory;

class ResourcePtr
{
public:
    ResourcePtr(Resource* ptr)
        : mPtr(ptr)
    {
    }

    operator Resource*() { return mPtr; }
    operator const Resource*() const { return mPtr; }

    Resource& operator*() { return *mPtr; }
    Resource* operator->() { return mPtr; }

    const Resource& operator*() const { return *mPtr; }
    const Resource* operator->() const { return mPtr; }

    bool isNull() const
    {
        return mPtr == nullptr;
    }

private:
    Resource* mPtr;
};

class ResourceMgr
{
    SEAD_SINGLETON_DISPOSER(ResourceMgr);

public:
    struct LoadArg
    {
        LoadArg();

        SafeString path;
        Heap* instance_heap;
        Heap* load_data_heap;
        s32 instance_alignment;
        s32 load_data_alignment;
        u8* load_data_buffer;
        u32 load_data_buffer_size;
        s32 load_data_buffer_size_alignment;
        ResourceFactory* factory;
        FileDevice* device;
        u32 div_size;
        bool enable_alloc_assert;
    };

    struct CreateArg
    {
        CreateArg();

        u8* buffer;
        u32 file_size;
        u32 buffer_size;
        bool need_unload;
        ResourceFactory* factory;
        SafeString ext;
        Heap* heap;
        s32 alignment;
    };

private:
    using FactoryList = TList<ResourceFactory*>;
    using DecompressorList = TList<Decompressor*>;

public:
    ResourceMgr();
    ~ResourceMgr();

    ResourcePtr loadWithoutDecomp(const LoadArg& arg)
    {
        ResourcePtr ret = tryLoadWithoutDecomp(arg);
        SEAD_ASSERT_MSG(!ret.isNull(), "loadWithoutDecomp failed: %s", arg.path.cstr());
        return ret;
    }

    ResourcePtr tryLoadWithoutDecomp(const LoadArg& arg);

    ResourcePtr load(const LoadArg& arg, const SafeString& convertExt, Decompressor* decomp)
    {
        ResourcePtr ret = tryLoad(arg, convertExt, decomp);
        SEAD_ASSERT_MSG(!ret.isNull(), "load failed: %s", arg.path.cstr());
        return ret;
    }

    ResourcePtr tryLoad(const LoadArg& arg, const SafeString& convertExt, Decompressor* decomp);

    void unload(Resource* resource);

    void registerFactory(ResourceFactory* factory, const SafeString& extName);
    void unregisterFactory(ResourceFactory* factory);
    ResourceFactory* findFactory(const SafeString& ext);

    ResourceFactory* getDefaultFactory() const { return mDefaultResourceFactory; }
    ResourceFactory* setDefaultFactory(ResourceFactory* factory);

    void registerDecompressor(Decompressor* decomp, const SafeString& extName);
    void unregisterDecompressor(Decompressor* decomp);
    Decompressor* findDecompressor(const SafeString& ext);

    // void postCreate();
    ResourcePtr create(const CreateArg& arg);

protected:
    FactoryList mFactoryList;
    DecompressorList mDecompList;
    ResourceFactory* mNullResourceFactory;
    ResourceFactory* mDefaultResourceFactory;
};

} // namespace sead
