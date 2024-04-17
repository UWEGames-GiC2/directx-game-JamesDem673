#include "pch.h"
#include "Player.h"
#include <dinput.h>
#include "GameData.h"
#include <iostream>

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.x = 7.5f;
	m_pos.y = 10.0f;
	m_pos.z = 22.5f;

	SetDrag(1.25);
	SetPhysicsOn(true);
}

Player::~Player()
{
	//tidy up anything I've created
}

  
void Player::Tick(GameData* _GD)
{

	//TURN AND FORWARD CONTROL HERE
	Vector3 forwardMove = 40.0f * Vector3::Forward;
	Vector3 sidewaysMove = 40.0f * Vector3::Left;

	Matrix rotMove = Matrix::CreateRotationY(m_yaw);
	forwardMove = Vector3::Transform(forwardMove, rotMove);
	sidewaysMove = Vector3::Transform(sidewaysMove, rotMove);

	if (_GD->m_KBS.W)
	{
		m_acc += forwardMove;
	}
	if (_GD->m_KBS.S)
	{
		m_acc -= forwardMove;
	}
	if (_GD->m_KBS.A)
	{
		m_acc += sidewaysMove;
	}
	if (_GD->m_KBS.D)
	{
		m_acc -= sidewaysMove;
	}

	//move player up and down
	if (_GD->m_KBS.R)
	{
		m_acc.y += 40.0f;
	}

	if (_GD->m_KBS.F)
	{
		m_acc.y -= 40.0f;
	}

	//Change orientation of player
	float rotSpeed = _GD->m_dt;
	m_yaw -= rotSpeed * _GD->m_MS.x;

	//apply my base behaviour
	CMOGO::Tick(_GD);

	_GD->pPosx = m_pos.x;
	_GD->pPosz = m_pos.z;
}
