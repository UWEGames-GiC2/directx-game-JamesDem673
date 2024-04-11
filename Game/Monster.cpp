#include "pch.h"
#include "Monster.h"
#include <dinput.h>
#include "GameData.h"
#include <math.h>
#include <iostream>

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
	_GD->ePosx = GetPos().x;
	_GD->ePosz = GetPos().z;

	trackPlayer(_GD->pPosx, _GD->pPosz, _GD);
}

void Monster::trackPlayer(float playerx, float playerz, GameData* _GD)
{
	float adjactentLength;
	float oppositeLength;

	adjactentLength = playerx - m_pos.x;
	oppositeLength = playerz - m_pos.z;

	float angleLookAt = atan2(adjactentLength, oppositeLength);

	angleLookAt = angleLookAt * (180.0 / 3.1415926);

	m_yaw = angleLookAt;

	CMOGO::Tick(_GD);
}

void Monster::travel()
{

}


