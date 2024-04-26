#include "pch.h"
#include "Gazebo.h"
#include "GameData.h"


Gazebo::Gazebo(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

Gazebo::~Gazebo()
{
	//tidy up anything I've created
}


void Gazebo::Tick(GameData* _GD)
{
	CMOGO::Tick(_GD);
}
