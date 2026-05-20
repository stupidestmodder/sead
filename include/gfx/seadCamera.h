#pragma once

#include <geom/seadLine.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class OrthoProjection;
class Projection;
class Viewport;

class Camera
{
    SEAD_RTTI_BASE(Camera);

public:
    Camera()
        : mMatrix(Matrix34f::ident)
    {
    }

    virtual ~Camera()
    {
    }

    virtual void doUpdateMatrix(Matrix34f* dst) const = 0;

    void updateMatrix()
    {
        doUpdateMatrix(&mMatrix);
    }

    const Matrix34f& getViewMatrix() const
    {
        return mMatrix;
    }

    void getWorldPosByMatrix(Vector3f* dst) const;
    void getLookVectorByMatrix(Vector3f* dst) const;
    void getRightVectorByMatrix(Vector3f* dst) const;
    void getUpVectorByMatrix(Vector3f* dst) const;

    void worldPosToCameraPosByMatrix(Vector3f* dst, const Vector3f& worldPos) const;
    void cameraPosToWorldPosByMatrix(Vector3f* dst, const Vector3f& cameraPos) const;

    void projectByMatrix(Vector2f* dst, const Vector3f& worldPos, const Projection& projection, const Viewport& viewport) const;

    void unprojectByMatrix(Vector3f* dst, const Vector3f& cameraPos) const
    {
        cameraPosToWorldPosByMatrix(dst, cameraPos);
    }

    void unprojectRayByMatrix(Ray3f* dst, const Vector3f& cameraPos) const;

protected:
    Matrix34f mMatrix;
};

class LookAtCamera : public Camera
{
    SEAD_RTTI_OVERRIDE(LookAtCamera, Camera);

public:
    LookAtCamera()
        : Camera()
        , mPos(0.0f, 0.0f, 10.0f)
        , mAt(0.0f, 0.0f, 0.0f)
        , mUp(0.0f, 1.0f, 0.0f)
    {
    }

    LookAtCamera(const Vector3f& pos, const Vector3f& at, const Vector3f& up);
    ~LookAtCamera() override;

    void doUpdateMatrix(Matrix34f* dst) const override;

    Vector3f& getPos() { return mPos; }
    const Vector3f& getPos() const { return mPos; }
    void setPos(const Vector3f& pos) { mPos = pos; }

    Vector3f& getAt() { return mAt; }
    const Vector3f& getAt() const { return mAt; }
    void setAt(const Vector3f& at) { mAt = at; }

    Vector3f& getUp() { return mUp; }
    const Vector3f& getUp() const { return mUp; }
    void setUp(const Vector3f& up) { mUp = up; mUp.normalize(); }

private:
    Vector3f mPos;
    Vector3f mAt;
    Vector3f mUp;
};

class OrthoCamera : public LookAtCamera
{
    SEAD_RTTI_OVERRIDE(OrthoCamera, LookAtCamera);

public:
    OrthoCamera();
    OrthoCamera(const Vector2f& center, f32 distance);
    OrthoCamera(const OrthoProjection& proj);
    ~OrthoCamera() override;

    void setByOrthoProjection(const OrthoProjection& proj);
    void setRotation(f32 rad);
};

class DirectCamera : public Camera
{
    SEAD_RTTI_OVERRIDE(DirectCamera, Camera);

public:
    DirectCamera()
        : Camera()
        , mDirectMatrix(Matrix34f::ident)
    {
    }

    ~DirectCamera() override;

    void doUpdateMatrix(Matrix34f* dst) const override;

    void setViewMatrix(const Matrix34f& matrix)
    {
        mDirectMatrix = matrix;
    }

protected:
    Matrix34f mDirectMatrix;
};

} // namespace sead
