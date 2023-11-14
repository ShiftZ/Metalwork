#pragma once

#include "RigidWorld.h"

class B2Body final : public RigidBody
{
	class b2Body* body;

	friend b2Body* GetB2Body(RigidBody*);
	friend class B2World;

public:
	B2Body(b2Body* body);
	vec2 GetPosition() override;
	void SetPosition(vec2 position) override;
	float GetAngle() override;
	void JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB = nullopt) override;
	void JoinDistant(RigidBody* with, vec2 anchor, float min, float max) override;
	void ApplyForce(vec2 force) override;
	void DrawShapes(IDebugDrawer& drawer) override;
	~B2Body();
};

inline b2Body* GetB2Body(RigidBody* body) { return static_cast<B2Body*>(body)->body; }
