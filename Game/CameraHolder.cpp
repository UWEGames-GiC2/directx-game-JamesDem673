#include "pch.h"
#include "CameraHolder.h"
#include "GameData.h"
#include <iostream>

CameraHolder::CameraHolder(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

CameraHolder::~CameraHolder()
{
	//tidy up anything I've created
}


void CameraHolder::Tick(GameData* _GD)
{
	m_pos.x = _GD->pPosx;
	m_pos.z = _GD->pPosz;
	m_pos.y = _GD->pPosy + 7.5;

	//change orinetation of player
	float rotSpeed = _GD->m_dt;
	m_yaw -= rotSpeed * _GD->m_MS.x;
	m_pitch -= rotSpeed * _GD->m_MS.y;

	if (m_pitch < 0.1)
	{
		m_pitch = 0.1;
	}
	else if (m_pitch > 3)
	{
		m_pitch = 3.0f;
	}
}
