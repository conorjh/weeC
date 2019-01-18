#include "error.h"
#include "lex.h"

using namespace wc;
using namespace wc::error;
using namespace wc::lex;
using namespace std;

wc::error::wcError::wcError(wcErrorCode _code)
{
	code = _code; text = errorStrings.find(_code)->second;
}

wc::error::wcError::wcError(wcErrorCode _code, string _text)
{
	code = _code; text = errorStrings.find(_code)->second + " " + _text;
}

wc::error::wcError::wcError(wcErrorCode _code, string _text, int _line, int _col)
{
	code = _code; text = errorStrings.find(_code)->second + " " + _text; line = _line, col = _col;
}

wc::error::wcError::wcError(wcErrorCode _code, wcToken _token)
{
	code = _code; text = errorStrings.find(_code)->second + " " + _token.data; line = _token.line, col = _token.column;
}

bool wc::error::wcError::operator>(const wcError & p_err) const
{
	return (code > p_err.code);
}

bool wc::error::wcError::operator>(const wcErrorCode & p_errCode) const
{
	return (code > p_errCode);
}

bool wc::error::wcError::operator<(const wcError & p_err) const
{
	return (code < p_err.code);
}

bool wc::error::wcError::operator<(const wcErrorCode & p_errCode) const
{
	return (code < p_errCode);
}

bool wc::error::wcError::operator>=(const wcError & p_err) const
{
	return (code >= p_err.code);
}

bool wc::error::wcError::operator>=(const wcErrorCode & p_errCode) const
{
	return (code >= p_errCode);
}

bool wc::error::wcError::operator<=(const wcError & p_err) const
{
	return (code <= p_err.code);
}

bool wc::error::wcError::operator<=(const wcErrorCode & p_errCode) const
{
	return (code <= p_errCode);
}

bool wc::error::wcError::operator==(const wcError& _err) const
{
	return (code == _err.code && text == _err.text);
}

bool wc::error::wcError::operator==(const wcErrorCode& _code) const
{
	return (code != _code);
}

bool wc::error::wcError::operator!=(const wcError& _err) const
{
	return (code != _err.code && text != _err.text);
}

bool wc::error::wcError::operator!=(const wcErrorCode& _code) const
{
	return (code != _code);
}

bool wc::error::wcError::operator!() const
{
	return !code;
}

wc::error::wcError::operator bool() const
{
	return code > 0;
}
