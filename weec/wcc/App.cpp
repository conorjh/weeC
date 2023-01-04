#include "App.h"
#include "cmd_line.h"
#include "config.h"
#include "Util.h"
#include "spdlog/spdlog.h"
#include <filesystem>

using namespace std;
using namespace App;
using namespace App::CommandLine;
using namespace App::Config;
using namespace Error;
using namespace Util;

using enum Cmd_LineArgumentType;

App::CommandLineSettings::CommandLineSettings(Cmd_Line CmdLine)
{
	Filepath = "";
	ConfigPath = "config.cfg";
	LogLevel = 0;

	ParseCommandLine(CmdLine);
}

bool App::CommandLineSettings::ParseCommandLine(Cmd_Line CmdLine)
{
	for (auto& Argument : CmdLine.Arguments)
		if (Argument.Type == ApplicationPath)
			this->Filepath = Argument.Data;

		else if (Argument.Data == "-cfg")
		{
			if (Argument.Parameters.size() != 1)
			{
				//error - no parameters
				this->Errors.AddError("-cfg: No Filepath specified");
				continue;
			}

			this->ConfigPath = Argument.Parameters[0];
		}

		else if (Argument.Data == "-log")
		{
			if (Argument.Parameters.size() != 1)
			{
				//error - no parameters
				this->Errors.AddError("-log: no log level specified (should be a number, 0 - 6)");
				continue;
			}

			auto IntBuff = stoi(Argument.Parameters[0]);
			if (IntBuff < 0 || IntBuff > 6)
			{
				//invalid log level bro
				this->Errors.AddError("-log: Invalid log level, should be a number 0 to 6, got: " + Argument.Parameters[0]);
				continue;
			}

			this->LogLevel = IntBuff;
		}

		else if (Argument.Data == "-sym")
		{
			this->Profile.BuildSymbolTable = true;
		}

		else
		{
			//source filepath has no argument
			if(this->Profile.SourceFilepath == "")
				this->Profile.SourceFilepath = Argument.Data;
			else
				//source filepath redefinition, warning
				this->Errors.AddError("Source filepath given more than once");
		}


	if(this->Profile.BuildSymbolTable && this->Profile.DestinationFilepath != "")
		this->Profile.SymbolTableFilepath = this->Profile.DestinationFilepath + ".sym";

	return true;
}


App::Application::Application(int argc, char* argv[]) : CmdLine(argc, argv), IO(Data)
{
	spdlog::set_level(spdlog::level::level_enum::trace);
	srand(time(0));

	spdlog::info("Application launch");
	spdlog::trace("Running dir " + std::filesystem::current_path().string());

	spdlog::debug("Parsing command line");
	CommandLineSettings Settings(CmdLine);
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

void App::AppData::UpdateFromConfig(ConfigData Cfg)
{

}
