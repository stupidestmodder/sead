#include <gfx/gl/seadPrimitiveDrawMgrGL.h>

#include <gfx/gl/seadTextureGL.h>
#include <gfx/seadGraphics.h>
#include <gfx/seadPrimitiveDrawUtil.h>

static bool CreateShader_(GLuint shader, const sead::SafeString& source, sead::Heap* heap)
{
    GLint length = source.calcLength() + 1;
    const char* str = source.cstr();

    glShaderSource(shader, 1, &str, &length);
    glCompileShader(shader);

    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result != GL_TRUE)
    {
        GLint logSize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

        char* buffer = new(heap) char[logSize];
        glGetShaderInfoLog(shader, logSize, &logSize, buffer);

        SEAD_ASSERT_MSG(false, "%s", buffer);

        delete[] buffer;

        return false;
    }

    return true;
}

static void CreateVertexBuffer_(GLuint* vertexBuffer, const sead::PrimitiveDrawUtil::Vertex* vtx, s32 vtxNum, const u16* idx, s32 idxNum)
{
    glGenBuffers(2, vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sead::PrimitiveDrawUtil::Vertex) * vtxNum, vtx, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idxNum, idx, GL_STATIC_DRAW);
}

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(PrimitiveDrawMgrGL);

PrimitiveDrawMgrGL::PrimitiveDrawMgrGL()
    : PrimitiveDrawMgrBase()
    , mVAO(GL_NONE)
    , mVS(GL_NONE)
    , mFS(GL_NONE)
    , mShaderProgram(GL_NONE)
    , mSampler(GL_NONE)
    , mParamWVP(-1)
    , mParamUser(-1)
    , mParamRate(-1)
    , mParamTex(-1)
    , mParamColor0(-1)
    , mParamColor1(-1)
    , mParamUVSrc(-1)
    , mParamUVSize(-1)
    , mAttrVertex(-1)
    , mAttrTexCoord0(-1)
    , mAttrColorRate(-1)
{
}

PrimitiveDrawMgrGL::~PrimitiveDrawMgrGL()
{
    // TODO
}

void PrimitiveDrawMgrGL::prepareFromBinaryImpl(Heap* heap, const void*, u32)
{
    Graphics::instance()->lockDrawContext();
    {
        {
            glGenVertexArrays(1, &mVAO);
            SEAD_ASSERT(mVAO != GL_NONE);
        }

        {
            SEAD_ASSERT(mVS == GL_NONE);
            mVS = glCreateShader(GL_VERTEX_SHADER);

            const char* vsShader = 
                "#version 460 core\n "
                "in vec3 aVertex; "
                "in vec2 aTexCoord0; "
                "in vec4 aColorRate; "
                "uniform mat4 uWVP; "
                "uniform mat4 uUser; "
                "uniform vec4 uColor0; "
                "uniform vec4 uColor1; "
                "uniform vec2 uUVSrc; "
                "uniform vec2 uUVSize; "
                "out vec4 vColor; "
                "out vec2 vTexCoord; "
                "void main(void) "
                "{ "
                "    gl_Position = vec4(aVertex, 1.0f) * uUser * uWVP; "
                "    vColor = uColor0 * (1.0 - aColorRate.r) + uColor1 * aColorRate.r; "
                "    vTexCoord = aTexCoord0 * uUVSize + uUVSrc; "
                "} ";

            if (!CreateShader_(mVS, vsShader, heap))
            {
                SEAD_ASSERT_MSG(false, "Can't create VertexShader.");

                glDeleteShader(mVS);
                mVS = GL_NONE;

                return;
            }
        }

        {
            SEAD_ASSERT(mFS == GL_NONE);
            mFS = glCreateShader(GL_FRAGMENT_SHADER);

            const char* fsShader = 
                "#version 460 core\n "
                "out vec4 oFragColor; "
                "uniform sampler2D uTexture0; "
                "uniform float uRate; "
                "in vec4 vColor; "
                "in vec2 vTexCoord; "
                "void main() "
                "{ "
                "    vec4 color0 = texture2D(uTexture0, vTexCoord); "
                "    oFragColor.r = color0.r * vColor.r * uRate + vColor.r * (1.0f - uRate); "
                "    oFragColor.g = color0.g * vColor.g * uRate + vColor.g * (1.0f - uRate); "
                "    oFragColor.b = color0.b * vColor.b * uRate + vColor.b * (1.0f - uRate); "
                "    oFragColor.a = color0.a * vColor.a * uRate + vColor.a * (1.0f - uRate); "
                "} ";

            if (!CreateShader_(mFS, fsShader, heap))
            {
                SEAD_ASSERT_MSG(false, "Can't create FragmentShader.");

                glDeleteShader(mFS);
                mFS = GL_NONE;
                glDeleteShader(mVS);
                mVS = GL_NONE;

                return;
            }
        }

        {
            SEAD_ASSERT(mShaderProgram == GL_NONE);
            mShaderProgram = glCreateProgram();

            glAttachShader(mShaderProgram, mVS);
            glAttachShader(mShaderProgram, mFS);
            glLinkProgram(mShaderProgram);

            GLint result;
            glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &result);

            if (result == GL_FALSE)
            {
                GLint logSize;
                glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &logSize);

                char* buffer = new(heap) char[logSize];
                glGetProgramInfoLog(mShaderProgram, logSize, &logSize, buffer);

                SEAD_ASSERT_MSG(false, "%s", buffer);

                delete[] buffer;

                glDeleteProgram(mShaderProgram);
                mShaderProgram = GL_NONE;
                glDeleteShader(mFS);
                mFS = GL_NONE;
                glDeleteShader(mVS);
                mVS = GL_NONE;

                return;
            }
        }

        {
            glGenSamplers(1, &mSampler);
            glSamplerParameteri(mSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glSamplerParameteri(mSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        {
            mParamWVP = glGetUniformLocation(mShaderProgram, "uWVP");
            mParamUser = glGetUniformLocation(mShaderProgram, "uUser");
            mParamRate = glGetUniformLocation(mShaderProgram, "uRate");
            mParamTex = glGetUniformLocation(mShaderProgram, "uTexture0");
            mParamColor0 = glGetUniformLocation(mShaderProgram, "uColor0");
            mParamColor1 = glGetUniformLocation(mShaderProgram, "uColor1");
            mParamUVSrc = glGetUniformLocation(mShaderProgram, "uUVSrc");
            mParamUVSize = glGetUniformLocation(mShaderProgram, "uUVSize");

            mAttrVertex = glGetAttribLocation(mShaderProgram, "aVertex");
            mAttrTexCoord0 = glGetAttribLocation(mShaderProgram, "aTexCoord0");
            mAttrColorRate = glGetAttribLocation(mShaderProgram, "aColorRate");
        }

        {
            glBindVertexArray(0);

            {
                PrimitiveDrawUtil::Vertex vtx[4];
                u16 idx[3 * 2];

                PrimitiveDrawUtil::setQuadVertex(vtx, idx);
                CreateVertexBuffer_(mQuadBuf, vtx, 4, idx, 3 * 2);
            }

            {
                PrimitiveDrawUtil::Vertex vtx[4];
                static const u16 idx[4] = { 0, 1, 3, 2 };

                PrimitiveDrawUtil::setQuadVertex(vtx, nullptr);
                CreateVertexBuffer_(mBoxBuf, vtx, 4, idx, 4);
            }

            {
                PrimitiveDrawUtil::Vertex vtx[2];
                u16 idx[2];

                PrimitiveDrawUtil::setLineVertex(vtx, idx);
                CreateVertexBuffer_(mLineBuf, vtx, 2, idx, 2);
            }
        }
    }
    Graphics::instance()->unlockDrawContext();
}

void PrimitiveDrawMgrGL::prepareImpl(Heap* heap, const SafeString&)
{
    prepareFromBinaryImpl(heap, nullptr, 0);
}

void PrimitiveDrawMgrGL::beginImpl(DrawContext* drawContext, const Matrix34f& viewMatrix, const Matrix44f& projectionMatrix)
{
    SEAD_UNUSED(drawContext);

    Matrix44f wvp;
    wvp.setMul(projectionMatrix, viewMatrix);

    glUseProgram(mShaderProgram);

    glUniformMatrix4fv(mParamWVP, 1, GL_FALSE, wvp.a);
    glUniformMatrix4fv(mParamUser, 1, GL_FALSE, Matrix44f::ident.a);

    glBindVertexArray(mVAO);

    glEnableVertexAttribArray(mAttrVertex);
    glEnableVertexAttribArray(mAttrTexCoord0);
    glEnableVertexAttribArray(mAttrColorRate);
}

void PrimitiveDrawMgrGL::endImpl(DrawContext* drawContext)
{
    SEAD_UNUSED(drawContext);

    glDisableVertexAttribArray(mAttrVertex);
    glDisableVertexAttribArray(mAttrTexCoord0);
    glDisableVertexAttribArray(mAttrColorRate);

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void PrimitiveDrawMgrGL::drawQuadImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
    SEAD_UNUSED(drawContext);

    drawPolygon_(modelMtx, mQuadBuf, 6, colorL, colorR);
}

void PrimitiveDrawMgrGL::drawQuadImpl(
    DrawContext* drawContext, const Matrix34f& modelMtx, const Texture& texture,
    const Color4f& colorL, const Color4f& colorR,
    const Vector2f& uvSrc, const Vector2f& uvSize)
{
    SEAD_UNUSED(drawContext);

    const TextureGL* textureGL = DynamicCast<const TextureGL>(&texture);
    if (!textureGL)
    {
        SEAD_ASSERT_MSG(false, "texture must be TextureGL.");
        return;
    }

    drawVertexBuffer_(modelMtx, mQuadBuf, colorL, colorR);

    glUniform1f(mParamRate, 1.0f);
    glUniform1i(mParamTex, 0);
    glUniform2fv(mParamUVSrc, 1, uvSrc.e);
    glUniform2fv(mParamUVSize, 1, uvSize.e);

    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, mSampler);
    glBindTexture(GL_TEXTURE_2D, textureGL->getID());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void PrimitiveDrawMgrGL::drawBoxImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& colorL, const Color4f& colorR)
{
    SEAD_UNUSED(drawContext);

    drawLines_(modelMtx, GL_LINE_LOOP, mBoxBuf, 4, colorL, colorR);
}

void PrimitiveDrawMgrGL::drawLineImpl(DrawContext* drawContext, const Matrix34f& modelMtx, const Color4f& c0, const Color4f& c1)
{
    SEAD_UNUSED(drawContext);

    drawLines_(modelMtx, GL_LINES, mLineBuf, 2, c0, c1);
}

void PrimitiveDrawMgrGL::drawVertexBuffer_(const Matrix34f& modelMtx, GLuint* vb, const Color4f& color0, const Color4f& color1)
{
    Matrix44f mat(modelMtx);
    glUniformMatrix4fv(mParamUser, 1, GL_FALSE, mat.a);

    glUniform4fv(mParamColor0, 1, color0.c);
    glUniform4fv(mParamColor1, 1, color1.c);

    glUniform1f(mParamRate, 0.0f);

    glBindBuffer(GL_ARRAY_BUFFER, vb[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb[1]);

    const u8* dummy = nullptr;
    glVertexAttribPointer(mAttrVertex, 3, GL_FLOAT, GL_FALSE, sizeof(PrimitiveDrawUtil::Vertex), dummy + 0);
    glVertexAttribPointer(mAttrTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimitiveDrawUtil::Vertex), dummy + sizeof(f32) * 3);
    glVertexAttribPointer(mAttrColorRate, 4, GL_FLOAT, GL_FALSE, sizeof(PrimitiveDrawUtil::Vertex), dummy + sizeof(f32) * 5);
}

void PrimitiveDrawMgrGL::drawPolygon_(const Matrix34f& modelMtx, GLuint* vb, s32 idxNum, const Color4f& color0, const Color4f& color1)
{
    drawVertexBuffer_(modelMtx, vb, color0, color1);

    glUniform2fv(mParamUVSrc, 1, Vector2f::zero.e);
    glUniform2fv(mParamUVSize, 1, Vector2f::ones.e);

    glDrawElements(GL_TRIANGLES, idxNum, GL_UNSIGNED_SHORT, nullptr);
}

void PrimitiveDrawMgrGL::drawLines_(const Matrix34f& modelMtx, GLenum lineType, GLuint* vb, s32 idxNum, const Color4f& color0, const Color4f& color1)
{
    drawVertexBuffer_(modelMtx, vb, color0, color1);

    glDrawElements(lineType, idxNum, GL_UNSIGNED_SHORT, nullptr);
}

} // namespace sead
