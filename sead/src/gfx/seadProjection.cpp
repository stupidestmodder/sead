#include <gfx/seadProjection.h>

#include <gfx/seadCamera.h>
#include <gfx/seadViewport.h>

namespace sead {

Projection::Projection()
    : mDirty(true)
    , mDeviceDirty(true)
    , mMatrix()
    , mDeviceMatrix()
    , mDevicePosture(Graphics::getDefaultDevicePosture())
    , mDeviceZScale(Graphics::getDefaultDeviceZScale())
    , mDeviceZOffset(Graphics::getDefaultDeviceZOffset())
{
}

Projection::~Projection()
{
}

const Matrix44f& Projection::getProjectionMatrix() const
{
    updateMatrixImpl_();
    return mMatrix;
}

const Matrix44f& Projection::getDeviceProjectionMatrix() const
{
    updateMatrixImpl_();
    return mDeviceMatrix;
}

/*void Projection::cameraPosToScreenPos(Vector3f* dst, const Vector3f& cameraPos) const
{
    dst->setMulAndDivByW(getProjectionMatrix(), cameraPos);
}

void Projection::screenPosToCameraPos(Vector3f* dst, const Vector3f& screenPos) const
{
    doScreenPosToCameraPosTo(dst, screenPos);
}

void Projection::screenPosToCameraPos(Vector3f* dst, const Vector2f& screenPos) const
{
    Vector3f arg(screenPos.x, screenPos.y, 0.0f);
    doScreenPosToCameraPosTo(dst, arg);
}

void Projection::project(Vector2f* dst, const Vector3f& cameraPos, const Viewport& viewport) const
{
    Vector3f screenPos;
    cameraPosToScreenPos(&screenPos, cameraPos);

    viewport.project(dst, screenPos);
}

void Projection::unproject(Vector3f* dst, const Vector3f& screenPos, const Camera& camera) const
{
    Vector3f cameraPos;
    screenPosToCameraPos(&cameraPos, screenPos);

    camera.unprojectByMatrix(dst, cameraPos);
}

void Projection::unprojectRay(Ray3f* dst, const Vector3f& screenPos, const Camera& camera) const
{
    Vector3f cameraPos;
    screenPosToCameraPos(&cameraPos, screenPos);

    camera.unprojectRayByMatrix(dst, cameraPos);
}*/

void Projection::updateAttributesForDirectProjection()
{
}

static void SwapMatrixXY(Matrix44f* tgt)
{
    Vector4f v1;
    Vector4f v2;
    tgt->getRow(&v1, 0);
    tgt->getRow(&v2, 1);

    tgt->setRow(0, v2);
    tgt->setRow(1, v1);
}

void Projection::doUpdateDeviceMatrix(Matrix44f* dst, const Matrix44f& src, Graphics::DevicePosture pose) const
{
    Matrix44f& m = *dst;
    m = src;

    switch (pose)
    {
        case Graphics::DevicePosture::eSame:
            break;

        case Graphics::DevicePosture::eRotateRight:
            m(0, 0) *= -1.0f;
            m(0, 1) *= -1.0f;
            m(0, 2) *= -1.0f;
            m(0, 3) *= -1.0f;

            SwapMatrixXY(dst);
            break;

        case Graphics::DevicePosture::eRotateLeft:
            m(1, 0) *= -1.0f;
            m(1, 1) *= -1.0f;
            m(1, 2) *= -1.0f;
            m(1, 3) *= -1.0f;

            SwapMatrixXY(dst);
            break;

        case Graphics::DevicePosture::eFlipXY:
            m(0, 0) *= -1.0f;
            m(0, 1) *= -1.0f;
            m(0, 2) *= -1.0f;
            m(0, 3) *= -1.0f;

            m(1, 0) *= -1.0f;
            m(1, 1) *= -1.0f;
            m(1, 2) *= -1.0f;
            m(1, 3) *= -1.0f;

            break;

        case Graphics::DevicePosture::eFlipX:
            m(0, 0) *= -1.0f;
            m(0, 1) *= -1.0f;
            m(0, 2) *= -1.0f;
            m(0, 3) *= -1.0f;

            break;

        case Graphics::DevicePosture::eFlipY:
            m(1, 0) *= -1.0f;
            m(1, 1) *= -1.0f;
            m(1, 2) *= -1.0f;
            m(1, 3) *= -1.0f;

            break;

        default:
            SEAD_ASSERT_MSG(false, "Invalid DevicePosture(%d).", static_cast<s32>(pose));
    }

    m(2, 0) = m(2, 0) * mDeviceZScale;
    m(2, 1) = m(2, 1) * mDeviceZScale;

    m(2, 2) = (m(2, 2) + m(3, 2) * mDeviceZOffset) * mDeviceZScale;
    m(2, 3) = m(2, 3) * mDeviceZScale + m(3, 3) * mDeviceZOffset;
}

Matrix44f& Projection::getProjectionMatrixMutable()
{
    updateMatrixImpl_();
    return mMatrix;
}

void Projection::updateMatrixImpl_() const
{
    if (mDirty)
    {
        doUpdateMatrix(const_cast<Matrix44f*>(&mMatrix));
        mDirty = false;
        mDeviceDirty = true;
    }

    if (mDeviceDirty)
    {
        doUpdateDeviceMatrix(const_cast<Matrix44f*>(&mDeviceMatrix), mMatrix, mDevicePosture);
        mDeviceDirty = false;
    }
}

PerspectiveProjection::PerspectiveProjection()
    : Projection()
    , mNear(1.0f)
    , mFar(10000.0f)
    , mAngle(0.0f)
    , mFovySin(0.0f)
    , mFovyCos(0.0f)
    , mFovyTan(0.0f)
    , mAspect(1024.0f / 768.0f)
    , mOffset(Vector2f::zero)
{
    setFovy_(Mathf::deg2rad(45.0f));
    setDirty();
}

PerspectiveProjection::PerspectiveProjection(f32 near, f32 far, f32 fovyRad, f32 aspect)
    : Projection()
    , mNear(near)
    , mFar(far)
    , mAngle(0.0f)
    , mFovySin(0.0f)
    , mFovyCos(0.0f)
    , mFovyTan(0.0f)
    , mAspect(aspect)
    , mOffset(Vector2f::zero)
{
    setFovy_(fovyRad);
    setDirty();
}

PerspectiveProjection::~PerspectiveProjection()
{
}

void PerspectiveProjection::set(f32 _near, f32 _far, f32 fovyRad, f32 aspect)
{
    setNear(_near);
    setFar(_far);
    setFovy(fovyRad);
    setAspect(aspect);
    setDirty();
}

void PerspectiveProjection::setFovx(f32 fovx)
{
    f32 rad = fovx / 2.0f;
    f32 tan = Mathf::tan(rad);
    mAspect = tan / mFovyTan;

    setDirty();
}

void PerspectiveProjection::setFovy_(f32 fovy)
{
    mAngle = fovy;

    f32 rad = fovy / 2.0f;
    mFovySin = Mathf::sin(rad);
    mFovyCos = Mathf::cos(rad);
    mFovyTan = Mathf::tan(rad);

    setDirty();
}

void PerspectiveProjection::createDividedProjection(PerspectiveProjection* dst, s32 partnoX, s32 partnoY, s32 divnumX, s32 divnumY) const
{
    SEAD_ASSERT(dst);

    dst->mFovyTan = mFovyTan / static_cast<f32>(divnumY);

    f32 rad = Mathf::atan2(dst->mFovyTan, 1.0f);

    dst->mFovySin = Mathf::sin(rad);
    dst->mFovyCos = Mathf::cos(rad);
    dst->mAngle = rad * 2.0f;
    dst->mAspect = (mAspect * static_cast<f32>(divnumY)) / static_cast<f32>(divnumX);

    Vector2f offset;
    offset.x =   (static_cast<f32>(partnoX) + 0.5f) - static_cast<f32>(divnumX) / 2.0f;
    offset.y = -((static_cast<f32>(partnoY) + 0.5f) - static_cast<f32>(divnumY) / 2.0f);
    offset += Vector2f(getOffset().x / static_cast<f32>(divnumX), getOffset().y / static_cast<f32>(divnumY));

    dst->setOffset(offset);
    dst->mNear = mNear;
    dst->mFar = mFar;
    dst->setDirty();
}

f32 PerspectiveProjection::getTop() const
{
    f32 height = calcNearClipHeight_();
    f32 offset = mOffset.y * height;
    return height / 2.0f + offset;
}

f32 PerspectiveProjection::getBottom() const
{
    f32 height = calcNearClipHeight_();
    f32 offset = mOffset.y * height;
    return -height / 2.0f + offset;
}

f32 PerspectiveProjection::getLeft() const
{
    f32 width = calcNearClipWidth_();
    f32 offset = mOffset.x * width;
    return -width / 2.0f + offset;
}

f32 PerspectiveProjection::getRight() const
{
    f32 width = calcNearClipWidth_();
    f32 offset = mOffset.x * width;
    return width / 2.0f + offset;
}

void PerspectiveProjection::setTBLR(f32 top, f32 bottom, f32 left, f32 right)
{
    f32 height = top - bottom;
    f32 width = right - left;

    setAspect(width / height);
    setFovy(Mathf::atan2(height / 2.0f, getNear()) * 2.0f);

    Vector2f offset;
    offset.x = ((right + left) / 2.0f) / width;
    offset.y = ((top + bottom) / 2.0f) / height;

    setOffset(offset);
    setDirty();
}

void PerspectiveProjection::doUpdateMatrix(Matrix44f* dst) const
{
    f32 h = calcNearClipHeight_();
    f32 w = calcNearClipWidth_();

    f32 offsetX = mOffset.x * w;
    f32 offsetY = mOffset.y * h;

    f32 t =  h / 2.0f + offsetY;
    f32 b = -h / 2.0f + offsetY;
    f32 l = -w / 2.0f + offsetX;
    f32 r =  w / 2.0f + offsetX;

    Matrix44f& m = *dst;

    f32 tmp = 1.0f / (r - l);

    m(0, 0) = mNear * 2.0f * tmp;
    m(0, 1) = 0.0f;
    m(0, 2) = (r + l) * tmp;
    m(0, 3) = 0.0f;

    tmp = 1.0f / (t - b);

    m(1, 0) = 0.0f;
    m(1, 1) = mNear * 2.0f * tmp;
    m(1, 2) = (t + b) * tmp;
    m(1, 3) = 0.0f;

    tmp = 1.0f / (mFar - mNear);

    m(2, 0) = 0.0f;
    m(2, 1) = 0.0f;
    m(2, 2) = -(mFar + mNear) * tmp;
    m(2, 3) = -(mFar * 2.0f * mNear) * tmp;

    m(3, 0) = 0.0f;
    m(3, 1) = 0.0f;
    m(3, 2) = -1.0f;
    m(3, 3) = 0.0f;
}

void PerspectiveProjection::doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screenPos) const
{
    dst->set(0.0f, 0.0f, -mNear);

    dst->y = (calcNearClipHeight_() / 2.0f) * (screenPos.y + mOffset.y * 2.0f);
    dst->x = (calcNearClipWidth_() / 2.0f) * (screenPos.x + mOffset.x * 2.0f);
}

OrthoProjection::OrthoProjection()
    : Projection()
    , mNear(0.0f)
    , mFar(1.0f)
    , mTop(0.5f)
    , mBottom(-0.5f)
    , mLeft(-0.5f)
    , mRight(0.5f)
{
    setDirty();
}

OrthoProjection::OrthoProjection(f32 _near, f32 _far, f32 top, f32 bottom, f32 left, f32 right)
    : Projection()
    , mNear(_near)
    , mFar(_far)
    , mTop(top)
    , mBottom(bottom)
    , mLeft(left)
    , mRight(right)
{
    setDirty();
}

OrthoProjection::OrthoProjection(f32 _near, f32 _far, const BoundBox2f& box)
    : Projection()
    , mNear(_near)
    , mFar(_far)
    , mTop(box.getMax().y)
    , mBottom(box.getMin().y)
    , mLeft(box.getMin().x)
    , mRight(box.getMax().x)
{
    SEAD_ASSERT(!box.isUndef());

    setDirty();
}

OrthoProjection::OrthoProjection(f32 _near, f32 _far, const Viewport& vp)
    : Projection()
    , mNear(_near)
    , mFar(_far)
    , mTop(vp.getHalfSizeY())
    , mBottom(-vp.getHalfSizeY())
    , mLeft(-vp.getHalfSizeX())
    , mRight(vp.getHalfSizeX())
{
    SEAD_ASSERT(!vp.isUndef());

    setDevicePosture(vp.getDevicePosture());
    setDirty();
}

OrthoProjection::~OrthoProjection()
{
}

void OrthoProjection::setByViewport(const Viewport& vp)
{
    setTBLR(vp.getHalfSizeY(), -vp.getHalfSizeY(), -vp.getHalfSizeX(), vp.getHalfSizeX());
}

void OrthoProjection::setTBLR(f32 top, f32 bottom, f32 left, f32 right)
{
    mTop = top;
    mBottom = bottom;
    mLeft = left;
    mRight = right;
    setDirty();
}

void OrthoProjection::setBoundBox(const BoundBox2f& box)
{
    SEAD_ASSERT(!box.isUndef());

    setTBLR(box.getMax().y, box.getMin().y, box.getMin().x, box.getMax().x);
}

void OrthoProjection::createDividedProjection(OrthoProjection* dst, s32 partnoX, s32 partnoY, s32 divnumX, s32 divnumY) const
{
    SEAD_ASSERT(dst);

    dst->setLeft(((mRight - mLeft) * static_cast<f32>(partnoX)) / static_cast<f32>(divnumX) + mLeft);
    dst->setRight(dst->getLeft() + (mRight - mLeft) / static_cast<f32>(divnumX));
    dst->setTop(((mBottom - mTop) * static_cast<f32>(partnoY)) / static_cast<f32>(divnumY) + mTop);
    dst->setBottom(dst->getTop() + (mBottom - mTop) / static_cast<f32>(divnumY));

    dst->setNear(getNear());
    dst->setFar(getFar());

    dst->setDirty();
}

void OrthoProjection::doUpdateMatrix(Matrix44f* dst) const
{
    Matrix44f& m = *dst;

    Vector2f center((mRight + mLeft) / 2.0f, (mTop + mBottom) / 2.0f);

    {
        f32 size = (mRight - mLeft) / 2.0f;

        m(0, 0) = Mathf::inv(size);
        m(0, 1) = 0.0f;
        m(0, 2) = 0.0f;
        m(0, 3) = -center.x / size;
    }

    {
        f32 size = (mTop - mBottom) / 2.0f;

        m(1, 0) = 0.0f;
        m(1, 1) = Mathf::inv(size);
        m(1, 2) = 0.0f;
        m(1, 3) = -center.y / size;
    }

    {
        f32 q = Mathf::inv(mFar - mNear);

        m(2, 0) = 0.0f;
        m(2, 1) = 0.0f;
        m(2, 2) = -q;
        m(2, 3) = -mNear * q;
    }

    {
        m(3, 0) = 0.0f;
        m(3, 1) = 0.0f;
        m(3, 2) = 0.0f;
        m(3, 3) = 1.0f;
    }
}

void OrthoProjection::doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screenPos) const
{
    dst->x = ((mRight - mLeft) * screenPos.x) / 2.0f + (mLeft + mRight) / 2.0f;
    dst->y = ((mTop - mBottom) * screenPos.y) / 2.0f + (mTop + mBottom) / 2.0f;
    dst->z = -mNear;
}

} // namespace sead
