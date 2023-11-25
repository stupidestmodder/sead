#pragma once

#include <prim/seadDelegateEventSlot.h>

namespace sead {

class PrintConfig
{
public:
    struct PrintEventArg
    {
        PrintEventArg(const char* str_, s32 len_)
            : str(str_)
            , len(len_)
        {
        }
        
        const char* str;
        s32 len;
    };

    using PrintEvent = DelegateEvent<const PrintEventArg&>;

public:
    static void registerCallback(PrintEvent::Slot& slot);
    static void unregisterCallback(PrintEvent::Slot& slot);
    
    static s32 getCallbackNum()
    {
        return sPrintEvent.getSlotLength();
    }

    static void registerFinalCallback(IDelegate1<const PrintEventArg&>* callback);
    static void execCallbacks(const PrintEventArg& arg);

private:
    static bool sIsPrintEventUsed;
    static PrintEvent sPrintEvent;
    static IDelegate1<const PrintEventArg&>* sFinalCallback;
};

} // namespace sead
