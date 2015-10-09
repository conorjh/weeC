#include "export.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace bc
{
	namespace bcc
	{
		//const char * bcc_exportDelimiter = ",";
	}
}



void bc::exp::exportToByteCodeFile(vm::bcExecContext* p_ec, string p_fn)
{
	ofstream f;
	string dl = ",";	//find a way to make this a global setting
	f.open(p_fn.c_str());

	//write header
	f << bcVerMajor << dl << bcVerMinor << dl << bcVerPatch << dl << bcVerRC << dl;
	f << p_fn << dl;							//program name
	f << p_ec->istream.size() << dl;			//size of body
	f << sizeof(p_ec->stack.size()) << dl;		//size of stack

	//write main body
	for (int t = 0; t < p_ec->istream.size(); ++t)
	{
		f << p_ec->istream[t].op << dl;
		f << p_ec->istream[t].arg1 << dl;
		f << p_ec->istream[t].arg2 << dl;
	}

	//write stack
	for (int t = 0; t < p_ec->stack.size(); ++t)
	{
		f << p_ec->stack.at(t) << dl;
	}

	f.close();
}