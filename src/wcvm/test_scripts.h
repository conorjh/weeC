//test weec scripts
std::vector<string> testScripts = {
	//push an int to the stack
	{	"int a;" },
	//push 2 ints to the stack
	{	"int a;\n"
		"int b;\n" },
	//push 2 ints, do some sum
	{	"int a = 11;\n"
		"int b = 2;\n"
		"return a = b + 100;\n" },
	//manipulate an array
	{	"int a[3];\n"
		"a[0] = 10;\n"
		"a[1] = 20;\n"
		"a[2] = 30;\n"
		"a[0] = a[1] + a[2];\n"
		"return a[0];" },
	//manipulate an array with ints also on the stack
	{	"int a;\n"
		"int b[3];\n"
		"int c;\n"
		"b[0] = 10;\n"
		"b[1] = 20;\n"
		"b[2] = 30;\n"
		"b[0] = b[1] + b[2];\n"
		"a = b[0] * 2;\n"
		"c = a * b[0];\n"
		"return c;" },
	//push 2 ints to the stack - tests the 
	//local variables of different dataSizes
	{	"int a = 11;\n"
		"int b = 22;\n"
		"flt c = 33;\n"
		"int d = 44;\n"
		"c = c + 1;\n"
		"b = 23;\n"
		"a = b;\n"
		"d = a + c;\n"
		"return d;"},
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
	//push 2 ints, do a simple expression with them, set one to the other whilst returning
	{	"int a = 11;\n"
		"int b = 2;\n"
		"a = b + 100;\n"
		"return b = a;" },
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
		"return c = b = a = 66;\n"},
	//array variable
	{	"//int a[5];\n"
		"//a[2] = 100;\n"
		"//return a[2];"},
	//if statements
	{	"if(2!=1)\n"
		"\treturn 22;\n"
		"return 33;" },

	{	"int a = 2;\n"
		"int b = 150;\n"
		"if(a>=b)\n"
		"\treturn 1;\n"
		"return 0;" },

	{	"if(22!=22)\n"
		"   return 111;\n"
		"else\n"
		"   return 222;\n" },

	{	"if(666==777)\n"
		"{\n"
		"   return 333;\n"
		"}\n"
		"else\n"
		"{\n"
		"   return 444;\n" 
		"}"	},

	//function calls
	{	"int a = 33; \n"
		"func int f1()\n"
		"{\n"
		"return 22;\n"
		"}\n"
		"return a;" },

	{	"func int f1()\n"
		"{\n"
		"return 22;\n"
		"}\n"
		"return f1();" },

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
	" a + func1;\n" },

	//erroneous scripts
	//undeclared ident
	{	"int a = 2;\n"
		"return a + b;"}
};