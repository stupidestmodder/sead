#pragma once

#include <hostio/seadHostIOPortBase.h>
#include <thread/seadMessageQueue.h>

#include <basis/win/seadWindows.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

namespace sead {

class DelegateThread;
class Heap;

namespace hostio {

class WinPortDirectConnect : public PortBase
{
public:
    enum class State
    {
        eIdle,
        eOpened,
        eConnected
    };

public:
    WinPortDirectConnect(Heap* heap, u32 recvBufSize, u32 portNumber, const char* threadName);
    ~WinPortDirectConnect() override;

    s32 send(const void* buf, s32 len) override;
    s32 recv(void* buf, s32 len) override;
    s32 getAvailableSize() override;
    void polling() override;
    bool isConnecting() const override;
    Thread* getThread() override;

private:
    void threadFunc_(Thread* thread, MessageQueue::Element msg);

private:
    State mState;
    u32 mPortNumber;
    u8* mRecvBuffer;
    u32 mRecvBufSize;
    s32 mReadedSize;
    u8* mBuffer;
    SOCKET mSock;
    SOCKET mSock0;
    DelegateThread* mThread;
};

} } // namespace sead::hostio
