#include "ConfigManager.h"
#include <fstream>

bool CConfigManager::ReadConfigurationFromFile(const std::string & filename)
{
	std::fstream confFile(filename);
	if (!confFile.is_open())
	{
		throw std::runtime_error("Could not open file " + filename);
	}

	std::string line;
	while (std::getline(confFile, line))
	{
		if (line.empty() || line[0] == '#') continue;
		auto signPos = line.find('=');
		if (std::string::npos == signPos) continue;
		std::string param = trim(line.substr(0, signPos));
		std::string value = trim(line.substr(signPos + 1));

		/*if ("X0" == param)  currentPoint.x = std::stod(value);
		else if ("Y0" == param) currentPoint.y = std::stod(value);
		else if ("Z0" == param) currentPoint.z = std::stod(value);
		else if ("R" == param) minRadius = std::stoi(value);
		else if ("V0" == param) linearVelocity = std::stod(value);
		else if ("Az" == param) azimuth = std::stod(value);
*/
	}
	confFile.close();
	return true;
}

std::string CConfigManager::trim(const std::string & str)
{
	return std::string();
}
