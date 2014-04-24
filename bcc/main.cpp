#include "bcscript.h"
#include "test.h"
#include <iostream>

using namespace bc::test;
using namespace bc::lex;
using namespace bc::parse;
using namespace bc::vm;
using namespace std;

int main()
{
	string line;
	bcLexer l,conlex;
	l.startup();
	conlex.startup();
	bcParser p;	
	bcByteCodeGen g;
	p.startup();	p.lexer=&l;	

	//return test_parser();
	cout << "bcScript Compiler   ";
	cout << bc::bcVerMajor << "." << bc::bcVerMinor << "." << bc::bcVerPatch << "." << bc::bcVerRC << "-" << bc::bcVerDemo << endl;
	bool exit = false;
	while(!exit)
	{
		std::getline(std::cin, line);
		conlex.source->push_back(line);
		if(conlex.nextToken())
			switch(conlex.getToken()->type)
			{
			case tt_ident:
				if(conlex.getToken()->data == "run")
					if(conlex.nextToken() && conlex.getToken()->type == tt_strlit)
						if(bc::util::bcreadfile(conlex.getToken()->data.c_str(),l.source))
						{
							p.parse();
							g.ast=&p.ast;
							//g.gen();
							printLexer(&l);
							printParser(&p);
						}
						else
						{
							cout << "Could not load "<< conlex.getToken()->data.c_str() << endl;
						}
				else if(conlex.getToken()->data == "exit")
					exit=true;
				break;

			default:
				break;
			}

		p.clear();
		p.startup();
		p.lexer=&l;
		line="";
		l.clear();
		conlex.clear();
		conlex.startup();
		l.startup();
	}
	return 1;
}