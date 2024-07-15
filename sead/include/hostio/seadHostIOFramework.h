#pragma once

#include <prim/seadSafeString.h>

namespace sead {

class Heap;

namespace hostio {

enum class Command;
class Context;
class Config;
class Reflexible;

class TmpContext
{
public:
    TmpContext(Command command);
    ~TmpContext();

    operator Context*()
    {
        return mContext;
    }

    Context* operator->()
    {
        return mContext;
    }

private:
    Context* mContext;
};

void Init(Heap* heap, const Config* argConfig);
void Finalize();
void MessageLoop();

void SetRootNode(const SafeString&, Reflexible*, const SafeString&);

} } // namespace sead::hostio
