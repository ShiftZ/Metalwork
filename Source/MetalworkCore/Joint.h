#pragma once

class Joint
{
public:
	Name name;

public:
	virtual class Body* GetBodyA() = 0;
	virtual class Body* GetBodyB() = 0;
	virtual Float GetForce() = 0;
	virtual ~Joint() = default;
};