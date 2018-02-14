#include "parse.h"
#include "lex.h"
#include <sstream>

using namespace std;
using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::error;

namespace wc
{
	namespace lex
	{
		
		wcLexer lexingLexer;
	}
}

bool wc::lex::setErrorReturnFalse(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return false;
}

wcTokenType wc::lex::wcLexer::deriveTokenType(string p_input)
{
	return lexingLexer.deriveTokenType(p_input.c_str());
}

//returns the the token type of a given string
wcTokenType wc::lex::wcLexer::deriveTokenType(char p_input)
{
	stringstream ss;
	ss << p_input;
	return lexingLexer.deriveTokenType(ss.str().c_str());
}

//returns the the token type of a given string
wcTokenType wc::lex::wcLexer::deriveTokenType(const char * p_input)
{
	if (p_input == "")	
		return tt_null;

	auto result = tokenStrings.find(p_input);

	if (result == tokenStrings.end())
		return tt_ident;	//no match, treat as identifier (variable name etc)

	return result->second;	//matches a known token string
}

bool wc::lex::wcLexer::isPunctuation(wcTokenType p_type)
{
	if (p_type == tt_period || p_type == tt_dcolon)
		return true;
	return false;
}

//is a given string a delimiter
bool wc::lex::wcLexer::isDelim(string p_input)
{
	return isDelim(lexingLexer.deriveTokenType(p_input));
}

bool wc::lex::wcLexer::isDelim(wcTokenType p_type)
{
	if (p_type != tt_null)
	{
		auto result = std::find(delimTypes.begin(), delimTypes.end(), p_type);
		if (result != delimTypes.end())
			return true;
		else
			return false;
	}
	return false;
}

//whether or not to add a given token to the output
bool wc::lex::wcLexer::isDelimDroppable(wcTokenType p_tokenType)
{
	switch (p_tokenType)
	{
	CASE_TT_WS
		if (wc_lexer_dropWS)
			return true;
		else
			return false;
	default:
		return false;
	}
}

bool wc::lex::wcLexer::isDelimDroppable(wcToken p_token)
{
	return isDelimDroppable(deriveTokenType(p_token.data));
}

wc::lex::wcToken::wcToken()
{
	type = tt_null;
	line = col = 0;
	data = "";
}

wc::lex::wcToken::wcToken(wc::parse::wcSymbol p_sym)
{
	*this = wcToken();
	type = tt_ident;
	data = p_sym.fullyQualifiedIdent;
}

wc::lex::wcToken::wcToken(wcTokenType p_tt)
{
	*this = wcToken();
	type = p_tt;
}

wc::lex::wcToken::wcToken(wcTokenType p_tt, string p_data)
{
	*this = wcToken();
	type = p_tt;
	data = p_data;
}

wc::lex::wcToken::wcToken(wcTokenType p_tt, string p_data, int p_line, int p_col)
{
	type = p_tt;
	data = p_data;
	line = p_line;
	col = p_col;
}

wc::lex::wcToken::wcToken(string p_data)
{
	*this = wcToken();
	type = lexingLexer.deriveTokenType(p_data);
	data = p_data;
}

wc::lex::wcLexer::wcLexer()
{

}

wc::lex::wcLexer::~wcLexer()
{

}

wc::lex::wcLexIndex::wcLexIndex()
{
	index = line = column = 0;
	source = nullptr;
}

//reset the index back to the first line and column
void wc::lex::wcLexIndex::reset()
{
	index = line = column = 0;
}

//return the character currently pointed to by the index
string wc::lex::wcLexIndex::getChar()
{
	string out = source->at(line);
	if (isValid())
		return out.substr(column,1);
	return "";
}

//move the index to the next character in the stream, updating info as we go
string wc::lex::wcLexIndex::nextChar()
{
	if (!isValid())
		return "";
	
	++index;	++column;
	if (column >= source[line].size())
		if (line + 1 < source->size())
		{
			line++;
			column = 0;
			return getChar();	//next character from the new line
		}
		else
			return "";	//end of stream

	return getChar();	//next character
}

//retrieve the next character from the stream without moving the index ahead
string wc::lex::wcLexIndex::peekChar()
{
	if (!isValid())
		return "";

	//check theres another character
	if (column + 1 >= source[line].size())
		if (line + 1 >= source->size())
			return "";	//eos
		else
			return source->at(line + 1).substr(0, 1);	//next character is on a newline
	else
		return source->at(line).substr(column + 1,1); //next character
}

//do we have a source loaded, and are there still character to read
bool wc::lex::wcLexIndex::isValid()
{
	if(source != nullptr && line > -1 && column > -1 && index > -1 && (column < getSize()))
		return true;
	return false;
}

unsigned int wc::lex::wcLexIndex::getSize()
{
	string s = source->at(line);
	if (!source->size())
		return 0;
	return s.size();
}

bool wc::lex::wcToken::operator==(const wcToken& p_token) const
{
	return (this->type == p_token.type && this->data == p_token.data);
}

bool wc::lex::wcToken::operator!=(const wcToken& p_token) const
{
	return !(this->type == p_token.type && this->data == p_token.data);
}

bool wc::lex::wcLexer::isError()
{
	return error != ec_null;
}

wc::error::wcError wc::lex::wcLexer::getError()
{
	return error;
}

void wc::lex::wcLexer::setError(wcError p_err)
{
	error = p_err;
}

vector<wcToken> wc::lex::wcLexer::lex(vector<string> p_input)
{
	vector<wcToken> out;
	lexIndex.source = &p_input;

	//loop through characters making tokens until end of stream
	while (lexIndex.isValid())
		switch (lexingLexer.deriveTokenType(lexIndex.getChar()))
		{
			//string literal
		case tt_dquote:
			if (!lex_stringLiteral(out, lexIndex, error))
				return out;
			break;

			//integer literals
		case tt_intlit:
			if (!lex_intLiteral(out, lexIndex, error))
				return out;
			break;

			//all 2 part tokens (operators etc)
		case tt_greater: case tt_less: case tt_lognot:
		case tt_assign:	case tt_plus: case tt_minus:
		case tt_amper: case tt_pipe: case tt_colon:
			if (!lex_2step(out, lexIndex, error))
				return out;
			break;

			//whitespace
		CASE_TT_WS
			if (!lex_ws(out, lexIndex, error))
				return out;
			break;

			//comments
		case tt_div:
			if (!lex_comment(out, lexIndex, error))
				return out;
			break;

			//possible delimiter or identifier etc
		default:
			if (!lex_default(out, lexIndex, error))
				return out;
			break;
		}

	return out;
}

vector<wcToken> wc::lex::wcLexer::lex(string p_input)
{
	return lex(p_input.c_str());
}

vector<wcToken> wc::lex::wcLexer::lex(const char * p_input)
{
	vector<string> inputHolder = { p_input };
	vector<wcToken> out;
	lexIndex.source = &inputHolder;

	return lex(inputHolder);
}

bool wc::lex::lex_default(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	string strBuff;
	int origLine = p_index.line;
	int origCol = p_index.column;

	//build up the token until we hit a delimiter or eos
	while (p_index.isValid() && !lexingLexer.isDelim(p_index.getChar()))
	{
		strBuff += p_index.getChar();
		p_index.nextChar();
	}
	
	//create a token for the delimiter unless it was end of stream
	wcToken preDelim(lexingLexer.deriveTokenType(strBuff), strBuff, origLine, origCol);
	if (lexingLexer.isDelim(p_index.getChar()))
	{
		if(preDelim.type != tt_null)
			p_output.push_back(preDelim);
		
		wcToken delim(lexingLexer.deriveTokenType(p_index.getChar()), p_index.getChar(), p_index.line, p_index.column);
		if(!lexingLexer.isDelimDroppable(delim))
			p_output.push_back(delim);
	}
	else
	{
		strBuff += p_index.getChar();	//eos
		p_output.push_back(preDelim);
	}

	p_index.nextChar();
	return true;
}

bool wc::lex::lex_2step(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	//dual step tokens	

	if (!(
		( lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar())) ) != tt_greaterequal && (lexingLexer.deriveTokenType( (p_index.getChar() + p_index.peekChar() ))) != tt_lessequal &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_notequal && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_equal &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_incr && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_plusassign &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_decr && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_minusassign &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_logand && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_logor &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_dcolon
		))
	{
		wcToken twoStep(lexingLexer.deriveTokenType(p_index.getChar() + p_index.peekChar()), p_index.getChar() + p_index.peekChar(),p_index.line, p_index.column);
		p_output.push_back(twoStep);
		p_index.nextChar();
		p_index.nextChar();
	}
	else
	{
		p_output.push_back(wcToken(lexingLexer.deriveTokenType(p_index.getChar()), p_index.getChar(), p_index.line, p_index.column));
		p_index.nextChar();
	}
	return true;
}

bool wc::lex::lex_intLiteral(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	string leftValue,rightValue;
	int origCol = p_index.column;
	int origLine = p_index.line;
	bool firstHalf = true; 
	bool breakWhile = false;

	//make sure we have an opening int
	if (lexingLexer.deriveTokenType(p_index.getChar()) != tt_intlit)
		return setErrorReturnFalse(p_error, wcError(ec_lex_unexpectedtoken, "Expected a numeric value (0-9)", origLine, origCol));

	//get all the numbers til we hit not a number
	while (p_index.isValid() && !breakWhile)
		switch (lexingLexer.deriveTokenType(p_index.getChar()))
		{
		case tt_intlit:
			if(firstHalf)
				leftValue += p_index.getChar();
			else
				rightValue += p_index.getChar();
			p_index.nextChar();
			break;

		case tt_period:
			if (!leftValue.size() && firstHalf)
				return setErrorReturnFalse(p_error, wcError(ec_lex_unexpectedtoken, "Expected a numeric value (0-9)", p_index.line, p_index.column));
			else if (!firstHalf)
				return setErrorReturnFalse(p_error, wcError(ec_lex_unexpectedtoken, "Encountered a second period", p_index.line, p_index.column));

			//float encountered, continue lexing the second half after the decimal
			firstHalf = false;
			p_index.nextChar();
			break;

		default:
			breakWhile = true;
			break;
		}

	//check what we found
	if (firstHalf && leftValue.size() && (!rightValue.size()))
		p_output.push_back(wcToken(tt_intlit, leftValue, origLine, origCol));
	else if (!firstHalf && leftValue.size() && rightValue.size())
		p_output.push_back(wcToken(tt_fltlit, leftValue + "." + rightValue, origLine, origCol));
	else
		return setErrorReturnFalse(p_error, wcError(ec_lex_somekindoferror, "Some kind of error lexing a float", origLine, origCol));
	return true;
}

//parse a string literal within double quotes
bool wc::lex::lex_stringLiteral(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	string strBuff;
	int origLine = p_index.line;
	int origCol = p_index.column;

	//make sure we have an opening double quote
	if (lexingLexer.deriveTokenType(p_index.getChar()) != tt_dquote)
	{
		p_error = wcError(ec_lex_unexpectedtoken, "Expected '\"'", origLine, origCol);
		return false;
	}
	p_index.nextChar();

	//loop through until we reach the closing quote, or end of stream
	while (p_index.isValid())
		switch (lexingLexer.deriveTokenType(p_index.getChar()))
		{
		case tt_dquote:
			p_output.push_back(wcToken(tt_strlit, strBuff, origLine, origCol));
			p_index.nextChar();
			return true;

		default:
			strBuff += p_index.getChar();
			p_index.nextChar();
			break;
		}

	//end of stream before end of string, error
	return setErrorReturnFalse(p_error, wcError(ec_lex_eos_strlit, strBuff, origLine, origCol));
}

//parse whitespace, tabs and newlines as one solid block. no token produced if wc_lexer_dropWS == true
bool wc::lex::lex_ws(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	string strBuff;
	bool breakWhile = false;
	int origLine = p_index.line;
	int origCol = p_index.column;
	wcTokenType tt = lexingLexer.deriveTokenType(p_index.getChar());

	//make sure we have an opening whitespace
	if (tt != tt_ws && tt!= tt_tab && tt!= tt_newline)
		return setErrorReturnFalse(p_error, wcError(ec_lex_unexpectedtoken, "Expected '\"'", origLine, origCol));

	//clump all consecutive whitespace together
	while (p_index.isValid() && !breakWhile)
		switch (lexingLexer.deriveTokenType(p_index.getChar()))
		{
		CASE_TT_WS
			if (!wc_lexer_dropWS)
				strBuff += p_index.getChar();		//preserve whitespace tokens
			p_index.nextChar();
			break;

		//end of whitespace as soon as we reach another token or eos
		default:
			breakWhile = true;
			break;
		}

	if (!wc_lexer_dropWS)
		p_output.push_back(wcToken(lexingLexer.deriveTokenType(strBuff), strBuff, origLine, origCol));
	return true;
}

//parse single (//) and multi-line (/*  */) comments, no token produced
bool wc::lex::lex_comment(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	//make sure we have an opening / 
	if (lexingLexer.deriveTokenType(p_index.getChar()) != tt_div)
		return setErrorReturnFalse(p_error, wcError(ec_lex_unexpectedtoken, "Expected '/'", p_index.line, p_index.column));

	//handle multi line elsewhere
	if (lexingLexer.deriveTokenType(p_index.peekChar()) == tt_mult)
		return lex_commentMultiLine(p_output, p_index, p_error);
	else if (lexingLexer.deriveTokenType(p_index.peekChar()) != tt_div)
	{
		//just a single division symbol (/) not actually a comment (//)
		p_output.push_back(wcToken(tt_div, p_index.getChar(), p_index.line, p_index.column));
		p_index.nextChar();
		return true;
	}

	//go through until end of line or eos
	while(p_index.isValid())
		switch (lexingLexer.deriveTokenType(p_index.getChar()))
		{
		default:
			p_index.nextChar();
			break;

		case tt_newline:
			p_index.nextChar();
			return true;
		}

	//end of stream
	return true;
}

//lex and drop any multi line comments
//assume first token is / (tt_div), and second token is tt_mult
bool wc::lex::lex_commentMultiLine(vector<wcToken>& p_output, wcLexIndex& p_index, wcError& p_error)
{
	//skip past tt_mult
	p_index.nextChar();

	//go through until */ or eos
	while (p_index.isValid())
		switch (lexingLexer.deriveTokenType(p_index.getChar()))
		{
		case tt_mult:
				if (lexingLexer.deriveTokenType(p_index.nextChar()) == tt_div)
				{
					p_index.nextChar();
					return true;	//end of comment found
				}

		default:
			p_index.nextChar();
			break;
		}

	//end of stream
	return true;
}