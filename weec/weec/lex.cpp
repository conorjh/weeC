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

wc::lex::wcTokenDefinitionBank::wcTokenDefinitionBank() : definitions
({
	//misc
	wcTokenDefinition(tt_null, ""),
	wcTokenDefinition(tt_ws, " ", true),
	wcTokenDefinition(tt_newline, "\n", true),
	wcTokenDefinition(tt_tab, "\t", true),
	wcTokenDefinition(tt_period, ".", true),
	wcTokenDefinition(tt_comma, ",", true),
	wcTokenDefinition(tt_scolon, ";", true),
	wcTokenDefinition(tt_colon, ":", true),
	wcTokenDefinition(tt_dcolon, "::", true),
	wcTokenDefinition(tt_tilde, "`", true),
	wcTokenDefinition(tt_underscore, "_", true),
	wcTokenDefinition(tt_amper, "&", true),
	wcTokenDefinition(tt_squote, "'", true),
	wcTokenDefinition(tt_dquote, "\"", true),
	wcTokenDefinition(tt_oparen, "(", true),
	wcTokenDefinition(tt_cparen, ")", true),
	wcTokenDefinition(tt_obrace, "{", true),
	wcTokenDefinition(tt_cbrace, "}", true),
	wcTokenDefinition(tt_obracket, "[", true),
	wcTokenDefinition(tt_cbracket, "]", true),

	//arithmetic
	wcTokenDefinition(tt_plus, "+", true),
	wcTokenDefinition(tt_minus, "-", true),
	wcTokenDefinition(tt_div, "/", true),
	wcTokenDefinition(tt_mult, "*", true),
	wcTokenDefinition(tt_mod, "%", true),
	wcTokenDefinition(tt_expo, "^", true),
	wcTokenDefinition(tt_incr, "++", true),
	wcTokenDefinition(tt_decr, "--", true),

	//logical/bool
	wcTokenDefinition(tt_lognot, "!", true),
	wcTokenDefinition(tt_logor, "||", true),
	wcTokenDefinition(tt_logand, "&&", true),
	wcTokenDefinition(tt_greater, ">", true),
	wcTokenDefinition(tt_greaterequal, ">=", true),
	wcTokenDefinition(tt_less, "<", true),
	wcTokenDefinition(tt_lessequal, "<=", true),
	wcTokenDefinition(tt_equal, "==", true),
	wcTokenDefinition(tt_notequal, "!=", true),

	//keywords
	wcTokenDefinition(tt_keyword_int, "int"),
	wcTokenDefinition(tt_keyword_chr, "char"),
	wcTokenDefinition(tt_keyword_string, "string"),
	wcTokenDefinition(tt_keyword_float, "float"),
	wcTokenDefinition(tt_keyword_bool, "bool"),
	wcTokenDefinition(tt_keyword_true, "true"),
	wcTokenDefinition(tt_keyword_false, "false"),
	wcTokenDefinition(tt_keyword_if, "if"),
	wcTokenDefinition(tt_keyword_else, "else"),
	wcTokenDefinition(tt_keyword_return, "return"),
	wcTokenDefinition(tt_keyword_while, "while"),
	wcTokenDefinition(tt_keyword_break, "break"),
	wcTokenDefinition(tt_keyword_continue, "continue"),
	wcTokenDefinition(tt_keyword_namespace, "namespace"),
	wcTokenDefinition(tt_keyword_function, "namespace"),

	wcTokenDefinition(tt_intlit, {"0","1","2","3","4","5","6","7","8","9"})
	})
{
	populaterDelimiterTypes();
}

wc::lex::wcTokenDefinitionBank::wcTokenDefinitionBank(const std::vector<wcTokenDefinition>& _definitions) : definitions(_definitions)
{
	populaterDelimiterTypes();
}

const bool wc::lex::wcTokenDefinitionBank::exists(const char * identToCheck)
{
	for (int t = 0; t < definitions.size(); ++t)
		for (int y = 0; t < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == identToCheck)
				return true;
	return false;
}

const bool wc::lex::wcTokenDefinitionBank::exists(char identToCheck)
{
	stringstream ss;
	ss << identToCheck;
	return exists(ss.str().c_str());
}

const bool wc::lex::wcTokenDefinitionBank::exists(std::string identToCheck)
{
	return exists(identToCheck.c_str());
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(const char *identToCheck)
{
	for (int t = 0; t < definitions.size(); ++t)
		for (int y = 0; t < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == identToCheck)
				return definitions[t];
	return wcTokenDefinition();
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(char identToCheck)
{
	stringstream ss;
	ss << identToCheck;
	return find(ss.str().c_str());
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(std::string identToCheck)
{
	return find(identToCheck.c_str());
}

void wc::lex::wcTokenDefinitionBank::populaterDelimiterTypes()
{
	for (int t = 0; t < definitions.size(); ++t)
		if (definitions[t].delimiter)
			delimiterTypes.push_back(definitions[t].type);
}

wc::lex::wcTokenDefinition::wcTokenDefinition() : type(tt_null), identifiers({}), delimiter(false)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, const char * identifier) : type(_type), identifiers({ identifier }), delimiter(false)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, const char * identifier, bool _isDelim) : type(_type), identifiers({ identifier }), delimiter(_isDelim)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers) : type(_type), identifiers(_identifiers), delimiter(false)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter) : type(_type), identifiers(_identifiers), delimiter(_isDelimiter)
{

}

const bool wc::lex::wcTokenDefinition::isNull()
{
	if (type == tt_null && delimiter == false && identifiers.size() == 0)
		return true;
	else
		return false;
}

const bool wc::lex::wcTokenDefinition::isSingleCharacterToken()
{
	if (identifiers.size() == 0)
		return false;

	unsigned int largestIdentifierSize = 0;
	for (int t = 0; t < identifiers.size(); ++t)
		if (identifiers[t].size() > largestIdentifierSize)
			largestIdentifierSize = identifiers[t].size();

	return !(largestIdentifierSize > 1);
}

bool wc::lex::setErrorReturnFalse(wcError& p_error, wcError p_newError)
{
	p_error = p_newError;
	return false;
}

vector<string> wc::lex::tokenizeString(string input)
{
	vector<string> output;
	auto tokens = lexingLexer.lex(input);
	for (int t = 0; t < tokens.size(); ++t)
		output.push_back(tokens[t].data);
	return output;
}

wc::lex::wcTokenTypeDeriver::wcTokenTypeDeriver() : definitionsBank(wcTokenDefinitionBank())
{

}

wc::lex::wcTokenTypeDeriver::wcTokenTypeDeriver(wcTokenDefinitionBank &_definitionsBank) : definitionsBank(_definitionsBank)
{

}


wcTokenType wc::lex::wcTokenTypeDeriver::derive(string input)
{
	return derive(input.c_str());
}

//returns the the token type of a given string
wcTokenType wc::lex::wcTokenTypeDeriver::derive(char input)
{
	stringstream ss;
	ss << input;
	return derive(ss.str().c_str());
}

//returns the the token type of a given string
wcTokenType wc::lex::wcTokenTypeDeriver::derive(const char * input)
{
	if (input == "")	
		return tt_null;

	wcTokenDefinition possibleDefinition = definitionsBank.find(input);

	if (possibleDefinition.isNull())
		return tt_ident;	//no match, treat as identifier (variable name etc)

	return possibleDefinition.type;	//matches a known token string
}

//is a given string a delimiter
bool wc::lex::wcTokenTypeDeriver::isDelim(string input)
{
	return isDelim(derive(input));
}

bool wc::lex::wcTokenTypeDeriver::isDelim(const char *input)
{
	return isDelim(derive(input));
}

bool wc::lex::wcTokenTypeDeriver::isDelim(char input)
{
	stringstream ss;
	ss << input;
	return isDelim(ss.str().c_str());
}

bool wc::lex::wcTokenTypeDeriver::isDelim(wcTokenType p_type)
{
	//.find() returns a tt_null typed wcTokenDefinition if no match was found
	return definitionsBank.find(p_type).delimiter;
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

wc::lex::wcLexer::wcLexer() : deriver(definitionsBank)
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
	if (column >= source->at(line).size())
		if (line + 1 < source->size())
		{
			line++;
			column = 0;
			return getChar();	//next character from the new line
		}
		else
		{
			return "";	//end of stream
		}

	return getChar();	//next character
}

//retrieve the next character from the stream without moving the index ahead
string wc::lex::wcLexIndex::peekChar()
{
	if (!isValid())
		return "";

	//check theres another character
	if (column + 1 >= source->at(line).size())
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

vector<wcToken> wc::lex::wcLexer::lex(vector<string> input)
{
	vector<wcToken> out;
	lexIndex.source = &input;

	//loop through characters making tokens until end of stream
	while (lexIndex.isValid())
		switch (deriver.derive(lexIndex.getChar()))
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

vector<wcToken> wc::lex::wcLexer::lex(string input)
{
	return lex(input.c_str());
}

vector<wcToken> wc::lex::wcLexer::lex(const char * input)
{
	vector<string> inputHolder = { input };
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
	while (p_index.isValid() && !deriver.isDelim(p_index.getChar()))
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