#pragma once

#include <prim/seadBitFlag.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>
#include <resource/seadResourceMgr.h>

namespace sead {

class Heap;
class ReadStream;

class Resource
{
    SEAD_RTTI_BASE(Resource);

public:
    Resource();
    virtual ~Resource();
};

class DirectResource : public Resource
{
    SEAD_RTTI_OVERRIDE(DirectResource, Resource);

    enum SettingFlag
    {
        eNeedDelete = 0
    };

public:
    DirectResource();
    ~DirectResource() override;

    virtual s32 getLoadDataAlignment() const
    {
        return 4;
    }

    void create(u8* data, u32 size, u32 bufferSize, bool needDelete, Heap* instanceHeap);

protected:
    virtual void doCreate_(u8* data, u32 size, Heap* instanceHeap)
    {
        SEAD_UNUSED(data);
        SEAD_UNUSED(size);
        SEAD_UNUSED(instanceHeap);
    }

protected:
    u8* mRawData;
    u32 mRawSize;
    u32 mBufferSize;
    BitFlag32 mSettingFlag;
};

class IndirectResource : public Resource
{
    SEAD_RTTI_OVERRIDE(IndirectResource, Resource);

public:
    IndirectResource();
    ~IndirectResource() override;

    void create(ReadStream* stream, u32 size, Heap* instanceHeap);

protected:
    virtual void doCreate_(ReadStream* stream, u32 size, Heap* instanceHeap)
    {
        SEAD_UNUSED(stream);
        SEAD_UNUSED(size);
        SEAD_UNUSED(instanceHeap);
    }
};

class ResourceFactory : public TListNode<ResourceFactory*>, IDisposer
{
    SEAD_RTTI_BASE(ResourceFactory);

public:
    ResourceFactory()
        : TListNode<ResourceFactory*>(this)
        , IDisposer()
        , mExt()
    {
    }

    ~ResourceFactory() override;

    virtual Resource* tryCreate(const ResourceMgr::LoadArg& arg) = 0;
    virtual Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& arg, Decompressor* decomp) = 0;
    virtual Resource* create(const ResourceMgr::CreateArg& arg) = 0;

    const SafeString& getExt() const
    {
        return mExt;
    }

    void setExt(const SafeString& ext)
    {
        mExt.copy(ext);
    }

protected:
    FixedSafeString<32> mExt;
};

class DirectResourceFactoryBase : public ResourceFactory
{
    SEAD_RTTI_OVERRIDE(DirectResourceFactoryBase, ResourceFactory);

public:
    DirectResourceFactoryBase()
        : ResourceFactory()
    {
    }

    ~DirectResourceFactoryBase() override
    {
    }

    Resource* tryCreate(const ResourceMgr::LoadArg& arg) override;
    Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& arg, Decompressor* decomp) override;
    Resource* create(const ResourceMgr::CreateArg& arg) override;

protected:
    virtual DirectResource* newResource_(Heap* heap, s32 alignment) = 0;
};

template <typename T>
class DirectResourceFactory : public DirectResourceFactoryBase
{
    SEAD_RTTI_OVERRIDE(DirectResourceFactory, DirectResourceFactoryBase);

public:
    DirectResourceFactory()
        : DirectResourceFactoryBase()
    {
    }

    ~DirectResourceFactory() override
    {
    }

protected:
    DirectResource* newResource_(Heap* heap, s32 alignment) override
    {
        return new(heap, alignment) T;
    }
};

class IndirectResourceFactoryBase : public ResourceFactory
{
    SEAD_RTTI_OVERRIDE(IndirectResourceFactoryBase, ResourceFactory);

public:
    IndirectResourceFactoryBase()
        : ResourceFactory()
    {
    }

    ~IndirectResourceFactoryBase() override
    {
    }

    Resource* tryCreate(const ResourceMgr::LoadArg& arg) override;
    Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& arg, Decompressor* decomp) override;
    Resource* create(const ResourceMgr::CreateArg& arg) override;

protected:
    virtual IndirectResource* newResource_(Heap* heap, s32 alignment) = 0;
};

template <typename T>
class IndirectResourceFactory : public IndirectResourceFactoryBase
{
    SEAD_RTTI_OVERRIDE(IndirectResourceFactory, IndirectResourceFactoryBase);

public:
    IndirectResourceFactory()
        : IndirectResourceFactoryBase()
    {
    }

    ~IndirectResourceFactory() override
    {
    }

protected:
    IndirectResource* newResource_(Heap* heap, s32 alignment) override
    {
        return new(heap, alignment) T;
    }
};

} // namespace sead
