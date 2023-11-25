#include <devenv/seadPrintConfig.h>

#include <basis/seadRawPrint.h>

namespace sead {

bool PrintConfig::sIsPrintEventUsed = false;
PrintConfig::PrintEvent PrintConfig::sPrintEvent;
IDelegate1<const PrintConfig::PrintEventArg&>* PrintConfig::sFinalCallback = nullptr;

void PrintConfig::registerCallback(PrintEvent::Slot& slot)
{
    sPrintEvent.connect(slot);
    sIsPrintEventUsed = true;
}

void PrintConfig::unregisterCallback(PrintEvent::Slot& slot)
{
    sPrintEvent.disconnect(slot);
}

void PrintConfig::registerFinalCallback(IDelegate1<const PrintEventArg&>* callback)
{
    sFinalCallback = callback;
}

void PrintConfig::execCallbacks(const PrintEventArg& arg)
{
    if (sIsPrintEventUsed)
        sPrintEvent.fire(arg);

    if (sFinalCallback)
        sFinalCallback->invoke(arg);
    else
        system::PrintStringImpl(arg.str, arg.len);
}

} // namespace sead
