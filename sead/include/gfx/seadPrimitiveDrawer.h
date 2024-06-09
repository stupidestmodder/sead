#pragma once

#include <gfx/seadColor.h>
#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

namespace sead {

class Camera;
class DrawContext;
class PrimitiveDrawMgrBase;
class Projection;
class Texture;

// TODO
class PrimitiveDrawer
{
public:
    class QuadArg
    {
    public:
        QuadArg()
            : mCenter(Vector3f::zero)
            , mSize(Vector3f::ones)
            , mColor0(Color4f::cWhite)
            , mColor1(Color4f::cWhite)
            , mHorizontal(false)
        {
        }

        QuadArg& setCenter(const Vector3f& p) { mCenter = p; return *this; }
        QuadArg& setCenter(f32 x, f32 y, f32 z) { mCenter.set(x, y, z); return *this; }
        QuadArg& setSize(const Vector2f& size) { mSize = size; return *this; }
        QuadArg& setSize(f32 x, f32 y) { mSize.set(x, y); return *this; }
        QuadArg& setCornerAndSize(const Vector3f& p, const Vector2f& size);
        QuadArg& setCornerAndSize(f32 x, f32 y, f32 z, const Vector2f& size) { return setCornerAndSize(Vector3f(x, y, z), size); }
        QuadArg& setCornerAndSize(const Vector3f& p, f32 w, f32 h) { return setCornerAndSize(p, Vector2f(w, h)); }
        QuadArg& setCornerAndSize(f32 x, f32 y, f32 z, f32 w, f32 h) { return setCornerAndSize(Vector3f(x, y, z), Vector2f(w, h)); }
        QuadArg& setBoundBox(const BoundBox2f& box, f32 z);
        QuadArg& setColor(const Color4f& colorT, const Color4f& colorB);
        QuadArg& setColor(const Color4f& color) { return setColor(color, color); }
        QuadArg& setColorHorizontal(const Color4f& colorL, const Color4f& colorR);

        const Vector3f& getCenter() const { return mCenter; }
        const Vector2f& getSize() const { return mSize; }
        const Color4f& getColor0() const { return mColor0; }
        const Color4f& getColor1() const { return mColor1; }
        bool isHorizontal() const { return mHorizontal; }

    private:
        Vector3f mCenter;
        Vector2f mSize;
        Color4f mColor0;
        Color4f mColor1;
        bool mHorizontal;
    };

    class UVArg
    {
    };

    class CubeArg
    {
    };

public:
    explicit PrimitiveDrawer(DrawContext* drawContext);
    virtual ~PrimitiveDrawer();

    static void prepareMgrFromBinary(Heap*, const void*, u32);
    static void prepareMgr(Heap* heap, const SafeString& shaderPath);

    void setCamera(const Camera* camera);
    void setCameraViewMatrix(const Matrix34f*);
    void setProjection(const Projection* projection);
    void setProjectionMatrix(const Matrix44f*);
    void setModelMatrix(const Matrix34f* modelMtx);
    void setDrawContext(DrawContext* drawContext);

    void begin();
    void end();

    void drawQuad(const Color4f& colorL, const Color4f& colorR);
    void drawQuad(const Texture& texture, const Color4f& colorL, const Color4f& colorR, const Vector2f& uvSrc = Vector2f::zero, const Vector2f& uvSize = Vector2f::ones);
    void drawQuad(const QuadArg& arg);
    void drawQuad(const Texture&, const QuadArg&, const UVArg&);
    void drawBox(const Color4f& colorL, const Color4f& colorR);
    void drawBox(const QuadArg& arg);
    void drawCube(const Color4f&, const Color4f&);
    void drawCube(const CubeArg&);
    void drawWireCube(const Color4f&, const Color4f&);
    void drawWireCube(const CubeArg&);
    void drawLine(const Color4f& c0, const Color4f& c1);
    void drawLine(const Vector3f& from, const Vector3f& to, const Color4f& c0, const Color4f& c1);
    void drawLine(const Vector3f& from, const Vector3f& to, const Color4f& color);

    // TODO: Add draw methods

protected:
    static PrimitiveDrawMgrBase* createDrawMgrInstance_(Heap* heap);
    static PrimitiveDrawMgrBase* getDrawMgr_();

protected:
    const Matrix34f* mModelMtx;
    const Matrix34f* mViewMtx;
    const Matrix44f* mProjMtx;
    DrawContext* mDrawContext;
};

} // namespace sead
