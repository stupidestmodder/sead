#include <resource/seadResourceMgr.h>

#include <filedevice/seadPath.h>
#include <heap/seadHeapMgr.h>
#include <resource/seadDecompressor.h>
#include <resource/seadResource.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(ResourceMgr);

ResourceMgr::ResourceMgr()
    : mFactoryList()
    , mDecompList()
    , mNullResourceFactory(nullptr)
    , mDefaultResourceFactory(nullptr)
{
    HeapMgr* heapMgr = HeapMgr::instance();
    if (!heapMgr)
    {
        SEAD_ASSERT_MSG(false, "ResourceMgr need HeapMgr");
        return;
    }

    Heap* heap = heapMgr->findContainHeap(this);

    mNullResourceFactory = new(heap) DirectResourceFactory<DirectResource>();
    registerFactory(mNullResourceFactory, "");
}

ResourceMgr::~ResourceMgr()
{
    if (mNullResourceFactory)
    {
        delete mNullResourceFactory;
        mNullResourceFactory = nullptr;
    }
}

ResourcePtr ResourceMgr::tryLoadWithoutDecomp(const LoadArg& arg)
{
    ResourceFactory* factory;
    if (arg.factory)
    {
        factory = arg.factory;
    }
    else
    {
        FixedSafeString<32> ext;
        bool success = Path::getExt(&ext, arg.path);
        if (success)
        {
            factory = findFactory(ext);
            SEAD_ASSERT(factory);
        }
        else
        {
            factory = mNullResourceFactory;
        }
    }

    return factory->tryCreate(arg);
}

ResourcePtr ResourceMgr::tryLoad(const LoadArg& arg, const SafeString& convertExt, Decompressor* decomp)
{
    SafeString ext;
    FixedSafeString<32> extBuf;
    ResourceFactory* factory;

    if (!decomp)
    {
        bool success = Path::getExt(&extBuf, arg.path);
        if (!success)
        {
            SEAD_ASSERT_MSG(false, "no file extention");
            return nullptr;
        }

        decomp = findDecompressor(extBuf);
    }

    if (decomp)
    {
        ext = convertExt;
    }
    else
    {
        ext = extBuf;
    }

    if (arg.factory)
    {
        factory = arg.factory;
    }
    else
    {
        factory = findFactory(ext);
        SEAD_ASSERT(factory);
    }

    if (decomp)
    {
        return factory->tryCreateWithDecomp(arg, decomp);
    }
    else
    {
        return factory->tryCreate(arg);
    }
}

void ResourceMgr::unload(Resource* resource)
{
    delete resource;
}

void ResourceMgr::registerFactory(ResourceFactory* factory, const SafeString& extName)
{
    factory->setExt(extName);
    mFactoryList.pushBack(factory);
}

void ResourceMgr::unregisterFactory(ResourceFactory* factory)
{
    mFactoryList.erase(factory);
}

ResourceFactory* ResourceMgr::findFactory(const SafeString& ext)
{
    for (ResourceFactory* factory : mFactoryList)
    {
        if (factory->getExt() == ext)
        {
            return factory;
        }
    }

    return mNullResourceFactory;
}

ResourceFactory* ResourceMgr::setDefaultFactory(ResourceFactory* factory)
{
    ResourceFactory* old = mDefaultResourceFactory;
    if (!factory)
    {
        factory = mNullResourceFactory;
    }

    mDefaultResourceFactory = factory;
    registerFactory(factory, "");

    return old;
}

void ResourceMgr::registerDecompressor(Decompressor* decomp, const SafeString& extName)
{
    if (extName != SafeString::cEmptyString)
    {
        decomp->setExt(extName);
    }

    mDecompList.pushBack(decomp);
}

void ResourceMgr::unregisterDecompressor(Decompressor* decomp)
{
    mDecompList.erase(decomp);
}

Decompressor* ResourceMgr::findDecompressor(const SafeString& ext)
{
    for (Decompressor* decomp : mDecompList)
    {
        if (decomp->getExt() == ext)
        {
            return decomp;
        }
    }

    return nullptr;
}

ResourcePtr ResourceMgr::create(const CreateArg& arg)
{
    if (!arg.buffer)
    {
        SEAD_ASSERT_MSG(false, "buffer null");
        return nullptr;
    }
    else if (arg.file_size == 0)
    {
        SEAD_ASSERT_MSG(false, "file_size is 0");
        return nullptr;
    }
    else if (arg.buffer_size == 0)
    {
        SEAD_ASSERT_MSG(false, "buffer_size is 0");
        return nullptr;
    }

    ResourceFactory* factory = arg.factory;
    if (factory)
    {
        return factory->create(arg);
    }

    factory = findFactory(arg.ext);
    if (factory)
    {
        return factory->create(arg);
    }

    SEAD_ASSERT_MSG(false, "factory not found: %s", arg.ext.cstr());
    return nullptr;
}

} // namespace sead
