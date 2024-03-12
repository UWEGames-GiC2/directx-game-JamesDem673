#ifndef _MONSTER_H_
#define _MONSTER_H_
#include "CMOGO.h"

//=================================================================
//Base Monster Class (Controls movement)
//=================================================================

class Monster : public CMOGO
{

public:
	Monster(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Monster();

	virtual void Tick(GameData* _GD) override;

protected:

};

#endif