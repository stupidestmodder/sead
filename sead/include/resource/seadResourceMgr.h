#pragma once

#include <container/seadTList.h>
#include <heap/seadDisposer.h>

namespace sead {

class Decompressor;
class ResourceFactory;

// TODO
class ResourceMgr
{
    SEAD_SINGLETON_DISPOSER(ResourceMgr);

private:
    using FactoryList = TList<ResourceFactory*>;
    using DecompressorList = TList<Decompressor*>;

public:
    ResourceMgr();
    ~ResourceMgr();

protected:
    FactoryList mFactoryList;
    DecompressorList mDecompList;
    ResourceFactory* mNullResourceFactory;
    ResourceFactory* mDefaultResourceFactory;
};

} // namespace sead
