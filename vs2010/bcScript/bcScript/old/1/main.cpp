#include "bcscript.h"
#include <Windows.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>


void PrintSource(vector<string> in)
{
	cout<<"Source Code" << endl;
	for(int t=0;t<in.size();++t)
	{
		cout << "000" << t+1 << ": " << in.at(t) ;
	}
}

void PrintLexer(bcLexer in)
{
	cout<<"Lexer" << endl;
	cout<<"Type		Line   Col   Data" << endl;
	for(int t=0;t<in.data.size();++t)
	{
		cout << in.data.at(t).type << "   " << in.data.at(t).line << ", " << in.data.at(t).col << "    \"" <<  in.data.at(t).data << "\"" << endl;
	}
}

void PrintParser(bcParser in)
{
	tree<bcParseNode>::iterator it=in.ptree.begin();
	cout<<"Parser" << endl;
	while(it!=in.ptree.end()) 
	{
		for(int t=0;t<in.ptree.depth(it);++t)
		{
			cout << " ";
		}
		cout << (it->type) << endl;
		++it;
    }
}

void PrintSymTab(bcParser in)
{
	string buff="";
	map <string,bcSymbol>::iterator it;
	map <string,bcSymbol>::iterator scopeit;
	cout<<"Symbol Table" << endl;
	for(it = in.symtab.begin();it!=in.symtab.end();it++)
	{		
		scopeit=it->second.scope;
		buff=scopeit->first;
		cout << "Ident: " << it->first << "\t\tType: " << it->second.type << "\tscope: "  << buff << endl;
	}
}

void PrintIStream(bcByteCodeGen in)
{
	cout<<"Instruction Stream" << endl;
	for(int t=0;t<in.istream.size();++t)
	{
		cout << "Opcode: " << in.istream.at(t).opcode << "   Operand Type: " << in.istream.at(t).operands.at(0).type << "    Operand Data: " << in.istream.at(t).operands.at(0).data <<   endl;
	}
}

int main()
{
	vector<string> source;// = LoadText();
	//LoadFile("test.txt");
	//source.clear();
	bcreadfile("test2.txt",&source);
	//bcreadfile("tree.hh",&source);
	bcLexer Lexer;
	bcParser Parser;
	bcByteCodeGen Gen;
	bcVM VM;
	
	clock_t start;
	double genavg=0;
	double parseavg=0;
	double lexavg=0;
	
	double lextotal=0;
	double parsetotal=0;
	double gentotal=0;
	
	double lexdiff=0;
	double parsediff=0;
	double gendiff=0;
		
	for(int y=1;y<200;++y)
	{
		start = clock();
		Lexer.Lex(&source);
		lexdiff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		lextotal+=lexdiff;
		lexavg=lextotal/y;

		start = clock();
		Parser.Parse(&Lexer.data);
		parsediff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		parsetotal+=parsediff;
		parseavg=parsetotal/y;

		//Parser.ptree.clear();
		if(!Parser.IsError())
		{
			start = clock();
			Gen.Init(&Parser.ptree,&Parser.symtab,&Parser.funcsigs,&Lexer.data,&Parser.stackframes);
			Gen.Gen();
			gendiff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
			gentotal+=gendiff;
			genavg=gentotal/y;
		}
	}

	PrintSource(source);
	PrintLexer(Lexer);
	PrintParser(Parser);
	PrintSymTab(Parser);
	PrintIStream(Gen);

	if(Parser.IsError())
	{
		cout << "Error: " << Parser.GetError() << "   @" << Parser.GetErrorLine() << ", " << Parser.GetErrorCol() << endl;
	}
	if(Gen.IsError())
	{
		cout <<"Gen Error " << Gen.GetError() << endl;
	}

	cout << "Average Lex Time Taken: " << lexavg << endl;
	cout << "Average Parse Time Taken: " << parseavg << endl;
	cout << "Average Generator Time Taken: " << genavg << endl;
	cout << "Total Time Taken: " << lexavg+parseavg+genavg <<endl<<endl;
	cout << "Running VM..." << endl;

	VM.Init(Gen.istream);
	VM.Run();

	string buff="";
	cin >> buff;

	return 1;
}