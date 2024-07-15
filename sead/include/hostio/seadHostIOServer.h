#pragma once

#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIODefine.h>
#include <hostio/seadHostIOPortBase.h>

namespace sead {

class Heap;
class RamReadStream;

namespace hostio {

class Config;
class PaletteEventListener;
class PortBase;

// TODO
class Server
{
public:
    struct PaletteEntry
    {
        SafeString name;
        SafeString func;
        SafeString meta;
        SafeString metaFile;
        u32 listenerID;
    };

public:
    static void createInstance(Heap* heap, const Config* config);
    static void deleteInstance();
    static Server* getInstance();

    static bool isInitialized();

    void createPortDirectConnect(const Config*);
    void polling();
    bool dispatchMessage();
    void setRootNode(const SafeString&, Reflexible*, const SafeString&);
    Reflexible* getRootNode(s32);
    void searchNode();
    Context* attachContext(Command);
    void releaseContext(Context*);
    void getPort();
    void entryPalette(const SafeString&, const SafeString&, PaletteEventListener*, const SafeString&, const SafeString&);
    void removePalette(const SafeString&);
    void updatePaletteMeta(const SafeString&, const SafeString&, const SafeString&);
    void addSecurityUnlockFile(const SafeString&);
    void setExternalContext();

    Server(Heap*, const Config*, void*, s32);
    ~Server();

    void receiveData(Command, s8*, s32);
    s32 sendData(Command, const void*, s32);

private:
    void sendRootNode_();
    void getObjInfo_();
    void sendExtensionFuncPalatteEntry_();
    void callPaletteEventListenr_(RamReadStream*, PaletteEventType, PaletteEntry*, PaletteEventListener*);
    void sendSecurityUnlockFilePath_();

private:
    PortBase* mPort;
    bool mAnalysisPacket;
    PacketHeader mHeader;
    Command mAttachedCommand;
    Heap* mHeap;
    Context mContext;
    Context* mExternalContext;
    u32 mRootNodeCreateID[4];
    bool mIsRootNodeUpdate;
    s8* mRecvBuf[2];
    s32 mRecvBufSize;
    s32 mRecvBufCur;
    s32 mReadingDataSize[2];
    PaletteEntry mPaletteEntries[8];
    FixedSafeString<256> mSecurityUnlockFiles[4];

    static Server* sInstance;
};

} } // namespace sead::hostio
