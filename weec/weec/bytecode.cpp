#include "bytecode.h"
#include "vm.h"
#include "util.h"

using namespace std;
using namespace wc::util;
using namespace wc::vm;
using namespace wc::bytecode;
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
	opcode = (wcOpcode)p_opcode;
}

wc::bytecode::wcInstructionPlusOperand::wcInstructionPlusOperand()
{

}

wc::bytecode::wcInstructionPlusOperand::wcInstructionPlusOperand(unsigned short int p_opcode, int p_op1)
{
	opcode = (wcOpcode)p_opcode;
	operand1 = p_op1;
}

wc::bytecode::wcInstructionPlusOperands::wcInstructionPlusOperands()
{
}

wc::bytecode::wcInstructionPlusOperands::wcInstructionPlusOperands(unsigned short int p_opcode, int p_op1, int p_op2)
{
	opcode = (wcOpcode)p_opcode;
	operand1 = p_op1;
	operand2 = p_op2;
}

void wc::bytecode::wcExecStack::push(wcChunk p_value)
{
	container.push_back(p_value);
}

wcChunk wc::bytecode::wcExecStack::pop()
{
	wcChunk returnValue = container[container.size() - 1];
	container.erase(container.end() - 1);
	return returnValue;
}

wcChunk& wc::bytecode::wcExecStack::peek(int p_index)
{
	return container[p_index];
}

wcChunk wc::bytecode::wcExecStack::top()
{
	if (!container.size())
		return wcChunk();
	return container[container.size()-1];
}

void wc::bytecode::wcExecStack::set(int index, wcChunk value)
{
	container[index] = value;
}

int wc::bytecode::wcExecStack::size()
{
	return container.size();
}

void wc::bytecode::wcExecStack::clear()
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

wc::bytecode::wcChunk::wcChunk()
{

}

int wc::bytecode::wcChunk::i()
{
	return 0;
}

float wc::bytecode::wcChunk::f()
{
	return 0.0f;
}

std::string wc::bytecode::wcChunk::s()
{
	return std::string();
}

wc::bytecode::wcChunki::wcChunki(int p_i)
{
	_i = p_i;
}

int wc::bytecode::wcChunki::i()
{
	return _i;
}

float wc::bytecode::wcChunki::f()
{
	if (_i >= 0)
		return int(_i + 0.5);
	return int(_i - 0.5);
}

std::string wc::bytecode::wcChunki::s()
{
	return itos(_i);
}

wc::bytecode::wcChunkf::wcChunkf(float p_f)
{
	_f = p_f;
}

int wc::bytecode::wcChunkf::i()
{
	return _f;
}

float wc::bytecode::wcChunkf::f()
{
	return _f;
}

std::string wc::bytecode::wcChunkf::s()
{
	return util::ftos(_f);
}

wc::bytecode::wcChunks::wcChunks(string p_s)
{
	_s = p_s;
}

int wc::bytecode::wcChunks::i()
{
	return stoi(_s);
}

float wc::bytecode::wcChunks::f()
{
	return stof(_s);
}

std::string wc::bytecode::wcChunks::s()
{
	return _s;
}

int& wc::bytecode::wcExecContextRegisters::operator[](int p_int)
{
	switch (p_int)
	{
	case 1:
		return pc;
	case 2:
		return t1;
	case 3:
		return t2;
	case 4:
		return cmp;
	case 5:
		return instr;
	default:
	case 6:
		return halt;
	}
}
