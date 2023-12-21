#include "DebugDrawer.h"

const float Transparency = 0.2;

void DebugDrawer::Circle(vec2 Center, float Radius, Color Col)
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawCircle(World, Center * UEScale, {1, 0, 0}, {0, 0, 1}, Color, Radius * UEScale, 20, false, -1, 0, 2);
}

void DebugDrawer::SolidCircle(vec2 Center, vec2 Axis, float Radius, Color Col)
{
	const int32 NumSegments = 32;
	TArray<FVector> CircleVerts;
	TArray<int32> Indices;

	CircleVerts.Add(Center * UEScale);
	CircleVerts.Add((Center + vec2(Radius, 0)) * UEScale);

	for (int i = 1; i <= NumSegments; ++i)
	{
	    float Angle = i * TWO_PI / NumSegments;
	    vec2 Vertex = Center + Radius * vec2(cos(Angle), sin(Angle));
	    CircleVerts.Add(Vertex * UEScale);
        Indices.Add(0), Indices.Add(i), Indices.Add(i + 1);
	}

	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();

	DrawDebugMesh(World, CircleVerts, Indices, Color);
}

void DebugDrawer::Poly(span<vec2> Verts, Color Col)
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	for (auto [P1, P2] : Verts | views::adjacent<2>)
		DrawDebugLine(World, P1 * UEScale, P2 * UEScale, Color);
}

void DebugDrawer::SolidPoly(span<vec2> Verts, Color Col)
{
	TArray<FVector> TriVerts;
	for (vec2 V : Verts) TriVerts.Add(V * UEScale);

	TArray<int32> Indices;
	for (int i = 1; i < Verts.size() - 1; ++i)
		Indices.Add(0), Indices.Add(i), Indices.Add(i + 1);

	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();

	DrawDebugMesh(World, TriVerts, Indices, Color);
}

void DebugDrawer::Line(vec2 A, vec2 B, Color Col)
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawDebugLine(World, A * UEScale, B * UEScale, Color, false, -1, 0, 2);
}

void DebugDrawer::Point(vec2 P, float Size, Color Col)
{
	FColor Color = FLinearColor(Col.r, Col.g, Col.b, Col.a * Transparency).QuantizeRound();
	DrawDebugPoint(World, P * UEScale, Size * 0.1f, Color);
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
