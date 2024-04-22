#include "pch.h"
#include "ViewRadius.h"
#include "GameData.h"
#include <iostream>

ViewRadius::ViewRadius(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

ViewRadius::~ViewRadius()
{
	//tidy up anything I've created
}


void ViewRadius::Tick(GameData* _GD)
{
	m_pos.x = _GD->pPosx;
	m_pos.z = _GD->pPosz;
	CMOGO::Tick(_GD);
}

void ViewRadius::reduceScale()
{
	if (currentScale - 0.5f >= minScale)
	{
		currentScale -= 0.5f;
		SetScale(Vector3::One * currentScale);
	}
}

void ViewRadius::increaseScale()
{
	if (currentScale + 0.5f <= maxScale)
	{
		currentScale += 0.5f;
		SetScale(Vector3::One * currentScale);
	}
}