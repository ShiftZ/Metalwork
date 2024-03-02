#pragma once

#include "Body.h"

class B2Body final : public Body
{
public:
	b2Body* b2body;

public:
	B2Body(b2Body* b2body, Name name);
	Vec2 GetPosition() override;
	void SetPosition(Vec2 position) override;
	Float GetAngle() override;
	Float GetAngleMod() override;
	Float GetMass() override;
	Float GetInertia() override;
	Vec2 GetVelocity() override;
	Float GetAngVelocity() override;
	void SetGravityScale(Float scale) override;
	void SetAngDamping(Float factor) override;
	void SetLinearDamping(Float factor) override;
	void ApplyForce(Vec2 force) override;
	void ApplyForce(Vec2 force, Vec2 point) override;
	void ApplyTorque(Float torque) override;
	
	void DrawShapes(IDebugDrawer& drawer) override;
	~B2Body();
};

inline b2Body* GetB2Body(Body* body) { return static_cast<B2Body*>(body)->b2body; }
