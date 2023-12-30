#pragma once

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>

namespace sead {

// TODO
class FileDevice : public TListNode<FileDevice*>, public IDisposer
{
    SEAD_RTTI_BASE(FileDevice);

public:
    FileDevice(const SafeString&);
    ~FileDevice() override;

protected:
    FixedSafeString<32> mDriveName;
    bool mPermission;
};

} // namespace sead
