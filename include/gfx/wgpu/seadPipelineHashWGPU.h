#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <codec/seadHashCRC64.h>

#include <bit>
#include <type_traits>

#include <gfx/wgpu/seadWGPU.h>

namespace sead {

class PipelineHashWGPU
{
public:
    using Context = HashCRC64::Context;

public:
    static bool calcHash(u64* out, const WGPURenderPipelineDescriptor& desc)
    {
        const WGPUPrimitiveState& primitive = desc.primitive;
        const WGPUMultisampleState& multisample = desc.multisample;

        if (!isPlain_(desc.nextInChain) || !isPlain_(primitive.nextInChain) || !isPlain_(multisample.nextInChain))
            return false;

        Context ctx;
        hash_(&ctx, u32(0x70697065));  // "pipe"
        hash_(&ctx, desc.layout);

        if (!hashVertex_(&ctx, desc.vertex) || !hashFragment_(&ctx, desc.fragment) ||
            !hashDepthStencil_(&ctx, desc.depthStencil))
            return false;

        hash_(&ctx, primitive.topology);
        hash_(&ctx, primitive.stripIndexFormat);
        hash_(&ctx, primitive.frontFace);
        hash_(&ctx, primitive.cullMode);
        hash_(&ctx, primitive.unclippedDepth != WGPU_FALSE);

        hash_(&ctx, multisample.count);
        hash_(&ctx, multisample.mask);
        hash_(&ctx, multisample.alphaToCoverageEnabled != WGPU_FALSE);

        *out = ~ctx.hash;
        return true;
    }

protected:
    template <typename T>
    static void hash_(Context* ctx, T value)
    {
        u64 v;
        if constexpr (std::is_pointer_v<T>)
            v = reinterpret_cast<std::uintptr_t>(value);
        else
            v = static_cast<u64>(value);

        u8 bytes[8];
        for (u32 i = 0; i < 8; i++)
            bytes[i] = static_cast<u8>(v >> (8 * i));
        HashCRC64::calcHashWithContext(ctx, bytes, sizeof(bytes));
    }

    static bool hashFloat_(Context* ctx, f64 value)
    {
        if (value == 0.0)
            value = 0.0;
        hash_(ctx, std::bit_cast<u64>(value));
        return true;
    }

    static void hashString_(Context* ctx, const WGPUStringView& str)
    {
        size_t length = 0;
        if (str.data != nullptr)
            length = str.length == WGPU_STRLEN ? std::strlen(str.data) : str.length;

        hash_(ctx, length);
        if (length > 0)
            HashCRC64::calcHashWithContext(ctx, str.data, static_cast<u32>(length));
    }

    static bool isPlain_(const void* nextInChain)
    {
        SEAD_ASSERT(nextInChain == nullptr);
        return nextInChain == nullptr;
    }

    template <typename State>
    static bool hashStage_(Context* ctx, const State& state)
    {
        if (!isPlain_(state.nextInChain))
            return false;

        hash_(ctx, state.module);
        hashString_(ctx, state.entryPoint);

        hash_(ctx, state.constantCount);
        for (size_t i = 0; i < state.constantCount; i++)
        {
            const WGPUConstantEntry& constant = state.constants[i];
            if (!isPlain_(constant.nextInChain))
                return false;

            hashString_(ctx, constant.key);
            if (!hashFloat_(ctx, constant.value))
                return false;
        }
        return true;
    }

    static bool hashVertex_(Context* ctx, const WGPUVertexState& vertex)
    {
        if (!hashStage_(ctx, vertex))
            return false;

        hash_(ctx, vertex.bufferCount);
        for (size_t i = 0; i < vertex.bufferCount; i++)
        {
            const WGPUVertexBufferLayout& buffer = vertex.buffers[i];
            if (!isPlain_(buffer.nextInChain))
                return false;

            hash_(ctx, buffer.stepMode);
            hash_(ctx, buffer.arrayStride);
            hash_(ctx, buffer.attributeCount);
            for (size_t j = 0; j < buffer.attributeCount; j++)
            {
                const WGPUVertexAttribute& attribute = buffer.attributes[j];
                if (!isPlain_(attribute.nextInChain))
                    return false;

                hash_(ctx, attribute.format);
                hash_(ctx, attribute.offset);
                hash_(ctx, attribute.shaderLocation);
            }
        }
        return true;
    }

    static bool hashFragment_(Context* ctx, const WGPUFragmentState* fragment)
    {
        hash_(ctx, fragment != nullptr);
        if (fragment == nullptr)
            return true;

        if (!hashStage_(ctx, *fragment))
            return false;

        hash_(ctx, fragment->targetCount);
        for (size_t i = 0; i < fragment->targetCount; i++)
        {
            const WGPUColorTargetState& target = fragment->targets[i];
            if (!isPlain_(target.nextInChain))
                return false;

            hash_(ctx, target.format);
            hash_(ctx, target.writeMask);
            hash_(ctx, target.blend != nullptr);
            if (target.blend != nullptr)
            {
                hashBlend_(ctx, target.blend->color);
                hashBlend_(ctx, target.blend->alpha);
            }
        }
        return true;
    }

    static void hashBlend_(Context* ctx, const WGPUBlendComponent& blend)
    {
        hash_(ctx, blend.operation);
        hash_(ctx, blend.srcFactor);
        hash_(ctx, blend.dstFactor);
    }

    static bool hashDepthStencil_(Context* ctx, const WGPUDepthStencilState* ds)
    {
        hash_(ctx, ds != nullptr);
        if (ds == nullptr)
            return true;

        if (!isPlain_(ds->nextInChain))
            return false;

        hash_(ctx, ds->format);
        hash_(ctx, ds->depthWriteEnabled);
        hash_(ctx, ds->depthCompare);
        hashStencilFace_(ctx, ds->stencilFront);
        hashStencilFace_(ctx, ds->stencilBack);
        hash_(ctx, ds->stencilReadMask);
        hash_(ctx, ds->stencilWriteMask);
        hash_(ctx, ds->depthBias);
        return hashFloat_(ctx, ds->depthBiasSlopeScale) && hashFloat_(ctx, ds->depthBiasClamp);
    }

    static void hashStencilFace_(Context* ctx, const WGPUStencilFaceState& face)
    {
        hash_(ctx, face.compare);
        hash_(ctx, face.failOp);
        hash_(ctx, face.depthFailOp);
        hash_(ctx, face.passOp);
    }
};

} // namespace sead
