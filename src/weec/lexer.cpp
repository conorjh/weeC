#include "lexer.h"

using namespace std;
using namespace wc;
using namespace lex;

namespace wc 
{
	namespace lex 
	{

		//nextToken clarity methods
		wcToken* nextToken_lexWS(wcLexer* p_l, wcToken p_t);
		wcToken* nextToken_lexDiv(wcLexer* p_l, wcToken p_t);
		wcToken* nextToken_lexIntlit(wcLexer* p_l, wcToken p_t);
		wcToken* nextToken_lexStrlit(wcLexer* p_l, wcToken p_t);
		wcToken* nextToken_lexDualStep(wcLexer* p_l, wcToken p_t);

	}	//namespace wc::lex
}	//namespace wc

wc::lex::wcLexer::wcLexer()
{
	this->clear();
}

//reset the lexer to an initial state
void wc::lex::wcLexer::clear()
{
	data.tokens.clear();
	data.oldx = data.oldy = data.offset = 0;
	data.x = -1; data.y = 0;
	data.done = false;
}

//returns the most recently created token
wcToken* wc::lex::wcLexer::getToken()
{
	if (!data.tokens.size())
		return nullptr;
	return &data.tokens[data.tokens.size() - 1 - data.offset];
}

//for when we parse an ident, but also need parseFullExp to include it
//remove last token, set x/y to old values
void wc::lex::wcLexer::rewind()
{
	data.tokens.erase(data.tokens.end() - 1);
	data.x = data.oldx; data.y = data.oldy;
	data.offset = 0;
	data.done = false;
}

//lex through entire input
int wc::lex::lexInput(wcLexer* p_l)
{
	while (p_l->nextToken())
		true;
	return true;
}

//returns the second to last token
wcToken* wc::lex::wcLexer::prevToken()
{
	data.offset = +1;
	return getToken();
}

//just sanitizes the p_inputVector param, doesnt create a new container
//to hold the source code
bool wc::lex::wcLexer::loadSource(vector<string>* p_inputVector)
{
	string lineBuffer = "";
	vector<string> output;

	//fill a vector with the input string, accounting for /n and new lines	
	for (int v = 0; v < p_inputVector->size(); ++v)
		for (int t = 0; t < p_inputVector->at(v).size(); ++t)
			if (p_inputVector->at(v).substr(t, 1) == "\n")
			{
				output.push_back(lineBuffer);
				lineBuffer = "";
			}
			else
				lineBuffer += p_inputVector->at(v).substr(t,1);

	//push the last line
	output.push_back(lineBuffer);
	*p_inputVector = output;
	data.source = p_inputVector;

	return true;
}

bool wc::lex::wcLexer::loadSource(const char * p_inputString)
{
	return loadSource(new vector<string>({ p_inputString }));
}

bool wc::lex::wcLexer::finished()
{
	return data.done;
}

wcToken* wc::lex::nextToken_lexWS(wcLexer* p_l, wcToken p_t)
{
	//chomp up reocurring whitespace
	while (p_t.type == tt_ws || p_t.type == tt_tab || p_t.type == tt_newline)
	{
		if (!p_l->inc())
		{
			p_l->data.done = true;
			return nullptr;
		}
		p_t.type = getTokenType(p_l->getChar());
	}
	p_l->dec();
	return p_l->nextToken();
}

wcToken* wc::lex::nextToken_lexDiv(wcLexer* p_l, wcToken p_t)
{
	if (getTokenType(p_l->peekChar()) == tt_div)
	{
		p_l->inc();
		while (getTokenType(p_l->getChar()) != tt_newline)
		{
			if (!p_l->inc())
				break;
			p_t.type = getTokenType(p_l->getChar());
		}
		p_l->dec();
		return p_l->nextToken();
	}
	else if (getTokenType(p_l->peekChar()) == tt_mult)
	{
		//multiline comment /* */
		p_l->inc();
		while (p_l->inc())
		{
			p_t.type = getTokenType(p_l->getChar());
			if (p_t.type == tt_mult)
				if (getTokenType(p_l->peekChar()) == tt_div)
				{
					p_l->inc();
					return p_l->nextToken();
				}
		}
		//eos before end of comment error
		return nullptr;
	}
	else 
		p_l->data.tokens.push_back(p_t);
	
	return p_l->getToken();
}

wcToken* wc::lex::nextToken_lexIntlit(wcLexer* p_l, wcToken p_t)
{
	//check for early termination
	if (!p_l->inc())
	{
		p_l->data.tokens.push_back(p_t);
		return p_l->getToken(); //end of stream, just an single digit integer
	}
	
	//get all the numbers til we hit not a number
	while (getTokenType(p_l->getChar()) == tt_intlit)
	{
		p_t.data += p_l->getChar();
		if (!p_l->inc())
			break;	//end of stream
	}
	
	//if its a period, it could still be a float  (could also be end of stream)
	if (!p_l->data.done && getTokenType(p_l->getChar()) == tt_period)
	{
		p_t.data += ".";
		while (getTokenType(p_l->getChar()) == tt_intlit)
		{
			p_t.data += p_l->getChar();
			if (!p_l->inc())		
				break;	//end of stream
		}
		if (p_t.data.substr(p_t.data.size() - 1, 1) == ".")
			return nullptr;//error, no rvalue
		p_t.type = tt_fltlit;
	}
	
	//leave cursor on the last character, save token
	p_l->dec();	
	p_l->data.tokens.push_back(p_t);
	return p_l->getToken();
}

wcToken* wc::lex::nextToken_lexStrlit(wcLexer* p_l, wcToken p_t)
{
	p_t.data = "";	//data field is the string minus quotes
	p_t.type = tt_strlit;
	if (!p_l->inc())		
		return nullptr;	//eos after first dquote		

	while (getTokenType(p_l->getChar()) != tt_dquote)
	{
		//collect the inner contents of the quote
		p_t.data += p_l->getChar();
		if (!p_l->inc())		
			return nullptr;	//eos before last quote
	}

	p_l->data.tokens.push_back(p_t);
	return p_l->getToken();

}

//handles 2 character tokens that also have a valid token as their
//first character
wcToken* wc::lex::nextToken_lexDualStep(wcLexer* p_l, wcToken p_t)
{
	//dual step tokens		
	if (!((getTokenType((p_l->getChar() + p_l->peekChar())) != tt_greaterequal) && (getTokenType((p_l->getChar() + p_l->peekChar())) != tt_lessequal) &&
		(getTokenType((p_l->getChar() + p_l->peekChar())) != tt_notequal) && (getTokenType((p_l->getChar() + p_l->peekChar())) != tt_equal) &&
		(getTokenType((p_l->getChar() + p_l->peekChar())) != tt_incr) && (getTokenType((p_l->getChar() + p_l->peekChar())) != tt_plusassign) &&
		(getTokenType((p_l->getChar() + p_l->peekChar())) != tt_decr) && (getTokenType((p_l->getChar() + p_l->peekChar())) != tt_minusassign) &&
		(getTokenType((p_l->getChar() + p_l->peekChar())) != tt_logand) && (getTokenType((p_l->getChar() + p_l->peekChar())) != tt_logor) &&
		(getTokenType((p_l->getChar() + p_l->peekChar())) != tt_dcolon)))
		{
			p_l->inc();
			p_t.data += p_l->getChar();
			p_t.type = getTokenType(p_t.data);
		}

	p_l->data.tokens.push_back(p_t);
	return p_l->getToken();
}

//consumes characters from input until a valid token is found
//return value is a pointer to allow no token to be returned
wcToken* wc::lex::wcLexer::nextToken()
{
	//attempt to get the next character
	data.offset = 0;
	data.oldx = data.x;	data.oldy = data.y;
	if (!this->inc())
		return nullptr;	//end of stream

	wcToken tok;
	tok.x = data.x;	tok.y = data.y;
	tok.data = this->getChar();
	tok.type = getTokenType(tok.data);

	//Lexer State Machine
	switch (tok.type)
	{
		//drop whitespace
	case tt_null:
		return nullptr;

	case tt_ws:	case tt_tab:case tt_newline:
		return nextToken_lexWS(this, tok);

		//possible comment	DROP
	case tt_div:
		return nextToken_lexDiv(this, tok);

	case tt_intlit:	//int/float lit
		return nextToken_lexIntlit(this, tok);

	case tt_dquote:	//string literal
		return nextToken_lexStrlit(this, tok);

	case tt_greater: case tt_less: case tt_lognot:	
	case tt_assign:	case tt_plus: case tt_minus:	
	case tt_amper: case tt_pipe: case tt_colon:		
		return nextToken_lexDualStep(this, tok);


	default:	//otherwise, build token, check for delims	
		if (!isDelim(tok.data))
			while ((!isDelim(this->peekChar()) && (!data.done)))
			{
				//add char to data buffer until we hit a delim
				this->inc();	
				tok.data += this->getChar();
			}
		
		tok.type = getTokenType(tok.data);
		data.tokens.push_back(tok);
		return getToken();
	}
}

//increments our cursor to next character
//returns false if we did or have reached the end of input
bool wc::lex::wcLexer::inc()
{
	if (data.done)
		return false;

	if (data.x + 1 <= int(data.source->at(data.y).size()) - 1)
	{
		data.x++;
	}
	else
	{
		//newline
		data.x = 0, data.y++;
		if (data.y > data.source->size() - 1)
		{
			data.done = true;
			return false;	//eos
		}
	}
	return true;
}

//move cursor backwards one character
bool wc::lex::wcLexer::dec()
{
	if (data.x == 0)
		if (data.y > 0)
		{
			--data.y;
			data.x = data.source->at(data.y).size() - 1;
		}
		else
		{
			return false;
		}
	else
		--data.x;
	return true;
}

//returns the current character pointed to by the cursor
string wc::lex::wcLexer::getChar()
{
	return data.source->at(data.y).substr(data.x, 1);
}

//returns the next character without changing the cursor
string wc::lex::wcLexer::peekChar()
{
	int _y, _x;
	if (this->inc())
	{
		_x = data.x;
		_y = data.y;
		this->dec();
		return data.source->at(_y).substr(_x, 1);
	}
	else
	{
		return "";
	}
}

//returns true if a given string is a delimeter
//delimiters both define the boundary between two tokens in
//the input stream, and are valid tokens themselves
bool lex::isDelim(wcLexer*p_l, string p_s)
{
	auto result = find( p_l->data.delimTypes.begin(), p_l->data.delimTypes.end(), getTokenType(p_s));
	if (result == p_l->data.delimTypes.end())
		return false;	//no match, 
	return true;	//matches a known delimeter string
}

bool lex::isDelim(string p_s)
{
	return isDelim(&wcLexer(), p_s);
}

//returns the the token type of a given string
wcTokenType wc::lex::getTokenType(wcLexer* p_l, string p_s)
{
	if (p_s == "")	return tt_null;
	//std::unordered_multimap<const char *, wcTokenType>::const_iterator
	auto result = p_l->data.tokenStrings.find(p_s.c_str());
	if (result == p_l->data.tokenStrings.end())
		return tt_ident;	//no match, treat as identifier (variable name etc)
	return result->second;	//matches a known token string
}

wcTokenType wc::lex::getTokenType(string p_s)
{
	return getTokenType(&wcLexer(), p_s);
}

//lexes the input into tokens, and returns the tokens as strings
vector<string> wc::lex::tokenizeString(string p_in)
{
	//create lexer
	wcLexer l;
	l.data.source = new vector<string>();
	l.data.source->push_back(p_in);

	//lex	TODO: check for errors
	lexInput(&l);

	//fill a vector with all the tokens .data field
	vector<string> out;
	for (int t = 0; t < l.data.tokens.size(); ++t)
		out.push_back(l.data.tokens[t].data);
	delete l.data.source;
	return out;
}