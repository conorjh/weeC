#include "bcscript.h"
#include "test.h"
#include <iostream>

using namespace bc::test;
using namespace bc::lex;
using namespace bc::parse;
using namespace bc::vm;
using namespace std;

int main(int argc,const char* args[])
{
	string line;
	bcLexer l,conlex;
	bcParser p;	
	bcByteCodeGen g;
	bcVM vm;
	l.startup();
	conlex.startup();
	p.startup();	p.lexer=&l;	

	cout << "bcScript Compiler   ";
	cout << bc::bcVerMajor << "." << bc::bcVerMinor << "." << bc::bcVerPatch << "." << bc::bcVerRC << "-" << bc::bcVerDemo << endl;
	bool exit = false;
	while(!exit)
	{
		std::getline(std::cin, line);
		//line="run \"2.bcs\"";
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
							vm.con=g.gen();
							printLexer(&l);
							printParser(&p);
							printGen(&g);
							std::cout << std::endl;
							while(!vm.con->halt)
								vm.exec(0);
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
		cout << bc::util::bcitos(*vm.con->stack.at(vm.con->stack.size()-1)) << std::endl;
		p.clear();	p.startup();
		vm=bcVM();
		p.lexer=&l;	line="";
		l.clear();conlex.clear();
		conlex.startup();l.startup();
	}
	return 1;
}