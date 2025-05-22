#include "lex.h"
#include <ctype.h>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

using namespace weec;
using namespace weec::lex;
using weec::lex::wcToken;

//used by default constructors
wcStringToken weec::lex::misc::NullToken;

weec::lex::wcStringToken::wcStringToken()
	: Column(0), Line(0), Data(), Type(wcStringTokenType::Null)
{
}

weec::lex::wcStringToken::wcStringToken(std::string& Data, wcStringTokenType Type, int Line, int Column)
	: Column(Column), Line(Line), Data(Data), Type(Type)
{
}

weec::lex::wcStringToken::wcStringToken(std::string Data, wcStringTokenType Type, int Line, int Column)
	: Column(Column), Line(Line), Data(Data), Type(Type)
{
}

wcStringToken& weec::lex::wcStringToken::operator=(wcStringToken input)
{
	this->Column = input.Column;
	this->Data = input.Data;
	this->Line = input.Line;
	this->Type = input.Type;
	return *this;
}

bool weec::lex::wcStringToken::operator==(const wcStringToken& Other)
{
	if (Other.Data == Data && Other.Type == Type)
		return true;
	return false;
}

bool weec::lex::wcStringToken::operator==(wcStringToken& Other)
{
	if (Other.Data == Data && Other.Type == Type)
		return true;
	return false;
}

weec::lex::wcToken::wcToken() : StringToken(misc::NullToken)
{
	Type = wcTokenType::NullToken;
}

weec::lex::wcToken::wcToken(const wcStringToken& _string)
{
	Type = wcTokenType::NullToken;  //todo get type from _string.Type;
	StringToken = _string;
}

bool weec::lex::wcToken::IsOperator() const
{
	switch (Type)
	{
	case wcTokenType::AssignOperator:
	case wcTokenType::DecrementOperator:
	case wcTokenType::DivAssignOperator:
	case wcTokenType::DivideOperator:
	case wcTokenType::EqualOperator:
	case wcTokenType::ExponentOperator:
	case wcTokenType::GreaterEqualOperator:
	case wcTokenType::GreaterOperator:
	case wcTokenType::IncrementOperator:
	case wcTokenType::LessEqualOperator:
	case wcTokenType::LessOperator:
	case wcTokenType::LogAndOperator:
	case wcTokenType::LogOrOperator:
	case wcTokenType::MinusAssignOperator:
	case wcTokenType::MinusOperator:
	case wcTokenType::ModulusOperator:
	case wcTokenType::MultAssignOperator:
	case wcTokenType::MultiplyOperator:
	case wcTokenType::NotEqualOperator:
	case wcTokenType::PlusAssignOperator:
	case wcTokenType::PlusOperator:
	case wcTokenType::TernaryOperator:
		return true;
	default:
		return false;
	}
}

bool weec::lex::wcToken::IsBuiltinType() const
{
	switch (Type)
	{
	case wcTokenType::IntKeyword:
	case wcTokenType::FloatKeyword:
	case wcTokenType::CharKeyword:
	case wcTokenType::StringKeyword:
	case wcTokenType::UIntKeyword:
	case wcTokenType::VoidKeyword:
		return true;
	default:
		return false;
	}
}


weec::lex::wcStringTokenizer::wcStringTokenizer(std::string& const Source)
	: Feeder(Source), Index(0), Finished(false), TokenBuffer(), Line()
{
}

weec::lex::wcStringTokenizer::wcStringTokenizer(const wcStringTokenizer& otherCopy)
	: Feeder(otherCopy.Feeder), Index(0), Finished(false), TokenBuffer(otherCopy.TokenBuffer), Line(otherCopy.Line)
{
}

wcStringTokenizer& weec::lex::wcStringTokenizer::operator=(wcStringTokenizer& input)
{
	this->Feeder = input.Feeder;
	this->Index = input.Index;
	this->Line = input.Line;
	this->TokenBuffer = input.TokenBuffer;
	this->Finished = input.Finished;
	return *this;
}

wcStringToken weec::lex::wcStringTokenizer::GetStringToken() const
{
	return TokenBuffer;
}

bool weec::lex::wcStringTokenizer::Lookahead(wcStringToken& output)
{
	return Lookahead(output, 1);
}

bool weec::lex::wcStringTokenizer::Lookahead(wcStringToken& output, int lookaheadCount)
{
	//save state
	wcStringTokenizer state = *this;

	for (int t = 0; t < lookaheadCount; ++t)
		if (!IncIndex())
		{
			*this = state;
			return false;
		}

	output = this->TokenBuffer;

	//restore state
	*this = state;
	return true;
}

bool weec::lex::wcStringTokenizer::LookaheadAndMatch(wcStringToken& output, int lookaheadCount, char matchChar)
{
	if (!Lookahead(output, lookaheadCount))
		return false;
	return (output.Data.size() < 1 && output.Data.at(0) == matchChar);
}

bool weec::lex::wcStringTokenizer::LookaheadAndMatch(wcStringToken& output, int lookaheadCount, string matchString)
{
	if (!Lookahead(output, lookaheadCount))
		return false;
	return (output.Data == matchString);
}

bool weec::lex::wcStringTokenizer::LookaheadAndMatch(wcStringToken& output, int lookaheadCount, wcStringTokenType matchType)
{
	if (!Lookahead(output, lookaheadCount))
		return false;
	return (wcStringTokenTypeAlizer().Get(output.Data) == matchType);
}

bool weec::lex::wcStringTokenizer::NextStringToken()
{
	return NextStringToken(wcStringTokenType::Null);
}

bool weec::lex::wcStringTokenizer::IsFinished() const
{
	return Finished;
}

bool weec::lex::wcStringTokenizer::Match(wcStringTokenType _expectedType)
{
	if (_expectedType != wcStringTokenType::Null && TokenBuffer.Type != _expectedType)
		return false;
	return true;
}

bool weec::lex::wcStringTokenizer::Match(char _expectedChar)
{
	if (TokenBuffer.Data.size() && _expectedChar != '\0' 
		&& TokenBuffer.Data.at(TokenBuffer.Data.size() - 1) != _expectedChar)
		return false;
	return true;
}

bool weec::lex::wcStringTokenizer::Match(string _expectedString)
{
	if (!_expectedString.empty() && TokenBuffer.Data != _expectedString)
		return false;
	return true;
}

bool weec::lex::wcStringTokenizer::IncIndex()
{
	//feed in the next line if needed
	if (Index >= Line.size())
	{
		if (!Feeder.NextLine())
		{
			Finished = true;
			return false;
		}
		else
			Index = 0;
		Line = Feeder.GetLine();
	}

	TokenBuffer.Column = Index;
	TokenBuffer.Line = Feeder.GetLineNum();
	TokenBuffer.Data = "";

	wcStringTokenType thisType = wcStringTokenType::Null, lastType = wcStringTokenType::Null;
	while (Index < Line.size())
	{
		thisType = wcStringTokenTypeAlizer().Get(Line.at(Index));

		if (((lastType != wcStringTokenType::Null && lastType != thisType) 
			|| lastType == wcStringTokenType::Punctuation || lastType == wcStringTokenType::Special)
			&& !(lastType == wcStringTokenType::Alpha && Line.at(Index) == '_') )
			break;

		TokenBuffer.Data += Line.at(Index);
		TokenBuffer.Type = thisType;
		Index++;
		lastType = thisType;
	}

	return true;
}

bool weec::lex::wcStringTokenizer::NextStringToken(wcStringTokenType _expectedType)
{
	if (!IncIndex())
	{
		Finished = true;
		return false;
	}

	//match with an expected type
	return Match(_expectedType);
}

bool weec::lex::wcStringTokenizer::NextStringToken(char _expectedChar)
{
	if (!IncIndex())
	{
		Finished = true;
		return false;
	}

	//match with an expected type
	return Match(_expectedChar);
}

bool weec::lex::wcStringTokenizer::NextStringToken(string _expectedString)
{
	if (!IncIndex())
	{
		Finished = true;
		return false;
	}

	//match with an expected type
	return Match(_expectedString);
}

wcStringTokenType weec::lex::wcStringTokenTypeAlizer::Get(char test) const
{
	if (test == '\n')
		return wcStringTokenType::NewLine;
	else if (isspace(test))
		return wcStringTokenType::Whitespace;
	else if (isdigit(test))
		return wcStringTokenType::Number;
	else if (isalpha(test))
		return wcStringTokenType::Alpha;
	else if (ispunct(test))
		return wcStringTokenType::Punctuation;
	else
		return wcStringTokenType::Special;
}

wcStringTokenType weec::lex::wcStringTokenTypeAlizer::Get(string test) const
{
	if (test == "\n")
		return wcStringTokenType::NewLine;
	else if (isspace(test[0]))
		return wcStringTokenType::Whitespace;
	else if (isdigit(test[0]))
		return wcStringTokenType::Number;
	else if (isalpha(test[0]))
		return wcStringTokenType::Alpha;
	else if (ispunct(test[0]))
		return wcStringTokenType::Punctuation;
	else
		return wcStringTokenType::Special;
}

bool weec::lex::wcTokenTypeAlizer::IsValidIdent(std::string Ident)
{
	//get each part in between seperators (:: and .), and make sure that is a valid ident
	//each part inbetween seperators must be valid as an ident for the whole ident (seperators and all) to be valid
	unsigned int index = 0; string PartBuffer;
	while (index < Ident.size())
	{
		PartBuffer = "";

		//increment until we find a seperator
		while (index < Ident.size() && (Ident[index] != '.' && Ident[index] != ':'))
		{
			PartBuffer += Ident[index];
			index++;
		}

		//check this part is valid
		if (PartBuffer.size() == 0)
			return false;	//empty ident before seperator
		if (!IsPartValidIdent(PartBuffer))
			return false;

		//handle double colon seperators by reading ahead and consuming the second colon
		if (Ident[index] == ':' && index + 1 < Ident.size() && Ident[index + 1] == ':')
			index++;
		else if (Ident[index] == ':' && index + 1 < Ident.size() && Ident[index + 1] != ':')
			return false;	//incomplete seperator

		index++;
	}

	//check this final (or possibly only) part is valid
	if (PartBuffer.size() == 0)
		return false;	//empty ident before seperator
	if (!IsPartValidIdent(PartBuffer))
		return false;

	return true;
}

bool weec::lex::wcTokenTypeAlizer::IsIdentQualified(std::string Ident)
{
	if (!IsValidIdent(Ident))
		return false;

	return (Ident.find("::") != std::string::npos || Ident.find(".") != std::string::npos)
		? true
		: false;
}

bool  weec::lex::wcTokenTypeAlizer::IsPartValidIdent(std::string Ident)
{
	if (Ident.size() == 0)
		return false;	//cant be empty

	return (Ident[0] != '_' && !isalpha(Ident[0]))
		? false	//must start with an underscore or letter
		: true;

}

wcTokenType weec::lex::wcTokenTypeAlizer::Get(std::string _testString)
{
	if (definitionsBank.exists(_testString))
		return definitionsBank.find(_testString).Type;
	
	return IsValidIdent(_testString)
		? wcTokenType::Identifier
		: wcTokenType::NullToken;
}

wcToken& weec::lex::wcToken::operator=(const wcToken& _rvalue)
{
	StringToken = _rvalue.StringToken;
	Type = _rvalue.Type;
	return *this;
}

bool weec::lex::wcToken::operator==(const wcToken& Other)
{
	return (Other.StringToken == StringToken && Other.Type == Type)
		? true
		: false;
}

bool weec::lex::wcToken::operator==(wcToken& Other)
{
	if (Other.StringToken == StringToken 
		&& Other.Type == Type)
		return true;
	return false;
}

bool weec::lex::operator==(const wcToken& lhs, wcToken& rhs)
{
	return (lhs.StringToken == rhs.StringToken && lhs.Type == rhs.Type)
		? true
		: false;
}

bool weec::lex::operator==(wcToken& lhs, const wcToken& rhs)
{
	return (lhs.StringToken == rhs.StringToken && lhs.Type == rhs.Type)
		? true
		: false;
}

wcLineFeeder::wcLineFeeder(std::string&  Source)
	: Source(Source), Index(0), Line(0), Buffer()
{
}

wcLineFeeder::wcLineFeeder(const wcLineFeeder& _otherCopy)
	: Source(_otherCopy.Source), Index(_otherCopy.Index), Line(_otherCopy.Line), Buffer(_otherCopy.Buffer)
{
}

wcLineFeeder& weec::lex::wcLineFeeder::operator=(wcLineFeeder& Other)
{
	this->Source = Other.Source;
	this->Buffer = Other.Buffer;
	this->Line = Other.Line;
	this->Index = Other.Index;

	return *this;
}

std::string weec::lex::wcLineFeeder::GetLine()
{
	return Buffer;
}

bool weec::lex::wcLineFeeder::NextLine()
{
	if (!Source.size() || Index >= Source.size())
		return false;

	Buffer = "";
	while (Index < Source.size())
	{
		Buffer += Source.at(Index);
		if (Source.at(Index) == '\n')
		{
			Line++;
			Index++;
			return true;
		}
		Index++;
	}

	return true;
}

weec::lex::wcTokenizerError::wcTokenizerError()
{
	Code = wcTokenizerErrorCode::None;
}

weec::lex::wcTokenizerError::wcTokenizerError(wcTokenizerErrorCode _Code, wcStringToken _Data)
{
	Code = _Code;
	Data = _Data;
}

weec::lex::wcTokenizerError::wcTokenizerError(wcTokenizerErrorCode _Code, wcStringToken _Data, std::string _Msg)
{
	Code = _Code;
	Data = _Data;
	Msg = _Msg;
}

weec::lex::wcTokenizer::wcTokenizer(std::string& Source, bool _NextToken) 
	: stringTokenizer(Source)
{
	if (_NextToken)
		NextToken();
}

wcTokenizer& weec::lex::wcTokenizer::operator=(wcTokenizer& Other)
{
	Error = Other.Error;
	stringTokenizer = Other.stringTokenizer;
	lookaheadBuffer = Other.lookaheadBuffer;
	TokenBuffer = Other.TokenBuffer;
	tokenTypeAlizer = Other.tokenTypeAlizer;
	return *this;
}

wcToken weec::lex::wcTokenizer::GetToken() const
{
	return TokenBuffer;
}

bool weec::lex::wcTokenizer::NextToken(wcTokenType Type)
{
	//brute force lookahead, save the entire state, restore it if no match found
	auto OldState = *this;

	if (!NextToken())
	{
		//end of stream
		*this = OldState;
		return false;
	}

	if (GetToken().Type != Type)
	{
		Error = wcTokenizerError(wcTokenizerErrorCode::UnexpectedToken, stringTokenizer.GetStringToken());
		return false;
	}

	return true;
}

bool weec::lex::wcTokenizer::IsFinished() const
{
	return stringTokenizer.IsFinished();
}

bool weec::lex::wcTokenizer::IsErrored() const
{
	return (Error.Code != wcTokenizerErrorCode::None);
}

//returns false for unknown tokens, or if we reached end of input
bool weec::lex::wcTokenizer::NextToken()
{
	if (!stringTokenizer.NextStringToken())
		return false;	//end of input

	TokenBuffer.StringToken = stringTokenizer.GetStringToken();
	TokenBuffer.Type = tokenTypeAlizer.Get(TokenBuffer.StringToken.Data);

	switch (TokenBuffer.StringToken.Type)
	{
	case wcStringTokenType::Number:
		return NextToken_Number();

	case wcStringTokenType::Special:
		return NextToken_Special();

	case wcStringTokenType::Alpha:
		return NextToken_Alpha();

	case wcStringTokenType::Punctuation:
		if (TokenBuffer.Type == wcTokenType::DoubleQuote)
			return NextToken_StringLiteral();
		else if (TokenBuffer.Type == wcTokenType::SingleQuote)
			return NextToken_CharLiteral();
		else
			return NextToken_Punctuation();

		//skip over whitespace
	case wcStringTokenType::Whitespace:
	case wcStringTokenType::NewLine:
		return NextToken();

	default:
		Error = wcTokenizerError(wcTokenizerErrorCode::UnknownToken, stringTokenizer.GetStringToken());
		return false;	//unknown token
	}

	//shouldnt get here
	Error = wcTokenizerError(wcTokenizerErrorCode::FuckKnows, stringTokenizer.GetStringToken());
	return false;
}

bool weec::lex::wcTokenizer::NextToken_StringLiteral()
{
	string Buffer = "";

	while (stringTokenizer.NextStringToken())
		if (wcTokenTypeAlizer().Get(stringTokenizer.GetStringToken().Data) != wcTokenType::DoubleQuote)
			Buffer += stringTokenizer.GetStringToken().Data;
		else if (wcTokenTypeAlizer().Get(stringTokenizer.GetStringToken().Data) == wcTokenType::NewLine)
		{
			//error, newline in string literal, can only be on one line
			auto ErrorStringToken = TokenBuffer.StringToken;	
			ErrorStringToken.Data = Buffer;
			Error = wcTokenizerError(wcTokenizerErrorCode::NewLineInStringLiteral, ErrorStringToken);
			return false;
		}
		else
		{
			//string literal token initialised in NextToken(), finish it here
			TokenBuffer.StringToken.Data = Buffer;
			TokenBuffer.Type = wcTokenType::StringLiteral;
			return true;
		}

	//end of file reached before closing double quote
	auto ErrorStringToken = TokenBuffer.StringToken;
	ErrorStringToken.Data = Buffer;
	Error = wcTokenizerError(wcTokenizerErrorCode::UnclosedStringLiteral, ErrorStringToken);
	return false;
}

bool weec::lex::wcTokenizer::NextToken_CharLiteral()
{
	string Buffer = "";

	while (stringTokenizer.NextStringToken())
		if (wcTokenTypeAlizer().Get(stringTokenizer.GetStringToken().Data) != wcTokenType::SingleQuote)
			Buffer += stringTokenizer.GetStringToken().Data;
		else if (wcTokenTypeAlizer().Get(stringTokenizer.GetStringToken().Data) == wcTokenType::NewLine)
		{
			//error, newline encountered in char literal, can only be on one line
			auto ErrorStringToken = TokenBuffer.StringToken;
			ErrorStringToken.Data = Buffer;
			Error = wcTokenizerError(wcTokenizerErrorCode::NewLineInCharLiteral, ErrorStringToken);
			return false;
		}
		else
		{
			TokenBuffer.StringToken.Data = Buffer;
			TokenBuffer.Type = wcTokenType::CharLiteral;
			return true;
		}

	//end of file reached before closing single quote
	auto ErrorStringToken = TokenBuffer.StringToken;
	ErrorStringToken.Data = Buffer;
	Error = wcTokenizerError(wcTokenizerErrorCode::UnclosedCharLiteral, ErrorStringToken);
	return false;
}

bool weec::lex::wcTokenizer::NextToken_Number()
{
	using enum weec::lex::wcTokenType;

	wcStringToken buffer1;
	if (stringTokenizer.Lookahead(buffer1, 1) && buffer1.Data == ".")
		if (stringTokenizer.LookaheadAndMatch(buffer1, 2, wcStringTokenType::Number))
		{
			//float literal
			stringTokenizer.NextStringToken();	stringTokenizer.NextStringToken();

			TokenBuffer.StringToken = wcStringToken(TokenBuffer.StringToken.Data + "." + buffer1.Data, wcStringTokenType::FloatNumber, TokenBuffer.StringToken.Line, TokenBuffer.StringToken.Column);
			TokenBuffer.Type = FloatLiteral;
			return true;
		}

	//int literal
	TokenBuffer.Type = IntLiteral;
	return true;
}

bool weec::lex::wcTokenizer::NextToken_Special()
{
	return true;
}

bool weec::lex::wcTokenizer::NextToken_Alpha()
{
	return (TokenBuffer.Type == wcTokenType::Identifier)
		? NextToken_Ident()
		: true;
}

bool weec::lex::wcTokenizer::NextToken_Punctuation()
{
	if (!stringTokenizer.Lookahead(lookaheadBuffer))
		return true;

	//handle ::, >=, == etc
	auto possibleOperatorType = tokenTypeAlizer.Get(TokenBuffer.StringToken.Data + lookaheadBuffer.Data);

	if (possibleOperatorType == wcTokenType::Comment)
		return NextToken_SingleLineComment();
	else if (possibleOperatorType == wcTokenType::MultiLineCommentStart)
		return NextToken_MultiLineComment();
	else if (possibleOperatorType != wcTokenType::NullToken)
	{
		stringTokenizer.NextStringToken();
		TokenBuffer.StringToken.Data += lookaheadBuffer.Data;
		TokenBuffer.Type = possibleOperatorType;
	}
	return true;
}

bool weec::lex::wcTokenizer::NextToken_Ident()
{
	TokenBuffer.Type = wcTokenType::Identifier;

	bool LastPartWasASeperator = false; wcStringToken Buffer, ColonBuffer;
	while (stringTokenizer.Lookahead(Buffer, 1))
		switch (wcTokenTypeAlizer().Get(Buffer.Data))
		{
		case wcTokenType::Colon:
			if (stringTokenizer.Lookahead(ColonBuffer, 2))
				if (wcTokenTypeAlizer().Get(ColonBuffer.Data) == wcTokenType::Colon)
				{
					TokenBuffer.StringToken.Data += ColonBuffer.Data;
					stringTokenizer.NextStringToken();	//actualy part of a double colon, so fallthrough
				}
				else
					goto _wcTokenizer_NextToken_Ident_End_Of_Ident;
			[[fallthrough]];
		case wcTokenType::DoubleColon:
			[[fallthrough]];
		case wcTokenType::Period:
			if (LastPartWasASeperator)
			{
				auto ErrorStringToken = TokenBuffer.StringToken;
				ErrorStringToken.Data += Buffer.Data;
				Error = wcTokenizerError(wcTokenizerErrorCode::MalformedIdentifier, ErrorStringToken);
				return false;	//two seperators in a row, error
			}

			TokenBuffer.StringToken.Data += Buffer.Data;
			LastPartWasASeperator = true;
			stringTokenizer.NextStringToken();
			continue;

		case wcTokenType::IntLiteral:
		WC_SWITCHCASE_TOKENS_BUILTIN_TYPES
		case wcTokenType::Identifier:
			//if (!LastPartWasASeperator)
			//	return true;	//two idents in a row somehow, possible error?

			TokenBuffer.StringToken.Data += Buffer.Data;
			LastPartWasASeperator = false;
			stringTokenizer.NextStringToken();
			continue;

		case wcTokenType::Whitespace:
		default:
			return true;
		}

	//end of stream/ident
_wcTokenizer_NextToken_Ident_End_Of_Ident:
	if (LastPartWasASeperator)
	{
		auto ErrorStringToken = TokenBuffer.StringToken;
		ErrorStringToken.Data += Buffer.Data;
		Error = wcTokenizerError(wcTokenizerErrorCode::MalformedIdentifier, ErrorStringToken);
		return false;	//incomplete ident
	}
	else
		return true;	//end of stream, but valid ident parsed

}

bool weec::lex::wcTokenizer::NextToken_SingleLineComment()
{
	while (stringTokenizer.GetStringToken().Type != wcStringTokenType::NewLine)
		if (!stringTokenizer.NextStringToken())
			return false;
	return NextToken();		//skip over comments
}

bool weec::lex::wcTokenizer::NextToken_MultiLineComment()
{
	stringTokenizer.NextStringToken();

	while (stringTokenizer.NextStringToken())
		if (tokenTypeAlizer.Get(stringTokenizer.GetStringToken().Data) == wcTokenType::MultiplyOperator)
			if (stringTokenizer.Lookahead(lookaheadBuffer) && tokenTypeAlizer.Get(lookaheadBuffer.Data) == wcTokenType::DivideOperator)
			{
				stringTokenizer.NextStringToken();
				return NextToken();		//skip over comments
			}

	//error, never found a closing comment token
	return false;
}


weec::lex::wcTokenDefinition::wcTokenDefinition() 
	: Type(wcTokenType::NullToken), identifiers({}), delimiter(false), punctuation(false), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier) 
	: Type(_type), identifiers({ identifier }), delimiter(false), punctuation(false), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier, bool _isDelim) 
	: Type(_type), identifiers({ identifier }), delimiter(_isDelim), punctuation(false), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, string identifier, bool _isDelim, bool _isPunc) 
	: Type(_type), identifiers({ identifier }), delimiter(_isDelim), punctuation(_isPunc), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers) 
	: Type(_type), identifiers(_identifiers), delimiter(false), punctuation(false), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, vector<string> _identifiers, bool _isDelimiter) 
	: Type(_type), identifiers(_identifiers), delimiter(_isDelimiter), punctuation(false), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(wcTokenType _type, std::vector<std::string> _identifiers, bool _isDelimiter, bool _isPunctuation) 
	: Type(_type), identifiers(_identifiers), delimiter(_isDelimiter), punctuation(_isPunctuation), precedence(0)
{
}

weec::lex::wcTokenDefinition::wcTokenDefinition(const wcTokenDefinition& input) 
	: Type(input.Type), identifiers(input.identifiers), delimiter(input.delimiter), punctuation(input.punctuation), precedence(input.precedence)
{
}


bool weec::lex::wcTokenDefinition::isNull() const
{
	return (Type == wcTokenType::NullToken && !delimiter && !identifiers.size())
		? true
		: false;
}

bool weec::lex::wcTokenDefinition::isSingleCharacterToken() const
{
	if (identifiers.size() == 0)
		return false;

	for (unsigned int t = 0; t < identifiers.size(); ++t)
		if (identifiers[t].size() > 1)
			return false;

	return true;
}


weec::lex::wcTokenDefinitionsBank::wcTokenDefinitionsBank()
{
	cache = nullptr;
	populateDelimiterTypes();
}

weec::lex::wcTokenDefinitionsBank::wcTokenDefinitionsBank(const std::vector<wcTokenDefinition>&)
{
	cache = nullptr;
	populateDelimiterTypes();
}

bool weec::lex::wcTokenDefinitionsBank::exists(const string& identToCheck) 
{
	for (unsigned int t = 0; t < WC_VAR_DEFINITIONCOUNT; ++t)
		for (int y = 0; y < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == string(identToCheck))
			{
				cache = &definitions[t];
				return true;
			}
	return false;
}

bool weec::lex::wcTokenDefinitionsBank::exists(const char& identToCheck) 
{
	stringstream ss;
	ss << identToCheck;
	return exists(ss.str());
}

bool weec::lex::wcTokenDefinitionsBank::exists(const wcTokenType& typeToCheck) 
{
	for (unsigned int t = 0; t < WC_VAR_DEFINITIONCOUNT; ++t)
		if (definitions[t].Type == typeToCheck)
		{
			cache = &definitions[t];
			return true;
		}
	return false;
}
                                                     
const wcTokenDefinition weec::lex::wcTokenDefinitionsBank::find(const string& identToCheck) const
{
	//check cache
	if (cache != nullptr)
		for (int y = 0; y < cache->identifiers.size(); ++y)
			if (cache->identifiers[y] == identToCheck)
				return *cache;

	for (unsigned int t = 0; t < WC_VAR_DEFINITIONCOUNT; ++t)
		for (unsigned int y = 0; y < definitions[t].identifiers.size(); ++y)
			if (definitions[t].identifiers[y] == identToCheck)
				return definitions[t];
	return wcTokenDefinition();
}

const wcTokenDefinition weec::lex::wcTokenDefinitionsBank::find(const wcTokenType& typeToCheck) const
{
	//check cache
	if (cache != nullptr)
		if (cache->Type == typeToCheck)
			return *cache;

	for (unsigned int t = 0; t < WC_VAR_DEFINITIONCOUNT; ++t)
		if (definitions[t].Type == typeToCheck)
			return definitions[t];
	return wcTokenDefinition();
}

const wcTokenDefinition weec::lex::wcTokenDefinitionsBank::find(const char& identToCheck) const
{
	stringstream ss;
	ss << identToCheck;
	return find(ss.str());
}

void weec::lex::wcTokenDefinitionsBank::populateDelimiterTypes()
{
	for (unsigned int t = 0; t < WC_VAR_DEFINITIONCOUNT; ++t)
		if (definitions[t].delimiter)
			delimiterTypes.push_back(definitions[t].Type);
}

std::string weec::lex::wcTokenTypeToString(wcTokenType Type)
{
	switch (Type)
	{
	case wcTokenType::Amper:
		return "wcTokenType::Amper";
	case wcTokenType::AssignOperator:
		return "wcTokenType::AssignOperator";
	default:
		return "";
	}
}

