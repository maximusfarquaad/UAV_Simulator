// UavSimulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#ifdef _DEBUG
#include <windows.h> 
#endif // DEBUG


#include "UavSimulator.h"
#include "CUav.h"

using namespace std;



CSimulator& CSimulator::GetInstance()
{
	static CSimulator instance;
	return instance;
}

void CSimulator::Run()
{
	using namespace std::chrono;
	//double time(0);

	double accumulatedTime = 0.0;   // Accumulate time until it reaches timeStep
	double elapsedTime = 0.0;
	auto startTime = high_resolution_clock::now();
	auto lastUpdateTime = startTime;

	while (elapsedTime < timeLimit)
	{
		// Get current time
		auto currentTime = high_resolution_clock::now();
		// Calculate time since last frame
		deltaTime = duration<double>(currentTime - lastUpdateTime).count();
		elapsedTime = duration<double>(currentTime - startTime).count();

		#ifdef _DEBUG
		std::string msg;
		msg = "Elapsed Time [s]: " + std::to_string(elapsedTime) + " delta Time [s]: " + std::to_string(deltaTime) + "\n";
		OutputDebugStringA(msg.data());
		#endif // DEBUG
		
		// Accumulate the time
		accumulatedTime += deltaTime;
		if (accumulatedTime >= timeStep)
		{
			WayPoint destPoint;
			if (!taskQueue.empty())
			{
				auto task = taskQueue.top();

				
				#ifdef _DEBUG
				std::string msg;
				msg = "accumulated Time [s]: " + std::to_string(accumulatedTime)+ "\n" ;
				OutputDebugStringA(msg.data());
				#endif // DEBUG

				if (IsTaskScheduled(task.scheduledTime, elapsedTime))
				{
					// when time  is come get task from a que 
					auto plane = UAVManager.getUAV(task.tailNum);
					if (plane)
					{
						plane->getCommand = true;
						plane->setDestination(task.destination);
					}
					taskQueue.pop();
				}
			}


			UAVManager.Run(timeStep, elapsedTime);
			accumulatedTime -= timeStep;
			
		}

		lastUpdateTime = currentTime;
		// small sleep to prevent CPU overload
		std::this_thread::sleep_for(microseconds(100)); // 0.1ms sleep
		
	}
}

void CSimulator::Init()
{
	//Add UAV  to simulation
	UAVManager.clearAll();
	for (size_t i = 0; i < planesNumber; i++)
	{
		UAVManager.addUAV(i);
	}
}


CSimulator::CSimulator() :timeStep(0), timeLimit(0), planesNumber(0)
{
	ReadConfigurationFromFile(ConfigurationFileName);
	ReadCommandsFromFile(CommandFileName);
};

UAVTask CSimulator::ParseCommandLine(const std::string & line)
{
	std::stringstream ss(line);
	UAVTask task;

	// Parse all expected input values
	if (!(ss >> task.scheduledTime >> task.tailNum >> task.destination.x >> task.destination.y)) {
		throw std::invalid_argument("Invalid command line format: Expected 'scheduledTime tailNum x y z'");
	}

	return task;
}

bool CSimulator::ReadConfigurationFromFile(const std::string & filename)
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
		if ("Dt" == param) timeStep = std::stod(value);
		else if ("N_uav" == param) planesNumber = std::stoi(value);
		else if ("TimeLim" == param) timeLimit = std::stod(value);

	}
	confFile.close();
	return true;
}

bool CSimulator::ReadCommandsFromFile(const std::string & filename)
{
	std::ifstream Commands(filename);
	if (!Commands.is_open())
	{
		throw std::runtime_error("Could not open file " + filename);
	}
	std::string line;
	while (std::getline(Commands, line))
	{
		if (line.empty() || line[0] == '#') continue;

		auto task = ParseCommandLine(line);
		taskQueue.push(task);
	}

	return true;
}

std::string CSimulator::trim(const std::string & str)
{
	size_t first = str.find_first_not_of(" \t");
	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

bool CSimulator::IsTaskScheduled(double scheduledTime, double time)
{
	double tollerance = deltaTime ;
	auto res = std::abs(scheduledTime - time) < tollerance;
	return res;
}


