#pragma once
#include "types.h"
#include "error.h"
#include <vector>
#include <map>
#include "tree.hh"

namespace bc
{
	namespace parser
	{		
		class bcParser
		{
		public:
			bcParser();
			bcParser(vector<bcToken>*);	
			bcAST*							Get();
			tree<bcParseNode>*				GetTree();
			std::map<std::string,bcSymbol>* GetSymtab();
			int Setup(vector<bcToken>*);
			int Parse();
			
			//token input
			bcToken* GetToken();
			bcToken* PeekToken();
			bcToken* NextToken();
			bcToken* NextToken(bcTokenType);			

			//Parse index							//pointer to the parse tree
			tree<bcParseNode>::iterator* GetNode();				
			tree<bcParseNode>::iterator* AddNode(bcParseNode);	//add node and point pindex to the new child node
			tree<bcParseNode>::iterator* AddChild(bcParseNode);	//add node, but pindex remains on parent node
			void Parent();
			
			//error
			bool IsError();
			bcErrorCode GetError();
			int GetErrorLine();
			int GetErrorCol();
			void SetError(bcErrorCode,int line,int col);
			void ResetError();

		private:	
			bcAST ast;
			vector<bcToken>* in;
			int tindex;								//Cuurent token we are looking at			
			tree<bcParseNode>::iterator pindex;		//Current parse node we are working on	
			bcErrorCode errorcode;			
		};
		
		//Recursive parsing methods			
		//level 1
		void ParseStatement(bcParser*);
		//level 2
		void ParseBlock(bcParser*);
		void ParseBlockNoDec(bcParser*);
		void ParseWhile(bcParser*);
		void ParseAssignment(bcParser*);
		void ParseDec(bcParser*);
		void ParseFuncCall(bcParser*);
		void ParseFExp(bcParser*);
		void ParseIf(bcParser*);
		//level 3
		void ParseDecFunc(bcParser*);
		void ParseDecVar(bcParser*);
		void ParseBreak(bcParser*);
		void ParseReturn(bcParser*);
		void ParseContinue(bcParser*);			
		bcToken		ParseExp(bcParser*);
		//level 4
		void ParseParamList(bcParser*);	
		void ParseDecParamList(bcParser*);			
		void ParseDecFunc_Type(bcParser*);
		void ParseDecFunc_Ident(bcParser*);		
		void ParseIdent(bcParser*);
		bcToken		ParseSubExp(bcParser*);		
		//level 5		
		void ParseDecInParam(bcParser*);
		bcToken		ParseTerm(bcParser*);
		//level 6
		bcToken		ParseFactor(bcParser*);

		//type compatability
		bool CheckOperandTypes(bcParser*,bcToken,bcToken,bcTokenType);
		bool CheckOperandInt(bcParser*,bcToken,bcTokenType);
		bool CheckOperandFloat(bcParser*,bcToken,bcTokenType);
		bool CheckOperandString(bcParser*,bcToken,bcTokenType);
		bool CheckOperandBool(bcParser*,bcToken,bcTokenType);
		bool CheckOperandObject(bcParser*,bcToken,bcTokenType);
		bool CheckOperandVar(bcParser*,bcToken,bcTokenType);
		bool CheckOperandFunction(bcParser*,bcToken,bcTokenType);
		bool CompareFuncSigs(bcParser*,string, vector<map<string,bcSymbol>::iterator>);
		string GetTag(bcParser*,bcSymbol*);

		bcParseNodeType DeriveType(bcToken);
	}
}