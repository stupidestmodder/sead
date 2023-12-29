#pragma once

#include <framework/seadFramework.h>

namespace sead {

class ConsoleFrameworkWin : public Framework
{
    SEAD_RTTI_OVERRIDE(ConsoleFrameworkWin, Framework);

public:
    ConsoleFrameworkWin();
    ~ConsoleFrameworkWin() override;

    static void initialize(const InitializeArg& arg);

    FrameBuffer* getMethodFrameBuffer(s32) const override { return nullptr; }
    bool setProcessPriority(ProcessPriority priority) override;

protected:
    void runImpl_() override;
    MethodTreeMgr* createMethodTreeMgr_(Heap* heap) override;
};

} // namespace sead
