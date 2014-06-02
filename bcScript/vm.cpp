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

//instructions = 0, executes until error/halt signal or end of istream
unsigned int bcVM::exec(unsigned int instructions)
{
	bool loop=(instructions<1);
	int oldpc,*rpc;
	while(con->istream.size() && !con->halt && (instructions>0 || loop))
	{
		oldpc=con->reg[pc];
		rpc = &con->reg[pc];		//shorthand for the program counter
		switch(con->istream[*rpc].op)
		{
		case oc_sf:
			ocSf(con);
			break;
		case oc_rf:
			ocRf(con);
			break;
		case oc_lr:
			ocLr(con);
			break;
		case oc_lrfs:
			ocLrfs(con);
			break;
		case oc_ls:
			ocLs(con);
			break;
		case oc_lsfr:
			ocLsfr(con);
			break;
		case oc_mov:
			ocMov(con);			
			break;
		case oc_push:
			ocPush(con);
			break;
		case oc_pushfs:
			ocPushfs(con);
			break;
		case oc_pop:
			ocPop(con);
			break;
		case oc_cmp: 
			ocCmp(con);
			break;
		case oc_jmp:
			ocJmp(con);
			break;
		case oc_je: 
			ocJe(con);
			break;
		case oc_jne:
			ocJne(con);		
			break; 
		case oc_jg: 
			ocJg(con);
			break; 
		case oc_jl: 
			ocJl(con);
			break; 
		case oc_jge:  
			ocJge(con);
			break;
		case oc_jle:  
			ocJle(con);
			break;
		case oc_assign: 
			ocAssign(con);
			break;
		case oc_plus: 
			ocPlus(con);
			break;
		case oc_minus: 
			ocMinus(con);
			break; 
		case oc_mult: 
			ocMult(con);
			break; 
		case oc_div: 
			ocDiv(con);
			break; 
		case oc_expo: 
			ocExpo(con);
			break;
		case oc_mod: 
			ocMod(con);
			break;
		case oc_inc:
			ocInc(con);
			break;
		case oc_dec: 
			ocDec(con);
			break;
		case oc_and: 
			ocAnd(con);
			break;
		case oc_or: 
			ocOr(con);
			break;
		case oc_xor:
			ocXor(con);
			break; 
		case oc_not:
			ocNot(con);
			break; 
		case oc_shfl:
			ocShfl(con);
			break;
		case oc_shfr:
			ocShfr(con);
			break; 
		case oc_call:
			ocCall(con);
			break;
		case oc_ret:
			ocRet(con);
			break; 
		case oc_callvm: 
			ocCallvm(con);
			break;
		case oc_pause:
			ocPause(con);
			break;
		case oc_halt:
			ocHalt(con);
			break;

		case oc_nop:
		default:
				break;
		}
		
		//inc program counter if it hasnt been altered by an instruction
		if(con->reg[pc]==oldpc)
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
	//*ec->stack.at(ec->istream[ ec->reg[pc] ].arg1) = *ec->stack.at(ec->istream[ ec->reg[pc] ].arg2);
}

//set flag arg1 to literal value arg2
inline void bc::vm::ocSf(bcExecContext* ec)
{
	ec->regFlags[ec->istream[ ec->reg[pc] ].arg1] = ec->istream[ ec->reg[pc] ].arg2;
}

//push value from flag arg1 to stack
inline void bc::vm::ocRf(bcExecContext* ec)
{
	ec->stack.push(ec->istream[ ec->reg[pc] ].arg1);
}

//load register arg1, with value arg2. if register sti=1, arg2 is treated as a stackindex
inline void bc::vm::ocLr(bcExecContext* ec)
{
	if(ec->reg[sti])
	{
		ec->reg[ ec->istream[ ec->reg[pc] ].arg1 ] = ec->istream[ ec->reg[pc] ].arg2;
		ec->reg[sti]=0;
	}
	else
		ec->reg[ ec->istream[ ec->reg[pc] ].arg1 ] = *ec->stack.at(ec->istream[ ec->reg[pc] ].arg2);
}

//load local stack element arg1, with value arg2. if register sti=1, arg2 is treated as a stackindex
inline void bc::vm::ocLs(bcExecContext* ec)
{
	if(ec->reg[sti])
	{
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
		ec->reg[sti]=0;
	}
	else
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = ec->istream[ec->reg[pc]].arg2;
}

//load register arg1, with value from stack index arg2. if register sti=1, the value at arg2 is treated as a stackindex 
inline void bc::vm::ocLrfs(bcExecContext* ec)
{
	if(ec->reg[sti])
	{
		ec->reg[ ec->istream[ ec->reg[pc] ].arg1 ] = *ec->stack.at(ec->istream[ ec->reg[pc] ].arg2);
		ec->reg[sti]=0;
	}
	else
		ec->reg[ ec->istream[ ec->reg[pc] ].arg1 ] = *ec->stack.at(ec->stack.size()-1);
}

//load local stack element arg1, with value from register index arg2. if register sti=1, the value at arg2 is treated as a stackindex
inline void bc::vm::ocLsfr(bcExecContext* ec)
{
	if(ec->reg[sti])
	{
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
		ec->reg[sti]=0;
	}
	else
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = ec->reg[ec->istream[ec->reg[pc]].arg2];
}

//push literal to stack
inline void bc::vm::ocPush(bcExecContext* ec)
{
	ec->stack.push(ec,ec->istream[ ec->reg[pc] ].arg1);	
}

//push literal to stack, from a value already in the stack
inline void bc::vm::ocPushfs(bcExecContext* ec)
{
	ec->stack.push(ec,*ec->stack.at(ec->istream[ec->reg[pc]].arg1));	
}

//pop off the top element
inline void bc::vm::ocPop(bcExecContext* ec)
{
	ec->stack.pop(ec);
}

//compare top 2 stack items
inline void bc::vm::ocCmp(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->reg[cmp] = ec->reg[t1] - ec->reg[t2];
}

inline void bc::vm::ocJmp(bcExecContext* ec)
{
	ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJe(bcExecContext* ec)
{
	if(!ec->reg[cmp])
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;	
}

inline void bc::vm::ocJne(bcExecContext* ec)
{
	if(ec->reg[cmp])
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJg(bcExecContext* ec)
{
	if(ec->reg[cmp]>0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJl(bcExecContext* ec)
{
	if(ec->reg[cmp]<0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJge(bcExecContext* ec)
{
	if(ec->reg[cmp]>=0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocJle(bcExecContext* ec)
{
	if(ec->reg[cmp]<=0)
		ec->reg[pc]=ec->istream[ec->reg[pc]].arg1;
}

//pop top 2 stack items, assign value at t1 to stack index at arg1
inline void bc::vm::ocAssign(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec); ec->stack.pop(ec);	
	ec->reg[t2] = ec->istream[ec->reg[pc]].arg1;		//always treat as stackindex
	*ec->stack.at(ec->reg[t2])=ec->reg[t1];
	ec->stack.push(ec,ec->reg[t1]);
}

//pop top 2 stack items, add together, push back result to stack
inline void bc::vm::ocPlus(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]+ec->reg[t2]);
}

//pop top 2 stack items, do t1-t2, push back result to stack
inline void bc::vm::ocMinus(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]-ec->reg[t2]);
}

//pop top 2 stack items, do t1/t2, push back result to stack
inline void bc::vm::ocDiv(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]/ec->reg[t2]);
}

//pop top 2 stack items, do t1*t2, push back result to stack
inline void bc::vm::ocMult(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]*ec->reg[t2]);
}

//pop top 2 stack items, do t1%t2, push back result to stack
inline void bc::vm::ocMod(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]%ec->reg[t2]);
}

//pop top 2 stack items, do t1^t2, push back result to stack
inline void bc::vm::ocExpo(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]^ec->reg[t2]);
}

//pop stack, decrement, push back onto stack
inline void bc::vm::ocDec(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]--);
}

//pop stack, increment, push back onto stack
inline void bc::vm::ocInc(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1]++);
}

//pop top 2 stack items, do t1&&t2, push back result to stack
inline void bc::vm::ocAnd(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1] && ec->reg[t2]);
}

//pop top 2 stack items, do t1||t2, push back result to stack
inline void bc::vm::ocOr(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1] || ec->reg[t2]);
}

//todo fix
inline void bc::vm::ocXor(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1] && ec->reg[t2]);
}

inline void bc::vm::ocNot(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,!ec->reg[t1]);
}

inline void bc::vm::ocShfl(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1] << ec->reg[t2]);
}

inline void bc::vm::ocShfr(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec,ec->reg[t1] >> ec->reg[t2]);
}

inline void bc::vm::ocCall(bcExecContext* ec)
{

}

inline void bc::vm::ocCallvm(bcExecContext* ec)
{

}

inline void bc::vm::ocRet(bcExecContext* ec)
{

}

inline void bc::vm::ocHalt(bcExecContext* ec)
{
	ec->reg[hlt] = ec->istream[ec->reg[pc]].arg1;
}

inline void bc::vm::ocPause(bcExecContext* ec)
{

}