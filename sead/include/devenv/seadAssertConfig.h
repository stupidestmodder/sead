#pragma once

#include <prim/seadDelegateEventSlot.h>

namespace sead {

class AssertConfig
{
public:
    using AssertEvent = DelegateEvent<const char*>;

public:
    static void registerCallback(AssertEvent::Slot& slot);
    static void unregisterCallback(AssertEvent::Slot& slot);

    static s32 getCallbackNum()
    {
        return sAssertEvent.getSlotLength();
    }

    static void registerFinalCallback(IDelegate1<const char*>* callback);
    static void execCallbacks(const char* message);

private:
    static AssertEvent sAssertEvent;
    static IDelegate1<const char*>* sFinalCallback;
};

} // namespace sead
