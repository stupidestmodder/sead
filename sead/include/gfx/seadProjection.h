#pragma once

#include <geom/seadLine.h>
#include <gfx/seadGraphics.h>
#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class Camera;
class Viewport;

class Projection
{
    SEAD_RTTI_BASE(Projection);

public:
    enum class Type
    {
        ePerspective = 0,
        eOrtho,
        eUndefined
    };

public:
    Projection();
    virtual ~Projection();

    const Matrix44f& getProjectionMatrix() const;
    const Matrix44f& getDeviceProjectionMatrix() const;

    void cameraPosToScreenPos(Vector3f* dst, const Vector3f& cameraPos) const;
    void screenPosToCameraPos(Vector3f* dst, const Vector3f& screenPos) const;
    void screenPosToCameraPos(Vector3f* dst, const Vector2f& screenPos) const;

    void project(Vector2f* dst, const Vector3f& cameraPos, const Viewport& viewport) const;
    void unproject(Vector3f* dst, const Vector3f& screenPos, const Camera& camera) const;
    void unprojectRay(Ray3f* dst, const Vector3f& screenPos, const Camera& camera) const;

    virtual f32 getNear() const = 0;
    virtual f32 getFar() const = 0;
    virtual f32 getFovy() const = 0;
    virtual f32 getAspect() const = 0;
    virtual void getOffset(Vector2f* dst) const = 0;
    virtual void updateAttributesForDirectProjection();

    virtual Type getProjectionType() const = 0;
    virtual void doUpdateMatrix(Matrix44f* dst) const = 0;
    virtual void doUpdateDeviceMatrix(Matrix44f* dst, const Matrix44f& src, Graphics::DevicePosture pose) const;
    virtual void doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screenPos) const = 0;

    Matrix44f& getProjectionMatrixMutable();

    void setDirty()
    {
        mDirty = true;
    }

    void setDeviceDirty()
    {
        mDeviceDirty = true;
    }

    void setDevicePosture(Graphics::DevicePosture pos)
    {
        mDevicePosture = pos;
        setDeviceDirty();
    }

protected:
    void updateMatrixImpl_() const;

protected:
    mutable bool mDirty;
    mutable bool mDeviceDirty;
    Matrix44f mMatrix;
    Matrix44f mDeviceMatrix;
    Graphics::DevicePosture mDevicePosture;
    f32 mDeviceZScale;
    f32 mDeviceZOffset;
};

class PerspectiveProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(PerspectiveProjection, Projection);

public:
    PerspectiveProjection();
    PerspectiveProjection(f32 near, f32 far, f32 fovyRad, f32 aspect);
    ~PerspectiveProjection() override;

    void setNear(f32 _near)
    {
        mNear = _near;
        setDirty();
    }

    void setFar(f32 _far)
    {
        mFar = _far;
        setDirty();
    }

    void setFovy(f32 fovyRad)
    {
        setFovy_(fovyRad);
    }

    void setAspect(f32 aspect)
    {
        mAspect = aspect;
        setDirty();
    }

    void setOffset(const Vector2f& offset)
    {
        mOffset = offset;
        setDirty();
    }

    void set(f32 _near, f32 _far, f32 fovyRad, f32 aspect);
    void setFovx(f32 fovx);

    void createDividedProjection(PerspectiveProjection* dst, s32 partnoX, s32 partnoY, s32 divnumX, s32 divnumY) const;

    f32 getTop() const;
    f32 getBottom() const;
    f32 getLeft() const;
    f32 getRight() const;

    void setTBLR(f32 top, f32 bottom, f32 left, f32 right);

    f32 getNear() const override { return mNear; }
    f32 getFar() const override { return mFar; }
    f32 getFovy() const override { return mAngle; }
    f32 getAspect() const override { return mAspect; }

    void getOffset(Vector2f* dst) const override
    {
        dst->x = mOffset.x;
        dst->y = mOffset.y;
    }

    const Vector2f& getOffset() const
    {
        return mOffset;
    }

    Type getProjectionType() const override { return Type::ePerspective; }
    void doUpdateMatrix(Matrix44f* dst) const override;
    void doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screenPos) const override;

protected:
    void setFovy_(f32 fovy);

    f32 calcNearClipHeight_() const
    {
        return mNear * 2.0f * mFovyTan;
    }

    f32 calcNearClipWidth_() const
    {
        return calcNearClipHeight_() * mAspect;
    }

protected:
    f32 mNear;
    f32 mFar;
    f32 mAngle;
    f32 mFovySin;
    f32 mFovyCos;
    f32 mFovyTan;
    f32 mAspect;
    Vector2f mOffset;
};

class OrthoProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(OrthoProjection, Projection);

public:
    OrthoProjection();
    OrthoProjection(f32 _near, f32 _far, f32 top, f32 bottom, f32 left, f32 right);
    OrthoProjection(f32 _near, f32 _far, const BoundBox2f& box);
    OrthoProjection(f32 _near, f32 _far, const Viewport& vp);
    virtual ~OrthoProjection();

    void setNear(f32 _near)
    {
        mNear = _near;
        setDirty();
    }

    void setFar(f32 _far)
    {
        mFar = _far;
        setDirty();
    }

    void setTop(f32 top)
    {
        mTop = top;
        setDirty();
    }

    void setBottom(f32 bottom)
    {
        mBottom = bottom;
        setDirty();
    }

    void setLeft(f32 left)
    {
        mLeft = left;
        setDirty();
    }

    void setRight(f32 right)
    {
        mRight = right;
        setDirty();
    }

    void setByViewport(const Viewport& vp);
    void setTBLR(f32 top, f32 bottom, f32 left, f32 right);
    void setBoundBox(const BoundBox2f& box);

    void createDividedProjection(OrthoProjection* dst, s32 partnoX, s32 partnoY, s32 divnumX, s32 divnumY) const;

    f32 getNear() const override { return mNear; }
    f32 getFar() const override { return mFar; }
    f32 getFovy() const override { return 0.0f; }
    f32 getAspect() const override { return (mRight - mLeft) / (mTop - mBottom); }

    void getOffset(Vector2f* dst) const override
    {
        dst->x = ((mRight + mLeft) / 2.0f) / (mRight - mLeft);
        dst->y = ((mTop + mBottom) / 2.0f) / (mTop - mBottom);
    }

    f32 getTop() const { return mTop; }
    f32 getBottom() const { return mBottom; }
    f32 getLeft() const { return mLeft; }
    f32 getRight() const { return mRight; }

    Type getProjectionType() const override { return Type::eOrtho; }
    void doUpdateMatrix(Matrix44f* dst) const override;
    void doScreenPosToCameraPosTo(Vector3f* dst, const Vector3f& screenPos) const override;

protected:
    f32 mNear;
    f32 mFar;
    f32 mTop;
    f32 mBottom;
    f32 mLeft;
    f32 mRight;
};

// TODO
class FrustumProjection
{
};

// TODO
class DirectProjection
{
};

} // namespace sead
