#ifndef WC_TEST_WCEXPRESSIONPARSER_H
#define WC_TEST_WCEXPRESSIONPARSER_H
#include <string>
#include "test_listings.h"
#include "test_utils.h"
#include "lex.h"
#include "parse.h"

namespace weec
{
	namespace test
	{
		inline int Test_wcExpressionParser_1()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression1, 5);
		}

		inline int Test_wcExpressionParser_60()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_59()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_58()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_57()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_56()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_55()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_54()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_53()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_52()
		{
			return 0;
		}

		inline int Test_wcExpressionParser_51()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression51, 0);
		}

		inline int Test_wcExpressionParser_2()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression2, 6);
		}

		inline int Test_wcExpressionParser_3()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression3, 89);
		}

		inline int Test_wcExpressionParser_4()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression4, 4);
		}

		inline int Test_wcExpressionParser_5()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression5, 133);
		}

		inline int Test_wcExpressionParser_6()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression6, 87);
		}

		inline int Test_wcExpressionParser_7()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression7, 48);
		}

		inline int Test_wcExpressionParser_8()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression8, 5);
		}

		inline int Test_wcExpressionParser_9()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression9, (float)2.5);
		}

		inline int Test_wcExpressionParser_10()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression10, (float)(4 * 2.5 + 8.5 + 1.5 / 3.0));
		}

		inline int Test_wcExpressionParser_11()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression11, (float)(5.0005 + 0.0095));
		}

		inline int Test_wcExpressionParser_12()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression12, 69);
		}

		inline int Test_wcExpressionParser_13()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression13, -5);
		}

		inline int Test_wcExpressionParser_14()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression14, 35);
		}

		inline int Test_wcExpressionParser_15()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression15, 2);
		}
		inline int Test_wcExpressionParser_16()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression16, 10);
		}
		inline int Test_wcExpressionParser_17()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression17, 6);
		}
		inline int Test_wcExpressionParser_18()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression18, 10);
		}
		inline int Test_wcExpressionParser_19()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression19, 6);
		}

		inline int Test_wcExpressionParser_20()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression20, (2 * 3 - 4 * 5 + 6 / 3));
		}

		inline int Test_wcExpressionParser_21()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression21, (2 * 3 * 4 / 8 - 5 / 2 * 4 + 6 + 0 / 3));
		}

		inline int Test_wcExpressionParser_22()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression22, (10 / 4));
		}

		inline int Test_wcExpressionParser_23()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression23, (5 / 3));
		}

		inline int Test_wcExpressionParser_24()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression24, (3 + 8 / 5 - 1 - 2 * 5));
		}

		inline int Test_wcExpressionParser_25()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression25, 0);
		}
		inline int Test_wcExpressionParser_26()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression26, 0);
		}
		inline int Test_wcExpressionParser_27()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression27, 0);
		}
		inline int Test_wcExpressionParser_28()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression28, (5 + +6));
		}
		inline int Test_wcExpressionParser_29()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression29, (-5 + 2));
		}

		inline int Test_wcExpressionParser_30()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression30, 5 / 1);
		}

		inline int Test_wcExpressionParser_31()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression31, 2 - 1 + 14 / 1 + 7);
		}

		inline int Test_wcExpressionParser_32()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression32, (2));
		}

		inline int Test_wcExpressionParser_33()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression33, (5 + 2 * 3 - 1 + 7 * 8));
		}

		inline int Test_wcExpressionParser_34()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression34, (67 + 2 * 3 - 67 + 2 / 1 - 7));
		}

		inline int Test_wcExpressionParser_35()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression35, ((2) + (17 * 2 - 30) * (5) + 2 - (8 / 2) * 4));
		}
		inline int Test_wcExpressionParser_36()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression36, ((5 * 7 / 5) + (23) - 5 * (98 - 4) / (6 * 7 - 42)));
		}
		inline int Test_wcExpressionParser_37()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression37, (((((5))))));
		}
		inline int Test_wcExpressionParser_38()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression38, ((((2)) + 4)) * ((5)));
		}
		inline int Test_wcExpressionParser_39()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression39, 0);
		}
		inline int Test_wcExpressionParser_40()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression40, 0);
		}
		inline int Test_wcExpressionParser_41()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression41, 0);
		}
		inline int Test_wcExpressionParser_42()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression42, 0);
		}
		inline int Test_wcExpressionParser_43()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression43, (1 == +(-1) == 1.00));
		}
		inline int Test_wcExpressionParser_44()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression44, +(-1) * 10);
		}
		inline int Test_wcExpressionParser_45()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression45, !2);
		}
		inline int Test_wcExpressionParser_46()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression46, 22 && (11 + 11));
		}
		inline int Test_wcExpressionParser_47()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression47, 0 || (33 + 33));
		}
		inline int Test_wcExpressionParser_48()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression48, +(-2) * 10);
		}
		inline int Test_wcExpressionParser_49()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression49, !!!2);
		}
		inline int Test_wcExpressionParser_50()
		{
			return utils::Test_ExpressionParserTemplate(listing::list_expression50, !!!!(123));
		}



	}
}

#endif