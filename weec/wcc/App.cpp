#include "App.h"
#include "Util.h"
#include "spdlog/spdlog.h"
#include <filesystem>

using namespace std;
using namespace App;
using namespace Error;
using namespace Util;

using enum CommandLineArgumentType;

App::CommandLineSettings::CommandLineSettings()
{
	Filepath = "";
	ConfigPath = "config.cfg";
	LogLevel = 0;
}


CommandLineSettings ParseCommandLine(CommandLine CmdLine)
{
	CommandLineSettings Output;

	for (auto& Argument : CmdLine.Arguments)
		if (Argument.Type == ApplicationPath)
		{
			Output.Filepath = Argument.Data;
		}

		else if (Argument.Data == "-cfg")
		{
			if (Argument.Parameters.size() != 1)
			{
				//error - no parameters
				Output.Errors.AddError("-cfg: No Filepath specified");
				continue;
			}

			Output.ConfigPath = Argument.Parameters[0];
		}

		else if (Argument.Data == "-log")
		{
			if (Argument.Parameters.size() != 1)
			{
				//error - no parameters
				Output.Errors.AddError("-log: no log level specified (should be a number, 0 - 6)");
				continue;
			}

			auto IntBuff = stoi(Argument.Parameters[0]);
			if (IntBuff < 0 || IntBuff > 6)
			{
				//invalid log level bro
				Output.Errors.AddError("-log: Invalid log level, should be a number 0 to 6, got: " + Argument.Parameters[0]);
				continue;
			}

			Output.LogLevel = IntBuff;
		}

		else
		{
			//unknown parameter - error
			Output.Errors.AddError("Invalid argument: " + Argument.Data);
		}

	return Output;
}

App::ConfigParser::ConfigParser(std::string _Filepath)
{
	Filepath = _Filepath;
	
	if (!FileExists(Filepath))
	{
		Errors.AddError("Missing file @ " + Filepath);
		return;
	}
}

Config App::ConfigParser::Parse()
{
	if(Errors.HasErrored())
		return Config();

	Config Output;


	
	return Output;
}

App::Application::Application(int argc, char* argv[]) : CmdLine(argc, argv), IO(Data)
{
	spdlog::set_level(spdlog::level::level_enum::trace);
	srand(time(0));

	spdlog::info("Application launch");
	spdlog::trace("Running dir " + std::filesystem::current_path().string());

	spdlog::debug("Parsing command line");
	auto Settings = ParseCommandLine(CmdLine);
	if(Settings.Errors.HasErrored())
		spdlog::debug("Parsing command line");
	
	/*
	* Config parser - TODO
	spdlog::debug("Opening config " + Settings.ConfigPath);
	ConfigParser CfgParser(Settings.ConfigPath);
	if (CfgParser.Errors.HasErrored())
	{
		//error with config file, use exist defaults
		spdlog::warn("Errors in config @ " + Settings.ConfigPath);
	}
	*/
	Data.UpdateFromConfig(Cfg);
}

bool App::Application::Ended() const
{
	return Data.Halted;
}


bool App::Application::Init()
{
	spdlog::info("Application init");

	srand(time(0));

	spdlog::debug("IO Init");
	IO.Init();


	return true;
}

void App::Application::Update()
{
	IO.Update();
}

App::AppIO::AppIO(AppData& _Data) : Data(_Data)
{

}

bool App::AppIO::Init()
{
	return true;
}

void App::AppIO::Update()
{
	Esc = false;
}

App::AppData::AppData()
{
}

void App::AppData::UpdateFromConfig(Config Cfg)
{
	ScreenWidth = Cfg.ScreenWidth; ScreenHeight = Cfg.ScreenHeight;
	ShowFPS = Cfg.ShowFPS;
}


/*
Takes the standard C/C++ char* array, returns a vector of CommandLineArgument objects
Arguments have the following syntax
	-argument
	/argument

Any argument may have unlimited parameters
	-argument param1
	/argument param1 param2

	eg. "app.exe -filepath c:/path/file.csv"
	eg. "app.exe /move c:/src/file.csv c:/dst/file.csv"

Free floating parameters have no parameters
	argument1 argument2

	eg "app.exe hidelaucher showfps"
*/
App::CommandLine::CommandLine(int argc, char* argv[])
{
	CommandLineArgument arg;
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
				arg = CommandLineArgument();
			}

			if (argv[index][0] == '--')
				arg.Type = OptionalArgument;
			else
				arg.Type = Argument;

			arg.Data = string(argv[index]).substr(1, strlen(argv[index]) - 1);
			parsingParams = true;
			break;

			//parameter
		default:
			arg.Parameters.push_back(argv[index]);
			if (!parsingParams)
			{
				//the first item is always the exe location
				arg.Type = index == 0 ? ApplicationPath : FreeFloatingParameter;
				arg.Data = argv[index];
				Arguments.push_back(arg);
				arg = CommandLineArgument();
			}
			break;
		}
		index++;
	}

	if (parsingParams)
		Arguments.push_back(arg);
}
