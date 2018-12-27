#include "error.h"
#include "lex.h"

using namespace wc;
using namespace wc::error;
using namespace wc::lex;
using namespace std;

wc::error::wcError::wcError(wcErrorCode p_ec)
{
	code = p_ec; text = errorStrings.find(p_ec)->second;
}

wc::error::wcError::wcError(wcErrorCode p_ec, string p_text)
{
	code = p_ec; text = errorStrings.find(p_ec)->second + " " + p_text;
}

wc::error::wcError::wcError(wcErrorCode p_ec, string p_text, int p_line, int p_col)
{
	code = p_ec; text = errorStrings.find(p_ec)->second + " " + p_text; line = p_line, col = p_col;
}

wc::error::wcError::wcError(wcErrorCode p_ec, wcToken p_token)
{
	code = p_ec; text = errorStrings.find(p_ec)->second + " " + p_token.data; line = p_token.line, col = p_token.column;
}

bool wc::error::wcError::operator==(const wcError& p_err) const
{
	return (this->code == p_err.code && this->text == p_err.text);
}

bool wc::error::wcError::operator==(const wcErrorCode& p_errCode) const
{
	return (this->code != p_errCode);
}

bool wc::error::wcError::operator!=(const wcError& p_err) const
{
	return (this->code != p_err.code && this->text != p_err.text);
}

bool wc::error::wcError::operator!=(const wcErrorCode& p_errCode) const
{
	return (this->code != p_errCode);
}