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
	bool loop = (instructions < 1);
	int oldpc, *rpc;
	while (con->istream.size() && !con->halt && (instructions>0 || loop))
	{
		oldpc = con->reg[pc];
		rpc = &con->reg[pc];		//shorthand for the program counter
		switch (con->istream[*rpc].op)
		{
		case oc_setflag:
			ocSf(con);
			break;
		case oc_readflag:
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
		case oc_pushfr:
			ocPushfr(con);
			break;
		case oc_pop:
			ocPop(con);
			break;
		case oc_popr:
			ocPopr(con);
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
		case oc_pushsf:
			ocPushsf(con);
			break;
		case oc_popsf:
			ocPopsf(con);
			break;
		case oc_nop:
		default:
			ocNop(con);
			break;
		}

		//inc program counter if it hasnt been altered by an instruction
		if (con->reg[pc] == oldpc)
			(*rpc)++;
		if (*rpc >= con->istream.size())
		{
			con->halt = true;
			break;
		}
		--instructions;
	}

	return true;
}

void bc::vm::ocNop(bcExecContext *)
{

}

//copy value of register arg1, to register arg2. if sti flg is set, arg1 and arg2 become stack indexes
void bc::vm::ocMov(bcExecContext* ec)
{
	if (ec->regFlags[sti] == 1)
	{
		ec->regFlags[sti] = 0;
		*ec->stack.at(ec->istream[ec->reg[pc]].arg2) = *ec->stack.at(ec->istream[ec->reg[pc]].arg1);
	}
	else
	{
		ec->reg[ec->istream[ec->reg[pc]].arg2] = ec->reg[ec->istream[ec->reg[pc]].arg1];
	}
}

//set flag arg1 to literal value arg2
void bc::vm::ocSf(bcExecContext* ec)
{
	ec->regFlags[ec->istream[ec->reg[pc]].arg1] = ec->istream[ec->reg[pc]].arg2;
}

//push value from flag arg1 to stack
void bc::vm::ocRf(bcExecContext* ec)
{
	ec->stack.push(ec->regFlags[ec->istream[ec->reg[pc]].arg1]);
}

//load register arg1, with value arg2. if register sti=1, arg2 is treated as a stackindex
void bc::vm::ocLr(bcExecContext* ec)
{
	if (ec->reg[sti])
	{
		ec->reg[ec->istream[ec->reg[pc]].arg1] = ec->istream[ec->reg[pc]].arg2;
		ec->reg[sti] = 0;
	}
	else
		ec->reg[ec->istream[ec->reg[pc]].arg1] = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
}

//load local stack element arg1, with value arg2. if register sti=1, arg2 is treated as a stackindex
void bc::vm::ocLs(bcExecContext* ec)
{
	if (ec->reg[sti])
	{
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
		ec->reg[sti] = 0;
	}
	else
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = ec->istream[ec->reg[pc]].arg2;
}

//load register arg1, with value from stack index arg2. if register sti=1, the value at arg2 is treated as a stackindex 
void bc::vm::ocLrfs(bcExecContext* ec)
{
	if (ec->reg[sti])
	{
		ec->reg[ec->istream[ec->reg[pc]].arg1] = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
		ec->reg[sti] = 0;
	}
	else
		ec->reg[ec->istream[ec->reg[pc]].arg1] = *ec->stack.at(ec->stack.size() - 1);
}

//load local stack element arg1, with value from register index arg2. if register sti=1, the value at arg2 is treated as a stackindex
void bc::vm::ocLsfr(bcExecContext* ec)
{
	if (ec->reg[sti])
	{
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = *ec->stack.at(ec->istream[ec->reg[pc]].arg2);
		ec->reg[sti] = 0;
	}
	else
		*ec->stack.at(ec->istream[ec->reg[pc]].arg1) = ec->reg[ec->istream[ec->reg[pc]].arg2];
}

//push literal to stack
void bc::vm::ocPush(bcExecContext* ec)
{
	ec->stack.push(ec, ec->istream[ec->reg[pc]].arg1);
}

//push stackindex arg1, to top of stack
void bc::vm::ocPushfs(bcExecContext* ec)
{
	ec->stack.push(ec, *ec->stack.at(ec->istream[ec->reg[pc]].arg1));
}

//push register arg1 to top of stack
void bc::vm::ocPushfr(bcExecContext* ec)
{
	ec->stack.push(ec, ec->reg[ec->istream[ec->reg[pc]].arg1]);
}

//pop off the top element
void bc::vm::ocPop(bcExecContext* ec)
{
	ec->stack.pop(ec);
}

//pop off the top element to register arg1
void bc::vm::ocPopr(bcExecContext* ec)
{
	ec->reg[ec->istream[ec->reg[pc]].arg1] = ec->stack.pop((ec));
}

//compare top 2 stack items
void bc::vm::ocCmp(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->reg[cmp] = ec->reg[t1] - ec->reg[t2];
}

void bc::vm::ocJmp(bcExecContext* ec)
{
	ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJe(bcExecContext* ec)
{
	if (!ec->reg[cmp])
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJne(bcExecContext* ec)
{
	if (ec->reg[cmp])
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJg(bcExecContext* ec)
{
	if (ec->reg[cmp] > 0)
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJl(bcExecContext* ec)
{
	if (ec->reg[cmp] < 0)
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJge(bcExecContext* ec)
{
	if (ec->reg[cmp] >= 0)
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocJle(bcExecContext* ec)
{
	if (ec->reg[cmp] <= 0)
		ec->reg[pc] = ec->istream[ec->reg[pc]].arg1;
}

//pop top 2 stack items, assign stackindex at t1 to value at t2, if sti is set, t2 is also a stackindex
void bc::vm::ocAssign(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	if (ec->regFlags[sti])
	{
		ec->regFlags[sti] = 0;
		*ec->stack.at(ec->reg[t1]) = *ec->stack.at(ec->reg[t2]);
		ec->stack.push(*ec->stack.at(ec->reg[t2]));

	}
	else
	{
		*ec->stack.at(ec->reg[t1]) = ec->reg[t2];
		ec->stack.push(ec->reg[t2]);
	}
}

//pop top 2 stack items, add together, push back result to stack
void bc::vm::ocPlus(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] + ec->reg[t2]);
}

//pop top 2 stack items, do t1-t2, push back result to stack
void bc::vm::ocMinus(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] - ec->reg[t2]);
}

//pop top 2 stack items, do t1/t2, push back result to stack
void bc::vm::ocDiv(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] / ec->reg[t2]);
}

//pop top 2 stack items, do t1*t2, push back result to stack
void bc::vm::ocMult(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] * ec->reg[t2]);
}

//pop top 2 stack items, do t1%t2, push back result to stack
void bc::vm::ocMod(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] % ec->reg[t2]);
}

//pop top 2 stack items, do t1^t2, push back result to stack
void bc::vm::ocExpo(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] ^ ec->reg[t2]);
}

//pop stack, decrement, push back onto stack
void bc::vm::ocDec(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1]--);
}

//pop stack, increment, push back onto stack
void bc::vm::ocInc(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1]++);
}

//pop top 2 stack items, do t1&&t2, push back result to stack
void bc::vm::ocAnd(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] && ec->reg[t2]);
}

//pop top 2 stack items, do t1||t2, push back result to stack
void bc::vm::ocOr(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] || ec->reg[t2]);
}

//todo fix
void bc::vm::ocXor(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] && ec->reg[t2]);
}

void bc::vm::ocNot(bcExecContext* ec)
{
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, !ec->reg[t1]);
}

void bc::vm::ocShfl(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] << ec->reg[t2]);
}

void bc::vm::ocShfr(bcExecContext* ec)
{
	ec->reg[t2] = ec->stack.pop(ec);
	ec->reg[t1] = ec->stack.pop(ec);
	ec->stack.push(ec, ec->reg[t1] >> ec->reg[t2]);
}

void bc::vm::ocCall(bcExecContext* ec)
{

}

void bc::vm::ocCallvm(bcExecContext* ec)
{

}

void bc::vm::ocRet(bcExecContext* ec)
{

}

//reserve space on the stack for a given stackframe
void bc::vm::ocPushsf(bcExecContext *ec)
{
	for (int t = 0; t < ec->stackFrames[ec->istream[ec->reg[pc]].arg1].size(); ++t)
		ec->stack.push(0);
}

//remove a given stackframe from the stack
void bc::vm::ocPopsf(bcExecContext *ec)
{
	//stackframe index
	int arg = ec->istream[ec->reg[pc]].arg1;

	//create storage frame if there isnt one
	if (ec->storedFrames.find(arg) == ec->storedFrames.end())
	{
		ec->storedFrames.insert(std::make_pair(arg, bcStoredStackFrame()));
		for (int n = 0; n < ec->stackFrames[arg].size(); ++n)
			ec->storedFrames[arg].data.push(0);
	}

	for (int t = ec->stackFrames[arg].size() - 1; t > -1; --t)
		*(ec->storedFrames[arg].data.at(t)) = ec->stack.pop(0);
}

void bc::vm::ocHalt(bcExecContext* ec)
{
	ec->reg[hlt] = ec->istream[ec->reg[pc]].arg1;
}

void bc::vm::ocPause(bcExecContext* ec)
{

}