#pragma once

#include "CoreInterface.h"

class DebugDrawer : public IDebugDrawer
{
	UWorld* World;
	bool bPersistent;

public:
	DebugDrawer(UWorld* World, bool bPersistent) : World(World), bPersistent(bPersistent) {}

	void Circle(vec2 Center, float Radius, Color Col) const override;
	void SolidCircle(vec2 Center, vec2 Axis, float Radius, Color Col) const override;
	void Poly(span<vec2> Verts, Color Col) const override;
	void SolidPoly(span<vec2> Verts, Color Col) const override;
	void Line(vec2 A, vec2 B, Color Col) const override;
	void Point(vec2 P, float Size, Color Col) const override;
};