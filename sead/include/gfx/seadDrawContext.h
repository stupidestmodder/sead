#pragma once

#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class Framework;

// TODO
class DrawContext
{
    SEAD_RTTI_BASE(DrawContext);

public:
    DrawContext() {}
    virtual ~DrawContext() {}

    void setDefaultCommandBufferFromFramework(Framework*) {}

protected:
#if SEAD_GFX_BACKEND == GL_GFX_GL
#else
#error "Unsupported GFX backend"
#endif
};

} // namespace sead
