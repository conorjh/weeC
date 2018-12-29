#ifndef WCT_TESTS_H
#define WCT_TESTS_H
#include <vector>
#include "lex.h"

namespace wctest
{
	namespace lex
	{
		int l_wcLexInputStream_1(), l_wcLexInputStream_2(), l_wcLexInputStream_3();
		int l_wcLexInputStreamIndex_1(), l_wcLexInputStreamIndex_2(), l_wcLexInputStreamIndex_3();

		int l_basic_1(), l_basic_2(), l_basic_3(), l_basic_4(), l_basic_5(), l_basic_6(), l_basic_7();

		const std::vector<int(*)()> allLexTests =
		{
			&l_wcLexInputStream_1,&l_wcLexInputStream_2,&l_wcLexInputStream_3,
			&l_wcLexInputStreamIndex_1, &l_wcLexInputStreamIndex_2, &l_wcLexInputStreamIndex_3,

			&l_basic_1, &l_basic_2, &l_basic_3, &l_basic_4 , &l_basic_5 , &l_basic_6, &l_basic_7
		};
	}

	namespace parse
	{
		
	}

	namespace gen
	{
	}

	namespace exp
	{
	}

	namespace vm
	{
	}

	namespace api
	{
	}

	int test_runall();
}

#endif
