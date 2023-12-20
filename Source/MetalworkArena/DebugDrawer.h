#pragma once

#include "CoreInterface.h"

class DebugDrawer : public IDebugDrawer
{
	UWorld* World;

public:
	DebugDrawer(UWorld* World) : World(World) {}

	void Circle(vec2 Center, float Radius, Color Col) override;
	void SolidCircle(vec2 Center, vec2 Axis, float Radius, Color Col) override;
	void Poly(span<vec2> Verts, Color Col) override;
	void SolidPoly(span<vec2> Verts, Color Col) override;
	void Line(vec2 A, vec2 B, Color Col) override;
	void Point(vec2 P, float Size, Color Col) override;
	void Edge(vec2 A, vec2 B) override;
};