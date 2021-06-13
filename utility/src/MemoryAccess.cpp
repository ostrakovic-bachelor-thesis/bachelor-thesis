#include "MemoryAccess.h"


#ifdef UNIT_TEST_DRIVER
NiceMock<MemoryAccessHook> *MemoryAccess::s_memoryAccessHookPtr = nullptr;
#endif // #ifdef UNIT_TEST_DRIVER
