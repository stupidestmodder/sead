#include <devenv/seadSeadMenuMgr.h>

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(SeadMenuMgr);

SeadMenuMgr::SeadMenuMgr(const TaskConstructArg& arg)
    : Task(arg, "sead::SeadMenuMgr")
{
}

} // namespace sead
