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
	wcTokenDefinition(tt_ws, " ", true, true),
	wcTokenDefinition(tt_newline, "\n", true),
	wcTokenDefinition(tt_tab, "\t", true),
	wcTokenDefinition(tt_period, ".", true, true),
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
	wcTokenDefinition(tt_assign, "=", true),
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
	for (unsigned int t = 0; t < definitions.size(); ++t)
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

const bool wc::lex::wcTokenDefinitionBank::exists(wcTokenType typeToCheck)
{
	for (unsigned int t = 0; t < definitions.size(); ++t)
		if (definitions[t].type == typeToCheck)
			return true;
	return false;
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(string identToCheck)
{
	for (unsigned int t = 0; t < definitions.size(); ++t)
		for (int y = 0; y < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == identToCheck)
				return definitions[t];
	return wcTokenDefinition();
}

const wcTokenDefinition wc::lex::wcTokenDefinitionBank::find(wcTokenType typeToCheck)
{
	for (unsigned int t = 0; t < definitions.size(); ++t)
		if (definitions[t].type == typeToCheck)
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
	for (unsigned int t = 0; t < definitions.size(); ++t)
		if (definitions[t].delimiter)
			delimiterTypes.push_back(definitions[t].type);
}

wc::lex::wcTokenDefinition::wcTokenDefinition() : type(tt_null), identifiers({}), delimiter(false), punctuation(false), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier) : type(_type), identifiers({ identifier }), delimiter(false), punctuation(false), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier, bool _isDelim) : type(_type), identifiers({ identifier }), delimiter(_isDelim), punctuation(false), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier, bool _isDelim, bool _isPunc) : type(_type), identifiers({ identifier }), delimiter(_isDelim), punctuation(_isPunc), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers) : type(_type), identifiers(_identifiers), delimiter(false), punctuation(false), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers, bool _isDelimiter) : type(_type), identifiers(_identifiers), delimiter(_isDelimiter), punctuation(false), precedence(0)
{

}

wc::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter, bool _isPunctuation) : type(_type), identifiers(_identifiers), delimiter(_isDelimiter), punctuation(_isPunctuation), precedence(0)
{

}

bool wc::lex::wcTokenDefinition::isNull() const
{
	if (type == tt_null && delimiter == false && identifiers.size() == 0)
		return true;
	else
		return false;
}

bool wc::lex::wcTokenDefinition::isSingleCharacterToken() const
{
	if (identifiers.size() == 0)
		return false;

	unsigned int largestIdentifierSize = 0;
	for (unsigned int t = 0; t < identifiers.size(); ++t)
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

bool wc::lex::wcTokenTypeDeriver::isPunctuation(char charToCheck)
{
	if (definitionsBank.exists(charToCheck))
		return definitionsBank.find(charToCheck).punctuation;
	return false;
}

bool wc::lex::wcTokenTypeDeriver::isPunctuation(wcTokenType typeToCheck)
{
	if (definitionsBank.exists(typeToCheck))
		return definitionsBank.find(typeToCheck).punctuation;
	return false;
}

bool wc::lex::wcTokenTypeDeriver::isPunctuation(std::string stringToCheck)
{
	if (definitionsBank.exists(stringToCheck))
		return definitionsBank.find(stringToCheck).punctuation;
	return false;
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

wc::lex::wcLexInputStreamIndex::wcLexInputStreamIndex(wcLexInputStream &stream, int _index) : source(stream)
{
	*this = wcLineColumnIndex(source, _index);
}

//postdecrement lvalue--
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator--(int)
{
	wcLexInputStreamIndex preDecrementValue = *this;
	*this = *this - 1;
	return preDecrementValue;
}

//postincrement lvalue++
wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator++(int)
{
	wcLexInputStreamIndex preIncrementValue = *this;
	*this = *this + 1;
	return preIncrementValue;
}

//preincrement ++lvalue
wcLexInputStreamIndex& wc::lex::wcLexInputStreamIndex::operator++()
{
	return *this = *this + 1;
}

//predecrement --lvalue
wcLexInputStreamIndex& wc::lex::wcLexInputStreamIndex::operator--()
{
	return *this = *this - 1;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(int subtraction)
{
	return wcLexInputStreamIndex(*this) = wcLineColumnIndex(source, index - subtraction);
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(int addition)
{
	return wcLexInputStreamIndex(*this) = wcLineColumnIndex(source, index + addition);
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator-(wcLexInputStreamIndex newIndex)
{
	return wcLexInputStreamIndex(*this) = *this + newIndex;
}

wcLexInputStreamIndex wc::lex::wcLexInputStreamIndex::operator+(wcLexInputStreamIndex newIndex)
{
	return wcLexInputStreamIndex(*this) = *this - newIndex;
}

wcLexInputStreamIndex& wc::lex::wcLexInputStreamIndex::operator=(wcLexInputStreamIndex input)
{
	index = input.index;
	column = input.column;
	line = input.line;
	source = source;
	return *this;
}

wcLexInputStreamIndex& wc::lex::wcLexInputStreamIndex::operator=(int newIndex)
{
	return *this = wcLineColumnIndex(source, newIndex);
}

wcLexInputStreamIndex& wc::lex::wcLexInputStreamIndex::operator=(wcLineColumnIndex newLineColumnIndex)
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
	if(isValid())
		return source.get(*this);
	return "";
}

unsigned int wc::lex::wcLexInputStreamIndex::size()
{
	return source.size();
}

//do we have a source loaded, and are there still characters to read
bool wc::lex::wcLexInputStreamIndex::isValid()
{
	if (source.size() && line > -1 && column > -1 && index > -1 && index < source.size() && (column <= source.size(line)))
		return true;
	return false;
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

wcLexInputStream& wc::lex::wcLexInputStream::operator=(wcLexInputStream &otherStream)
{
	this->container = otherStream.container;
	return *this;
}

wcLexInputStream& wc::lex::wcLexInputStream::operator=(const char * input)
{
	this->container.push_back(input);
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
		return "";

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

	for (unsigned int t = 0; t < lines(); ++t)
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


bool wc::lex::wcToken::operator==(const wcToken& token) const
{
	//dont compare line/column
	return (this->type == token.type && this->data == token.data);
}

string wc::lex::wcToken::operator[](unsigned int index)
{
	return data.substr(index, 1);
}

wcToken& wc::lex::wcToken::operator=(string input)
{
	data = input;
	return *this;
}

wcToken& wc::lex::wcToken::operator+=(std::string input)
{
	data += input;
	return *this;
}

wcToken& wc::lex::wcToken::operator=(wcToken input)
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
		CASE_LEX_ALL_2_PARTERS
				if ((out += lex_2step(lexIndex)).isError())
					return wcTokenStream(out.error);
			break;

		//whitespace
		CASE_LEX_TT_WS
				if ((lex_ws(lexIndex)).isError())
					return wcTokenStream(out.error);
			break;

		//comments
		case tt_div:
			if ((lex_comment(lexIndex)).isError())
				return wcTokenStream(out.error);
			break;

		//possible delimiter or identifier etc
		default:
			if ((out += lex_default(lexIndex)).isError())
				return wcTokenStream(out.error);
		}
	}

	return out;
}

wcTokenStream wc::lex::wcLexer::lex_stringLiteral(wcLexInputStreamIndex & index)
{
	wcToken token(tt_strlit, "", index.line, index.column);

	//make sure we have an opening double quote
	if (deriver.derive(index.get()) != tt_dquote)
		return wcTokenStream(wcError(ec_lex_unexpectedtoken, token += "Expected '\"'"));
	++index;
	
	//loop through until we reach the closing quote, or end of stream
	string dataBuffer = "";
	while (index.isValid())
		switch (deriver.derive(index.get()))
		{
		case tt_dquote:
			index++;
			token.data = dataBuffer;
			return wcTokenStream() += token;

		default:
			//get current index string, but also increment the index
			dataBuffer += (index++).get();
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
			index++;
			break;

		case tt_newline:
			index++;
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
	if(!(	thisAndNext != tt_greaterequal && thisAndNext != tt_lessequal && thisAndNext != tt_notequal && thisAndNext != tt_equal &&
		thisAndNext != tt_incr && thisAndNext != tt_plusassign && thisAndNext != tt_decr && thisAndNext != tt_minusassign &&
		thisAndNext != tt_logand && thisAndNext != tt_logor &&	thisAndNext != tt_dcolon ))
	{
		out += wcToken(thisAndNext, index.get() + index.peek(), index.line, index.column);
		index++;;
	}
	else
		out += wcToken(deriver.derive(index.get()), index.get(), index.line, index.column);

	index++;
	return out;
}

wcTokenStream wc::lex::wcLexer::lex_ws(wcLexInputStreamIndex & index)
{
	//make sure we have an opening whitespace
	wcTokenType tt = deriver.derive(index.get());
	if (tt != tt_ws && tt != tt_tab && tt != tt_newline)
		return wcTokenStream(wcError(ec_lex_unexpectedtoken, "Expected '\"'", index.line, index.column));

	//clump all consecutive whitespace together
	while (index.isValid() && (deriver.derive(index.get()) == tt_ws ||
		deriver.derive(index.get()) == tt_tab || deriver.derive(index.get()) == tt_newline))
			index++;

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
				leftValue += (index++).get();
			else
				rightValue += (index++).get();
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

	//build up the token until we hit a delimiter or eos
	string dataBuffer = "";
	while (index.isValid() && !deriver.isDelim(index.get()))
		dataBuffer += (index++).get();
	
	token.data = dataBuffer;
	token.type = deriver.derive(dataBuffer);

	//create a token for the delimiter unless it was end of stream
	wcTokenStream out;
	if (index.isValid() && deriver.isDelim(index.get()))
	{
		if (token.type != tt_null && !deriver.isPunctuation(token.type))
			out += token;

		wcToken delim(deriver.derive(index.get()),string() += index.get(), index.line, index.column);
		if (delim.type != tt_null && !deriver.isPunctuation(delim.type))
			out += delim;
		index++;
	}
	else
		out += token += index.get();

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
	if ((newIndex < 0) || newIndex > source.size())
	{
		index = line = column = -1;
		return false;
	}

	//progress through the input until we reach our given index
	int tempIndex = 0, tempCol = 0;
	for (unsigned int t = 0; t < source.lines(); ++t)
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

wc::lex::wcTokenStream::wcTokenStream()
{

}

wc::lex::wcTokenStream::wcTokenStream(error::wcError _error)
{
	error = _error;
}

wcTokenStream wc::lex::wcTokenStream::operator+(wcTokenStream stream)
{
	wcTokenStream out(*this);

	for (unsigned int t = 0; t < stream.container.size(); ++t)
		out.container.push_back(stream.container[t]);
	
	if (stream.error.code)
		out.error = stream.error;

	return out;
}

wcTokenStream wc::lex::wcTokenStream::operator+(wcToken token)
{
	wcTokenStream out(*this);

	out.container.push_back(token);

	return out;
}

wcTokenStream& wc::lex::wcTokenStream::operator+=(wcToken token)
{
	return *this = *this + token;
}

wcTokenStream& wc::lex::wcTokenStream::operator+=(wcTokenStream stream)
{	
	return *this = *this + stream;
}

bool wc::lex::wcTokenStream::isError() const
{
	return error.code > 0;
}

wcToken wc::lex::wcTokenStream::get(wcTokenStreamIndex &index)
{
	if (index.isValid())
		return container.at(index.index);
	return wcToken();
}

wcToken wc::lex::wcTokenStream::peek(wcTokenStreamIndex &index)
{
	wcTokenStreamIndex tempIndex = index;
	return get(tempIndex++);
}

wcToken wc::lex::wcTokenStream::next(wcTokenStreamIndex &index)
{
	return get(index++);
}


wc::lex::wcTokenStreamIndex::wcTokenStreamIndex(wcTokenStream &stream) : source(stream)
{
	index = 0;
}

wc::lex::wcTokenStreamIndex::wcTokenStreamIndex(wcTokenStream &_source, int _index) : source(_source)
{
	index = _index;
}

bool wc::lex::wcTokenStreamIndex::isValid() const
{
	if(index < source.container.size() && source.container.size())
		return true;
	return false;
}

void wc::lex::wcTokenStreamIndex::reset()
{
	index = 0;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(wcTokenStreamIndex otherIndex)
{
	wcTokenStreamIndex output(*this);
	for (unsigned int t = 0; t < otherIndex.source.container.size(); ++t)
		output.source.container.push_back(otherIndex.source.container[t]);
	return output;
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator+(int addition)
{
	return wcTokenStreamIndex(source, index + addition);
}

wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator-(int subtraction)
{
	return wcTokenStreamIndex(source, index - subtraction);
}

wcTokenStreamIndex& wc::lex::wcTokenStreamIndex::operator=(wcTokenStreamIndex otherIndex)
{
	index = otherIndex.index;
	source = otherIndex.source;
	return *this;
}

//postincrement lvalue++
wcTokenStreamIndex& wc::lex::wcTokenStreamIndex::operator++()
{
	return *this = *this + 1;
}

//postdecrement lvalue--
wcTokenStreamIndex& wc::lex::wcTokenStreamIndex::operator--()
{
	return *this = *this - 1;
}

//preincrement ++lvalue
wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator++(int)
{
	wcTokenStreamIndex preIncrementValue = *this;
	*this = *this + 1;
	return preIncrementValue;
}

//predecrement --lvalue
wcTokenStreamIndex wc::lex::wcTokenStreamIndex::operator--(int)
{
	wcTokenStreamIndex preDecrementValue = *this;
	*this = *this + 1;
	return preDecrementValue;
}

