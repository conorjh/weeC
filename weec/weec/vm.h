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
			int exec(int handle);
			int load(bytecode::wcExecContext&);
			bool remove(int handle);

			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);
		private:
			int newHandle();
			bool handleExists(int);
			void removeHandle(int);

			std::unordered_map<int, bytecode::wcExecContext> conPool;
		};

		class wcClassicVM : wcBaseVM
		{
			wcClassicVM();
			virtual int execInstruction(bytecode::wcExecContext& context, bytecode::wcInstruction instr);
		};
	}
}
#endif