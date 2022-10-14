#include "test.h"
#include "lex.h"
#include "parse.h"
#include "interpreter.h"
#include "tree_util.hh"
#include <string>
#include <vector>
#include <iostream>

using std::string;

using namespace std;
using namespace weec;
using namespace weec::lex;
using namespace weec::parse;
using namespace weec::interpreter;
using weec::lex::wcToken;

namespace weec
{
	namespace lex
	{

	}
}



void printTree(tree<wcParseNode> pTree)
{
	auto it = pTree.begin();

	while (it != pTree.end())
	{
		for (int t = 0; t < pTree.depth(it); t++)
			cout << " - ";
		cout << it->Token.StringToken.Data << " (" << wcParseNodeTypeToString(it->Type) << ")" << endl;
		it++;
	}
}

int main(int argc, char* argv[])
{
	weec::test::lex::Test_AllLex();
	weec::test::lex::Test_AllParse();
}

int weec::test::lex::Test_AllLex()
{
	std::cout << "Running lex tests..." << std::endl;
	for (int t = 0; t < allLexTests.size(); ++t)
	{
		int testResult = allLexTests[t]();
		if (testResult)
			std::cout << "\tTest #" << t + 1 << " failed: " << testResult << std::endl;
		else
			std::cout << "\tTest #" << t + 1 << " passed" << std::endl;
	}

	return 0;
}

int weec::test::lex::Test_AllParse()
{
	std::cout << "Running parse tests..." << std::endl;
	for (int t = 0; t < allParseTests.size(); ++t)
	{
		int testResult = allParseTests[t]();
		if (testResult)
			std::cout << "\tTest #" << t + 1 << " failed: " << testResult << std::endl;
		else
			std::cout << "\tTest #" << t + 1 << " passed" << std::endl;
	}

	return 0;
}

int weec::test::lex::Test_StringTokenizer1()
{
	string input = listing::list_stringtokenizer4;
	wcStringTokenizer stringTokenizer(input);

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Hello")
		return 1;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "World")
		return 2;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Longer")
		return 3;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "sentence")
		return 5;
	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "Finishing")
		return 6;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "with")
		return 7;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::Whitespace)
		return 4;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Data != "newline" || stringTokenizer.GetStringToken().Line != 4)
		return 8;

	if (!stringTokenizer.NextStringToken() || stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		return 4;

	if (stringTokenizer.NextStringToken())	//end of stream
		return 9;

	return 0;
}

int weec::test::lex::Test_Tokenizer1()
{
	string input = listing::list_tokenizer1;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::FloatLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer2()
{
	string input = listing::list_tokenizer2;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer3()
{
	string input = listing::list_tokenizer3;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharLiteral)
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer4()
{
	string input = listing::list_tokenizer4;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ConstKeyword  && tokenizer.GetToken().StringToken.Data == "const")
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "TestIdent")
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 7;

	return 0;
}

int weec::test::lex::Test_Tokenizer5()
{
	string input = listing::list_tokenizer5;
	wcTokenizer tokenizer(input);

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "std::string")
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "TestIdent")
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::StringLiteral)
		return 4;

	return 0;
}

int weec::test::lex::Test_Tokenizer6()
{
	string input = listing::list_tokenizer6;
	wcTokenizer tokenizer(input);

	//- + += * / + ( ) { } [ ] # \\ / % & ^ ? |

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MinusOperator)
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusAssignOperator)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 7;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 8;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
		return 9;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
		return 10;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
		return 11;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
		return 12;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Hash)
		return 13;

	return 0;
}

int weec::test::lex::Test_Tokenizer7()
{
	string input = listing::list_tokenizer7;
	wcTokenizer tokenizer(input);

	//int main(int argc, char* argv[])
	//{
	//	return 0;
	//}

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 1;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 3;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 4;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Comma)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
		return 7;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::MultiplyOperator)
		return 8;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 9;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBracket)
		return 10;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBracket)
		return 11;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 12;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenBrace)
		return 13;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::ReturnKeyword)
		return 14;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 15;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
		return 16;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseBrace)
		return 17;

	return 0;
}

int weec::test::lex::Test_Tokenizer8()
{
	string input = listing::list_tokenizer8;
	wcTokenizer tokenizer(input);

	//  // comment  
	//  /* multi 
	//  line 
	//  comment */


	if (tokenizer.NextToken())
		return 1;

	return 0;
}

int weec::test::lex::Test_Tokenizer9()
{
	string input = listing::list_tokenizer9;
	wcTokenizer tokenizer(input);

	//  // comment  
	//  /* multi 
	//  line 
	//  comment */
	//	with extra line //then comment at the end


	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 3;

	return 0;
}

int weec::test::lex::Test_Tokenizer10()
{
	string input = listing::list_tokenizer10;
	wcTokenizer tokenizer(input);

	//  int IdentTest; char IdentTest::WithNamespace

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntKeyword)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::SemiColon)
		return 3;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CharKeyword)
		return 4;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier || tokenizer.GetToken().StringToken.Data != "IdentTest::WithNamespace")
		return 5;

	return 0;
}

int weec::test::lex::Test_Tokenizer12()
{
	string input = listing::list_tokenizer12;
	wcTokenizer tokenizer(input);

	// ident.withMember ident::withNamespace::withObject.withMember

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident.withMember")
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier && tokenizer.GetToken().StringToken.Data == "ident::withNamespace::withObject.withMember")
		return 2;

	return 0;
}

int weec::test::lex::Test_Tokenizer11()
{
	string input = listing::list_tokenizer11;
	wcTokenizer tokenizer(input);

	//x = 88 + (42 / 99)

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::Identifier)
		return 1;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::AssignOperator)
		return 2;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 3;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::PlusOperator)
		return 4;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::OpenParenthesis)
		return 5;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 6;

	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::DivideOperator)
		return 7;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::IntLiteral)
		return 8;
	if (!tokenizer.NextToken() || tokenizer.GetToken().Type != wcTokenType::CloseParenthesis)
		return 9;

	return 0;
}


int weec::test::lex::Test_wcParseNode_1()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer11;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer11 << endl;
	printTree(Output.AST);

	wcInterpreter Interp(Output);
	Interp.Exec();

	return 0;
}

int weec::test::lex::Test_wcParseNode_2()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer12;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer12 << endl;
	printTree(Output.AST);

	return 0;
}

int weec::test::lex::Test_wcParseNode_3()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer13;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer13 << endl;
	printTree(Output.AST);

	return 0;
}

int weec::test::lex::Test_wcParseNode_4()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer14;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer14 << endl;
	printTree(Output.AST);

	wcInterpreter Interp(Output);
	Interp.Exec();

	return 0;
}

int weec::test::lex::Test_wcParseNode_5()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer15;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer15 << endl;
	printTree(Output.AST);

	return 0;
}

int Test_ExpressionParserTemplate(string Listing)
{
	wcTokenizer tokenizer(Listing); tokenizer.NextToken();

	auto Expr = wcExpressionParser(tokenizer, *new wcParseSymbolTable()).ParseExpression();

	cout << Listing << endl;

	printTree(Expr.AST);

	return 0;
}

int weec::test::lex::Test_wcExpressionParser_1()
{
	return Test_ExpressionParserTemplate(listing::list_expression1);
}

int weec::test::lex::Test_wcExpressionParser_2()
{
	return Test_ExpressionParserTemplate(listing::list_expression2);
}

int weec::test::lex::Test_wcExpressionParser_3()
{
	return Test_ExpressionParserTemplate(listing::list_expression3);
}

int weec::test::lex::Test_wcExpressionParser_4()
{
	return Test_ExpressionParserTemplate(listing::list_expression4);
}

int weec::test::lex::Test_wcExpressionParser_5()
{
	return Test_ExpressionParserTemplate(listing::list_expression5);
}

int weec::test::lex::Test_wcExpressionParser_6()
{
	return Test_ExpressionParserTemplate(listing::list_expression6);
}

int weec::test::lex::Test_wcParseNode_6()
{
	//x = 88 + (42 / 99)
	string input = listing::list_tokenizer16;

	wcTokenizer tokenizer(input);
	wcParser Parser(tokenizer);

	auto Output = Parser.Parse();
	cout << listing::list_tokenizer16 << endl;
	printTree(Output.AST);

	return 0;
}


/*
describe Parser do
  before(:each) do
	@parser = Parser.new
  end

  it 'should compute 5 when given 2 + 3' do
	@parser.parse('2 + 3').should == 5
  end

  it 'should compute 6 when given 2 * 3' do
	@parser.parse('2 * 3').should == 6
  end

  it 'should compute 89 when given 89' do
	@parser.parse('89').should == 89
  end

  it 'should raise an error when input is empty' do
	lambda {@parser.parse('')}.should raise_error()
  end

  it 'should omit white spaces' do
	@parser.parse('   12        -  8   ').should == 4
	@parser.parse('142        -9   ').should == 133
	@parser.parse('72+  15').should == 87
	@parser.parse(' 12*  4').should == 48
	@parser.parse(' 50/10').should == 5
  end

  it 'should treat dot separated floating point numbers as a valid input' do
	@parser.parse('2.5').should == 2.5
	@parser.parse('4*2.5 + 8.5+1.5 / 3.0').should == 19
	@parser.parse('5.0005 + 0.0095').should be_close(5.01, 0.01)
  end

  it 'should handle tight expressions' do
	@parser.parse('67+2').should == 69
	@parser.parse(' 2-7').should == -5
	@parser.parse('5*7 ').should == 35
	@parser.parse('8/4').should == 2
  end

  it 'should calculate long additive expressions from left to right' do
	@parser.parse('2 -4 +6 -1 -1- 0 +8').should == 10
	@parser.parse('1 -1   + 2   - 2   +  4 - 4 +    6').should == 6
  end

  it 'should calculate long multiplicative expressions from left to right' do
	@parser.parse('2 -4 +6 -1 -1- 0 +8').should == 10
	@parser.parse('1 -1   + 2   - 2   +  4 - 4 +    6').should == 6
  end

  it 'should calculate long, mixed additive and multiplicative expressions from left to right' do
	@parser.parse(' 2*3 - 4*5 + 6/3 ').should == -12
	@parser.parse('2*3*4/8 -   5/2*4 +  6 + 0/3   ').should == -1
  end

  it 'should return float pointing numbers when division result is not an integer' do
	@parser.parse('10/4').should == 2.5
	@parser.parse('5/3').should be_close(1.66, 0.01)
	@parser.parse('3 + 8/5 -1 -2*5').should be_close(-6.4, 0.01)
  end

  it 'should raise an error on wrong token' do
	lambda {@parser.parse('  6 + c')}.should raise_error()
	lambda {@parser.parse('  7 & 2')}.should raise_error()
	lambda {@parser.parse('  %')}.should raise_error()
  end

  it 'should raise an error on syntax error' do
	lambda {@parser.parse(' 5 + + 6')}.should raise_error()
	lambda {@parser.parse(' -5 + 2')}.should raise_error()
  end

  it 'should return Infinity when attempt to divide by zero occurs' do
	@parser.parse('5/0').should be_infinite
	@parser.parse(' 2 - 1 + 14/0 + 7').should be_infinite
  end

  it 'should compute 2 when given (2)' do
	@parser.parse('(2)').should == 2
  end

  it 'should compute complex expressions enclosed in parenthesis' do
	@parser.parse('(5 + 2*3 - 1 + 7 * 8)').should == 66
	@parser.parse('(67 + 2 * 3 - 67 + 2/1 - 7)').should == 1
  end

  it 'should compute expressions with many subexpressions enclosed in parenthesis' do
	@parser.parse('(2) + (17*2-30) * (5)+2 - (8/2)*4').should == 8
	@parser.parse('(5*7/5) + (23) - 5 * (98-4)/(6*7-42)').should be_infinite
  end

  it 'should handle nested parenthesis' do
	@parser.parse('(((((5)))))').should == 5
	@parser.parse('(( ((2)) + 4))*((5))').should == 30
  end

  it 'should raise an error on unbalanced parenthesis' do
	lambda {@parser.parse('2 + (5 * 2')}.should raise_error()
	lambda {@parser.parse('(((((4))))')}.should raise_error()
	lambda {@parser.parse('((2)) * ((3')}.should raise_error()
	lambda {@parser.parse('((9)) * ((1)')}.should raise_error()
  end
end
*/