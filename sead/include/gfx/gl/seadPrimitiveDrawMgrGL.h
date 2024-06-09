#pragma once

#include <gfx/gl/seadGL.h>
#include <gfx/seadPrimitiveDrawMgrBase.h>
#include <heap/seadDisposer.h>

namespace sead {

// TODO
class PrimitiveDrawMgrGL : public PrimitiveDrawMgrBase
{
    SEAD_SINGLETON_DISPOSER(PrimitiveDrawMgrGL);

public:
    PrimitiveDrawMgrGL();
    ~PrimitiveDrawMgrGL() override;

    void prepareFromBinaryImpl(Heap* heap, const void* binData, u32 binSize) override;
    void prepareImpl(Heap* heap, const SafeString& path) override;
    void beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix) override;
    void endImpl(DrawContext* drawContext) override;
    void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) override;
    void drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
                      const Color4f& colorL, const Color4f& colorR,
                      const Vector2f& uvSrc, const Vector2f& uvSize) override;
    void drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR) override;
    void drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1) override;

protected:
    void drawVertexBuffer_(const Matrix34f& modelMtx, GLuint* vb, const Color4f& color0, const Color4f& color1);
    void drawPolygon_(const Matrix34f& modelMtx, GLuint* vb, s32 idxNum, const Color4f& color0, const Color4f& color1);
    void drawLines_(const Matrix34f& modelMtx, GLenum lineType, GLuint* vb, s32 idxNum, const Color4f& color0, const Color4f& color1);

protected:
    GLuint mVAO;
    GLuint mVS;
    GLuint mFS;
    GLuint mShaderProgram;
    GLuint mSampler;

    GLint mParamWVP;
    GLint mParamUser;
    GLint mParamRate;
    GLint mParamTex;
    GLint mParamColor0;
    GLint mParamColor1;
    GLint mParamUVSrc;
    GLint mParamUVSize;
    GLint mAttrVertex;
    GLint mAttrTexCoord0;
    GLint mAttrColorRate;

    GLuint mQuadBuf[2];
    GLuint mBoxBuf[2];
    GLuint mLineBuf[2];
};

} // namespace sead
