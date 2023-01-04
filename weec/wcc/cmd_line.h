#ifndef CMD_LINE_H
#define CMD_LINE_H
/*
cmd_line.h
Parse your standard argc,argv command line arguments

by Conor Haddock
*/
#include <string>
#include <vector>

namespace App
{
	namespace CommandLine
	{
		enum class Cmd_LineArgumentType
		{
			ApplicationPath, Argument, OptionalArgument, FreeFloatingParameter
		};

		struct Cmd_LineArgument
		{
			Cmd_LineArgumentType Type;
			std::string Data;
			std::vector<std::string> Parameters;
		};

		class Cmd_Line
		{
		public:
			Cmd_Line(int argc, char* argv[])
			{
				Cmd_LineArgument arg;
				bool parsingParams = false;
				int index = 0;
				while (index < argc)
				{
					switch (argv[index][0])
					{
					case '//':
					case '-':	//command			
						if (parsingParams)
						{
							Arguments.push_back(arg);
							arg = Cmd_LineArgument();
						}

						if (argv[index][0] == '--')
							arg.Type = Cmd_LineArgumentType::OptionalArgument;
						else
							arg.Type = Cmd_LineArgumentType::Argument;

						arg.Data = std::string(argv[index]).substr(1, strlen(argv[index]) - 1);
						parsingParams = true;
						break;

						//parameter
					default:
						arg.Parameters.push_back(argv[index]);
						if (!parsingParams)
						{
							//the first item is always the exe location
							arg.Type = index == 0 ? Cmd_LineArgumentType::ApplicationPath : Cmd_LineArgumentType::FreeFloatingParameter;
							arg.Data = argv[index];
							Arguments.push_back(arg);
							arg = Cmd_LineArgument();
						}
						break;
					}
					index++;
				}

				if (parsingParams)
					Arguments.push_back(arg);
			}

			std::vector<Cmd_LineArgument> Arguments;
		};

	}
}


#endif