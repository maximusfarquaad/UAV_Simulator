#pragma once
#ifndef UAVSIMULATOR_PROJ_H
#define UAVSIMULATOR_PROJ_H

#include <string>
#include <vector>
//#include <map>
#include <queue> // for taskQueue 
#include "UavManager.h"
#include "CUav.h"


class CSimulator
{
public:
	static CSimulator& GetInstance();
	void Run();
	void Init();
	
	CUavManager UAVManager;
	
	CSimulator(const CSimulator&) = delete;
	CSimulator& operator=(const CSimulator&) = delete;
protected:
	UAVTask ParseCommandLine(const std::string& taskLine);
	bool ReadConfigurationFromFile(const std::string& filename);
	bool ReadCommandsFromFile(const std::string& filename);
	bool IsTaskScheduled(double scheduletime, double time);
private:
	CSimulator();
	~CSimulator() {};
	std::string trim(const std::string& str);
		
	const std::string CommandFileName ="SimCmds.txt";
	const std::string ConfigurationFileName = "SimParams.ini";
	double timeStep; 
	double timeLimit;
	size_t planesNumber;
	double deltaTime;
	std::priority_queue<UAVTask,
		std::vector<UAVTask>,
		/*std::greater<UAVTask>*/ CompareUAVTask> taskQueue;
	
};






#endif // !UAVSIMULATOR_PROJ_H
