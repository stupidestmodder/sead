#pragma once

#include <gfx/seadPrimitiveDrawer.h>
#include <heap/seadDisposer.h>

namespace sead {

class Viewport;

// TODO
class PrimitiveRenderer
{
    SEAD_SINGLETON_DISPOSER(PrimitiveRenderer);

public:
    using QuadArg = PrimitiveDrawer::QuadArg;
    using UVArg = PrimitiveDrawer::UVArg;
    using CubeArg = PrimitiveDrawer::CubeArg;

public:
    PrimitiveRenderer();
    virtual ~PrimitiveRenderer();

    void prepareFromBinary(Heap*, const void*, u32);
    void prepare(Heap* heap, const SafeString& shaderPath);

    void setCamera(const Camera&);
    void setProjection(const Projection&);
    void setModelMatrix(const Matrix34f&);

    void begin();
    void end();

    void drawQuad(const Color4f& colorL, const Color4f& colorR);

    // TODO: Add draw methods

protected:
    PrimitiveDrawer mDrawer;
    Matrix34f mViewMtx;
    Matrix44f mProjMtx;
    Matrix34f mModelMtx;
};

} // namespace sead
