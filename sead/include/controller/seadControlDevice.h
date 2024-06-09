#pragma once

#include <container/seadListImpl.h>
#include <controller/seadControllerDefine.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class ControllerMgr;

class ControlDevice
{
    SEAD_RTTI_BASE(ControlDevice);

public:
    explicit ControlDevice(ControllerMgr* mgr)
        : mListNode()
        , mId(ControllerDefine::DeviceId::eNull)
        , mMgr(mgr)
    {
    }

    virtual ~ControlDevice()
    {
    }

    virtual void calc() = 0;

    ControllerDefine::DeviceId getId() const { return mId; }

protected:
    ListNode mListNode;
    ControllerDefine::DeviceId mId;
    ControllerMgr* mMgr;

    friend class ControllerMgr;
};

} // namespace sead
