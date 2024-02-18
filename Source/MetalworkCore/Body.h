#pragma once

class Body
{
public:
	class RigidObject* object = nullptr;
	Name name, model;
	Vec2 offset;

public:
	Body(Name name, Name model, Vec2 shift) : name(name), model(model), offset(shift) {}

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

	virtual ~Body() = default;
};