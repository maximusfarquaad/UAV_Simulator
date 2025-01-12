#pragma once
#include "CUav.h"

class IFlightMode
{
public:
	virtual ~IFlightMode() = default;
	virtual void Execute(CUav& uav, double dt, double time) = 0; //pure virtual
};

class WithoutDestinationMode : public IFlightMode
{
public:
	void Execute(CUav& uav, double dt, double time) override;
	
};

class WithDestinationMode : public IFlightMode
{
public:
	void Execute(CUav& uav, double dt, double time) override;
};

class InCircleMode : public IFlightMode
{
public:
	void Execute(CUav& uav, double dt, double time) override;
	
};