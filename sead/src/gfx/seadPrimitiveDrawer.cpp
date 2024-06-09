#include <gfx/seadPrimitiveDrawer.h>

#include <gfx/seadCamera.h>
#include <gfx/seadProjection.h>
#include <math/seadQuat.h>

#if SEAD_GFX_BACKEND == SEAD_GFX_GL
#include <gfx/gl/seadPrimitiveDrawMgrGL.h>
#endif

namespace sead {

PrimitiveDrawer::PrimitiveDrawer(DrawContext* drawContext)
    : mModelMtx(&Matrix34f::ident)
    , mViewMtx(&Matrix34f::ident)
    , mProjMtx(&Matrix44f::ident)
    , mDrawContext(drawContext)
{
}

PrimitiveDrawer::~PrimitiveDrawer()
{
}

void PrimitiveDrawer::prepareMgr(Heap* heap, const SafeString& shaderPath)
{
    PrimitiveDrawMgrBase* drawMgr = createDrawMgrInstance_(heap);
    drawMgr->prepareImpl(heap, shaderPath);
}

void PrimitiveDrawer::setCamera(const Camera* camera)
{
    mViewMtx = &camera->getViewMatrix();
}

void PrimitiveDrawer::setProjection(const Projection* projection)
{
    mProjMtx = &projection->getDeviceProjectionMatrix();
}

void PrimitiveDrawer::setModelMatrix(const Matrix34f* modelMtx)
{
    mModelMtx = modelMtx;
}

void PrimitiveDrawer::setDrawContext(DrawContext* drawContext)
{
    mDrawContext = drawContext;
}

void PrimitiveDrawer::begin()
{
    getDrawMgr_()->beginImpl(mDrawContext, *mViewMtx, *mProjMtx);
}

void PrimitiveDrawer::end()
{
    getDrawMgr_()->endImpl(mDrawContext);
}

void PrimitiveDrawer::drawQuad(const Color4f& colorL, const Color4f& colorR)
{
    getDrawMgr_()->drawQuadImpl(mDrawContext, *mModelMtx, colorL, colorR);
}

void PrimitiveDrawer::drawQuad(const Texture& texture, const Color4f& colorL, const Color4f& colorR, const Vector2f& uvSrc, const Vector2f& uvSize)
{
    getDrawMgr_()->drawQuadImpl(mDrawContext, *mModelMtx, texture, colorL, colorR, uvSrc, uvSize);
}

void PrimitiveDrawer::drawQuad(const QuadArg& arg)
{
    Matrix34f mtx;

    if (arg.isHorizontal())
        mtx.makeSRT(Vector3f(arg.getSize().y, arg.getSize().x, 1.0f), Vector3f(0.0f, 0.0f, Mathf::deg2rad(90.0f)), arg.getCenter());
    else
        mtx.makeST(Vector3f(arg.getSize().x, arg.getSize().y, 1.0f), arg.getCenter());

    Matrix34f ret;
    ret.setMul(*mModelMtx, mtx);

    getDrawMgr_()->drawQuadImpl(mDrawContext, ret, arg.getColor0(), arg.getColor1());
}

void PrimitiveDrawer::drawBox(const Color4f& colorL, const Color4f& colorR)
{
    getDrawMgr_()->drawBoxImpl(mDrawContext, *mModelMtx, colorL, colorR);
}

void PrimitiveDrawer::drawBox(const QuadArg& arg)
{
    Matrix34f mtx;

    if (arg.isHorizontal())
        mtx.makeSRT(Vector3f(arg.getSize().y, arg.getSize().x, 1.0f), Vector3f(0.0f, 0.0f, Mathf::deg2rad(90.0f)), arg.getCenter());
    else
        mtx.makeST(Vector3f(arg.getSize().x, arg.getSize().y, 1.0f), arg.getCenter());

    Matrix34f ret;
    ret.setMul(*mModelMtx, mtx);

    getDrawMgr_()->drawBoxImpl(mDrawContext, ret, arg.getColor0(), arg.getColor1());
}

void PrimitiveDrawer::drawLine(const Color4f& c0, const Color4f& c1)
{
    getDrawMgr_()->drawLineImpl(mDrawContext, *mModelMtx, c0, c1);
}

void PrimitiveDrawer::drawLine(const Vector3f& from, const Vector3f& to, const Color4f& c0, const Color4f& c1)
{
    Vector3f dir = to - from;

    Matrix34f nextMtx;
    nextMtx.makeS(dir.length(), 1.0f, 1.0f);

    dir.normalize();

    Quatf q;
    q.makeVectorRotation(Vector3f(1.0f, 0.0f, 0.0f), dir);

    Matrix34f qmat;
    qmat.fromQuat(q);

    Matrix34f ret;
    ret.setMul(qmat, nextMtx);

    dir = to - from;
    dir.multScalar(0.5f);
    dir += from;
    ret.setTranslation(dir);

    Matrix34f ret2;
    ret2.setMul(*mModelMtx, ret);

    getDrawMgr_()->drawLineImpl(mDrawContext, ret2, c0, c1);
}

void PrimitiveDrawer::drawLine(const Vector3f& from, const Vector3f& to, const Color4f& color)
{
    drawLine(from, to, color, color);
}

PrimitiveDrawMgrBase* PrimitiveDrawer::createDrawMgrInstance_(Heap* heap)
{
#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    return PrimitiveDrawMgrGL::createInstance(heap);
#endif
}

PrimitiveDrawMgrBase* PrimitiveDrawer::getDrawMgr_()
{
#if SEAD_GFX_BACKEND == SEAD_GFX_GL
    return PrimitiveDrawMgrGL::instance();
#endif
}

PrimitiveDrawer::QuadArg& PrimitiveDrawer::QuadArg::setCornerAndSize(const Vector3f& p, const Vector2f& size)
{
    mCenter.set(size.x * 0.5f + p.x, size.y * 0.5f + p.y, p.z);
    mSize = size;

    return *this;
}

PrimitiveDrawer::QuadArg& PrimitiveDrawer::QuadArg::setBoundBox(const BoundBox2f& box, f32 z)
{
    Vector2f c;
    box.getCenter(&c);

    mCenter.set(c.x, c.y, z);
    mSize.set(box.getSizeX(), box.getSizeY());

    return *this;
}

PrimitiveDrawer::QuadArg& PrimitiveDrawer::QuadArg::setColor(const Color4f& colorT, const Color4f& colorB)
{
    mHorizontal = false;

    mColor0 = colorT;
    mColor1 = colorB;

    return *this;
}

PrimitiveDrawer::QuadArg& PrimitiveDrawer::QuadArg::setColorHorizontal(const Color4f& colorL, const Color4f& colorR)
{
    mHorizontal = true;

    mColor0 = colorL;
    mColor1 = colorR;

    return *this;
}

} // namespace sead
