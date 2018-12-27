#include "parse.h"
#include "lex.h"
#include <sstream>

using namespace std;
using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::error;

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
	wcTokenDefinition(tt_keyword_char, "char"),
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
	populateDelimiterTypes();
}

wc::lex::wcTokenDefinitionBank::wcTokenDefinitionBank(const std::vector<wcTokenDefinition>& _definitions) : definitions(_definitions)
{
	populateDelimiterTypes();
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

void wc::lex::wcTokenDefinitionBank::populateDelimiterTypes()
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

wc::lex::wcToken::wcToken(wcTokenType p_tt, const char * p_data, int p_line, int p_col)
{
	type = p_tt;
	data = p_data;
	line = p_line;
	column = p_col;
}

wc::lex::wcLexer::wcLexer() : deriver(definitionsBank)
{

}

wc::lex::wcLexer::~wcLexer()
{

}

wc::lex::wcLexInputStreamIndex::wcLexInputStreamIndex(wcLexInputStream &stream) : source(stream)
{
	line = column = index = 0;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator--()
{
	wcLineColumnIndex newLineColumnIndex(source, index - 1);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator++()
{
	wcLineColumnIndex newLineColumnIndex(source, index + 1);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(int subtraction)
{
	wcLineColumnIndex newLineColumnIndex(source, index - subtraction);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(int addition)
{
	wcLineColumnIndex newLineColumnIndex(source, index + addition);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(wcLexInputStreamIndex newIndex)
{
	int currentIntIndex = index, newIntIndex = newIndex.index;
	wcLineColumnIndex newLineColumnIndex(source, currentIntIndex - newIntIndex);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(wcLexInputStreamIndex newIndex)
{
	int currentIntIndex = index, newIntIndex = newIndex.index;
	wcLineColumnIndex newLineColumnIndex(source, currentIntIndex + newIntIndex);
	*this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(wcLexInputStreamIndex input)
{
	index = input.index;
	column = input.column;
	line = input.line;
	source = source;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(int newIndex)
{
	*this = wcLineColumnIndex(source, newIndex);
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(wcLineColumnIndex newLineColumnIndex)
{
	line = newLineColumnIndex.line;
	column = newLineColumnIndex.column;
	index = newLineColumnIndex.index;
	source = newLineColumnIndex.source;
}

//pass through to the InputStream
const char * wc::lex::wcLexInputStreamIndex::operator[](int index)
{
	return source[index];
}

//reset the index back to the first line and column
void wc::lex::wcLexInputStreamIndex::reset()
{
	index = line = column = 0;
}

unsigned int wc::lex::wcLexInputStreamIndex::size()
{
	return sizeof(column) + sizeof(line) + sizeof(index) + sizeof(source);
}

wc::lex::wcLexInputStream::wcLexInputStream()
{

}

wc::lex::wcLexInputStream::wcLexInputStream(const char * input)
{
	container.push_back(input);
}

wc::lex::wcLexInputStream::wcLexInputStream(std::string input)
{
	container.push_back(input.c_str());
}

wc::lex::wcLexInputStream::wcLexInputStream(std::vector<const char*> input)
{
	container = input;
}

const char * wc::lex::wcLexInputStream::operator[](int charIndex)
{
	//get the correct line and column
	wcLineColumnIndex lcIndex(*this, charIndex);

	return string(container[lcIndex.line]).substr(lcIndex.column, 1).c_str();
}

const char * wc::lex::wcLexInputStream::next(wcLexInputStreamIndex &index)
{
	index++;
	return get(index);
}

const char * wc::lex::wcLexInputStream::get(wcLexInputStreamIndex &index)
{
	if (!(index).isValid())
		return nullptr;

	return string(container[index.line]).substr(index.column, 1).c_str();
}

const char * wc::lex::wcLexInputStream::get(int _line, int _column)
{
	wcLineColumnIndex lcIndex(*this, _line, _column);

	if (lcIndex.isValid())
		return string(container[_line]).substr(_column, 1).c_str();
	else
		return nullptr;
}

//get the amount of characters in stream
const unsigned int wc::lex::wcLexInputStream::size()
{
	if (lines() == 1)
		return strlen(container[0]);

	unsigned int sizeAccumulator = 0;
	for (int t = 0; t < lines(); ++t)
		sizeAccumulator += strlen(container[t]);

	return sizeAccumulator;
}

//get the amount of characters in a given line 
const unsigned int wc::lex::wcLexInputStream::size(unsigned int lineNumber)
{
	return strlen(container[lineNumber]);
}

//get the amount of lines in the input
const unsigned int wc::lex::wcLexInputStream::lines()
{
	return container.size();
}

//do we have a source loaded, and are there still characters to read
bool wc::lex::wcLexInputStreamIndex::isValid()
{
	if (line > -1 && column > -1 && index > -1 && (column < source.size(line)))
		return true;
	return false;
}

bool wc::lex::wcToken::operator==(const wcToken& p_token) const
{
	//dont compare line/column
	return (this->type == p_token.type && this->data == p_token.data);
}

const char * wc::lex::wcToken::operator[](unsigned int index)
{
	return std::string(data).substr(index, 1).c_str();
}

wcToken wc::lex::wcToken::operator=(const char *input)
{
	data = input;
	return *this;
}

wcToken wc::lex::wcToken::operator=(wcToken input)
{
	line = input.line;
	column = input.column;
	data = input.data;
	type = input.type;
	return *this;
}

wc::lex::wcToken::wcToken(wcTokenType _type, const char * _data, int _line, int _column)
{
	type = _type;
	data = _data;
	line = _line;
	column = _column;
}

bool wc::lex::wcToken::operator!=(const wcToken& p_token) const
{
	return !(this->type == p_token.type && this->data == p_token.data);
}

wcTokenStream wc::lex::wcLexer::lex(wcLexInputStream& stream)
{
	wcLexInputStreamIndex lexIndex(stream);
	wcTokenStream out;

	//loop through characters making tokens until end of stream
	while ((lexIndex++).isValid())
		switch (deriver.derive(stream.get(lexIndex)))
		{
		//string literal
		case tt_dquote:
			if ((out += lex_stringLiteral(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

		//integer literals
		case tt_intlit:
			if ((out += lex_intLiteral(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

		//all 2 part tokens (operators etc)
		CASE_ALL_2_PARTERS
			if ((out += lex_2step(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

		//whitespace
		CASE_TT_WS
			if ((out += lex_ws(lexIndex)).isError())
				return out;
			break;

		//comments
		case tt_div:
			if ((out += lex_comment(lexIndex)).isError())
				return out;
			break;

		//possible delimiter or identifier etc
		default:
			if ((out += lex_default(lexIndex)).isError())
				return out;
		}

	return out;
}


bool wc::lex::lex_default(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
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
		if (preDelim.type != tt_null)
			p_output.push_back(preDelim);

		wcToken delim(lexingLexer.deriveTokenType(p_index.getChar()), p_index.getChar(), p_index.line, p_index.column);
		if (!lexingLexer.isDelimDroppable(delim))
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

bool wc::lex::lex_2step(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
{
	//dual step tokens	

	if (!(
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_greaterequal && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_lessequal &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_notequal && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_equal &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_incr && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_plusassign &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_decr && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_minusassign &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_logand && (lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_logor &&
		(lexingLexer.deriveTokenType((p_index.getChar() + p_index.peekChar()))) != tt_dcolon
		))
	{
		wcToken twoStep(lexingLexer.deriveTokenType(p_index.getChar() + p_index.peekChar()), p_index.getChar() + p_index.peekChar(), p_index.line, p_index.column);
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

bool wc::lex::lex_intLiteral(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
{
	string leftValue, rightValue;
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
			if (firstHalf)
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
bool wc::lex::lex_stringLiteral(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
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
bool wc::lex::lex_ws(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
{
	string strBuff;
	bool breakWhile = false;
	int origLine = p_index.line;
	int origCol = p_index.column;
	wcTokenType tt = lexingLexer.deriveTokenType(p_index.getChar());

	//make sure we have an opening whitespace
	if (tt != tt_ws && tt != tt_tab && tt != tt_newline)
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
bool wc::lex::lex_comment(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
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
	while (p_index.isValid())
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
bool wc::lex::lex_commentMultiLine(vector<wcToken>& p_output, wcLexInputStreamIndex& p_index, wcError& p_error)
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

wc::lex::wcLineColumnIndex::wcLineColumnIndex(wcLexInputStream & _source, int _index) : source(_source)
{
	updateFromIndex(_index);
}

wc::lex::wcLineColumnIndex::wcLineColumnIndex(wcLexInputStream & _source, int _line, int _column) : source(_source)
{
	updateFromLineColumn(_line, _column);
}

bool wc::lex::wcLineColumnIndex::updateFromIndex(int newIndex)
{
	if ((newIndex < 0) || (line > source.lines()) || newIndex >= source.size())
	{
		line = -1;
		column = -1;
		return false;
	}

	//progress through the input until we reach our given index
	int tempIndex = 0, tempCol = 0;
	for (int t = 0; t < source.lines(); ++t)
		if (tempIndex + source.size(t) < newIndex)
		{
			//havent reached the correct line yet
			tempIndex += source.size(t);
			tempCol = source.size(t);
		}
		else
		{
			this->column = newIndex - tempIndex;
			this->line = t;
			this->index = newIndex;
			return true;
		}

	//error somehow
	return false;
}

bool wc::lex::wcLineColumnIndex::updateFromLineColumn(int _line, int _column)
{
	int indexAccumulator = 0;
	for (int t = 0; t <= _line - 1; ++t)
		indexAccumulator += source.size(t);
	index = indexAccumulator + _column;

	line = _line;
	column = _column;
}

bool wc::lex::wcLineColumnIndex::isValid()
{
	if (column < 0 || line < 0 || line >= source.lines())
		return false;


}

bool wc::lex::wcTokenStreamIndex::isValid()
{
	return false;
}

void wc::lex::wcTokenStreamIndex::reset()
{
}

wcTokenStream wc::lex::wcLexer::lex_stringLiteral(wcLexInputStreamIndex & index)
{
	string strBuff;
	int origLine = index.line;
	int origCol = index.column;

	//make sure we have an opening double quote
	if (deriver.derive(index.getChar()) != tt_dquote)
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

wcTokenStream wc::lex::wcLexer::lex_commentMultiLine(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_comment(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_default(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_2step(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_ws(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_intLiteral(wcLexInputStreamIndex & index)
{
	return wcTokenStream();
}

wc::lex::wcTokenStream::wcTokenStream(error::wcError _error)
{
	error = _error;
}

wcTokenStream wc::lex::wcTokenStream::operator+(wcTokenStream stream)
{
	for (int t = 0; t < stream.container.size(); ++t)
		container.push_back(stream.container[t]);
	
	//transfer error data unless there were no errors
	if (stream.error.code)
		error = stream.error;

	return *this;
}

wcTokenStream wc::lex::wcTokenStream::operator+(wcToken token)
{
	container.push_back(token);
	return *this;
}

wcTokenStream wc::lex::wcTokenStream::operator+=(wcToken token)
{
	return *this + token;
}

wcTokenStream wc::lex::wcTokenStream::operator+=(wcTokenStream stream)
{
	return *this + stream;
}

wc::lex::wcTokenStreamIndex::wcTokenStreamIndex(wcTokenStream &)
{
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator-(int)
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(wcTokenStreamIndex)
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator-(wcTokenStreamIndex)
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator=(wcTokenStreamIndex)
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator++()
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator--()
{
	return wcTokenStreamIndex();
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(int)
{
	return wcTokenStreamIndex();
}
