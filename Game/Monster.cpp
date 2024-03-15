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
	trackPlayer(_GD->pPosx, _GD->pPosz);
}

void Monster::trackPlayer(float playerx, float playerz)
{
	float adjactentLength;
	float oppositeLength;

	adjactentLength = playerx - m_pos.x;
	oppositeLength = playerz - m_pos.z;

	float angleLookAt = atan(adjactentLength / oppositeLength);

	angleLookAt = angleLookAt * (180.0 / 3.1415926);

	std::cout << angleLookAt << std::endl;

	m_yaw = angleLookAt;
}
