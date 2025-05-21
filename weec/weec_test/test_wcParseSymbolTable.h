#ifndef WC_TEST_WCPARSESYMBOLTABLE_H
#define WC_TEST_WCPARSESYMBOLTABLE_H
#include <string>
#include "parse.h"

namespace weec
{
	namespace test
	{
		int Test_wcParseSymbolTable_default_constructor()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			//default constructor, check built in types
			wcParseSymbolTable SymbolTable;
			if (!SymbolTable.Exists("void"))	return 10;
			if (!SymbolTable.Exists("int"))		return 20;
			if (!SymbolTable.Exists("uint"))	return 30;
			if (!SymbolTable.Exists("double"))	return 40;
			if (!SymbolTable.Exists("float"))	return 50;
			if (!SymbolTable.Exists("bool"))	return 60;
			if (!SymbolTable.Exists("string"))	return 70;

			auto VoidSymbol = SymbolTable.Get("void");
			if (VoidSymbol.Arguments != 0) 					return 11;
			if (VoidSymbol.Const != false) 					return 12;
			if (VoidSymbol.DataType != "") 					return 13;
			if (VoidSymbol.FullIdent != "void") 			return 14;
			if (VoidSymbol.HasOverloads != false) 			return 15;
			if (VoidSymbol.IdentToken != wcToken()) 		return 16;
			if (VoidSymbol.Registered != true) 				return 17;
			if (VoidSymbol.Type != wcParseSymbolType::Type)	return 18;

			auto IntSymbol = SymbolTable.Get("int");
			if (IntSymbol.Arguments != 0) 					return 21;
			if (IntSymbol.Const != false) 					return 22;
			if (IntSymbol.DataType != "") 					return 23;
			if (IntSymbol.FullIdent != "int") 				return 24;
			if (IntSymbol.HasOverloads != false) 			return 25;
			if (IntSymbol.IdentToken != wcToken()) 			return 26;
			if (IntSymbol.Registered != true) 				return 27;
			if (IntSymbol.Type != wcParseSymbolType::Type)	return 28;

			auto UIntSymbol = SymbolTable.Get("uint");
			if (UIntSymbol.Arguments != 0) 					return 31;
			if (UIntSymbol.Const != false) 					return 32;
			if (UIntSymbol.DataType != "") 					return 33;
			if (UIntSymbol.FullIdent != "uint") 			return 34;
			if (UIntSymbol.HasOverloads != false) 			return 35;
			if (UIntSymbol.IdentToken != wcToken()) 		return 36;
			if (UIntSymbol.Registered != true) 				return 37;
			if (UIntSymbol.Type != wcParseSymbolType::Type)	return 38;

			auto DoubleSymbol = SymbolTable.Get("double");
			if (DoubleSymbol.Arguments != 0) 					return 41;
			if (DoubleSymbol.Const != false) 					return 42;
			if (DoubleSymbol.DataType != "") 					return 43;
			if (DoubleSymbol.FullIdent != "double") 			return 44;
			if (DoubleSymbol.HasOverloads != false) 			return 45;
			if (DoubleSymbol.IdentToken != wcToken()) 			return 46;
			if (DoubleSymbol.Registered != true) 				return 47;
			if (DoubleSymbol.Type != wcParseSymbolType::Type)	return 48;

			auto FloatSymbol = SymbolTable.Get("float");
			if (FloatSymbol.Arguments != 0) 					return 51;
			if (FloatSymbol.Const != false) 					return 52;
			if (FloatSymbol.DataType != "") 					return 53;
			if (FloatSymbol.FullIdent != "float") 				return 54;
			if (FloatSymbol.HasOverloads != false) 				return 55;
			if (FloatSymbol.IdentToken != wcToken()) 			return 56;
			if (FloatSymbol.Registered != true) 				return 57;
			if (FloatSymbol.Type != wcParseSymbolType::Type)	return 58;

			auto BoolSymbol = SymbolTable.Get("bool");
			if (BoolSymbol.Arguments != 0) 					return 61;
			if (BoolSymbol.Const != false) 					return 62;
			if (BoolSymbol.DataType != "") 					return 63;
			if (BoolSymbol.FullIdent != "bool") 			return 64;
			if (BoolSymbol.HasOverloads != false) 			return 65;
			if (BoolSymbol.IdentToken != wcToken()) 		return 66;
			if (BoolSymbol.Registered != true) 				return 67;
			if (BoolSymbol.Type != wcParseSymbolType::Type)	return 68;

			auto StringSymbol = SymbolTable.Get("string");
			if (StringSymbol.Arguments != 0) 					return 71;
			if (StringSymbol.Const != false) 					return 72;
			if (StringSymbol.DataType != "") 					return 73;
			if (StringSymbol.FullIdent != "string") 			return 74;
			if (StringSymbol.HasOverloads != false) 			return 75;
			if (StringSymbol.IdentToken != wcToken()) 			return 76;
			if (StringSymbol.Registered != true) 				return 77;
			if (StringSymbol.Type != wcParseSymbolType::Type)	return 78;

			return 0;
		}

		int Test_wcParseSymbolTable_add_symbol()
		{
			using namespace std;
			using namespace weec::lex;
			using namespace weec::parse;

			wcParseSymbolTable SymbolTable;
			wcToken FakeToken(wcStringToken("a", wcStringTokenType::Alpha, 0, 0));

			auto SymbolCountBeforeAdd = SymbolTable.Count();
			SymbolTable.Add(wcParseSymbol(wcParseSymbolType::Variable, wcFullIdentifier("a"), wcFullIdentifier("int"), FakeToken));
			auto OurSymbol = SymbolTable.Get("a");

			if (SymbolTable.Count() != SymbolCountBeforeAdd + 1)	return 1;
			if (OurSymbol.Arguments != 0) 							return 2;
			if (OurSymbol.Const != false) 							return 3;
			if (OurSymbol.DataType != "int") 						return 4;
			if (OurSymbol.FullIdent != "a") 						return 5;
			if (OurSymbol.HasOverloads != false) 					return 6;
			if (OurSymbol.IdentToken != FakeToken) 					return 7;
			if (OurSymbol.Registered != true) 						return 8;
			if (OurSymbol.Type != wcParseSymbolType::Variable)		return 9;

			return 0;
		}

	}
}

#endif