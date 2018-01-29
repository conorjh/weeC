#ifndef WCT_TESTS_H
#define WCT_TESTS_H
#include <vector>
#include "lex.h"

namespace wctest
{
	namespace lex
	{
		int l_basic_1();
		int l_basic_2();
		int l_basic_3();
		int l_basic_4();
		int l_basic_5();
		int l_basic_6();
		int l_basic_7();

		const std::vector<int(*)()> allLexTests = { &l_basic_1, &l_basic_2, &l_basic_3, &l_basic_4 , &l_basic_5 , &l_basic_6, &l_basic_7 };
	}

	namespace parse
	{
		int p_basic_1();
		int p_basic_2();
		int p_basic_3();
		int p_basic_4();
		int p_basic_5();
		int p_basic_6();

		const std::vector<int(*)()> allParseTests = { &p_basic_1,&p_basic_2,&p_basic_3,&p_basic_4 ,&p_basic_5, &p_basic_6 };
	}

	namespace gen
	{
		int g_basic_1();

		const std::vector<int(*)()> allGenTests = { &g_basic_1 };
	}

	namespace vm
	{
		int vm_basic_1();
		int vm_basic_2();
		int vm_basic_3();
		int vm_basic_4();
		int vm_basic_5();
		int vm_basic_6();
		int vm_basic_7();

		const std::vector<int(*)()> allVMTests = { &vm_basic_1 ,&vm_basic_2 ,&vm_basic_3,&vm_basic_4,&vm_basic_5,&vm_basic_6,&vm_basic_7 };
	}

	namespace api
	{
		int api_basic_1();
	}

	int test_runall();
}

#endif
