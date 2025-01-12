#pragma once

#include <map>

class CUav;
struct WayPoint;

class CUavManager
{

public:
	void Run(double timeStep, double time);
	
	// Add a new UAV to the map
	bool addUAV(int id);
	
	// Remove a UAV from the map
	bool removeUAV(int id);
	
	// Get UAV by ID
	CUav* getUAV(int id);
	
	// Clear all UAVs
	void clearAll();
	
	int getUavNumbers() { return planeTable.size(); }

	~CUavManager() {clearAll();	}


private:
	std::map<int, CUav*> planeTable;

};



