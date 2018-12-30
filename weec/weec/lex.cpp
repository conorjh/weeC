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

wc::lex::wcTokenDefinitionBank::wcTokenDefinitionBank(const vector<wcTokenDefinition>& _definitions) : definitions(_definitions)
{
	populateDelimiterTypes();
}

const bool wc::lex::wcTokenDefinitionBank::exists(string identToCheck)
{
	for (int t = 0; t < definitions.size(); ++t)
		for (int y = 0; t < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == string(identToCheck))
				return true;
	return false;
}

const bool wc::lex::wcTokenDefinitionBank::exists(char identToCheck)
{
	stringstream ss;
	ss << identToCheck;
	return exists(ss.str());
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(string identToCheck)
{
	for (int t = 0; t < definitions.size(); ++t)
		for (int y = 0; y < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == string(identToCheck))
				return definitions[t];
	return wcTokenDefinition();
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(char identToCheck)
{
	stringstream ss;
	ss << identToCheck;
	return find(ss.str());
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

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier) : type(_type), identifiers({ identifier }), delimiter(false)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier, bool _isDelim) : type(_type), identifiers({ identifier }), delimiter(_isDelim)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers) : type(_type), identifiers(_identifiers), delimiter(false)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers, bool _isDelimiter) : type(_type), identifiers(_identifiers), delimiter(_isDelimiter)
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

bool wc::lex::setErrorReturnFalse(wcError& error, wcError newError)
{
	error = newError;
	return false;
}

wc::lex::wcTokenTypeDeriver::wcTokenTypeDeriver() : definitionsBank(wcTokenDefinitionBank())
{

}

wc::lex::wcTokenTypeDeriver::wcTokenTypeDeriver(wcTokenDefinitionBank &_definitionsBank) : definitionsBank(_definitionsBank)
{

}

//returns the the token type of a given string
wcTokenType wc::lex::wcTokenTypeDeriver::derive(char input)
{
	stringstream ss;
	ss << input;
	return derive(ss.str());
}

//returns the the token type of a given string
wcTokenType wc::lex::wcTokenTypeDeriver::derive(string input)
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

bool wc::lex::wcTokenTypeDeriver::isDelim(char input)
{
	stringstream ss;
	ss << input;
	return isDelim(ss.str());
}

bool wc::lex::wcTokenTypeDeriver::isDelim(wcTokenType type)
{
	//.find() returns a tt_null typed wcTokenDefinition if no match was found
	return definitionsBank.find(type).delimiter;
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

//postdecrement
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator--(int)
{
	wcLexInputStreamIndex retVal = *this;
	wcLineColumnIndex newLineColumnIndex(source, index - 1);
	*this = newLineColumnIndex;
	return retVal;
}
//postincrement
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator++(int)
{
	wcLexInputStreamIndex retVal = *this;
	wcLineColumnIndex newLineColumnIndex(source, index + 1);
	*this = newLineColumnIndex;
	return retVal;
}

//predecrement
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(int subtraction)
{
	wcLineColumnIndex newLineColumnIndex(source, index - subtraction);
	return *this = newLineColumnIndex;
}

//preincrement
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator++()
{
	wcLineColumnIndex newLineColumnIndex(source, index + 1);
	return *this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator--()
{
	wcLineColumnIndex newLineColumnIndex(source, index + 1);
	return *this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(int addition)
{
	wcLineColumnIndex newLineColumnIndex(source, index + addition);
	return *this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(wcLexInputStreamIndex newIndex)
{
	int currentIntIndex = index, newIntIndex = newIndex.index;
	wcLineColumnIndex newLineColumnIndex(source, currentIntIndex - newIntIndex);
	return *this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(wcLexInputStreamIndex newIndex)
{
	int currentIntIndex = index, newIntIndex = newIndex.index;
	wcLineColumnIndex newLineColumnIndex(source, currentIntIndex + newIntIndex);
	return *this = newLineColumnIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(wcLexInputStreamIndex input)
{
	index = input.index;
	column = input.column;
	line = input.line;
	source = source;
	return *this;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(int newIndex)
{
	return *this = wcLineColumnIndex(source, newIndex);
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator=(wcLineColumnIndex newLineColumnIndex)
{
	line = newLineColumnIndex.line;
	column = newLineColumnIndex.column;
	index = newLineColumnIndex.index;
	source = newLineColumnIndex.source;
	return *this;
}

//pass through to the InputStream
string wc::lex::wcLexInputStreamIndex::operator[](int index)
{
	return source[index];
}

//reset the index back to the first line and column
void wc::lex::wcLexInputStreamIndex::reset()
{
	index = line = column = 0;
}

string wc::lex::wcLexInputStreamIndex::next()
{
	return source.next(*this);
}

std::string wc::lex::wcLexInputStreamIndex::peek(int lookAhead)
{
	if ((*this + lookAhead).isValid())
		return (*this + lookAhead).get();
	return "";
}

std::string wc::lex::wcLexInputStreamIndex::peek()
{
	wcLexInputStreamIndex peekIndex = *this;
	if((++peekIndex).isValid())
		return peekIndex.get();
	return "";
}

string wc::lex::wcLexInputStreamIndex::get(int _line, int _column)
{
	return source.get(_line,_column);
}

string wc::lex::wcLexInputStreamIndex::get()
{
	return source.get(*this);
}

unsigned int wc::lex::wcLexInputStreamIndex::size()
{
	return source.size();
}

wc::lex::wcLexInputStream::wcLexInputStream()
{

}

wc::lex::wcLexInputStream::wcLexInputStream(const char * input)
{
	container.push_back(input);
}

wc::lex::wcLexInputStream::wcLexInputStream(string input)
{
	container.push_back(input);
}

wc::lex::wcLexInputStream::wcLexInputStream(vector<string> input)
{
	container = input;
}

string wc::lex::wcLexInputStream::operator[](int charIndex)
{
	//get the correct line and column
	wcLineColumnIndex lcIndex(*this, charIndex);

	return container[lcIndex.line].substr(lcIndex.column, 1);
}

bool wc::lex::wcLexInputStream::operator==(const wcLexInputStream& otherStream) const
{
	if (otherStream.container != container)
		return false;
	return true;
}

bool wc::lex::wcLexInputStream::operator!=(const wcLexInputStream& otherStream) const
{
	if (otherStream.container != container)
		return true;
	return false;
}

wcLexInputStream wc::lex::wcLexInputStream::operator=(wcLexInputStream &otherStream)
{
	this->container = otherStream.container;
	return *this;
}

string wc::lex::wcLexInputStream::next(wcLexInputStreamIndex &index)
{
	return get(++index);
}

string wc::lex::wcLexInputStream::getLine(int line)
{
	return container[line];
}

string wc::lex::wcLexInputStream::getLine(wcLexInputStreamIndex &index)
{
	return container[index.line];
}

string wc::lex::wcLexInputStream::get(wcLexInputStreamIndex &index)
{
	if (!index.isValid())
		return 0;

	return container[index.line].substr(index.column, 1);
}

string wc::lex::wcLexInputStream::get(int _line, int _column)
{
	wcLineColumnIndex lcIndex(*this, _line, _column);

	if (lcIndex.isValid())
		return container[_line].substr(_column, 1);
	else
		return 0;
}

//get the amount of characters in stream
const unsigned int wc::lex::wcLexInputStream::size()
{
	unsigned int sizeAccumulator = 0;
	for (int t = 0; t < lines(); ++t)
		sizeAccumulator += container[t].size();

	return sizeAccumulator;
}

//get the amount of characters in a given line 
const unsigned int wc::lex::wcLexInputStream::size(unsigned int lineNumber)
{
	return container[lineNumber].size();
}

//get the amount of lines in the input
const unsigned int wc::lex::wcLexInputStream::lines()
{
	return container.size();
}

//do we have a source loaded, and are there still characters to read
bool wc::lex::wcLexInputStreamIndex::isValid()
{
	if (source.size() && line > -1 && column > -1 && index > -1 && (column < source.size(line)))
		return true;
	return false;
}

bool wc::lex::wcToken::operator==(const wcToken& token) const
{
	//dont compare line/column
	return (this->type == token.type && this->data == token.data);
}

string wc::lex::wcToken::operator[](unsigned int index)
{
	return data.substr(index, 1);
}

wcToken wc::lex::wcToken::operator=(string input)
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

wc::lex::wcToken::wcToken()
{
}

wc::lex::wcToken::wcToken(wcTokenType _type, string _data, int _line, int _column)
{
	type = _type;
	data = _data;
	line = _line;
	column = _column;
}

bool wc::lex::wcToken::operator!=(const wcToken& token) const
{
	return !(this->type == token.type && this->data == token.data);
}

wcTokenStream wc::lex::wcLexer::lex(wcLexInputStream& stream)
{
	wcLexInputStreamIndex lexIndex(stream);
	wcTokenStream out;

	//loop through characters making tokens until end of stream
	while ((lexIndex).isValid())
	{
		switch (deriver.derive(stream.get(lexIndex)))
		{
			//string literal
		case tt_dquote:
			if ((out += lex_stringLiteral(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

			//integer literals
		case tt_intlit:
			if ((out += lex_intOrFloatLiteral(lexIndex)).isError())
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
					return wcTokenStream(out.error);
			break;

			//comments
		case tt_div:
			if ((out += lex_comment(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

			//possible delimiter or identifier etc
		default:
			if ((out += lex_default(lexIndex)).isError())
				return wcTokenStream(out.error);
		}
		lexIndex++;
	}

	return out;
}

wcTokenStream wc::lex::wcLexer::lex_stringLiteral(wcLexInputStreamIndex & index)
{
	wcToken token(tt_strlit, "", index.line, index.column);

	//make sure we have an opening double quote
	if (deriver.derive(index.get()) != tt_dquote)
		return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Expected '\"'", token.line, token.column));
	++index;

	wcTokenStream out;
	string dataBuffer = "";
	//loop through until we reach the closing quote, or end of stream
	while (index.isValid())
		switch (deriver.derive(index.get()))
		{
		case tt_dquote:
			token.data = dataBuffer;
			return out += token;

		default:
			dataBuffer += index.next();
			break;
		}

	//end of stream before end of string, error
	return wcTokenStream(wcError(ec_lex_eos_strlit, token));
}

wcTokenStream wc::lex::wcLexer::lex_commentMultiLine(wcLexInputStreamIndex & index)
{
	//skip past tt_mult
	index++;

	//go through until */ or eos
	while (index.isValid())
		switch (deriver.derive(index.get()))
		{
		case tt_mult:
			if (deriver.derive(index.next()) == tt_div)
			{
				index.next();
				return wcTokenStream();	//end of comment found
			}

		default:
			index++;
		}

	//end of stream
	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_comment(wcLexInputStreamIndex & index)
{
	wcTokenStream out;

	//handle multi line elsewhere
	if (deriver.derive(index.peek()) == tt_mult)
		return lex_commentMultiLine(index);
	
	if (deriver.derive(index.peek()) != tt_div)
	{
		//just a single division symbol (/) not actually a comment (//)
		out += wcToken(tt_div, index.get(), index.line, index.column);
		index++;
		return out;
	}

	//go through until end of line or eos
	while (index.isValid())
		switch (deriver.derive(index.get()))
		{
		default:
			index.next();
			break;

		case tt_newline:
			index.next();
			return wcTokenStream();
		}

	//end of stream - ok
	return out;
}

wcTokenStream wc::lex::wcLexer::lex_2step(wcLexInputStreamIndex & index)
{
	//construct a second index so our increment wont leave the parameter invalid, still return error
	if (!(wcLexInputStreamIndex(index)++).isValid())
		return wcTokenStream(wcError(ec_lex_eos, index.get(), index.line, index.column));

	wcTokenType thisAndNext = deriver.derive(index.get() + index.peek());
	wcTokenStream out;
	if(	thisAndNext != tt_greaterequal && thisAndNext != tt_lessequal && thisAndNext != tt_notequal && thisAndNext != tt_equal &&
		thisAndNext != tt_incr && thisAndNext != tt_plusassign && thisAndNext != tt_decr && thisAndNext != tt_minusassign &&
		thisAndNext != tt_logand && thisAndNext != tt_logor &&	thisAndNext != tt_dcolon )	
	{
		out += wcToken(thisAndNext, index.get() + index.peek(), index.line, index.column);
		index++;;
	}
	else
		out += wcToken(thisAndNext, index.get(), index.line, index.column);

	index++;
	return out;
}

wcTokenStream wc::lex::wcLexer::lex_ws(wcLexInputStreamIndex & index)
{
	wcTokenType tt = deriver.derive(index.get());

	//make sure we have an opening whitespace
	if (tt != tt_ws && tt != tt_tab && tt != tt_newline)
		return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Expected '\"'", index.line, index.column));

	bool breakWhile = false;
	//clump all consecutive whitespace together
	while (index.isValid() && !breakWhile)
		switch (deriver.derive(index.get()))
		{
			CASE_TT_WS
			index++;
			break;

			//end of whitespace as soon as we reach another token or eos
		default:
			breakWhile = true;
		}

	return wcTokenStream();
}

wcTokenStream wc::lex::wcLexer::lex_intOrFloatLiteral(wcLexInputStreamIndex & index)
{
	int origCol = index.column;
	int origLine = index.line;
	//make sure we have an opening int
	if (deriver.derive(index.get()) != tt_intlit)
		return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Expected a numeric value (0-9)", origLine, origCol));

	bool firstHalf = true;
	bool breakWhile = false;
	string leftValue, rightValue;
	//get all the numbers til we hit not a number
	while (index.isValid() && !breakWhile)
		switch (deriver.derive(index.get()))
		{
		case tt_intlit:
			if (firstHalf)
				leftValue += index.get();
			else
				rightValue += index.get();
			index++;
			break;

		case tt_period:
			if (!leftValue.size() && firstHalf)
				return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Expected a numeric value (0-9)", index.line, index.column));
			else if (!firstHalf)
				return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Encountered a second period", index.line, index.column));

			//float encountered, continue lexing the second half after the decimal
			firstHalf = false;
			index++;
			break;

		default:
			breakWhile = true;
			break;
		}

	//check what we found
	if (firstHalf && leftValue.size() && (!rightValue.size()))
		return wcTokenStream() += wcToken(tt_intlit, leftValue, origLine, origCol);
	else if (!firstHalf && leftValue.size() && rightValue.size())
		return wcTokenStream() += wcToken(tt_fltlit, leftValue + "." + rightValue, origLine, origCol);
	else
		return wcTokenStream(wcError(ec_lex_somekindoferror, "Some kind of error lexing a float", origLine, origCol));
}

wcTokenStream wc::lex::wcLexer::lex_default(wcLexInputStreamIndex& index)
{
	wcToken token(tt_null, "", index.line, index.column);

	string dataBuffer = "";
	//build up the token until we hit a delimiter or eos
	while (index.isValid() && !deriver.isDelim(index.get()))
		dataBuffer += (index++).get();
	
	token.data = dataBuffer;
	token.type = deriver.derive(dataBuffer);

	//create a token for the delimiter unless it was end of stream
	wcTokenStream out;
	if (deriver.isDelim(index.get()))
	{
		if (token.type != tt_null)
			out.container.push_back(token);

		wcToken delim(deriver.derive(index.get()),string() += index.get(), index.line, index.column);
		out.container.push_back(delim);
	}
	else
	{
		dataBuffer += index.get();	//eos
		out.container.push_back(token);
	}

	++index;
	return out;
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
	if ((newIndex < 0) || newIndex >= source.size())
	{
		index = line = column = -1;
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
			column = newIndex - tempIndex;
			line = t;
			index = newIndex;
			return true;
		}

	//error somehow
	return false;
}

bool wc::lex::wcLineColumnIndex::updateFromLineColumn(int _line, int _column)
{
	//check for bad line/column
	if (_line >= source.size() || source.getLine(_line).size() >= _column)
	{
		line = column = index = -1;
		return false;
	}

	int indexAccumulator = 0;
	for (int t = 0; t <= _line - 1; ++t)
		indexAccumulator += source.size(t);
	index = indexAccumulator + _column;

	line = _line;
	column = _column;

	return true;
}

bool wc::lex::wcLineColumnIndex::isValid()
{
	if (!source.lines() || column < 0 || line < 0 || line >= source.lines())
		return false;
	else
		return true;
}

bool wc::lex::wcTokenStreamIndex::isValid()
{
	return false;
}

void wc::lex::wcTokenStreamIndex::reset()
{
}

wc::lex::wcTokenStream::wcTokenStream()
{
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

bool wc::lex::wcTokenStream::isError()
{
	return false;
}

wcToken wc::lex::wcTokenStream::get(wcTokenStreamIndex &)
{
	return wcToken();
}

wcToken wc::lex::wcTokenStream::peek(wcTokenStreamIndex &)
{
	return wcToken();
}

wcToken wc::lex::wcTokenStream::next(wcTokenStreamIndex &)
{
	return wcToken();
}

wcTokenStream wc::lex::wcTokenStream::operator+=(wcTokenStream stream)
{
	return *this + stream;
}

wc::lex::wcTokenStreamIndex::wcTokenStreamIndex(wcTokenStream &stream) : source(stream)
{

}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator-(int)
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(wcTokenStreamIndex)
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator-(wcTokenStreamIndex)
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator=(wcTokenStreamIndex)
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator++()
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator--()
{
	return *this;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(int)
{
	return *this;
}
