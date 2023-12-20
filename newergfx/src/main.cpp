#include <basis/seadRawPrint.h>
#include <framework/seadFramework.h>

int main()
{
    SEAD_PRINT("Program Start\n");

    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 0x50000;
    sead::Framework::initialize(initArg);

    SEAD_PRINT("Program End\n");
    return 0;
}
