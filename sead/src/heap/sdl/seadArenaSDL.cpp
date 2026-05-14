#include <heap/seadArena.h>

#include <basis/seadAssert.h>

#include <cstdlib>

namespace sead {

Arena::Arena()
    : mStart(nullptr)
    , mSize(0)
    , mInitWithStartAddress(false)
{
}

Arena::~Arena()
{
}

void Arena::initialize(size_t size)
{
    SEAD_ASSERT_MSG(!mStart, "initialize twice");

    mSize = size;
    mStart = static_cast<u8*>(std::malloc(mSize));
    mInitWithStartAddress = false;
}

void Arena::initialize(u8* start, size_t size)
{
    SEAD_ASSERT_MSG(!mStart, "initialize twice");

    mSize = size;
    mStart = start;
    mInitWithStartAddress = true;
}

void Arena::destroy()
{
    SEAD_ASSERT_MSG(mStart, "not intialized");

    if (!mInitWithStartAddress)
        std::free(mStart);

    mStart = nullptr;
    mSize = 0;
    mInitWithStartAddress = false;
}

} // namespace sead
