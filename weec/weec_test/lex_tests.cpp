#include "tests.h"
#include "lex.h"
#include "error.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::error;
using namespace std;

namespace wctest
{
	namespace lex
	{
		int l_compare(vector<wcToken> p_correctVec, vector<wcToken> p_testVec);

		string					testString = "Test String";
		const char *			testConstChar = "Test String";
		vector<string>			twoLinesVector = { "Line 1" , "Line 2" }, 
								threeLinesVector = { "Line 1" , "Line 2", "Line 3" };
	}
}

//compare two vectors of wcTokens, return 1 if theyre equivalent, 0 if not
int wctest::lex::l_compare(vector<wcToken> p_correctVec, vector<wcToken> p_testVec)
{
	for (int t = 0; t < p_testVec.size(); ++t)
		if (p_correctVec[t] != p_testVec[t])
			return 0;
	return 1;
}

//create a lexer, lex a simple string ,dispose of the lexer
int wctest::lex::l_basic_1()
{

	return 0;
}

//lex some integers
int wctest::lex::l_basic_2()
{

	return 0;
}

//lex some floats
int wctest::lex::l_basic_3()
{

	return 0;
}

//lex with errors
int wctest::lex::l_basic_4()
{

	return 0;
}

//lex comments and numbers 
int wctest::lex::l_basic_5()
{

	return 0;
}

//lex random identifiers
int wctest::lex::l_basic_6()
{

	return 0;
}

int wctest::lex::l_basic_7()
{

	return 0;
}

//constructor tests / size, line & getLine methods
int wctest::lex::l_wcLexInputStream_1()
{
	wcLexInputStream stream1, stream2(testConstChar), stream5(twoLinesVector), stream6(threeLinesVector), 
		stream7(testString);

	//default constructor
	if (stream1.lines() != 0 && stream1.size() != 0)
		return 1;
	//const char *
	if (stream2.lines() != 1 && stream1.size() != 4 && stream2.getLine(0) != testConstChar)
		return 2;
	
	//vector<string>
	if (stream5.lines() != 2 && stream5.size() != 12 && stream5.getLine(0) != twoLinesVector[0] && stream5.getLine(1) != twoLinesVector[1])
		return 5;
	//vector<string> (3 lines)
	if (stream6.lines() != 3 && stream6.size() != 18 && stream6.getLine(0) != threeLinesVector[0] && stream6.getLine(1) != threeLinesVector[1] && stream6.getLine(2) != threeLinesVector[2])
		return 6;
	//string
	if (stream7.lines() != 1 && stream7.size() != 4 && stream7.getLine(0) != testString)
		return 7;

	//check first 3 letters are the same (operator[])
	if (stream7[0] != testString.substr(0, 1) || stream7[1] != testString.substr(1, 1) || stream7[2] != testString.substr(2, 1))
		return 8;

	return 0;
}

//operators
int wctest::lex::l_wcLexInputStream_2()
{
	wcLexInputStream stream1, stream2(testConstChar), stream5(twoLinesVector), stream6(threeLinesVector), stream7(testString);

	//operator== and operator!=
	if (stream1 != wcLexInputStream() || !(stream1 == wcLexInputStream()))
		return 1;
	if (stream2 != stream7 || !(stream2 == stream7))
		return 2;
	
	if (stream2.getLine(0) != testConstChar)
		return 5;

	return 0;
}

int wctest::lex::l_wcLexInputStream_3()
{
	return 0;
}

int wctest::lex::l_wcLexInputStreamIndex_1()
{
	wcLexInputStream stream = wcLexInputStream(), stream1(testString), stream2(twoLinesVector);

	wcLexInputStreamIndex index(stream), index1(stream1), index2(stream2);

	if (!(index.column == index.line == index.index == 0) || index.size() != 0 || index.isValid())
		return 1;

	if (!(index1.column == index1.line == index1.index == 0) || index1.size() != testString.size() || !index1.isValid()) 
		return 2;
	
	if (!(index2.column == index2.line == index2.index == 0) || index2.size() != twoLinesVector[0].size() + twoLinesVector[1].size() || !index2.isValid())
		return 3;

	return 0;
}

int wctest::lex::l_wcLexInputStreamIndex_2()
{
	wcLexInputStream stream = wcLexInputStream(), stream1(testString), stream2(twoLinesVector);

	wcLexInputStreamIndex index(stream), index1(stream1), index2(stream2);

	if((++index).isValid())
		return 1;

	if ((++index1).get() != testString.substr(1,1) || (index1 - 1).get() != testString.substr(0,1))
		return 2;

	wcLexer l;
	l.lex(wcLexInputStream("HE"));

	return 0;
}

int wctest::lex::l_wcLexInputStreamIndex_3()
{
	return 0;
}

