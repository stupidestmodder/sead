#pragma once

#include <framework/seadGameFramework.h>

class AppFramework : public sead::GameFramework
{
    SEAD_RTTI_OVERRIDE(AppFramework, sead::GameFramework);

public:
    AppFramework();

    static void initialize(const InitializeArg& arg);

    sead::FrameBuffer* getMethodFrameBuffer(s32) const override
    {
        return nullptr;
    }

    f32 calcFps() override
    {
        return 0.0f;
    }

protected:
    sead::MethodTreeMgr* createMethodTreeMgr_(sead::Heap* heap) override;
};
