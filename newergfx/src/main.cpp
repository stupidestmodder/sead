#include <basis/seadRawPrint.h>

#include <container/seadTList.h>

struct A
{
    using List = sead::TList<A*>;
    using ListNode = sead::TListNode<A*>;

    ListNode listNode;
};

void PrintList(const A::List& list)
{
    SEAD_PRINT("List Size: %d\n", list.size());

    s32 count = 0;
    for (auto& a : list)
    {
        SEAD_PRINT("  [%d]: 0x%p\n", count, &a);
        count++;
    }
}

int main()
{
    SEAD_PRINT("Program Start\n");

    A::List list;

    A a;
    A b;

    list.pushBack(&a.listNode);
    list.pushBack(&b.listNode);

    PrintList(list);

    return 0;
}
