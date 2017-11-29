#include "vm.h"
#include "util.h"

using namespace std;
using namespace wc;
using namespace wc::bytecode;
using namespace wc::vm;
using namespace wc::util;
int wc::vm::wcBaseVM::execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr)
{

	return 0;
}

wc::vm::wcBaseVM::wcBaseVM()
{

}

int wc::vm::wcBaseVM::exec(int p_handle)
{
	if (!handleExists(p_handle))
		return -1;

	wcExecContext& con = conPool[p_handle];

	while (!con.execStopped())
		execInstruction(con, con.getInstr());

	return 0;
}

int wc::vm::wcBaseVM::load(wcExecContext& p_input)
{
	//look for new handle
	int possNewHandle = newHandle();
	if (possNewHandle < 0)
		return -1;

	//add to pool
	conPool.insert(make_pair(possNewHandle, p_input));

	//return new handle
	return possNewHandle;
}

bool wc::vm::wcBaseVM::remove(int p_handle)
{
	if (!handleExists(p_handle))
		return false;

	conPool.erase(p_handle);
	return true;
}

int wc::vm::wcBaseVM::newHandle()
{
	int possNewHandle = randomUInt();
	while (handleExists(possNewHandle))
		possNewHandle = randomUInt();
	return possNewHandle;
}
bool wc::vm::wcBaseVM::handleExists(int p_handle)
{
	if (conPool.find(p_handle) != conPool.end())
		return true;
	return false;
}

void wc::vm::wcBaseVM::removeHandle(int p_handle)
{
	if (handleExists(p_handle))
		removeHandle(p_handle);
}

