#pragma once

#include "CoreInterface.h"

class DebugDrawer : public IDebugDrawer
{
	UWorld* World;
	bool bPersistent;

public:
	DebugDrawer(UWorld* World, bool bPersistent) : World(World), bPersistent(bPersistent) {}

	void Circle(Vec2 Center, float Radius, Color Col) const override;
	void SolidCircle(Vec2 Center, Vec2 Axis, float Radius, Color Col) const override;
	void Poly(span<Vec2> Verts, Color Col) const override;
	void SolidPoly(span<Vec2> Verts, Color Col) const override;
	void Line(Vec2 A, Vec2 B, Color Col) const override;
	void Point(Vec2 P, float Size, Color Col) const override;
};