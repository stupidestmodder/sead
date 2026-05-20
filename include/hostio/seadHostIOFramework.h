#pragma once

#include <prim/seadSafeString.h>

namespace sead {

class Heap;

namespace hostio {

enum class Command;
class Context;
class Config;
class Node;
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
bool IsInitialized();

void MessageLoop();

void AddNode(Node* parent, const SafeString& name, Node* node, const SafeString& meta);
void SetRootNode(const SafeString& name, Reflexible* node, const SafeString& meta);

} } // namespace sead::hostio
