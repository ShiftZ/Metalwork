#pragma once

#include "CoreInterface.h"

class DebugDrawer : public IDebugDrawer
{
	UWorld* World;

public:
	DebugDrawer(UWorld* World) : World(World) {}
	void Circle(vec2 Center, float Radius) override;
	void Poly(span<vec2> Verts) override;
	void Edge(vec2 A, vec2 B) override;
};