#include "types.h"
#include "error.h"
#include "tree.hh"

namespace bc
{
	namespace gen
	{
		const bool bcCodeGen_CheckEmptyInput = false;

		class bcByteCodeGen
		{
		public:
			bcByteCodeGen();
			bcByteCodeGen(tree<parser::bcParseNode>*, map<string,bcSymbol>* _symtab,map<string,vector<map<string,bcSymbol>::iterator>>*,
							vector<bcToken>*,map<string,vector<map<string,bcSymbol>::iterator>>* );
	
			int Init(tree<parser::bcParseNode>*,  map<string,bcSymbol>* _symtab,map<string,vector<map<string,bcSymbol>::iterator>>*,
						vector<bcToken>*, map<string,vector<map<string,bcSymbol>::iterator>>*);

			int Gen();
			int GenDec();
			int GenDecFunc();
			int GenStatement();
			int GenAssignment();
			int GenBlock();
			int GenFuncCall();
			int GenParamList();
			int GenIf();
			int GenWhile();
			int GenBreak();
			int GenReturn();
			int GenContinue();
			int GenDecParamList();
			int GenDecInParam();
	
			int GenExp();
			bcToken GenSubExp();
			bcToken GenTerm();
			bcToken GenFactor();

			int GenStackFrame(string);
			int GenStackFramePops(string);

			int AddInstr(bcOpcode,bcOperandType,string);	//for instructions with one operand
			int AddInstr(bcOpcode,vector<bcOperand> ops);

			tree<parser::bcParseNode>::iterator CurrentNode();
			tree<parser::bcParseNode>::iterator NextNode();
			tree<parser::bcParseNode>::iterator PreviousNode();
			int IncNode();
			int IncNode(parser::bcParseNodeType);	

			void SetError(bcErrorCode);
			bcErrorCode GetError();
			bool IsError();
	
			vector<string> strlittab;		//string literals
			map<string,int> functab;		//internal ident to line number
			vector<bcInstr> istream;

			tree<parser::bcParseNode>::iterator index;
			tree<parser::bcParseNode>* in;
			map<string,bcSymbol>* symtab;
			vector<bcToken>* tokens;
			map<string,vector<map<string,bcSymbol>::iterator>>* funcsigs;
			map<string,vector<map<string,bcSymbol>::iterator>>* stackframes;
	

			bcErrorCode errorcode;
	
			bool error;
			bool safe;


		};
	}
}