#ifndef APP_H
#define APP_H
#include "Util.h"
#include "Wcc.h"

namespace App
{
	enum class CommandLineArgumentType
	{
		ApplicationPath, Argument, OptionalArgument, FreeFloatingParameter
	};

	struct CommandLineArgument
	{
		CommandLineArgumentType Type;
		std::string Data;
		std::vector<std::string> Parameters;
	};

	class CommandLine
	{
	public:
		CommandLine(int argc, char* argv[]);

		std::vector<CommandLineArgument> Arguments;
	};

	struct Config
	{
		Config() {}

		unsigned int Ver = 1;
		std::string VerString = "0.0.0.1";
	};

	class ConfigParser
	{
		std::string Filepath;
	public:
		ConfigParser(std::string _Filepath);

		Config Parse();

		Error::ErrorData Errors;
	};

	struct CommandLineSettings
	{
		CommandLineSettings();

		//settings
		std::string Filepath, ConfigPath; 
		unsigned int LogLevel;
		
		wcc::CompilerProfile Profile;

		//errors
		Error::ErrorData Errors;
	};


	struct AppData
	{
		AppData();

		void UpdateFromConfig(Config Cfg);


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
		CommandLine CmdLine;
		Config Cfg;
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