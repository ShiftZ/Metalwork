#pragma once

#include "Mirror.h"

enum class BodyType { Static, Kinematic, Dynamic };

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

	virtual Vec2 GetPosition() = 0;
	virtual void SetPosition(Vec2 position) = 0;
	virtual Float GetAngle() = 0;
	virtual Float GetAngleMod() = 0;
	virtual Float GetMass() = 0;
	virtual Float GetInertia() = 0;
	virtual Vec2 GetVelocity() = 0;
	virtual Float GetAngVelocity() = 0;
	virtual void SetGravityScale(Float scale) = 0;
	virtual void SetAngDamping(Float factor) = 0;
	virtual void SetLinearDamping(Float factor) = 0;
	virtual void ApplyForce(Vec2 force) = 0;
	virtual void ApplyForce(Vec2 force, Vec2 point) = 0;
	virtual void ApplyTorque(Float torque) = 0;
	virtual void DrawShapes(class IDebugDrawer& drawer) = 0;

	void SetRole(Role role);
};

void SetType(Body* body, BodyType type);
void SetGravityScale(Body* body, Float scale);