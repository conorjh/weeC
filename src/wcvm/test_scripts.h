//test weec scripts
std::vector<string> testScripts = {
	//push an int to the stack
	{	"int a;" },
	//push 2 ints to the stack
	{	"int a;\n"
		"int b;\n" },
	//return a value
	{	"return 666*666;" },
	//simple arithmetic
	{	"24 + 72;" },
	{	"66 - 33;" },
	{	"99 * 99;" },
	{	"99 / 11;" },
	//simple greater than comparison 
	{	"1 > 0;"},
	{	"(1 > ( 2 + 2 ));" },
	{	"int a = 2;\n"
		"return a < 4;"},
	//equals comp
	{	"33 == 33;" },
	//not equal comp
	{	"99 != 100;" },
	{	"99 != 99;" },
	{	"99 != (3 * 33);"},
	//logical AND 
	{	"1 && 1;"	},
	{	"1 && 0;" },
	//expression with precedence
	{	"50 * ( 2 + 6 ) / 2;" },
	//instantiate a variable
	{	"int a = 42 * 42;" },
	//push an int to the stack and give it a value
	{	"int a = 2;" },
	//push an int, do a simple expression
	{	"int a = 44 + 1;"	},
	//push an int, do a simple expression
	{	"int a = 44 + 1 * 7;" },
	//push an int, do a simple expression
	{	"int a = 44 + 1 * 7 - 2 + 5;" },
	//push an int, do a simple expression
	{	"int a = 100 * 10 * ( 33 + ( 66 * 33 ) );" },
	//push 2 ints, do a simple expression with them
	{	"int a = 11;\n"
		"int b = 2;\n"
		"a = b + 100;\n"},
	//push 2 ints, do a simple expression with them, set one to the other
	{	"int a = 11;\n"
		"int b = 2;\n"
		"a = b + 100;\n"
		"b = a;" },
	//push an int with a value, then return it
	{	"int a = 2;\n"
		"return a;" },
	//assign to variables, then instantiate a third with them
	{	"int a = 2; \n"
		"int b = 3;\n"
		"int c = a + b;\n"
		"return b * c;"},
	//fallthrough assignment - doesnt work somehow
	{	"int a;\n"
		"int b;\n"
		"int c;\n"
		"return c = b = a = 2;\n"},
	
	{	"int a =2; \n"
		"func int f1()\n"
		"{\n"
		"return 22;\n"
		"}\n"
		"return a;" },

	{	"int a = 2; \n"
		"int b = 3;\n"
		"int c = a + b;\n"
		"func int main\n"
		"{\n"
		"return 0;\n"
		"}" },

	{	"int a = 2; \n"
		"int b = 3; \n"
		"int c = a + b * a + b * b;\n"
		"return c;" },

	{	"func int func1()\n"
		"{\n"
		"	int f1 = 2;\n"
		"	f1;\n"
		"}\n"
		"return func1;\n" },

	{ "int g = 1;\n"
	"func int func1\n"
	"{\n"
	"int f1 = g;\n"
	"return f1;\n"
	"}\n"
	"return func1;\n" },

	{ "int g = 1;\n"
	"func int func1\n"
	"{\n"
	"int f1 = g;\n"
	" f1;\n"
	"}\n"
	"func int func2\n"
	"{\n"
	"int f2 = g * 2;\n"
	"return f2;\n"
	"}\n"
	"return 1;\n" },

	{ "func int func1\n"
	"{\n"
	"int f1 = 2;\n"
	" f1;\n"
	"}\n"
	"int a=3;\n"
	" a + func1;\n" }
};