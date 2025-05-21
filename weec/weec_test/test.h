#ifndef WC_TEST_H
#define WC_TEST_H
#include <string>
#include <vector>
#include "test_wcIdentalyzer.h"
#include "test_wcIdentifier.h"
#include "test_wcFullIdentifier.h"
#include "test_wcFunctionIdentifier.h"
#include "test_wcStringTokenizer.h"
#include "test_wcParseSymbolTable.h"
#include "test_wcIfParser.h"
#include "test_wcDeclarationParser.h"
#include "test_wcBlockParser.h"
#include "test_wcTokenizer.h"

#include "test_c_compiler.h"

namespace weec
{
	namespace test
	{
		namespace lex
		{
			int Test_wcParseScope();
			int Test_wcParseScopes();

			int Test_wcParser_1(), Test_wcParser_2(), Test_wcParser_3(), Test_wcParser_4(), Test_wcParser_5(),
				Test_wcParser_6(), Test_wcParser_7(), Test_wcParser_8(), Test_wcParser_9(), Test_wcParser_10(),
				Test_wcParser_11(), Test_wcParser_12(), Test_wcParser_13(), Test_wcParser_14(), Test_wcParser_15(),
				Test_wcParser_16(), Test_wcParser_17(), Test_wcParser_18(), Test_wcParser_19(), Test_wcParser_20(),
				Test_wcParser_21(), Test_wcParser_22(), Test_wcParser_23(), Test_wcParser_24(), Test_wcParser_25(),
				Test_wcParser_26(), Test_wcParser_27(), Test_wcParser_28(), Test_wcParser_29(), Test_wcParser_30(),
				Test_wcParser_31(), Test_wcParser_32(), Test_wcParser_33(), Test_Fibonacci();

			int Test_wcExpressionParser_1(), Test_wcExpressionParser_2(), Test_wcExpressionParser_3(), Test_wcExpressionParser_4(), Test_wcExpressionParser_5(),
				Test_wcExpressionParser_6(), Test_wcExpressionParser_7(), Test_wcExpressionParser_8(), Test_wcExpressionParser_9(), Test_wcExpressionParser_10(),
				Test_wcExpressionParser_11(), Test_wcExpressionParser_12(), Test_wcExpressionParser_13(), Test_wcExpressionParser_14(), Test_wcExpressionParser_15(),
				Test_wcExpressionParser_16(), Test_wcExpressionParser_17(), Test_wcExpressionParser_18(), Test_wcExpressionParser_19(), Test_wcExpressionParser_20(),
				Test_wcExpressionParser_21(), Test_wcExpressionParser_22(), Test_wcExpressionParser_23(), Test_wcExpressionParser_24(), Test_wcExpressionParser_25(),
				Test_wcExpressionParser_26(), Test_wcExpressionParser_27(), Test_wcExpressionParser_28(), Test_wcExpressionParser_29(), Test_wcExpressionParser_30(),
				Test_wcExpressionParser_31(), Test_wcExpressionParser_32(), Test_wcExpressionParser_33(), Test_wcExpressionParser_34(), Test_wcExpressionParser_35(),
				Test_wcExpressionParser_36(), Test_wcExpressionParser_37(), Test_wcExpressionParser_38(), Test_wcExpressionParser_39(), Test_wcExpressionParser_40(),
				Test_wcExpressionParser_41(), Test_wcExpressionParser_42(), Test_wcExpressionParser_43(), Test_wcExpressionParser_44(), Test_wcExpressionParser_45(),
				Test_wcExpressionParser_46(), Test_wcExpressionParser_47(), Test_wcExpressionParser_48(), Test_wcExpressionParser_49(), Test_wcExpressionParser_50(),
				Test_wcExpressionParser_51(), Test_wcExpressionParser_52(), Test_wcExpressionParser_53(), Test_wcExpressionParser_54(), Test_wcExpressionParser_55(),
				Test_wcExpressionParser_56(), Test_wcExpressionParser_57(), Test_wcExpressionParser_58(), Test_wcExpressionParser_59(), Test_wcExpressionParser_60();


			std::vector<int(*)()> allLexTests =
			{
				/*
				Test_Tokenizer1  ,Test_Tokenizer2, Test_Tokenizer3, Test_Tokenizer4, Test_Tokenizer5,
				Test_Tokenizer6, Test_Tokenizer7, Test_Tokenizer8, Test_Tokenizer9, Test_Tokenizer10, Test_Tokenizer11, Test_Tokenizer12,
				*/
				//Test_StringTokenizer1

			};
			int Test_AllLex();

			std::vector<int(*)()> allParseTests =
			{
				//wcStringTokenizer
				//Test_StringTokenizer1,

				/*
				//wcTokenizer
				Test_Tokenizer1, Test_Tokenizer2, Test_Tokenizer3, Test_Tokenizer4, Test_Tokenizer5,
				Test_Tokenizer6, Test_Tokenizer7, Test_Tokenizer8, Test_Tokenizer9, Test_Tokenizer10, Test_Tokenizer11, Test_Tokenizer12,
				*/

				//wcIdentifier
				Test_wcIdentifier_blank_identifier,
				Test_wcIdentifier1_string_constructed,
				Test_wcIdentifier_string_constructed_with_namespace_in_string,
				Test_wcIdentifier_string_constructed_with_function_in_string,
				Test_wcIdentifier_string_constructed_with_member_in_string,
				Test_wcIdentifier_string_constructed_with_function_and_member_in_string1,
				Test_wcIdentifier_string_constructed_with_function_in_string2,
				Test_wcIdentifier_string_constructed_with_namespace_function_and_member_in_string1,
				Test_wcIdentifier_string_constructed_with_namespace_function_and_member_in_string2,
				Test_wcIdentifier_string_constructed_with_fully_qualified_function_in_string,
				Test_wcIdentifier_copy_constructed_with_fully_qualified_identifier_with_global_namespace_in_string,
				Test_wcIdentifier_copy_constructed_with_fully_qualified_function_identifier_with_global_namespace_in_string,

				//wcIdentalyzer
				Test_wcIdentalyzer_ContainsGlobal,
				Test_wcIdentalyzer_ContainsNamespace,
				Test_wcIdentalyzer_IsFunction,
				Test_wcIdentalyzer_IsQualified,
				Test_wcIdentalyzer_IsMember,
				Test_wcIdentalyzer_IsQualifiedWithGlobal,
				Test_wcIdentalyzer_GetIdentifierFromQualifiedIdentifier,
				Test_wcIdentalyzer_GetNamespaceFromQualifiedIdentifier,
				Test_wcIdentalyzer_GetParameterListIdentifierString,
				Test_wcIdentalyzer_StripArgumentsFromFunctionIdentifier,

				//wcFullIdentifier
				Test_wcFullIdentifier_default_constructor,
				Test_wcFullIdentifier_default_constructor,
				Test_wcFullIdentifier_string_constructor,
				Test_wcFullIdentifier_string_constructor_with_namespace,
				Test_wcFullIdentifier_string_constructor_with_function_and_namespace,
				Test_wcFullIdentifier_string_constructor_with_namespace_and_global,
				Test_wcFullIdentifier_string_constructor_with_function_namespace_and_global,
				Test_wcFullIdentifier_copy_constructor_with_function_namespace_and_global,
				Test_wcFullIdentifier_part_constructor_with_function_namespace_and_global,
				Test_wcFullIdentifier_part_constructor_with_function_parameters_namespace_and_global,
				Test_wcFullIdentifier_part_constructor_with_function_local,

				//wcFunctionIdentifier
				Test_wcFunctionIdentifier_default_constructor,
				Test_wcFunctionIdentifier_default_constructor,
				Test_wcFunctionIdentifier_string_constructor_no_parenthesis_in_identifier,
				Test_wcFunctionIdentifier_string_constructor_with_parameters,
				Test_wcFunctionIdentifier_string_constructor_with_parameters_and_namespace,

				//wcDeclarationParser
				Test_wcDeclarationParser_simple_declaration,
				Test_wcDeclarationParser_simple_declaration_and_assignment,
				Test_wcDeclarationParser_simple_function_declaration,
				Test_wcDeclarationParser_simple_function_declaration_with_body,
				Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body,
				Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body2,
				Test_wcDeclarationParser_simple_function_declaration_with_arguments_and_body3,
				

				//wcIfParser
				Test_wcIfParser_if_true_single_line,
				Test_wcIfParser_if_true_else_single_line,
				Test_wcIfParser_if_true_else_newlines_and_braces,
				Test_wcIfParser_if_true_else_newlines,

				//wcFunctionIdentifier
				Test_wcFunctionIdentifier_default_constructor,
				Test_wcFunctionIdentifier_default_constructor,
				Test_wcFunctionIdentifier_string_constructor_no_parenthesis_in_identifier,
				Test_wcFunctionIdentifier_string_constructor_with_parameters,
				Test_wcFunctionIdentifier_string_constructor_with_parameters_and_namespace,

				//wcParseSymbolTable
				Test_wcParseSymbolTable_default_constructor,
				Test_wcParseSymbolTable_add_symbol,

				Test_wcBlockParser1,


				Test_wcParseScope,

				Test_wcParseScopes,

				Test_wcBlockParser1,


				Test_wcParser_1, Test_wcParser_2, Test_wcParser_3, Test_wcParser_4, Test_wcParser_5,
				Test_wcParser_6, Test_wcParser_7, Test_wcParser_8, Test_wcParser_9, Test_wcParser_10,
				Test_wcParser_11, Test_wcParser_12, Test_wcParser_13, Test_wcParser_14, Test_wcParser_15,
				Test_wcParser_16, Test_wcParser_17, Test_wcParser_18, Test_wcParser_19, Test_wcParser_20,
				Test_wcParser_21, Test_wcParser_22, Test_wcParser_23, Test_wcParser_24, Test_wcParser_25,
				Test_wcParser_26, Test_wcParser_27, Test_wcParser_28, Test_wcParser_29, Test_wcParser_30, Test_wcParser_31,Test_wcParser_32,Test_wcParser_33,
				Test_Fibonacci,

				Test_wcExpressionParser_1, Test_wcExpressionParser_2, Test_wcExpressionParser_3, Test_wcExpressionParser_4, Test_wcExpressionParser_5,
				Test_wcExpressionParser_6, Test_wcExpressionParser_7, Test_wcExpressionParser_8, Test_wcExpressionParser_9, Test_wcExpressionParser_10,
				Test_wcExpressionParser_11, Test_wcExpressionParser_12, Test_wcExpressionParser_13, Test_wcExpressionParser_14, Test_wcExpressionParser_15,
				Test_wcExpressionParser_16, Test_wcExpressionParser_17, Test_wcExpressionParser_18, Test_wcExpressionParser_19, Test_wcExpressionParser_20,
				Test_wcExpressionParser_21, Test_wcExpressionParser_22, Test_wcExpressionParser_23, Test_wcExpressionParser_24, Test_wcExpressionParser_25,
				Test_wcExpressionParser_26, Test_wcExpressionParser_27, Test_wcExpressionParser_28, Test_wcExpressionParser_29, Test_wcExpressionParser_30,
				Test_wcExpressionParser_31, Test_wcExpressionParser_32, Test_wcExpressionParser_33, Test_wcExpressionParser_34, Test_wcExpressionParser_35,
				Test_wcExpressionParser_36, Test_wcExpressionParser_37, Test_wcExpressionParser_38, Test_wcExpressionParser_39, Test_wcExpressionParser_40,
				Test_wcExpressionParser_41, Test_wcExpressionParser_42, Test_wcExpressionParser_43, Test_wcExpressionParser_44, Test_wcExpressionParser_45,
				Test_wcExpressionParser_46, Test_wcExpressionParser_47, Test_wcExpressionParser_48, Test_wcExpressionParser_49, Test_wcExpressionParser_50
				, Test_wcExpressionParser_51

			};
			int Test_AllParse();

		}

	}
}

#endif