#include "vm.h"

using namespace bc::vm;
using bc::vm::bcReg;

bcVM::bcVM()
{
//	con->stack.ec=con;
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
		int* rpc = &con->reg[pc];		//shorthand for the program counter
		switch(con->istream[*rpc].op)
		{
		case oc_mov:
			ocMov(con);			
			break;
		case oc_push:
			ocPush(con);
			con->stack.push(con,con->istream[*rpc].arg1);
			break;
		case oc_pop:
			ocPop(con);
			break;
		case oc_cmp: 
			ocCmp(con);
			break;
		case oc_jmp:
			ocJmp(con);
		case oc_je: 
			ocJe(con);
			break;
		case oc_jne:
			if(con->reg[cmp1])
				*rpc = con->istream[*rpc].arg2;			
			break; 
		case oc_jg: 
			if(con->reg[cmp1]>0)
				*rpc = con->istream[*rpc].arg2;			
			break; 
		case oc_jl: 
			if(con->reg[cmp1]<0)
				*rpc = con->istream[*rpc].arg2;				
			break; 
		case oc_jge:  
			if(con->reg[cmp1]>=0)
				*rpc = con->istream[*rpc].arg2;	
			break;
		case oc_jle:  
			if(con->reg[cmp1]<=0)
				*rpc = con->istream[*rpc].arg2;			
			break;
		case oc_plus: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]+con->reg[t2]);
			break;
		case oc_minus: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]-con->reg[t2]);
			break; 
		case oc_mult: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]*con->reg[t2]);
			break; 
		case oc_div: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]/con->reg[t2]);
			break; 
		case oc_expo: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]^con->reg[t2]);
			break;
		case oc_mod: 
			con->reg[t2] = con->stack.top();
			con->stack.pop(con);
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]%con->reg[t2]);
			break;
		case oc_inc:
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]++);
			break;
		case oc_dec: 
			con->reg[t1] = con->stack.top();
			con->stack.pop(con);
			con->stack.push(con,con->reg[t1]--);
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
		(*rpc)++;
		if(*rpc >= con->istream.size())
		{
			con->halt = true;
			break;
		}
		--instructions;
	}

	return true;
}

inline void bc::vm::ocMov(bcExecContext* ec)
{
	*ec->stack.at(ec->istream[ ec->reg[pc] ].arg1) = *ec->stack.at(ec->istream[ ec->reg[pc] ].arg2);
}

//load register arg1, with value arg2
inline void bc::vm::ocLr(bcExecContext* ec)
{
	ec->reg[ ec->istream[ ec->reg[pc] ].arg1 ] = ec->istream[ ec->reg[pc] ].arg2;
}

//load local stack element arg1, with value arg2
inline void bc::vm::ocLs(bcExecContext* ec)
{
	ec->stack.at(ec->istream[ec->reg[pc]]) = 
}

inline void bc::vm::ocPush(bcExecContext* ec)
{
	ec->stack.push(ec,ec->istream[ ec->reg[pc] ].arg1);	
}

inline void bc::vm::ocPop(bcExecContext* ec)
{
	ec->stack.pop(ec);
}

inline void bc::vm::ocCmp(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.top();
	ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.top();
	ec->stack.pop(ec);
	ec->reg[cmp1] = ec->reg[t1] - ec->reg[t2];
}
inline void bc::vm::ocJmp(bcExecContext* ec)
{
	ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJe(bcExecContext* ec)
{
	if(!ec->reg[cmp1])
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;	
}

inline void bc::vm::ocJne(bcExecContext* ec)
{
	if(ec->reg[cmp1])
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJg(bcExecContext* ec)
{
	if(ec->reg[cmp1]>0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJl(bcExecContext* ec)
{
	if(ec->reg[cmp1]<0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

