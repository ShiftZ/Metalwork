#pragma once

#include "Mirror.h"

enum class Movability { Static, Kinematic, Dynamic };

class Body
{
	CLASS(Body)

public:
	enum Role { ENUM(Role, None, Prop) };

public:
	class RigidObject* object = nullptr;
	Name name;
	Vec2 offset;

	Role PROPERTY(role, SETTER(SetRole)) = None;
	float PROPERTY(snap_impulse) = 0;
	Name PROPERTY(model);

public:
	Body(Name name, Vec2 shift) : name(name), offset(shift) {}
	virtual ~Body() = default;

	METALWORKCORE_API Vec2 GetPosition();
	void SetPosition(Vec2 position);
	METALWORKCORE_API Float GetAngle();
	Float GetAngleMod();
	Float GetMass();
	Float GetInertia();
	METALWORKCORE_API Vec2 GetVelocity();
	Float GetAngVelocity();
	void SetGravityScale(Float scale);
	void SetAngDamping(Float factor);
	void SetLinearDamping(Float factor);
	void ApplyForce(Vec2 force);
	void ApplyForce(Vec2 force, Vec2 point);
	void ApplyTorque(Float torque);
	void ApplyImpulse(Vec2 impulse, Vec2 point);
	Movability GetMovability();
	void SetMovablity(Movability movability);

	void DrawShapes(class IDebugDrawer& drawer);

	void SetRole(Role role);
};