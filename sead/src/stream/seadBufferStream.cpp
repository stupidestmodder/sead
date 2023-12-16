#include <stream/seadBufferStream.h>

#include <basis/seadAssert.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadMemUtil.h>

namespace sead {

BufferWriteStreamSrc::BufferWriteStreamSrc(StreamSrc* src, void* start, u32 size)
    : StreamSrc()
    , mStreamSrc(src)
    , mBufferAddr(static_cast<u8*>(start))
    , mBufferSize(size)
    , mCurrentPos(0)
{
    SEAD_ASSERT(mStreamSrc);
}

BufferWriteStreamSrc::~BufferWriteStreamSrc()
{
}

u32 BufferWriteStreamSrc::read(void*, u32)
{
    SEAD_ASSERT_MSG(false, "not supported");
    return 0;
}

u32 BufferWriteStreamSrc::write(const void* src, u32 size)
{
    u32 writeSize = 0;
    const u8* data = static_cast<const u8*>(src);

    do
    {
        if (mCurrentPos < mBufferSize)
        {
            u32 remainBufSize = mBufferSize - mCurrentPos;
            u32 writeStep = size - writeSize;

            if (writeStep > remainBufSize)
                writeStep = remainBufSize;

            MemUtil::copy(mBufferAddr + mCurrentPos, data + writeSize, writeStep);

            writeSize += writeStep;
            mCurrentPos += writeStep;
        }
    } while (writeSize < size && flush());

    return writeSize;
}

u32 BufferWriteStreamSrc::skip(s32)
{
    SEAD_ASSERT_MSG(false, "not supported");
    return 0;
}

void BufferWriteStreamSrc::rewind()
{
    flush();
    mStreamSrc->rewind();
}

bool BufferWriteStreamSrc::flush()
{
    if (mCurrentPos == 0)
        return true;

    u32 writeSize = mStreamSrc->write(mBufferAddr, mCurrentPos);
    SEAD_ASSERT_MSG(writeSize == mCurrentPos, "cannot write to stream.[%d/%d]", writeSize, mCurrentPos);

    bool full = writeSize >= mCurrentPos;
    mCurrentPos = 0;
    return full;
}

BufferMultiByteTextWriteStreamSrc::BufferMultiByteTextWriteStreamSrc(StreamSrc* src, void* start, u32 size)
    : BufferWriteStreamSrc(src, start, size)
{
    SEAD_ASSERT_MSG(size >= 4, "size[%u] must be larger or equal than 4", size);
}

u32 BufferMultiByteTextWriteStreamSrc::write(const void* src, u32 size)
{
    u32 writeSize = 0;
    const u8* data = static_cast<const u8*>(src);

    do
    {
        if (mCurrentPos < mBufferSize)
        {
            u32 remainBufSize = mBufferSize - mCurrentPos;
            u32 writeStep = size - writeSize;

            if (writeStep > remainBufSize)
            {
                writeStep = remainBufSize;

                u32 characterOffset = 0;

                u8 lastByte = data[writeSize + remainBufSize - 1];
                if ((lastByte & 0x80) != 0)
                {
                    if ((lastByte & 0xC0) == 0x80)
                    {
                        for (s32 i = 2; i <= Mathi::min(remainBufSize, 4); i++)
                        {
                            lastByte = data[writeSize + remainBufSize - i];
                            if ((lastByte & 0xC0) != 0x80)
                            {
                                s32 multiByteLen = 0;
                                if ((lastByte & 0xE0) == 0xC0)
                                    multiByteLen = 2;
                                else if ((lastByte & 0xF0) == 0xE0)
                                    multiByteLen = 3;
                                else if ((lastByte & 0xF8) == 0xF0)
                                    multiByteLen = 4;

                                if (multiByteLen > i)
                                    characterOffset = i;

                                break;
                            }
                        }
                    }
                    else
                    {
                        characterOffset = 1;
                    }
                }

                if (characterOffset > 0)
                {
                    writeStep = remainBufSize - characterOffset;
                    mBufferAddr[mBufferSize - characterOffset] = '\0';
                }
            }

            MemUtil::copy(mBufferAddr + mCurrentPos, data + writeSize, writeStep);

            writeSize += writeStep;
            mCurrentPos += writeStep;
        }
    } while (writeSize < size && flush());

    return writeSize;
}

bool BufferMultiByteNullTerminatedTextWriteStreamSrc::flush()
{
    SEAD_ASSERT(mCurrentPos <= mBufferSize);

    mBufferAddr[mCurrentPos] = '\0';
    return BufferWriteStreamSrc::flush();
}

} // namespace sead
