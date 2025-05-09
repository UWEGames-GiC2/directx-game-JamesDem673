#include "pch.h"
#include "GameObject2D.h"
#include "gamedata.h"
#include "DrawData2D.h"

using namespace DirectX;

GameObject2D::GameObject2D()
{
	m_colour = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_pos = Vector2::Zero;
	m_rotation = 0.0f;
	m_scale = Vector2::One;
	render = false;
}

bool GameObject2D::isRendered()
{
	return render;
}

Vector2 GameObject2D::GetPos()
{
	return m_pos;
}

void GameObject2D::SetRendered(bool state)
{
	render = state;
}

