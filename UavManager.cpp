#include "UavManager.h"
#include "CUav.h"

void CUavManager::Run(double timeStep, double time )
{
	for (const auto& pair :planeTable)
	{
		auto* uav = pair.second;
		if (!uav)
		{
			throw std::runtime_error("CUavManager::Run: Null UAV pointer in planeTable.");
		}
		uav->FlyTo(timeStep,time);
	}
}

bool CUavManager::addUAV(int id)
{
	if (id < 0 || planeTable.count(id)>0)
	{
		return false;
	}
	planeTable[id] = new CUav(id);
	return true;
}

bool CUavManager::removeUAV(int id)
{
	auto it = planeTable.find(id);
	if (it == planeTable.end())
	{
		return false;
	}
	else
	{
		delete it->second;
		planeTable.erase(it);
	}
	return true;
}

CUav * CUavManager::getUAV(int id)
{
	auto it = planeTable.find(id);
	return (it != planeTable.end()) ? it->second : nullptr;
}

void CUavManager::clearAll()
{
	for (auto& pair : planeTable)
	{
		delete pair.second;  // Free memory for each UAV
	}
	planeTable.clear();
}
