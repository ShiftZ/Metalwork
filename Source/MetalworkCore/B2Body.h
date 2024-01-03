#pragma once

#include "RigidWorld.h"

class B2Body final : public RigidBody
{
	class b2Body* b2body;

	friend b2Body* GetB2Body(RigidBody*);
	friend class B2World;

public:
	B2Body(b2Body* b2body, Name name, Name model);
	vec2 GetPosition() override;
	void SetPosition(vec2 position) override;
	float GetAngle() override;
	float GetMass() override;
	float GetInertia() override;
	vec2 GetVelocity() override;
	float GetAngVelocity() override;
	void SetGravityScale(float scale) override;
	void SetAngDamping(float factor) override;
	void SetLinearDamping(float factor) override;
	void JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB = nullopt) override;
	void JoinDistant(RigidBody* with, vec2 anchor, float min, float max) override;
	void ApplyForce(vec2 force) override;
	void ApplyForce(vec2 force, vec2 point) override;
	void ApplyTorque(float torque) override;
	
	void DrawShapes(IDebugDrawer& drawer) override;
	~B2Body();
};

inline b2Body* GetB2Body(RigidBody* body) { return static_cast<B2Body*>(body)->b2body; }
