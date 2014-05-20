#include "vm.h"

using namespace bc::vm;
using bc::vm::bcReg;

bcVM::bcVM()
{
	//con->halt = false;
	//con->reg[pc] = 0;
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
		switch(con->istream[con->reg[pc]].op)
		{
		case oc_mov:
			//con->stack.
			break;
		case oc_push:
			con->stack.push(con->istream[con->reg[pc]].arg1);
			break;
		case oc_pop:
			con->stack.pop();
			break;
		case oc_cmp: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->reg[cmp1] = con->reg[t1] - con->reg[t2];
			break;
		case oc_je: 
			if(!con->reg[cmp1])
				con->reg[pc] = con->istream[pc].arg2;			
			break;
		case oc_jne:
			if(con->reg[cmp1])
				con->reg[pc] = con->istream[pc].arg2;			
			break; 
		case oc_jg: 
			if(con->reg[cmp1]>0)
				con->reg[pc] = con->istream[pc].arg2;			
			break; 
		case oc_jl: 
			if(con->reg[cmp1]<0)
				con->reg[pc] = con->istream[pc].arg2;				
			break; 
		case oc_jge:  
			if(con->reg[cmp1]>=0)
				con->reg[pc] = con->istream[pc].arg2;	
			break;
		case oc_jle:  
			if(con->reg[cmp1]<=0)
				con->reg[pc] = con->istream[pc].arg2;			
			break;
		case oc_plus: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]+con->reg[t2]);
			break;
		case oc_minus: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]-con->reg[t2]);
			break; 
		case oc_mult: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]*con->reg[t2]);
			break; 
		case oc_div: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]/con->reg[t2]);
			break; 
		case oc_expo: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]^con->reg[t2]);
			break;
		case oc_mod: 
			con->reg[t2] = con->stack.top();
			con->stack.pop();
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]%con->reg[t2]);
			break;
		case oc_inc:
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]++);
			break;
		case oc_dec: 
			con->reg[t1] = con->stack.top();
			con->stack.pop();
			con->stack.push(con->reg[t1]--);
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

		case oc_nop:
		default:
				break;
		}
		
		//inc program counter
		con->reg[pc]++;
		if(con->reg[pc] >= con->istream.size())
		{
			con->halt = true;
			break;
		}
		--instructions;
	}

	return true;
}