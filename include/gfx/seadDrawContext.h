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
#if defined(SEAD_USE_GL)
    // Nothing
#else
#error "Unsupported GFX backend"
#endif
};

} // namespace sead
