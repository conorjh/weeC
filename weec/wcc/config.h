#ifndef CONFIG_H
#define CONFIG_H
/*
config.h
Config file class and parser

by Conor Haddock
*/
#include <string>
#include "util.h"

namespace App
{
	namespace Config
	{
		struct ConfigData
		{
			ConfigData() {}

			//members go here
		};

		struct ConfigParserError
		{
			std::string Msg;
		};

		class ConfigParser
		{
			std::string Filepath;

		public:
			ConfigParser(std::string _Filepath)
			{
				Filepath = _Filepath;

				if (!Util::FileExists(Filepath))
				{
					Error.Msg = "Missing file @ " + Filepath;
					return;
				}
			}

			bool HasErrored()
			{
				return Error.Msg.size() > 0 ? true : false;
			}

			ConfigData Parse() 
			{
				if (HasErrored())
					return ConfigData();

				ConfigData Output;


				return Output;
			}

			ConfigParserError Error;
		};
	}
	
}
#endif