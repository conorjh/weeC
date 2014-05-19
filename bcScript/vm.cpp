#include "vm.h"

using namespace bc::vm;
using bc::vm::bcReg;

bcVM::bcVM()
{
	//con->halt = false;
	//con->reg[pc].val = 0;
}

/*void bcVM::load(bcByteCodeGen* g)
{
	con->istream = *g->istream;
	con->fstream = *g->fstream;
}*/

unsigned int bcVM::exec(unsigned int instructions)
{
	while(con->istream.size() && !con->halt && instructions>0)
	{
		switch(con->istream[con->reg[pc].val].op)
		{
		case oc_mov:
			switch(con->istream[con->reg[pc].val].arg1.type)
			{
				//case bcValType::vt_stack
				default:
					break;
			}
			break;
		case oc_push:
			con->stack.push(con->istream[con->reg[pc].val].arg1.val);
			break;
		case oc_pop:
			con->stack.pop();
			break;
		case oc_je: 
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val == con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break;
		case oc_jne: 
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val != con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break; 
		case oc_jg: 
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val > con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break; 
		case oc_jl: 
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val < con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break; 
		case oc_jge:  
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val >= con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break;
		case oc_jle:  
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			if(con->reg[t1].val <= con->reg[t2].val)
				con->reg[pc].val = con->istream[pc].arg2.val;			
			break;
		case oc_plus: 
			con->reg[con->reg[t2].val].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val+con->reg[t2].val);
			break;
		case oc_minus: 
			con->reg[t2].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val-con->reg[t2].val);
			break; 
		case oc_mult: 
			con->reg[t2].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val*con->reg[t2].val);
			break; 
		case oc_div: 
			con->reg[t2].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val/con->reg[t2].val);
			break; 
		case oc_expo: 
			con->reg[t2].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val^con->reg[t2].val);
			break;
		case oc_mod: 
			con->reg[t2].val = con->stack.top();
			con->stack.pop();
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val%con->reg[t2].val);
			break;
		case oc_inc:
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val++);
			break;
		case oc_dec: 
			con->reg[t1].val = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1].val--);
			break;
		case oc_and: 
		case oc_or: 
		case oc_xor: 
		case oc_not: 
		case oc_shfl:
		case oc_shfr: 
		case oc_call:
		case oc_ret: 
		case oc_callvm: 
		case oc_pause:
		case oc_halt:
		default:
				break;
		}
		
		//inc program counter
		con->reg[pc].val++;
		if(con->reg[pc].val>= con->istream.size())
		{
			con->halt = true;
			break;
		}
		--instructions;
	}

	return true;
}