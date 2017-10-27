#include "bytecode.h"
#include "util.h"

using namespace wc::util;

wc::bytecode::wcExecContext::wcExecContext()
{
	contextID = registers.pc = 0;
	registers.halt = false;
	targetPlatform = wcTargetPlatform::ct_bytecode;
}

wc::bytecode::wcInstruction::wcInstruction()
{
}

wc::bytecode::wcInstruction::wcInstruction(unsigned short int p_opcode)
{
	opcode = p_opcode;
}

wc::bytecode::wcInstructionPlusOperand::wcInstructionPlusOperand()
{

}

wc::bytecode::wcInstructionPlusOperand::wcInstructionPlusOperand(unsigned short int p_opcode, int p_op1)
{
	opcode = p_opcode;
	operand1 = p_op1;
}

wc::bytecode::wcInstructionPlusOperands::wcInstructionPlusOperands()
{
}

wc::bytecode::wcInstructionPlusOperands::wcInstructionPlusOperands(unsigned short int p_opcode, int p_op1, int p_op2)
{
	opcode = p_opcode;
	operand1 = p_op1;
	operand2 = p_op2;
}

void wc::bytecode::wcStack::push(int p_value)
{
	container.push_back(p_value);
}

int wc::bytecode::wcStack::pop()
{
	int returnValue = container[container.size() - 1];
	container.erase(container.end() - 1);
	return returnValue;
}

int wc::bytecode::wcStack::peek(int p_index)
{
	return container[p_index];
}

int wc::bytecode::wcStack::top()
{
	if (!container.size())
		return 0;
	return container[container.size()-1];
}

int wc::bytecode::wcStack::size()
{
	return container.size();
}

void wc::bytecode::wcStack::clear()
{
	container.clear();
}

std::string wc::bytecode::wcStringTable::getString(unsigned int p_index)
{
	if (intTable.find(p_index) == intTable.end())
		return "";
	return intTable.find(p_index)->second;
}

unsigned int wc::bytecode::wcStringTable::getIndex(std::string p_str)
{
	if (strTable.find(p_str) == strTable.end())
		return 0;
	return strTable.find(p_str)->second;
}

bool wc::bytecode::wcStringTable::doesIndexExist(unsigned int p_index)
{
	if (intTable.find(p_index) == intTable.end())
		return false;
	return true;
}

bool wc::bytecode::wcStringTable::doesStringExist(std::string p_str)
{
	if (strTable.find(p_str) == strTable.end())
		return false;
	return true;
}

unsigned int wc::bytecode::wcStringTable::addEntry(std::string p_str)
{
	unsigned int newIndex = randomUInt();
	while (doesIndexExist(newIndex))
		newIndex = randomUInt();

	intTable.insert(std::make_pair(newIndex, p_str));
	strTable.insert(std::make_pair(p_str, newIndex));

	return newIndex;
}
