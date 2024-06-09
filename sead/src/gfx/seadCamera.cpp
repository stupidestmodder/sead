#include <gfx/seadCamera.h>

#include <basis/seadWarning.h>
#include <gfx/seadProjection.h>

namespace sead {

void Camera::getWorldPosByMatrix(Vector3f* dst) const
{
    const Matrix34f& m = getViewMatrix();

    dst->set((-m(0, 0) * m(0, 3) - m(1, 0) * m(1, 3)) - m(2, 0) * m(2, 3),
             (-m(0, 1) * m(0, 3) - m(1, 1) * m(1, 3)) - m(2, 1) * m(2, 3),
             (-m(0, 2) * m(0, 3) - m(1, 2) * m(1, 3)) - m(2, 2) * m(2, 3));
}

void Camera::getLookVectorByMatrix(Vector3f* dst) const
{
    const Matrix34f& m = getViewMatrix();

    dst->set(m(2, 0), m(2, 1), m(2, 2));
}

void Camera::getRightVectorByMatrix(Vector3f* dst) const
{
    const Matrix34f& m = getViewMatrix();

    dst->set(m(0, 0), m(0, 1), m(0, 2));
}

void Camera::getUpVectorByMatrix(Vector3f* dst) const
{
    const Matrix34f& m = getViewMatrix();

    dst->set(m(1, 0), m(1, 1), m(1, 2));
}

/*void Camera::worldPosToCameraPosByMatrix(Vector3f* dst, const Vector3f& worldPos) const
{
    dst->setMul(getViewMatrix(), worldPos);
}

void Camera::cameraPosToWorldPosByMatrix(Vector3f* dst, const Vector3f& cameraPos) const
{
    Vector3f vUp;
    Vector3f vRight;
    Vector3f vLook;

    getUpVectorByMatrix(&vUp);
    getRightVectorByMatrix(&vRight);
    getLookVectorByMatrix(&vLook);

    vUp.multScalar(cameraPos.y);
    vLook.multScalar(cameraPos.z);
    vRight.multScalar(cameraPos.x);

    getWorldPosByMatrix(dst);

    dst->add(vUp);
    dst->add(vLook);
    dst->add(vRight);
}

void Camera::projectByMatrix(Vector2f* dst, const Vector3f& worldPos, const Projection& projection, const Viewport& viewport) const
{
    Vector3f cameraPos;
    worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    projection.project(dst, cameraPos, viewport);
}

void Camera::unprojectRayByMatrix(Ray3f* dst, const Vector3f& cameraPos) const
{
    Vector3f tgt;
    cameraPosToWorldPosByMatrix(&tgt, cameraPos);

    Vector3f pos;
    getWorldPosByMatrix(&pos);

    dst->setBy2Points(pos, tgt);
}*/

LookAtCamera::LookAtCamera(const Vector3f& pos, const Vector3f& at, const Vector3f& up)
    : Camera()
    , mPos(pos)
    , mAt(at)
    , mUp(up)
{
    SEAD_ASSERT(mPos != mAt);
    mUp.normalize();
}

LookAtCamera::~LookAtCamera()
{
}

void LookAtCamera::doUpdateMatrix(Matrix34f* dst) const
{
    if (mPos == mAt)
    {
        SEAD_WARNING("Warning! LookAtCamera::mPos == LookAtCamera::mAt\n");
        return;
    }

    Vector3f view = mPos;
    view -= mAt;
    view.normalize();

    Vector3f right;
    right.setCross(mUp, view);
    right.normalize();

    Vector3f up;
    up.setCross(view, right);

    f32 tx = right.dot(mPos);
    f32 ty = up.dot(mPos);
    f32 tz = view.dot(mPos);

    Matrix34f& m = *dst;

    m(0, 0) = right.x;
    m(0, 1) = right.y;
    m(0, 2) = right.z;
    m(0, 3) = -tx;

    m(1, 0) = up.x;
    m(1, 1) = up.y;
    m(1, 2) = up.z;
    m(1, 3) = -ty;

    m(2, 0) = view.x;
    m(2, 1) = view.y;
    m(2, 2) = view.z;
    m(2, 3) = -tz;
}

OrthoCamera::OrthoCamera()
    : LookAtCamera(Vector3f(0.0f, 0.0f, 1.0f),
                   Vector3f(0.0f, 0.0f, 0.0f),
                   Vector3f(0.0f, 1.0f, 0.0f))
{
}

OrthoCamera::OrthoCamera(const Vector2f& center, f32 distance)
    : LookAtCamera(Vector3f(center.x, center.y, distance),
                   Vector3f(center.x, center.y, distance - 1.0f),
                   Vector3f(0.0f, 1.0f, 0.0f))
{
}

OrthoCamera::OrthoCamera(const OrthoProjection& proj)
    : LookAtCamera(Vector3f((proj.getLeft() + proj.getRight()) / 2.0f, (proj.getTop() + proj.getBottom()) / 2.0f, proj.getNear()),
                   Vector3f((proj.getLeft() + proj.getRight()) / 2.0f, (proj.getTop() + proj.getBottom()) / 2.0f, proj.getNear() - 1.0f),
                   Vector3f(0.0f, 1.0f, 0.0f))
{
}

OrthoCamera::~OrthoCamera()
{
}

void OrthoCamera::setByOrthoProjection(const OrthoProjection& proj)
{
    setPos(Vector3f((proj.getLeft() + proj.getRight()) / 2.0f, (proj.getTop() + proj.getBottom()) / 2.0f, proj.getNear()));
    setAt(Vector3f((proj.getLeft() + proj.getRight()) / 2.0f, (proj.getTop() + proj.getBottom()) / 2.0f, proj.getNear() - 1.0f));
    setUp(Vector3f(0.0f, 1.0f, 0.0f));
}

void OrthoCamera::setRotation(f32 rad)
{
    Vector3f up;
    up.x = Mathf::cos(rad - Mathf::deg2rad(90.0f));
    up.y = Mathf::sin(rad - Mathf::deg2rad(90.0f));
    up.y = -up.y;
    up.z = 0.0f;

    setUp(up);
}

DirectCamera::~DirectCamera()
{
}

void DirectCamera::doUpdateMatrix(Matrix34f* dst) const
{
    *dst = mDirectMatrix;
}

} // namespace sead
