#include <hostio/win/seadHostIOPortDirectConnectWin.h>

#include <thread/seadDelegateThread.h>

namespace sead { namespace hostio {

WinPortDirectConnect::WinPortDirectConnect(Heap* heap, u32 recvBufSize, u32 portNumber, const char* threadName)
    : PortBase()
    , mState(State::eIdle)
    , mPortNumber(portNumber)
    , mRecvBuffer(new(heap) u8[recvBufSize])
    , mRecvBufSize(recvBufSize)
    , mReadedSize(0)
    , mBuffer(nullptr)
    , mSock(INVALID_SOCKET)
    , mSock0(INVALID_SOCKET)
    , mThread(nullptr)
{
    mThread = new(heap) DelegateThread(
        threadName, new(heap) Delegate2<WinPortDirectConnect, Thread*, MessageQueue::Element>(this, &WinPortDirectConnect::threadFunc_), heap,
        Thread::cDefaultPriority, MessageQueue::BlockType::eNoBlock
    );

    mThread->start();
}

WinPortDirectConnect::~WinPortDirectConnect()
{
    shutdown(mSock, SD_BOTH);
    closesocket(mSock);
    mSock = INVALID_SOCKET;

    shutdown(mSock0, SD_BOTH);
    closesocket(mSock0);
    mSock0 = INVALID_SOCKET;

    mThread->quitAndDestroySingleThread(false);
    delete mThread;

    delete[] mRecvBuffer;
}

s32 WinPortDirectConnect::send(const void* buf, s32 len)
{
    if (mState == State::eConnected)
    {
        for (s32 len_ = len; len_ > 0; )
        {
            s32 sendLen = len_ > 0x40000 ? 0x40000 : len_;
            s32 actualSendLen = ::send(mSock, static_cast<const char*>(PtrUtil::addOffset(buf, len - len_)), sendLen, 0);
            if (actualSendLen != sendLen)
            {
                closesocket(mSock);
                mSock = INVALID_SOCKET;
                mState = State::eOpened;
                return -1;
            }

            len_ -= actualSendLen;
        }
    }

    return len;
}

s32 WinPortDirectConnect::recv(void* buf, s32 len)
{
    SEAD_ASSERT(len <= mReadedSize);

    memcpy(buf, mBuffer, len);
    mBuffer += len;
    mReadedSize -= len;

    return len;
}

s32 WinPortDirectConnect::getAvailableSize()
{
    return mReadedSize;
}

void WinPortDirectConnect::polling()
{
    if (mState != State::eConnected || mReadedSize != 0)
    {
        return;
    }

    fd_set set;
    set.fd_array[0] = mSock;
    set.fd_count = 1;

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    ::select(0, &set, nullptr, nullptr, &timeout);
    if (__WSAFDIsSet(mSock, &set) == 0)
    {
        return;
    }

    s32 recvSize = ::recv(mSock, reinterpret_cast<char*>(mRecvBuffer), mRecvBufSize, 0);
    if (recvSize == SOCKET_ERROR || recvSize == 0)
    {
        ::closesocket(mSock);
        mSock = INVALID_SOCKET;

        mState = State::eOpened;
        return;
    }

    mReadedSize = recvSize;
    mBuffer = mRecvBuffer;
}

bool WinPortDirectConnect::isConnecting() const
{
    return mState == State::eConnected;
}

Thread* WinPortDirectConnect::getThread()
{
    return mThread;
}

void WinPortDirectConnect::threadFunc_(Thread*, MessageQueue::Element)
{
    switch (mState)
    {
        case State::eIdle:
        {
            mSock0 = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = ::htons(mPortNumber);
            addr.sin_addr.S_un.S_addr = ::inet_addr("127.0.0.1");
            ::bind(mSock0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            ::listen(mSock0, 5);

            mState = State::eOpened;
            break;
        }

        case State::eOpened:
        {
            sockaddr_in client;
            s32 len = sizeof(client);

            mSock = ::accept(mSock0, reinterpret_cast<sockaddr*>(&client), &len);
            if (mSock != INVALID_SOCKET)
            {
                mReadedSize = 0;
                mState = State::eConnected;
            }

            break;
        }
    }

    Thread::sleep(TickSpan::makeFromMilliSeconds(10));
}

} } // namespace sead::hostio
