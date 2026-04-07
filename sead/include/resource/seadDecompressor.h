#pragma once

#include <resource/seadResourceMgr.h>

namespace sead {

class Decompressor : public TListNode<Decompressor*>, IDisposer
{
public:
    Decompressor(const SafeString& defaultExt)
        : TListNode<Decompressor*>(this)
        , IDisposer()
        , mExt(defaultExt)
    {
    }

    ~Decompressor() override
    {
        ResourceMgr* mgr = ResourceMgr::instance();
        if (mgr)
        {
            mgr->unregisterDecompressor(this);
        }
    }

    virtual u8* tryDecompFromDevice(const ResourceMgr::LoadArg& arg, Resource* res, u32* outSize, u32* outBufferSize, bool* outNeedDelete) = 0;

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

} // namespace sead
