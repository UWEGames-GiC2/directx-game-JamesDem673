#include "pch.h"
#include "Monster.h"
#include <dinput.h>
#include "GameData.h"

Monster::Monster(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

Monster::~Monster()
{
	//tidy up anything I've created
}


void Monster::Tick(GameData* _GD)
{
	
}