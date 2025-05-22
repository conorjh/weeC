#ifndef WC_TEST_UTILS_H
#define WC_TEST_UTILS_H
#include "interpreter.h"
#include "lex.h"
#include "parse.h"
#include <string>
#include <any>
#include <iostream>
#include <chrono>

namespace weec
{
	namespace test
	{
		namespace utils
		{
			inline void printTree(tree<weec::parse::wcParseNode> pTree)
			{
				using namespace std;

				auto it = pTree.begin();

				while (it != pTree.end())
				{
					for (int t = 0; t < pTree.depth(it); t++)
						cout << " - ";
					cout << it->Token.StringToken.Data << " (" << to_string(it->Type) << ")" << endl;
					it++;
				}
			}

			inline std::string AnyToString(std::any In)
			{
				using namespace std;

				if (strcmp(In.type().name(), "int") == 0)
					return to_string(any_cast<int>(In));
				else if (strcmp(In.type().name(), "unsigned int") == 0)
					return to_string(any_cast<unsigned int>(In));
				else if (strcmp(In.type().name(), "float") == 0)
					return to_string(any_cast<float>(In));
				else if (strcmp(In.type().name(), "double") == 0)
					return to_string(any_cast<double>(In));
				else if (strcmp(In.type().name(), "bool") == 0)
					return to_string(any_cast<bool>(In));
				return "";
			}

			inline bool printIsExpected(weec::interpreter::wcInterpreter Interp, std::any ExpectedResult)
			{
				using namespace weec::interpreter;
				using namespace std;

				if (Interp.Error.Code != wcInterpreterErrorCode::None)
					cout << "Interp error: " << Interp.to_string((int)Interp.Error.Code) << " " << Interp.to_string(Interp.Error.Code) << endl;

				bool IsExpected = false;
				if (strcmp(Interp.Return.type().name(), "int") == 0)
				{
					if (ExpectedResult.has_value())
					{
						IsExpected = any_cast<int>(Interp.Return) == any_cast<int>(ExpectedResult);
						cout << "Expected: " << any_cast<int>(ExpectedResult) << "   ";
					}
					cout << "Out: " << any_cast<int>(Interp.Return) << endl;
				}
				else if (strcmp(Interp.Return.type().name(), "unsigned int") == 0)
				{
					if (ExpectedResult.has_value())
					{
						IsExpected = any_cast<unsigned int>(Interp.Return) == any_cast<int>(ExpectedResult);
						cout << "Expected: " << any_cast<unsigned int>(ExpectedResult) << "   ";
					}
					cout << "Out: " << any_cast<unsigned int>(Interp.Return) << endl;
				}
				else if (strcmp(Interp.Return.type().name(), "float") == 0)
				{
					if (ExpectedResult.has_value())
					{
						IsExpected = any_cast<float>(Interp.Return) == any_cast<float>(ExpectedResult);
						cout << "Expected: " << any_cast<float>(ExpectedResult) << "   ";
					}
					cout << "Out: " << any_cast<float>(Interp.Return) << endl;
				}
				else if (strcmp(Interp.Return.type().name(), "double") == 0)
				{
					if (ExpectedResult.has_value())
					{
						IsExpected = any_cast<double>(Interp.Return) == any_cast<double>(ExpectedResult);
						cout << "Expected: " << any_cast<double>(ExpectedResult) << "   ";
					}
					cout << "Out: " << any_cast<double>(Interp.Return) << endl;
				}
				else if (strcmp(Interp.Return.type().name(), "bool") == 0)
				{
					if (ExpectedResult.has_value())
					{
						//IsExpected = any_cast<bool>(Interp.Return) == any_cast<bool>(ExpectedResult);
			//			cout << "Expected: " << any_cast<bool>(ExpectedResult) << "   ";
					}
					//cout << "Out: " << any_cast<bool>(Interp.Return) << endl;
				}
				cout << "EAX: " << AnyToString(Interp.EAX) << endl;
				auto t = Interp.Return.type().name();
				cout << "Type: " << Interp.Return.type().name() << endl;
				return IsExpected;
			}

			inline int Test_ParserTemplate(std::string Listing)
			{
				using namespace weec::lex;
				using namespace weec::parse;
				using namespace weec::interpreter;
				using namespace std;

				wcTokenizer tokenizer(Listing); 

				auto parseStart = chrono::system_clock::now();
				auto Parsed = wcParser(tokenizer).Parse();
				auto parseEnd = chrono::system_clock::now();

				cout << endl << endl << Listing << endl;

				printTree(Parsed.AST);

				cout << "Parse time: " << chrono::duration_cast<chrono::milliseconds>(parseEnd - parseStart).count() << "ms" << endl;
				cout << endl << "Error code: " << (int)Parsed.Error.Code << "  " << to_string(Parsed.Error.Code) << endl;
				if (Parsed.Error.Code != wcParserErrorCode::None)
				{
					return (int)Parsed.Error.Code;
				}

				auto execStart = chrono::system_clock::now();
				auto Interp = wcInterpreter(Parsed);
				auto Result = Interp.Exec();
				auto execEnd = chrono::system_clock::now();

				cout << "Exec time: " << chrono::duration_cast<chrono::milliseconds>(execEnd - execStart).count() << "ms" << endl;
				return (Parsed.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, any())) ? 0 : 1;
			}


			inline int Test_ExpressionParserTemplate(std::string Listing, std::any ExpectedResult)
			{
				using namespace weec::lex;
				using namespace weec::parse;
				using namespace weec::interpreter;
				using namespace std;

				wcTokenizer tokenizer(Listing);

				auto Expr = wcParser(tokenizer).Parse();

				cout << endl << endl << Listing << endl;

				printTree(Expr.AST);

				if (Expr.Error.Code != wcParserErrorCode::None)
				{
					cout << endl << "Error code: " << (int)Expr.Error.Code << "  " << to_string(Expr.Error.Code) << endl;
					return (int)Expr.Error.Code;
				}
				auto Interp = wcInterpreter(Expr);
				auto Result = Interp.Exec();

				return (Expr.Error.Code == wcParserErrorCode::None && printIsExpected(Interp, ExpectedResult)) ? 0 : 1;
			}
		}
	}
}

#endif