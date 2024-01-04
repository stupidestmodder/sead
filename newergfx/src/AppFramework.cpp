#include "AppFramework.h"

#include <framework/seadSingleScreenMethodTreeMgr.h>

AppFramework::AppFramework()
    : sead::GameFramework()
{
}

void AppFramework::initialize(const InitializeArg& arg)
{
    sead::GameFramework::initialize(arg);
}

sead::MethodTreeMgr* AppFramework::createMethodTreeMgr_(sead::Heap* heap)
{
    return new(heap) sead::SingleScreenMethodTreeMgr();
}
