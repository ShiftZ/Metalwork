#include "B2Draw.h"
#include "CoreInterface.h"

void B2Draw::DrawPolygon(const b2Vec2* vertices, int32 n_vertices, const b2Color& col)
{
	vector<vec2> verts = {vertices, vertices + n_vertices};
	drawer.Poly(verts, col);
}

void B2Draw::DrawSolidPolygon(const b2Vec2* vertices, int32 n_vertices, const b2Color& col)
{
	vector<vec2> verts = {vertices, vertices + n_vertices};
	drawer.SolidPoly(verts, col);
}

void B2Draw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	drawer.Circle(center, radius, color);
}

void B2Draw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	drawer.SolidCircle(center, axis, radius, color);
}

void B2Draw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	drawer.Line(p1, p2, color);
}

void B2Draw::DrawTransform(const b2Transform& xf)
{
}

void B2Draw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	drawer.Point(p, size, color);
}
