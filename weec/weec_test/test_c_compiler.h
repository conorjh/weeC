#ifndef WC_TEST_C_COMPILER_H
#define WC_TEST_C_COMPILER_H
#include <string>
#include <vector>
#include "parse.h"

namespace weec
{
	namespace test
	{
		namespace listing
		{
			std::string c_compiler_invalid_lex_at_sign = "/* The @ symbol doesn't appear in any C tokens,\nexcept inside string or character literals. */\nint main(void) {\n\treturn 0@1;\n}";
			std::string c_compiler_invalid_lex_backslash = "/* A single backslash is not a valid token. */\n\\";
			std::string c_compiler_invalid_lex_backtick = "/* A backtick is not a valid token. */\n`";
			std::string c_compiler_invalid_lex_invalid_identifier = "/* '1foo' is not a valid token, because identifier can't start with digits. */\nint main(void) {\n\treturn 1foo;\n}";
			std::string c_compiler_invalid_lex_invalid_identifier2 = "int main(void)\n{\n\treturn @b;\n}";

			std::string c_compiler_invalid_parse_end_before_expr = "int main(void) {\nreturn";
			std::string c_compiler_invalid_parse_extra_junk = "int main(void)\n{\n\treturn 2;\n}\n// A single identifier outside of a declaration isn't a valid top-level construct\nfoo";
			std::string c_compiler_invalid_parse_invalid_function_name = "/* A function name must be an identifier, not a constant */\n\tint 3 (void) {\n\treturn 0;\n}";
			std::string c_compiler_invalid_parse_keyword_wrong_case = "int main(void) {\n\tRETURN 0;\n}";
			std::string c_compiler_invalid_parse_missing_type = "// note: in older versions of C this would be valid\n// and return type would default to 'int'\n// GCC/Clang will compile it (with a warning)\n// for backwards compatibility\n\tmain(void) {\nreturn 0;\n}";
			std::string c_compiler_invalid_parse_misspelt_keyword = "int main(void) {\n\treturns 0;\n}";
			std::string c_compiler_invalid_parse_invalid_type = "int main(void) {\n\treturn int;\n}";
			std::string c_compiler_invalid_parse_no_semicolon = "int main (void) {\n\treturn 0\n}";
			std::string c_compiler_invalid_parse_space_in_keyword = "int main(void){\n\tretur n 0;\n}";
			std::string c_compiler_invalid_parse_switched_parens = "int main )( {\n\treturn 0;\n}";
			std::string c_compiler_invalid_parse_unclosed_brace = "int main(void) {\n\treturn 0;\n";
			std::string c_compiler_invalid_parse_unclosed_paren = "int main( {\n\treturn 0;\n}";

			std::string c_compiler_invalid_parse_double_operation = "int main( {\n\treturn 1 * / 2;\n}";
			std::string c_compiler_invalid_parse_imbalanced_paren = "int main( {\n\treturn 1 + (2;\n}";
			std::string c_compiler_invalid_parse_malformed_paren = "int main( {\n\treturn 2 (- 3);\n}";
			std::string c_compiler_invalid_parse_misplaced_semicolon = "int main( {\n\treturn 1 + (2;)\n}";
			std::string c_compiler_invalid_parse_missing_first_op = "int main( {\n\treturn /3;\n}";
			std::string c_compiler_invalid_parse_missing_open_paren = "int main( {\n\treturn 1 + 2);\n}";
			std::string c_compiler_invalid_parse_missing_second_op = "int main( {\n\treturn 1 + ;\n}";
			std::string c_compiler_invalid_parse_no_semicolon = "int main( {\n\treturn 2*2\n}";


			std::string c_compiler_valid_multi_digit = "int main(void) {\n\t// test case w/ multi-digit constant\n\treturn 100;\n}";
			std::string c_compiler_valid_newlines = "int\nmain\n(\nvoid\n)\n{\nreturn\n0\n;\n}";
			std::string c_compiler_valid_no_newlines = "int main(void){return 0;}";
			std::string c_compiler_valid_return0 = "int main(void) {\n\treturn 0;\n}";
			std::string c_compiler_valid_return2 = "int main(void) {\n\treturn 2;\n}";
			std::string c_compiler_valid_spaces = "   int   main    (  void)  {   return  0 ; }";
			std::string c_compiler_valid_tabs = "int	main	(	void)	{	return	0	;	}";

			std::string c_compiler_valid_parse_add = "int main( {\n\treturn 1 + 2;\n}";
			std::string c_compiler_valid_parse_associativity = "int main( {\n\treturn 1 - 2 - 3;\n}";
			std::string c_compiler_valid_parse_associativity_and_precedence = "int main( {\n\treturn 5 * 4 / 2 - 3 % (2 + 1);\n}";
			std::string c_compiler_valid_parse_associativity_2 = "int main( {\n\treturn 6 / 3 / 2;\n}";
			std::string c_compiler_valid_parse_associativity_3 = "int main( {\n\treturn (3 / 2 * 4) + (5 - 4 + 3);\n}";
			std::string c_compiler_valid_parse_div = "int main( {\n\treturn 4 / 2;\n}";
			std::string c_compiler_valid_parse_div_neg = "int main( {\n\treturn (-12) / 5;\n}";
			std::string c_compiler_valid_parse_mod = "int main( {\n\treturn 4 % 2;\n}";
			std::string c_compiler_valid_parse_mult = "int main( {\n\treturn 2 * 3;\n}";
			std::string c_compiler_valid_parse_parens = "int main( {\n\treturn 2 * (3 + 4);\n}";
			std::string c_compiler_valid_parse_precedence = "int main( {\n\treturn 2 + 3 * 4;\n}";
			std::string c_compiler_valid_parse_sub = "int main( {\n\treturn 1 - 2;\n}";
			std::string c_compiler_valid_parse_sub_neg = "int main( {\n\treturn 2- -1;\n}";
			std::string c_compiler_valid_parse_unop_add = "int main( {\n\treturn ~2 + 3;\n}";
			std::string c_compiler_valid_parse_unop_parens = "int main( {\n\treturn ~(1 + 1);\n}";
		}
		
	}

}
#endif
