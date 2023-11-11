#include "DebugDrawer.h"

void DebugDrawer::Circle(vec2 Center, float Radius)
{
	DrawCircle(World, Center * UEScale, {1, 0, 0}, {0, 0, 1}, FColor::Cyan, Radius * UEScale, 20);
}

void DebugDrawer::Poly(span<vec2> Verts)
{
	for (auto [P1, P2] : Verts | views::adjacent<2>)
		DrawDebugLine(World, P1 * UEScale, P2 * UEScale, FColor::Cyan);
}

void DebugDrawer::Edge(vec2 A, vec2 B)
{
	DrawDebugLine(World, A * UEScale, B * UEScale, FColor::Cyan);
	auto [Dir, Len] = normalize_len(B - A);
	vec2 Norm = rotate90cw(Dir);
	for (float i = 0; i < Len; i += 0.1)
	{
		A = Dir * i;
		B = A + Norm * 0.05;
		DrawDebugLine(World, A * UEScale, B * UEScale, FColor::Cyan);
	}
}
