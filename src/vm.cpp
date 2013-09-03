#include "vm.h"
#include "util.h"/*
bcVM::bcVM()
{


}
//convert operand type to RTVal type
bcValType bcVM::ottort(bcOperandType _type)
{
	switch(_type)
	{
	case ot_null:
		return rt_null;
		break;

	case ot_int:
		return rt_int;
		break;

	case ot_string:
		return rt_string;
		break;

	case ot_float:
		return rt_float;
		break;

	case ot_object:
		return rt_object;
		break;

	case ot_bool:
		return rt_bool;
		break;

	case ot_stackindex:
		return rt_stackindex;
		break;

	case ot_stackindextop:
		return rt_stackindextop;
		break;

	default:
		return rt_null;
		break;
	}
	
}

bool bcVM::Add(bcVal* val1, bcVal* val2, bcVal* dest)
{
	switch(val1->type)
	{

	
	case rt_stackindex:

		break;

	case rt_stackindextop:

		break;
	
	case rt_int:
		dest->data = bcitos( bcstoi(val1->data) + bcstoi(val2->data) );
		return true;
		break;
	}

	return true;
}

bcVM::bcVM(vector<bcInstr> _istream)
{
	Init(_istream);
}

int bcVM::Init(vector<bcInstr> _istream)
{
	istream=_istream;
	pc=istream.begin();
	return 1;
}

int bcVM::Run()
{
	int run=1;
	while(run)
	{
		run=RunInstr();
		if(pc==istream.end()) run=false;		
	}
	return 1;
}

int bcVM::RunInstr()
{
	switch(pc->opcode)
	{
	case oc_mov:
		RunInstr_mov();
		break;
	case oc_push:
		RunInstr_push();
		break;
	case oc_pop:
		RunInstr_pop();
		break;
	case oc_call:
		RunInstr_call();
		break;

	case oc_add:
		RunInstr_add();
		break;
	case oc_sub:
		RunInstr_sub();
		break;
	case oc_div:
		RunInstr_div();
		break; 
	case oc_mult:
		RunInstr_mult();
		break; 
	case oc_pow:
		RunInstr_pow();
		break; 
	case oc_mod:
		RunInstr_mod();
		break; 
	case oc_inc:
		RunInstr_inc();
		break; 
	case oc_dec:
		RunInstr_dec();
		break; 
	case oc_ltn:
		RunInstr_ltn();
		break; 
	case oc_gtn:
		RunInstr_gtn();
		break; 
	case oc_leq:
		RunInstr_leq();
		break; 
	case oc_geq:
		RunInstr_geq();
		break; 
	case oc_eq:
		RunInstr_eq();
		break; 
	case oc_neq:
		RunInstr_neq();
		break; 
	case oc_land:
		RunInstr_land();
		break; 
	case oc_lor:
		RunInstr_lor();
		break;
	}
	pc++;
	return 1;
}

int bcVM::RunInstr_mov()
{ 
	bcVal* from;
	//move source (the thing we are moving)
	switch(pc->operands.at(0).type)
	{
		case ot_stackindex:

			return 1;

		case ot_stackindextop:
			from=&stk.top();
			return 1;
	default:

		break;
	}

	//move destination (where we are moving the thing to)
	return 1; 
}
int bcVM::RunInstr_push()
{ 
	bcVal val;
	
	switch(pc->operands.at(0).type)
	{
	case ot_int:
	case ot_stackindex:
		val.data=pc->operands.at(0).data;
		val.type=ottort(pc->operands.at(0).type);
		stk.push(val);
		break;

	default:
		val.data=pc->operands.at(0).data;
		val.type=ottort(pc->operands.at(0).type);
		stk.push(val);
		break;
	}

	return 1; 
}
int bcVM::RunInstr_pop()
{ 
	stk.pop();

	return 1; 
}
int bcVM::RunInstr_call()
{ 
	return 1; 
}
int bcVM::RunInstr_add()
{ 
	bcVal val1,val2;

	//pop the two top values
	val1=stk.top();
	stk.pop();
	val2=stk.top();
	stk.pop();

	//add them together, put the result in the second stack item
	if(!Add(&val1,&val2,&val2))
	{
		//error
		return 0;
	}

	//return the result, discard the first stack item
	stk.push(val2);

	return 1; 
}
int bcVM::RunInstr_sub()
{ 
	return 1; 
}
int bcVM::RunInstr_div()
{ 
	return 1; 
}
int bcVM::RunInstr_mult()
{ 
	return 1; 
}
int bcVM::RunInstr_pow()
{ 
	return 1; 
}
int bcVM::RunInstr_mod()
{ 
	return 1; 
}
int bcVM::RunInstr_inc()
{ 
	return 1; 
}
int bcVM::RunInstr_dec()
{ 
	return 1; 
}
int bcVM::RunInstr_ltn()
{
	return 1; 
}
int bcVM::RunInstr_gtn()
{
	return 1; 
}
int bcVM::RunInstr_leq()
{
	return 1; 
}
int bcVM::RunInstr_geq()
{
	return 1; 
}
int bcVM::RunInstr_eq()
{
	return 1; 
}
int bcVM::RunInstr_neq()
{
	return 1; 
}
int bcVM::RunInstr_land()
{ 
	return 1; 
}
int bcVM::RunInstr_lor()
{
	return 1; 
}*/