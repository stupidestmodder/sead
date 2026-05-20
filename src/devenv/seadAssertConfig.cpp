#include <devenv/seadAssertConfig.h>

namespace sead {

AssertConfig::AssertEvent AssertConfig::sAssertEvent;
IDelegate1<const char*>* AssertConfig::sFinalCallback = nullptr;

void AssertConfig::registerCallback(AssertEvent::Slot& slot)
{
    sAssertEvent.connect(slot);
}

void AssertConfig::unregisterCallback(AssertEvent::Slot& slot)
{
    sAssertEvent.disconnect(slot);
}

void AssertConfig::registerFinalCallback(IDelegate1<const char*>* callback)
{
    sFinalCallback = callback;
}

void AssertConfig::execCallbacks(const char* message)
{
    sAssertEvent.fire(message);

    if (sFinalCallback)
        sFinalCallback->invoke(message);
}

} // namespace sead
