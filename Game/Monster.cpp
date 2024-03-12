#include "pch.h"
#include "Monster.h"
#include <dinput.h>
#include "GameData.h"

Monster::Monster(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 10.0f;

	SetDrag(1.25);
	SetPhysicsOn(true);
}

Monster::~Monster()
{
	//tidy up anything I've created
}


void Monster::Tick(GameData* _GD)
{
	
}