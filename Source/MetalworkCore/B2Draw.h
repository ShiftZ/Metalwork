#pragma once

class B2Draw : public b2Draw
{
	class IDebugDrawer* drawer;

public:
	B2Draw(IDebugDrawer* drawer) : drawer(drawer) { m_drawFlags = 0; }

	void DrawPolygon(const b2Vec2* vertices, int32 n_vertices, const b2Color& col) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 n_vertices, const b2Color& col) override;
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
};