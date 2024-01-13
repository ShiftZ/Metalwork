#include "DebugDrawer.h"

const float Transparency = 0.2;

void DebugDrawer::Circle(Vec2 Center, float Radius, Color Col) const
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawCircle(World, Center * UEScale, {1, 0, 0}, {0, 0, 1}, Color, Radius * UEScale, 20, bPersistent, -1, 0, 2);
}

void DebugDrawer::SolidCircle(Vec2 Center, Vec2 Axis, float Radius, Color Col) const
{
	const int32 NumSegments = 32;
	TArray<FVector> CircleVerts;
	TArray<int32> Indices;

	CircleVerts.Add(Center * UEScale);
	CircleVerts.Add((Center + Vec2(Radius, 0)) * UEScale);

	for (int i = 1; i <= NumSegments; ++i)
	{
	    float Angle = i * TWO_PI / NumSegments;
	    Vec2 Vertex = Center + Radius * Vec2(cos(Angle), sin(Angle));
	    CircleVerts.Add(Vertex * UEScale);
        Indices.Add(0), Indices.Add(i), Indices.Add(i + 1);
	}

	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();

	DrawDebugMesh(World, CircleVerts, Indices, Color, bPersistent);
}

void DebugDrawer::Poly(span<Vec2> Verts, Color Col) const
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	for (auto [P1, P2] : Verts | views::adjacent<2>)
		DrawDebugLine(World, P1 * UEScale, P2 * UEScale, Color, bPersistent);
}

void DebugDrawer::SolidPoly(span<Vec2> Verts, Color Col) const
{
	TArray<FVector> TriVerts;
	for (Vec2 V : Verts) TriVerts.Add(V * UEScale);

	TArray<int32> Indices;
	for (int i = 1; i < Verts.size() - 1; ++i)
		Indices.Add(0), Indices.Add(i), Indices.Add(i + 1);

	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();

	DrawDebugMesh(World, TriVerts, Indices, Color, bPersistent);
}

void DebugDrawer::Line(Vec2 A, Vec2 B, Color Col) const
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawDebugLine(World, A * UEScale, B * UEScale, Color, bPersistent, -1, 0, 2);
}

void DebugDrawer::Point(Vec2 P, float Size, Color Col) const
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawDebugPoint(World, P * UEScale, Size * 0.1f, Color, bPersistent);
}