#pragma once
#include <vector>
#include <fstream>


class IFlightMode;


struct WayPoint
{
	double x, y, z;

	
	WayPoint(double x=0, double y=0, double z=0) : x(x), y(y), z(z) {}
	
	WayPoint& operator+=(const WayPoint& p);
	bool operator==(const WayPoint& p) const;
	// Stream operator
	friend std::ostream& operator<<(std::ostream& os, const WayPoint& p);
	
	// Function to calculate distance between two 3D points
	double calculateDistance(const WayPoint& p) const;
	// Function to calculate heading angle in the XY plane
	double calculateHeading(const WayPoint& p) const;


};

struct UAVTask
{
	WayPoint destination;
	double scheduledTime;
	int tailNum;

	UAVTask(const WayPoint& dest = WayPoint(), double t =0, int tail=0) : destination(dest), scheduledTime(t), tailNum(tail) {}
	bool operator>(const UAVTask& other) const { return scheduledTime > other.scheduledTime; }
};

struct CompareUAVTask
{
	bool operator()(const UAVTask& a, const UAVTask& b) const
	{
		return a.scheduledTime > b.scheduledTime; //For min-heap 
	}
};

enum class enmFlightMode:uint8_t
{
	withoutdestination,
	withdest,
	incircle
};

class CUav
{
public:
	explicit CUav(int tail = -1);
	//CUav(int tail,const WayPoint& start, int az, size_t Velocity, size_t R );
	virtual ~CUav();
	
	void FlyTo(double dt, double time);
	void FlyWithoutDest(double dt, double time);
	void FlyToDest(double dt, double time);
	void StopAtCoordinate(double dt, double time);
	
	int getId() const { return tailNum; }
	void setPosition(const WayPoint& pos);
	const WayPoint& getPosition() const { return currentPoint; };
	
	const WayPoint& getDestPosition() const { return destPoint; };
	void setDestination(const WayPoint& pos) { destPoint = pos; };
	
	double getDistanceFromOrigin() const { const WayPoint p;  return currentPoint.calculateDistance(p); } ;
	void setMode(enmFlightMode newMode);
	bool getCommand;

protected:
			
	bool ReadConfigurationFromFile(const std::string& filename);
	void OpenReportFile(int id);
	void Report(double time);
	void UpdatePosition(double time);
	std::string trim(const std::string& str);
	void CalculateTurnRate(double desiredHeading, double time);
	/*
	void HandleWithoutDestination(double dt, double time);
	void HandleWithDestination(double dt, double time);
	void HandleInCircle(double dt, double time);
	*/
private:
	size_t tailNum;
	WayPoint currentPoint;
	WayPoint destPoint;
	size_t minRadius;
	double linearVelocity, angularVelocity;
	double azimuth; //in degrees 0-360
	double distanceFromOrigin;
	std::string ReportFileName ;
	const std::string ConfigurationFileName = "SimParams.ini";
	std::ofstream reportFile;
	std::unique_ptr<IFlightMode> currentMode;
	//enmFlightMode mode;
};

