#pragma once

#include <prim/seadSafeString.h>

namespace sead { namespace hostio {

enum class Command
{
    ePing,
    eGameReset,
    eGetRootNode,
    ePutRootNode,
    eGetObjInfo,
    ePutObjInfo,
    ePropertyEvent,
    ePutUpdateProperties,
    eNodeEvent,
    eExecExtensionFunc,
    eExtensionFuncResult,
    eAddPalette,
    eRemovePalette,
    ePaletteEvent,
    eRemoteControl,
    eFileDialog,
    eFileDialogResut,
    eMessageBox,
    eMessageBoxResut,
    eUpdatePaletteMeta,
    eFolderDialog,
    eFolderDialogResut,
    eAddSecurityUnlockFile,
    eForceSetFolderCache,
    eInputBox,
    eTextResut,
    eNumCommand
};

enum class NodeEventID
{
    eSelected,
    eExpanded,
    eCollapsed,
    eLocalNodeSelected,
    eLocalNodeExpanded,
    eLocalNodeCollapsed,
    eChildDisposed,
    eTabSelected,
    eLocalNodeTabSelected
};

enum class PaletteEventType
{
    eExecute,
    eFileDrop,
    eUserEvent
};

struct FileInfo
{
    SafeString path;
    u64 size;
};

} } // namespace sead::hostio
