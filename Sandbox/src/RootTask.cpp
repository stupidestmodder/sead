#include <RootTask.h>

#include <container/seadSafeArray.h>
#include <container/seadTList.h>
#include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <controller/seadAccelerometerAddon.h>
#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>
#include <controller/seadControllerWrapper.h>
#include <framework/seadFramework.h>
#include <framework/seadMethodTreeMgr.h>
#include <random/seadGlobalRandom.h>
#include <time/seadDateTime.h>
#include <time/seadTickTime.h>

struct Node
{
    // sead::ListNode node;
    s32 value;
};

void PrintList(const sead::TList<Node>& list)
{
    SEAD_PRINT("List (%d):\n", list.size());

    u32 i = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        SEAD_PRINT(" - list[%d] = %d\n", i, it->value);

        i++;
    }
}

void PrintList(const sead::TList<s32>& list)
{
    SEAD_PRINT("List (%d):\n", list.size());

    u32 i = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        SEAD_PRINT(" - list[%d] = %d\n", i, *it);

        i++;
    }
}

void PrintArray(const sead::UnsafeArray<s32, 10>& list)
{
    SEAD_PRINT("Array (%d):\n", list.size());

    u32 i = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        SEAD_PRINT(" - array[%d] = %d\n", i, *it);

        i++;
    }
}

RootTask::RootTask(const sead::TaskConstructArg& arg)
    : sead::CalculateTask(arg, "RootTask")
{
}

void RootTask::prepare()
{
    {
        sead::Framework::CreateSystemTaskArg arg;
        getFramework()->createSystemTasks(this, arg);
    }

    // sead::TList<Node> list;
    sead::TList<s32> list;
    // list.initOffset(offsetof(Node, node));

    for (s32 i = 0; i < 10; i++)
    {
        s32 value = sead::GlobalRandom::instance()->getS32Range(0, 50);

        // Node* node = new Node();
        // node->value = value;

        sead::TListNode<s32>* node = new sead::TListNode<s32>();
        node->val() = value;

        list.pushBack(node);
    }

    PrintList(list);

    sead::TickTime start;

    // list.mergeSort([](const Node* a, const Node* b) -> s32
    // {
    //     return a->value - b->value;
    // });

    list.mergeSort();

    sead::TickSpan span = start.diffToNow();

    SEAD_PRINT("Sort took %lld nano, %lld micro, %lld milli seconds\n", span.toNanoSeconds(), span.toMicroSeconds(), span.toMilliSeconds());

    PrintList(list);

    // list.uniq([](const Node* a, const Node* b) -> s32
    // {
    //     return a->value - b->value;
    // });

    list.uniq();

    PrintList(list);

    sead::UnsafeArray<s32, 10> arr;
    for (s32 i = 0; i < arr.size(); i++)
    {
        arr[i] = i;
    }

    PrintArray(arr);

    adjustHeapAll();
}

sead::ControllerWrapper ctrl;

void RootTask::enter()
{
    sead::MethodTreeNode* node = getMethodTreeMgr()->getRootMethodTreeNode(0);
    SEAD_PRINT("Root(%s)\n", node->getName().cstr());

    // for (auto it = node->childBegin(); it != node->childEnd(); ++it)
    // {
    //     SEAD_PRINT("Node(%s)\n", (*it)->getName().cstr());
    // }

    sead::Controller* controller = sead::ControllerMgr::instance()->getController(0);
    ctrl.registerWith(controller, true);
}

void RootTask::exit()
{
}

void RootTask::calc()
{
    //SEAD_PRINT("RootTask::calc()\n");

    sead::KeyboardMouseDevice* device = sead::DynamicCast<sead::KeyboardMouseDevice>(sead::ControllerMgr::instance()->getControlDevice(sead::ControllerDefine::DeviceId::eKeyboardMouse));

    // if (device->isVkeyHold(VK_UP))
    // {
    //     // SEAD_PRINT("HOLD\n");
    // }

    // if (device->isVkeyTrig(VK_UP))
    // {
    //     SEAD_PRINT("TRIG\n");
    // }

    // if (device->isVkeyRelease(VK_UP))
    // {
    //     SEAD_PRINT("RELEASE\n");
    // }

    // if (device->isVkeyRepeat(VK_UP))
    // {
    //     SEAD_PRINT("REPEAT\n");
    // }

    u32 button = sead::Controller::PadMask::eMaskTouch;
    if (ctrl.isHold(button))
    {
        // SEAD_PRINT("HOLD\n");
    }

    if (ctrl.isTrig(button))
    {
        SEAD_PRINT("TRIG\n");
    }

    if (ctrl.isRelease(button))
    {
        SEAD_PRINT("RELEASE\n");
    }

    if (ctrl.isRepeat(button))
    {
        SEAD_PRINT("REPEAT\n");
    }

    sead::AccelerometerAddon* addon = sead::DynamicCast<sead::AccelerometerAddon>(ctrl.getController()->getAddon(sead::ControllerDefine::AddonId::eAccelerometer));
    if (addon && addon->isEnable())
    {
        // SEAD_PRINT("Acceleration(%f, %f, %f)\n", addon->getAcceleration().x, addon->getAcceleration().y, addon->getAcceleration().z);
    }

    // SEAD_PRINT("Pointer(%f, %f)\n", ctrl.getController()->getPointer().x, ctrl.getController()->getPointer().y);
    if (ctrl.getController()->isPointerOn())
    {
        // SEAD_PRINT("PointerOn\n");
    }

    if (ctrl.getController()->isPointerOnNow())
    {
        // SEAD_PRINT("PointerOnNow\n");
    }

    if (ctrl.getController()->isPointerOffNow())
    {
        // SEAD_PRINT("PointerOffNow\n");
    }

    sead::CalendarTime ct;
    sead::DateTime dt(0);
    dt.setNow();
    dt.getCalendarTime(&ct);

    SEAD_PRINT("Current time: %04d/%02d/%02d %02d:%02d:%02d (%s) (%d)\n",
               ct.getYear(), ct.getMonth(), ct.getDay(), ct.getHour(), ct.getMinute(), ct.getSecond(),
               ct.getMonth().getString().cstr(), ct.getWeekDay());
}
