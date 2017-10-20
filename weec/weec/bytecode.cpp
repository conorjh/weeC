#include "bytecode.h"

wc::bytecode::wcExecContext::wcExecContext()
{
	contextID = pc = 0;
	halt = false;
	target = wcTargetPlatform::ct_bytecode;
}