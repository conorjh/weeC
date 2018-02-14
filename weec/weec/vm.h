#ifndef WC_VM_H
#define WC_VM_H
#include <unordered_map>
#include "bytecode.h"

namespace wc
{
	namespace vm
	{
		class wcBaseVM
		{
		public:
			wcBaseVM();
			int exec(int handle);
			int load(bytecode::wcExecContext&);
			bool remove(int handle);
			bytecode::wcExecContext& getContext(int);

			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);
		protected:
			int newHandle();
			bool handleExists(int);
			void removeHandle(int);

			std::unordered_map<int, bytecode::wcExecContext> conPool;
		};

	}
}
#endif