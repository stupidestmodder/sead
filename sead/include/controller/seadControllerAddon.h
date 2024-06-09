#pragma once

#include <container/seadListImpl.h>
#include <controller/seadControllerDefine.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class Controller;

class ControllerAddon
{
    SEAD_RTTI_BASE(ControllerAddon);

public:
    explicit ControllerAddon(Controller* controller)
        : mListNode()
        , mId(ControllerDefine::AddonId::eNull)
        , mController(controller)
    {
        SEAD_ASSERT(controller);
    }

    virtual ~ControllerAddon()
    {
    }

    virtual bool calc() = 0;

    ControllerDefine::AddonId getId() const { return mId; }

protected:
    ListNode mListNode;
    ControllerDefine::AddonId mId;
    Controller* mController;

    friend class Controller;
};

} // namespace sead
