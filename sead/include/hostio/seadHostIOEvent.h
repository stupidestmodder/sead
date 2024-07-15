#pragma once

#include <hostio/seadHostIODefine.h>

namespace sead { namespace hostio {

struct NodeEvent
{
    u32 eventListenerID;
    NodeEventID id;
    u32 localNodeID;
    u32 extFlg;
    SafeString extText;
};

struct GenEvent
{
};

struct PropertyEvent
{
    u32 flag;
    u32 id;
    u32 eventListenerID;
    u32 localNodeID;
    u32 extFlg;
    s32 dataSize;
    const u8* data;
};

struct ExtensionFuncEvent
{
    u32 resultCode;
    s32 dataSize;
    const u8* data;
};

struct PaletteEventData
{
    u64 fileSize;
    SafeString filePath;
    u32 userEventID;
    SafeString userEventText;
};

struct PaletteEvent
{
    PaletteEventType type;
    SafeString paletteName;
    s32 numData;
    const PaletteEventData* data;
};

} } // namespace sead::hostio
