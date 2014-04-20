#include "vm.h"

using namespace bc::vm;

unsigned int bcVM::exec(unsigned int instructions)
{
	while(instructions>0)
	{
		switch(istream[pc].op)
		{
		case oc_mov:
			break;
		case oc_push:
		case oc_pop: 
		case oc_je: 
		case oc_jne: 
		case oc_jg: 
		case oc_jl: 
		case oc_jge: 
		case oc_jle: 
		case oc_plus: 
		case oc_minus: 
		case oc_mult: 
		case oc_div: 
		case oc_exp: 
		case oc_mod: 
		case oc_inc:
		case oc_dec: 
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
		pc++;
		--instructions;
	}
	return true;
}