#pragma once

namespace sead { namespace hostio {

// TODO
class Config
{
public:
    Config();

private:
    u32 mSendBufferSize;
    u32 mRecvBufferSize;
    u32 mMaxRecvDataSize;
    u32 mWinTCPPortNumber;
    u32 mCTRSerialChannel;
    u32 mRvlMcsChannel;
    u32 mORBehavior;
};

} } // namespace sead::hostio
