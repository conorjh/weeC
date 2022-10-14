#ifndef WC_INTERPRETER
#define WC_INTERPRETER
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include "tree.hh"
#include "lex.h"
#include "parse.h"

namespace weec
{
	namespace interpreter
	{
		class wcExpressionInterpeter
		{
			int Value;

			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator PC;
			std::any ChoosePath(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom);
			parse::wcParseSymbolTable& SymTab;

		public:
			wcExpressionInterpeter(parse::wcParseSymbolTable& SymTab, parse::wcParseOutput Input, tree<parse::wcParseNode>::iterator PC);

			std::any ExecSubExpression(),
				ExecEquality(), ExecAssignment(), ExecLogicOr(),
				ExecLogicAnd(), ExecComparison(),
				ExecTerm(), ExecFactor(),
				ExecUnary(), ExecPrimary(),
				ExecOperator();

			bool Exec();
		};

		class wcInterpreter
		{
			parse::wcParseOutput Input;
			tree<parse::wcParseNode>::iterator PC;
			parse::wcParseSymbolTable SymbolTable;

			bool ExecExpression();

		public:
			wcInterpreter(parse::wcParseOutput Input);

			void Reset();
			bool Exec(), ExecStatement();

			bool Halt;
		};

	}

}


#endif
