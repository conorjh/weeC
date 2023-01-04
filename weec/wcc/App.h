#ifndef APP_H
#define APP_H
#include "config.h"
#include "cmd_line.h"
#include "Util.h"
#include "Wcc.h"

namespace App
{
	struct CommandLineSettings
	{
		CommandLineSettings(CommandLine::Cmd_Line);

		//settings
		std::string Filepath, ConfigPath;
		unsigned int LogLevel;

		wcc::CompilerProfile Profile;

		//errors
		Error::ErrorData Errors;

	private:
		bool ParseCommandLine(CommandLine::Cmd_Line);
	};

	struct AppData
	{
		AppData();

		void UpdateFromConfig(Config::ConfigData Cfg);

		bool Halted = false;

	};

	class AppIO
	{
		AppData& Data;

	public:
		AppIO(AppData& _Data);

		bool Init();

		void Update();

		bool Esc = false;
	};

	class Application
	{
	public:
		CommandLine::Cmd_Line CmdLine;
		Config::ConfigData Cfg;
		AppIO IO;
		AppData Data;

		Application(int argc, char* argv[]);

		bool Ended() const;

		bool Init();
		void Update();

		void Splash();
	};


}

#endif