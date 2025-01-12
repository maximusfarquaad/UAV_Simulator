#define _USE_MATH_DEFINES  

#include "CUav.h"
#include <string>
#include <iomanip>  // for std::fixed and std::setprecision
#include "FlightMode.h"


CUav::CUav(int id) :tailNum(id), azimuth(0), linearVelocity(0), angularVelocity(0),
					minRadius(1), getCommand(false),
					currentMode(std::make_unique<WithoutDestinationMode>())
					//, mode(enmFlightMode::withoutdestination)
{
	ReadConfigurationFromFile(ConfigurationFileName);
	OpenReportFile(id);
	
}
/*
CUav::CUav(int tail,const WayPoint& start, int az, size_t Velocity, size_t R) : tailNum(tail), currentPoint(start),azimuth(az),linearVelocity(Velocity),minRadius(R)
{

}
*/
CUav::~CUav()
{
	if (reportFile.is_open()) {
		reportFile.close(); // Close the file 
	}
}


void CUav::FlyTo(double dt , double time)
{
	/*switch (mode)
	{
	case enmFlightMode::withoutdestination:
		HandleWithoutDestination(dt, time);
		break;
	case enmFlightMode::withdest:
		HandleWithDestination(dt, time);
		break;
	case enmFlightMode::incircle:
		HandleInCircle(dt, time);
		break;
	default:
		throw std::runtime_error("CUav::FlyTo: Unknown flight mode.");
		break;
	}*/

	currentMode->Execute(*this, dt, time);
}

void CUav::FlyWithoutDest(double dt, double time)
{
	UpdatePosition(dt);
	Report(time);
}

void CUav::FlyToDest(double dt, double time)
{
	// Calculate heading towards the destination and update position
	double desiredHeading = currentPoint.calculateHeading(destPoint);
	CalculateTurnRate(desiredHeading, dt);
	FlyWithoutDest(dt, time);
	
}

void CUav::StopAtCoordinate(double dt, double time)
{
	//Calculates heading to fly in a circle around the dest point.
		
	const double fullCircle = 360.0; // Full circle in degrees
	// Ensure the radius R is valid
	if (minRadius <= 0) {
		throw std::runtime_error("Invalid circle radius.");
	}

	// Calculate angular velocity
	angularVelocity = (linearVelocity / minRadius) *(180.0 / M_PI); // Convert ω to degrees/sec

	// Update angular position
	azimuth += angularVelocity * dt;

	// Keep angle within 0 to 2π for numerical stability
	if (azimuth >= fullCircle) {
		azimuth -= fullCircle;
	}
	// Convert angle to radians for trigonometric calculations
	double angleRad = azimuth * (M_PI / 180.0);

	// Calculate the new position based on the circular path
	currentPoint.x = minRadius * std::cos(angleRad) + destPoint.x;
	currentPoint.y = minRadius * std::sin(angleRad) + destPoint.y;
	// Log to file
	Report(time);
}

void CUav::setPosition(const WayPoint& pos)
{
	currentPoint = pos;
}

void CUav::setMode(enmFlightMode newMode)
{
	switch (newMode)
	{
	case enmFlightMode::withoutdestination:
		currentMode = std::make_unique<WithoutDestinationMode>();
		break;
	case enmFlightMode::withdest:
		currentMode = std::make_unique<WithDestinationMode>();
		break;
	case enmFlightMode::incircle:
		currentMode = std::make_unique<InCircleMode>();
		break;
	default:
		throw std::runtime_error("Invalid mode");
		break;
	}
}


bool CUav::ReadConfigurationFromFile(const std::string & filename)
{
	std::ifstream confFile(filename);
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
		
		if		("X0" == param)  currentPoint.x = std::stod(value);
		else if ("Y0" == param) currentPoint.y = std::stod(value);
		else if ("Z0" == param) currentPoint.z = std::stod(value);
		else if ("R" == param) minRadius = std::stoi(value);
		else if ("V0" == param) linearVelocity = std::stod(value);
		else if ("Az" == param) azimuth = std::stod(value);
		
	}
	confFile.close();
	return true;
}

void CUav::OpenReportFile(int id)
{
	const static  int width = 10;
	// Initialize the report file name
	ReportFileName = "UAV" + std::to_string(id) + ".txt";

	// Open the file in append mode
	reportFile.open(ReportFileName, std::ios::app);
	if (!reportFile.is_open()) {
		throw std::runtime_error("CUav::CUav: Could not open file " + ReportFileName);
	}

	// Check if the file already exists or is empty
	bool needHeader = (std::ifstream(ReportFileName).peek() == std::ifstream::traits_type::eof());

	// Write header if needed
	if (needHeader) {
		reportFile << std::fixed << std::setprecision(3);
		reportFile << "Time" << std::setw(width) << "X" << std::setw(width) << "Y"
			<< std::setw(width) << "Az" << std::endl;
		reportFile << "-------------------------------------------" << std::endl;
	}
}

void CUav::Report(double time)
{
	if (!reportFile.is_open()) {
		throw std::runtime_error("CUav::Report: Report file is not open.");
	}
	// Write the data
	reportFile << std::setw(8) << time << std::setw(8) << currentPoint.x
		<< std::setw(8) << currentPoint.y << std::setw(8) << azimuth << std::endl;
}

void CUav::UpdatePosition(double dt)
{
	// Ensure azimuth is in degrees and convert to radians
	double azimuthRad = azimuth * (M_PI / 180.0);
	// Update position based on velocity and direction
	currentPoint.x += linearVelocity * cos(azimuthRad) * dt;
	currentPoint.y += linearVelocity * sin(azimuthRad) * dt;
}

std::string CUav::trim(const std::string & str)
{
	size_t first = str.find_first_not_of(" \t");
	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}


void CUav::CalculateTurnRate(double desiredHeading, double dt)
{

	// Ensure valid radius
	if (minRadius <= 0) {
		throw std::runtime_error("Invalid turn radius: must be positive.");
	}

	// Calculate the angular velocity in radians per second
	angularVelocity = linearVelocity / minRadius;

	// Normalize desiredHeading and azimuth to [0, 360)
	desiredHeading = std::fmod(desiredHeading + 360.0, 360.0);
	azimuth = std::fmod(azimuth + 360.0, 360.0);

	// Calculate delta heading and normalize to [-180, 180]
	double deltaHeading = desiredHeading - azimuth;
	if (deltaHeading > 180.0) deltaHeading -= 360.0;
	if (deltaHeading < -180.0) deltaHeading += 360.0;

	// Update azimuth with angular velocity, constrained by delta heading
	const double maxTurn = angularVelocity * (dt * 180.0 / M_PI); // Convert to degrees
	if (std::abs(deltaHeading) > maxTurn) {
		azimuth += copysign(maxTurn, deltaHeading);
	}
	else {
		azimuth = desiredHeading;
	}

	// Normalize azimuth to [0, 360)
	azimuth = std::fmod(azimuth + 360.0, 360.0);
}

#if 0



void CUav::HandleWithoutDestination(double dt, double time)
{
	FlyWithoutDest(dt, time);
	if (getCommand)
	{
		mode = enmFlightMode::withdest;
	}
}

void CUav::HandleWithDestination(double dt, double time)
{
	FlyToDest(dt, time);
	constexpr double DESTINATION_THRESHOLD = 10.0;
	if (currentPoint.calculateDistance(destPoint) < DESTINATION_THRESHOLD)
	{B
		mode = enmFlightMode::incircle;
	}
}

void CUav::HandleInCircle(double dt, double time)
{
	StopAtCoordinate(dt, time);
	if (getCommand)
	{
		mode = enmFlightMode::withdest;
	}
}
#endif // 0

//////////////////////////////////////////////////////////////////
//						WAYPOINT
//////////////////////////////////////////////////////////////////

WayPoint& WayPoint::operator+=(const WayPoint& p)
{
	
	x += p.x;
	y += p.y;
	z += p.z;
	return *this;

}

bool WayPoint::operator==(const WayPoint & p) const
{
	const double EPSILON = 1e-10;  // For floating-point comparison
	return std::abs(x - p.x) < EPSILON &&
		std::abs(y - p.y) < EPSILON &&
		std::abs(z - p.z) < EPSILON;
}




double WayPoint::calculateDistance(const WayPoint& p) const
{
	double dx = x - p.x;
	double dy = y - p.y;
	double dz = z - p.z;
	return sqrt(dx*dx + dy * dy + dz * dz);
}

double WayPoint::calculateHeading(const WayPoint & p) const
{
	// Calculate the difference in x and y coordinates
	double dx = p.x - x;
	double dy = p.y - y;

	// Calculate the heading angle in radians
	// atan2 returns angle in range [-π, π]
	double heading = atan2(dy, dx);

	// Convert to degrees and normalize to [0, 360]
	double degrees = heading * 180.0 / M_PI;
	if (degrees < 0)
		degrees += 360.0;

	return degrees;
}

std::ostream & operator<<(std::ostream & os, const WayPoint & p)
{
	// Use original precision
	std::streamsize prec = os.precision();

	// Set precision for floating-point output
	os << std::fixed << std::setprecision(2);

	// Output the WayPoint
	os << "(" << p.x << ", " << p.y << ", " << p.z << ")";

	// Restore original precision
	os.precision(prec);

	return os;
}
