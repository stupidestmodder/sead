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

const s32 cRECV_BUFF_COUNT = 2;
const s32 cMAX_ROOT_NODE = 4;
const s32 cMAX_PALETTE_ENTRY = 8;        //? Official name not known
const s32 cMAX_SECURITY_UNLOCK_FILE = 4; //? Official name not known

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

    void createPortDirectConnect(const Config* config);
    void polling();
    bool dispatchMessage();
    void setRootNode(const SafeString& name, Reflexible* obj, const SafeString& meta);
    Reflexible* getRootNode(s32 ix);
    Reflexible* searchNode(const SafeString& url, BufferedSafeString* restUrl = nullptr);
    Context* attachContext(Command command);
    void releaseContext(Context* context);

    PortBase* getPort()
    {
        return mPort;
    }

    void entryPalette(const SafeString& name, const SafeString& func, PaletteEventListener* listener, const SafeString& meta, const SafeString& metaFile);
    void removePalette(const SafeString& name);
    void updatePaletteMeta(const SafeString& name, const SafeString& meta, const SafeString& metaFile);
    void addSecurityUnlockFile(const SafeString& path);
    Context* setExternalContext(Context* context);

    Server(Heap* heap, const Config* config, void* sendBuffer, s32 sendBufferSize);
    ~Server();

    void receiveData(Command command, s8* buf, s32 len);
    s32 sendData(Command command, const void* buf, s32 len);

private:
    void sendRootNode_();
    void getObjInfo_(u32 createID, u32 createIDOR);
    void sendExtensionFuncPalatteEntry_();
    void callPaletteEventListenr_(RamReadStream* reader, PaletteEventType type, PaletteEntry* entry, PaletteEventListener* listener);
    void sendSecurityUnlockFilePath_();

private:
    PortBase* mPort;
    bool mAnalysisPacket;
    PacketHeader mHeader;
    Command mAttachedCommand;
    Heap* mHeap;
    Context mContext;
    Context* mExternalContext;
    u32 mRootNodeCreateID[cMAX_ROOT_NODE];
    bool mIsRootNodeUpdate;
    s8* mRecvBuf[cRECV_BUFF_COUNT];
    s32 mRecvBufSize;
    s32 mRecvBufCur;
    s32 mReadingDataSize[cRECV_BUFF_COUNT];
    PaletteEntry mPaletteEntries[cMAX_PALETTE_ENTRY];
    FixedSafeString<256> mSecurityUnlockFiles[cMAX_SECURITY_UNLOCK_FILE];

    static Server* sInstance;
};

} } // namespace sead::hostio
