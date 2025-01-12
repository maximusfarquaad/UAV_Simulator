#include "FlightMode.h"
#include <Windows.h> // for OutputDebugStringA()
#include <string> //for std::to_string

void WithoutDestinationMode::Execute(CUav & uav, double dt, double time)
{

	// Implement the behavior specific to this state (no destination)
	uav.FlyWithoutDest(dt, time);  // Call a method for the 'withoutdestination' mode
	if (uav.getCommand)
	{
		uav.setMode(enmFlightMode::withdest);  // Transition to the next mode when command received
	}

}

void WithDestinationMode::Execute(CUav & uav, double dt, double time)
{
	// Implement behavior for the 'with destination' state
	uav.FlyToDest(dt, time);  // Command the UAV to fly towards a destination
	constexpr double DESTINATION_THRESHOLD = 1.0;
	auto dist = uav.getPosition().calculateDistance(uav.getDestPosition());
	
	#ifdef _DEBUG
	std::string msg;
	msg = "UAV ID ="+ std::to_string(uav.getId()) +  " distance [m] = " + std::to_string(dist) + "\n";
	OutputDebugStringA(msg.data());
	#endif // DEBUG

	if (dist < DESTINATION_THRESHOLD)
	{
		uav.setMode(enmFlightMode::incircle);  // Transition to 'incircle' when close to destination
	}
}

void InCircleMode::Execute(CUav & uav, double dt, double time)
{
	// Implement behavior for the 'incircle' state
	uav.StopAtCoordinate(dt, time);  // Command the UAV to stop at a specific coordinate
	if (uav.getCommand)
	{
		uav.setMode(enmFlightMode::withdest);  // Transition to 'withdest' mode when command received
	}
}
