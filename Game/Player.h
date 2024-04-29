#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "CMOGO.h"

//=================================================================
//Base Player Class (i.e. a GO the player controls)
//=================================================================

class Player : public CMOGO
{

public:
	Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Player();

	virtual void Tick(std::shared_ptr<GameData> _GD) override;

};

#endif