#pragma once

#include <string>

class CConfigManager
{
public:
	static bool ReadConfigurationFromFile(const std::string & filename);

private:
	static std::string trim(const std::string& str);
	const std::string ConfigurationFileName = "SimParams.ini";

};

