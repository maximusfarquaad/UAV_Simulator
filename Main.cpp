
#include "CUav.h"
#include "UavSimulator.h"
#include <iostream>

int main()
{

	WayPoint start(0, 0, 10);
	WayPoint end(100, 100, 10);
	double min_turn_radius = 100;

	//vector<WayPoint> flight_path = simulateFlight(start, end, min_turn_radius);
	using namespace std;
	cout << "<-------- Flight Simualtor ------->" << endl;

	try
	{
		/*
		auto t = CSimulator::GetInstance().UAVManager.addUAV(1);
		auto t1 = CSimulator::GetInstance().UAVManager.addUAV(1);
		auto t2 =  CSimulator::GetInstance().UAVManager.removeUAV(2);
		auto t3 =  CSimulator::GetInstance().UAVManager.removeUAV(1);
		auto t4 = CSimulator::GetInstance().UAVManager.getUavNumbers();

		auto t5 =  CSimulator::GetInstance().UAVManager.addUAV(1);
		auto t6 =  CSimulator::GetInstance().UAVManager.addUAV(2);
		auto t7 =  CSimulator::GetInstance().UAVManager.getUavNumbers();
		auto t8 =  CSimulator::GetInstance().UAVManager.getUAV(1);
		auto t9 =  CSimulator::GetInstance().UAVManager.getUAV(3);
		CSimulator::GetInstance().UAVManager.clearAll();
		auto t10 =  CSimulator::GetInstance().UAVManager.getUavNumbers();
		*/
		//auto t11 = CSimulator::GetInstance().UAVManager.addUAV(1);
		CSimulator::GetInstance().Init();
		CSimulator::GetInstance().Run();
		
		
		//std::cout << t10 << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	
	/*
	for (const WayPoint& point : flight_path) {
		cout << "  (" << point.x << ", " << point.y << ", " << point.z << ")" << endl;
	}
*/
	return 0;
}
